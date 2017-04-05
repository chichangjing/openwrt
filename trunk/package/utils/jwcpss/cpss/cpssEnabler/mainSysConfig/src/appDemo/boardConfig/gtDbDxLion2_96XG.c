/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDbDxLion2_96XG.c
*
* DESCRIPTION:
*       Initialization functions for the Lion board.
*
* FILE REVISION NUMBER:
*       $Revision: 57 $
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/boardConfig/appDemoLion2TrunkWa.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/sysHwConfig/gtAppDemoLioncubConfig.h>
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

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
#include <cpss/dxCh/dxChxGen/cutThrough/cpssDxChCutThrough.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortAp.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagDataIntegrity.h>
#include <cpss/dxCh/dxChxGen/diag/private/prvCpssDxChDiag.h>
#include <cpssDriver/pp/interrupts/dxExMx/prvCpssDrvExMxUtilLion.h>

/* Pizza Arbiter and slices management for specific units */
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>

/* include the external services */
#include <cmdShell/common/cmdExtServices.h>

#include <private/mvHwsPortMiscIf.h>
#include <silicon/lion2/mvHwsLion2PortIf.h>

/*******************************************************************************
 * External definitions
 ******************************************************************************/
extern GT_STATUS qt2025PhyConfig(GT_U8 dev, GT_U32 *xsmiAddrArrayPtr);

/* multi port group fdb look up section */
/* REV_ID_ENT : enum for supported board revisions
    REV_ID_RESERVED_1_E - reserved
    REV_ID_CALLER_CHOICE_E - the caller choose the exact infrastructure,
                             - the caller can choose not to do any special infrastructure.
    REV_ID_RESERVED_2_E - reserved
    REV_ID_ENHANCED_AUTO_4_FDB_256K_E - auto build for enhanced 256K mode -- quad FDB
    REV_ID_ENHANCED_AUTO_2_FDB_128K_E - auto build for enhanced 128K mode -- double FDB
*/
enum REV_ID_ENT{
    REV_ID_RESERVED_1_E      = 20 + 1,                               /*21*/
    REV_ID_CALLER_CHOICE_E,                                          /*22*/
    REV_ID_RESERVED_2_E,                                             /*23*/
    REV_ID_ENHANCED_AUTO_4_FDB_256K_E,                               /*24*/
    REV_ID_ENHANCED_AUTO_2_FDB_128K_E,                               /*25*/
    REV_ID_LION2_360MHz_4x10G_3x40G_E,                               /*26*/
    REV_ID_LAST_E
};

/*******************************************************************************
 * Global variables
 ******************************************************************************/
static GT_U8    ppCounter = 0; /* Number of Packet processors. */
static GT_U32   numOfActivePortGroups = 0; /* number of port groups -- assume single device */
#define LION2_CORES_BMP_CNS  0x000000FF
static GT_U32   activePortGroupsBmp = LION2_CORES_BMP_CNS;/* bitmap of active port groups
    some options to set this BMP:
    1. pre-init system :
        use  string "activePortGropsBmp" with the bmp value
    2. according to the boardRevId
    3. read from the device it's devType and know the active port groups from it.
        but the cpss was not initialized yet  so only 'de-reference' to the PP
        memory mapped on the CPU can help us.
        (for simulation can use scib functions)

    currently I'll add the string option
*/
/* Default Source ID*/
#define FIRST_SRC_ID_CNS 1
static GT_U32     networkPortDefaultSrcId = FIRST_SRC_ID_CNS;
static GT_U32     uplinkPortDefaultSrcId  = FIRST_SRC_ID_CNS + 1;
static GT_U32     relayPortDefaultSrcId   = FIRST_SRC_ID_CNS + 2;

/* Trunk ID*/
static GT_TRUNK_ID cscdTrunkId = 10;

extern GT_BOOL  changeToDownHappend[CPSS_MAX_PORTS_NUM_CNS];

extern GT_U32   trainingTrace;/* option to disable the print in runtime*/
#define TRAINING_DBG_PRINT_MAC(x) if(trainingTrace) cpssOsPrintSync x

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

/* The number of uplink ports */
#define PRV_UPLINK_PORTS_NUMBER 6

/* The number of ports occupied by 40G uplink ports */
#define PRV_UPLINK_OCCUPIED_PORTS_NUMBER (3 * 3)

/* The number of relay ports */
#define PRV_RELAY_PORTS_NUMBER 24

/* The number of SGMII ports */
#define PRV_SGMII_PORTS_NUMBER 1

/* The number of relay ports in cascade trunk  */
#define PRV_CSCD_TRUNK_RELAY_PORTS_NUMBER 16

/* The number of uplink ports in cascade trunk */
#define PRV_CSCD_TRUNK_UPLINK_PORTS_NUMBER  2

/* Dummy port for virtual cascade trunk */
#define PRV_VIRTUAL_CSCD_TRUNK_DUMMY_PORT   69

/* Virtual cascade trunk number */
#define PRV_VIRTUAL_CSCD_TRUNK              120

/* Lion2 Hooper device ID */
#define PRV_HOOPER_DEV_ID_CNS               0x8000

/* Virtual trunk ID */
static GT_TRUNK_ID virtualDefaultTrunkId = PRV_VIRTUAL_CSCD_TRUNK;

GT_STATUS lion2AppDemoCpuPortPhyConfig
(
    IN  GT_U8   devNum
);

#ifndef ASIC_SIMULATION_NO_HW
/* I intentionaly not define it as static to enable init at any time from command
   line */
GT_STATUS lion2AppDemoApInit
(
    IN  GT_U8   devNum
);
#endif

extern GT_STATUS prvCpssDxChPortTraceDelay
(
    IN GT_U8       devNum,
    IN GT_U32      portGroupId,
    IN GT_U32      millisec
);

extern GT_STATUS waTriggerInterruptsMask
(
    IN  GT_U8                       devNum,
    IN  GT_PHYSICAL_PORT_NUM        portNum,
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_EVENT_MASK_SET_ENT     operation
);

#ifndef ASIC_SIMULATION
static GT_STATUS intMaskSet
(
    IN  GT_U8                   dev,
    IN  GT_PHYSICAL_PORT_NUM    port,
    IN  CPSS_EVENT_MASK_SET_ENT operation
);
#endif 

static GT_U32 stabilizeDelay = 1;

GT_U32 stabilizeDelaySet
(
    GT_U32 newDelay
)
{
    GT_U32 oldDelay;

    oldDelay = stabilizeDelay;
    stabilizeDelay = newDelay;

    return oldDelay;
}

#ifndef ASIC_SIMULATION_NO_HW
/* counters for debug purposes to ensure number of treated AP interrupts 
 * same as number of issued interrupts by Z80
 */
static GT_U32 intCounter[8] = {0};
static GT_U32 hcdFoundCounter[8] = {0};
#endif

#ifdef _VXWORKS

#include "vxworks.h"
#include "taskLib.h"
#include "kernelLib.h"
#include "sysLib.h"

/*******************************************************************************
* kernelSliceTimeSet
*
* DESCRIPTION:
*       This routine enables round-robin selection among tasks of same priority
*       and sets the system time-slice to ticks. Round-robin scheduling is
*       disabled by default. A time-slice of zero ticks disables round-robin
*       scheduling.
*
* INPUTS:
*       ticks - time-slice in ticks or 0 to disable round-robin
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*       Round-robin scheduling allows the processor to be shared fairly by all
*       tasks of the same priority. Without round-robin scheduling, when
*       multiple tasks of equal priority must share the processor, a single
*       non-blocking task can usurp the processor until preempted by a task of
*       higher priority, thus never giving the other equal-priority tasks a
*       chance to run.
*       Round-robin scheduling is disabled by default. It can be enabled or
*       disabled with the routine kernelTimeSlice( ), which takes a parameter
*       for the "time slice" (or interval) that each task will be allowed to
*       run before relinquishing the processor to another equal-priority task.
*
*******************************************************************************/
GT_VOID kernelSliceTimeSet
(
    GT_U32 ticks
)
{
    kernelTimeSlice(ticks);

    return;
}
#endif

/*******************************************************************************
* lion2AppDemoDataIntegrityInit
*
* DESCRIPTION:
*       Lion2 Data Integrity module initialization
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS lion2AppDemoDataIntegrityInit
(
    IN  GT_U8   devNum
)
{
    GT_STATUS                                               rc;
    CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_ENT              memType;
    CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_ENT           errorType;
    CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_ENT  protectionType;

    for(memType = 0; memType < CPSS_DXCH_DIAG_DATA_INTEGRITY_MEM_TYPE_LION2_LAST_E; memType++)
    {
        rc = cpssDxChDiagDataIntegrityProtectionTypeGet(devNum,memType,&protectionType);
        if(GT_OK != rc)
        {
            return rc;
        }

        if(protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_NONE_E)
        {
            continue;
        }
        else if(protectionType == CPSS_DIAG_DATA_INTEGRITY_MEM_ERROR_PROTECTION_TYPE_PARITY_E)
        {
            errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_PARITY_E;
        }
        else
        {
            errorType = CPSS_DIAG_DATA_INTEGRITY_ERROR_CAUSE_TYPE_SINGLE_AND_MULTIPLE_ECC_E;
        }

        rc = cpssDxChDiagDataIntegrityEventMaskSet(devNum, memType, errorType, CPSS_EVENT_UNMASK_E);
        if(GT_OK != rc)
        {
            return rc;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* appDemoLion2TrunkExtServicesBind
*
* DESCRIPTION:
*       Lion2 application (that implements this function) will bind the
*       mainCmd (and GaltisWrapper) with OS,
*       external driver functions  and other application functions
*
* INPUTS:
*       None
* OUTPUTS:
*       None
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
static GT_VOID   appDemoLion2TrunkExtServicesBind
(
    GT_VOID
)
{
    cmdAppTrunkCascadeTrunkPortsSet = appDemoLion2TrunkCascadeTrunkPortsSet;
    cmdAppTrunkMembersSet = appDemoLion2TrunkMembersSet;
    cmdAppTrunkMemberAdd = appDemoLion2TrunkMemberAdd;
    cmdAppTrunkMemberDisable = appDemoLion2TrunkMemberDisable;
    cmdAppTrunkMemberEnable = appDemoLion2TrunkMemberEnable;
    cmdAppTrunkMemberRemove = appDemoLion2TrunkMemberRemove;
    cmdAppTrunkUpdatedPortsGet = appDemoLion2TrunkUpdatedPortsGet;
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
    GT_U8       i;                      /* Loop index.                  */
    GT_STATUS   rc;
    GT_U32      usedDevices[APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS]={0};
    GT_U32      index=0,devIndex0,activeGroupsNum;
    GT_U32      presteraDevId;
    GT_BOOL     foundFirstDev;
    GT_U8       firstDev;
    GT_BOOL     systemDualDeviceEnable;
    GT_U32      managementBus;
    GT_PCI_DEV_VENDOR_ID devVendorId;

    if (boardRevId == 10) /* LION2_MII */
    {
        appDemoSysConfig.cpuEtherPortUsed = GT_TRUE;
    }
    
    if(appDemoDbEntryGet("bus", &managementBus) != GT_OK)
    {
        managementBus = 0;
    }

    if(0 == managementBus)
    {
        if (boardRevId == 8 || boardRevId == 1) /*use kernel driver */
        {
            gtPresteraPciDontMap = GT_TRUE;
        }
        rc = appDemoSysGetPciInfo();
        if(rc != GT_OK)
        {
            return GT_NOT_FOUND;
        }
        devVendorId = appDemoPpConfigList[0].pciInfo.pciDevVendorId;
    }
    else
    {
        rc = appDemoSysGetSmiInfo();
        if(rc != GT_OK)
        {
            return GT_NOT_FOUND;
        }
        osMemCpy(&devVendorId, &appDemoPpConfigList[0].smiInfo.smiDevVendorId, 
                 sizeof(devVendorId));
CPSS_TBD_BOOKMARK_LION2
/* SMI supports traffic to CPU only by RGMII interface */
#if 0
        appDemoSysConfig.cpuEtherPortUsed = GT_TRUE;
#endif
    }

    /* PCI and SMI vendor ID structures equale */    
    presteraDevId = (((GT_U32)devVendorId.devId) << 16) | MARVELL_VENDOR_ID;

    /* ability to set different active port group bitmap */
    if(appDemoDbEntryGet((GT_CHAR*)"activePortGropsBmp", &activePortGroupsBmp) != GT_OK)
    {
        switch(devVendorId.devId)
        {
            /* Lion2 Hooper Device ID (base; for all flavors) is: 0x8000 */
            case PRV_HOOPER_DEV_ID_CNS:
                activePortGroupsBmp = (BIT_0 | BIT_1 | BIT_2 | BIT_3);
    
                /* enable 2 cores for Hooper (0..1) */
                if(boardRevId == 12)
                {
                    activePortGroupsBmp = BIT_0 | BIT_1;
                }
    
                break;
            default:
                /* 8 port groups -> BMP = 0xFF*/
    
                /* enable 4cores for Lion2 (0..3) */
                if(boardRevId == 12)
                {
                    activePortGroupsBmp = BIT_0 | BIT_1 | BIT_2 | BIT_3;
                }
                break;
        }
    }
    foundFirstDev = GT_FALSE;
    firstDev = 0;
    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            if( GT_FALSE == foundFirstDev )
            {
                foundFirstDev = GT_TRUE;
                firstDev = i;
            }

            if (appDemoSysConfig.cpuEtherPortUsed)  /* LION2_MII */
            {
                appDemoPpConfigList[i].cpuPortMode = CPSS_NET_CPU_PORT_MODE_MII_E;
            }

            if(((1<<(i-firstDev)) & activePortGroupsBmp) == 0)
            {
                appDemoPpConfigList[i].valid = GT_FALSE;
                index++;
                continue;
            }

            if(index == APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS)
            {
                return GT_FULL;
            }

            usedDevices[index++] = i;
            numOfActivePortGroups++;
        }
    }

    /* No PCI devices found */
    if(numOfActivePortGroups == 0)
    {
        return GT_NOT_FOUND;
    }

    devIndex0 =  usedDevices[0];

    switch(presteraDevId)
    {
        case CPSS_LION2_HOOPER_PORT_GROUPS_0123_DEVICES_CASES_MAC:
            appDemoPpConfigList[devIndex0].numOfPortGroups = 4;

            /* enable 2 cores for Hooper (0..1) */
            if(boardRevId == 12)
            {
                appDemoPpConfigList[devIndex0].numOfPortGroups = 2;
            }
            break;
        default:
            /* 8 port groups */
            /* the lion2 device has 8 port groups */
            appDemoPpConfigList[devIndex0].numOfPortGroups = 8;
            /* enable 4cores for Lion2 (0..3) */
            if(boardRevId == 12)
            {
                appDemoPpConfigList[devIndex0].numOfPortGroups = 4;
            }
            break;
    }

    /* set the first device to hold info about all the port groups */
    for(i = 0, index = 0, activeGroupsNum=0; i < appDemoPpConfigList[devIndex0].numOfPortGroups; i++, index++)
    {
        /*invalid all the devices , the first device will be validated after the loop */
        appDemoPpConfigList[usedDevices[index]].valid = GT_FALSE;

        if(((1<<i) & activePortGroupsBmp) == 0)
        {
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupActive = GT_FALSE;
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupPciInfo.pciBaseAddr = CPSS_PARAM_NOT_USED_CNS;
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupPciInfo.internalPciBase = CPSS_PARAM_NOT_USED_CNS;
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupSmiInfo.smiIdSel = CPSS_PARAM_NOT_USED_CNS;
            continue;
        }

        appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupActive = GT_TRUE;
        
        if (CPSS_CHANNEL_PCI_E == appDemoPpConfigList[devIndex0].channel)
        {
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupPciInfo =
                appDemoPpConfigList[usedDevices[index]].pciInfo;
        }
        else
        {
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupSmiInfo =
                appDemoPpConfigList[usedDevices[index]].smiInfo;
        }

        activeGroupsNum++;
    }

    /* validate the first device */
    appDemoPpConfigList[devIndex0].valid = GT_TRUE;

    appDemoPpConfigList[devIndex0].pciInfo.pciBaseAddr = CPSS_PARAM_NOT_USED_CNS;
    appDemoPpConfigList[devIndex0].pciInfo.internalPciBase =  CPSS_PARAM_NOT_USED_CNS;
    appDemoPpConfigList[devIndex0].smiInfo.smiIdSel = CPSS_PARAM_NOT_USED_CNS;

    if(activeGroupsNum != numOfActivePortGroups)
    {
        cpssOsPrintf("activeGroupsNum %d, numOfActivePortGroups %d\n",
                     activeGroupsNum, numOfActivePortGroups);
        /* the bmp of active port groups filtered one of the existing port groups */
        return GT_BAD_STATE;
    }

    ppCounter = 1;

    *numOfPp    = ppCounter;
    *numOfFa    = 0;
    *numOfXbar  = 0;

    *isB2bSystem = GT_FALSE;/* no B2B with Lion devices */

    /* Lion2/Hooper - 4 port groups case - the system is not dual device */
    if((12 == boardRevId) || (PRV_HOOPER_DEV_ID_CNS == devVendorId.devId))
    {
        systemDualDeviceEnable = GT_FALSE;
    }
    else /* all Lion2 configurations are dual device enabled */
    {
        systemDualDeviceEnable = GT_TRUE;
    }

    /* at this stage we are after cpssPpInit(...) and before cpssDxChHwPpPhase1Init(...) */
    rc = cpssSystemDualDeviceIdModeEnableSet(systemDualDeviceEnable);
    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* lion2AppDemoPortsConfig
*
* DESCRIPTION:
*     Init required ports of Lion2 to specified ifMode and speed
*
* INPUTS:
*       dev - device number
*       ifMode - port interface mode
*       speed - port speed
*       powerUp -   GT_TRUE - port power up
*                   GT_FALSE - serdes power down
*       numOfPorts - quantity of ports to configure
*                   use 0 if all device ports wanted
*       ... - numbers of ports to configure
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
GT_STATUS lion2AppDemoPortsConfig
(
    IN  GT_U8                           dev,
    IN  CPSS_PORT_INTERFACE_MODE_ENT    ifMode,
    IN  CPSS_PORT_SPEED_ENT             speed,
    IN  GT_BOOL                         powerUp,
    IN  GT_U32                          numOfPorts,
    ...
)
{
    GT_STATUS   rc;                 /* return code */
    GT_U32      i;                  /* interator */
    GT_PHYSICAL_PORT_NUM portNum;   /* port number */
    va_list     ap;                 /* arguments list pointer */
    CPSS_PORTS_BMP_STC initPortsBmp,/* bitmap of ports to init */
                        *initPortsBmpPtr;/* pointer to bitmap */
    GT_U32  coreClkDb;  /* core clock from SW DB */
    GT_U32  coreClkHw;  /* core clock from HW */
    CPSS_PORTS_BMP_STC sdTrainingBmp; /* bitmap of ports to run serdes 
                                                    auto-tuning/taining on */
    GT_U32  serdesTraining; /* serdes training required or not */
    GT_U32  trxTraining;    /* what kind of auto-tuning run on port */
    GT_BOOL isLinkUp;       /* link status on port */
#ifndef ASIC_SIMULATION
    GT_BOOL signalState;    /* is there signal on serdes */
    GT_U32  timeoutCounter; /* counter for link up timeout before run RX optimization */
#endif 
    CPSS_PORT_INTERFACE_MODE_ENT    currentIfMode;  /* interface mode configured 
                                                                already on port */
    CPSS_PORT_SPEED_ENT             currentSpeed;   /* speed configured 
                                                                already on port */

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&sdTrainingBmp);

    initPortsBmpPtr = &initPortsBmp;
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortsBmpPtr);

    rc = cpssDxChHwCoreClockGet(dev, &coreClkDb, &coreClkHw);
    if(rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwCoreClockGet", rc);
        return rc;
    }

    if(0 == numOfPorts)
    {
        /* loop over all ports */
        for(portNum = 0; portNum < PRV_CPSS_PP_MAC(dev)->numOfPorts; portNum++)
        {
            /* skip not existed ports */
            if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(dev, portNum))
                continue;
            /* skip CPU port */
            if (portNum == CPSS_CPU_PORT_NUM_CNS)
                continue;
            /* for 40G and XGMII - skip not supported ports*/
            if ((CPSS_PORT_SPEED_40000_E == speed) || 
                (CPSS_PORT_INTERFACE_MODE_XGMII_E == ifMode))
            {
                if((portNum%16) != 0 && (portNum%16) != 4 &&
                    (portNum%16) != 8 && (portNum%16) != 9)
                    continue;

                if((coreClkDb < 480) && ((portNum%16) == 9))
                    continue;
            }

            /* for 20G and RXAUI - skip not supported ports*/
            if ((CPSS_PORT_SPEED_20000_E == speed) || 
                (CPSS_PORT_INTERFACE_MODE_RXAUI_E == ifMode))
            {
                if((portNum%2 != 0) && ((portNum%16) != 9)
                    && ((portNum%16) != 11))
                    continue;

                if((coreClkDb < 480) && (((portNum%16) == 9)
                    || ((portNum%16) == 11)))
                    continue;
            }

            rc = cpssDxChPortInterfaceModeGet(dev, portNum, &currentIfMode);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = cpssDxChPortSpeedGet(dev, portNum, &currentSpeed);
            if (rc != GT_OK)
            {
                return rc;
            }

            if((GT_TRUE == powerUp) && (currentIfMode == ifMode) && (currentSpeed == speed))
            {
                rc = cpssDxChPortLinkStatusGet(dev, portNum, &isLinkUp);
                if (rc != GT_OK)
                {
                    return rc;
                }
    
                if(isLinkUp)
                { /* nothing to do this is just reconfiguration */
                    continue;
                }
            }

            rc = cpssDxChPortEnableSet(dev, portNum, GT_FALSE);
            if(rc != GT_OK)
            {
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortEnableSet", rc);
                return rc;
            }
            
            CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,portNum);
            rc = waTriggerInterruptsMask(dev, portNum, ifMode, CPSS_EVENT_MASK_E);
            if(rc != GT_OK)
            {
                return rc;
            }            
        }
    }
    else
    {
        va_start(ap, numOfPorts);
        for(i = 1; i <= numOfPorts; i++)
        {
            portNum = va_arg(ap, GT_U32);
            if (portNum != CPSS_CPU_PORT_NUM_CNS)
            {
                if((portNum >= PRV_CPSS_PP_MAC(dev)->numOfPorts) ||
                   !PRV_CPSS_PHY_PORT_IS_EXIST_MAC(dev,portNum))
                {
                    va_end(ap);
                    return GT_BAD_PARAM;
                }
            }

            rc = cpssDxChPortInterfaceModeGet(dev, portNum, &currentIfMode);
            if (rc != GT_OK)
            {
                return rc;
            }

            rc = cpssDxChPortSpeedGet(dev, portNum, &currentSpeed);
            if (rc != GT_OK)
            {
                return rc;
            }

            if((GT_TRUE == powerUp) && (currentIfMode == ifMode) && (currentSpeed == speed))
            {
                rc = cpssDxChPortLinkStatusGet(dev, portNum, &isLinkUp);
                if (rc != GT_OK)
                {
                    return rc;
                }
    
                if(isLinkUp)
                { /* nothing to do this is just reconfiguration */
                    continue;
                }
            }

            rc = cpssDxChPortEnableSet(dev, portNum, GT_FALSE);
            if(rc != GT_OK)
            {
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortEnableSet", rc);
                return rc;
            }
            
            CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,portNum);
            rc = waTriggerInterruptsMask(dev, portNum, ifMode, CPSS_EVENT_MASK_E);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
        va_end(ap);
    }
   
    TRAINING_DBG_PRINT_MAC(("cpssDxChPortModeSpeedSet:"));       
    rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, powerUp, ifMode, speed);
    if(rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortModeSpeedSet", rc);
        return rc;
    }
    TRAINING_DBG_PRINT_MAC(("rc=%d\n", rc));
  
    if(0 == powerUp)
    {/* don't unmask interrupts if ports powered down to not get them when port 
        will be restored */
        return GT_OK;
    }

    if (   (CPSS_PORT_INTERFACE_MODE_KR_E == ifMode) 
         ||(CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode)
         ||(CPSS_PORT_INTERFACE_MODE_XHGS_E  == ifMode)
         ||(CPSS_PORT_INTERFACE_MODE_XHGS_SR_E  == ifMode)
       )
    {
        if(appDemoDbEntryGet("serdesTraining", &serdesTraining) != GT_OK)
        {
            serdesTraining = 1;
        }
    }
    else
    {
        serdesTraining = 0;
    }

    if(serdesTraining)
    {
        CPSS_PORTS_BMP_BITWISE_OR_MAC(&sdTrainingBmp, &sdTrainingBmp, initPortsBmpPtr);
    }

    if(appDemoDbEntryGet("trxTraining", &trxTraining) != GT_OK)
    {
        trxTraining = 0;
    }

    for(portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
    {
        if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(initPortsBmpPtr, portNum))
        {
            continue;
        }
           
        TRAINING_DBG_PRINT_MAC(("portNum=%d:ifMode=%d,speed=%d\n", portNum, ifMode, speed));
#ifndef ASIC_SIMULATION
        rc = cpssDxChPortSerdesSignalDetectGet(dev, portNum, &signalState);
        if(rc != GT_OK)
        {
            cpssOsPrintSync("cpssDxChPortSerdesSignalDetectGet:rc=%d", rc);
            return rc;
        }
        TRAINING_DBG_PRINT_MAC(("port=%d:signalState=%d\n", portNum, signalState));
        if(!signalState)
        {           
            changeToDownHappend[portNum] = GT_TRUE;
            rc = intMaskSet(dev, portNum, CPSS_EVENT_UNMASK_E);
            if(rc != GT_OK)
            {
                return rc;
            }
            CPSS_PORTS_BMP_PORT_CLEAR_MAC(&sdTrainingBmp, portNum);
            continue;
        }
        else
        {
            changeToDownHappend[portNum] = GT_FALSE;
        }

        if(serdesTraining)
        {
            if ( (CPSS_PORT_INTERFACE_MODE_KR_E    == ifMode) || 
                 (CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode) ||
                 (CPSS_PORT_INTERFACE_MODE_XHGS_E  == ifMode) ||
                 (CPSS_PORT_INTERFACE_MODE_XHGS_SR_E  == ifMode)
               )
            {
                if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&sdTrainingBmp, portNum))
                {
                    continue;
                }
                TRAINING_DBG_PRINT_MAC(("port=%d:start training\n", portNum));                                
                if(trxTraining)
                {/* if other side started TRX training no gearBox lock could be */
                    cpssOsPrintf("SERDES TRx training - port %d\n",portNum);

                    rc = cpssDxChPortSerdesAutoTune(dev, portNum,
                                                    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_CFG_E);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortSerdesAutoTune(TRX_CFG,portNum=%d):rc=%d\n", portNum, rc);
                        return rc;
                    }

                    rc = cpssDxChPortSerdesAutoTune(dev, portNum,
                                                    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_START_E);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortSerdesAutoTune(TRX_START,portNum=%d):rc=%d\n", portNum, rc);
                        return rc;
                    }
                }
                else
                {
                    GT_BOOL gearBoxLock;
    
                    for(i = 0; i < 100; i++)
                    {
                        rc = cpssDxChPortPcsGearBoxStatusGet(dev, portNum, 
                                                                    &gearBoxLock);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("cpssDxChPortPcsGearBoxStatusGet:rc=%d,portNum=%d\n", 
                                            rc, portNum);
                            return rc;
                        }                    
                        if(gearBoxLock)
                        {
                            break;
                        }
                        cpssOsTimerWkAfter(10); 
                    }
    
                    if (!gearBoxLock)
                    {
                        changeToDownHappend[portNum] = GT_TRUE;
                        rc = intMaskSet(dev, portNum, CPSS_EVENT_UNMASK_E);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                        cpssOsPrintf("no gearBoxLock - port %d\n",portNum);
                        goto noGearBoxLock;
                    }

                    cpssOsPrintf("SERDES RX training - port %d\n",portNum);
                    rc = cpssDxChPortSerdesAutoTune(dev, portNum,
                            CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_RX_TRAINING_E);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintf("lion2AppDemoPortsConfig:RX training failed:portNum=%d:rc=%d\n", portNum, rc);
                        changeToDownHappend[portNum] = GT_TRUE;
                        CPSS_PORTS_BMP_PORT_CLEAR_MAC(&sdTrainingBmp, portNum);
                        rc = intMaskSet(dev, portNum, CPSS_EVENT_UNMASK_E);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("intMaskSet(port=%d,CPSS_EVENT_UNMASK_E):rc=%d\n", portNum, rc);
                        }
                        continue;
                    }
                }
            }
        }
        else
        {
            rc = appDemoDxChLion2PortLinkWa(dev, portNum);
            if(rc != GT_OK)
            {
                cpssOsPrintSync("appDemoDxChLion2PortLinkWa(portNum=%d):rc=%d\n", portNum, rc);
            }
            
            rc = cpssDxChPortEnableSet(dev, portNum, GT_TRUE);
            if(rc != GT_OK)
            {
                cpssOsPrintSync("cpssDxChPortEnableSet(portNum=%d,true):rc=%d\n", portNum, rc);
            }            
            
            (GT_VOID)waTriggerInterruptsMask(dev, portNum, ifMode, CPSS_EVENT_UNMASK_E);
           
            continue;
        }
