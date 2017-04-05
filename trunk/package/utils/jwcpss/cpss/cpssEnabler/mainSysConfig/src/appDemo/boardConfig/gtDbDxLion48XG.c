/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDbDxLion48XG.c
*
* DESCRIPTION:
*       Initialization functions for the Lion board.
*
* FILE REVISION NUMBER:
*       $Revision: 75 $
*
*******************************************************************************/

#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/sysHwConfig/gtAppDemoLioncubConfig.h>
#include <appDemo/userExit/dxCh/appDemoDxChEventHandle.h>
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>

#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>

#include <appDemo/boardConfig/gtDbDxLion48Config.h>

/* structures definition for the use of Dx Cheetah Prestera core SW */
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/private/prvCpssDxChHwInit.h>
#include <cpss/dxCh/dxChxGen/port/private/prvCpssDxChPortCtrl.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h>

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
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>

#include <string.h>
#include <stdlib.h>

#include <appDemo/sysHwConfig/gtAppDemoSfPhyConfig.h>
#include <appDemo/sysHwConfig/gtAppDemoSfPhyIf.h>

#ifdef ASIC_SIMULATION
#include <asicSimulation/SCIB/scib.h>
#endif /*ASIC_SIMULATION*/

/* SFT9100 PHY GRIVER */
/*  For mac_interface */
#define XAUI_INTERFACE 0
#define XFI_INTERFACE  1

#define SPEED_RES_100M_FD 0x0002 /*  100M Full Duplex */
#define SPEED_RES_1GIG_FD 0x0003 /*  1Gig Full Duplex */
#define SPEED_RES_10GIG   0x0004 /*  10Gig Full Duplex */
#define SPEED_RES_100M_HD 0x0005 /*  100M Half Duplex */
/*******************************************************************
 Enabling speeds
 Reading enabled speeds
 *******************************************************************/
#define SPEED_100M      0x0002 /*  This is full-duplex */
#define SPEED_1GIG      0x0004 /*  This is full-duplex */
#define SPEED_10GIG     0x0008
#define SPEED_100M_HD   0x0100 /*  This is half-duplex */
#define SPEED_OFF       0x00F0 /*  Speeds are not advertised. */
/* #define SPEED_1GIG_HD 0x0200  This is half-duplex  (Not supported for SGMII) */



/*******************************************************************************
 * External definitions
 ******************************************************************************/
 extern GT_STATUS qt2025PhyConfig(GT_U8 dev, GT_U32 *xsmiAddrArrayPtr);


/* multi port group fdb look up section */
/* REV_ID_ENT : enum for supported board revisions
    REV_ID_AUTO_128K_E - auto build for 128K mode
    REV_ID_CALLER_CHOICE_E - the caller choose the exact infrastructure,
                             - the caller can choose not to do any special infrastructure.
    REV_ID_AUTO_64K_E - auto build for 64K mode
    REV_ID_ENHANCED_AUTO_128K_E - auto build for enhanced 128K mode
    REV_ID_ENHANCED_AUTO_64K_E - auto build for enhanced 64K mode
*/
enum REV_ID_ENT{
    REV_ID_AUTO_128K_E      = 20 + 1,
    REV_ID_CALLER_CHOICE_E,
    REV_ID_AUTO_64K_E,
    REV_ID_ENHANCED_AUTO_128K_E,
    REV_ID_ENHANCED_AUTO_64K_E,

    REV_ID_LAST_E
};

/*******************************************************************************
 * Global variables
 ******************************************************************************/
#ifndef ASIC_SIMULATION_NO_HW
extern GT_U8 *lioncubSmiAddr;
/* XPCS port of Lioncub - Up0 or Up1 - for both DB and RD boards */
extern GT_U32 *lion48LcXpcsPortArrayPtr;
#endif
static GT_U32   startPhyAddr = 0;
static GT_U32   endPhyAddr = 11;
static GT_BOOL  lion48RdBoard = GT_FALSE; /* will be TRUE for Lion RD-48 */
static GT_U8    ppCounter = 0; /* Number of Packet processors. */
static GT_U32   numOfActivePortGrops = 0; /* number of port groups -- assume single device */
#define LION_CORES_BMP_CNS  0x0000000F
static GT_U32   activePortGroupsBmp = LION_CORES_BMP_CNS;/* bitmap of active port groups
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
static GT_U32   hiddenPortGroupsBmp = 0;/* bitmap of existing port groups - that are hidden from the CPSS.
                                        the appDemo must do 'init stage' for those port groups */

#ifndef ASIC_SIMULATION_NO_HW
static GT_BOOL lioncubInitDone[] = {GT_FALSE, GT_FALSE, GT_FALSE, GT_FALSE};

/* enable lioncub WA configuration debug trace */
GT_BOOL lionCubWaDbgPrintEn = GT_FALSE;
#define LIONCUBWA_DBG_DUMP(x)     if(lionCubWaDbgPrintEn == GT_TRUE) cpssOsPrintf x

#endif /* #ifndef ASIC_SIMULATION_NO_HW */

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

#ifndef ASIC_SIMULATION_NO_HW

static GT_U32 lionDbXsmiAddr[] =
{
    0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb, 0x00, 0x00, 0x00, 0x00,
    0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb, 0x00, 0x00, 0x00, 0x00,
    0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb, 0x00, 0x00, 0x00, 0x00,
    0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb, 0x00, 0x00, 0x00, 0x00
};

static GT_U32 lionRdXsmiAddr[] =
{
    0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb, 0xc, 0xd, 0xe, 0xf, 0x00, 0x00, 0x00, 0x00,
    0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb, 0xc, 0xd, 0xe, 0xf, 0x00, 0x00, 0x00, 0x00,
    0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb, 0xc, 0xd, 0xe, 0xf, 0x00, 0x00, 0x00, 0x00,
    0x4,0x5,0x6,0x7,0x8,0x9,0xa,0xb, 0xc, 0xd, 0xe, 0xf, 0x00, 0x00, 0x00, 0x00,
};

static GT_U32 lion48RdMpc8544XsmiAddr[] =
{
    0x6,0x7,0x4,0x5,0xa,0xb,0x8,0x9,0xe,0xf,0xc,0xd, 0x00, 0x00, 0x00, 0x00,
    0xd,0xc,0xe,0xf,0x9,0x8,0xb,0xa,0x5,0x4,0x7,0x6, 0x00, 0x00, 0x00, 0x00,
    0x6,0x7,0x4,0x5,0xa,0xb,0x8,0x9,0xe,0xf,0xc,0xd, 0x00, 0x00, 0x00, 0x00,
    0xd,0xc,0xf,0xe,0x9,0x8,0xb,0xa,0x5,0x4,0x7,0x6, 0x00, 0x00, 0x00, 0x00,
};

#endif

GT_BOOL mvIsSolarflare = GT_FALSE;
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
    GT_U32      usedDevices[APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS]={0};
    GT_U32      index=0,devIndex0,activeGroupsNum;
    GT_U32      value;
    GT_U32      presteraDevId;
    GT_BOOL     foundFirstDev;
    GT_U8       firstDev;
    GT_U32      actualDevicesFoundBmp = 0;/*bmp of devices found on the PEX */
    GT_U32      actualDevicesFound;/*number of PEX device found*/
	if(boardRevId == 12)
	{
		mvIsSolarflare = GT_TRUE;
	}

#ifndef ASIC_SIMULATION_NO_HW
    lioncubSmiAddr = lioncubSmiAddrDB;