#endif
    }

    if (    (CPSS_PORT_INTERFACE_MODE_KR_E    == ifMode) 
         || (CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode) 
         || (CPSS_PORT_INTERFACE_MODE_XHGS_E  == ifMode)
         || (CPSS_PORT_INTERFACE_MODE_XHGS_SR_E  == ifMode)
       )
    {
        if(trxTraining)
        {
            cpssOsTimerWkAfter(500);
        }

        for(portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
        {
            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&sdTrainingBmp, portNum))
            {
                continue;
            }

            if(trxTraining)
            {
#ifndef ASIC_SIMULATION
                rc = cpssDxChPortSerdesAutoTune(dev, portNum,
                                                CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_STATUS_E);
                if(rc != GT_OK)
                {
                    cpssOsPrintf("TRX training failed on portNum=%d:rc=%d\n", 
                                                                  portNum, rc);
                    changeToDownHappend[portNum] = GT_TRUE;
                    CPSS_PORTS_BMP_PORT_CLEAR_MAC(&sdTrainingBmp, portNum);
                    rc = intMaskSet(dev, portNum, CPSS_EVENT_UNMASK_E);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("intMaskSet(port=%d,CPSS_EVENT_UNMASK_E):rc=%d\n", portNum, rc);
                    }                   
                    continue;
                }
                
                cpssOsPrintf("port %d Trx training PASS\n", portNum);
#endif 
            }
        }

        if(trxTraining)
        {
            cpssOsTimerWkAfter(stabilizeDelay);
        }

        for(i = 0; i < 10; i++)
        {
            for(portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
            {
                if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&sdTrainingBmp, portNum))
                {
                    continue;
                }
            
                if((i > 0) && (!changeToDownHappend[portNum]))
                {/* WA succeeded in one of previous iterations */
                    continue;
                }

                TRAINING_DBG_PRINT_MAC(("i=%d)appDemoDxChLion2PortLinkWa(portNum=%d):", 
                                        i, portNum));
                rc = appDemoDxChLion2PortLinkWa(dev, portNum); 
                if(rc != GT_OK)
                {
                    if(rc != GT_NOT_READY)
                    {
                        cpssOsPrintSync("ConfigPortsDefaultByProfile:appDemoDxChLion2PortLinkWa(portNum=%d):rc=%d\n",
                                        portNum, rc);
                    }
                    changeToDownHappend[portNum] = GT_TRUE;
                }
                else
                {
                    rc = cpssDxChPortLinkStatusGet(dev, portNum, &isLinkUp);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("lion2AppDemoPortsConfig:cpssDxChPortLinkStatusGet(portNum=%d):rc=%d\n",
                                        portNum, rc);
                        return rc;
                    }
                    if(isLinkUp)
                    {
                        changeToDownHappend[portNum] = GT_FALSE;
                    }
                }
                TRAINING_DBG_PRINT_MAC(("rc=%d,changeToDownHappend=%d\n", 
                                        rc, changeToDownHappend[portNum]));
            }
            cpssOsTimerWkAfter(10);
        }
                
        for(portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
        {
            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&sdTrainingBmp, portNum))
            {
                continue;
            }

            if(!changeToDownHappend[portNum])
            {
                rc = cpssDxChPortEnableSet(dev, portNum, GT_TRUE); 
                if(rc != GT_OK)
                {
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortEnableSet", rc);
                    return rc;
                }                   
            }
            else
            {
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&sdTrainingBmp, portNum);
#ifndef ASIC_SIMULATION
                rc = intMaskSet(dev, portNum, CPSS_EVENT_UNMASK_E);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("intMaskSet(port=%d,CPSS_EVENT_UNMASK_E):rc=%d\n", portNum, rc);
                }                   
#endif                                    
                TRAINING_DBG_PRINT_MAC(("ConfigPortsDefaultByProfile:portNum=%d down\n",
                                portNum));
            }
        }                       
    }
    
#ifndef ASIC_SIMULATION
noGearBoxLock:
#endif 
    for(portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
    {
        if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(initPortsBmpPtr, portNum))
        {
            continue;
        }
        
#ifndef ASIC_SIMULATION
        if ((CPSS_PORT_INTERFACE_MODE_KR_E == ifMode) || 
            (CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode) ||
            (CPSS_PORT_INTERFACE_MODE_XHGS_E == ifMode) ||
            (CPSS_PORT_INTERFACE_MODE_XHGS_SR_E  == ifMode))
        {
            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&sdTrainingBmp, portNum))
            {
                continue;
            }           
            
            for (timeoutCounter = 3; timeoutCounter > 0; timeoutCounter--)
            {
                rc = cpssDxChPortLinkStatusGet(dev, portNum, &isLinkUp);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("lion2AppDemoPortsConfig:cpssDxChPortLinkStatusGet(portNum=%d):rc=%d\n",
                                    portNum, rc);
                    return rc;
                }
                if(isLinkUp != GT_TRUE)
                {
                    cpssOsTimerWkAfter(20);
                }
                else
                {
                    break;
                }
            }
    
            if(isLinkUp)
            {
                rc = cpssDxChPortForceLinkPassEnableSet(dev, portNum, GT_TRUE);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("lion2AppDemoPortsConfig:cpssDxChPortForceLinkPassEnableSet(portNum=%d,GT_TRUE):rc=%d\n",
                                    portNum, rc);
                    return rc;
                }
    
                rc = cpssDxChPortSerdesAutoTuneOptAlgRun(dev, portNum,
                                                         CPSS_PORT_SERDES_TRAINING_OPTIMISATION_FFE_E |
                                                         CPSS_PORT_SERDES_TRAINING_OPTIMISATION_ALIGN90_E);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("lion2AppDemoPortsConfig:cpssDxChPortSerdesAutoTuneOptAlgRun(portNum=%d):rc=%d\n",
                                    portNum, rc);
                    return rc;
                }
    
                rc = cpssDxChPortForceLinkPassEnableSet(dev, portNum, GT_FALSE);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("lion2AppDemoPortsConfig:cpssDxChPortForceLinkPassEnableSet(portNum=%d,GT_FALSE):rc=%d\n",
                                    portNum, rc);
                    return rc;
                }
            }
        }
#endif 
        rc = cpssDxChPortEnableSet(dev, portNum, GT_TRUE);
        if(rc != GT_OK)
        {
            cpssOsPrintSync("cpssDxChPortEnableSet(portNum=%d,true):rc=%d\n", portNum, rc);
        }
        
        (GT_VOID)waTriggerInterruptsMask(dev, portNum, ifMode, CPSS_EVENT_UNMASK_E);
    }

    return GT_OK;
}


/*******************************************************************************
* lion2AppDemoPortListConfig
*
* DESCRIPTION:
*     Init required ports of Lion2 to specified ifMode and speed
*
* INPUTS:
*       dev - device number
*       ifMode - port interface mode
*       speed - port speed
*       powerUp -   GT_TRUE - port power up
*                   GT_FALSE - serdes power down
*       numOfPorts - quantity of ports to configure
*                   use 0 if all device ports wanted
*       ... - numbers of ports to configure
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
typedef struct appDemoPortConfigSpec_STCT
{
    GT_U32                          localPort;
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode;
    CPSS_PORT_SPEED_ENT             speed;
}appDemoPortConfigSpec_STC;

static GT_STATUS lion2AppDemoPortListConfig
(
    IN  GT_U8                           dev,
    IN  GT_U32                          portGroupId,
    IN  GT_U32                          numOfPorts,
    IN  appDemoPortConfigSpec_STC  *    portListPtr
)
{
    GT_STATUS rc;
    GT_PHYSICAL_PORT_NUM port;
    GT_U32  i;

    for (i = 0 ; i < numOfPorts; i++,portListPtr++)
    {

        port = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(dev, portGroupId, portListPtr->localPort);
        /* skip not existed ports */
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(dev, port))
        {
            continue;
        }
        rc = lion2AppDemoPortsConfig(dev, portListPtr->ifMode, portListPtr->speed, GT_TRUE, 1, port);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* intMaskSet
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
*                           CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E.
*
*******************************************************************************/
#ifndef ASIC_SIMULATION
static GT_STATUS intMaskSet
(
    IN  GT_U8                   dev,
    IN  GT_PHYSICAL_PORT_NUM    port,
    IN  CPSS_EVENT_MASK_SET_ENT operation
)
{
    GT_STATUS   rc;
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;
    GT_U32      laneNum;

    rc = cpssDxChPortInterfaceModeGet(dev, port, &ifMode);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssDxChPortInterfaceModeGet(portNum=%d,ifMode=%d):rc=%d\n",
                        port, ifMode, rc);
        return rc;
    }

    if(CPSS_PORT_INTERFACE_MODE_RXAUI_E == ifMode)
    {
        for(laneNum = 0; laneNum < 2; laneNum++)
        {
            rc = cpssEventDeviceMaskWithEvExtDataSet(dev,
                                                     CPSS_PP_PORT_LANE_SYNC_STATUS_CHANGED_E,
                                                     ((port<<8)|laneNum),
                                                     operation);
            if(rc != GT_OK)
            {
                cpssOsPrintSync("cpssEventDeviceMaskWithEvExtDataSet(CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E,extData=0x%x):rc=%d\n",
                                ((port<<8)|laneNum), rc);
                return rc;
            }
        }
        rc = cpssEventDeviceMaskWithEvExtDataSet(dev,
                                     CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,
                                     port, CPSS_EVENT_MASK_E);
        if(rc != GT_OK)
        {
            cpssOsPrintf("cpssEventDeviceMaskSet(MMPCS_SIGNAL,port=%d,op=%d):%d", 
                         port, operation, rc);
        }
    }
    else if((CPSS_PORT_INTERFACE_MODE_KR_E == ifMode)
            || (CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode)
            || (CPSS_PORT_INTERFACE_MODE_XHGS_E == ifMode)
            || (CPSS_PORT_INTERFACE_MODE_XHGS_SR_E  == ifMode))
    {
        rc = cpssEventDeviceMaskWithEvExtDataSet(dev,
                                     CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,
                                     port, operation);
        if(rc != GT_OK)
        {
            cpssOsPrintf("cpssEventDeviceMaskSet(MMPCS_SIGNAL,port=%d,op=%d):%d", 
                         port, operation, rc);
        }
    }

    return rc;
}
#endif 

/*******************************************************************************
* lionPortInterfaceInitBoardConfigPortsDefaultByProfile
*
* DESCRIPTION:
*     Configs all ports from 0 to max in profile to default value
*
* INPUTS:
*       dev - device number
*       ifMode - interface mode
*       speed  - port speed
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
static GT_STATUS lionPortInterfaceInitBoardConfigPortsDefaultByProfile
(
    IN  GT_U8                          dev,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode,
    IN  CPSS_PORT_SPEED_ENT            speed
)
{
    GT_U32    portGroupId;
    GT_STATUS rc;
    GT_U32    localPort;
    GT_PHYSICAL_PORT_NUM port;
    CPSS_DXCH_PIZZA_PROFILE_STC * pizzaProfilePtrPtr;

    GT_U32  coreClkDb;
    GT_U32  coreClkHw;

    CPSS_PORTS_BMP_STC initPortsBmp,/* bitmap of ports to init */
                        *initPortsBmpPtr;/* pointer to bitmap */
    GT_U32  serdesTraining;
    GT_U32  trxTraining;    /* what kind of auto-tuning run on port */
#ifndef ASIC_SIMULATION
    GT_BOOL signalState;
#endif 
    CPSS_PORTS_BMP_STC sdTrainingBmp; /* bitmap of ports to run
                                               TRX training on */
    GT_U32  i;
    GT_BOOL isLinkUp;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&sdTrainingBmp);
    initPortsBmpPtr = &initPortsBmp;
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortsBmpPtr);    
    
    rc = cpssDxChHwCoreClockGet(dev, &coreClkDb, &coreClkHw);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(appDemoDbEntryGet("serdesTraining", &serdesTraining) != GT_OK)
    {
        serdesTraining = 1;
    }

    cpssOsPrintf("\n+--------------------------------------");
    cpssOsPrintf("\n|  INIT SYSTEM                         ");
    cpssOsPrintf("\n|    DEV   %d                          ",dev);
    cpssOsPrintf("\n|       CLOCK %3d MHz                  ",coreClkDb);
    cpssOsPrintf("\n+--------------------------------------");    
    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
    {
        rc = cpssDxChPortPizzaArbiterIfUserTableGet(dev,1<<(portGroupId),
                                                       /*OUT*/&pizzaProfilePtrPtr,
                                                       /*OUT*/(CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC**)NULL);
        if(rc != GT_OK)
        {
            return rc;
        }

        cpssOsPrintf("\n     core %d Num of ports to init %2d :",portGroupId,pizzaProfilePtrPtr->portNum);
        for(localPort = 0; localPort < pizzaProfilePtrPtr->portNum; localPort++)
        {
            cpssOsPrintf("%2d ",localPort);
            port = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(dev, portGroupId, localPort);
            /* skip not existed ports */
            if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(dev, port))
            {
                continue;
            }
            
            rc = cpssDxChPortEnableSet(dev, port, GT_FALSE);
            if(rc != GT_OK)
            {
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortEnableSet", rc);
                return rc;
            }
            
            rc = waTriggerInterruptsMask(dev, port, ifMode, CPSS_EVENT_MASK_E);
            if(rc != GT_OK)
            {
                return rc;
            }

            CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,port);

            if(serdesTraining)            
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(&sdTrainingBmp, port);
            }
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    cpssOsPrintf("\n");
    
    TRAINING_DBG_PRINT_MAC(("cpssDxChPortModeSpeedSet:"));
    rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, GT_TRUE, ifMode, speed);
    if(rc != GT_OK)
    {
        cpssOsPrintf("cpssDxChPortModeSpeedSet:rc=%d,ifMode=%d,speed=%d\n", 
                     rc, ifMode, speed);
        return rc;
    }
    TRAINING_DBG_PRINT_MAC(("rc=%d\n", rc));
    
    if(appDemoDbEntryGet("trxTraining", &trxTraining) != GT_OK)
    {
        trxTraining = 0;
    }

    for(port = 0; port < CPSS_MAX_PORTS_NUM_CNS; port++)
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(dev, port);

        if(CPSS_CPU_PORT_NUM_CNS == port)
        {
            continue;
        }

        if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(initPortsBmpPtr, port))
        {
            continue;
        }
#ifndef ASIC_SIMULATION

        rc = cpssDxChPortSerdesSignalDetectGet(dev, port, &signalState);
        if(rc != GT_OK)
        {
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortSerdesSignalDetectGet", rc);
            return rc;
        }

        TRAINING_DBG_PRINT_MAC(("port=%d:signalState=%d\n", port, signalState));
        if(!signalState)
        {
            changeToDownHappend[port] = GT_TRUE;            
            rc = intMaskSet(dev, port, CPSS_EVENT_UNMASK_E);
            if(rc != GT_OK)
            {
                return rc;
            }
            CPSS_PORTS_BMP_PORT_CLEAR_MAC(&sdTrainingBmp, port);
            continue;
        }

        if(serdesTraining)
        {
            if ((CPSS_PORT_INTERFACE_MODE_KR_E == ifMode) || 
                (CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode))
            {
                TRAINING_DBG_PRINT_MAC(("port=%d:start training\n", port));                
                if(trxTraining)
                {
                    cpssOsPrintf("SERDES TRx training - port %d\n",port);
                    rc = cpssDxChPortSerdesAutoTune(dev, port,
                                                    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_CFG_E);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortSerdesAutoTune(TRX_CFG,portNum=%d):rc=%d\n", port, rc);
                        return rc;
                    }

                    rc = cpssDxChPortSerdesAutoTune(dev, port,
                                                    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_START_E);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortSerdesAutoTune(TRX_START,portNum=%d):rc=%d\n", port, rc);
                        return rc;
                    }
                }
                else
                {
                    rc = cpssDxChPortSerdesAutoTune(dev, port,
                            CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_RX_TRAINING_E);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintf("lion2AppDemoPortsConfig:RX training failed:portNum=%d:rc=%d\n", port, rc);
                        changeToDownHappend[port] = GT_TRUE;
                        CPSS_PORTS_BMP_PORT_CLEAR_MAC(&sdTrainingBmp, port);
                        rc = intMaskSet(dev, port, CPSS_EVENT_UNMASK_E);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("intMaskSet(port=%d,CPSS_EVENT_UNMASK_E):rc=%d\n", port, rc);
                        }
                        continue;
                    }

                    cpssOsPrintf("port %d RX training PASS\n", port);
                }
            }
        }
#endif 
    }

    if(trxTraining)
    {
        cpssOsTimerWkAfter(500);
    }

    if(serdesTraining)
    {
        for(port = 0; port < CPSS_MAX_PORTS_NUM_CNS; port++)
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(dev, port);
    
            if(CPSS_CPU_PORT_NUM_CNS == port)
            {
                continue;
            }
    
            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&sdTrainingBmp, port))
            {
                continue;
            }
    
            if ((CPSS_PORT_INTERFACE_MODE_KR_E == ifMode) || 
                (CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode))
            {
                if(trxTraining)
                {
#ifndef ASIC_SIMULATION
                    TRAINING_DBG_PRINT_MAC(("port=%d:finish TRX training\n", port));
                    rc = cpssDxChPortSerdesAutoTune(dev, port,
                                                    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_STATUS_E);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortSerdesAutoTune(TRX_STATUS,portNum=%d):rc=%d\n", port, rc);
                        changeToDownHappend[port] = GT_TRUE; 
                        CPSS_PORTS_BMP_PORT_CLEAR_MAC(&sdTrainingBmp, port);
                        rc = intMaskSet(dev, port, CPSS_EVENT_UNMASK_E);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("intMaskSet(port=%d,CPSS_EVENT_UNMASK_E):rc=%d\n", port, rc);
                        }                   
                        continue;
                    }                   
                    else
                    {
                        cpssOsPrintf("port %d Trx training PASS\n", port);
                    }
#endif
                }
            }
        }
    }

    for(i = 0; i < 10; i++)
    {   
        for(port = 0; port < CPSS_MAX_PORTS_NUM_CNS; port++)
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(dev, port);
    
            if(CPSS_CPU_PORT_NUM_CNS == port)
            {
                continue;
            }

            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(initPortsBmpPtr, port))
            {
                continue;
            }
            
            if ((CPSS_PORT_INTERFACE_MODE_KR_E == ifMode) || 
                (CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode))
            {
                if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&sdTrainingBmp, port))
                {
                    continue;
                }
            }
            
            if((i > 0) && (!changeToDownHappend[port])) 
            {/* WA succeeded in one of previous iterations */
                continue;
            }
            
            TRAINING_DBG_PRINT_MAC(("i=%d)appDemoDxChLion2PortLinkWa(port=%d):", 
                                    i, port));
            rc = appDemoDxChLion2PortLinkWa(dev, port); 
            if(rc != GT_OK)
            {
                if(rc != GT_NOT_READY)
                {
                    cpssOsPrintSync("ConfigPortsDefaultByProfile:appDemoDxChLion2PortLinkWa(port=%d):rc=%d\n",
                                    port, rc);
                }
                changeToDownHappend[port] = GT_TRUE;
            }
            else
            {
                rc = cpssDxChPortLinkStatusGet(dev, port, &isLinkUp);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("lion2AppDemoPortsConfig:cpssDxChPortLinkStatusGet(port=%d):rc=%d\n",
                                    port, rc);
                    return rc;
                }
                if(isLinkUp)
                {
                    changeToDownHappend[port] = GT_FALSE;
                }
            }
            TRAINING_DBG_PRINT_MAC(("rc=%d,changeToDownHappend=%d\n", 
                                    rc, changeToDownHappend[port]));
        }
        cpssOsTimerWkAfter(10);
    }
    
    for(port = 0; port < CPSS_MAX_PORTS_NUM_CNS; port++)
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(dev, port);

        if(CPSS_CPU_PORT_NUM_CNS == port)
        {
            continue;
        }
    
        if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(initPortsBmpPtr, port))
        {
            continue;
        }
        
        if(!changeToDownHappend[port])
        {
            rc = cpssDxChPortEnableSet(dev, port, GT_TRUE); 
            if(rc != GT_OK)
            {
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortEnableSet", rc);
                return rc;
            }                   
        }
        else
        {
            TRAINING_DBG_PRINT_MAC(("ConfigPortsDefaultByProfile:portNum=%d down\n",
                            port));
        }
    }
    
    for(port = 0; port < CPSS_MAX_PORTS_NUM_CNS; port++)
    {
        PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(dev, port);

        if(CPSS_CPU_PORT_NUM_CNS == port)
        {
            continue;
        }

        if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(initPortsBmpPtr, port))
        {
            continue;
        }
        
        rc = waTriggerInterruptsMask(dev, port, ifMode, CPSS_EVENT_UNMASK_E);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    cpssOsPrintf("\n\n\n");
    return GT_OK;
}

/*******************************************************************************
* lionPortInterfaceInitBoard360MHz_4x10G_3x40G
*
* DESCRIPTION:
*     Init ports for board with clock 360 MHz 7 ports 4 x 10G  and  3 x 40G
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
static GT_STATUS lionPortInterfaceInitBoard360MHz_4x10G_3x40G
(
    GT_U8 dev
)
{
    GT_U32    portGroupId;
    GT_STATUS rc;
    static appDemoPortConfigSpec_STC localPortList[] =
    {
         { 0, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_10000_E }
        ,{ 1, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_10000_E }
        ,{ 2, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_10000_E }
        ,{ 3, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_10000_E }
        ,{ 4, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_40000_E }
        ,{ 8, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_40000_E }
        ,{ 9, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_40000_E }
    };

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev,portGroupId)
    {
        rc = lion2AppDemoPortListConfig(dev, portGroupId, sizeof(localPortList)/sizeof(localPortList[0]),&localPortList[0]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    return GT_OK;
}

/*******************************************************************************
* lionPortInterfaceInitJustSuperGop0
*
* DESCRIPTION:
*     by default configure just ports of Super GOP 0
*
* INPUTS:
*       dev - device number
*       ifMode - interface mode
*       speed  - port speed
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
static GT_STATUS lionPortInterfaceInitJustSuperGop0
(
    IN  GT_U8                          dev,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode,
    IN  CPSS_PORT_SPEED_ENT            speed
)
{
    GT_STATUS rc;
    GT_U32    port;

    /* by default configure just ports of Super GOP 0 */
    for(port = 0; port < 12; port++)
    {
        /* skip not existed ports */
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(dev, port))
            continue;

        rc = lion2AppDemoPortsConfig(dev, ifMode, speed, GT_TRUE, 1, port);
        if(rc != GT_OK)
            return rc;
    }
    return GT_OK;
}