#endif /* #ifndef ASIC_SIMULATION_NO_HW */

    /* ability to set different active port group bitmap */
    if(appDemoDbEntryGet("activePortGropsBmp", &value) == GT_OK)
    {
        activePortGroupsBmp = value;
    }

    if (boardRevId == 10) /* LION_MII */
    {
        appDemoSysConfig.cpuEtherPortUsed = GT_TRUE;
    }

    pciStat = appDemoSysGetPciInfo();
    if(pciStat != GT_OK)
    {
        return GT_NOT_FOUND;
    }

    presteraDevId =
        (((GT_U32)appDemoPpConfigList[0].pciInfo.pciDevVendorId.devId) << 16) |
        MARVELL_VENDOR_ID;

    switch(presteraDevId)
    {
        case CPSS_LION_PORT_GROUPS_01___CNS:
            activePortGroupsBmp = BIT_0 | BIT_1;
            break;
        default:
            /* 4 port groups */
            break;
    }

    if(boardRevId == 8)
    {
        activePortGroupsBmp = BIT_0 | BIT_1;
    }

    for(i = 0 ; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_FALSE)
        {
            /* the scan will return the devices in consecutive order */
            break;
        }

        actualDevicesFoundBmp |= (1<<i);
    }

    activePortGroupsBmp &= LION_CORES_BMP_CNS;

    /*prvCpssPpConfigBitmapNumBitsGet*/
    if((actualDevicesFoundBmp != activePortGroupsBmp) &&
       (actualDevicesFoundBmp != LION_CORES_BMP_CNS) &&
       activePortGroupsBmp != (BIT_0 | BIT_1))/* do not damage those devices */
    {
        GT_U32      jj;

        actualDevicesFound = prvCpssPpConfigBitmapNumBitsGet(actualDevicesFoundBmp);
        numOfActivePortGrops = prvCpssPpConfigBitmapNumBitsGet(activePortGroupsBmp);
        if(numOfActivePortGrops > actualDevicesFound)
        {
            numOfActivePortGrops = actualDevicesFound;
        }

        /* need to set the found devices on the PEX to align the needed bmp of port groups :

           assume PEX 'see' 3 devices , it can be one of the next options:
           0,1,2,-
           0,1,-,3
           0,-,2,3
           -,1,2,3
        */

        for(i = 0 ; i < 32; i++)
        {
            if(numOfActivePortGrops == 0)
            {
                /*reset the rest of the devices */
                for(/*continue*/ ; i < 32; i++)
                {
                    appDemoPpConfigList[i].valid = GT_FALSE;
                }

                break;
            }

            if(activePortGroupsBmp & (1 << i))
            {
                if(numOfActivePortGrops > 0)
                {
                    numOfActivePortGrops--;
                }
                continue;
            }

            /* move the devices to the left */
            for(jj = 31 ; jj > i; jj--)
            {
                appDemoPpConfigList[jj] = appDemoPpConfigList[jj-1];
            }

            appDemoPpConfigList[i].valid = GT_FALSE;
        }

        /* restore value not effect the next code*/
        numOfActivePortGrops = 0;
    }



    foundFirstDev = GT_FALSE;
    firstDev = 0;
    for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
    {
        if(appDemoPpConfigList[i].valid == GT_TRUE)
        {
            if(index == APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS)
            {
                return GT_FULL;
            }

            if( GT_FALSE == foundFirstDev )
            {
                foundFirstDev = GT_TRUE;
                firstDev = i;
            }

            if (boardRevId == 10)  /* LION_MII */
            {
                appDemoPpConfigList[i].cpuPortMode = CPSS_NET_CPU_PORT_MODE_MII_E;
            }

            if(((1<<(i-firstDev)) & activePortGroupsBmp) == 0)
            {
                appDemoPpConfigList[i].valid = GT_FALSE;
                continue;
            }

            usedDevices[index++] = i;
            numOfActivePortGrops++;
        }
    }


    /* No PCI devices found */
    if(numOfActivePortGrops == 0)
    {
        return GT_NOT_FOUND;
    }

    if(numOfActivePortGrops > APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS)
    {
        return GT_NOT_SUPPORTED;
    }

    devIndex0 =  usedDevices[0];

    /* the lion device has 4 port group */
    appDemoPpConfigList[devIndex0].numOfPortGroups = 4;

    /* set the first device to hold info about all the port groups */
    for(i = 0, index = 0, activeGroupsNum=0; i < appDemoPpConfigList[devIndex0].numOfPortGroups; i++)
    {
#ifndef ASIC_SIMULATION_NO_HW
        if(boardRevId == 9)
        {/* in lion_mpc8544 board cores order swapped */
            switch(i)
            {
                case 0:
                    index = 1;
                    break;
                case 1:
                    index = 0;
                    break;
                case 2:
                    index = 3;
                    break;
                case 3:
                    index = 2;
                    break;
                default:
                    return GT_INIT_ERROR;
            }
        }
#endif
        /*invalid all the devices , the first device will be validated after the loop */
        appDemoPpConfigList[i].valid = GT_FALSE;

        if(((1<<i) & activePortGroupsBmp) == 0)
        {
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupActive = GT_FALSE;
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupPciInfo.pciBaseAddr = CPSS_PARAM_NOT_USED_CNS;
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupPciInfo.internalPciBase = CPSS_PARAM_NOT_USED_CNS;

            hiddenPortGroupsBmp |= (1<<i);

            continue;
        }

        appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupActive = GT_TRUE;

        appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupPciInfo =
            appDemoPpConfigList[usedDevices[index++]].pciInfo;

        activeGroupsNum++;
    }

    /* validate the first device */
    appDemoPpConfigList[devIndex0].valid = GT_TRUE;

    appDemoPpConfigList[devIndex0].pciInfo.pciBaseAddr = CPSS_PARAM_NOT_USED_CNS;
    appDemoPpConfigList[devIndex0].pciInfo.internalPciBase =  CPSS_PARAM_NOT_USED_CNS;

    if(activeGroupsNum == 0)
    {
        return GT_BAD_STATE;
    }

    ppCounter = 1;

    *numOfPp    = ppCounter;
    *numOfFa    = 0;
    *numOfXbar  = 0;

    *isB2bSystem = GT_FALSE;/* no B2B with Lion devices */

    return GT_OK;
}

/*******************************************************************************
* lion48InitSmi
*
* DESCRIPTION:
*       Init SMI - set fast SMI speed and change M_SMI Configuration to non inverse mode
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
*       GT_HW_ERROR - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS lion48InitSmi
(
    GT_U8 dev,
    GT_U8 boardRevId
)
{
    GT_U32 portGroup;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev,portGroup)
    {
        if( (8 == boardRevId) ||
            (CPSS_98CX8203_CNS == PRV_CPSS_PP_MAC(dev)->devType) )
        {
            if( (2 == portGroup) || (3 == portGroup) )
                continue;
        }

        /* Change M_SMI Configuration to non inverse mode  - very important !!!! */
        if (prvCpssHwPpPortGroupWriteRegister(dev, portGroup, 0x4004200, 0x28000) != GT_OK)
        {
            return GT_HW_ERROR;
        }

        /* Change mode to fast SMI */
        if (prvCpssHwPpPortGroupSetRegField(dev, portGroup, 0x4004030, 30, 1, 1) != GT_OK)
        {
            return GT_HW_ERROR;
        }
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroup)

    return GT_OK;
}



#define NONE_SWAP 0
#define TX_SWAP 1
#define RX_SWAP 2 
static GT_U32 solarflare_PortLaneSwap[4][12]=
{
	{ RX_SWAP,TX_SWAP, TX_SWAP, TX_SWAP,   RX_SWAP,TX_SWAP, TX_SWAP, TX_SWAP,   RX_SWAP,TX_SWAP, TX_SWAP, TX_SWAP},
	{ RX_SWAP,RX_SWAP, RX_SWAP, TX_SWAP,   RX_SWAP,RX_SWAP, RX_SWAP, TX_SWAP,   RX_SWAP,RX_SWAP, RX_SWAP, TX_SWAP}, 
	{ RX_SWAP,TX_SWAP, TX_SWAP, TX_SWAP,   RX_SWAP,TX_SWAP, TX_SWAP, TX_SWAP,   RX_SWAP,TX_SWAP, TX_SWAP, TX_SWAP} , 
	{ RX_SWAP,RX_SWAP, RX_SWAP, TX_SWAP,   RX_SWAP,RX_SWAP, RX_SWAP, TX_SWAP,   RX_SWAP,RX_SWAP, RX_SWAP, TX_SWAP} , 
};