/*******************************************************************************
* appDemoHooperRdInit
*
* DESCRIPTION:
*     run specific Hooper RD ports configuration
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
static GT_STATUS appDemoHooperRdInit
(
    IN  GT_U8 dev
)
{
    GT_STATUS           rc;                 /* return code */
    GT_U32              portNum;            /* iterators */
    CPSS_PORTS_BMP_STC  initPortsBmp,       /* bitmap of ports to init */
                        *initPortsBmpPtr;   /* pointer to bitmap */
    GT_U32              coreClkDb, coreClkHw;/* core clock */

    rc = cpssDxChHwCoreClockGet(dev, &coreClkDb, &coreClkHw);
    if(rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwCoreClockGet", rc);
        return rc;
    }

    initPortsBmpPtr = &initPortsBmp;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortsBmpPtr);

    /* loop over all ports */
    for(portNum = 0; portNum < 8; portNum++)
    {
        /* skip not existed ports */
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(dev, portNum))
            continue;
        /* skip CPU port */
        if (portNum == CPSS_CPU_PORT_NUM_CNS)
            continue;

        CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,portNum);
    }

    rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, GT_TRUE, 
                                  CPSS_PORT_INTERFACE_MODE_KR_E, 
                                  CPSS_PORT_SPEED_10000_E);
    if(rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortModeSpeedSet:10G KR", rc);
        return rc;
    }

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortsBmpPtr);
    /* loop over all ports */
    for(portNum = 8; portNum < PRV_CPSS_PP_MAC(dev)->numOfPorts; portNum++)
    {
        /* skip not existed ports */
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(dev, portNum))
            continue;
        /* skip CPU port */
        if (portNum == CPSS_CPU_PORT_NUM_CNS)
            continue;

        if((portNum%16) != 0 && (portNum%16) != 4 &&
            (portNum%16) != 8 && (portNum%16) != 9)
            continue;

        if((coreClkDb < 480) && ((portNum%16) == 9))
            continue;

        CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,portNum);
    }

    rc = cpssDxChPortModeSpeedSet(dev, initPortsBmp, GT_TRUE, 
                                  CPSS_PORT_INTERFACE_MODE_KR_E, 
                                  CPSS_PORT_SPEED_40000_E);
    if(rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortModeSpeedSet:40G KR", rc);
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* lion2AppDemoPortInterfaceInit
*
* DESCRIPTION:
*     Init ports. Should be done after board initialization !
*
* INPUTS:
*       dev - device number
*       boardRevId - revision ID
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
GT_STATUS lion2AppDemoPortInterfaceInit
(
    IN  GT_U8 dev,
    IN  GT_U8 boardRevId
)
{
    GT_STATUS   rc;             /* return code */
    GT_U32  initSerdesDefaults, /* should appDemo configure ports modes */
            lion2AllPortsInit,  /* should appdemo init all existing ports of current Lion2 board */
            lion2PortsConfigType; /* which ports modes configuration wanted:
                                    0 - ports 0-11 - SGMII;
                                    1 - ports 0-11 - 10G_KR;
                                    2 - ports 0,2,4,6,8,9,10,11 - RXAUI;
                                    3 - ports 0,2,4,6,8,9,10,11 - 10G_KX2 (DHX);
                                    4 - ports 0,2,4,6,8,9,10,11 - 20G_KR2;
                                    5 - ports 0,4,8,9 - XGMII (XAUI);
                                    6 - ports 0,4,8,9 - 40G_KR4;
                                    7 - ports 0 - 100G_KR10;
                                    */
    /* GT_PHYSICAL_PORT_NUM port;*/
    CPSS_PORT_INTERFACE_MODE_ENT   ifMode;
    CPSS_PORT_SPEED_ENT  speed;

    cpssOsMemSet(changeToDownHappend, 0, sizeof(GT_BOOL)*CPSS_MAX_PORTS_NUM_CNS);

    /* check if user wants to init ports to default values */
    rc = appDemoDbEntryGet("initSerdesDefaults", &initSerdesDefaults);
    if(rc != GT_OK)
    {/* appDemo will initialize ports to some default values */
        initSerdesDefaults = 1;
    }
    else
    {
        if(0 == initSerdesDefaults)
        {
            (GT_VOID)cpssEventDeviceMaskSet(dev,
                                        CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,
                                        CPSS_EVENT_MASK_E);
            return GT_OK;
        }
    }

    if (boardRevId == REV_ID_LION2_360MHz_4x10G_3x40G_E)
    {
        return lionPortInterfaceInitBoard360MHz_4x10G_3x40G(dev);
    }
    else if(7 == boardRevId || 8 == boardRevId)
    {
        return appDemoHooperRdInit(dev);
    }

    rc = appDemoDbEntryGet("lion2PortsConfigType", &lion2PortsConfigType);
    if(rc != GT_OK)
    {
        switch(boardRevId)
        {
            case 2:
                lion2PortsConfigType = 9; /* 40G_SR_LR; */
                break;
            case 3:
                lion2PortsConfigType = 1; /* 10G KR */
                break;
            case 4:
                lion2PortsConfigType = 6; /* 40G KR */
                break;
            default:
                lion2PortsConfigType = 8; /* default configuration 10G_SR_LR */
                break;
        }
    }

    rc = appDemoDbEntryGet("lion2AllPortsInit", &lion2AllPortsInit);
    if(rc != GT_OK)
    {
        lion2AllPortsInit = 1; /* by default configure all ports of device,
                                 to reduce init time define it 0, then
                                 just ports of Super GOP 0 will be configured */
    }

    switch(lion2PortsConfigType)
    {
        case 0:
            ifMode = CPSS_PORT_INTERFACE_MODE_SGMII_E;
            speed = CPSS_PORT_SPEED_1000_E;
            break;
        case 1:
            ifMode = CPSS_PORT_INTERFACE_MODE_KR_E;
            speed = CPSS_PORT_SPEED_10000_E;
            break;
        case 2:
            ifMode = CPSS_PORT_INTERFACE_MODE_RXAUI_E;
            speed = CPSS_PORT_SPEED_10000_E;
            break;
        case 3:
            ifMode = CPSS_PORT_INTERFACE_MODE_HX_E;
            speed = CPSS_PORT_SPEED_10000_E;
            break;
        case 4:
            ifMode = CPSS_PORT_INTERFACE_MODE_KR_E;
            speed = CPSS_PORT_SPEED_20000_E;
            break;
        case 5:
            ifMode = CPSS_PORT_INTERFACE_MODE_XGMII_E;
            speed = CPSS_PORT_SPEED_10000_E;
            break;
        case 6:
            ifMode = CPSS_PORT_INTERFACE_MODE_KR_E;
            speed = CPSS_PORT_SPEED_40000_E;
            break;
        case 7:
            ifMode = CPSS_PORT_INTERFACE_MODE_KR_E;
            speed = CPSS_PORT_SPEED_100G_E;
            break;
        case 8:
            ifMode = CPSS_PORT_INTERFACE_MODE_SR_LR_E;
            speed = CPSS_PORT_SPEED_10000_E;
            break;
        case 9:
            ifMode = CPSS_PORT_INTERFACE_MODE_SR_LR_E;
            speed = CPSS_PORT_SPEED_40000_E;
            break;
        case 10:
            ifMode = CPSS_PORT_INTERFACE_MODE_SR_LR_E;
            speed = CPSS_PORT_SPEED_20000_E;
            break;
        case 11:
            ifMode = CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;
            speed = CPSS_PORT_SPEED_1000_E;
            break;
        default:
            return GT_BAD_PARAM;
    }

    if(1 == lion2AllPortsInit)
    {
        if ((CPSS_PORT_SPEED_10000_E == speed) && 
            ((CPSS_PORT_INTERFACE_MODE_KR_E == ifMode) 
             || (CPSS_PORT_INTERFACE_MODE_SR_LR_E == ifMode)))
        {
            rc = lionPortInterfaceInitBoardConfigPortsDefaultByProfile(dev, 
                                                                        ifMode,
                                                                        speed);
        }
        else
        {
            rc = lion2AppDemoPortsConfig(dev, ifMode, speed, GT_TRUE, 0);
        }
        if(rc != GT_OK)
        {
            return rc;
        }
    }
    else
    {
        return lionPortInterfaceInitJustSuperGop0(dev, ifMode, speed);
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

    localPpPh1Config.devNum = devIdx;

    if (CPSS_CHANNEL_PCI_E == appDemoPpConfigList[devIdx].channel)
    {
        localPpPh1Config.baseAddr =
                                appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr;
        localPpPh1Config.internalPciBase =
                            appDemoPpConfigList[devIdx].pciInfo.internalPciBase;
        localPpPh1Config.deviceId =
            ((appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.devId << 16) |
             (appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.vendorId));
    
        localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_PEX_E;
        if (boardRevId == 8 || boardRevId == 1)
        {
            /* force to use kernel driver */
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
    for(ii = 0 ; ii < appDemoPpConfigList[devIdx].numOfPortGroups ;ii++)
    {
        if (CPSS_CHANNEL_PCI_E == appDemoPpConfigList[devIdx].channel)
        {
            localPpPh1Config.portGroupsInfo[ii].busBaseAddr =
                appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupPciInfo.pciBaseAddr;
            localPpPh1Config.portGroupsInfo[ii].internalPciBase =
                appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupPciInfo.internalPciBase;
            /* required for boardRevId == 8 || boardRevId == 1, use kernel driver */
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

        if (CPSS_CHANNEL_PCI_E == appDemoPpConfigList[devIdx].channel)
        {
            rc = extDrvPcieGetInterruptNumber(
                    localPpPh1Config.portGroupsInfo[ii].hwAddr.busNo,
                    localPpPh1Config.portGroupsInfo[ii].hwAddr.devSel,
                    localPpPh1Config.portGroupsInfo[ii].hwAddr.funcNo,
                    &(localPpPh1Config.portGroupsInfo[ii].intVecNum));
            if (rc == GT_OK)
            {
                localPpPh1Config.portGroupsInfo[ii].intMask = (GT_UINTPTR)localPpPh1Config.portGroupsInfo[ii].intVecNum;
            }
        }
        else
        {
            extDrvGetPciIntVec(GT_PCI_INT_D - (ii%4) , &intVecNum);
            localPpPh1Config.portGroupsInfo[ii].intVecNum = (GT_U32)((GT_UINTPTR)intVecNum);
            extDrvGetIntMask(GT_PCI_INT_D - (ii%4) , &localPpPh1Config.portGroupsInfo[ii].intMask);
        }
    }

    /* retrieve PP Core Clock from HW */
    localPpPh1Config.coreClk = APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS;
    #ifdef ASIC_SIMULATION
        if (boardRevId == 6)  /* 240MHz clock */
        {
            localPpPh1Config.coreClk = 240;
        }
    #endif


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
* waInit
*
* DESCRIPTION:
*       init the WA needed after phase1.
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
static GT_STATUS waInit
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

    boardRevId = boardRevId;


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

    for(ii = SYSTEM_DEV_NUM_MAC(0); ii < SYSTEM_DEV_NUM_MAC(ppCounter); ++ii)
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

        if(1 == enableSerdesAccessWa)
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
    GT_STATUS rc;
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
    rc = waInit(boardRevId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("waInit", rc);
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

    boardRevId = boardRevId;

    localPpPh2Config.devNum     = oldDevNum;
    localPpPh2Config.deviceId   = appDemoPpConfigList[oldDevNum].deviceId;

    localPpPh2Config.fuqUseSeparate = GT_TRUE;
    retVal = appDemoAllocateDmaMem(localPpPh2Config.deviceId,RX_DESC_NUM_DEF,
                                   RX_BUFF_SIZE_DEF,RX_BUFF_ALLIGN_DEF,
                                   TX_DESC_NUM_DEF,
                                   AU_DESC_NUM_DEF*numOfActivePortGroups,
                                   &localPpPh2Config);

    osMemCpy(phase2Params,&localPpPh2Config,sizeof(CPSS_PP_PHASE2_INIT_PARAMS));

    phase2Params->auMessageLength= CPSS_AU_MESSAGE_LENGTH_8_WORDS_E;

    if(CPSS_NET_CPU_PORT_MODE_MII_E == appDemoPpConfigList[oldDevNum].cpuPortMode)
    {/* in existing DB boards RGMII interface welded to core #1,
        also could be updated by appDemoUpdatePpPhase2Params using appDemoDbEntry
        mechanism */
        phase2Params->netifSdmaPortGroupId = 1;
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

    devNum = devNum;
    port_num = port_num;

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
* configBoardAfterPhase2SetUserProfile
*
* DESCRIPTION:
*       This function set user profile if required by board copnfiguration
*       after phase2.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devNum          - devNum
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
extern const CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC portGroupPortSpeed2SliceNum_360MHz_30Slices;
extern const CPSS_DXCH_PIZZA_PROFILE_STC                          pizzaProfileLion2B0Clock360MHz12Ports30Sllices;


static GT_STATUS configBoardAfterPhase2SetUserProfile
(
    IN GT_U8 boardRevId,
    IN GT_U8     devNum
)
{
    GT_STATUS rc;
    GT_U32 portGroupBmp;        /* number of port group */

    if (boardRevId == REV_ID_LION2_360MHz_4x10G_3x40G_E)
    {
        /* all port groups use same profile */
        portGroupBmp = PRV_CPSS_PP_MAC(devNum)->portGroupsInfo.activePortGroupsBmp;
        rc = cpssDxChPortPizzaArbiterIfUserTableSet(devNum,portGroupBmp,
                                                    (CPSS_DXCH_PIZZA_PROFILE_STC *)&pizzaProfileLion2B0Clock360MHz12Ports30Sllices,
                                                    (CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC *)&portGroupPortSpeed2SliceNum_360MHz_30Slices);
        if (rc != GT_OK)
        {
            return rc;
        }
    }
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
    GT_U8                   dev;  /* device number */
    GT_PHYSICAL_PORT_NUM    port; /* port number */
    GT_U8                   index;
    GT_STATUS               rc;     /* return code */
    GT_U32                  mcFifo; /* multicast FIFO allocated for port */

    boardRevId = boardRevId;


    for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
    {
        rc = configBoardAfterPhase2SetUserProfile(boardRevId,dev);
        if (GT_OK != rc)
        {
            return rc;
        }

        index = 0;
        appDemoPpConfigList[dev].internal10GPortConfigFuncPtr = internalXGPortConfig;
        appDemoPpConfigList[dev].internalCscdPortConfigFuncPtr = cascadePortConfig;

        for(port = 0; port < CPSS_MAX_PORTS_NUM_CNS;port++)
        {
            if( !(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(dev)->existingPorts, port)))
            {
                continue;
            }

            appDemoPpConfigList[dev].ports10GToConfigureArr[index] = port;
            index++;

            /* split ports between MC FIFOs for Multicast arbiter, can't use modulo 2,
                because 40G except port 9 can be defined on even ports only and so
                the share won't be event between */
            mcFifo = (PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC((GT_U8)dev,
                                                            port) < 6) ? 0 : 1;
            rc = cpssDxChPortTxMcFifoSet((GT_U8)dev, port, mcFifo);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortTxMcFifoSet", rc);
            if( GT_OK != rc )
            {
                return rc;
            }
        }

        appDemoPpConfigList[dev].numberOf10GPortsToConfigure = index;

        /* Set Threshold for Interrupt */
        rc = cpssDxChDiagDeviceTemperatureThresholdSet((GT_U8)dev, 115);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChDiagDeviceTemperatureThresholdSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        /* Select Temperature Sensors */
        rc = cpssDxChDiagDeviceTemperatureSensorsSelectSet((GT_U8)dev,
                                   CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_AVERAGE_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChDiagDeviceTemperatureSensorsSelectSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }

        if (CPSS_NET_CPU_PORT_MODE_MII_E == appDemoPpConfigList[dev].cpuPortMode)
        {
            /* must init SMI here - we need it for PHY configuration*/
            rc = cpssDxChPhyPortSmiInit((GT_U8)dev);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiInit", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* configure the lion2 CPU port PHY - we need it here before
               cpssDxChNetIfMiiInit, because there BSP checks link up */
            cpssOsPrintf("configBoardAfterPhase2: configuring Lion2 MII CPU port PHY 88E1512.\n");
            rc = lion2AppDemoCpuPortPhyConfig((GT_U8)dev);
            if(rc != GT_OK)
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
    CPSS_PP_CONFIG_INIT_STC  localPpCfgParams = PP_LOGICAL_CONFIG_DEFAULT;

    boardRevId = boardRevId;
    devNum = devNum;

    localPpCfgParams.maxNumOfIpv4Prefixes = 3920;
     if (PRV_CPSS_PP_MAC(devNum)->devFamily == CPSS_PP_FAMILY_DXCH_LION2_E)
    {
        localPpCfgParams.numOfTrunks = APP_DEMO_127_TRUNKS_CNS;
    }
    else
    {
        localPpCfgParams.numOfTrunks = _2K-1;
    }

    localPpCfgParams.maxNumOfVirtualRouters = 1;
    localPpCfgParams.lpmDbFirstTcamLine = 100;
    localPpCfgParams.lpmDbLastTcamLine = 1204;

    if (CPSS_NET_CPU_PORT_MODE_MII_E == appDemoPpConfigList[devNum].cpuPortMode)
    {
      localPpCfgParams.miiNumOfTxDesc = TX_DESC_NUM_DEF;
      localPpCfgParams.miiTxInternalBufBlockSize = (TX_DESC_NUM_DEF / 2) *
        CPSS_GEN_NETIF_MII_TX_INTERNAL_BUFF_SIZE_CNS;
      localPpCfgParams.miiRxBufSize = RX_BUFF_SIZE_DEF;
      localPpCfgParams.miiRxBufBlockSize = RX_DESC_NUM_DEF * RX_BUFF_SIZE_DEF;
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

    /* there is no Policer lib init for CH3 devices */
    localLibInitParams.initPolicer               = GT_FALSE;

    osMemCpy(libInitParams,&localLibInitParams,
             sizeof(APP_DEMO_LIB_INIT_PARAMS));
    return GT_OK;
}

#ifndef ASIC_SIMULATION_NO_HW

/*******************************************************************************
* appDemoLinkFix
*
* DESCRIPTION:
*       Perform continuous polling of the ports with interface mode KR and
*           link down and executes WA to ensure connected link up.
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
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
unsigned __TASKCONV appDemoLinkFix
(
    GT_VOID * param
)
{
    GT_STATUS               rc;
    GT_U8                   devNum;
    GT_U32                  i;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_BOOL                 isLinkUp;
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode;
    CPSS_PORT_SPEED_ENT     speed;
    GT_BOOL                 portEnabled;
    CPSS_DXCH_IMPLEMENT_WA_ENT   waArr[1];
    GT_U32  additionalInfoBmpArr[1];

    while (1)
    {
        for(i = SYSTEM_DEV_NUM_MAC(0); i < SYSTEM_DEV_NUM_MAC(ppCounter); i++)
        {
            /* Do only for Lion2 devices */
            switch(appDemoPpConfigList[i].deviceId)
            {
                case CPSS_LION2_PORT_GROUPS_01234567_DEVICES_CASES_MAC:
                    break;      /* handle the device */
                case CPSS_LION2_HOOPER_PORT_GROUPS_0123_DEVICES_CASES_MAC:
                    break;      /* handle the device */
                default:
                    continue;   /* skip the device */
                    break;
            }

            devNum = appDemoPpConfigList[i].devNum;
            /* Skip devices with revision different than 0 */
            if(PRV_CPSS_PP_MAC(devNum)->revision != 0)
            {
                continue;
            }

            /* loop over all ports */
            for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
            {
                /* skip not existed ports */
                if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum))
                    continue;
                /* skip CPU port */
                if (portNum == CPSS_CPU_PORT_NUM_CNS)
                    continue;

                rc = cpssDxChPortSpeedGet(devNum, portNum, &speed);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortSpeedGet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* problem is only in 40G */
                if(speed != CPSS_PORT_SPEED_40000_E)
                    continue;

                rc = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifMode);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortInterfaceModeGet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }

                if((ifMode != CPSS_PORT_INTERFACE_MODE_KR_E)
                    && (ifMode != CPSS_PORT_INTERFACE_MODE_SR_LR_E))
                    continue;

                rc = cpssDxChPortLinkStatusGet(devNum, portNum, &isLinkUp);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPortLinkStatusGet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }

                /* interesting only links that are down */
                if(GT_TRUE == isLinkUp)
                    continue;

                rc = cpssDxChPortEnableGet(devNum, portNum, &portEnabled);
                if (rc != GT_OK)
                {
                    return rc;
                }

                if(GT_FALSE == portEnabled)
                    continue;

                waArr[0] = CPSS_DXCH_IMPLEMENT_WA_NO_ALLIGNMENT_LOCK_E;
                additionalInfoBmpArr[0] = portNum;
                rc = cpssDxChHwPpImplementWaInit(devNum,1,&waArr[0],
                                                 &additionalInfoBmpArr[0]);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChHwPpImplementWaInit", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }

            }/* for(portNum = 0; */

        }/* for(devNum = SYSTEM_DEV_NUM_MAC(0); */

        osTimerWkAfter(1000);

    }/*  while (1) */
}

#endif

/*******************************************************************************
* appDemoCt10And40ParamsSet
*
* DESCRIPTION:
*       Set cascaded trunk ID and
*       default source ID for network relay and uplink ports.
*
* INPUTS:
*       networkPortSrcId -  default source ID for network ports.
*       relayPortSrcId - default source ID for relay ports.
*       uplinkPortSrcId -  default source ID for uplink ports.
*       trunkId - cascaded trunk ID.
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
GT_VOID appDemoCt10And40ParamsSet
(
    GT_U32      networkPortSrcId,
    GT_U32      relayPortSrcId,
    GT_U32      uplinkPortSrcId,
    GT_TRUNK_ID trunkId,
    GT_TRUNK_ID virtualTrunkId
)
{
    networkPortDefaultSrcId = networkPortSrcId;
    relayPortDefaultSrcId = relayPortSrcId;
    uplinkPortDefaultSrcId = uplinkPortSrcId;
    cscdTrunkId = trunkId;
    virtualDefaultTrunkId = virtualTrunkId;
}

/* list of relay ports */
static    GT_PHYSICAL_PORT_NUM     relayPorts[PRV_RELAY_PORTS_NUMBER] =
                            { 64,  65,  66,  67,  72,  74,  75,
                              80,  81,  82,  83,  88,  90,  91,
                              96,  97,  98,  99, 104, 106, 107,
                             112, 113, 114};
/* list of uplink ports */
static    GT_PHYSICAL_PORT_NUM     uplinkPorts[PRV_UPLINK_PORTS_NUMBER] =
                            {68, 73, 84, 89, 100, 105};
/* Network ports bitmap */
static    CPSS_PORTS_BMP_STC       networkPortsBmp;

/*******************************************************************************
* appDemoLion2Ct10And40TrunkSet
*
* DESCRIPTION:
*       Configure uplink and relay trunks for mixed Cut-Through 40G and 10G
*       configuration.
*
* INPUTS:
*       devNum              - device number
*       numOfUplinkTrunks   - number of uplink trunks.
*       firstUplinkTrunkId  - trunk ID of first uplink trunk.
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
GT_STATUS appDemoLion2Ct10And40TrunkSet
(
    IN GT_U8        devNum,
    IN GT_U32       numOfUplinkTrunks,
    IN GT_TRUNK_ID  firstUplinkTrunkId
)
{
    GT_U32                  relayTrunksNumber;  /* number of trunks for relay ports */
    GT_U32                  relayMembers;       /* number of members in each of relay trunks */
    GT_U32                  upMembersNum;       /* number of members in each of uplink trunk */
    GT_U32                  uplinksInSourceId;  /* number of uplink ports in each relay source ID group */
    GT_U32                  ii, kk, idx, upIdx; /* iterators and indexes */
    GT_TRUNK_ID             trunkId;            /* trunk ID */
    CPSS_TRUNK_MEMBER_STC   membersArr[PRV_UPLINK_PORTS_NUMBER]; /* uplink trunk members */
    CPSS_TRUNK_MEMBER_STC   *membersArrPtr;                      /* pointer to members array */
    CPSS_PORTS_BMP_STC      relayTrunkBmp; /* bmp of ports members in the cascade trunk */
    CPSS_CSCD_LINK_TYPE_STC  cascadeLink;  /* cascade link info */
    GT_HW_DEV_NUM           hwDevNum;      /* HW device number */
    GT_STATUS               rc;            /* return code */

    /* get trunks information */
    switch (numOfUplinkTrunks)
    {
        case 0: /* configure separate relay trunk for each uplink port */
            relayTrunksNumber = 6; upMembersNum = 0; break;
        case 1: /* configure same number of uplink and relay trunks */
            relayTrunksNumber = 1; upMembersNum = 6; break;
        case 2: /* configure same number of uplink and relay trunks */
            relayTrunksNumber = 2; upMembersNum = 3; break;
        case 3: /* configure same number of uplink and relay trunks */
            relayTrunksNumber = 3; upMembersNum = 2; break;
        default: return GT_BAD_PARAM;
    }

    relayMembers = PRV_RELAY_PORTS_NUMBER / relayTrunksNumber;

    /* number of uplink ports in relay source ID group */
    uplinksInSourceId = (upMembersNum) ? upMembersNum : 1;

    /* delete previouse configured uplink trunks */
    for (ii = 0; ii < 3; ii++)
    {
        trunkId = (GT_TRUNK_ID)(firstUplinkTrunkId + ii);
        rc = cpssDxChTrunkMembersSet(devNum, trunkId, 0, NULL, 0, NULL);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkMembersSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* fill members list */
    hwDevNum = PRV_CPSS_HW_DEV_NUM_MAC(devNum);
    for (ii = 0; ii < PRV_UPLINK_PORTS_NUMBER; ii++)
    {
        membersArr[ii].port = uplinkPorts[ii];
        membersArr[ii].hwDevice = hwDevNum;
    }

    /* create uplink trunks */
    for (ii = 0; ii < numOfUplinkTrunks; ii++)
    {
        trunkId = (GT_TRUNK_ID)(firstUplinkTrunkId + ii);
        membersArrPtr = membersArr + ii * upMembersNum;
        rc = cpssDxChTrunkMembersSet(devNum, trunkId,
                                     upMembersNum, membersArrPtr,
                                     0, NULL);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkMembersSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* delete previouse configured relay trunks */
    /* clear the local ports bmp for the new cascade trunk and source ID group */
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&relayTrunkBmp);

    for (ii = 0; ii < 6; ii++)
    {
        trunkId = (GT_TRUNK_ID)(cscdTrunkId + ii);

        /* remove all relay ports from relay trunk */
        rc = cpssDxChTrunkCascadeTrunkPortsSet(devNum, trunkId, &relayTrunkBmp);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkCascadeTrunkPortsSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* configure relay trunks source ID groups */
        rc = cpssDxChBrgSrcIdGroupEntrySet(devNum,
                                           relayPortDefaultSrcId + ii,
                                           GT_TRUE, &relayTrunkBmp);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkCascadeTrunkPortsSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /* create relay trunks */
    upIdx = 0;
    for (ii = 0; ii < relayTrunksNumber; ii++)
    {
        trunkId = (GT_TRUNK_ID)(cscdTrunkId + ii);

        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&relayTrunkBmp);
        for (kk = 0; kk < relayMembers; kk++)
        {
            idx = ii*relayMembers + kk;
            if (idx >= PRV_RELAY_PORTS_NUMBER)
            {
                return GT_FAIL;
            }
            CPSS_PORTS_BMP_PORT_SET_MAC(&relayTrunkBmp,relayPorts[idx]);

            /* configure source ID per relay trunk */
            rc = cpssDxChBrgSrcIdPortDefaultSrcIdSet(devNum,
                                                     (GT_PORT_NUM)relayPorts[idx],
                                                     relayPortDefaultSrcId + ii);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdPortDefaultSrcIdSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* add relay port to own source ID group */
            rc = cpssDxChBrgSrcIdGroupPortAdd(devNum,
                                              relayPortDefaultSrcId + ii,
                                              relayPorts[idx]);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdGroupPortAdd", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* set relay ports to relay trunk */
        rc = cpssDxChTrunkCascadeTrunkPortsSet(devNum, trunkId, &relayTrunkBmp);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkCascadeTrunkPortsSet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* add uplink ports related relay trunk to source ID group of a trunk */
        for (kk = 0; kk < uplinksInSourceId; kk++)
        {
            if (upIdx >= PRV_UPLINK_PORTS_NUMBER)
            {
                return GT_FAIL;
            }

            rc = cpssDxChBrgSrcIdGroupPortAdd(devNum,
                                              relayPortDefaultSrcId + ii,
                                              uplinkPorts[upIdx++]);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdGroupPortAdd", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    /* configure device map table */
    cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;

    for(ii = 0; ii < PRV_UPLINK_PORTS_NUMBER; ii++)
    {
        if (numOfUplinkTrunks)
        {
            cascadeLink.linkNum = cscdTrunkId + (ii / upMembersNum);
        }
        else
        {
            cascadeLink.linkNum = cscdTrunkId + ii;
        }

        /* Set the cascade map table for uplink ports */
        rc = cpssDxChCscdDevMapTableSet(devNum, hwDevNum,
                                         0, uplinkPorts[ii],
                                        &cascadeLink,
                                        CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E,
                                        GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCscdDevMapTableSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoLion2Ct10And40CscdUplinkTrunkSet
*
* DESCRIPTION:
*       Configure cascade uplink trunk and relay trunks for mixed Cut-Through 40G and 10G
*       configuration.
*
* INPUTS:
*       devNum              - device number
*       hwDevNum            - HW device number
*       firstUplinkTrunkId  - trunk ID of first uplink trunk
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
GT_STATUS appDemoLion2Ct10And40CscdUplinkTrunkSet
(
    IN GT_U8            devNum,
    IN GT_HW_DEV_NUM    hwDevNum,
    IN GT_TRUNK_ID      firstUplinkTrunkId
)
{
    CPSS_PORTS_BMP_STC      portsMembersBmp;        /* bitmap ports members  */
    CPSS_CSCD_LINK_TYPE_STC cascadeLink;            /* cascade link info */
    GT_U32                  ii, kk, idx;            /* iterators and indexes */
    GT_PHYSICAL_PORT_NUM    portNum;                /* port number */
    GT_STATUS               rc;                     /* return code */
    GT_U32                  srcId;                  /* source ID */
    GT_U32                  srcIdTotal;             /* number of used source ID */
    GT_U32                  trunkIndex;             /* trunk index: relay trunks 0,1,2; uplink trunks 1,2. */

    rc = cpssDxChCscdDbRemoteHwDevNumModeSet(hwDevNum, CPSS_GEN_CFG_HW_DEV_NUM_MODE_DUAL_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCscdDbRemoteHwDevNumModeSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Set uplink ports to be cascade */
    for(ii = 2; ii < PRV_UPLINK_PORTS_NUMBER; ii++)
    {
        rc = cpssDxChCscdPortTypeSet(devNum, uplinkPorts[ii],
                                     CPSS_PORT_DIRECTION_BOTH_E,
                                     CPSS_CSCD_PORT_DSA_MODE_2_WORDS_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCscdPortTypeSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Configure uplink and relay trunks - 3 for relay and 3 for uplink ports */
    rc= appDemoLion2Ct10And40TrunkSet(devNum, 3, firstUplinkTrunkId);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* Delete uplink trunk for data ports */
    rc = cpssDxChTrunkMembersSet(devNum, firstUplinkTrunkId, 0, NULL, 0, NULL);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkMembersSet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsMembersBmp);

    /* Set virtual port member. All relay ports are in the second hemisphere,
       so dummy port belongs also to the second hemisphere */
    CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembersBmp, PRV_VIRTUAL_CSCD_TRUNK_DUMMY_PORT);

    /* Create virtual cascade trunk on dummy port */
    cpssDxChTrunkCascadeTrunkPortsSet(devNum, virtualDefaultTrunkId , &portsMembersBmp);

    /* Configure device map table */
    cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;
    cascadeLink.linkNum = PRV_VIRTUAL_CSCD_TRUNK;

    /* Loop over all ports on remote device */
    for(portNum = 0; portNum < 128; portNum++)
    {
        /* Set the cascade map table for all ports on remote device */
        rc = cpssDxChCscdDevMapTableSet(devNum, hwDevNum,
                                        0, portNum,
                                        &cascadeLink,
                                        CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E,
                                        GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChCscdDevMapTableSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Set all ports to non-trunk members */
    CPSS_PORTS_BMP_PORT_SET_ALL_MAC(&portsMembersBmp);

    /* Configure relay ports members for virtual cascade trunk */
    for (ii = 8; ii < PRV_RELAY_PORTS_NUMBER; ii++)
    {
        CPSS_PORTS_BMP_PORT_CLEAR_MAC(&portsMembersBmp, relayPorts[ii]);
    }
    /* Configure uplink ports members for virtual cascade trunk */
    for (ii = 2; ii < PRV_UPLINK_PORTS_NUMBER; ii++)
    {
        CPSS_PORTS_BMP_PORT_CLEAR_MAC(&portsMembersBmp, uplinkPorts[ii]);
    }

    /* Add ports to the trunk's non-trunk entry */
    rc = cpssDxChTrunkNonTrunkPortsEntrySet(devNum, virtualDefaultTrunkId, &portsMembersBmp);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChTrunkNonTrunkPortsAdd", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Set Src ID member table for all ports in all source ID groups */
    CPSS_PORTS_BMP_PORT_SET_ALL_MAC(&portsMembersBmp);

    for(ii = 0; ii < 8; ii++)
    {
        rc = cpssDxChBrgSrcIdGroupEntrySet(devNum, networkPortDefaultSrcId + ii,
                                           GT_TRUE, &portsMembersBmp);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdGroupEntrySet", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

    }

    srcId = networkPortDefaultSrcId;

    /* Set source ID for network ports */
    for (portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
    {
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&networkPortsBmp, portNum))
        {
            if(portNum % 2 == 0)
            {
                rc = cpssDxChBrgSrcIdPortDefaultSrcIdSet(devNum, portNum, srcId);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdPortDefaultSrcIdSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
            else
            {
                rc = cpssDxChBrgSrcIdPortDefaultSrcIdSet(devNum, portNum, srcId + 1);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdPortDefaultSrcIdSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }

    srcId += 2;

    /* Set source ID in relay trunks */
    for(ii = 0; ii < PRV_RELAY_PORTS_NUMBER; ii++)
    {
        trunkIndex = ii / 8;
        rc = cpssDxChBrgSrcIdPortDefaultSrcIdSet(devNum, relayPorts[ii], srcId + trunkIndex);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdPortDefaultSrcIdSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    srcId += 3;

    /* Set source ID of 40G up-link ports */
    rc = cpssDxChBrgSrcIdPortDefaultSrcIdSet(devNum, uplinkPorts[0], srcId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdPortDefaultSrcIdSet", rc);
    if(rc != GT_OK)
    {
        return rc;
    }
    rc = cpssDxChBrgSrcIdPortDefaultSrcIdSet(devNum, uplinkPorts[1], srcId + 1);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdPortDefaultSrcIdSet", rc);
    if(rc != GT_OK)
    {
        return rc;
    }
    srcId += 2;

    /* Total source ID numbers */
    srcIdTotal = srcId - networkPortDefaultSrcId;

    /* Clear members from Source ID table */
    for(idx = 0; idx < srcIdTotal; idx++)
    {
        switch(idx)
        {
            /* Network ports */
            case 0:
            case 1:
                /* Remove ports of uplink trunks 2 and 3 */
                for(ii = 0; ii < PRV_UPLINK_PORTS_NUMBER; ii++)
                {
                    rc = cpssDxChBrgSrcIdGroupPortDelete(devNum,
                                                         networkPortDefaultSrcId + idx,
                                                         uplinkPorts[ii]);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdGroupPortDelete", rc);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
                trunkIndex = (idx == 0) ? 2 : 1;
                /* Remove ports of relay trunk 2 or 3 */
                for(ii = 0; ii < PRV_RELAY_PORTS_NUMBER; ii++)
                {
                    kk = ii / 8;
                    if(kk == trunkIndex)
                    {
                        rc = cpssDxChBrgSrcIdGroupPortDelete(devNum,
                                                             networkPortDefaultSrcId + idx,
                                                             relayPorts[ii]);
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdGroupPortDelete", rc);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                }
                break;
            /* Relay ports */
            case 2:
            case 3:
            case 4:
                /* Remove ports of all relay trunks */
                for(ii = 0; ii < PRV_RELAY_PORTS_NUMBER; ii++)
                {
                    rc= cpssDxChBrgSrcIdGroupPortDelete(devNum,
                                                        networkPortDefaultSrcId + idx,
                                                        relayPorts[ii]);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdGroupPortDelete", rc);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
                /* Remove all network ports */
                for (portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
                {
                    if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&networkPortsBmp, portNum))
                    {
                        rc = cpssDxChBrgSrcIdGroupPortDelete(devNum,
                                                             networkPortDefaultSrcId + idx,
                                                             portNum);
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdGroupPortDelete", rc);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                }
                if(idx == 2)
                {
                    /* Remove ports of uplink trunks 2 and 3 */
                    for(ii = 2; ii < PRV_UPLINK_PORTS_NUMBER; ii++)
                    {
                        rc = cpssDxChBrgSrcIdGroupPortDelete(devNum,
                                                             networkPortDefaultSrcId + idx,
                                                             uplinkPorts[ii]);
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdGroupPortDelete", rc);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                }
                else
                {
                    /* Remove 2x40G uplink ports */
                    rc = cpssDxChBrgSrcIdGroupPortDelete(devNum,
                                                         networkPortDefaultSrcId + idx,
                                                         uplinkPorts[0]);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdGroupPortDelete", rc);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                    rc = cpssDxChBrgSrcIdGroupPortDelete(devNum,
                                                         networkPortDefaultSrcId + idx,
                                                         uplinkPorts[1]);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdGroupPortDelete", rc);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }

                    trunkIndex = (idx == 3) ? 2 : 1;
                    /* Remove ports of uplink trunk 2 or 3 */
                    for(ii = 2; ii < PRV_RELAY_PORTS_NUMBER; ii++)
                    {
                        kk = ii / 2;
                        if(kk == trunkIndex)
                        {
                            rc = cpssDxChBrgSrcIdGroupPortDelete(devNum,
                                                                 networkPortDefaultSrcId + idx,
                                                                 uplinkPorts[ii]);
                            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdGroupPortDelete", rc);
                            if(rc != GT_OK)
                            {
                                return rc;
                            }
                        }
                    }
                }
                break;
            /* Uplink 40G ports */
            case 5:
            case 6:
                /* Remove ports of all relay trunks */
                for(ii = 0; ii < PRV_RELAY_PORTS_NUMBER; ii++)
                {
                    rc = cpssDxChBrgSrcIdGroupPortDelete(devNum,
                                                         networkPortDefaultSrcId + idx,
                                                         relayPorts[ii]);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdGroupPortDelete", rc);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
                trunkIndex = (idx == 5) ? 2 : 1;
                /* Remove ports of uplink trunk 2 or 3 */
                for(ii = 2; ii < PRV_RELAY_PORTS_NUMBER; ii++)
                {
                    kk = ii / 2;
                    if(kk == trunkIndex)
                    {
                        rc = cpssDxChBrgSrcIdGroupPortDelete(devNum,
                                                             networkPortDefaultSrcId + idx,
                                                             uplinkPorts[ii]);
                        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgSrcIdGroupPortDelete", rc);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                }
                break;
            default:
                return GT_OUT_OF_RANGE;
        }
    }

    return rc;
}

GT_STATUS appDemoDxChSourceIdDump
(
    IN GT_U8 devNum,
    IN GT_U32 startIdx,
    IN GT_U32 numOfGroups
)
{
    GT_U32                  ii; /* iterators and indexes */
    CPSS_PORTS_BMP_STC      membersBmp; /* bmp of ports members in the cascade trunk */
    GT_BOOL                 cpu;
    GT_STATUS               rc = GT_OK;            /* return code */

    cpssOsPrintf(" Entry CPU word 0 word 1 word 2 word 3\n");
    for (ii = startIdx; ii < numOfGroups; ii++)
    {
        rc = cpssDxChBrgSrcIdGroupEntryGet(devNum,ii,&cpu,&membersBmp);
        if (rc != GT_OK)
        {
            cpssOsPrintf("Read entry [%d] error %d\n", ii, rc);
        }

        cpssOsPrintf(" %2d %d %08X %08X %08X %08X\n", ii, cpu, membersBmp.ports[0],membersBmp.ports[1],membersBmp.ports[2],membersBmp.ports[3]);
    }
    cpssOsPrintf(" ==================================\n");
    return rc;
}

/*******************************************************************************
* prvLoopBackCt10And40Config
*
* DESCRIPTION:
*       loopBack configurations.
*
* INPUTS:
*       devNum     -  Device number.
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
static GT_STATUS prvLoopBackCt10And40Config
(
    GT_U8 devNum
)
{
    GT_STATUS   rc;
    GT_PHYSICAL_PORT_NUM     uplinkOccupiedPorts[PRV_UPLINK_OCCUPIED_PORTS_NUMBER] =
                            {69, 70, 71, 85, 86, 87, 101, 102, 103};
                                           /* port number occupied by 40G uplink pors.
                                              Should be skipped by configurations. */

    GT_PHYSICAL_PORT_NUM     sgmiiPorts[PRV_SGMII_PORTS_NUMBER] =
                            {115};

    CPSS_PORTS_BMP_STC       uplinkPortsBmp;
    GT_U32 i;
    CPSS_PORTS_BMP_STC       cscdTrunkBmp;/* bmp of ports members in the cascade trunk */
    GT_U8                    portNum;   /* port number */
    CPSS_DIRECTION_ENT       portDirection;
    GT_BOOL                  devMapLookupEnable;
    GT_U32                   defaultSrcId;
    GT_U32                   filterSrcId;
    GT_BOOL                  cutThroughEnable;
    CPSS_CSCD_LINK_TYPE_STC  cascadeLink;
    CPSS_PORT_SPEED_ENT      portSpeed;
    GT_BOOL skipPort;       /* skip port from network ports configuration */
    CPSS_DXCH_TRUNK_LBH_CRC_MASK_ENTRY_STC maskEntry; /* CRC Hash Mask entry */
    GT_PORT_GROUPS_BMP       portGroupsBmp;

    /* clear uplink ports */
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&uplinkPortsBmp);

    /* clear network ports */
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&networkPortsBmp);

    for(i = 0; i < PRV_UPLINK_PORTS_NUMBER; i++)
    {
        CPSS_PORTS_BMP_PORT_SET_MAC(&uplinkPortsBmp,uplinkPorts[i]);
        /* save to DB the uplink ports */
        CPSS_PORTS_BMP_PORT_SET_MAC((&appDemoPpConfigList[devNum].uplinkPortGroupPortsBmp),
            uplinkPorts[i]);
    }

     /* clear the local ports bmp for the new cascade trunk */
     CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&cscdTrunkBmp);

     for(i = 0; i < PRV_RELAY_PORTS_NUMBER; i++)
     {
         CPSS_PORTS_BMP_PORT_SET_MAC(&cscdTrunkBmp,relayPorts[i]);
         /* save to DB the relay ports */
         CPSS_PORTS_BMP_PORT_SET_MAC((&appDemoPpConfigList[devNum].relayPortGroupPortsBmp),
             relayPorts[i]);
     }

     /* Insert all Relay Ports in to Cascade Trunk */
     rc = cpssDxChTrunkCascadeTrunkPortsSet(devNum, cscdTrunkId, &cscdTrunkBmp);
     if(rc != GT_OK)
     {
         return rc;
     }

     /* Change Uplink Ports from 10GB to 40GB */
     rc = cpssDxChPortModeSpeedSet(devNum, uplinkPortsBmp, GT_TRUE,
                                   CPSS_PORT_INTERFACE_MODE_SR_LR_E,
                                   CPSS_PORT_SPEED_40000_E);
     if(rc != GT_OK)
     {
         return rc;
     }

     /* MRU configuration for all ports */
     for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
     {
         /* skip not existed ports */
         if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum))
             continue;
         /* skip CPU port */
         if (portNum == CPSS_CPU_PORT_NUM_CNS)
             continue;

         /* MRU Configuration */
         rc = cpssDxChPortMruSet(devNum, portNum, 10240);
         if(rc != GT_OK)
         {
             return rc;
         }
     }

     /* set maximal VLAN MRU for CT traffic */
     rc = cpssDxChBrgVlanMruProfileValueSet(devNum, 0, 0xFFFF);
     if(rc != GT_OK)
     {
         return rc;
     }

     /* Cscd Dsa Src Dev Filter - Enable Own Dev Filter set to false */
     rc = cpssDxChCscdDsaSrcDevFilterSet(devNum, GT_FALSE);
     if(rc != GT_OK)
     {
         return rc;
     }

     /* Set relay ports to be cascade */
     for(i = 0; i < PRV_RELAY_PORTS_NUMBER; i++)
     {
         rc = cpssDxChCscdPortTypeSet(devNum, relayPorts[i],
                                      CPSS_PORT_DIRECTION_BOTH_E,
                                      CPSS_CSCD_PORT_DSA_MODE_2_WORDS_E);
         if(rc != GT_OK)
         {
             return rc;
         }

     }

     /* Set Ingress Src Id */
     for(portNum = 0; portNum < PRV_CPSS_PP_MAC(devNum)->numOfPorts; portNum++)
     {
         /* skip not existed ports */
         if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum))
             continue;
         /* skip CPU port */
         if (portNum == CPSS_CPU_PORT_NUM_CNS)
             continue;

         skipPort = GT_FALSE;
         /* skip port number occupied by 40 G ports */
         for(i = 0; i < PRV_UPLINK_OCCUPIED_PORTS_NUMBER ; i++)
         {
             if(portNum == uplinkOccupiedPorts[i])
             {
                 skipPort = GT_TRUE;
                 break;
             }
         }

         /* skip SGMII ports */
         for(i = 0; i < PRV_SGMII_PORTS_NUMBER ; i++)
         {
             if(portNum == sgmiiPorts[i])
             {
                 skipPort = GT_TRUE;
                 break;
             }
         }

         if (skipPort == GT_TRUE)
         {
             continue;
         }

         /* set default configurations for network ports */
         portDirection = CPSS_DIRECTION_INGRESS_E;
         devMapLookupEnable = GT_TRUE;
         defaultSrcId = networkPortDefaultSrcId;

         /* need to remove networks ports from relay ports Source ID group */
         filterSrcId = relayPortDefaultSrcId;
         cutThroughEnable = GT_TRUE;
         portSpeed = CPSS_PORT_SPEED_10000_E;

         /* override configuration settings for relay ports */
         for(i = 0; i < PRV_RELAY_PORTS_NUMBER; i++)
         {
             if(portNum == relayPorts[i])
             {
                 devMapLookupEnable = GT_FALSE;
                 defaultSrcId = relayPortDefaultSrcId;
                 /* need to remove relay ports from Uplink ports Source ID group */
                 filterSrcId = uplinkPortDefaultSrcId;
                 cutThroughEnable = GT_FALSE;
                 portSpeed = CPSS_PORT_SPEED_10000_E;
                 break;
             }
         }

         /* override configuration settings for relay ports */
         for(i = 0; i < PRV_UPLINK_PORTS_NUMBER; i++)
         {
             if(portNum == uplinkPorts[i])
             {
                 portDirection = CPSS_DIRECTION_EGRESS_E;
                 defaultSrcId = uplinkPortDefaultSrcId;
                 /* need to remove uplink ports from Network ports Source ID group */
                 filterSrcId = networkPortDefaultSrcId;
                 cutThroughEnable = GT_TRUE;
                 portSpeed = CPSS_PORT_SPEED_40000_E;
                 break;
             }
         }

         /* Set network ports bitmap */
         if(defaultSrcId == networkPortDefaultSrcId)
         {
             CPSS_PORTS_BMP_PORT_SET_MAC(&networkPortsBmp, portNum);
         }

         /*  Enable the local target port for device map lookup
             for network and uplink ports */
         if(devMapLookupEnable == GT_TRUE)
         {
             rc = cpssDxChCscdPortLocalDevMapLookupEnableSet(devNum,
                                                     (GT_PHYSICAL_PORT_NUM)portNum,
                                                      portDirection, GT_TRUE);
             if(rc != GT_OK)
             {
                 return rc;
             }
         }

        /* Ingress Src Id Configuration */
         rc = cpssDxChBrgSrcIdPortSrcIdForceEnableSet(devNum,
                                                      (GT_PORT_NUM)portNum,
                                                       GT_TRUE);
         if(rc != GT_OK)
         {
             return rc;
         }

         rc = cpssDxChBrgSrcIdPortDefaultSrcIdSet(devNum, (GT_PORT_NUM)portNum,
                                                  defaultSrcId);
         if(rc != GT_OK)
         {
             return rc;
         }

         /* SRC Id filter configurations */
         rc =  cpssDxChBrgSrcIdGroupPortDelete(devNum, filterSrcId,
                                               (GT_PORT_NUM)portNum);
         if(rc != GT_OK)
         {
             return rc;
         }

         if(cutThroughEnable == GT_TRUE)
         {
             rc = cpssDxChCutThroughPortEnableSet(devNum,
                                                  (GT_PHYSICAL_PORT_NUM)portNum,
                                                  GT_TRUE, GT_TRUE);
             if(rc != GT_OK)
             {
                 return rc;
             }
         }
         /* CT TXDMA rate limit */
         rc = cpssDxChCutThroughMemoryRateLimitSet(devNum,
                                                  (GT_PHYSICAL_PORT_NUM)portNum,
                                                   GT_TRUE, portSpeed);
         if(rc != GT_OK)
         {
             return rc;
         }
     }

     /* CT Enable per UP */
     for(i = 0; i < 8; i++)
     {
         rc = cpssDxChCutThroughUpEnableSet(devNum, (GT_U8)i, GT_TRUE);
         if(rc != GT_OK)
         {
             return rc;
         }

     }

     /* Set lookup mode for accessing the Device Map table */
     rc = cpssDxChCscdDevMapLookupModeSet(devNum,
                              CPSS_DXCH_DEV_MAP_LOOKUP_MODE_TRG_DEV_TRG_PORT_E);
     if(rc != GT_OK)
     {
         return rc;
     }

     cascadeLink.linkType = CPSS_CSCD_LINK_TYPE_TRUNK_E;
     cascadeLink.linkNum = cscdTrunkId;

     for(i = 0; i < PRV_UPLINK_PORTS_NUMBER; i++)
     {
         /* Set the cascade map table for uplink ports */
         rc = cpssDxChCscdDevMapTableSet(devNum, PRV_CPSS_HW_DEV_NUM_MAC(devNum),
                                          0, uplinkPorts[i],
                                         &cascadeLink,
                                         CPSS_DXCH_CSCD_TRUNK_LINK_HASH_IS_INGR_PIPE_HASH_E,
                                         GT_FALSE);
         if(rc != GT_OK)
         {
             return rc;
         }
     }

     /* trunk CRC hash configuration */
     rc = cpssDxChTrunkHashGlobalModeSet(devNum,CPSS_DXCH_TRUNK_LBH_PACKETS_INFO_CRC_E);
     if(rc != GT_OK)
     {
         return rc;
     }

     /* using the CRC 6, seed is 0x15 */
     rc = cpssDxChTrunkHashCrcParametersSet(devNum, CPSS_DXCH_TRUNK_LBH_CRC_6_MODE_E, 0x15);
     if(rc != GT_OK)
     {
         return rc;
     }

     /* Mask: for all packet types configured to mask out the UDBs, and to include all the other fields.*/
     cpssOsMemSet(&maskEntry, 0, sizeof(maskEntry));
     maskEntry.l4DstPortMaskBmp = maskEntry.l4SrcPortMaskBmp = 3; /* two bytes */
     maskEntry.ipDipMaskBmp = maskEntry.ipSipMaskBmp = 0xFFFF; /* 16 bytes */
     maskEntry.macDaMaskBmp = maskEntry.macSaMaskBmp = 0x3F;   /* 6 bytes */
     maskEntry.mplsLabel0MaskBmp =
     maskEntry.mplsLabel1MaskBmp =
     maskEntry.mplsLabel2MaskBmp =
     maskEntry.ipv6FlowMaskBmp = 7; /* three bytes */
/*   maskEntry.localSrcPortMaskBmp = 1; */ /* one byte */

     /* configure entries for all packet types */
     for (i = 16; i < 28; i++)
     {
         rc = cpssDxChTrunkHashMaskCrcEntrySet(devNum, i, &maskEntry);
         if(rc != GT_OK)
         {
             return rc;
         }
     }

     /* Pearson table: configured to an arbitrary permutation of the numbers 0…63 */
     for (i = 0; i < 64; i++)
     {
         /* set value = index */
         rc = cpssDxChTrunkHashPearsonValueSet(devNum, i, i);
         if(rc != GT_OK)
         {
             return rc;
         }
     }

     for(i = 0; i < PRV_RELAY_PORTS_NUMBER; i++)
     {
         /* Set Port loopback Configuration*/
         rc = cpssDxChPortInternalLoopbackEnableSet(devNum, relayPorts[i],
                                                    GT_TRUE);
         if(rc != GT_OK)
         {
             return rc;
         }

         rc = cpssDxChPortXGmiiModeSet(devNum, relayPorts[i],
                                       CPSS_PORT_XGMII_FIXED_E);
         if(rc != GT_OK)
         {
             return rc;
         }

         /* set shorter IPG : 8 bytes  */
         rc  = cpssDxChPortIpgBaseSet(devNum, relayPorts[i],
                                      CPSS_PORT_XG_FIXED_IPG_8_BYTES_E);
         if(rc != GT_OK)
         {
             return rc;
         }

         /* set short preamble */
         rc = cpssDxChPortPreambleLengthSet(devNum, relayPorts[i],
                                            CPSS_PORT_DIRECTION_BOTH_E, 4);
         if(rc != GT_OK)
         {
             return rc;
         }

         /* disable router SA modification */
         rc = cpssDxChIpRouterMacSaModifyEnable(devNum, relayPorts[i], GT_FALSE);
         if(rc != GT_OK)
         {
             return rc;
         }

         /* enable trust DSA */
         rc = cpssDxChCosTrustDsaTagQosModeSet(devNum, relayPorts[i], GT_TRUE);
         if(rc != GT_OK)
         {
             return rc;
         }

         /* Enable Force Link Pass on relay ports */
         rc = cpssDxChPortForceLinkPassEnableSet(devNum, relayPorts[i], GT_TRUE);
         if(rc != GT_OK)
         {
             return rc;
         }

         /* Enable forwarding a new mac address message to CPU */
         rc = cpssDxChBrgFdbNaToCpuPerPortSet(devNum, relayPorts[i], GT_FALSE);
         if(rc != GT_OK)
         {
             return rc;
         }

     }

     /* set the router source id assignmnet */
     portGroupsBmp = 0x8F;
     rc = cpssDxChIpPortGroupRouterSourceIdSet(devNum, portGroupsBmp, CPSS_IP_UNICAST_E,
                                               networkPortDefaultSrcId,
                                               0x0FFF /* source id mask */);

     if(rc != GT_OK)
     {
         return rc;
     }

     rc = cpssDxChIpPortGroupRouterSourceIdSet(devNum, portGroupsBmp,
                                               CPSS_IP_MULTICAST_E,
                                               networkPortDefaultSrcId,
                                               0x0FFF /* source id mask */);

     if(rc != GT_OK)
     {
         return rc;
     }

     /* configure uplink source port groups */
     portGroupsBmp = 0x70;
     rc = cpssDxChIpPortGroupRouterSourceIdSet(devNum, portGroupsBmp,
                                               CPSS_IP_UNICAST_E,
                                               uplinkPortDefaultSrcId,
                                               0x0FFF /* source id mask */);

     if(rc != GT_OK)
     {
         return rc;
     }

     rc = cpssDxChIpPortGroupRouterSourceIdSet(devNum, portGroupsBmp,
                                               CPSS_IP_MULTICAST_E,
                                               uplinkPortDefaultSrcId,
                                               0x0FFF /* source id mask */);
     if(rc != GT_OK)
     {
         return rc;
     }

     /* set zero uplink trunks by default */
     rc = appDemoLion2Ct10And40TrunkSet(devNum, 0, 45/* do not care */);
     if(rc != GT_OK)
     {
         return rc;
     }

     /* Init cut-through WA for trunks */
     rc = appDemoLion2TrunkWaInit(devNum, cscdTrunkId, relayPortDefaultSrcId,
                                  uplinkPorts, relayPorts);
     if(rc != GT_OK)
     {
         return rc;
     }

     /* Bind Galtis wrappers with application functions */
     appDemoLion2TrunkExtServicesBind();

     return rc;
}

GT_STATUS appDemoLion2DevMapDump(void)
{
    GT_U32 egr, portGroupId, egrBase;
    GT_U32 srcEnAddr, dstEnAddr;
    GT_U32 srcEnData[4], dstEnData[2];
    GT_STATUS rc;
    GT_U8 devNum = 0;

    for (egr = 0; egr < 16; egr++)
    {
        portGroupId = egr / 2;
        egrBase = 0x12000000 + (egr & 1)*0x00800000;

        srcEnAddr = egrBase + 0x1110;
        dstEnAddr = egrBase + 0x1120;

        /*read src enable */
        rc = prvCpssDrvHwPpPortGroupReadRam(devNum, portGroupId, srcEnAddr, 4, srcEnData);
        if(rc != GT_OK)
        {
            return rc;
        }

        /*read dst enable */
        rc = prvCpssDrvHwPpPortGroupReadRam(devNum, portGroupId, dstEnAddr, 2, dstEnData);
        if(rc != GT_OK)
        {
            return rc;
        }

        osPrintf("\nEGR %d SRC Addr %08X DST Addr %08X\n", egr, srcEnAddr, dstEnAddr);
        osPrintf("SRC Data %08X %08X %08X %08X\n", srcEnData[0],srcEnData[1],srcEnData[2],srcEnData[3]);
        osPrintf("DST Data %08X %08X\n", dstEnData[0],dstEnData[1]);
    }

    /* dist unit */
    srcEnAddr = 0x13000210;
    portGroupId = 0;

    /*read src enable */
    rc = prvCpssDrvHwPpPortGroupReadRam(devNum, portGroupId, srcEnAddr, 4, srcEnData);
    if(rc != GT_OK)
    {
        return rc;
    }

    osPrintf("\nDist SRC Addr %08X\n", srcEnAddr);
    osPrintf("SRC Data %08X %08X %08X %08X\n", srcEnData[0],srcEnData[1],srcEnData[2],srcEnData[3]);

    /* dist unit */
    srcEnAddr = 0x13000220;

    /*read dst enable */
    rc = prvCpssDrvHwPpPortGroupReadRam(devNum, portGroupId, srcEnAddr, 4, srcEnData);
    if(rc != GT_OK)
    {
        return rc;
    }

    osPrintf("Dist DST Addr %08X\n", srcEnAddr);
    osPrintf("DST Data %08X %08X %08X %08X\n\n", srcEnData[0],srcEnData[1],srcEnData[2],srcEnData[3]);

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
    GT_STATUS               rc, rcGetNext;
    GT_U8                   dev;
    GT_U32                  trxTraining;
    GT_PHYSICAL_PORT_NUM    portNum;

#ifdef _VXWORKS
        /* tick's calculation based on testing - need for training and AP tasks */
        kernelSliceTimeSet(20/(1000/sysClkRateGet()));
#endif

    /* allow processing of AA messages */
    appDemoSysConfig.supportAaMessage = GT_TRUE;

    if(appDemoSysConfig.forceAutoLearn == GT_FALSE)
    {
        /* the Lion need 'Controlled aging' because the port groups can't share refresh info */
        /* the AA to CPU enabled from appDemoDxChFdbInit(...) --> call cpssDxChBrgFdbAAandTAToCpuSet(...)*/
        for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
        {
            rc = cpssDxChBrgFdbActionModeSet(dev,CPSS_FDB_ACTION_AGE_WITHOUT_REMOVAL_E);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    if(appDemoDbEntryGet("trxTraining", &trxTraining) != GT_OK)
    {
        trxTraining = 0;
    }

    /* note that the appDemo sets the aging mode to 'Triggered mode' by default
       so no auto aging unless you set it explicitly */
    for(dev = 0xff; (rcGetNext = cpssPpCfgNextDevGet(dev, &dev)) == GT_OK;)
    {
        if(!trxTraining)
        {/* if only RX training used can configure default optimizations algorithms */
            /* loop over all ports */
            for(portNum = 0; portNum < PRV_CPSS_PP_MAC(dev)->numOfPorts; portNum++)
            {
                /* skip not existed ports */
                if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(dev, portNum))
                    continue;
                /* skip CPU port */
                if (portNum == CPSS_CPU_PORT_NUM_CNS)
                    continue;

                rc = cpssDxChPortSerdesAutoTuneOptAlgSet(dev, portNum, 
                                 CPSS_PORT_SERDES_TRAINING_OPTIMISATION_FFE_E
                                 | CPSS_PORT_SERDES_TRAINING_OPTIMISATION_ALIGN90_E);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }
        if (systemRecoveryInfo.systemRecoveryProcess == CPSS_SYSTEM_RECOVERY_PROCESS_NOT_ACTIVE_E)
        {
            rc = lion2AppDemoPortInterfaceInit(dev, boardRevId);
            if(rc != GT_OK)
            {
                return rc;
            }

            if(boardRevId == 5)
            {
                rc = prvLoopBackCt10And40Config(dev);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
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

    if((rcGetNext != GT_OK) && (rcGetNext != GT_NO_MORE))
    {
        return rcGetNext;
    }
    if (systemRecoveryInfo.systemRecoveryProcess == CPSS_SYSTEM_RECOVERY_PROCESS_NOT_ACTIVE_E)
    {
        /* when Lion2 - param 11 - work in PFC enabled mode */
        if (boardRevId == 11)
        {
            for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
            {
                rc = appDemoPfcEnableConfiguration(dev);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
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
* lion2AppDemoCpuPortPhyConfig
*
* DESCRIPTION:
*       Initiate the Phy (88E1512). It prepares the Phy device to automatically
*       adapt itself for working at Copper either at SGMII modes (it will be done
*       via the MSMI_0 path).
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       gigPhyRegWrite 0 0 0 0x16 0 0xff
*       gigPhyRegWrite 0 0 0 0x11 0 0x214b
*       gigPhyRegWrite 0 0 0 0x10 0 0x2144
*       gigPhyRegWrite 0 0 0 0x11 0 0xc28
*       gigPhyRegWrite 0 0 0 0x10 0 0x2146
*       gigPhyRegWrite 0 0 0 0x11 0 0xb233
*       gigPhyRegWrite 0 0 0 0x10 0 0x214d
*       gigPhyRegWrite 0 0 0 0x11 0 0xcc0c
*       gigPhyRegWrite 0 0 0 0x10 0 0x2159
*       gigPhyRegWrite 0 0 0 0x16 0 0xfb
*       gigPhyRegWrite 0 0 0 0x7 0 0xc00d
*       gigPhyRegWrite 0 0 0 0x16 0 0x12
*       gigPhyRegWrite 0 0 0 0x14 0 0x8017
*       gigPhyRegWrite 0 0 0 0x16 0 0x0
*
*******************************************************************************/
GT_STATUS lion2AppDemoCpuPortPhyConfig
(
    IN  GT_U8   devNum
)
{
    GT_STATUS           rc;
    GT_U32              i;
    GT_U32              phyConfigArray[][2] =
    {   /* regAddr      data    */
        {   0x16,       0xff    }
        ,{  0x11,       0x214b  }
        ,{  0x10,       0x2144  }
        ,{  0x11,       0xc28   }
        ,{  0x10,       0x2146  }
        ,{  0x11,       0xb233  }
        ,{  0x10,       0x214d  }
        ,{  0x11,       0xcc0c  }
        ,{  0x10,       0x2159  }
        ,{  0x16,       0xfb    }
        ,{  0x7 ,       0xc00d  }
        ,{  0x16,       0x12    }
        ,{  0x14,       0x8017  }
        ,{  0x16,       0x0     }
        ,{  0xffff,     0xffff  }
    };

    /* Disable SMI AutoNeg */
    if (prvCpssDrvHwPpSetRegField(devNum, PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                  smiRegs.lms0PhyAutoNeg[0], 7, 1, 1) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    for(i = 0; phyConfigArray[i][0] != 0xffff; i++)
    {
        rc = cpssSmiRegisterWriteShort(devNum, 1/* SMI bus connected to core 0 */,
                                       CPSS_PHY_SMI_INTERFACE_0_E,
                                       0 /* smiAddr */,
                                       phyConfigArray[i][0],
                                       (GT_U16)phyConfigArray[i][1]);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    /* Enable SMI AutoNeg */
    if (prvCpssDrvHwPpSetRegField(devNum, PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->
                                  smiRegs.lms0PhyAutoNeg[0], 7, 1, 0) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    return GT_OK;
}

#ifndef ASIC_SIMULATION_NO_HW

#if (defined _VXWORKS) && (defined CPU_78200)
extern void mvSysTimerMeasureStart
(
    void
);

extern void mvSysTimerMeasureGet
(
    GT_U32 *miliSecPtr
);

#endif /* (defined _VXWORKS) && (defined CPU_78200) */

#ifdef LINUX

GT_U32  startSeconds;
GT_U32  startNanoSeconds;

void mvSysTimerMeasureStart
(
    void
)
{
    GT_STATUS   rc;

    rc = cpssOsTimeRT(&startSeconds, &startNanoSeconds);
    if(rc != GT_OK)
    {
        cpssOsPrintf("cpssOsTimeRT failed\n");
    }

    return;
}

void mvSysTimerMeasureGet
(
    GT_U32 *miliSecPtr
)
{
    GT_STATUS   rc;
    GT_U32      seconds, secondsDiff;
    GT_U32      nanoSeconds, nanoSecondsDiff;

    rc = cpssOsTimeRT(&seconds, &nanoSeconds);
    if(rc != GT_OK)
    {
        cpssOsPrintf("cpssOsTimeRT failed\n");
        return;
    }

    secondsDiff = seconds - startSeconds;
    if(nanoSeconds > startNanoSeconds)
    {
        nanoSecondsDiff = nanoSeconds - startNanoSeconds;
    }
    else
    {
        secondsDiff--;/* borrow for lower grade substruction */
        nanoSecondsDiff = 1000000000+nanoSeconds - startNanoSeconds;
    }

    *miliSecPtr = secondsDiff*1000 + nanoSecondsDiff/1000000;

    return;
}
#endif

static GT_U32 doTimeMeasure = 0;
static GT_U32 doTimeMeasurePort = 0;
extern GT_U32 apDoTimeMeasure
(
    IN  GT_U32                  enable,
    IN  GT_PHYSICAL_PORT_NUM    portNum
)
{
    GT_U32  oldState = doTimeMeasure;
    doTimeMeasure = enable;
    doTimeMeasurePort = portNum;

    return oldState;
}

#ifdef _VXWORKS

#define INT_LOCK(key)                                                   \
    key = 0;                                                            \
    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &(key))

#define INT_UNLOCK(key)                                                 \
    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &(key))

#else /* Linux and FreeBSD */

#define INT_LOCK(key)                                                   \
    key = 0;                                                            \
    cpssOsTaskLock();                                                    \
    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_LOCK_E, &(key))

#define INT_UNLOCK(key)                                                     \
    (GT_VOID)cpssExtDrvSetIntLockUnlock(CPSS_OS_INTR_MODE_UNLOCK_E, &(key));\
    cpssOsTaskUnLock()

#endif /* _VXWORKS */

static CPSS_OS_SIG_SEM apRxSmid; /* semaphore to manage AP RX optimization task */

/* for now maximal number of serdes per port where align90 could run is 4 */
CPSS_DXCH_PORT_ALIGN90_PARAMS_STC serdesParams[CPSS_MAX_PORTS_NUM_CNS][4];

static GT_U32   allowPrint=0;/* option to disable the print in runtime*/
#define AP_DBG_PRINT_MAC(x) if(allowPrint) cpssOsPrintf x

extern GT_U32 apTaskPrintControl(IN GT_U32  allowPrintNew)
{
    GT_U32  oldState = allowPrint;
    allowPrint = allowPrintNew;

    return oldState;
}

static GT_U32 trxTrainingEn = 1;
extern GT_U32 trxTrainingControl(IN GT_U32  trxTrainingEnNew)
{
    GT_U32  oldState = trxTrainingEn;
    trxTrainingEn = trxTrainingEnNew;

    return oldState;
}

typedef struct
{
    GT_U8       devNum;     /* device number */
    GT_U32      portGroup;   /* number of core to serve */
    GT_UINTPTR  evntHndl;    /* event handler pointer */
} APPDEMO_AP_TASK_PARAMS_STC;

static CPSS_PORTS_BMP_STC todoRxTuneBmp; /* bitmap of ports to run additional 
                                                optimized RX training */

/*******************************************************************************
* apRestartOnPort
*
* DESCRIPTION:
*       Cause restart AP on port
*
* INPUTS:
*       devNum - device number
*       portNum - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       other - on fail
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS apRestartOnPort
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum   
)
{
    GT_STATUS rc;

    AP_DBG_PRINT_MAC(("mac and pcs reset:portNum=%d\n", portNum));
    rc = cpssDxChPortMacResetStateSet(devNum, portNum, GT_TRUE);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssDxChPortMacResetStateSet:rc=%d,portNum=%d\n",
                        rc, portNum);
        return rc;
    }
    rc = cpssDxChPortPcsResetSet(devNum, portNum, 
                                 CPSS_PORT_PCS_RESET_MODE_ALL_E,
                                 GT_TRUE);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssDxChPortPcsResetSet:rc=%d,portNum=%d\n",
                        rc, portNum);
        return rc;
    }

    rc = cpssDxChPortApUnLock(devNum, portNum);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssDxChPortApUnLock:portNum=%d,rc=%d\n",
                        portNum, rc);
        return rc;
    }

    rc = cpssDxChPortEnableSet(devNum, portNum, GT_TRUE);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssDxChPortEnableSet:rc=%d,portNum=%d,GT_TRUE\n",
                        rc, portNum);
    }

    return rc;
}
static GT_CHAR **dumpArrayPtr = NULL;
static GT_SEM dumpArraySem = 0;

#define MAX_DUMP_STRING_LENGTH_CNS  128

static GT_CHAR str[MAX_DUMP_STRING_LENGTH_CNS];

static GT_BOOL dumpToBuffer = GT_TRUE;

GT_U32 dumpToBufferSet
(
    GT_U32 dumpToBufferNew
)
{
    GT_U32 dumpToBufferOld;

    dumpToBufferOld = dumpToBuffer;
    dumpToBuffer = (1 == dumpToBufferNew) ? GT_TRUE : GT_FALSE;

    return dumpToBufferOld;
}


static GT_VOID addRecord
(
    IN  GT_CHAR **dumpArrayPtr,    
    IN  const GT_CHAR *strPtr
)
{
    static GT_U32  index = 0;    

    if(NULL == dumpArrayPtr) 
    {
        cpssOsPrintf("NULL == dumpArrayPtr\n");
        return;
    }
        
    cpssOsSigSemWait(dumpArraySem, 0 /* wait for ever */);

    if(NULL == dumpArrayPtr[index])
    {
        cpssOsPrintf("NULL == dumpArrayPtr[%d]\n", index);
        return;
    }

    cpssOsSprintf(dumpArrayPtr[index], "%s", strPtr);
    
    index = (index + 1)%_8K;

    cpssOsSigSemSignal(dumpArraySem);

    return;
}


static GT_VOID dumpArray
(
    IN  GT_CHAR **dumpArrayPtr
)
{
    GT_U32  i;
    
    if(NULL == dumpArrayPtr)
    {
        cpssOsPrintf("NULL == dumpArrayPtr\n");
        return;
    }

    cpssOsSigSemWait(dumpArraySem, 0 /* wait for ever */);
    for(i = 0; i < _8K; i++)
    {
        if(NULL == dumpArrayPtr[i])
        {
            cpssOsPrintf("NULL == dumpArrayPtr[%d]\n", i);            
            break;
        }
        if(cpssOsStrlen(dumpArrayPtr[i]) != 0) 
        {
            cpssOsPrintf("%s", dumpArrayPtr[i]);
        }
        else
        {
            break;
        }
    }
    cpssOsSigSemSignal(dumpArraySem);

    return;
}
/*******************************************************************************
* lion2AppDemoApResolvedPortsConfig
*
* DESCRIPTION:
*       Configure ports of core that get AP resolution and get bitmap of ports
*       where need to run TRX training
*
* INPUTS:
*       devNum - device number
*       portGroupId - core number
*
* OUTPUTS:
*       trxTrainBmpPtr - (pointer to) bitmap of ports where need to run TRX
*                           training
*
* RETURNS:
*       None.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_VOID lion2AppDemoApResolvedPortsConfig
(
    IN      GT_U8               devNum,
    IN      GT_U32              portGroupId,
    INOUT   CPSS_PORTS_BMP_STC *trxTrainBmpPtr
)
{
    GT_STATUS           rc;                /* return code         */
    GT_U32              i;                 /* iterator            */
    CPSS_PORTS_BMP_STC  initPortBmp, *initPortBmpPtr; /* port
                                            to configure resolved interface */
    CPSS_DXCH_PORT_AP_STATUS_STC    apStatus;   /* current AP status of port where
                                                    AP resolved */
    CPSS_PORT_FLOW_CONTROL_ENT  fcState;
    CPSS_DXCH_PORT_FEC_MODE_ENT fecMode;
    GT_U32              apResolvedPortsBmp1;/* bitmap of ports on local core where
                                                AP resolved*/
    GT_PHYSICAL_PORT_NUM            portNum;    /* global number of port where
                                                    AP resolved */
    GT_BOOL                 apPortState; /* is port in use by AP processor */

#if ((defined _VXWORKS) && (defined CPU_78200)) || (defined LINUX)
    GT_U32  portCreateTime; /* port interface configuration in mili-seconds */
#endif

    initPortBmpPtr = &initPortBmp;
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortBmpPtr);

    rc = cpssDxChPortApResolvedPortsBmpGet(devNum, portGroupId,
                                           &apResolvedPortsBmp1);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssDxChPortApResolvedPortsBmpGet:rc=%d\n", rc);
        return;
    }
    AP_DBG_PRINT_MAC(("cpssDxChPortApResolvedPortsBmpGet:portGroupId=%d,apResolvedPortsBmp1=0x%x\n",
                      portGroupId, apResolvedPortsBmp1));

    for(i = 0; apResolvedPortsBmp1>>i; i++)
    {
        if((apResolvedPortsBmp1>>i) & 1)
        {
			hcdFoundCounter[portGroupId]++;
            portNum =
                PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,
                                                       portGroupId, i);
#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX)&& (!defined ASIC_SIMULATION_NO_HW))
            if((1 == doTimeMeasure) && (portNum == doTimeMeasurePort))
            {
                mvSysTimerMeasureStart();
            }
#endif
            rc = cpssDxChPortApPortStatusGet(devNum, portNum, &apStatus);
            if(rc != GT_OK)
            {
                cpssOsPrintSync("cpssDxChPortApPortStatusGet:rc=%d\n", rc);
                return;
            }

            if(apStatus.hcdFound)
            {
                rc = cpssDxChPortApLock(devNum, portNum, &apPortState);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortApPortStatusGet:rc=%d\n", rc);
                    return;
                }
    
                if(GT_FALSE == apPortState)
                {/* HOST failed to lock port, i.e. it's in use by AP processor */
                    continue;
                }

				/* to overcome the buffer counter stuck */
                rc = cpssDxChPortEnableSet(devNum,portNum,GT_FALSE);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortEnableSet:rc=%d,portNum=%d,FALSE\n",
                                    rc, portNum);
                    (GT_VOID)apRestartOnPort(devNum, portNum);
                    return;
                }

                CPSS_PORTS_BMP_PORT_SET_MAC(initPortBmpPtr,portNum);
                AP_DBG_PRINT_MAC(("cpssDxChPortModeSpeedSet:portNum=%d\n", portNum));
				if(dumpToBuffer)
				{
				    cpssOsSprintf(str, "cpssDxChPortModeSpeedSet:portNum=%d\n", portNum);
				    addRecord(dumpArrayPtr, str);
				}
				
                rc = cpssDxChPortModeSpeedSet(devNum, initPortBmp,
                                              GT_TRUE,
                                              apStatus.portMode.ifMode,
                                              apStatus.portMode.speed);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortModeSpeedSet:rc=%d,portNum=%d,ifMode=%d,speed=%d\n",
                                    rc, portNum, apStatus.portMode.ifMode,
                                    apStatus.portMode.speed);
                    (GT_VOID)apRestartOnPort(devNum, portNum);
                    return;
                }

                if(apStatus.fcRxPauseEn && apStatus.fcTxPauseEn)
                {
                    fcState = CPSS_PORT_FLOW_CONTROL_RX_TX_E;
                }
                else if(apStatus.fcRxPauseEn)
                {
                    fcState = CPSS_PORT_FLOW_CONTROL_RX_ONLY_E;
                }
                else if(apStatus.fcTxPauseEn)
                {
                    fcState = CPSS_PORT_FLOW_CONTROL_TX_ONLY_E;
                }
                else
                {
                    fcState = CPSS_PORT_FLOW_CONTROL_DISABLE_E;
                }

                rc = cpssDxChPortFlowControlEnableSet(devNum,portNum,fcState);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortFlowControlEnableSet:rc=%d\n", rc);
                    (GT_VOID)apRestartOnPort(devNum, portNum);
                    return;
                }

                fecMode = (GT_TRUE == apStatus.fecEnabled) ?
                            CPSS_DXCH_PORT_FEC_MODE_ENABLED_E :
                                CPSS_DXCH_PORT_FEC_MODE_DISABLED_E;
                rc = cpssDxChPortFecModeSet(devNum,portNum,fecMode);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortFecModeSet:rc=%d\n", rc);
                    (GT_VOID)apRestartOnPort(devNum, portNum);
                    return;
                }

                rc = cpssDxChPortApSetActiveMode(devNum, portNum,
                                                 apStatus.portMode.ifMode,
                                                 apStatus.portMode.speed);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortApSetActiveMode:rc=%d\n", rc);
                    (GT_VOID)apRestartOnPort(devNum, portNum);
                    return;
                }

                /* do not perform auto-tune on GE ports */
                if((apStatus.portMode.speed > CPSS_PORT_SPEED_1000_E)
                    && (apStatus.portMode.speed != CPSS_PORT_SPEED_2500_E)
                    && (apStatus.portMode.ifMode != CPSS_PORT_INTERFACE_MODE_XGMII_E))
                {
                    CPSS_PORTS_BMP_PORT_SET_MAC(trxTrainBmpPtr,portNum);
                }
                else
                {
                    rc = cpssDxChPortApUnLock(devNum, portNum);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortApUnLock:portNum=%d,rc=%d\n", 
                                                    portNum, rc);
                        return;
                    }

                    rc = cpssDxChPortEnableSet(devNum,portNum,GT_TRUE);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortEnableSet:rc=%d,portNum=%d,TRUE_1\n",
                                        rc, portNum);
                        return;
                    }
                }

                CPSS_PORTS_BMP_PORT_CLEAR_MAC(initPortBmpPtr,portNum);
#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX) && (!defined ASIC_SIMULATION_NO_HW))
                if((1 == doTimeMeasure) && (portNum == doTimeMeasurePort))
                {
                    mvSysTimerMeasureGet(&portCreateTime);
                    cpssOsPrintf("port=%d:portCreateTime=%d(miliSec)\n", portNum,
                                    portCreateTime);
                    mvSysTimerMeasureStart(); /* restart timer */
                }
#endif
            }
        }
    }

    return;
}

/*******************************************************************************
* lion2AppDemoApTrxTrainingStart
*
* DESCRIPTION:
*       Start TRX training on defined ports
*
* INPUTS:
*       devNum - device number
*       portNum - global number of first port in core where AP resolved on any port
*       trxTrainBmpPtr - (pointer to) bitmap of ports where need to run TRX
*                           training
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
static GT_VOID lion2AppDemoApTrxTrainingStart
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum,
    IN  CPSS_PORTS_BMP_STC      *trxTrainBmpPtr
)
{
    GT_STATUS           rc;                /* return code         */
    GT_U32              i;                 /* iterator            */

    AP_DBG_PRINT_MAC(("cpssDxChPortSerdesAutoTune-portNum=%d\n", portNum));

    for(i = portNum; i < portNum+12; i++)
    {
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(trxTrainBmpPtr, i))
        {
            AP_DBG_PRINT_MAC(("cpssDxChPortSerdesAutoTune(TRX):portNum=%d\n", i));
            rc = cpssDxChPortSerdesAutoTune(devNum, i,
                                            CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_CFG_E);
            if(rc != GT_OK)
            {
                 CPSS_PORTS_BMP_PORT_CLEAR_MAC(trxTrainBmpPtr, i);
                cpssOsPrintSync("cpssDxChPortSerdesAutoTune(TRX_CFG,portNum=%d):rc=%d\n", i, rc);
                (GT_VOID)apRestartOnPort(devNum, i);
                continue;
            }
        }
    }

    for(i = portNum; i < portNum+12; i++)
    {
        if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(trxTrainBmpPtr, i))
        {
            rc = cpssDxChPortSerdesAutoTune(devNum, i,
                                            CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_START_E);
            if(rc != GT_OK)
            {
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(trxTrainBmpPtr, i);
                cpssOsPrintSync("cpssDxChPortSerdesAutoTune(TRX_START,portNum=%d):rc=%d\n", i, rc);
                (GT_VOID)apRestartOnPort(devNum, i);
                continue;
            }
        }
    }

    return;
}

/* for debug - enable/disable RX optimization */
GT_BOOL rxOptEn = GT_FALSE;
GT_U32 rxOptEnSet
(
    GT_U32 rxOptNew
)
{
    GT_U32 rxOptOld;

    rxOptOld = rxOptEn ? 1 : 0;
    rxOptEn = (1 == rxOptNew) ? 1 : 0;

    return rxOptOld;
}

/* for debug - enable/disable align90 optimization */
GT_BOOL align90En = GT_TRUE;
GT_U32 align90EnSet
(
    GT_U32 align90New
)
{
    GT_U32 align90Old;

    align90Old = align90En ? 1 : 0;
    align90En = (1 == align90New) ? 1 : 0;

    return align90Old;
}

GT_VOID debugDump()
{
    dumpArray(dumpArrayPtr);
    return;
}

/*******************************************************************************
* lion2AppDemoApTrxTrainingStatusCheck
*
* DESCRIPTION:
*       Check if there is still on core port with not complited TRX training
*
* INPUTS:
*       devNum - device number
*       portNum - global number of first port in core where AP resolved on any port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE  - TRX training not complited of some port of core
*       GT_FALSE - no more ports running TRX training
*
* COMMENTS:
*       None.
*
*******************************************************************************/
#define APPDEMO_AP_TRX_TRAINING_THRESHOLD_CNS 12
/* minimum time between training status polls in msec */
#define APPDEMO_AP_TRX_TRAINING_POLLING_PERIOD_CNS 16
static GT_BOOL lion2AppDemoApTrxTrainingStatusCheck
(
    IN      GT_U8                   devNum,
    IN      GT_PHYSICAL_PORT_NUM    portNum,
    INOUT   CPSS_PORTS_BMP_STC      *trxTrainBmpPtr,
    INOUT   GT_U32                  *trxTimeoutArray,
    OUT     CPSS_PORTS_BMP_STC      *newAlign90BmpPtr
)
{
    GT_STATUS   rc;           /* return code         */
    GT_U32      i;            /* iterator            */
    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT rxTuneStatus;
    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT txTuneStatus;
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode;
    CPSS_PORT_SPEED_ENT             speed;
#if ((defined _VXWORKS) && (defined CPU_78200)) || (defined LINUX)
    GT_U32                          trxTrainingTime;
#endif 

    for(i = portNum; i < portNum+12; i++)
    {
        if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(trxTrainBmpPtr, i))
        {
            continue;
        }

        rc = cpssDxChPortInterfaceModeGet(devNum,i,&ifMode);
        if((rc != GT_OK) || (CPSS_PORT_INTERFACE_MODE_NA_E == ifMode))
        {
            CPSS_PORTS_BMP_PORT_CLEAR_MAC(trxTrainBmpPtr, i);
            trxTimeoutArray[i-portNum] = 0;
            cpssOsPrintSync("cpssDxChPortInterfaceModeGet(portNum=%d):rc=%d\n", i, rc);
            (GT_VOID)apRestartOnPort(devNum, i);
            continue;
        }

        rc = cpssDxChPortSpeedGet(devNum,i,&speed);
        if((rc != GT_OK) || (CPSS_PORT_SPEED_NA_E == speed))
        {
            CPSS_PORTS_BMP_PORT_CLEAR_MAC(trxTrainBmpPtr, i);
            trxTimeoutArray[i-portNum] = 0;
            cpssOsPrintSync("cpssDxChPortSpeedGet(portNum=%d):rc=%d\n", i, rc);
            (GT_VOID)apRestartOnPort(devNum, i);
            continue;
        }

        if(trxTimeoutArray[i-portNum] < APPDEMO_AP_TRX_TRAINING_THRESHOLD_CNS)
        {/* no sense to check status of TRX training on port before pass 
            12*16(appDemo system tick) = 192 mSec from start, it even may 
            harm finish training */
            trxTimeoutArray[i-portNum]++;
            continue;
        }

        rc = cpssDxChPortSerdesAutoTuneStatusGet(devNum,i,&rxTuneStatus,&txTuneStatus);
        if(rc != GT_OK)
        {
            CPSS_PORTS_BMP_PORT_CLEAR_MAC(trxTrainBmpPtr, i);
            cpssOsPrintSync("cpssDxChPortSerdesAutoTuneStatusGet(portNum=%d):rc=%d\n", i, rc);
            (GT_VOID)apRestartOnPort(devNum, i);
            continue;
        }

        switch(txTuneStatus)
        {
            case   CPSS_DXCH_PORT_SERDES_AUTO_TUNE_PASS_E:
            case   CPSS_DXCH_PORT_SERDES_AUTO_TUNE_FAIL_E:
                AP_DBG_PRINT_MAC(("AutoTuneStatus-%d:portNum=%d\n", txTuneStatus, i));
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(trxTrainBmpPtr, i);
                trxTimeoutArray[i-portNum] = 0;
                rc = cpssDxChPortSerdesAutoTune(devNum, i,
                                                CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TRAINING_STOP_E);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortSerdesAutoTune(TRX_STOP,portNum=%d,txTuneStatus=%d):rc=%d\n",
                                    i, txTuneStatus, rc);
                    (GT_VOID)apRestartOnPort(devNum, i);
                    continue;
                }

#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX)&& (!defined ASIC_SIMULATION_NO_HW))
        if((1 == doTimeMeasure) && (i == doTimeMeasurePort))
        {
            mvSysTimerMeasureGet(&trxTrainingTime);
            cpssOsPrintf("port=%d:trxTrainingTime=%d(miliSec),status=%d\n", i,
                            trxTrainingTime,txTuneStatus);
            mvSysTimerMeasureStart(); /* for align90 */
        }
#endif
                if(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_PASS_E == txTuneStatus)
                {
					if(dumpToBuffer)
					{
					    cpssOsSprintf(str, "AutoTune:%d OK\n", i);
					    addRecord(dumpArrayPtr, str);
					}
				
                    if(align90En)
                    {
                        rc = cpssDxChPortForceLinkPassEnableSet(devNum, i, GT_TRUE);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("cpssDxChPortForceLinkPassEnableSet(devNum=%d,portNum=%d,GT_TRUE):rc=%d\n",
                                            devNum, i, rc);
                        }
    
                        rc = cpssDxChPortSerdesAlign90StateSet(devNum, i, GT_TRUE,
                            (CPSS_DXCH_PORT_ALIGN90_PARAMS_STC*)&(serdesParams[i]));
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("cpssDxChPortSerdesAlign90StateSet:rc=%d,portNum=%d\n",
                                            rc, i);
                            (GT_VOID)apRestartOnPort(devNum, i);
                            (GT_VOID)cpssDxChPortForceLinkPassEnableSet(devNum, i, GT_FALSE);
                        }
                        else
                        {
                            CPSS_PORTS_BMP_PORT_SET_MAC(newAlign90BmpPtr, i);
                        }
                    }
                    else
                    {
                        rc = cpssDxChPortApUnLock(devNum, i);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("cpssDxChPortApUnLock:portNum=%d,rc=%d\n",
                                            i, rc);
                        }
    
                        /* enable port to allow traffic pass */
                        rc = cpssDxChPortEnableSet(devNum, i, GT_TRUE);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintSync("ApTrxTrainingStatus:cpssDxChPortEnableSet:rc=%d,portNum=%d,GT_TRUE\n",
                                            rc, i);
                        }
                    }
                }
                else
                {/* if training failed on port reset mac and pcs to cause AP restart */
                    if(dumpToBuffer)
                    {
                        cpssOsSprintf(str, "AutoTune:%d failed\n", i);
                        addRecord(dumpArrayPtr, str);
                    }
				
                    rc = cpssDxChPortApUnLock(devNum, i);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortApUnLock:portNum=%d,rc=%d\n",
                                        i, rc);
                    }

                    /* enable port to allow traffic pass */
                    rc = cpssDxChPortEnableSet(devNum, i, GT_TRUE);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("ApTrxTrainingStatus:cpssDxChPortEnableSet:rc=%d,portNum=%d,GT_TRUE\n",
                                        rc, i);
                    }
                    (GT_VOID)apRestartOnPort(devNum, i);
                }

                break;

            case   CPSS_DXCH_PORT_SERDES_AUTO_TUNE_NOT_COMPLITED_E:
                if((trxTimeoutArray[i-portNum]+1) > 32)
                {
                    AP_DBG_PRINT_MAC(("AutoTuneStatus-TIMEOUT:portNum=%d\n", i));
                    CPSS_PORTS_BMP_PORT_CLEAR_MAC(trxTrainBmpPtr, i);
                    trxTimeoutArray[i-portNum] = 0;
                    rc = cpssDxChPortSerdesAutoTune(devNum, i,
                                                    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TRAINING_STOP_E);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortSerdesAutoTune(TRX_STOP on timeout,portNum=%d):rc=%d\n", i, rc);
                    }