/*******************************************************************************
* lion48RDSwapFix
*
* DESCRIPTION:
*       Fix for swaped lines for Lion 48 RD
*
* INPUTS:
*       dev - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_HW_ERROR - hw error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS lion48RDSwapFix
(
    GT_U8 dev
)
{
	if(mvIsSolarflare == GT_TRUE)
	{
        GT_U32 i, portGroupId, addr, value;
        
		osPrintf("LionRD48 with solarflare, laneswap config.\n");
		for(portGroupId = 0; portGroupId < 4 ; portGroupId++)
		{
			for(i=0 ; i < 12 ; i++)
			{
				/* Tx swap */
				if(solarflare_PortLaneSwap[portGroupId][i] == TX_SWAP)
				{
					addr = 0x8800200 + i*(0x400);
					value = 0x46;
					/* Reset XPCS */
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
					
					/* bit 13:11 = RxSwp, bit 10:8 = TxSwp */
					addr = 0x8800254 + i*(0x400);
					value = ((0 << 11) | (2 << 8));
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
				 
					addr = 0x8800298 + i*(0x400);
					value = ((1 << 11) | (3 << 8)) ;
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
					
					
					addr = 0x88002dc + i*(0x400);
					value = ((2 << 11) | (0 << 8));
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
							
					addr = 0x8800320 + i*(0x400);
					value = ((3 << 11) | (1 << 8)) ;
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
				  
					addr = 0x8800200 + i*(0x400);
					value = 0x47 ;
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
				}
				/* Rx swap */
				else if(solarflare_PortLaneSwap[portGroupId][i] == RX_SWAP)
				{
					addr = 0x8800200 + i*(0x400);
					value = 0x46 ;
					/* Reset XPCS */
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
				 
					
					/* bit 13:11 = RxSwp, bit 10:8 = TxSwp */
					addr = 0x8800254 + i*(0x400);
					value = ((2 << 11) | (0 << 8));
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
				 
					addr = 0x8800298 + i*(0x400);
					value = ((3 << 11) | (1 << 8)) ;
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
					
					
					addr = 0x88002dc + i*(0x400);
					value = ((0 << 11) | (2 << 8));
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
							
					addr = 0x8800320 + i*(0x400);
					value = ((1 << 11) | (3 << 8)) ;
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
				  
					addr = 0x8800200 + i*(0x400);
					value = 0x47 ;
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
				}
				else  /*none swap*/
				{
					addr = 0x8800200 + i*(0x400);
					value = 0x46 ;
					/* Reset XPCS */
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
				 
					
					/* bit 13:11 = RxSwp, bit 10:8 = TxSwp */
					addr = 0x8800254 + i*(0x400);
					value = ((0 << 11) | (0 << 8));
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
				 
					addr = 0x8800298 + i*(0x400);
					value = ((1 << 11) | (1 << 8)) ;
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
					
					
					addr = 0x88002dc + i*(0x400);
					value = ((2 << 11) | (2 << 8));
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
							
					addr = 0x8800320 + i*(0x400);
					value = ((3 << 11) | (3 << 8)) ;
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
				  
					addr = 0x8800200 + i*(0x400);
					value = 0x47 ;
					if (prvCpssHwPpPortGroupWriteRegister(dev, portGroupId, addr, value) != GT_OK)
					{
						return GT_HW_ERROR;
					}
				}
			}
		}
		
		
		return GT_OK;
	}
    /* Reset XPCS port 8 */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802200, 0x46) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Port 8 RX SWAP */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802254, 0x1000) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802298, 0x1900) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x88022dc, 0x200) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802320, 0xB00) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Normal operation XPCS port 8 */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802200, 0x47) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Reset XPCS port 10 */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802A00, 0x46) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Port 10 RX SWAP */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802a54, 0x1000) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802a98, 0x1900) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802adc, 0x200) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802b20, 0xB00) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Normal operation XPCS port 10 */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802A00, 0x47) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Reset XPCS port 11 */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802E00, 0x46) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Port 11 RX SWAP */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802e54, 0x1000) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802e98, 0x1900) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802edc, 0x200) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802f20, 0xB00) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Normal operation XPCS port 11 */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 0, 0x8802E00, 0x47) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Reset XPCS port 40 */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 2, 0x8802200, 0x46) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Port 40 RX SWAP */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 2, 0x8802254, 0x1000) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 2, 0x8802298, 0x1900) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 2, 0x88022dc, 0x200) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 2, 0x8802320, 0xB00) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Normal operation XPCS port 40 */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 2, 0x8802200, 0x47) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Reset XPCS port 43 */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 2, 0x8802E00, 0x46) != GT_OK)
    {
        return GT_HW_ERROR;
    }

     /* Port 43 RX SWAP */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 2, 0x8802e54, 0x1000) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 2, 0x8802e98, 0x1900) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 2, 0x8802edc, 0x200) != GT_OK)
    {
        return GT_HW_ERROR;
    }
    if (prvCpssHwPpPortGroupWriteRegister(dev, 2, 0x8802f20, 0xB00) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    /* Normal operation XPCS port 43 */
    if (prvCpssHwPpPortGroupWriteRegister(dev, 2, 0x8802E00, 0x47) != GT_OK)
    {
        return GT_HW_ERROR;
    }

    return GT_OK;
}

/*******************************************************************************
* lionSerdesConfig
*
* DESCRIPTION:
*       Init SERDES registers for Lion 48 and 24
*
* INPUTS:
*       dev - device number
*       boardRevId - board revision Id
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
*       None.
*
*******************************************************************************/
typedef struct
{
    GT_U32 firstRegAddr;
    GT_U32 lastRegAddr;
    GT_U32 data;
    GT_U32 waitTime;
}GT_SERDES_INIT;


static GT_STATUS lionSerdesConfig
(
    IN  GT_U8 devNum,
    IN  GT_U8 boardRevId
)
{
    GT_U32 portGroupId, port, regAddr, initSerdesDefaults = 1;
    GT_STATUS rc;
    GT_BOOL isLion48Box[4] = { GT_FALSE };
    GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];
    GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];
    CPSS_PORTS_BMP_STC initPortsBmpXaui, initPortsBmpRxaui;

    PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
    {
        /* in available boxes 24 and 48 can be distinguished by existing lioncub
         * if exists then we are on 48 box, otherwise - 24 */
        isLion48Box[portGroupId] = lioncubConnected(devNum, (GT_U8)portGroupId);

        if(mvIsSolarflare == GT_TRUE)
        {
            isLion48Box[portGroupId] = GT_TRUE ;
        }
        /* wait 10 mSec */
        cpssOsTimerWkAfter(10);
    }
    PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    /* check if user wants to init ports to default values */
    rc = appDemoDbEntryGet("initSerdesDefaults", &initSerdesDefaults);
    if((rc != GT_OK) && (rc != GT_NO_SUCH))
        return rc;
    
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&initPortsBmpXaui);
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&initPortsBmpRxaui);

    /* loop over all GE and FE ports */
    for(port = 0; port < PRV_CPSS_PP_MAC(devNum)->numOfPorts; port++)
    {
        /* skip not existed ports */
        if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, port))
            continue;

        /* convert the 'Physical port' to portGroupId */
        portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, port);

        /* to save time - skip port interface mode here it's done later on in afterInitBoardConfig */
        if((boardRevId == 6) || (boardRevId == 7) || (0 == initSerdesDefaults))
            goto afterPortIfConfig;

        if((isLion48Box[portGroupId] == GT_TRUE) || (port%2 == 0))
        {/* Lion 24 uses XAUI mode where just even ports connected to SerDes */
            /* by default configure ports of Lion 48 to RXAUI mode */
            if(isLion48Box[portGroupId] == GT_TRUE)
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmpRxaui,port);
            }
            else
            {
                CPSS_PORTS_BMP_PORT_SET_MAC(&initPortsBmpXaui,port);
            }

        }

afterPortIfConfig:

        /* in available 24 DB boxes needed lane swap configuration
         * Tx && Rx lane swapping 0<->2 , 1<->3
         */
        if(isLion48Box[portGroupId] == GT_TRUE)
            continue;

        rxSerdesLaneArr[0] = 2;
        txSerdesLaneArr[0] = 2;
        rxSerdesLaneArr[1] = 3;
        txSerdesLaneArr[1] = 3;
        rxSerdesLaneArr[2] = 0;
        txSerdesLaneArr[2] = 0;
        rxSerdesLaneArr[3] = 1;
        txSerdesLaneArr[3] = 1;
        rc = cpssDxChPortXgPscLanesSwapSet(devNum, (GT_U8)port, rxSerdesLaneArr, txSerdesLaneArr);
        if(rc != GT_OK)
            return rc;
    }

    if(GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(&initPortsBmpRxaui))
    {
        rc = cpssDxChPortsFastInit(devNum, initPortsBmpRxaui, 
                                   CPSS_PORT_INTERFACE_MODE_RXAUI_E, 
                                   CPSS_PORT_SPEED_10000_E);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(GT_FALSE == CPSS_PORTS_BMP_IS_ZERO_MAC(&initPortsBmpXaui))
    {
        rc = cpssDxChPortsFastInit(devNum, initPortsBmpXaui, 
                                   CPSS_PORT_INTERFACE_MODE_XGMII_E, 
                                   CPSS_PORT_SPEED_10000_E);
        if(rc != GT_OK)
        {
            return rc;
        }
    }

    if(boardRevId == 9)
    {
        GT_U32 i;
        GT_SERDES_INIT lionSerdesInitArray[] =
        {
            /* first reg , lastreg , offset , waitTime */
            {0x8800000, 0x8802c00, 0x2700, 0xA}, /* Disable port and assert port reset */
            {0x9800000, 0x9805c00, 0x4e00, 0xa}, /* Configuring reference clock to 156.25MHz */
            {0x9800004, 0x9805c04, 0x0008, 0xa}, /* Deactivate SerDes Reset */
            /*--- Configuring SerDes to 6.25Gbps --- */
            {0x980020c, 0x9805e0c, 0x800a, 0x0}, /* Configuring LP SerDes Offset 0x03 */
            {0x9800210, 0x9805e10, /* Foxconn test 0x5515*/ 0x4415, 0x0}, /* Configuring LP SerDes Offset 0x04 */
            {0x9800214, 0x9805e14, 0x8150, 0x1}, /* Configuring LP SerDes Offset 0x05 */
            {0x9800214, 0x9805e14, 0xa150, 0x0}, /* Configuring LP SerDes Offset 0x05 */
            {0x9800218, 0x9805e18, 0xfbaa, 0x0}, /* Configuring LP SerDes Offset 0x06 */
            {0x980021c, 0x9805e1c, 0x8720, 0x0}, /* Configuring LP SerDes Offset 0x07 */
            {0x98003cc, 0x9805fcc, /* 0x2000 */ 0, 0x0}, /* Configuring LP SerDes Offset 0x73 */
            {0x980022c, 0x9805e2c, 0x0000, 0x0}, /* Configuring LP SerDes Offset 0x0b */
            {0x9800230, 0x9805e30, 0x0000, 0x0}, /* Configuring LP SerDes Offset 0x0c */
            {0x9800234, 0x9805e34, 0x4000, 0x0}, /* Configuring LP SerDes Offset 0x0d */
            {0x980023c, 0x9805e3c, /* 0x0008 */ 0x18, 0x0}, /* Configuring LP SerDes Offset 0x0f */
            {0x9800250, 0x9805e50, /* Foxconn test 0x8060*/0xa060 , 0x0}, /* Configuring LP SerDes Offset 0x14 */
            {0x9800254, 0x9805e54, /* Foxconn test 0x7f20*/ 0x7f2d, 0x0}, /* Configuring LP SerDes Offset 0x15 */
            {0x9800258, 0x9805e58, /* 0x0900 *//* Foxconn test 0*/0x100 , 0x0}, /* Configuring LP SerDes Offset 0x16 */
            {0x980027c, 0x9805e7c, /* Foxconn test  0x905a */0x90aa, 0x0}, /* Configuring LP SerDes Offset 0x1F */
            {0x9800280, 0x9805e80, /* 0x4800 */ 0x800, 0x0}, /* Configuring LP SerDes Offset 0x20 */
            {0x980028c, 0x9805e8c, /* Foxconn test 0x336f su 0x036f*/0x0363 , 0x0}, /* Configuring LP SerDes Offset 0x23 */
            {0x980035c, 0x9805f5c, 0x423f, 0x0}, /* Configuring LP SerDes Offset 0x57 */
            {0x9800364, 0x9805f64, 0x5554, 0x0}, /* -----------tommy new 1.8 version -----------------------*/
            {0x980036c, 0x9805f6c, 0x0000, 0x0}, /* Configuring LP SerDes Offset 0x5b*/
            {0x9800234, 0x9805e34, /* 0x0400 */ 0x4000, 0x0}, /* Configuring LP SerDes Offset 0x0D */
            {0x9800228, 0x9805e28, 0xe0c0, 0x0}, /* Configuring LP SerDes Offset 0x0a */
            {0x980036c, 0x9805f6c, 0x8040, 0x1}, /* Configuring LP SerDes Offset 0x5b*/
            {0x980036c, 0x9805f6c, 0x8000, 0x5}, /* Configuring LP SerDes Offset 0x5b*/
            {0x9800224, 0x9805e24, 0x0000, 0x1}, /* Configuring LP SerDes Offset 0x09 */
            {0x9800224, 0x9805e24, 0x8000, 0x1}, /* Configuring LP SerDes Offset 0x09 */
            {0x9800224, 0x9805e24, 0x0000, 0xA}, /* Configuring LP SerDes Offset 0x09 */
            /*--- End of SerDes Configuration --- */
            {0x9800004, 0x9805e04, 0x0018, 0xA}, /* Set RxInit to 0x1 */
            {0x9800004, 0x9805e04, 0x0008, 0xA}, /* Set RxInit to 0x0 */
            {0x9800004, 0x9805e04, 0x0048, 0xA}, /* Set RxInit to 0x1 */
            {0x8800200, 0x8802e00, 0x0046, 0x0}, /* XPCS Configurations */
            {0x8800200, 0x8802e00, 0x0047, 0xA}, /* De assert PCS Reset */
            {0x8800000, 0x8802c00, 0x2703, 0xA}, /* Enable port and De-assert port reset */
        };

        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)
        {
            /* check if current port group connected to lioncub */
            if (lioncubConnected(devNum, (GT_U8)portGroupId) == GT_FALSE)
            {
                continue;
            }

            /* Set ports to RXAUI mode */
            if (prvCpssHwPpPortGroupWriteRegister(devNum, portGroupId, 0x364, 0xfc0) != GT_OK)
            {
                return GT_HW_ERROR;
            }

            /* Set SERDES configuration */
            for (i = 0; i < (sizeof(lionSerdesInitArray) / sizeof(GT_SERDES_INIT)); i++)
            {
                for (regAddr = lionSerdesInitArray[i].firstRegAddr; regAddr <= lionSerdesInitArray[i].lastRegAddr; regAddr+=0x400)
                {
                    if (prvCpssHwPpPortGroupWriteRegister(devNum, portGroupId, regAddr, lionSerdesInitArray[i].data) != GT_OK)
                    {
                        return GT_HW_ERROR;
                    }
                }
                /* wait if delay is not zero */
                if (lionSerdesInitArray[i].waitTime != 0)
                {
                    cpssOsTimerWkAfter(lionSerdesInitArray[i].waitTime);
                }
            }
        }/* for (portGroup = 0; */
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(devNum,portGroupId)

    }/* if(boardRevId == 9) */

    /* For Lion RD board - fix swapped lines */
    if ((GT_TRUE == lion48RdBoard) && (boardRevId != 9))
    {
        if (lion48RDSwapFix(devNum) != GT_OK)
        {
            return GT_FAIL;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* lionPortInterfaceInit
*
* DESCRIPTION:
*     Init lion, lioncub and phys - and perform WA. Should be done after board initialization !
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
static GT_STATUS lionPortInterfaceInit
(
    IN  GT_U8 dev,
    IN  GT_U8 boardRevId
)
{
    GT_STATUS rc;
    GT_U8 port;
#ifndef ASIC_SIMULATION_NO_HW
    GT_U32  portGroup;
    GT_LIONCUB_INIT *lioncubInitArray;
    CPSS_LIONCUB_REV_ENT rev;
    GT_U32 *xsmiAddrArray;

    lion48LcXpcsPortArrayPtr = lion48LcXpcsPortArray;
#endif

    /* If it's RD board fix SMI addresses pointer and LcXpcs pointer */
    if (GT_TRUE == lion48RdBoard)
    {
        startPhyAddr = 4;
        endPhyAddr   = 15;
#ifndef ASIC_SIMULATION_NO_HW
        lioncubSmiAddr = lioncubSmiAddrRD;
#endif
    }

    rc = lion48InitSmi(dev, boardRevId);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("lionPortInterfaceInit", rc);
    if(rc != GT_OK)
    {
        return rc;
    }

    /* Serdes init for Lion */
    if (lionSerdesConfig(dev, boardRevId) != GT_OK)
    {
        osPrintf("lionPortInterfaceInit: lionSerdesConfig failed for dev%d.\n",dev);
        return GT_FAIL;
    }
    else
    {
        osPrintf("lionPortInterfaceInit: lionSerdesConfig OK for dev%d.\n",dev);
    }

	if(mvIsSolarflare == GT_TRUE)
    {
		/* Do nothing, phy is configured by flash */
		osPrintf("lionPortInterfaceInit: Solarflare phy is done by flash\n",dev);
    }	
    /* init only if at least one portGroup connected to Lioncub */
    if ((lioncubConnected(dev, 0) == GT_TRUE) ||
        (lioncubConnected(dev, 1) == GT_TRUE) ||
        (lioncubConnected(dev, 2) == GT_TRUE) ||
        (lioncubConnected(dev, 3) == GT_TRUE))
    {
        /* must disable learning, to avoid garbage in FDB, because WA sends packets internally */
        for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfPorts; port++)
        {
            if (PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType ==
                 PRV_CPSS_PORT_NOT_EXISTS_E)
                continue;

            rc = cpssDxChBrgFdbPortLearnStatusSet(dev, port, GT_FALSE, CPSS_LOCK_DROP_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbPortLearnStatusSet - controlled learning", rc);
            if (GT_OK != rc)
            {
                return rc;
            }

            /* note the cascade init will make sure to disable the sending from
               the cascade ports */
            rc = cpssDxChBrgFdbNaToCpuPerPortSet(dev, port, GT_FALSE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbNaToCpuPerPortSet", rc);
            if (GT_OK != rc)
            {
                return rc;
            }
        }
#ifndef ASIC_SIMULATION_NO_HW
        PRV_CPSS_GEN_PP_START_LOOP_PORT_GROUPS_MAC(dev,portGroup)
        {
            if(CPSS_98CX8203_CNS == PRV_CPSS_PP_MAC(dev)->devType)
            {
                if( (2 == portGroup) || (3 == portGroup) )
                    continue;
            }

            /* get Lioncub revision to decide what init array to use */
            if((rc = lioncubRevisionGet(dev, portGroup, &rev))!= GT_OK)
            {
                osPrintf("lionPortInterfaceInit: lioncubRevisionGet failed, rc=0x%x\n", rc);
                return rc;
            }

            switch (rev)
            {
                case CPSS_LIONCUB_REV_A0_E:
                    if (lion48RdBoard == GT_TRUE)
                    {
                        lioncubInitArray = lioncubInitArrayRD_A0;
                    }
                    else
                    {
                        lioncubInitArray = lioncubInitArrayDB_A0;
                    }
                    break;
                case CPSS_LIONCUB_REV_A1_E:
                    if (lion48RdBoard == GT_TRUE)
                    {
                        /* init Lioncub to default - 10G for RD board */
                        lioncubInitArray = lioncubInitArrayRD_A1_10G;
                    }
                    else
                    {
                        /* init Lioncub to default - 10G */
                        lioncubInitArray = lioncubInitArrayDB_A1_10G;
                    }
                break;
                default:
                    continue;
            }

            if((rc = lioncubInit(dev, portGroup, lioncubInitArray)) != GT_OK)
            {
                osPrintf("lionPortInterfaceInit: lioncub rev A%d init failed for dev%d portGroup %d.\n", rev, dev, portGroup);
                return rc;
            }
            else
            {
                lioncubInitDone[portGroup] = GT_TRUE;
                osPrintf("lionPortInterfaceInit: lioncub rev A%d init OK for dev%d portGroup %d.\n",
                            rev, dev, portGroup);
            }
        }
        PRV_CPSS_GEN_PP_END_LOOP_PORT_GROUPS_MAC(dev,portGroup)

        if (GT_TRUE == lion48RdBoard)
        {
            xsmiAddrArray = (boardRevId != 9) ? lionRdXsmiAddr : lion48RdMpc8544XsmiAddr;
        }
        else
        {
            xsmiAddrArray = lionDbXsmiAddr;
        }

        /* Phy init for Lion 48 */
        if (qt2025PhyConfig((GT_U8)dev, xsmiAddrArray) != GT_OK)
        {
            osPrintf("lionPortInterfaceInit: qt2025PhyConfig failed for dev%d.\n",dev);
            return GT_FAIL;
        }
        else
        {
            osPrintf("lionPortInterfaceInit: qt2025PhyConfig OK for dev%d.\n",dev);
        }
#endif

        for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfPorts; port++)
        {
            if (PRV_CPSS_PP_MAC(dev)->phyPortInfoArray[port].portType ==
                 PRV_CPSS_PORT_NOT_EXISTS_E)
                continue;

            rc = cpssDxChBrgFdbPortLearnStatusSet(dev, port, GT_TRUE, CPSS_LOCK_FRWRD_E);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbPortLearnStatusSet - controlled learning", rc);
            if (GT_OK != rc)
            {
                return rc;
            }

            /* note the cascade init will make sure to disable the sending from
               the cascade ports */
            rc = cpssDxChBrgFdbNaToCpuPerPortSet(dev, port, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChBrgFdbNaToCpuPerPortSet", rc);
            if (GT_OK != rc)
            {
                return rc;
            }
        }
    }/* if lioncubConnected... */

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
    GT_STATUS rc = GT_OK;
    CPSS_PP_PHASE1_INIT_PARAMS    localPpPh1Config = CPSS_PP_PHASE1_DEFAULT;
    void    *intVecNum;
    GT_U32  ii;
#ifndef ASIC_SIMULATION
    GT_U32  regData;
    GT_BOOL doByteSwap = GT_FALSE;
#endif /*ASIC_SIMULATION*/

    /* revision 5 for lion Rd-48 board from MTL */
    if ((boardRevId == 5) || (boardRevId == 9) || (boardRevId == 12))
    {
        lion48RdBoard = GT_TRUE;
    }

    localPpPh1Config.devNum = devIdx;

    localPpPh1Config.baseAddr =
                            appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr;
    localPpPh1Config.internalPciBase =
                        appDemoPpConfigList[devIdx].pciInfo.internalPciBase;
    localPpPh1Config.deviceId =
        ((appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.devId << 16) |
         (appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.vendorId));

    localPpPh1Config.mngInterfaceType = CPSS_CHANNEL_PEX_E;

    localPpPh1Config.numOfPortGroups = appDemoPpConfigList[devIdx].numOfPortGroups;
    for(ii = 0 ; ii < appDemoPpConfigList[devIdx].numOfPortGroups ;ii++)
    {
        localPpPh1Config.portGroupsInfo[ii].busBaseAddr =
            appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupPciInfo.pciBaseAddr;
        localPpPh1Config.portGroupsInfo[ii].internalPciBase =
            appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupPciInfo.internalPciBase;

        if(appDemoPpConfigList[devIdx].portGroupsInfo[ii].portGroupPciInfo.pciBaseAddr ==
           CPSS_PARAM_NOT_USED_CNS)
        {
            continue;
        }

        extDrvGetPciIntVec(GT_PCI_INT_D - ii , &intVecNum);
        localPpPh1Config.portGroupsInfo[ii].intVecNum = (GT_U32)((GT_UINTPTR)intVecNum);
        extDrvGetIntMask(GT_PCI_INT_D - ii , &localPpPh1Config.portGroupsInfo[ii].intMask);
    }

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

    if((boardRevId == 6) || (boardRevId == 7))
    {
        rc = appDemoDebugDeviceIdSet(devIdx, CPSS_98DX5198_CNS);
    }
    else if(boardRevId == 8)
    {
        appDemoDebugDeviceIdSet(devIdx, CPSS_98CX8203_CNS);
    }

    if((boardRevId == 8) || hiddenPortGroupsBmp)
    {
        /* trigger start init sequence for cores 2 & 3 */

#ifndef ASIC_SIMULATION
        /* find if byte swap is needed */
        rc = cpssDxChDiagRegRead(
            appDemoPpConfigList[devIdx].portGroupsInfo[0].portGroupPciInfo.pciBaseAddr,
            CPSS_CHANNEL_PEX_E,
            CPSS_DIAG_PP_REG_INTERNAL_E,
            0x50,
            &regData,
            doByteSwap);
        if( GT_OK != rc )
        {
            return rc;
        }

        if( 0x11AB != regData )
        {
            doByteSwap = GT_TRUE;
        }

        /* check if start init already finished successfully */
        rc = cpssDxChDiagRegRead(
            appDemoPpConfigList[devIdx].portGroupsInfo[0].portGroupPciInfo.pciBaseAddr,
            CPSS_CHANNEL_PEX_E,
            CPSS_DIAG_PP_REG_INTERNAL_E,
            0x5C,
            &regData,
            doByteSwap);

        if( GT_OK != rc )
        {
            return rc;
        }

        if( 0x6000 != (regData & 0x6000) )
        {
            for( ii = 2 ; ii < 4 ; ii++ )
            {
                if(((1 << ii) & hiddenPortGroupsBmp) == 0)
                {
                    /* skip cores that are not 'hidden' */
                    continue;
                }

                rc = cpssDxChDiagRegRead(
                    appDemoPpConfigList[devIdx+ii].pciInfo.pciBaseAddr,
                    CPSS_CHANNEL_PEX_E,
                    CPSS_DIAG_PP_REG_INTERNAL_E,
                    0x58,
                    &regData,
                    doByteSwap);

                if( GT_OK != rc )
                {
                    return rc;
                }

                rc = cpssDxChDiagRegWrite(
                    appDemoPpConfigList[devIdx+ii].pciInfo.pciBaseAddr,
                    CPSS_CHANNEL_PEX_E,
                    CPSS_DIAG_PP_REG_INTERNAL_E,
                    0x58,
                    (regData & 0x7FFFFFFF),
                    doByteSwap);
                if( GT_OK != rc )
                {
                    return rc;
                }
            }
        }
#endif /*  ASIC_SIMULATION */
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
    GT_U32  enablePortUnidirectWa = 0; /* do we want to enable WA for GE ports unidirect */
    GT_U32  enableSerdesAccessWa; /* sequential access to internal serdes registers WA */
    GT_U32  enableAuFifoWa; /* enable FIFO to send AU messages to CPU */

    boardRevId = boardRevId;


    if(GT_OK != appDemoDbEntryGet("RESTRICTED_ADDRESS_FILTERING_WA_E", &enableAddrFilterWa))
    {
        enableAddrFilterWa = 0;
    }

    if(GT_OK != appDemoDbEntryGet("GT_CORE_SYS_WA_OPTION_GE_PORT_UNIDIRECT_WA_E", &enablePortUnidirectWa))
    {
        enablePortUnidirectWa = 0;
    }

    if(GT_OK != appDemoDbEntryGet("GT_CORE_SYS_WA_OPTION_SERDES_INTERNAL_REG_ACCESS_WA_E", &enableSerdesAccessWa))
    {
        enableSerdesAccessWa = 0;
    }

    if(GT_OK != appDemoDbEntryGet("enableAuFifoWa", &enableAuFifoWa))
    {
        enableAuFifoWa = 0;
    }

    for(ii = SYSTEM_DEV_NUM_MAC(0); ii < SYSTEM_DEV_NUM_MAC(ppCounter); ++ii)
    {
        waIndex = 0;
        devNum = appDemoPpConfigList[ii].devNum;

        /* state that application want CPSS to implement the WA */
        waFromCpss[waIndex++] =
            CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FU_FROM_NON_SOURCE_PORT_GROUP_E;

        if (enableAddrFilterWa == 1)
        {
            /* state that application want CPSS to implement the Restricted
               Address Filtering WA */
            waFromCpss[waIndex++] =
                CPSS_DXCH_IMPLEMENT_WA_RESTRICTED_ADDRESS_FILTERING_E;
        }

        if(enablePortUnidirectWa)
        {
            /* state that application want CPSS to implement the GE port unidirect
                mode WA */
            waFromCpss[waIndex++] =
                            CPSS_DXCH_IMPLEMENT_WA_GE_PORT_UNIDIRECT_E;
        }

        if(enableSerdesAccessWa)
        {
            /* state that application want CPSS to implement the serdes internal
                registers sequencial access WA */
            waFromCpss[waIndex++] =
                            CPSS_DXCH_IMPLEMENT_WA_SERDES_INTERNAL_REG_ACCESS_E;
        }

        if(enableAuFifoWa == 1)
        {
            /* state that application wants PP to send AU messages over FIFO */
            waFromCpss[waIndex++] = CPSS_DXCH_IMPLEMENT_WA_FDB_AU_FIFO_E;
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
    }

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

#ifdef ASIC_SIMULATION
    {
        GT_U32 simDeviceId , i;
        GT_U32  devNum,portGroupId;

        for(i = 0; i < PRV_CPSS_MAX_PP_DEVICES_CNS; i++)
        {
            if(appDemoPpConfigList[i].valid == GT_FALSE)
            {
                continue;
            }
            devNum = i;

            /* loop on non-active port groups ... and disconnect from them */
            PRV_CPSS_GEN_PP_START_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(devNum,portGroupId)
            {
                simDeviceId = portGroupId + 1;/* this is ugly but the CPSS not hold info about how to reach this deviceId because it is not bound to it. */
                scibUnBindDevice(simDeviceId);
            }
            PRV_CPSS_GEN_PP_END_LOOP_NON_ACTIVE_PORT_GROUPS_MAC(devNum,portGroupId)
        }
    }

#endif/*ASIC_SIMULATION*/


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
    OUT CPSS_PP_PHASE2_INIT_PARAMS    *phase2Params
)
{
    CPSS_PP_PHASE2_INIT_PARAMS  localPpPh2Config = CPSS_PP_PHASE2_DEFAULT;
    GT_STATUS                   retVal = GT_OK;
    GT_U32                      auDescNum = 0;
    boardRevId = boardRevId;

    localPpPh2Config.devNum     = oldDevNum;
    localPpPh2Config.deviceId   = appDemoPpConfigList[oldDevNum].deviceId;

    localPpPh2Config.fuqUseSeparate = GT_TRUE;
    auDescNum = AU_DESC_NUM_DEF * numOfActivePortGrops;

    retVal = appDemoAllocateDmaMem(localPpPh2Config.deviceId,RX_DESC_NUM_DEF,
                                   RX_BUFF_SIZE_DEF,RX_BUFF_ALLIGN_DEF,
                                   TX_DESC_NUM_DEF,
                                   auDescNum,
                                   &localPpPh2Config);

    osMemCpy(phase2Params,&localPpPh2Config,sizeof(CPSS_PP_PHASE2_INIT_PARAMS));
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
    GT_PHYSICAL_PORT_NUM port; /* port number */
    GT_U8  index;

    boardRevId = boardRevId;

    for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
    {
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
        }

        appDemoPpConfigList[dev].numberOf10GPortsToConfigure = index;

        /* lion48Init((GT_U8)dev); */
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
    localPpCfgParams.numOfTrunks = APP_DEMO_127_TRUNKS_CNS;
    localPpCfgParams.maxNumOfVirtualRouters = 1;
    localPpCfgParams.lpmDbFirstTcamLine = 100;
    localPpCfgParams.lpmDbLastTcamLine = 1204;

    if (boardRevId == 10) /* LION_MII */
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

/*******************************************************************************
* preformanceTestBoardConfig
*
* DESCRIPTION:
*       Performace tests configurations.
*
* INPUTS:
*       devNum  - device number.
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
static GT_STATUS preformanceTestBoardConfig
(
    IN  GT_U8   devNum
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      regAddr;    /* register address */
    GT_PHYSICAL_PORT_NUM  port;       /* port number */
    GT_U32      i,j;        /* loop counters */
    CPSS_DXCH3_POLICER_MNG_CNTR_ENTRY_STC mngCntr; /* management counter entry */

    /* Raise Core clock frequency to 366Mhz */

    /* Write to core_PLL_params register (address 0xB4) value 0x230AF013 */
    rc = prvCpssHwPpWriteRegister(devNum, 0xB4, 0x230AF013);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* Write to core_PLL_ctrl rergister (address 0xB8) value 0x10C */
    rc = prvCpssHwPpWriteRegister(devNum, 0xB8, 0x10C);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* Change link list last pointer */

    /* Write to bufMngMetalFix (address 0x3000060) value 0xFFFF0011 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngMetalFix;
    rc = prvCpssHwPpWriteRegister(devNum, regAddr, 0xFFFF0011);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* Write to Linked List Tail Pointer (address 0x3000200 value 0x7FF */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.linkedListTailPtr;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 13, 0x7FF);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* Write to bufMngMetalFix (address 0x3000060) value 0xFFFF0010 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngMetalFix;
    rc = prvCpssHwPpWriteRegister(devNum, regAddr, 0xFFFF0010);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* Configure BM to 2K and global Xon and Xoff values */

    /* MaxBufLimit - 2016 (=63*32) */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->bufferMng.bufMngGlobalLimit;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 22, 9, 63);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* Global Xon - 56 */
    rc = cpssDxChGlobalXonLimitSet(devNum, 28);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* Global Xoff - 2040 */
    rc = cpssDxChGlobalXoffLimitSet(devNum, 1020);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* Configure profile0 values in BM */

    /* ProfileXon - 40 */
    rc = cpssDxChPortXonLimitSet(devNum, CPSS_PORT_RX_FC_PROFILE_1_E, 20);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* ProfileXoff - 120 */
    rc = cpssDxChPortXoffLimitSet(devNum, CPSS_PORT_RX_FC_PROFILE_1_E, 60);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* ProfileRxBufLimit - 160 */
    rc = cpssDxChPortRxBufLimitSet(devNum, CPSS_PORT_RX_FC_PROFILE_1_E, 40);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* Bind all ports to profile 0 */
    for(port = 0 ; port < CPSS_MAX_PORTS_NUM_CNS ; port++)
    {
        if( !(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(devNum)->existingPorts, port)))
        {
            continue;
        }

        rc = cpssDxChPortRxFcProfileSet(devNum, port, CPSS_PORT_RX_FC_PROFILE_1_E);
        if( GT_OK != rc)
        {
            return rc;
        }
    }

    /* Reset Policer counters - the policer counter sets are not reset when resetting */
    /* the chip so we need to manually reset them.                                    */
    mngCntr.duMngCntr.l[0] = 0;
    mngCntr.duMngCntr.l[1] = 0;
    mngCntr.packetMngCntr  = 0;

    for ( i = CPSS_DXCH3_POLICER_MNG_CNTR_SET0_E ;
          i <= CPSS_DXCH3_POLICER_MNG_CNTR_SET2_E ;
          i++ )
    {
        for ( j = CPSS_DXCH3_POLICER_MNG_CNTR_GREEN_E ;
              j <= CPSS_DXCH3_POLICER_MNG_CNTR_DROP_E ;
              j++ )
        {
            rc = cpssDxCh3PolicerManagementCountersSet(devNum,
                                                       CPSS_DXCH_POLICER_STAGE_INGRESS_0_E,
                                                       i,j,
                                                       &mngCntr);
            if( GT_OK != rc)
            {
                return rc;
            }
        }
    }

    /* Inverse MDC clock */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->smiRegs.lms0MiscConfig;
    rc = prvCpssHwPpWriteRegister(devNum, regAddr, 0x28000);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* TxQ thresholds */

    if(0 == PRV_CPSS_DXCH_PP_HW_INFO_TXQ_REV_1_OR_ABOVE_MAC(devNum))
    {
    /* Change TxQ global buffers limit to 1550 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.totalBufferLimitConfig;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 18, 1550);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* TxQ control register, TotalDescLimit - 1999 */
    regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->egrTxQConf.txQueueConfig;
    rc = prvCpssHwPpSetRegField(devNum, regAddr, 16, 16, 1999);
    if( GT_OK != rc)
    {
        return rc;
        }
    }
    else
    {
        /* global buffers limit to 1550 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.limits.globalBufferLimit;
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 14, 1550);
        if( GT_OK != rc)
        {
            return rc;
        }

        /* TotalDescLimit - 1999 */
        regAddr = PRV_CPSS_DXCH_DEV_REGS_MAC(devNum)->txqVer1.queue.tailDrop.limits.globalDescriptorsLimit;
        rc = prvCpssHwPpSetRegField(devNum, regAddr, 0, 14, 1999);
        if( GT_OK != rc)
        {
            return rc;
        }
    }

    /* SharedPoolDescLimit - 1696(=1999-25*12), SharedPoolBufLimit - 1068(=1550-40*12) */
    rc = cpssDxChPortTxSharedGlobalResourceLimitsSet(devNum, 1068, 1696);
    if( GT_OK != rc)
    {
        return rc;
    }
    /* Disable flow control */
    rc = cpssDxChPortFcHolSysModeSet(devNum, CPSS_DXCH_PORT_HOL_E);
    if( GT_OK != rc)
    {
        return rc;
    }

    for(port = 0 ; port < CPSS_MAX_PORTS_NUM_CNS ; port++)
    {
        if( !(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(devNum)->existingPorts, port)))
        {
            continue;
        }

        /* Disable flow control per port */
        rc = cpssDxChPortFlowControlEnableSet(devNum, port, GT_FALSE);
        if( GT_OK != rc)
        {
            return rc;
        }

        /* Enable Jumbo frames, FrameSizeLimit - 10240 */
        rc = cpssDxChPortMruSet(devNum, port, 10240);
        if( GT_OK != rc)
        {
            return rc;
        }
    }

    /* Allow Jumbo frames in ingress pipe */

    for(port = 0 ; port < CPSS_MAX_PORTS_NUM_CNS ; port++)
    {
        if( !(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(devNum)->existingPorts, port)))
        {
            continue;
        }

        /* PCL - OverSizeUntaggedPcktsFltrEn - disable */
        rc = cpssDxChBrgGenPortOversizeUntaggedPacketsFilterEnableSet(devNum, port, GT_FALSE);
        if( GT_OK != rc)
        {
            return rc;
        }
    }

    /* PolicerMRU - set to 10240 */
    rc = cpssDxCh3PolicerMruSet(devNum, CPSS_DXCH_POLICER_STAGE_INGRESS_0_E, 10240);
    if( GT_OK != rc)
    {
        return rc;
    }

    /* L2I - configure the MRU in each of the 8 profiles, LMRUProfile - 10240) */
    for (i = 0 ; i <= 7 ; i++ )
    {
        rc = cpssDxChBrgVlanMruProfileValueSet(devNum, i, 10240);
        if( GT_OK != rc)
        {
            return rc;
        }
    }

    /* Router - configure the MTU in the 8 MTU indexes, NextHopMTU - 10240 */
    for (i = 0 ; i <= 7 ; i++ )
    {
        rc = cpssDxChIpMtuProfileSet(devNum, i, 10240);
        if( GT_OK != rc)
        {
            return rc;
        }
    }

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
    GT_U8       port;

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

    /* note that the appDemo sets the aging mode to 'Triggered mode' by default
       so no auto aging unless you set it explicitly */

    /* do configurations for performance tests */
    if( 2 == boardRevId )
    {
        for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
        {
            rc = preformanceTestBoardConfig(dev);
            if(rc != GT_OK)
            {
                return rc;
            }
        }
    }

    for(dev = SYSTEM_DEV_NUM_MAC(0); dev < SYSTEM_DEV_NUM_MAC(ppCounter); dev++)
    {
        if(systemRecoveryInfo.systemRecoveryState !=
                CPSS_SYSTEM_RECOVERY_COMPLETION_STATE_E)
        {   /* skip lionCub/ports/phy configuration during system recovery */
            break;
        }

        lionPortInterfaceInit(dev, boardRevId);

        if((boardRevId == 6) || (boardRevId == 7))
        {
            for (port = 0; port < PRV_CPSS_PP_MAC(dev)->numOfPorts; port++)
            {
                /* skip not existed ports */
                if (! PRV_CPSS_PHY_PORT_IS_EXIST_MAC(dev, port))
                    continue;

                rc = gtAppDemoLionPortModeSpeedSet(dev, port, ((boardRevId == 6) ?
                                                                CPSS_PORT_INTERFACE_MODE_1000BASE_X_E
                                                                : CPSS_PORT_INTERFACE_MODE_SGMII_E),
                                                                CPSS_PORT_SPEED_1000_E);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }
        }
    }

    if (boardRevId == 10) /* LION_MII */
    {
        /* configure the lion asic for mii mode */
        osPrintf("afterInitBoardConfig: configuring Lion in MII mode.\n");

        /*    Config cpu_port_distribution register of port groups 0-2.
                         Egress Port Group for TO_CPU packets is set to port group 3 */
        prvCpssHwPpPortGroupSetRegField(0, 0, 0x1800000, 0, 3, 0x6);
        prvCpssHwPpPortGroupSetRegField(0, 1, 0x1800000, 0, 3, 0x6);
        prvCpssHwPpPortGroupSetRegField(0, 2, 0x1800000, 0, 3, 0x6);
        /* For port group 3 set local port */
        prvCpssHwPpPortGroupSetRegField(0, 3, 0x1800000, 0, 3, 0x7);

        /* Config Cpu port of the Lion */

        /* Set force link-down */
        prvCpssHwPpPortGroupWriteRegister(0, 3, 0xA80FC0C, 0x9229);
        /* Port disable */
        prvCpssHwPpPortGroupWriteRegister(0, 3, 0xA80fc00, 0x8be4);
        /* change CPU port to Active */
        prvCpssHwPpPortGroupWriteRegister(0, 3, 0xA0,      0xb);
        /* port MAC reset */
        prvCpssHwPpPortGroupWriteRegister(0, 3, 0xA80FC08, 0xC248);
        /* port enable */
        prvCpssHwPpPortGroupWriteRegister(0, 3, 0xA80fc00, 0x8be5);
        prvCpssHwPpPortGroupWriteRegister(0, 3, 0xA80FC08, 0xC240);

        /* port MAC un-reset */
        prvCpssHwPpPortGroupWriteRegister(0, 3, 0xA80FC08, 0xC200);

        cpssOsTimerWkAfter(100);

        /* Set speed to 1000, force link-up */
        prvCpssHwPpPortGroupWriteRegister(0, 3, 0xA80FC0C, 0x926A);
    }

    /* FEr#3112 WA:  register a CB function */
    rc = appDemoDxChNetRxPacketCbRegister(appDemoDxChNetPtpOverUdpWaCb);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* gtAppDemoLioncubPortSpeedSet
*
* DESCRIPTION:
*      External API for initialization of one port of lioncub
*
* INPUTS:
*       devNum  - device number
*       portNum - global port number
*       speed   - ASIC port data speed
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
*       At the moment supported just:
*           CPSS_PORT_SPEED_1000_E  - CPSS_DXCH_PORT_SERDES_SPEED_1_25_E
*           CPSS_PORT_SPEED_2500_E  - CPSS_DXCH_PORT_SERDES_SPEED_3_125_E
*           CPSS_PORT_SPEED_10000_E - CPSS_DXCH_PORT_SERDES_SPEED_6_25_E
*
*******************************************************************************/
GT_STATUS gtAppDemoLioncubPortSpeedSet
(
    IN GT_U8   devNum,
    IN GT_U8   portNum,
    IN CPSS_PORT_SPEED_ENT  speed
)
{
#ifndef ASIC_SIMULATION_NO_HW
    GT_STATUS   rc;
#endif
    GT_LIONCUB_INIT *lioncubInitArray;
    GT_U8           portGroup;
    CPSS_LIONCUB_REV_ENT rev;

    portGroup = (GT_U8)PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);
    if (lioncubConnected(devNum, portGroup) == GT_FALSE)
        return GT_OK; /* the configuration just not needed - don't fail */

#ifndef ASIC_SIMULATION_NO_HW
    /* get Lioncub revision to decide what init array to use */
    if((rc = lioncubRevisionGet(devNum, portGroup, &rev))!= GT_OK)
    {
        osPrintf("lionPortInterfaceInit: lioncubRevisionGet failed, rc=0x%x\n", rc);
        return rc;
    }
#else
    rev = CPSS_LIONCUB_REV_A1_E;
#endif 

    switch(speed)
    {
        case CPSS_PORT_SPEED_1000_E:
            if((CPSS_LIONCUB_REV_A0_E == rev) || (lion48RdBoard == GT_TRUE))
                return GT_NOT_SUPPORTED;
            lioncubInitArray = lioncubInitArrayDB_A1_1G;
            break;
        case CPSS_PORT_SPEED_2500_E:
            if((CPSS_LIONCUB_REV_A0_E == rev) || (lion48RdBoard == GT_TRUE))
                return GT_NOT_SUPPORTED;
            lioncubInitArray = lioncubInitArrayDB_A1_2_5G;
            break;
        case CPSS_PORT_SPEED_10000_E:
            switch (rev)
            {
                CPSS_COVERITY_NON_ISSUE_BOOKMARK
                /* coverity[dead_error_condition] */
                /* coverity[dead_error_begin] */
                case CPSS_LIONCUB_REV_A0_E:
                    if (lion48RdBoard == GT_TRUE)
                    {
                        lioncubInitArray = lioncubInitArrayRD_A0;
                    }
                    else
                    {
                        lioncubInitArray = lioncubInitArrayDB_A0;
                    }
                    break;
                case CPSS_LIONCUB_REV_A1_E:
                    if (lion48RdBoard == GT_TRUE)
                    {
                        /* init Lioncub to default - 10G for RD board */
                        lioncubInitArray = lioncubInitArrayRD_A1_10G;
                    }
                    else
                    {
                        /* init Lioncub to default - 10G */
                        lioncubInitArray = lioncubInitArrayDB_A1_10G;
                    }
                break;
                default:
                    return GT_NOT_SUPPORTED;
            }
            break;
        default:
            return GT_NOT_SUPPORTED;
    }

    return lioncubInitPort(devNum,portNum,lioncubInitArray);
}