#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX)&& (!defined ASIC_SIMULATION_NO_HW))
        if((1 == doTimeMeasure) && (i == doTimeMeasurePort))
        {
            mvSysTimerMeasureGet(&trxTrainingTime);
            cpssOsPrintf("port=%d:trxTrainingTime=%d(miliSec)-timedout\n", i,
                            trxTrainingTime);
        }
#endif
                    /* if training timed-out on port - reset mac and pcs to cause AP restart */
					if(dumpToBuffer)
					{
					    cpssOsSprintf(str, "AutoTune timed-out:%d\n", i);
					    addRecord(dumpArrayPtr, str);
					}
                    (GT_VOID)apRestartOnPort(devNum, i);
                }
                else
                {
                    trxTimeoutArray[i-portNum]++;
                    AP_DBG_PRINT_MAC(("AutoTuneStatus-NOT_COMPLETED:portNum=%d\n", i));
                }

                break;

            default:
                cpssOsPrintSync("AutoTuneStatus-bad state:portNum=%d,txTuneStatus=%d\n",
                                  i, txTuneStatus);
                break;
        }
    }

    if(GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(trxTrainBmpPtr))
    {
        return GT_TRUE;
    }
    else
    {
        return GT_FALSE;
    }
}

/*******************************************************************************
* lion2AppDemoApAlign90StatusCheck
*
* DESCRIPTION:
*       Check if there is still on core port with not complited TRX training
*
* INPUTS:
*       devNum - device number
*       portNum - global number of first port in core where AP resolved on any port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE  - TRX training not complited of some port of core
*       GT_FALSE - no more ports running TRX training
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_BOOL lion2AppDemoApAlign90StatusCheck
(
    IN      GT_U8                   devNum,
    IN      GT_PHYSICAL_PORT_NUM    firstPortInCoreGlobNum,
    INOUT   CPSS_PORTS_BMP_STC      *align90BmpPtr,
    INOUT   GT_U32                  *timeoutArray
)
{
    GT_STATUS   rc;           /* return code         */
    GT_U32      i;            /* iterator            */
    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT align90Status;
#if ((defined _VXWORKS) && (defined CPU_78200)) || (defined LINUX)
    GT_U32                          trxTrainingTime;
#endif 
    GT_BOOL                 stopAlign90;

    for(i = firstPortInCoreGlobNum; i < firstPortInCoreGlobNum+12; i++)
    {
        if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(align90BmpPtr, i))
        {
            continue;
        }

        rc = cpssDxChPortSerdesAlign90StatusGet(devNum, i, &align90Status);
        if(rc != GT_OK)
        {/* if error in execution */
            cpssOsPrintSync("cpssDxChPortSerdesAlign90StatusGet(portNum=%d):rc=%d\n", i, rc);
            stopAlign90 = GT_TRUE;
        }
        else if(((timeoutArray[i-firstPortInCoreGlobNum]+1) > 7) && 
            (CPSS_DXCH_PORT_SERDES_AUTO_TUNE_NOT_COMPLITED_E == align90Status))
        {/* if timeout*/
            stopAlign90 = GT_TRUE;
            AP_DBG_PRINT_MAC(("Align90-timeout:portNum=%d\n", i));
			if(dumpToBuffer)
			{
			    cpssOsSprintf(str, "Align90:%d timedout\n", i);
			    addRecord(dumpArrayPtr, str);
			}
        }
        else if(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_FAIL_E == align90Status)
        {/* if timeout*/
            stopAlign90 = GT_TRUE;
            AP_DBG_PRINT_MAC(("Align90-fail:portNum=%d\n", i));
			if(dumpToBuffer)
			{
			    cpssOsSprintf(str, "Align90:%d failed\n", i);
			    addRecord(dumpArrayPtr, str);
			}
        }
        else if(CPSS_DXCH_PORT_SERDES_AUTO_TUNE_PASS_E == align90Status)
        {
            stopAlign90 = GT_TRUE;
            AP_DBG_PRINT_MAC(("Align90-OK:portNum=%d\n", i));
			if(dumpToBuffer)
			{
			    cpssOsSprintf(str, "Align90:%d OK\n", i);
			    addRecord(dumpArrayPtr, str);
			}
        }
        else
        {
            timeoutArray[i-firstPortInCoreGlobNum]++;
            AP_DBG_PRINT_MAC(("Align90-NOT_COMPLETED:portNum=%d\n", i));
            stopAlign90 = GT_FALSE;
        }

        if(stopAlign90)
        {
            rc = cpssDxChPortSerdesAlign90StateSet(devNum, i, GT_FALSE,
                        (CPSS_DXCH_PORT_ALIGN90_PARAMS_STC*)&(serdesParams[i]));
            if(rc != GT_OK)
            {
                cpssOsPrintSync("cpssDxChPortSerdesAlign90StateSet:portNum=%d,FALSE,rc=%d\n",
                                i, rc);
            }
            CPSS_PORTS_BMP_PORT_CLEAR_MAC(align90BmpPtr, i);
            timeoutArray[i-firstPortInCoreGlobNum] = 0;

#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX)&& (!defined ASIC_SIMULATION_NO_HW))
        if((1 == doTimeMeasure) && (i == doTimeMeasurePort))
        {
            mvSysTimerMeasureGet(&trxTrainingTime);
            cpssOsPrintf("port=%d:align90Time=%d(miliSec)\n", i, trxTrainingTime);
        }
#endif
            if(align90Status != CPSS_DXCH_PORT_SERDES_AUTO_TUNE_PASS_E)
            { /* if align90 failed on port - reset mac and pcs to cause AP restart */
                rc = cpssDxChPortForceLinkPassEnableSet(devNum, i, GT_FALSE);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortForceLinkPassEnableSet(devNum=%d,portNum=%d,GT_FALSE):rc=%d\n",
                                    devNum, i, rc);
                }     
      
                (GT_VOID)apRestartOnPort(devNum, i);
            }
            else
            {
                if(rxOptEn)
                {/* run RX optimization algorithm */
                    CPSS_PORTS_BMP_PORT_SET_MAC(&todoRxTuneBmp, i);
                    rc = cpssOsSigSemSignal(apRxSmid);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("lion2AppDemoApAlign90StatusCheck:cpssOsSigSemSignal:rc=%d\n", rc);
                    }
                }
                else
                {   
                    rc = cpssDxChPortForceLinkPassEnableSet(devNum, i, GT_FALSE);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortForceLinkPassEnableSet(devNum=%d,portNum=%d,GT_FALSE):rc=%d\n",
                                        devNum, i, rc);
                    }     

                    rc = cpssDxChPortApUnLock(devNum, i);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("cpssDxChPortApUnLock:portNum=%d,rc=%d\n",
                                        i, rc);
                    }

                    /* enable port to allow traffic pass */
                    rc = cpssDxChPortEnableSet(devNum,i,GT_TRUE);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintSync("trxTask_1:cpssDxChPortEnableSet:rc=%d,portNum=%d,GT_TRUE\n",
                                        rc, i);
                    }
                }            
            }
        }
    }

    if(GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(align90BmpPtr))
    {
        return GT_TRUE;
    }
    else
    {
        return GT_FALSE;
    }
}