/*******************************************************************************
* gtAppDemoLionPortModeSpeedSet
*
* DESCRIPTION:
*       Example of configuration sequence of different ports interface modes
*       and speeds
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       ifMode   - Interface mode.
*       speed    - port speed
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
*       At this moment supported:
*           CPSS_PORT_INTERFACE_MODE_RXAUI_E        10G
*           CPSS_PORT_INTERFACE_MODE_XGMII_E (XAUI) 10G, 20G
*           CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E  10G
*           CPSS_PORT_INTERFACE_MODE_SGMII_E        10M, 100M, 1G, 2.5G
*           CPSS_PORT_INTERFACE_MODE_1000BASE_X_E   1G
*           CPSS_PORT_INTERFACE_MODE_XLG_E          40G
*
*******************************************************************************/
GT_STATUS gtAppDemoLionPortModeSpeedSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS  rc;
    GT_U32     lanesBmp;
    CPSS_PORT_INTERFACE_MODE_ENT ifModeTmp;
    CPSS_PORT_SPEED_ENT  speedForLionCub = CPSS_PORT_SPEED_10000_E;
    CPSS_DXCH_CFG_DEV_INFO_STC   devInfo;

    rc = cpssDxChCfgDevInfoGet(devNum, &devInfo);
    if(rc != GT_OK)
    {
        return rc;
    }

    if(devInfo.genDevInfo.devFamily != CPSS_PP_FAMILY_DXCH_LION_E)
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    if((ifMode == CPSS_PORT_INTERFACE_MODE_RXAUI_E) && (speed == CPSS_PORT_SPEED_10000_E))
    {
        lanesBmp = 0x3;
    }
    else if((ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) || (ifMode == CPSS_PORT_INTERFACE_MODE_1000BASE_X_E))
    {
        if(speed <= CPSS_PORT_SPEED_1000_E)
        {
            if((ifMode == CPSS_PORT_INTERFACE_MODE_1000BASE_X_E) && (speed < CPSS_PORT_SPEED_1000_E))
                return GT_BAD_PARAM;

            speedForLionCub = CPSS_PORT_SPEED_1000_E;
        }
        else if(speed == CPSS_PORT_SPEED_2500_E)
        {
            if(ifMode == CPSS_PORT_INTERFACE_MODE_1000BASE_X_E)
                return GT_BAD_PARAM;

            speedForLionCub = CPSS_PORT_SPEED_2500_E;
        }
        else
            return GT_NOT_SUPPORTED;

        lanesBmp = 0x1;
    }
    else if(ifMode == CPSS_PORT_INTERFACE_MODE_XGMII_E)
    {
        /* usually no lionCub on line-cards supporting XAUI, but UT's can run
           on RXAUI line-cards */
        speedForLionCub = (CPSS_PORT_SPEED_10000_E == speed) ? 
                            CPSS_PORT_SPEED_2500_E : CPSS_PORT_SPEED_10000_E;

        lanesBmp = 0xf;
    }
    else if(ifMode == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E)
    {
        /* although it's XAUI just 2 local serdeses of port used */
        lanesBmp = 0x3;
    }
    else if(ifMode == CPSS_PORT_INTERFACE_MODE_HX_E)
    {
        if((speed == CPSS_PORT_SPEED_5000_E) || (speed == CPSS_PORT_SPEED_10000_E))
            lanesBmp = 0x3;
        else
            return GT_NOT_SUPPORTED;
    }
    else if(ifMode == CPSS_PORT_INTERFACE_MODE_XLG_E)
    {
        /* usually no lionCub on line-cards supporting XAUI, but UT's can run
           on RXAUI line-cards */
        speedForLionCub = CPSS_PORT_SPEED_2500_E;

        lanesBmp = 0xff; /* 8 serdes (4 ports) used by XLG */
    }
    else
    {
        return GT_NOT_SUPPORTED;
    }

    if((rc = gtAppDemoLioncubPortSpeedSet(devNum, portNum, speedForLionCub)) != GT_OK)
        return rc;

    /* 1. Set XG MAC to reset <MACResetn> =0x0.
        – Register offset 0x8800000 bit [1].
        2. Set XPCS to reset < PCSReset_> = 0x0.
        – Register offset 0x8800200 bit [0].
        3. Set SERDES to Reset <sd_reset_in_> = 0x0.
        – Register offset 0x9800004 and 0x9800404 bit [3] - two SERDES per RXAUI interface.
        – Register offset 0x9800010 bits [5:0] - one bit per port.
        4. Set MAC to 1G <MacModeSelect> = 0x0.
        – Register offset 0x880001C bits [15:13].

        5. see bellow in cpssDxChPortSerdesPowerStatusSet

        6. Set media interface to XAUI <media_interface_mode> = 0x0.
            – In 1G mode only even bits are relevant.
            – Register offset 0x38C bits [23:0] - bit per SERDES.

        15. Configure 1G MAC.
        – Set port type to SGMII <PortType> =0x0
        register offset 0xA800000 bit [1].
    */
    if((rc = cpssDxChPortInterfaceModeSet(devNum, portNum, ifMode)) != GT_OK)
        return rc;

    if((ifMode == CPSS_PORT_INTERFACE_MODE_LOCAL_XGMII_E) && (portNum%2 != 0))
    {
        if((rc = cpssDxChPortInterfaceModeGet(devNum, (GT_U8)(portNum-1), &ifModeTmp)) != GT_OK)
            return rc;

        if(ifModeTmp == CPSS_PORT_INTERFACE_MODE_XGMII_E)
            return GT_OK;
    }

    if((rc = cpssDxChPortSpeedAutoNegEnableSet(devNum,portNum,GT_FALSE)) != GT_OK)
        return rc;

    if((rc = cpssDxChPortSpeedSet(devNum, portNum, speed)) != GT_OK)
        return rc;

    if((rc = cpssDxChPortDuplexAutoNegEnableSet(devNum,portNum,GT_FALSE)) != GT_OK)
        return rc;

    if((rc = cpssDxChPortDuplexModeSet(devNum, portNum, CPSS_PORT_FULL_DUPLEX_E)) != GT_OK)
        return rc;

    /* 5. Set mode to 10bit <SDSel10bitMode> = 0x1.
        – Register offset 0x9800000 bit [15].
        in prvCpssDxChLpPreInitSerdesSequence

        6. see above

        7. De-assert SERDES Reset <sd_reset_in_> = 0x1.
        – Bit [3] of the register at offset 0x9800004.

        8. Delay 10 ms.
         in prvCpssDxChLpPreInitSerdesSequence

        9. SERDES Power up sequence (one SERDES per 1G port - even SERDESes)
            - see Lion SGMII settings inTable 1, SERDES Settings for 1.25G
            (SGMII Mode), on page 9.

        10. Set SERDES Init <SDRxInit> = 0x1.
        – Bit [4] of the register at offset 0x9800004.
        11. Set SERDES Init <SDRxInit> = 0x0.
        – Bit [4] of the register at offset 0x9800004.
        12. Delay 10 ms.
        13. De-assert Reset <SERDES Miscellaneous> = 0x1.
        – Register offset 0x9800010 bits [5:0] - one bit per port.
        14. Delay 10 ms.
            In prvCpssDxChPortLionSerdesPowerUpSet

        15.
            – Enable port <PortEn> = 0x1
            register offset 0xA800000 bit [0].
            – Un-reset port <PortMacReset> = 0x0
            register offset 0xA800008 bit [6].
        16. Delay 5 ms.
    */

    if((rc = cpssDxChPortSerdesPowerStatusSet(devNum, portNum, CPSS_PORT_DIRECTION_BOTH_E,
                                                lanesBmp, GT_TRUE)) != GT_OK)
    {
        return rc;
    }

    if((ifMode == CPSS_PORT_INTERFACE_MODE_SGMII_E) && (speed <= CPSS_PORT_SPEED_1000_E))
    {
        if((rc = cpssDxChPortInBandAutoNegBypassEnableSet(devNum,portNum,GT_FALSE)) != GT_OK)
        {
            return rc;
        }

        if((rc = cpssDxChPortInbandAutoNegEnableSet(devNum,portNum,GT_TRUE)) != GT_OK)
        {
            return rc;
        }
    }

    /* In 1000BaseX mode to get link with smartBits, or other partner that doesn't suppport
     * inband autoneg, InBandAutoNegBypass must be enabled
     */
    if(ifMode == CPSS_PORT_INTERFACE_MODE_1000BASE_X_E)
    {
        if((rc = cpssDxChPortInBandAutoNegBypassEnableSet(devNum,portNum,GT_TRUE)) != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}
/*******************************************************************************
* gtDbDxLionBoardCleanDbDuringSystemReset
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
static GT_STATUS gtDbDxLionBoardCleanDbDuringSystemReset
(
    IN  GT_U8   boardRevId
)
{
    /* next static are and maybe changed from compilation time */
    dxChInitRegDefaults = GT_FALSE;

    return GT_OK;
}