#include <private/mvHwsPortMiscIf.h>
static unsigned __TASKCONV lion2AppDemoApRxTrainingTask
(
    GT_VOID * param
)
{
    GT_PHYSICAL_PORT_NUM       portNum;    /* Port number */
    GT_STATUS   rc;         /* Return status */
    GT_U8       devNum;     /* Device number */
    GT_UINTPTR  tmp;        /* temporary variable */
    GT_BOOL     isLinkUp;   /* current link status */
    GT_U32      timeoutCounter;/* wait for link up after TRX training */

    /* to avoid warning: cast from pointer to integer of different size */
    tmp = (GT_UINTPTR)param;
    devNum = (GT_U8)tmp;

    while(1)
    {
        cpssOsSigSemWait(apRxSmid, 0);
    
        for(portNum = 0; portNum < CPSS_MAX_PORTS_NUM_CNS; portNum++)
        {
            PRV_CPSS_DXCH_SKIP_NOT_EXIST_PORT_MAC(devNum, portNum);
    
            if(CPSS_CPU_PORT_NUM_CNS == portNum)
            {
                continue;
            }
    
            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&todoRxTuneBmp, portNum))
            {
                continue;
            }
    
            for (timeoutCounter = 3; timeoutCounter > 0; timeoutCounter--)
            {
                rc = cpssDxChPortLinkStatusGet(devNum, portNum, &isLinkUp);
                if(rc != GT_OK)
                {
                    return rc;
                }
                if(isLinkUp != GT_TRUE)
                {
                    cpssOsTimerWkAfter(20);
                }
                else
                {
                    break;
                }
            }
    
            if(isLinkUp)
            {
                (GT_VOID)mvHwsPortSetOptAlgoParams(devNum, 
                                                    CPSS_PORT_GROUP_UNAWARE_MODE_CNS, 
                                                    -3, -5, 5);
                rc = cpssDxChPortSerdesAutoTuneOptAlgRun(devNum, portNum,
                                                         CPSS_PORT_SERDES_TRAINING_OPTIMISATION_FFE_E |
                                                         CPSS_PORT_SERDES_TRAINING_OPTIMISATION_ALIGN90_E);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortSerdesAutoTuneOptAlgRun(devNum=%d,portNum=%d,0x6):rc=%d\n",
                                    devNum, portNum, rc);
                }
        
                rc = cpssDxChPortForceLinkPassEnableSet(devNum, portNum, GT_FALSE);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortForceLinkPassEnableSet(devNum=%d,portNum=%d,GT_FALSE):rc=%d\n",
                                    devNum, portNum, rc);
                }
        
                CPSS_PORTS_BMP_PORT_CLEAR_MAC(&todoRxTuneBmp, portNum);
            }
    
            rc = cpssDxChPortApUnLock(devNum, portNum);
            if(rc != GT_OK)
            {
                cpssOsPrintSync("cpssDxChPortApUnLock:portNum=%d,rc=%d\n",
                                portNum, rc);
            }
    
            /* enable port to allow traffic pass */
            rc = cpssDxChPortEnableSet(devNum, portNum, GT_TRUE);
            if(rc != GT_OK)
            {
                cpssOsPrintSync("rxOptTask:cpssDxChPortEnableSet:rc=%d,portNum=%d,GT_TRUE\n",
                                rc, portNum);
            }

        } /* for(portNum = 0; */

    } /* while(1)*/

#ifdef __GNUC__
    /* to avoid warnings in gcc */
    return 0;
#endif

}

/*******************************************************************************
* lion2AppDemoApHndlTask
*
* DESCRIPTION:
*       This routine is the event handler for 802.3ap (Z80) event
*
* INPUTS:
*       param - The process data structure.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*    This function is part of the AP appDemo code that can be used as reference code.
*    This code was only tested on a single-device system. 
*    Code changes may be required for a multi-device system (controlled by one CPU).
* 
*******************************************************************************/

extern GT_U32 intCounterPrint
(
    GT_U32  portGroupId
)
{
    return intCounter[portGroupId];
}

extern GT_U32 hcdFoundCounterPrint
(
    GT_U32  portGroupId
)
{
    return hcdFoundCounter[portGroupId];
}

#define PRV_APPDEMO_LION2_PORTS_NUM_IN_CORE_CNS 12
static unsigned __TASKCONV lion2AppDemoApHndlTask
(
    GT_VOID * param
)
{
    GT_STATUS           rc;                /* return code         */
    GT_UINTPTR          evHndl;            /* event handler       */
    GT_U32              portGroupId;       /* core number  */
    GT_U8               devNum;            /* device number       */
    GT_PHYSICAL_PORT_NUM    firstPortInCoreGlobNum; /* global number of first port
                       on core where one of ports get AP resolution */
    CPSS_PORTS_BMP_STC sdTrainingBmp; /* bitmap of ports to run
                                           TRX training on */
    CPSS_PORTS_BMP_STC align90Bmp; /* bitmap of ports to run
                                           align90 on */
    CPSS_PORTS_BMP_STC newAlign90Bmp; /* bitmap of ports to run
                                           align90 on */
    CPSS_PORTS_BMP_STC newTrxTrainBmp; /* bitmap of ports to run
                                           TRX training on */
    APPDEMO_AP_TASK_PARAMS_STC apParam;     /* task parameters */
    GT_U32              extendedData;       /* event extended data */
    GT_U32  timeoutArray[PRV_APPDEMO_LION2_PORTS_NUM_IN_CORE_CNS];
    GT_U32  align90TimeoutArray[PRV_APPDEMO_LION2_PORTS_NUM_IN_CORE_CNS];

    apParam = *(APPDEMO_AP_TASK_PARAMS_STC*)param;
    evHndl = apParam.evntHndl;
    devNum = apParam.devNum;
    portGroupId = apParam.portGroup;

    /* calculate global number of first port in core */
    firstPortInCoreGlobNum = PRV_CPSS_LOCAL_PORT_TO_GLOBAL_PORT_CONVERT_MAC(devNum,
                                                       portGroupId, 0);

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&sdTrainingBmp);
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&align90Bmp);

    cpssOsMemSet(timeoutArray, 0, PRV_APPDEMO_LION2_PORTS_NUM_IN_CORE_CNS*
                                                                sizeof(GT_U32));
    cpssOsMemSet(align90TimeoutArray, 0, PRV_APPDEMO_LION2_PORTS_NUM_IN_CORE_CNS*
                                                                sizeof(GT_U32));
    while (1)
    {
        rc = cpssEventSelect(evHndl, NULL, NULL, 0);
        if(rc != GT_OK)
        {
            cpssOsPrintSync("lion2AppDemoApHndlTask:cpssEventSelect: err [%d]\n",
                            rc);
            return rc;
        }

        if ((rc = cpssEventRecv(evHndl, CPSS_PP_PORT_802_3_AP_E +
                                portGroupId, &extendedData, &devNum)) == GT_OK)
        {
            if(cpssDrvPpHwRegisterWrite(devNum, portGroupId, 0xF0, intCounter[portGroupId]) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterWrite(0xF0):data=%d failed\n", 
                                intCounter[portGroupId]);
            }

            if(dumpToBuffer)
            {
                cpssOsSprintf(str, "------------- intCounter[%d]=%d ----------------\n", 
                                portGroupId, intCounter[portGroupId]);
                addRecord(dumpArrayPtr, str);
            }

            intCounter[portGroupId]++;
            AP_DBG_PRINT_MAC(("cpssEventRecv:portGroupId=%d,firstPortInCoreGlobNum=%d\n",
                              portGroupId, firstPortInCoreGlobNum));

            do
            {
                CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&newTrxTrainBmp);
                lion2AppDemoApResolvedPortsConfig(devNum,portGroupId,&newTrxTrainBmp);                

                if(trxTrainingEn)
                {
                    if(GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(&newTrxTrainBmp))
                    {
                        lion2AppDemoApTrxTrainingStart(devNum, firstPortInCoreGlobNum,
                                                       &newTrxTrainBmp);
                        CPSS_PORTS_BMP_BITWISE_OR_MAC(&sdTrainingBmp, &sdTrainingBmp,
                                                      &newTrxTrainBmp);
                    }

                    if(GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(&sdTrainingBmp))
                    {
                        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&newAlign90Bmp);
                        /* we must check TRX training and stop it if complited/failed/timed-out */
                        lion2AppDemoApTrxTrainingStatusCheck(devNum,
                                                             firstPortInCoreGlobNum,
                                                             &sdTrainingBmp,
                                                             timeoutArray,
                                                             &newAlign90Bmp);
                        CPSS_PORTS_BMP_BITWISE_OR_MAC(&align90Bmp, &align90Bmp,
                                                      &newAlign90Bmp);
                    }

                    if(GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(&align90Bmp))
                    {
                        /* we must check Align90 status and stop it if complited/failed/timed-out */
                        lion2AppDemoApAlign90StatusCheck(devNum,
                                                         firstPortInCoreGlobNum,
                                                         &align90Bmp,
                                                         align90TimeoutArray);
                    }

                    if((GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(&sdTrainingBmp))
                        || (GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(&align90Bmp)))
                    {
                        cpssOsTimerWkAfter(APPDEMO_AP_TRX_TRAINING_POLLING_PERIOD_CNS);
                    }
                }

            }while((GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(&sdTrainingBmp))
                    || (GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(&align90Bmp)));
        }
        else
        {
            if(rc != GT_NO_MORE)
            {
                cpssOsPrintSync("lion2AppDemoApHndlTask:cpssEventRecv: err [%d]\n",
                                rc);
            }
        }
    } /* while(1) */

#ifdef __GNUC__
    /* to avoid warnings in gcc */
    return 0;
#endif

}

extern GT_STATUS mvApDebugTimeoutSet
(
 GT_U8   devNum,
 GT_U32  portGroup,
 GT_U32  timeout
);

static APPDEMO_AP_TASK_PARAMS_STC apTaskParamsArr[8];
/*******************************************************************************
* lion2AppDemoApInit
*
* DESCRIPTION:
*       Example of init for 802.3ap support.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*   This function is part of the AP appDemo code that can be used as reference code.
*   This code was only tested on a single-device system. 
*   Code changes may be required for a multi-device system (controlled by one CPU).
*
********************************************************************************/
GT_STATUS lion2AppDemoApInit
(
    IN  GT_U8   devNum
)
{
    GT_STATUS   rc;             /* return code */
    GT_32       intKey;         /* The interrupt lock key       */
    GT_CHAR     apTaskName[65]; /* string to build per core AP task */
    GT_TASK     apTaskId,       /* AP Task Id */
                rxTuneTaskId;   /* RX training task ID */
    GT_U32      portGroupId;    /* iterator over port groups */
    CPSS_UNI_EV_CAUSE_ENT evHndlrCauseAllArr[1];/* AP(Z80) port resolved interrupt */
    GT_U32      ticks;
    GT_U32      i;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&todoRxTuneBmp);
    if (cpssOsSigSemBinCreate("apRxSem", CPSS_OS_SEMB_EMPTY_E, &apRxSmid) != GT_OK)
    {
        return GT_NO_RESOURCE;
    }

    rc = cpssOsGetSysClockRate(&ticks);
    if(rc != GT_OK)
    {
        cpssOsPrintf("cpssOsGetSysClockRate failed\n");
    }
    else
    {
        cpssOsPrintf("cpssOsGetSysClockRate:ticks=%d\n", ticks);
    }

    dumpArrayPtr = (GT_CHAR **)cpssOsMalloc(_8K*sizeof(GT_CHAR *));
    if(NULL == dumpArrayPtr)
    {
        cpssOsPrintf("NULL == dumpArrayPtr\n");
        return GT_NO_RESOURCE;
    }
    rc = cpssOsSigSemBinCreate("dumpArraySem", 1, &dumpArraySem);
    if(rc != GT_OK)
    {
        cpssOsPrintf("cpssOsSigSemBinCreate:rc=%d\n", rc);
        return GT_NO_RESOURCE;        
    }
    for(i = 0; i < _8K; i++)
    {
        dumpArrayPtr[i] = NULL;
        dumpArrayPtr[i] = (GT_CHAR *)cpssOsMalloc(MAX_DUMP_STRING_LENGTH_CNS * 
                                                                sizeof(GT_CHAR));
        if(NULL == dumpArrayPtr[i])
        {
            cpssOsPrintf("NULL == dumpArrayPtr[%d]\n", i);
            return GT_NO_RESOURCE;
        }
        else
        {
            cpssOsMemSet(dumpArrayPtr[i], 0, MAX_DUMP_STRING_LENGTH_CNS * 
                                                            sizeof(GT_CHAR));
        }
    }
	
    for(portGroupId = 0; portGroupId < APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS;
         portGroupId++)
    {
        if(appDemoPpConfigList[devNum].portGroupsInfo[portGroupId].
                                                        portGroupActive)
        {
            rc = cpssDxChPortApEnableSet(devNum,(1<<portGroupId),GT_TRUE);
            if (GT_OK != rc)
            {
                return rc;
            }

            evHndlrCauseAllArr[0] = CPSS_PP_PORT_802_3_AP_E+portGroupId;

            /* lock section to disable interruption of ISR while unmasking events */
            INT_LOCK(intKey);

            rc = cpssEventBind(evHndlrCauseAllArr, 1,
                               &apTaskParamsArr[portGroupId].evntHndl);
            if (GT_OK != rc)
            {
                INT_UNLOCK(intKey);
                return rc;
            }

            /* call the CPSS to enable those interrupts in the HW of the device */
            rc = cpssEventDeviceMaskSet(devNum, CPSS_PP_PORT_802_3_AP_E
                                        +portGroupId, CPSS_EVENT_UNMASK_E);
            if (GT_OK != rc)
            {
                INT_UNLOCK(intKey);
                return rc;
            }

            INT_UNLOCK(intKey);

            apTaskParamsArr[portGroupId].devNum = devNum;
            apTaskParamsArr[portGroupId].portGroup = portGroupId;
            cpssOsSprintf(apTaskName, "apD%dGp%d", devNum, portGroupId);
            rc = cpssOsTaskCreate(apTaskName,               /* Task Name      */
                                  210,                        /* Task Priority  */
                                  _32K,                     /* Stack Size     */
                                  lion2AppDemoApHndlTask,   /* Starting Point */
                                  &apTaskParamsArr[portGroupId],/* Arguments list */
                                  &apTaskId);               /* task ID        */
            if (rc != GT_OK)
            {
                return rc;
            }
        }
    }

    rc = cpssOsTaskCreate("apRxTune",               /* Task Name      */
                          210,                        /* Task Priority  */
                          _32K,                     /* Stack Size     */
                          lion2AppDemoApRxTrainingTask,   /* Starting Point */
                          (GT_VOID*)((GT_UINTPTR)devNum),/* Arguments list */
                          &rxTuneTaskId);               /* task ID        */
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* lion2AppDemoAutoDetect
*
* DESCRIPTION:
*       Implementation of integrated AP and auto-detect mechanism.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*   
*
********************************************************************************/
static GT_U32   autoDetTrace = 1;/* option to enable/disable auto-detect trace 
                                    print in runtime*/
#define AUTODETCT_DBG_PRINT_MAC(x) if(autoDetTrace) cpssOsPrintSync x

extern GT_U32 autodetPrintControl(IN GT_U32  allowPrintNew)
{
    GT_U32  oldState = autoDetTrace;
    autoDetTrace = allowPrintNew;

    return oldState;
}

static GT_U32   autoDetTimeCheck = 1;/* option to enable/disable auto-detect execution 
                                        time check */
extern GT_U32 autodetTimingControl(IN GT_U32  timeCheckNew)
{
    GT_U32  oldState = autoDetTimeCheck;
    autoDetTimeCheck = timeCheckNew;

    return oldState;
}

GT_STATUS lion2AppDemoAutoDetect
(
    IN  GT_U8                   devNum,
    IN  GT_PHYSICAL_PORT_NUM    portNum
)
{
    GT_STATUS   rc; /* return code */
    GT_U32      caseNum;    /* random first case number */
    GT_U32      i, j;  /* iterator */
    GT_BOOL     linkStatus; /* linkUp status */
    /* parameters for AP test */
    CPSS_DXCH_PORT_AP_PARAMS_STC    apParams;
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode; /* port interface to configure */
    CPSS_PORT_SPEED_ENT             speed;  /* port speed to configure */
    GT_BOOL                         apEnabled;  /* AP initialized on device */
#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX) && (!defined ASIC_SIMULATION_NO_HW))
    GT_U32                          autoDetectTime; /* time of auto-detect in 
                                                        mili-seconds */
#endif 
    CPSS_PORTS_BMP_STC              initPortsBmp;/* bitmap of ports to init */
    CPSS_DXCH_PORT_AP_STATUS_STC    apStatus;   /* AP status of port */
    GT_BOOL                         portApEnable;
    CPSS_DXCH_PORT_AP_PARAMS_STC    portApParams;


    apParams.fcAsmDir = CPSS_DXCH_PORT_AP_FLOW_CONTROL_SYMMETRIC_E;
    apParams.fcPause = GT_TRUE;
    apParams.fecRequired = GT_FALSE;
    apParams.fecSupported = GT_TRUE;
    apParams.laneNum = 0;
    apParams.noneceDisable = GT_TRUE;
    apParams.modesAdvertiseArr[0].ifMode = CPSS_PORT_INTERFACE_MODE_1000BASE_X_E;
    apParams.modesAdvertiseArr[0].speed = CPSS_PORT_SPEED_1000_E;
    apParams.modesAdvertiseArr[1].ifMode = CPSS_PORT_INTERFACE_MODE_KR_E;
    apParams.modesAdvertiseArr[1].speed = CPSS_PORT_SPEED_10000_E;
    if (portNum%4 == 0)
    {
        apParams.modesAdvertiseArr[2].ifMode = CPSS_PORT_INTERFACE_MODE_XGMII_E; 
        apParams.modesAdvertiseArr[2].speed = CPSS_PORT_SPEED_10000_E;
        apParams.modesAdvertiseArr[3].ifMode = CPSS_PORT_INTERFACE_MODE_KR_E;
        apParams.modesAdvertiseArr[3].speed = CPSS_PORT_SPEED_40000_E;
        apParams.modesAdvertiseArr[4].ifMode = CPSS_PORT_INTERFACE_MODE_NA_E;
        apParams.modesAdvertiseArr[4].speed = CPSS_PORT_SPEED_NA_E;
    }
    else
    {
        apParams.modesAdvertiseArr[2].ifMode = CPSS_PORT_INTERFACE_MODE_NA_E;
        apParams.modesAdvertiseArr[2].speed = CPSS_PORT_SPEED_NA_E;
    }

    if (1 == autoDetTimeCheck)
    {
#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX) && (!defined ASIC_SIMULATION_NO_HW))
        mvSysTimerMeasureStart(); /* start timer */
#endif 
    }

    /* just for debug, to ensure tester didn't forget to init AP */
    rc = cpssDxChPortApEnableGet(devNum, 0, &apEnabled);
    if (rc != GT_OK)
    {
        cpssOsPrintf("cpssDxChPortApEnableGet(pGroup=%d):rc=%d\n", 0, rc);
        return rc;
    }

    AUTODETCT_DBG_PRINT_MAC(("cpssDxChPortApEnableGet pass\n"));
    if (GT_FALSE == apEnabled)
    {
        cpssOsPrintf("!!! You forget to enable AP, use appDemoDbEntryAdd apEnable!!!\n");
        return GT_FAIL;
    }

    rc = cpssDxChPortApPortConfigGet(devNum, portNum, &portApEnable, &portApParams);
    if(rc != GT_OK)
    {
        cpssOsPrintSync("cpssDxChPortApPortConfigGet:rc=%d\n", rc);
        return rc;
    }

    if (portApEnable)
    {
        AUTODETCT_DBG_PRINT_MAC(("AP was enabled on port-disable it:portNum=%d\n", portNum));
        rc = cpssDxChPortApPortConfigSet(devNum, portNum, GT_FALSE, NULL);
        if (rc != GT_OK)
        {
            cpssOsPrintf("cpssDxChPortApPortConfigSet(port=%d,GT_FALSE):rc=%d\n", 
                         portNum, rc);
            return rc;
        }
        cpssOsTimerWkAfter(50);
        AUTODETCT_DBG_PRINT_MAC(("AP disabled\n"));

        AUTODETCT_DBG_PRINT_MAC(("link down-power down:port=%d\n", portNum));
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&initPortsBmp);
        CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmp, portNum);

        rc = cpssDxChPortModeSpeedSet(devNum, initPortsBmp, GT_FALSE, 
                                      CPSS_PORT_INTERFACE_MODE_KR_E, 
                                      CPSS_PORT_SPEED_10000_E);
        if(rc != GT_OK)
        {
            cpssOsPrintf("cpssDxChPortModeSpeedSet(port=%d,FALSE,KR,10G):rc=%d\n", 
                         portNum, rc);
            return rc;
        }
        AUTODETCT_DBG_PRINT_MAC(("cpssDxChPortModeSpeedSet(port=%d,FALSE,KR,10G) pass\n", 
                                 portNum));
    }

    cpssOsSrand(cpssOsTime()); 
    AUTODETCT_DBG_PRINT_MAC(("cpssOsSrand(cpssOsTime()) pass\n"));

    for (i = 0, caseNum = cpssOsRand() % 3; i < 3; i++, caseNum=(caseNum+1)%3)
    {
        switch (caseNum)
        {
            case 0:
                AUTODETCT_DBG_PRINT_MAC(("case 0:AP,portNum=%d\n", portNum));
                rc = cpssDxChPortApPortConfigSet(devNum, portNum, GT_TRUE, 
                                                 &apParams);
                if (rc != GT_OK)
                {
                    cpssOsPrintf("cpssDxChPortApPortConfigSet(port=%d,GT_TRUE):rc=%d\n", 
                                 portNum, rc);
                    continue;
                }
                AUTODETCT_DBG_PRINT_MAC(("cpssDxChPortApPortConfigSet pass\n"));
                /* this time must be enough for our system to reach HCD found if
                   partner enabled AP */
                cpssOsTimerWkAfter(1000);

                rc = cpssDxChPortApPortStatusGet(devNum, portNum, &apStatus);
                if(rc != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortApPortStatusGet:rc=%d\n", rc);
                    return rc;
                }

                if(apStatus.hcdFound)
                {
                    AUTODETCT_DBG_PRINT_MAC(("!!! HCD found: ifMode=%d,speed=%d !!!\n", 
                                             apStatus.portMode.ifMode, apStatus.portMode.speed));
                    return GT_OK;
                }
                else
                {
                    AUTODETCT_DBG_PRINT_MAC(("Call cpssDxChPortApPortConfigSet(FALSE)\n"));
                    rc = cpssDxChPortApPortConfigSet(devNum, portNum, GT_FALSE, NULL);
                    if (rc != GT_OK)
                    {
                        cpssOsPrintf("cpssDxChPortApPortConfigSet(port=%d,GT_FALSE):rc=%d\n", 
                                     portNum, rc);
                        return rc;
                    }
                    cpssOsTimerWkAfter(50);
                    AUTODETCT_DBG_PRINT_MAC(("cpssDxChPortApPortConfigSet(FALSE) pass\n"));

                    continue;
                }

                break; 

            case 1: /* DXAUI */
                if (portNum%4 == 0)
                {
                    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&initPortsBmp);
                    CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmp, portNum);

                    AUTODETCT_DBG_PRINT_MAC(("case 1:DXAUI,portNum=%d\n", portNum));

                    ifMode = CPSS_PORT_INTERFACE_MODE_XGMII_E;
                    speed = CPSS_PORT_SPEED_20000_E;

                    rc = waTriggerInterruptsMask(devNum, portNum, ifMode, CPSS_EVENT_MASK_E);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }            
                    
                    rc = cpssDxChPortModeSpeedSet(devNum, initPortsBmp, GT_TRUE, ifMode, speed);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintf("cpssDxChPortModeSpeedSet(port=%d,DXAUI):rc=%d\n", 
                                     portNum, rc);
                        (GT_VOID)waTriggerInterruptsMask(devNum, portNum, ifMode, 
                                                         CPSS_EVENT_UNMASK_E);
                        continue;
                    }
                    else
                    {
                        AUTODETCT_DBG_PRINT_MAC(("cpssDxChPortModeSpeedSet(DXAUI) pass\n"));
                    }

                    rc = waTriggerInterruptsMask(devNum, portNum, ifMode, CPSS_EVENT_UNMASK_E);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }

                    /* This time must be enough to get link if partner already
                       configured to XGMII 20G or just starts configuration */
                    cpssOsTimerWkAfter(100);
                }
                else
                    continue;

                break;

            case 2:
                if (portNum%4 == 0)
                {
                    GT_BOOL             gbLock;     /* gear box lock */

                    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&initPortsBmp);
                    CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmp, portNum);

                    AUTODETCT_DBG_PRINT_MAC(("case 2:SR_LR_40G,portNum=%d\n", portNum));

                    ifMode = CPSS_PORT_INTERFACE_MODE_SR_LR_E; 
                    speed = CPSS_PORT_SPEED_40000_E;

                    rc = waTriggerInterruptsMask(devNum, portNum, ifMode, CPSS_EVENT_MASK_E);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }            

                    if (2 == i)
                    {/* This is WA for issue that if 40G SR_LR tested after AP case,
                        link doesn't up although it's right interface */
                        rc = cpssDxChPortModeSpeedSet(devNum, initPortsBmp, GT_TRUE, ifMode, speed);
                        if(rc != GT_OK)
                        {
                            cpssOsPrintf("cpssDxChPortModeSpeedSet#1(port=%d,40G_SR_LR):rc=%d\n", 
                                         portNum, rc);
                            goto exitCase;
                        }
                        AUTODETCT_DBG_PRINT_MAC(("cpssDxChPortModeSpeedSet(40G_SR_LR) pass #1\n"));
                    }
                    rc = cpssDxChPortModeSpeedSet(devNum, initPortsBmp, GT_TRUE, ifMode, speed);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintf("cpssDxChPortModeSpeedSet#2(port=%d,40G_SR_LR):rc=%d\n", 
                                     portNum, rc);
                        goto exitCase;
                    }
                    AUTODETCT_DBG_PRINT_MAC(("cpssDxChPortModeSpeedSet(40G_SR_LR) pass #2\n"));

                    /* Check Gear Box Status */
                    for(j = 0; j < 5; j++)
                    {/* ensure we get real gear box lock and not false indication */
                        rc = cpssDxChPortPcsGearBoxStatusGet(devNum, portNum, &gbLock);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                        if (!gbLock)
                        {
                            AUTODETCT_DBG_PRINT_MAC(("no Gear Box Lock\n"));
                            goto exitCase;
                        }
                        cpssOsTimerWkAfter(5);
                    }

                    rc = cpssDxChPortSerdesAutoTune(devNum, portNum,
                            CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_RX_TRAINING_E);
                    if(GT_FAIL == rc)
                    {
                        rc = cpssDxChPortSerdesAutoTune(devNum, portNum,
                                                CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_RX_TRAINING_E);
                        if(GT_FAIL == rc)
                        {
                            cpssOsPrintf("lion2AppDemoPortsConfig:cpssDxChPortSerdesAutoTune:RX training failed,portNum=%d:rc=%d", 
                                         portNum, rc);
                        }
                    }
                    else if((rc != GT_OK) && (rc != GT_FAIL))
                    {
                        cpssOsPrintf("cpssDxChPortSerdesAutoTune(RX_TRAINING,portNum=%d):rc=%d\n", portNum, rc);
                    }

                    rc = appDemoDxChLion2PortLinkWa(devNum, portNum);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintf("appDemoDxChLion2PortLinkWa(portNum=%d):rc=%d\n", portNum, rc);
                    }
exitCase:
                    rc = waTriggerInterruptsMask(devNum, portNum, ifMode, CPSS_EVENT_UNMASK_E);
                    if(rc != GT_OK)
                    {
                        return rc;
                    }
                }
                else
                    continue;

                break;

            default:
                return GT_BAD_STATE;
        }

        rc = cpssDxChPortLinkStatusGet(devNum, portNum, &linkStatus);
        if (rc != GT_OK)
        {
            cpssOsPrintf("cpssDxChPortLinkStatusGet(port=%d):rc=%d\n", 
                         portNum, rc);
        }
        AUTODETCT_DBG_PRINT_MAC(("cpssDxChPortLinkStatusGet pass\n"));

        if (GT_TRUE == linkStatus)
        {
            AUTODETCT_DBG_PRINT_MAC(("!!! link UP: ifMode=%d,speed=%d !!!\n", ifMode, speed));

            if (1 == autoDetTimeCheck)
            {
#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX) && (!defined ASIC_SIMULATION_NO_HW))
                mvSysTimerMeasureGet(&autoDetectTime);
                cpssOsPrintf("autoDetectTime=%d(miliSec)\n", autoDetectTime);
#endif
            }
            return GT_OK;
        }
        else
        {
            switch(caseNum)
            {
                case 1:
                case 2:
                    AUTODETCT_DBG_PRINT_MAC(("link down-power down:port=%d\n", portNum));
                    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&initPortsBmp);
                    CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmp, portNum);

                    rc = cpssDxChPortModeSpeedSet(devNum, initPortsBmp, GT_FALSE, ifMode, speed);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintf("cpssDxChPortModeSpeedSet(port=%d,FALSE,ifMode=%d):rc=%d\n", 
                                     portNum, ifMode, rc);
                        continue;
                    }
                    AUTODETCT_DBG_PRINT_MAC(("cpssDxChPortModeSpeedSet(port=%d,FALSE,ifMode=%d) pass\n", 
                                             portNum, ifMode));
                    break;

                default:
                    return GT_BAD_STATE;
            }
        }

    }/* for (i = 0,*/
    
    AUTODETCT_DBG_PRINT_MAC(("Failed to get link Up\n"));

    return GT_FAIL;
}

#endif /* ASIC_SIMULATION_NO_HW */
/*******************************************************************************
* gtDbDxLion2BoardCleanDbDuringSystemReset
*
* DESCRIPTION:
 *      clear the DB of the specific board config file , as part of the 'system rest'
 *      to allow the 'cpssInitSystem' to run again as if it is the first time it runs
*
* INPUTS:
*       boardRevId      - The board revision Id.
*
* OUTPUTS:
*
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS gtDbDxLion2BoardCleanDbDuringSystemReset
(
    IN  GT_U8   boardRevId
)
{
    /* next static are and maybe changed from compilation time */
    dxChInitRegDefaults = GT_FALSE;

    numOfActivePortGroups = 0;

    return GT_OK;
}



/*******************************************************************************
* gtDbDxLion296XgBoardReg
*
* DESCRIPTION:
*       Registration function for the Lion2 96XG ports board .
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
GT_STATUS gtDbDxLion296XgBoardReg
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
    boardCfgFuncs->boardCleanDbDuringSystemReset= gtDbDxLion2BoardCleanDbDuringSystemReset;

    switch(boardRevId)
    {
        case REV_ID_CALLER_CHOICE_E:                /*22*/
        case REV_ID_ENHANCED_AUTO_4_FDB_256K_E:     /*24*/
        case REV_ID_ENHANCED_AUTO_2_FDB_128K_E:     /*25*/
#ifndef EXMXPM_FAMILY
            /* support of multi port group fdb lookup */
            return gtDbDxLion2_ForMultiCoreFdbLookup(boardRevId,boardCfgFuncs);
#endif /*EXMXPM_FAMILY*/
        default:
            break;
    }

    return GT_OK;
}

/*******************************************************************************
**   Pizza Dump Print
**
*******************************************************************************/
static void PizzaSim_ProfilePriorityMatrPrint(CPSS_DXCH_PORT_PRIORITY_MATR_STC * pPortPriorityMatrPtr, char * msg)
{
    CPSS_DXCH_PIZZA_PORT_ID  * portPriorityListPtr;
    GT_U32 p;
    GT_U32 v;

    osPrintf("\n    %s",msg);
    osPrintf("\n        portNum  = %d",(int)(pPortPriorityMatrPtr->portN));
    for (p = 0 ; p < pPortPriorityMatrPtr->portN ; p++)
    {
        portPriorityListPtr = &pPortPriorityMatrPtr->portPriorityList[p][0];
        osPrintf("\n        port %2d :",(int)p);
        for (v = 0 ; portPriorityListPtr[v] != CPSS_PA_INVALID_PORT ; v ++ )
        {
            osPrintf(" %2d",portPriorityListPtr[v]);
        }
    }
}

static void PizzaSim_ProfilePizzaPrint(CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC * pPizzaCfgPtr, char * msg)
{
    GT_U32 p; /* port */
    GT_U32 slIdx;
    GT_U32 cfg;
    CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC          * pPizzaCfgSliceList;
    CPSS_DXCH_PIZZA_SLICE_ID              * pSliceList;

    osPrintf("\n    %s",msg);
    if (NULL == pPizzaCfgPtr)
    {
        osPrintf("\n        ------------");
        return;
    }
    osPrintf("\n        portNum  = %d",(int)pPizzaCfgPtr->portNum);
    osPrintf("\n        sliceNum = %d",(int)pPizzaCfgPtr->totalSlices);
    for (cfg = 0 ; pPizzaCfgPtr->pizzaCfg[cfg].sliceNum2Config != 0 ; cfg++)
    {
        osPrintf("\n        pizza %d slices",(int)pPizzaCfgPtr->pizzaCfg[cfg].sliceNum2Config);

        pPizzaCfgSliceList = pPizzaCfgPtr->pizzaCfg[cfg].pizzaPtr;
        for (p = 0 ; p < pPizzaCfgSliceList->maxPortNum; p++)
        {
            pSliceList = &pPizzaCfgSliceList->portConfArr[p][0];
            osPrintf("\n              %2d :",(int)p);
            for (slIdx = 0; pSliceList[slIdx] != CPSS_PA_INVALID_SLICE; slIdx++)
            {
                osPrintf(" %2d",pSliceList[slIdx]);
            }
        }
    }
}

static void PizzaSim_ProfilePrint(CPSS_DXCH_PIZZA_PROFILE_STC * pProfilePtr)
{
    osPrintf("\n    portNum  = %d",(int)pProfilePtr->portNum);
    osPrintf("\n    sliceNum = %d",(int)pProfilePtr->maxSliceNum);
    osPrintf("\n    txQ rep  = %d",(int)pProfilePtr->txQDef.pizzaRepetitionNum);
    PizzaSim_ProfilePriorityMatrPrint(pProfilePtr->pPortPriorityMatrPtr,"Port Dependency Matr");
    PizzaSim_ProfilePizzaPrint(pProfilePtr->regModePizzaPtr,"Regular Mode Pizza");
    PizzaSim_ProfilePizzaPrint(pProfilePtr->extModePizzaPtr,"Extende Mode Pizza");
}


typedef struct SpeedEnumStrEntry_STC
{
    CPSS_PORT_SPEED_ENT speed;
    char * str;
}SpeedEnumStrEntry_STC;

static SpeedEnumStrEntry_STC speed2StrTbl[CPSS_PORT_SPEED_NA_E] =
{
     { CPSS_PORT_SPEED_10_E,    "10M"    }
    ,{ CPSS_PORT_SPEED_100_E,   "100M"   }
    ,{ CPSS_PORT_SPEED_1000_E,  "1G"     }
    ,{ CPSS_PORT_SPEED_2500_E,  "2.5G"   }
    ,{ CPSS_PORT_SPEED_5000_E,  "5G"     }
    ,{ CPSS_PORT_SPEED_10000_E, "10G"    }
    ,{ CPSS_PORT_SPEED_12000_E, "12G"    }
    ,{ CPSS_PORT_SPEED_13600_E, "13.6G"  }
    ,{ CPSS_PORT_SPEED_16000_E, "16G"    }
    ,{ CPSS_PORT_SPEED_15000_E, "15G"    }
    ,{ CPSS_PORT_SPEED_20000_E, "20G"    }
    ,{ CPSS_PORT_SPEED_40000_E, "40G"    }
    ,{ CPSS_PORT_SPEED_75000_E, "75G"    }
    ,{ CPSS_PORT_SPEED_100G_E,  "100G"   }
};

static char * PizzaSim_SpeedStrGet(CPSS_PORT_SPEED_ENT  speed)
{
    GT_U32 i;
    static char str[10];
    for (i = 0 ; i < sizeof(speed2StrTbl)/sizeof(speed2StrTbl[0]); i++)
    {
        if (speed2StrTbl[i].speed == speed)
        {
            return speed2StrTbl[i].str;
        }
    }
    osSprintf(&str[0],"%d",(int)speed);
    return &str[0];
}

static void PizzaSim_Speed2SlaceConvTblPrint(CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC * table2UsePtr)
{
    GT_U32 portIdx;
    GT_U32 speedIdx;
    /*CPSS_PORT_SPEED_INVALIDE_E*/
    CPSS_DXCH_PORT_SPEED_TO_SLICENUM_LIST_ITEM_STC * pPortEntryPtr;
    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC   * pSpeedEntryPtr;
    char * pSpeedStr;


    pPortEntryPtr = &table2UsePtr->arr[0];
    for (portIdx = 0 ; pPortEntryPtr->portN != CPSS_PA_INVALID_PORT ; portIdx++, pPortEntryPtr++)
    {
        osPrintf("\n    Port %2d : ",(int)pPortEntryPtr->portN);
        pSpeedEntryPtr = pPortEntryPtr->portSpeed2SliceNumListPtr;
        for (speedIdx = 0 ; pSpeedEntryPtr->slicesNum != CPSS_INVALID_SLICE_NUM; speedIdx++,pSpeedEntryPtr++)
        {
            pSpeedStr = PizzaSim_SpeedStrGet(pSpeedEntryPtr->speed);
            osPrintf("\n        %-10s : %2d",pSpeedStr, (int)pSpeedEntryPtr->slicesNum);
        }
    }
}

/*******************************************************************************
* cpssDxChPortPizzaArbiterIfTablesDump
*
* DESCRIPTION:
*       Print Pizza Arbiter tables for given device portGroup
*          1.  Pizza profile
*          2.  Speed to slice conversion table
*
* APPLICABLE DEVICES:
*        None
*
* NOT APPLICABLE DEVICES:
*        None
*
*
* INPUTS:
*       devNum   - physical device number
*       portGroupId  - number of port group
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS cpssDxChPortPizzaArbiterIfTablesDump
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId
)
{
    GT_STATUS rc;
    CPSS_DXCH_PIZZA_PROFILE_STC * pPizzaProfilePtr;
    CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC * portGroupPortSpeed2SliceNumPtr;

    rc = cpssDxChPortPizzaArbiterIfUserTableGet(/*IN*/devNum,
                                                  /*IN*/portGroupId,
                                                  /*OUT*/&pPizzaProfilePtr,
                                                  /*OUT*/&portGroupPortSpeed2SliceNumPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    osPrintf("\ndevNum = %2d portGroupId = %2d",(int)devNum,(int)portGroupId);
    osPrintf("\nPizza Profile");
    PizzaSim_ProfilePrint(pPizzaProfilePtr);
    osPrintf("\nSpeed Conv Table");
    PizzaSim_Speed2SlaceConvTblPrint(portGroupPortSpeed2SliceNumPtr);
    osPrintf("\n");
    return GT_OK;
}

/*-------------------------------------------------------------------------------
**     Pizza Arbiter State Print Function :
**         prints configuration and state of all slice for each unit
**         <Rx/Tx DMA DMA_CTU  BM TxQ
**-------------------------------------------------------------------------------*/


#ifdef WIN32
    #define cpssOsPrintf osPrintf
#endif


static void gtLion2PortPizzaArbiterIfDumpSliceStatePrint(GT_BOOL isEnable, GT_U32 occupiedBy)
{
    if (GT_FALSE  != isEnable )
    {
        cpssOsPrintf(" %2d",occupiedBy);
    }
    else
    {
        cpssOsPrintf(" %2s","X");
    }
}

static void gtLion2PortPizzaArbiterIfDumpUnitStatePrint(CPSS_DXCH_UNIT32_SLICES_PIZZA_ARBITER_STATE_STC * pUnitPtr, char * msg)
{
    GT_U32      sliceId;
    GT_U32      slicesNumberPerGop;


    slicesNumberPerGop = sizeof(pUnitPtr->slice_enable)/sizeof(pUnitPtr->slice_enable[0]); /* pUnitPtr->totalSlices; */
    cpssOsPrintf("\n    %s",msg);
    cpssOsPrintf("\n       Total Slices       = %d",pUnitPtr->totalConfiguredSlices);
    cpssOsPrintf("\n       StrictPriority Bit = %d",pUnitPtr->strictPriorityBit);
    cpssOsPrintf("\n       Slice to port map : slice :");
    for (sliceId = 0 ; sliceId < slicesNumberPerGop ; sliceId++)
    {
        cpssOsPrintf(" %2d",sliceId);
    }
    cpssOsPrintf("\n       Slice to port map : port  :");
    for (sliceId = 0 ; sliceId < slicesNumberPerGop ; sliceId++)
    {
        gtLion2PortPizzaArbiterIfDumpSliceStatePrint(pUnitPtr->slice_enable[sliceId],
                                                                  pUnitPtr->slice_occupied_by[sliceId]);
    }
}



typedef struct gtPizzaArbiterCpuType2Str_STCT
{
    CPSS_DXCH_CPUPORT_TYPE_ENT cpuType;
    char *                     str;
}gtPizzaArbiterCpuType2Str_STC;


static char * gtPizzaArbiterCpuType2Str
(
    CPSS_DXCH_CPUPORT_TYPE_ENT cpuType
)
{
    static gtPizzaArbiterCpuType2Str_STC arr[] =
    {
         { CPSS_DXCH_CPUPORT_TYPE_UNDEFINED_E, "no cpu"   }
        ,{ CPSS_DXCH_CPUPORT_TYPE_FIXED_E,     "fixed"    }
        ,{ CPSS_DXCH_CPUPORT_TYPE_EXTERNAL_E,  "external" }
    };

    GT_U32 i;
    for (i = 0; i < sizeof(arr)/sizeof(arr[0]); i++)
    {
        if (cpuType == arr[i].cpuType)
        {
            return arr[i].str;
        }
    }
    return "unknown";
}

/*******************************************************************************
* gtLion2PortPizzaArbiterIfStateDump
*
* DESCRIPTION:
*       Dump Lion 2,3 Pizza Arbiter Registers
*
* APPLICABLE DEVICES:
*        Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portGroupId  - number of port group
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS gtLion2PortPizzaArbiterIfStateDump
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      sliceId;
    GT_U32      i;
    static CPSS_DXCH_DEV_PIZZA_ARBITER_STATE_STC  pizzaDeviceState;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    rc = cpssDxChPortPizzaArbiterDevStateInit(&pizzaDeviceState);
    if (rc != GT_OK)
    {
        return rc;
    }


    rc = cpssDxChPortPizzaArbiterDevStateGet(devNum,portGroupId, /*OUT*/&pizzaDeviceState);
    if (rc != GT_OK)
    {
        return rc;
    }


    cpssOsPrintf("\nPizza Arbiter State device = %2d port group = %2d", devNum,portGroupId);
    gtLion2PortPizzaArbiterIfDumpUnitStatePrint(&pizzaDeviceState.devState.lion2.rxDMA,    "RxDMA Pizza Arbiter:");
    gtLion2PortPizzaArbiterIfDumpUnitStatePrint(&pizzaDeviceState.devState.lion2.rxDMA_CTU,"RxDMA CTU Pizza Arbiter:");
    gtLion2PortPizzaArbiterIfDumpUnitStatePrint(&pizzaDeviceState.devState.lion2.txDMA,    "TxDMA Pizza Arbiter:");
    gtLion2PortPizzaArbiterIfDumpUnitStatePrint(&pizzaDeviceState.devState.lion2.txDMA_CTU,"TxDMA CTU Pizza Arbiter:");
    gtLion2PortPizzaArbiterIfDumpUnitStatePrint(&pizzaDeviceState.devState.lion2.BM,       "BM Pizza Arbiter:");

    cpssOsPrintf("\n    TxQ Pizza Arbiter:");
    cpssOsPrintf("\n       Total Slices       = %d",pizzaDeviceState.devState.lion2.TxQ.totalConfiguredSlices);
    cpssOsPrintf("\n       Slices in GOP      = %d",pizzaDeviceState.devState.lion2.TxQ.slicesNumInGop);
    cpssOsPrintf("\n       Slice to port map : slice :");

    for (sliceId = 0 ; sliceId < UNIT_TOTAL_SLICES ; sliceId++)
    {
        cpssOsPrintf(" %2d",sliceId);
    }

    for (sliceId = 0 ; sliceId < TxQ_TOTAL_AVAIBLE_SLICES_CNS ; sliceId++)
    {
        if (0 == sliceId % UNIT_TOTAL_SLICES)
        {
            cpssOsPrintf("\n       Slice %3d : %3d   : port  :",sliceId, sliceId + UNIT_TOTAL_SLICES -1 );
        }
        gtLion2PortPizzaArbiterIfDumpSliceStatePrint(pizzaDeviceState.devState.lion2.TxQ.slice_enable[sliceId],
                                                                      pizzaDeviceState.devState.lion2.TxQ.slice_occupied_by[sliceId]);
    }

    if (pizzaDeviceState.devState.lion2.TxQ.cpu_port_type != CPSS_DXCH_CPUPORT_TYPE_UNDEFINED_E)
    {
        cpssOsPrintf("\n       CPU type : %s",gtPizzaArbiterCpuType2Str(pizzaDeviceState.devState.lion2.TxQ.cpu_port_type));
        for (i = 0 ; pizzaDeviceState.devState.lion2.TxQ.cpu_port_sliceId[i] != CPSS_PA_INVALID_SLICE; i++)
        {
            cpssOsPrintf("\n       CPU port Slice %3d :",pizzaDeviceState.devState.lion2.TxQ.cpu_port_sliceId[i]);
            gtLion2PortPizzaArbiterIfDumpSliceStatePrint(pizzaDeviceState.devState.lion2.TxQ.cpu_port_slice_is_enable[i],
                                                             pizzaDeviceState.devState.lion2.TxQ.cpu_port_slice_occupied_by[i]);
        }
    }
    cpssOsPrintf("\n");
    return GT_OK;
}