/*******************************************************************************
* gtDbDxLion48XgBoardReg
*
* DESCRIPTION:
*       Registration function for the Lion 48XG ports board .
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
GT_STATUS gtDbDxLion48XgBoardReg
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
    boardCfgFuncs->boardCleanDbDuringSystemReset= gtDbDxLionBoardCleanDbDuringSystemReset;
    /*boardCfgFuncs->boardGetUserPorts            = getUserPorts;*/


    switch(boardRevId)
    {
        case REV_ID_AUTO_128K_E:
        case REV_ID_CALLER_CHOICE_E:
        case REV_ID_AUTO_64K_E:
        case REV_ID_ENHANCED_AUTO_128K_E:
        case REV_ID_ENHANCED_AUTO_64K_E:
#ifndef EXMXPM_FAMILY
            /* support of multi port group fdb lookup */
            return gtDbDxLion_ForMultiCoreFdbLookup(boardRevId,boardCfgFuncs);
#endif /*EXMXPM_FAMILY*/
        default:
            break;
    }


    return GT_OK;
}

/* debug functions */
GT_STATUS gtDbDxLion48XgIsLinkUp (int port)
{
	void	*context = (void*)0;
    unsigned short ret;

	ret = SFPhyLasiIsLinkUp(context,
                            port,
                            XAUI_INTERFACE,
                            SPEED_RES_1GIG_FD);
/*	
	cpssOsPrintf("\n %s, link:%d \n\r", __func__, ret);
*/		
    					
	return ((ret == 0) || (ret == 1)) ? GT_OK : GT_FAIL;
}

GT_STATUS gtDbDxLion48XgForce1G (int port)
{
	void	*context = (void*)0;

	SFPhyEnableSpeeds(context,
                      port,
                      SPEED_1GIG,
                      1);
						
	return GT_OK;
} 