/*******************************************************************************
* gtLion2PortPizzaArbiterIfRegistersDump
*
* DESCRIPTION:
*       Dump Lion 2,3 Pizza Arbiter Registers
*
* APPLICABLE DEVICES:
*        Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portGroupId  - number of port group
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/

GT_STATUS gtLion2PortPizzaArbiterIfRegistersDump
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regValue;   /* current value of register */
    GT_U32      regAddr;    /* register address */
    GT_U32      i;          /* iterator */
    GT_U32      cpuPortPortGroupId,
                txQSlicesNum;
    PRV_CPSS_DXCH_PORT_PIZZA_DEV_INFO_STC    pizzaDevInfo;
    CPSS_DXCH_PIZZA_PROFILE_STC       *  pizzaProfilePtr;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
                                            | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E
                                            | CPSS_LION_E | CPSS_XCAT2_E);

    /* number of slices in RX DMA pizza arbiter */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.rxdmaPZCtrlReg;
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("rxdmaPZCtrlReg:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                    devNum, portGroupId, regAddr, regValue);

    /* number of slices in TX DMA pizza arbiter */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.txdmaPZCtrlReg;
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
    if(rc != GT_OK)
    {
        return rc;
    }
    cpssOsPrintf("txdmaPZCtrlReg:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                    devNum, portGroupId, regAddr, regValue);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.
                                            portArbiter.pizzaArbiterConfig;
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
    if(rc != GT_OK)
    {
        return rc;
    }
    cpssOsPrintf("pizzaArbiterConfig:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                    devNum, portGroupId, regAddr, regValue);

    cpssOsPrintf("devNum=%d,cpuPortMode=%d,CPU_PORT_PORT_GROUP_ID=%d,SDMA_PORT_GROUP_ID=%d\n",
                  devNum, PRV_CPSS_PP_MAC(devNum)->cpuPortMode,
                  PRV_CPSS_CPU_PORT_PORT_GROUP_ID_MAC(devNum),
                  PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum));


    rc = prvCpssDxChPortPizzaArbiterIfDevInfoBuild(/*IN*/devNum,/*OUT*/&pizzaDevInfo);

    if (rc != GT_OK)
    {
        return rc;
    }

    /* get the pizza definition by device parameters */
    rc = cpssDxChPortPizzaArbiterIfUserTableGet(/*IN*/devNum,/*IN*/portGroupId,/*OUT*/&pizzaProfilePtr,NULL);
    if (GT_OK != rc )
    {
        return rc;
    }

    cpuPortPortGroupId = (PRV_CPSS_PP_MAC(devNum)->cpuPortMode !=
                            CPSS_NET_CPU_PORT_MODE_SDMA_E) ?
                                PRV_CPSS_CPU_PORT_PORT_GROUP_ID_MAC(devNum) :
                                    PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

    txQSlicesNum = pizzaProfilePtr->maxSliceNum;
    if(portGroupId == cpuPortPortGroupId)
    {/* to minimize tx queue time wasted on CPU port lets multiply number of
        slices on GOP where CPU connected, and add one slice for CPU;
        CPU port txq slice mapping already defined at init */
        txQSlicesNum = (txQSlicesNum*pizzaProfilePtr->txQDef.pizzaRepetitionNum)+1;
    }

    cpssOsPrintf("devType=0x%X,slicesNumberPerGop=%d,cpuPortPortGroupId=%d,txQSlicesNum=%d\n",
                 PRV_CPSS_PP_MAC(devNum)->devType, pizzaProfilePtr->maxSliceNum,
                 cpuPortPortGroupId, txQSlicesNum);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.
                                            portArbiter.pizzaArbiterMap[(txQSlicesNum-1)/6];
    rc = prvCpssHwPpPortGroupGetRegField(devNum, portGroupId, regAddr,
                                            ((txQSlicesNum-1)%6)*5, 5, &regValue);
    if(rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf("CPU port portGroupId=%d,pizzaArbiterMap[%d]:offset=%d,field=0x%X\n",
                    portGroupId, (txQSlicesNum-1)/6, ((txQSlicesNum-1)%6)*5, regValue);

    for(i = 0; i < 42; i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.
                                                portArbiter.pizzaArbiterMap[i];
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
        cpssOsPrintf("pizzaArbiterMap[%d]:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                        i, devNum, portGroupId, regAddr, regValue);
    }

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngPZGenConfiglReg;
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
    if(rc != GT_OK)
    {
        return rc;
    }
    cpssOsPrintf("bufMngPZGenConfiglReg:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                    devNum, portGroupId, regAddr, regValue);

    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngPZSlotEnableReg;
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
    if(rc != GT_OK)
    {
        return rc;
    }
    cpssOsPrintf("bufMngPZSlotEnableReg:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                    devNum, portGroupId, regAddr, regValue);

    for(i = 0; i < 8; i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.rxdmaPZSliceMapReg[i];
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
        cpssOsPrintf("rxdmaPZSliceMapReg[%d]:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                        i, devNum, portGroupId, regAddr, regValue);
    }

    for(i = 0; i < 8; i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.txdmaPZSliceMapReg[i];
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
        cpssOsPrintf("txdmaPZSliceMapReg[%d]:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                        i, devNum, portGroupId, regAddr, regValue);
    }

    for(i = 0; i < 4; i++)
    {
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngPZSlotConfigReg[i];
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
        cpssOsPrintf("bufMngPZSlotConfigReg[%d]:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                        i, devNum, portGroupId, regAddr, regValue);
    }

    return GT_OK;
}

/* Debug function for memory state reconstruction */
GT_STATUS prvLion2DiagDataIntegrityMislMemReconstruct
(
    IN GT_U8 devNum
)
{
    GT_STATUS rc; /* return code */
    GT_U32 regAddr;

    /* PIPE 4 */
    rc = prvCpssDfxMemoryPipeIdSet(devNum, 4);

    regAddr = 0x01808514;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808554;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808594;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x018085d4;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808614;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808654;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808694;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x018086d4;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808714;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808754;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808794;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x018087d4;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808814;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808854;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808894;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x018088d4;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808914;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808954;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808994;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x018089d4;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808a14;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808a54;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808a94;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808ad4;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808b14;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808b54;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808b94;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808bd4;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808c14;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808c54;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808c94;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808cd4;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    /* PIPE 5 */
    rc = prvCpssDfxMemoryPipeIdSet(devNum, 5);

    regAddr = 0x01800294;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x018002d4;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01800314;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01800354;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01800394;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x018003d4;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01800414;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01800454;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808294;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x018082d4;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808314;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808354;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808394;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x018083d4;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808414;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    regAddr = 0x01808454;
    rc |= prvCpssHwPpWriteRegister(devNum, regAddr, 0);

    if(rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

GT_STATUS appDemoDataIntegrityMemoryRegRead
(
    IN GT_U8 devNum,
    IN GT_U32 pipeId,
    IN GT_U32 clientId,
    IN GT_U32 memNumber,
    IN GT_U32 memReg
)
{
    GT_STATUS rc; /* return status */
    GT_U32 regData; /* register data */

    rc = prvCpssDfxMemoryRegRead(devNum, pipeId, clientId, memNumber, memReg, &regData);
    if(GT_OK != rc)
    {
        return rc;
    }

    cpssOsPrintf("regData = 0x%x\r\n", regData);

    return GT_OK;
}

GT_STATUS appDemoDataIntegrityMemoryRegWrite
(
    IN GT_U8 devNum,
    IN GT_U32 pipeId,
    IN GT_U32 clientId,
    IN GT_U32 memNumber,
    IN GT_U32 memReg,
    IN GT_U32 regData
)
{
    return prvCpssDfxMemoryRegWrite(devNum, pipeId, clientId, memNumber, memReg, regData);
}

GT_STATUS appDemoDataIntegrityClientRegRead
(
    IN GT_U8 devNum,
    IN GT_U32 pipeId,
    IN GT_U32 clientId,
    IN GT_U32 clientReg
)
{
    GT_STATUS rc; /* return status */
    GT_U32 regData; /* register data */

    rc = prvCpssDfxClientRegRead(devNum, pipeId, clientId, clientReg, &regData);
    if(GT_OK != rc)
    {
        return rc;
    }

    cpssOsPrintf("regData = 0x%x\r\n", regData);

    return GT_OK;
}

GT_STATUS trxTrainingOnePortTime
(
    GT_U8                   devNum,
    GT_PHYSICAL_PORT_NUM    portNum
)
{
#define TIMEOUT_MAX 500
    GT_STATUS   rc;
    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT rxTuneStatus;
    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_STATUS_ENT txTuneStatus;
#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX)&& (!defined ASIC_SIMULATION_NO_HW))	
    GT_U32      trxTrainingTime;
#endif	
    GT_U32      timeoutCounter;

    /* TRX training config */
    rc = cpssDxChPortSerdesAutoTune(devNum, portNum,
                                    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_CFG_E);
    if(rc != GT_OK)
    {
        cpssOsPrintf("cpssDxChPortSerdesAutoTuneStatusGet(portNum=%d):rc=%d\n", portNum, rc);
        return rc;
    }

    /* TRX training status polling loop */
#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX)&& (!defined ASIC_SIMULATION_NO_HW))
    mvSysTimerMeasureStart();
#endif

    /* TRX training start */
    rc = cpssDxChPortSerdesAutoTune(devNum, portNum,
                                    CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TX_TRAINING_START_E);
    if(rc != GT_OK)
    {
        cpssOsPrintf("cpssDxChPortSerdesAutoTuneStatusGet(portNum=%d):rc=%d\n", portNum, rc);
        return rc;
    }
   
    for(timeoutCounter = 0; timeoutCounter < TIMEOUT_MAX; timeoutCounter++)
    {
        rc = prvCpssDxChPortTraceDelay(devNum, 0, 1);
        if(rc != GT_OK)
        {
            cpssOsPrintf("prvCpssDxChPortTraceDelay:rc=%d\n", rc);
            return rc;
        }

        rc = cpssDxChPortSerdesAutoTuneStatusGet(devNum,portNum,&rxTuneStatus,&txTuneStatus);
        if(rc != GT_OK)
        {
            cpssOsPrintf("cpssDxChPortSerdesAutoTuneStatusGet(portNum=%d):rc=%d\n", portNum, rc);
            return rc;
        }
    
        switch(txTuneStatus)
        {
            case   CPSS_DXCH_PORT_SERDES_AUTO_TUNE_PASS_E:
            case   CPSS_DXCH_PORT_SERDES_AUTO_TUNE_FAIL_E:
                rc = cpssDxChPortSerdesAutoTune(devNum, portNum,
                                                CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TRAINING_STOP_E);
                cpssOsPrintf("cpssDxChPortSerdesAutoTune(TRX_STOP,portNum=%d,txTuneStatus=%d):rc=%d\n",
                                portNum, txTuneStatus, rc);

                if(rc != GT_OK)
                {
                    return rc;
                }
                else
                {
        
#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX)&& (!defined ASIC_SIMULATION_NO_HW))
            mvSysTimerMeasureGet(&trxTrainingTime);
            cpssOsPrintf("port=%d:trxTrainingTime=%d(miliSec),timeoutCounter=%d,status=%d\n", 
                         portNum, trxTrainingTime, timeoutCounter, txTuneStatus);
    #endif
                }
                return GT_OK;
    
            case   CPSS_DXCH_PORT_SERDES_AUTO_TUNE_NOT_COMPLITED_E:   
                break;

            default:
                cpssOsPrintf("bad txTuneStatus=%d\n", txTuneStatus);
                return GT_BAD_STATE;
        }
    }

    if(TIMEOUT_MAX == timeoutCounter)
    {
        cpssOsPrintf("trxTrainingOnePortTime: timed out\n");
        rc = cpssDxChPortSerdesAutoTune(devNum, portNum,
                                        CPSS_DXCH_PORT_SERDES_AUTO_TUNE_MODE_TRAINING_STOP_E);
        if(rc != GT_OK)
        {
            cpssOsPrintf("cpssDxChPortSerdesAutoTune(TRX_STOP,portNum=%d,txTuneStatus=%d):rc=%d\n",
                            portNum, txTuneStatus, rc);
            return rc;
        }
    }

    return GT_OK;
}

GT_VOID forceLinkDownPrint
(
    GT_U8                   devNum,
    GT_PHYSICAL_PORT_NUM    portNum
)
{
    GT_U32        forceLinkDownRegAddr; /* register address */
    GT_U32        value;                /* value to write into the register */
    GT_U32        forceLinkDownOffset;  /* bit number inside register       */
    GT_U32  portGroupId;/*the port group Id - support multi-port-groups device */
    PRV_CPSS_PORT_TYPE_ENT portMacType;

    /* convert the 'Physical port' to portGroupId,local port -- supporting multi-port-groups device */
    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    portMacType = PRV_CPSS_DXCH_PORT_TYPE_MAC(devNum,portNum);

    /* for 10 Gb interface and up */
    if(portMacType >= PRV_CPSS_PORT_XG_E)
    {
        PRV_CPSS_DXCH_PORT_MAC_CTRL_REG_MAC(devNum,portNum,portMacType,&forceLinkDownRegAddr);
        forceLinkDownOffset = 2;
    }
    else
    {
        PRV_CPSS_DXCH_PORT_AUTO_NEG_CTRL_REG_MAC(devNum,portNum,&forceLinkDownRegAddr);
        forceLinkDownOffset = 0;
    }
    if(PRV_CPSS_SW_PTR_ENTRY_UNUSED == forceLinkDownRegAddr)
        return;

    if (prvCpssDrvHwPpPortGroupGetRegField(devNum, portGroupId, forceLinkDownRegAddr,
                                           forceLinkDownOffset, 1, &value) != GT_OK)
    {
        return;
    }

    cpssOsPrintf("state=%d\n", BIT2BOOL_MAC(value));
    return;
}




GT_STATUS lion2PortInfoPrint
(
    IN   GT_U8                   devNum,
    IN   GT_PHYSICAL_PORT_NUM    portNum
)
{
    GT_STATUS rc;
    prvCpssDxChLion2PortInfo_STC serdesState;


    rc = lion2PortInfoGet(devNum,portNum, /*OUT*/&serdesState);
    if (rc  != GT_OK)
    {
        return rc;
    }
    cpssOsPrintf("\nSerdes State : port %d",portNum);
    cpssOsPrintf("\n   rxDma If Width                  ( 0 - 64bit       2 - 256 bit      ): %d",serdesState.rxDmaIfWidth);
    cpssOsPrintf("\n   rxDma Source;                   ( 0 - regular Mac 1 - extended Mac ): %d",serdesState.rxDmaSource);
    cpssOsPrintf("\n   mppm  Xlg Mode[0]               ( 0 - ~40G        1 - 40G          ): %d",serdesState.mppmXlgMode[0]);
    cpssOsPrintf("\n   mppm  Xlg Mode[1]               ( 0 - ~40G        1 - 40G          ): %d",serdesState.mppmXlgMode[1]);
    cpssOsPrintf("\n   txDma ThhresholdOverrideEnable  ( 0 - Disable     1 - enable       ): %d",serdesState.txDmaThhresholdOverrideEnable);
    return GT_OK;
}


/* for debug - stop buffersTest */
GT_BOOL buffersTestEnable = GT_TRUE;
GT_U32 buffersTestEnableSet
(
    GT_U32 buffersTestEnableNew
)
{
    GT_U32 buffersTestEnableOld;

    buffersTestEnableOld = buffersTestEnable ? 1 : 0;
    buffersTestEnable = (1 == buffersTestEnableNew) ? 1 : 0;

    return buffersTestEnableOld;
}

/* for debug - set delay between commands */
GT_U32 buffersTestDelay1 = 0;
GT_U32 buffersTestDelay2 = 0;
GT_U32 buffersTestDelay3 = 0;
GT_U32 buffersTestDelaySet
(
    GT_U32 buffersTestDelayNew1,
    GT_U32 buffersTestDelayNew2,
    GT_U32 buffersTestDelayNew3
)
{
    GT_U32 buffersTestDelayOld;

    buffersTestDelayOld = buffersTestDelay1;
    buffersTestDelay1 = buffersTestDelayNew1;
    buffersTestDelay2 = buffersTestDelayNew2;
    buffersTestDelay3 = buffersTestDelayNew3;

    return buffersTestDelayOld;
}

static unsigned __TASKCONV buffersTestTask
(
    GT_VOID * param
)
{
    GT_STATUS rc;
    CPSS_PORTS_BMP_STC initPortsBmp;
    GT_U32      buffersNum;
    GT_U32      pollingPeriod;
    GT_U8                   devNum;
    GT_PHYSICAL_PORT_NUM    portNum;

    devNum = 0;
    portNum = 0;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&initPortsBmp);
    CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmp, portNum);

    pollingPeriod = 0;
    do
    {        
        cpssOsPrintf(".");
        rc = cpssDxChPortEnableSet(devNum,portNum,GT_FALSE);
        if(rc != GT_OK)
        {
            cpssOsPrintSync("cpssDxChPortEnableSet(false):rc=%d\n", rc);
            return rc;
        }
        cpssOsTimerWkAfter(buffersTestDelay1);

        pollingPeriod = (pollingPeriod+1)%20;

        if(0 == pollingPeriod)
        {
            if(cpssDrvPpHwRegisterRead(devNum, 0, 0x3000100+(4*portNum), &buffersNum) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                0x3000100+(4*portNum));
                return GT_HW_ERROR;
            }
            cpssOsPrintf("\nbuffersNum=%d\n", buffersNum);
        }

        rc = cpssDxChPortModeSpeedSet(devNum, initPortsBmp, GT_FALSE, 
                                        CPSS_PORT_INTERFACE_MODE_SR_LR_E, 
                                        CPSS_PORT_SPEED_40000_E);
        if(rc != GT_OK)
        {
            cpssOsPrintSync("cpssDxChPortModeSpeedSet:rc=%d\n", rc);
            return rc;
        }
		cpssOsTimerWkAfter(buffersTestDelay2);

        rc = cpssDxChPortModeSpeedSet(devNum, initPortsBmp, GT_TRUE, 
                                        CPSS_PORT_INTERFACE_MODE_SR_LR_E, 
                                        CPSS_PORT_SPEED_40000_E);
        if(rc != GT_OK)
        {
            cpssOsPrintSync("cpssDxChPortModeSpeedSet:rc=%d\n", rc);
            return rc;
        }

		cpssOsTimerWkAfter(buffersTestDelay3);

        rc = cpssDxChPortEnableSet(devNum,portNum,GT_TRUE);
        if(rc != GT_OK)
        {
            cpssOsPrintSync("cpssDxChPortEnableSet(true):rc=%d\n", rc);
            return rc;
        }

        cpssOsTimerWkAfter(1000);

    }while(buffersTestEnable); 

    return GT_OK;
}



GT_STATUS buffersTest
(
    GT_U8                   devNum,
    GT_PHYSICAL_PORT_NUM    portNum
)
{
    CPSS_TASK   buffersTestTaskId;

    return cpssOsTaskCreate("bufTstTask",               /* Task Name      */
                          190,                        /* Task Priority  */
                          _32K,                     /* Stack Size     */
                          buffersTestTask,   /* Starting Point */
                          NULL,                     /* Arguments list */
                          &buffersTestTaskId);      /* task ID        */
}

#if ((defined _VXWORKS) && (defined CPU_78200)) || ((defined LINUX) && (!defined ASIC_SIMULATION_NO_HW))

/* for debug - stop AP test */
GT_BOOL apTestEnable = GT_TRUE;
GT_U32 apTestEnableSet
(
    GT_U32 apTestEnableNew
)
{
    GT_U32 apTestEnableOld;

    apTestEnableOld = apTestEnable ? 1 : 0;
    apTestEnable = (1 == apTestEnableNew) ? 1 : 0;

    return apTestEnableOld;
}

/* for debug - enable/disable dump prints */
GT_BOOL apTestPrint = GT_TRUE;
GT_U32 apTestPrintSet
(
    GT_U32 apTestPrintNew
)
{
    GT_U32 apTestPrintOld;

    apTestPrintOld = apTestPrint ? 1 : 0;
    apTestPrint = (1 == apTestPrintNew) ? 1 : 0;

    return apTestPrintOld;
}

/* for debug - enable/disable wait for link up */
GT_BOOL apLinkWait = GT_TRUE;
GT_U32 apLinkWaitSet
(
    GT_U32 apLinkWaitNew
)
{
    GT_U32 apLinkWaitOld;

    apLinkWaitOld = apLinkWait ? 1 : 0;
    apLinkWait = (1 == apLinkWaitNew) ? 1 : 0;

    return apLinkWaitOld;
}

/* for debug - set delay between commands */
GT_U32 apTestDelay1 = 50;
GT_U32 apTestDelay2 = 500;
GT_U32 apTestDelay3 = 3000;
GT_U32 apTestDelaySet
(
    GT_U32 apTestDelayNew1,
    GT_U32 apTestDelayNew2,
    GT_U32 apTestDelayNew3
)
{
    GT_U32 apTestDelayOld;

    apTestDelayOld = apTestDelay1;
    apTestDelay1 = apTestDelayNew1;
    apTestDelay2 = apTestDelayNew2;
    apTestDelay3 = apTestDelayNew3;

    return apTestDelayOld;
}

#define DBG_PRINT_MAC(x) if(apTestPrint) cpssOsPrintf x


/* with dump - old test 2 sec with polling every 100mSec */
static unsigned __TASKCONV apTestTask
(
    GT_VOID * param
)
{
    GT_STATUS               rc;
    GT_U8                   devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    CPSS_DXCH_PORT_AP_PARAMS_STC    apParams;
    GT_U32  localPort, portGroup;
    GT_U32  i;
    GT_BOOL isLinkUp;
    GT_U32  iteration;
    GT_U32  serdes;
    GT_U32  regAddr, data;
    GT_U32  loopsNum;
    GT_U32  timeStamp;
    GT_BOOL noLinkAfter2Sec;

    devNum = 0; 
    portNum = *(GT_PHYSICAL_PORT_NUM*)param;
    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    apParams.fcPause       = GT_FALSE;
    apParams.fcAsmDir      = CPSS_DXCH_PORT_AP_FLOW_CONTROL_SYMMETRIC_E;
    apParams.fecSupported  = GT_FALSE;
    apParams.fecRequired   = GT_FALSE;
    apParams.noneceDisable = GT_TRUE;
    apParams.laneNum       = 0;
    apParams.modesAdvertiseArr[0].ifMode = CPSS_PORT_INTERFACE_MODE_KR_E;
    apParams.modesAdvertiseArr[0].speed  = CPSS_PORT_SPEED_10000_E;
    apParams.modesAdvertiseArr[1].ifMode = CPSS_PORT_INTERFACE_MODE_NA_E;
    apParams.modesAdvertiseArr[1].speed  = CPSS_PORT_SPEED_NA_E;

    loopsNum = 2000/apTestDelay1;    
    for(iteration = 0, noLinkAfter2Sec = GT_FALSE; GT_TRUE == apTestEnable; iteration++)
    {        
mvSysTimerMeasureGet(&timeStamp); 
if(!dumpToBuffer)
{
    cpssOsPrintf("\niteration-%d:%d\n", iteration, timeStamp);
}
else
{
    cpssOsSprintf(str, "\niteration-%d:%d\n", iteration, timeStamp);
    addRecord(dumpArrayPtr, str);
}
        rc = cpssDxChPortApPortConfigSet(devNum, portNum, GT_TRUE, &apParams);
        if(rc != GT_OK)
        {
            cpssOsPrintf("cpssDxChPortApPortConfigSet(true) failed\n");
            continue;
        }

dumpStart:    
        for(i = 0; i < loopsNum; i++)
        {
            regAddr = 0x88C0430; 
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
            
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}

            regAddr = 0x88C04D0; 
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}

            regAddr = 0x88C054C; 
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}

            regAddr = 0x88C000C; 
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}

            regAddr = 0x88C0000; 
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x\n", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x\n", regAddr, data);
    addRecord(dumpArrayPtr, str);
}
            regAddr = 0x370E0;
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}
            regAddr = 0x370E8;
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}
            regAddr = 0x370D4;
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}
            if(cpssDrvPpHwRegisterWrite(devNum, portGroup, regAddr, 0) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterWrite(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }

            regAddr = 0x370DC;
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}
            regAddr = 0x370F0;
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}
            regAddr = 0x37120;
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}
            regAddr = 0x37004;
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}
            regAddr = 0x37034;
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}
            regAddr = 0x9800000+localPort*0x400;
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}
            for(serdes = localPort; serdes < localPort+4; serdes++)
            {
                regAddr = 0x9800018 + serdes * 0x400; 
                if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
                {
                    cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                    regAddr);
                    return GT_HW_ERROR;
                }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x,", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x,", regAddr, data);
    addRecord(dumpArrayPtr, str);
}
            }

            regAddr = 0x9800004+localPort*0x400;
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("0x%x:0x%x\n", regAddr, data));
}
else
{
    cpssOsSprintf(str, "0x%x:0x%x\n", regAddr, data);
    addRecord(dumpArrayPtr, str);
}
            cpssOsTimerWkAfter(apTestDelay1);    
        }

        rc = cpssDxChPortLinkStatusGet(devNum, portNum, &isLinkUp);
        if(rc != GT_OK)
        {
            cpssOsPrintf("cpssDxChPortLinkStatusGet failed\n");
        }

        if(isLinkUp)
        {
            mvSysTimerMeasureGet(&timeStamp); 
            if(!dumpToBuffer)
            {
                DBG_PRINT_MAC(("link UP:%d!!!\n", timeStamp));
            }
            else
            {
                cpssOsSprintf(str, "link UP:%d!!!\n", timeStamp);
                addRecord(dumpArrayPtr, str);
            }
        }

        if(!isLinkUp || noLinkAfter2Sec)
        {
            if(!isLinkUp)
            {
                mvSysTimerMeasureGet(&timeStamp); 
                if(!dumpToBuffer)
                {
                    DBG_PRINT_MAC(("NO LINK:%d!!!\n", timeStamp));
                }
                else
                {
                    cpssOsSprintf(str, "NO LINK:%d!!!\n", timeStamp);
                    addRecord(dumpArrayPtr, str);
                }
            }
            if(!noLinkAfter2Sec)
            {
                noLinkAfter2Sec = GT_TRUE;
                goto dumpStart;
            }

            dumpArray(dumpArrayPtr);
            noLinkAfter2Sec = GT_FALSE;
        }
        rc = cpssDxChPortApPortConfigSet(devNum, portNum, GT_FALSE, NULL); 
        if(rc != GT_OK)
        {
            cpssOsPrintf("cpssDxChPortApPortConfigSet(false) failed\n");
        }

        cpssOsTimerWkAfter(2000);
    }

    return GT_OK;
}

static unsigned __TASKCONV apTestPassiveTask
(
    GT_VOID * param
)
{
    GT_U8                   devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U32  localPort, portGroup;
    GT_U32  iteration;
    GT_U32  serdes;
    GT_U32  regAddr, data;
    GT_BOOL isLinkUp;
    GT_U32  timeStamp;
    GT_STATUS   rc;

    devNum = 0;
    portNum = *(GT_PHYSICAL_PORT_NUM*)param;
    portGroup = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    localPort = PRV_CPSS_GLOBAL_PORT_TO_LOCAL_PORT_CONVERT_MAC(devNum, portNum);

    for(iteration = 0; GT_TRUE == apTestEnable; iteration++)
    {
        mvSysTimerMeasureGet(&timeStamp); 
        if(!dumpToBuffer)
        {
            DBG_PRINT_MAC(("%d-timeStamp:%d\n", portNum, timeStamp));
        }
        else
        {
            cpssOsSprintf(str, "%d-timeStamp:%d\n", portNum, timeStamp);
            addRecord(dumpArrayPtr, str);
        }

        regAddr = 0x88C0430+localPort*0x1000; 
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x,", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x,", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        regAddr = 0x88C04D0+localPort*0x1000; 
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x,", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x,", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        regAddr = 0x88C054C+localPort*0x1000; 
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x,", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x,", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        regAddr = 0x88C000C+localPort*0x1000; 
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x,", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x,", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        regAddr = 0x88C0000+localPort*0x1000; 
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x\n", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x\n", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        regAddr = 0x370E0;
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x,", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x,", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        regAddr = 0x370E8;
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x,", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x,", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        regAddr = 0x370D4;
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x,", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x,", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        if(cpssDrvPpHwRegisterWrite(devNum, portGroup, regAddr, 0) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterWrite(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }

        regAddr = 0x370DC;
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x,", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x,", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        regAddr = 0x370F0+localPort*4;
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x,", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x,", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        regAddr = 0x37120;
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x,", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x,", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        regAddr = 0x37004+localPort*4;
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x,", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x,", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        regAddr = 0x37034+localPort*4;
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x\n", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x\n", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        regAddr = 0x9800000+localPort*0x400;
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x,", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x,", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        for(serdes = localPort; serdes < localPort+4; serdes++)
        {
            regAddr = 0x9800018+serdes*0x400;
            if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
            {
                cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                                regAddr);
                return GT_HW_ERROR;
            }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x,", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x,", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        }

        regAddr = 0x9800004+localPort*0x400;
        if(cpssDrvPpHwRegisterRead(devNum, portGroup, regAddr, &data) != GT_OK) 
        {
            cpssOsPrintSync("cpssDrvPpHwRegisterRead(addr=0x%x) failed\n", 
                            regAddr);
            return GT_HW_ERROR;
        }
if(!dumpToBuffer)
{
    DBG_PRINT_MAC(("%d-0x%x:0x%x\n", portNum, regAddr, data));
}
else
{
    cpssOsSprintf(str, "%d-0x%x:0x%x\n", portNum, regAddr, data);
    addRecord(dumpArrayPtr, str);
}
        rc = cpssDxChPortLinkStatusGet(devNum, portNum, &isLinkUp);
        if(rc != GT_OK)
        {
            cpssOsPrintf("cpssDxChPortLinkStatusGet failed\n");
        }

        if(isLinkUp)
        {
            rc = cpssDxChPortForceLinkPassEnableGet(devNum, portNum, &isLinkUp);
            if(rc != GT_OK)
            {
                cpssOsPrintf("cpssDxChPortForceLinkDownEnableGet failed\n");
            }
            
            mvSysTimerMeasureGet(&timeStamp);                 
            if(isLinkUp)
            {
                if(!dumpToBuffer)
                {
                    DBG_PRINT_MAC(("%d-forcedUp:%d\n", portNum, timeStamp));
                }
                else
                {
                    cpssOsSprintf(str, "%d-forcedUp:%d\n", portNum, timeStamp);
                    addRecord(dumpArrayPtr, str);
                }                   
            }
            else
            {
                if(!dumpToBuffer)
                {
                    DBG_PRINT_MAC(("%d-link UP:%d\n", portNum, timeStamp));
                }
                else
                {
                    cpssOsSprintf(str, "%d-link UP:%d\n", portNum, timeStamp);
                    addRecord(dumpArrayPtr, str);
                }
            }
        }

        cpssOsTimerWkAfter(apTestDelay1);     
    }

    return GT_OK;
}

static GT_PHYSICAL_PORT_NUM apTestPortNum;
GT_STATUS apTest
(
    GT_U8                   devNum,
    GT_PHYSICAL_PORT_NUM    portNum
)
{
    CPSS_TASK   apTestTaskId;

    apTestPortNum = portNum;
    return cpssOsTaskCreate("apTestTask",               /* Task Name      */
                          190,                        /* Task Priority  */
                          _32K,                     /* Stack Size     */
                          apTestTask,   /* Starting Point */
                          &apTestPortNum,                     /* Arguments list */
                          &apTestTaskId);      /* task ID        */
}

#define MAX_AP_PASSIVE_PORTS_CNS 2
static GT_PHYSICAL_PORT_NUM apTestPortNumPas[MAX_AP_PASSIVE_PORTS_CNS];
static CPSS_TASK   apTestTaskId[MAX_AP_PASSIVE_PORTS_CNS] = { 0 };

GT_STATUS apTestPassive
(
    GT_U8                   devNum,
    GT_PHYSICAL_PORT_NUM    portNum
)
{    
    GT_U32  i;
    GT_CHAR taskName[16];

    for(i = 0; i < MAX_AP_PASSIVE_PORTS_CNS; i++)
    {
        if(0 == apTestTaskId[i])
        {
            apTestPortNumPas[i] = portNum; 
            cpssOsSprintf(taskName, "apPTsk%d", portNum);
            return cpssOsTaskCreate(taskName,        /* Task Name      */
                                  190,                  /* Task Priority  */
                                  _32K,                 /* Stack Size     */
                                  apTestPassiveTask,    /* Starting Point */
                                  &apTestPortNumPas[i], /* Arguments list */
                                  &apTestTaskId[i]);    /* task ID        */            
        }
    }

    cpssOsPrintf("Only %d ports supported\n", MAX_AP_PASSIVE_PORTS_CNS);

    return GT_NO_RESOURCE;
}

#endif

GT_STATUS appDemoConfigApPorts
(
    GT_U8 devNum,
    GT_BOOL apEnable,
    GT_U32 numOfPortsToConfig,
    CPSS_PORTS_BMP_STC *apPrtBitmapPtr,
    CPSS_DXCH_PORT_AP_PARAMS_STC *apPortsArrPtr,
    GT_BOOL *useCpssDefultsArrPtr
)
{
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_STATUS                       result;
    GT_BOOL                         supported;
    GT_U32                          laneNum,
                                    lanesCount;
    GT_U32                          numOfPortsConfig;


    for(portNum = 0, numOfPortsConfig = 0; portNum < CPSS_MAX_PORTS_NUM_CNS && numOfPortsConfig < numOfPortsToConfig; portNum++)
    {
        if(GT_FALSE == CPSS_PORTS_BMP_IS_PORT_SET_MAC(apPrtBitmapPtr, portNum))
        {
            continue;
        }

        if(GT_TRUE == apEnable)
        {
            GT_U32 portMacMap;
            result = cpssDxChPortInterfaceSpeedSupportGet(devNum, portNum,
                                                      CPSS_PORT_INTERFACE_MODE_XGMII_E,
                                                      CPSS_PORT_SPEED_10000_E,
                                                      &supported);
            if(result != GT_OK)
            {
                cpssOsPrintSync("cpssDxChPortInterfaceSpeedSupportGet(portNum=%d,XGMII):rc=%d\n",
                                portNum, result);
                return result;
            }

            PRV_CPSS_DXCH_PORT_NUM_OR_CPU_PORT_CHECK_AND_MAC_NUM_GET_MAC(devNum,
                                                                     portNum,
                                                                     portMacMap);
            if(supported)
            {
                lanesCount = 6;
                for(laneNum = 0; laneNum < lanesCount; laneNum++)
                {
                    (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                                                CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E,
                                                                ((portMacMap<<8)|laneNum),
                                                                CPSS_EVENT_MASK_E);
                }
            }
            else
            {
                result = cpssDxChPortInterfaceSpeedSupportGet(devNum, portNum,
                                                          CPSS_PORT_INTERFACE_MODE_RXAUI_E,
                                                          CPSS_PORT_SPEED_10000_E,
                                                          &supported);
                if(result != GT_OK)
                {
                    cpssOsPrintSync("cpssDxChPortInterfaceSpeedSupportGet(portNum=%d,RXAUI):rc=%d\n",
                                    portNum, result);
                    return result;
                }
                if(supported)
                {
                    lanesCount = 2;
                    for(laneNum = 0; laneNum < lanesCount; laneNum++)
                    {
                        (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                                                    CPSS_PP_PORT_LANE_SIGNAL_DETECT_CHANGED_E,
                                                                    ((portMacMap<<8)|laneNum),
                                                                    CPSS_EVENT_MASK_E);
                    }
                }
            }

            (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                                        CPSS_PP_PORT_SYNC_STATUS_CHANGED_E,
                                                        portMacMap,
                                                        CPSS_EVENT_MASK_E);

            (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                                        CPSS_PP_PORT_MMPCS_SIGNAL_DETECT_CHANGE_E,
                                                        portMacMap,
                                                        CPSS_EVENT_MASK_E);

            if(CPSS_PP_FAMILY_DXCH_LION2_E == PRV_CPSS_PP_MAC(devNum)->devFamily)
            {
                (GT_VOID)cpssEventDeviceMaskWithEvExtDataSet(devNum,
                                                            CPSS_PP_PORT_LINK_STATUS_CHANGED_E,
                                                            portMacMap,
                                                            CPSS_EVENT_MASK_E);
            }
        }

        /* call cpss api function */
        if(useCpssDefultsArrPtr[numOfPortsConfig] == GT_TRUE)
        {
            result = cpssDxChPortApPortConfigSet(devNum, portNum, apEnable, NULL);
        }
        else
        {
            result = cpssDxChPortApPortConfigSet(devNum, portNum, apEnable, &apPortsArrPtr[numOfPortsConfig]);
        }

        if(result != GT_OK)
        {
            cpssOsPrintSync("cpssDxChPortApPortConfigSet: error unable to config port %d, error = %d\n",
                            portNum, result);
            return result;
        }
        else
        {
            cpssOsPrintSync("cpssDxChPortApPortConfigSet: config port %d\n",portNum);
        }
        numOfPortsConfig++;
    }
    return GT_OK;

}



