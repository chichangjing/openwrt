/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDbDxBobcat2PhyConfig.c
*
* DESCRIPTION:
*       Initialization board phy config for the Bobcat2 - SIP5 - board.
*
* FILE REVISION NUMBER:
*       $Revision: 10$
*
*******************************************************************************/
#include <appDemo/boardConfig/appDemoBoardConfig.h>
#include <appDemo/boardConfig/appDemoCfgMisc.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfigDefaults.h>
#include <appDemo/sysHwConfig/gtAppDemoTmConfig.h>
#include <appDemo/sysHwConfig/appDemoDb.h>
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>
#include <appDemo/boardConfig/gtDbDxBobcat2PhyConfig.h>
#include <cpss/generic/smi/cpssGenSmi.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>

#include <cpss/dxCh/dxChxGen/port/PortMapping/prvCpssDxChPortMappingShadowDB.h>

extern CPSS_TM_SCENARIO   appDemoTmScenarioMode;

/*--------------------------------------
 *  Phy configuration on each board
 *--------------------------------------
* DB
  +------+-------------+---------+--------+
  |  SMI |   address   | pp port | PHY    |
  +------+-------------+---------+--------+
  |  0   |     4-F     | 24 - 35 | 3x1548 | (quad-phy : 4 ports)
  |  1   |     4-F     | 36 - 47 | 3x1548 |
  |  2   |     4-F     |  0 - 11 | 3x1548 |
  |  3   |     4-F     | 12 - 23 | 3x1548 |
  +------+-------------+---------+--------+

* RD MSI
  +------+-------------+---------+--------+
  |  SMI |   address   | pp port | PHY    |
  +------+-------------+---------+--------+
  |  0   |    0-7,     |  0 - 7  |  1680  | (octal-phy : 8 ports)
  |  0   |    8-F      |  8 - 15 |  1680  | (octal-phy : 8 ports)
  |  1   |   10-17     | 16 - 23 |  1680  |
  |  2   |    0-7      | 24 - 31 |  1680  |
  |  2   |    8-F      | 32 - 39 |  1680  |
  |  3   |   10-17     | 40 - 47 |  1680  |
  |  3   |     0       |   62    |  1512  |
  |  3   |     1       |   63    |  1512  |
  +------+-------------+---------+--------+
* RD MTL (????)
  +------+-------------+---------+--------+
  |  SMI |   address   | pp port | PHY    |
  +------+-------------+---------+--------+
  |  0   |    0-7,     |  0 - 7  |  1680  | (octal-phy : 8 ports)
  |  0   |    8-F      |  8 - 15 |  1680  | (octal-phy : 8 ports)
  |  1   |   10-17     | 16 - 23 |  1680  |
  |  2   |    0-7      | 24 - 31 |  1680  |
  |  2   |    8-F      | 32 - 39 |  1680  |
  |  3   |   10-17     | 40 - 47 |  1680  |
  +------+-------------+---------+--------+

  Algo.
  1.    Distinguish between DB board and RD board  by reading Phy Vendor ID from SMI = 0 smiAddress(PHY) = 4 phyReg = 2
        (from first common port on DB and on RD boards)
  2.    if read fails -->
          if ASIC SIMULATION is defined
               determine boardType by boardRevId
          else
               unknown board --> DB by definition
  3.    If vendor is not MRVL  ---> unknown board --> DB
  4.    read phy id (from phy reg 3)
  5.    if id == 1548 --> DB board
  6.    if id == 1680 --> RD board , but which one MSI or MTL
  6.1.      read DMI = 3 smiAddr(PHY) = 1 phy id
  6.2.      if read fails --> No phy found --> MTL board
  6.2.      if phy Id == 1512 --> MSI board
  6.3.      otherise unknown phy , by default RD MSI board
  7.    otherwise unknown phy --> DB board.


  */


#define PHY_VENDOR_ID_REG_ADDR_CNS                2
#define PHY_ID_REG_ADDR_CNS                       3
#define PHY_ID_MASK_CNS                           0xFFF0
#define PHY_REV_MASK_CNS                          0x000F

/* Marvell PHYs unique vendor ID - register 2 */
#define MRVL_PHY_UID_CNS 0x141

/* PHY ID  register 3*/
#define PHY_1512_ID_CNS  0x0DD0
#define PHY_1548M_ID_CNS 0x0EC0
#define PHY_1680M_ID_CNS 0x0EE0


#define PHY_1548M_NUM_OF_PORTS_CNS 4
#define PHY_1680M_NUM_OF_PORTS_CNS 8
#define PHY_1512_NUM_OF_PORTS_CNS  1

/* use this defines for DEBUG purpose PHY configuration data print */
#define APP_DEMO_BOARD_TYPE_GET_DEBUG             0
#define APP_DEMO_PHY_CONFIG_DEBUG                 0
#define APP_DEMO_PHY_CONFIG_POLLING_DEBUG         0
#define APP_DEMO_PHY_CONFIG_POLLING_WRITE_DEBUG   0
#define APP_DEMO_PHY_CONFIG_DETAIL_DEBUG          0


GT_STATUS bobcat2BoardPhyVendorIdGet
(
    IN   GT_U8                      devNum,
    IN   CPSS_PHY_SMI_INTERFACE_ENT smiIf,
    IN   GT_U32                     smiAddr,
    OUT  GT_U16                    *vendorIdPtr
)
{
    GT_STATUS rc;
    /* read vendor ID */
    /* cpssSmiRegisterReadShort(devNum, portGroupsBmp =1 , smiInterface = CPSS_PHY_SMI_INTERFACE_0_E, smiAddr = 4 regAddr = 2*/
    rc = cpssSmiRegisterReadShort(devNum, 1, smiIf, smiAddr, PHY_VENDOR_ID_REG_ADDR_CNS, /*OUT*/vendorIdPtr);
    if (rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssSmiRegisterReadShort", rc);
        return rc;
    }
    return GT_OK;
}


GT_STATUS bobcat2BoardPhyIdGet
(
    IN   GT_U8                      devNum,
    IN   CPSS_PHY_SMI_INTERFACE_ENT smiIf,
    IN   GT_U32                     smiAddr,
    OUT  GT_U16                    *phyIdPtr
)
{
    GT_STATUS rc;
    /* read PHY ID */
    /* cpssSmiRegisterReadShort(devNum, portGroupsBmp =1 , smiInterface = CPSS_PHY_SMI_INTERFACE_0_E, smiAddr = 4 regAddr = 2*/
    #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
        cpssOsPrintf("\n       smi IF = %2d smiAddr = %2d regAddr = %2d",smiIf,smiAddr,PHY_ID_REG_ADDR_CNS);
    #endif

    rc = cpssSmiRegisterReadShort(devNum, 1, smiIf, smiAddr, PHY_ID_REG_ADDR_CNS, phyIdPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
        cpssOsPrintf("\n       Phy ID = 0x%4X",*phyIdPtr);
    #endif

    if (*phyIdPtr != 0xFFFF)
    {
        *phyIdPtr = (*phyIdPtr) & PHY_ID_MASK_CNS;
    }

    return GT_OK;
}


/*******************************************************************************
* bobkBoardTypeGet
*
* DESCRIPTION:
*       This function recognizes type of BobK board.
*       User Defined Register[3] is updated by UBOOT with board type value.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       bc2BoardTypePtr - pointer to board type.
*                         0 - DB board
*                         1 - RD board
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bobkBoardTypeGet
(
    IN   GT_U8    devNum,
    IN   GT_U8    boardRevID,
    OUT  GT_U32  *bc2BoardTypePtr
)
{
    GT_U32    value;

    if(appDemoDbEntryGet("bc2BoardType", &value) == GT_OK)
    {
        *bc2BoardTypePtr = value;
        return GT_OK;
    }

#ifndef ASIC_SIMULATION

#ifdef _linux

    GT_STATUS rc;

    rc = extDrvBoardIdGet(bc2BoardTypePtr);

    if (rc != GT_OK)
    {
        CPSS_ENABLER_DBG_TRACE_RC_MAC("extDrvBoardIdGet", rc);
        return rc;
    }

    if (*bc2BoardTypePtr == 0xFFFB)
    {
        switch(appDemoPpConfigList[devNum].deviceId)
        {
            case CPSS_BOBK_CAELUM_DEVICES_CASES_MAC:
                *bc2BoardTypePtr = APP_DEMO_CAELUM_BOARD_DB_CNS;
                break;
            case CPSS_BOBK_CETUS_DEVICES_CASES_MAC:
                *bc2BoardTypePtr = APP_DEMO_CETUS_BOARD_DB_CNS;
                break;
            default:
                cpssOsPrintf("\r\nbobkBoardTypeGet: illlegal device id 0x%x\r\n", appDemoPpConfigList[devNum].deviceId);
                return GT_BAD_PARAM;
        }
    }
    
#else

    cpssOsPrintf("\r\nbobkBoardTypeGet: can not identify board type\r\n");
    return GT_BAD_STATE;

#endif /* _linux */

#else
    switch(appDemoPpConfigList[devNum].deviceId)
    {
        case CPSS_BOBK_CAELUM_DEVICES_CASES_MAC:
            *bc2BoardTypePtr = APP_DEMO_CAELUM_BOARD_DB_CNS;
            break;
        case CPSS_BOBK_CETUS_DEVICES_CASES_MAC:
            *bc2BoardTypePtr = APP_DEMO_CETUS_BOARD_DB_CNS;
            break;
        default:
            cpssOsPrintf("\r\nbobkBoardTypeGet: illlegal device id 0x%x\r\n", appDemoPpConfigList[devNum].deviceId);
            return GT_BAD_PARAM;
    }
#endif

    return GT_OK;
}


GT_BOOL g_userForceBoardType = GT_FALSE;
GT_U32  g_userForcedBoardType = 0;

GT_STATUS userForceBoardTypeCaelum(GT_VOID)
{
    g_userForceBoardType = GT_TRUE;
    g_userForcedBoardType = APP_DEMO_CAELUM_BOARD_DB_CNS;
    return GT_OK;
}

/*******************************************************************************
* bobcat2BoardTypeGet
*
* DESCRIPTION:
*       This function recognizes type of Bobcat2 board.
*       The function reads PHY type for SMI#0 address 4 (port 24 on DB board,
*       port 4 on RD board).
*       If PHY is not connected or PHY ID is 1548M the board is DB.
*       Otherwise the board is RD.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       bc2BoardTypePtr - pointer to board type.
*                         0 - DB board
*                         1 - RD board
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bobcat2BoardTypeGet
(
    IN   GT_U8    devNum,
    IN   GT_U8    boardRevID,
    OUT  GT_U32  *bc2BoardTypePtr
)
{
    GT_STATUS rc;                /* return status*/
    GT_U16    phyVendorId = 0xFFFF;  /* PHY register data,
                                    init value is used for simulation that
                                    does not support cpssSmiRegisterReadShort */
    GT_U16    phyId;

    if (g_userForceBoardType == GT_TRUE)
    {
        *bc2BoardTypePtr = g_userForcedBoardType;
        return GT_OK;
    }

    #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
         cpssOsPrintf("\nbobcat2BoardTypeGet() revId = %d\n",boardRevID);
    #endif

    switch(appDemoPpConfigList[devNum].deviceId)
    {
        case CPSS_BOBK_CAELUM_DEVICES_CASES_MAC:
        case CPSS_BOBK_CETUS_DEVICES_CASES_MAC:
            return bobkBoardTypeGet(devNum, boardRevID, bc2BoardTypePtr);
        default:
            break;
    }

    /* default is DB board */
    *bc2BoardTypePtr = APP_DEMO_BC2_BOARD_DB_CNS;

    if (boardRevID == BOARD_REV_ID_RDMTL_E || boardRevID == BOARD_REV_ID_RDMTL_TM_E) /* MTL board */
    {
        *bc2BoardTypePtr = APP_DEMO_BC2_BOARD_RD_MTL_CNS;
        return GT_OK;
    }

    /* check PHY ID that is connected to SMI#0 address 4 - port 24 on DB board, port 4 on RD board */
    /* phy at address 4 prestents both at DB and RD boards, read vendor ID from it */
    #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
        cpssOsPrintf("\n   Read vendor Id");
    #endif

    rc = bobcat2BoardPhyVendorIdGet(devNum, CPSS_PHY_SMI_INTERFACE_0_E, /*smiAddr*/4, /*OUT*/&phyVendorId);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (phyVendorId != MRVL_PHY_UID_CNS)
    {
        /* the PHY is not connected - DB board */
        #ifdef ASIC_SIMULATION
            if (boardRevID == BOARD_REV_ID_DB_E || boardRevID == BOARD_REV_ID_DB_TM_E)
            {
                #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
                     cpssOsPrintf("\n   ASIC SIMULATION : DB");
                #endif

                *bc2BoardTypePtr = APP_DEMO_BC2_BOARD_DB_CNS;
                return GT_OK;
            }
            if (boardRevID == BOARD_REV_ID_RDMSI_E  || boardRevID == BOARD_REV_ID_RDMSI_TM_E )
            {
                #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
                     cpssOsPrintf("\n   ASIC SIMULATION : RD MSI");
                #endif
                *bc2BoardTypePtr = APP_DEMO_BC2_BOARD_RD_MSI_CNS;
                return GT_OK;
            }
            if (boardRevID == BOARD_REV_ID_RDMTL_E  || boardRevID == BOARD_REV_ID_RDMTL_TM_E )
            {
                #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
                     cpssOsPrintf("\n   ASIC SIMULATION : RD MTL");
                #endif
                *bc2BoardTypePtr = APP_DEMO_BC2_BOARD_RD_MTL_CNS;
                return GT_OK;
            }
        #else
            #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
                 cpssOsPrintf("\n   NOT MRVL PHY : DB board");
            #endif
            *bc2BoardTypePtr = APP_DEMO_BC2_BOARD_DB_CNS;
            boardRevID = boardRevID;
            return GT_OK;
        #endif
    }
    /*--------------------------------------------------------------------------*/
    /* phy at address 4 prestents both at DB and RD boards, read Phy ID from it */
    /*--------------------------------------------------------------------------*/
    rc = bobcat2BoardPhyIdGet(devNum, CPSS_PHY_SMI_INTERFACE_0_E, /*smiAddr */4, /*OUT*/&phyId);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (phyId  == PHY_1548M_ID_CNS)
    {
        /* the PHY is 1548M - DB board */
        #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
                cpssOsPrintf("\n   Phy  PHY_1548M : DB");
        #endif

        *bc2BoardTypePtr = APP_DEMO_BC2_BOARD_DB_CNS;
        return GT_OK;
    }
    if (phyId == PHY_1680M_ID_CNS)   /* RD board , but don't know which RD MSI or MTL */
    {
        #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
            cpssOsPrintf("\n   Phy 1680M : RD, yet don't know which one : MSI or MTL");
            cpssOsPrintf("\n   Trying to read ID of 1512 Phy...");
        #endif
        /* ---------------------------------------------------------------------*/
        /* get PHY vendor ID smi 3 addr 0  , at RD MSI shall be placed PHY 1512 */
        /* ---------------------------------------------------------------------*/
        rc = bobcat2BoardPhyIdGet(devNum, CPSS_PHY_SMI_INTERFACE_3_E, /*smiAddr */0, /*OUT*/&phyId);
        if (rc != GT_OK) /* we failed read  ---> MTL board */
        {
            #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
                cpssOsPrintf("\n   failed read 1512 Phy ID -->  RD MTL");
            #endif

            *bc2BoardTypePtr = APP_DEMO_BC2_BOARD_RD_MTL_CNS;
            return GT_OK;
        }
        if (phyId == 0xFFFF)  /* no phy present --> MTL board */
        {
            #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
                cpssOsPrintf("\n   failed read 1512 Phy ID -->  RD MTL");
            #endif
            *bc2BoardTypePtr = APP_DEMO_BC2_BOARD_RD_MTL_CNS;
            return GT_OK;
        }
        if (phyId == PHY_1512_ID_CNS)
        {
            #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
                cpssOsPrintf("\n   read 1512 Phy ID -->  MSI");
            #endif
            *bc2BoardTypePtr = APP_DEMO_BC2_BOARD_RD_MSI_CNS;
            return GT_OK;
        }
        else /* default RD board  ??? */
        {
            cpssOsPrintf("\n+-----------------------------------------------+");
            cpssOsPrintf("\n| Warning : Undefined Phy ID  -->  RD MSI board |");
            cpssOsPrintf("\n+-----------------------------------------------+");
            *bc2BoardTypePtr = APP_DEMO_BC2_BOARD_RD_MSI_CNS;
            return GT_OK;
        }
    }
    cpssOsPrintf("\n+-----------------------------------------------+");
    cpssOsPrintf("\n| Warning : Undefined MRVL Phy --> DB board     |");
    cpssOsPrintf("\n+-----------------------------------------------+");
    *bc2BoardTypePtr = APP_DEMO_BC2_BOARD_DB_CNS;
    return GT_OK;
}



/*******************************************************************************
 * Global variables
 ******************************************************************************/


/*
 * typedef: struct QUAD_PHY_CFG_STC
 *
 * Description: PHY Configuration entry
 *
 * Fields:
 *    allPorts           - Configure all 4 ports of PHY
 *                             AD_FIRST_PORT  - configure only port 0 of PHY
 *                             AD_ALL_PORTS   - configure all  ports on phy
 *                             AD_BAD_PORT    - bad port , may be used for end of sequence determination
 *    phyRegAddr         - Address of PHY register
 *    phyRegData         - Data to write to a PHY register
 *    sleepTime          - sleep after write in ms
 *
 * Comments:
 */

typedef enum GT_PHY_WRITE_TYPE_ENT
{
    AD_FIRST_PORT = 0
   ,AD_ALL_PORTS  = 1
   ,AD_BAD_PORT  = -1
}GT_PHY_WRITE_TYPE_ENT;

typedef struct
{
    GT_PHY_WRITE_TYPE_ENT allPorts;
    GT_U8                 phyRegAddr;
    GT_U16                phyRegData;
}QUAD_PHY_CFG_STC;

typedef struct
{
    QUAD_PHY_CFG_STC *seqPtr;
    GT_U32            sleepTimeAfter_ms;
}QUAD_PHY_CFG_STC_PHASE_STC;


#define BAD_PHY_ADDR 0xFFFF

static QUAD_PHY_CFG_STC  macSecPHY_88E1548P_RevA0_Init_Array_phase_1[] =
{
    /* for all/first port, regaddr, regvalue, delay if need in ms */
    { AD_ALL_PORTS,           22, 0x0000 } /* RW u1 P0-3 R22 H0000 */
   ,{ AD_ALL_PORTS,           4,  0x01E1 } /* RW u1 P0-3 R4 H01E1 - restore default of register #4
                                              that may be overridden by BC2 B0 during power-up */
    /*---------------------------------------*/
    /* MACSEC disable                        */
    /*---------------------------------------*/
   ,{ AD_ALL_PORTS,           22, 0x0012 } /* RW u1 P0-3 R22 H0012 */
   ,{ AD_ALL_PORTS,           27, 0x0000 } /* RW u1 P0-3 R27 H0000 */
#if 0     /* currently not relevenat for MACSEC disable */
    /*---------------------------------------*/
    /* PHY EEE Initialization                */
    /*---------------------------------------*/
   ,{ AD_ALL_PORTS,           22, 0x00FF } /* RW u1 P0-3 R22 h00FF */
   ,{ AD_ALL_PORTS,           17, 0x2148 } /* RW u1 P0-3 R17 h2148 */
   ,{ AD_ALL_PORTS,           16, 0x2144 } /* RW u1 P0-3 R16 h2144 */
   ,{ AD_ALL_PORTS,           17, 0xDC0C } /* RW u1 P0-3 R17 hDC0C */
   ,{ AD_ALL_PORTS,           16, 0x2159 } /* RW u1 P0-3 R16 h2159 */
#endif
    /* ---------------------------------------*/
    /* QSGMII Amp                             */
    /* ---------------------------------------*/
   ,{ AD_ALL_PORTS,           22, 0x00FD }  /* RW u1 P0-3 R22 h00FD */
   ,{ AD_ALL_PORTS,           11, 0x1D70 }  /* RW u1 P0-3 R11 h1D70 */
    /* ---------------------------------------*/
    /* QSGMII block power down/up WA - ?      */
    /* ---------------------------------------*/
   ,{ AD_ALL_PORTS,           22, 0x0004 }  /* RW u1 P0-3 R22 h0004 */
   ,{ AD_ALL_PORTS,           26, 0x3802 }  /* RW u1 P0-3 R26 h3802 , Wait 300ms */
   ,{  AD_BAD_PORT,            0, 0x0000 }  /* end seq */
};

static QUAD_PHY_CFG_STC  macSecPHY_88E1548P_RevA0_Init_Array_phase_2[] =
{
    { AD_ALL_PORTS,           26, 0x3002 }  /* RW u1 P0-3 R26 h3002 , Wait 200ms */
   ,{  AD_BAD_PORT,            0, 0x0000 }  /* end seq */
};

static QUAD_PHY_CFG_STC  macSecPHY_88E1548P_RevA0_Init_Array_phase_3[] =
{
    /*--------------------------------------------------*/
    /* Incorrect Q_ANEG configuration errata workaround */
    /*--------------------------------------------------*/
    {  AD_ALL_PORTS,           22, 0x0004 } /* RW u1 P0-3 R22 h0004 */
   ,{  AD_ALL_PORTS,            0, 0x9140 } /* RW u1 P0-3 R0  h9140 */
    /* ---------------------------------------*/
    /* Soft-Reset                             */
    /* ---------------------------------------*/
   ,{ AD_ALL_PORTS,           22, 0x0000 }/* RW u1 P0-3 R22 H0000 */
   ,{ AD_ALL_PORTS,           16, 0x3060 }/* RW u1 P0-7 R16 H3060 */ /* Disable Energy Detect - bits8:9 = 0 to make VCT working */
   ,{ AD_ALL_PORTS,            0, 0x9140 }/* RW u1 P0-3 R0  H9140 */
   ,{  AD_BAD_PORT,            0, 0x0000 }  /* end seq */
};

QUAD_PHY_CFG_STC_PHASE_STC macSecPHY_88E1548P_RevA0_Init_Array[] =
{
     { &macSecPHY_88E1548P_RevA0_Init_Array_phase_1[0], 300 }
    ,{ &macSecPHY_88E1548P_RevA0_Init_Array_phase_2[0], 200 }
    ,{ &macSecPHY_88E1548P_RevA0_Init_Array_phase_3[0],   0 }
    ,{ NULL                                           ,   0 }
};



static QUAD_PHY_CFG_STC  macSec_PTP_disable_PHY_88E1680M_RevA0_RD_MSI_Init_Array [] =
{
    { AD_ALL_PORTS,  22, 0x0000 } /* RW u1 P0-7 R22 H0000 */ /* ensure we on page 0 */
   ,{ AD_ALL_PORTS,   4, 0x01E1 } /* RW u1 P0-3 R4 H01E1 - restore default of register #4
                                     that may be overridden by BC2 B0 during power-up */
   /* ---------------------------------------*/
   /* Link indication config                 */
   /* ---------------------------------------*/
   ,{ AD_ALL_PORTS,  22, 0x0003 } /* RW u1 P0-7 R22 h0003 */
   ,{ AD_ALL_PORTS,  16, 0x1771 } /* RW u1 P0-7 R16 h1771 */
   /* ---------------------------------------*/
   /* MACSec and PTP disable                 */
   /* ---------------------------------------*/
   ,{ AD_ALL_PORTS,  22, 0x0012 } /* RW u1 P0-7 R22 h0012 */
   ,{ AD_ALL_PORTS,  27, 0x0000 } /* RW u1 P0-7 R27 h0000 */
   /* ---------------------------------------*/
   /* QSGMII Tx Amp change                   */
   /* ---------------------------------------*/
   ,{ AD_ALL_PORTS,  22, 0x00FD } /* RW u1 P0-7 R22  h00FD */
   ,{ AD_ALL_PORTS,   8, 0x0B53 } /* RW u1 P0-7 R8   h0B53 */
   ,{ AD_ALL_PORTS,   7, 0x200D } /* RW u1 P0-7 R7   h200D */
#if 0     /* currently not relevenat for MACSEC disable */
    /*---------------------------------------*/
    /* PHY EEE Initialization                */
    /*---------------------------------------*/
   ,{ AD_ALL_PORTS,  22, 0x00FF } /* RW u1 P0-7 R22 h00FF */
   ,{ AD_ALL_PORTS,  17, 0xB030 } /* RW u1 P0-7 R17 hB030 */
   ,{ AD_ALL_PORTS,  16, 0x215C } /* RW u1 P0-7 R16 h215C */
#endif
   /* ---------------------------------------*/
   /* Power Down disable & Soft-Reset        */
   /* ---------------------------------------*/
   ,{ AD_ALL_PORTS,  22, 0x0000 } /* RW u1 P0-7 R22 H0000 */
   ,{ AD_ALL_PORTS,  16, 0x3070 } /* RW u1 P0-7 R16 H3070 */ /* PHY power up (reg 0x10_0.2=0 and reg 0x0_0.11=0) */
   ,{ AD_ALL_PORTS,   0, 0x9140 } /* RW u1 P0-7 R0  H9140 */ /* Soft Reset */
};


static QUAD_PHY_CFG_STC  macSec_PTP_disable_PHY_88E1680M_RevA0_RD_MTL_Init_Array [] =
{
    /* for all/first port, regaddr, regvalue, delay if need in ms */
    { AD_ALL_PORTS,  22, 0x0000 } /* ensure we on page 0 */
   ,{ AD_ALL_PORTS,   4, 0x01E1 } /* RW u1 P0-3 R4 H01E1 - restore default of register #4
                                              that may be overridden by BC2 B0 during power-up */
   ,{ AD_ALL_PORTS,  16, 0x3370 } /* PHY power up (reg 0x10_0.2=0 and reg 0x0_0.11=0) */
   ,{ AD_ALL_PORTS,   0, 0x1140 }
   ,{ AD_ALL_PORTS,  22, 0x0003 } /* Link indication config */
   ,{ AD_ALL_PORTS,  16, 0x1117 }
   ,{ AD_ALL_PORTS,  22, 0x0004 } /* Link indication config */
   ,{ AD_ALL_PORTS,  27, 0x3FA0 }
   ,{ AD_ALL_PORTS,  22, 0x0000 }
   ,{ AD_ALL_PORTS,  22, 0x0012 } /* MACSec and PTP disable */
   ,{ AD_ALL_PORTS,  27, 0x0000 }
   ,{ AD_ALL_PORTS,  22, 0x0000 }
   ,{ AD_ALL_PORTS,  22, 0x00FD } /* QSGMII Tx Amp change */
   ,{ AD_ALL_PORTS,   8, 0x0B53 }
   ,{ AD_ALL_PORTS,   7, 0x200D }
   ,{ AD_ALL_PORTS,  22, 0x00FF } /* EEE Initialization */
   ,{ AD_ALL_PORTS,  17, 0xB030 }
   ,{ AD_ALL_PORTS,  16, 0x215C }
   ,{ AD_ALL_PORTS,  22, 0x0000 }
   ,{ AD_ALL_PORTS,   0, 0x9140 }  /* Soft Reset */
};


/*-------------------------------
 * //-------------------------------------------------------
 * // new algorithm : delay after phase after all group of ports
 * //-------------------------------------------------------
 * for phase = 0:N
 *     for phy:0:N
 *        firstPort = f(phy);
 *        lastPort  = f(phy);
 *        for line = 0 ; lastLine
 *           Config line for First port
 *           if config all ports
 *               for port=firstPort+1:lastPort
 *                  Config line for port
 *               end
 *            end
 *        end
 *     end
 *     delayAfterPhase
 * end
 *
 * //-------------------------------------------------------
 * // old algorithm : delay after phase after group of ports
 * //-------------------------------------------------------
 * for phy:0:N-1
 *     for phase = 0:N-1
 *        for port=firstPort:firstPort+4
 *            for line = 0 ; lastLine
 *            end
 *        end
 *        delayAfterPhase
 *     end
 * end
 *-------------------------------------*/



/*******************************************************************************
* macSecPHY_88E1548P_RevA0_Init
*
* DESCRIPTION:
*  Function applies to 88E1548P MACSec Rev A0 PHY Initialization
*  This function is used as example to perform phy and macsec initialization for Port 0-3
*  - MACSec Initialization
*  - PHY Initialization
*
* INPUTS:
*       devNum          - device number
*       firstPort       - first port of PP with 88E1548P PHY connected
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
static GT_STATUS macSecPHY_88E1548P_RevA0_Init
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  totalPortNum
)
{
    GT_STATUS       rc;           /* return code */
    GT_U32          phaseId;
    GT_U32          ii;           /* iterator */
    GT_U32          numOfPorts;   /* number of ports for configuration */
    GT_U32          ppPort;       /* port of PFY device */
    GT_U8           phyRegAddr;   /* PHY reg address */
    GT_U16          phyRegData;   /* PHY reg data */
    QUAD_PHY_CFG_STC *currentArrayPtr;
    QUAD_PHY_CFG_STC_PHASE_STC * phasePtr;
    GT_PHYSICAL_PORT_NUM firstPort;


    for (phaseId = 1, phasePtr = &macSecPHY_88E1548P_RevA0_Init_Array[0]; phasePtr->seqPtr != NULL; phaseId++, phasePtr++)
    {
        #if (APP_DEMO_PHY_CONFIG_DEBUG == 1)
            cpssOsPrintf("\n   macSecPHY_88E1548P_RevA0_Init() phase config [%d]",phaseId);
        #endif

        for (firstPort  = 0; firstPort  < totalPortNum; firstPort += PHY_1548M_NUM_OF_PORTS_CNS)
        {
            CPSS_ENABLER_PORT_SKIP_CHECK(devNum, firstPort);

            currentArrayPtr = phasePtr->seqPtr;
            for (ii = 0; currentArrayPtr->allPorts != AD_BAD_PORT; ii++, currentArrayPtr++)  /* loop over commands */
            {
                /* get number of PHY ports to config */
                if (currentArrayPtr->allPorts == AD_ALL_PORTS)
                {
                    numOfPorts = totalPortNum-firstPort;
                    if (numOfPorts > PHY_1548M_NUM_OF_PORTS_CNS)
                    {
                        numOfPorts = PHY_1548M_NUM_OF_PORTS_CNS;
                    }
                }
                else /* AD_FIRST_PORT */
                {
                    numOfPorts = 1;
                }

                for (ppPort = firstPort; ppPort < firstPort + numOfPorts; ppPort++)
                {
                    phyRegAddr = (GT_U8)currentArrayPtr->phyRegAddr;
                    phyRegData = currentArrayPtr->phyRegData;

                    #if (APP_DEMO_PHY_CONFIG_DEBUG == 1)
                        if (ii == 0)
                        {
                            cpssOsPrintf("\n        PP phase = %d port[%d] Reg[%d] Data[%04X] ... ", phaseId, ppPort,phyRegAddr, phyRegData);
                        }
                    #endif

                    rc = cpssDxChPhyPortSmiRegisterWrite(devNum, ppPort, phyRegAddr, phyRegData);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiRegisterWrite", rc);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintf("\n-->ERROR : macSecPHY_88E1548P_RevA0_Init() failure: phase = %2d PP port[%d] Reg[%d] Data[%04X]\n", phaseId,ppPort, phyRegAddr, phyRegData);
                        return rc;
                    }
                    #if (APP_DEMO_PHY_CONFIG_DEBUG == 1)
                        if (ii == 0)
                        {
                            cpssOsPrintf("Done");
                        }
                    #endif
                }
            }
        }
        /* whether there is a need for a delay */
        if (phasePtr->sleepTimeAfter_ms > 0)
        {
            cpssOsTimerWkAfter(phasePtr->sleepTimeAfter_ms);
        }
        #if (APP_DEMO_PHY_CONFIG_DEBUG == 1)
            cpssOsPrintf("\n   Done");
        #endif

    }
    return GT_OK;
}


/*******************************************************************************
* macSec_PTP_disable_PHY_88E1680M_RevA0_Init
*
* DESCRIPTION:
*  Function applies to 88E1680M Rev A0 PHY Initialization with MACSec and PTP disabled.
*  This function is used as example to perform phy and macsec initialization for Port 0-7
*  - MACSec and PTP Disable
*  - PHY Initialization
*
* INPUTS:
*       devNum          - device number
*       firstPort       - first port of PP with 88E1680M PHY connected
*       numOfConfigs    - number of configurations
*       configArrayPtr  - pointer to PHY configuration array
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
static GT_STATUS macSec_PTP_disable_PHY_88E1680M_RevA0_Init
(
    IN  GT_U8                 devNum,
    IN  GT_PHYSICAL_PORT_NUM  firstPort,
    IN  GT_U32                numOfConfigs,  /* number of configurations */
    IN  QUAD_PHY_CFG_STC     *configArrayPtr /* pointer to PHY configuration array */

)
{
    GT_STATUS       rc;           /* return code */
    GT_U32          ii;           /* iterator */
    GT_U32          numOfPorts;   /* number of ports for configuration */
    GT_U32          phyPort;      /* port of PFY device */
    GT_PHYSICAL_PORT_NUM ppPort;  /* port of PP for PHY configuration */
    GT_U8           phyRegAddr;   /* PHY reg address */
    GT_U16          phyRegData;   /* PHY reg data */


    #if (APP_DEMO_PHY_CONFIG_DEBUG == 1)
        cpssOsPrintf("\n    macSec_PTP_disable_PHY_88E1680M_RevA0_Init() first port [%2d]",firstPort);
    #endif

    for (ii = 0; ii < numOfConfigs; ii++)
    {
        #if (APP_DEMO_PHY_CONFIG_DETAIL_DEBUG == 1)
            cpssOsPrintf("\n        macSec_PTP_disable_PHY_88E1680M_RevA0_Init() config[%d]",ii);
        #endif
        /* get number of PHY ports to config */
        numOfPorts = (configArrayPtr[ii].allPorts == AD_ALL_PORTS)? 8 : 1;
        for (phyPort = 0; phyPort < numOfPorts; phyPort++)
        {
            ppPort = firstPort + phyPort;

            CPSS_ENABLER_PORT_SKIP_CHECK(devNum,ppPort);

            phyRegAddr = (GT_U8)configArrayPtr[ii].phyRegAddr;
            phyRegData = configArrayPtr[ii].phyRegData;
            #if (APP_DEMO_PHY_CONFIG_DETAIL_DEBUG == 1)
                cpssOsPrintf("\n            PP port[%d] Reg[%d] Data[%04X] ... ", ppPort,phyRegAddr, phyRegData);
            #endif
            rc = cpssDxChPhyPortSmiRegisterWrite(devNum, ppPort, phyRegAddr, phyRegData);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiRegisterWrite", rc);
            if(rc != GT_OK)
            {
                cpssOsPrintf("--> ERROR : macSec_PTP_disable_PHY_88E1680M_RevA0_Init failure: PP port[%d] Reg[%d] Data[%04X]\n", ppPort, phyRegAddr, phyRegData);
                return rc;
            }
            #if (APP_DEMO_PHY_CONFIG_DETAIL_DEBUG == 1)
                cpssOsPrintf("Done");
            #endif
        }
        #if (APP_DEMO_PHY_CONFIG_DETAIL_DEBUG == 1)
            cpssOsPrintf("\n        Done");
        #endif
    }
    return GT_OK;
}



/*******************************************************************************
* PHY_Board_RD_88E1512_RevA0_Init
*
* DESCRIPTION:
*  Function applies to 88E1512 Rev A0 PHY Initialization
*  This function is used as example to perform phy
*  - PHY Initialization
*
* INPUTS:
*       devNum          - device number
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
typedef struct
{
    CPSS_PHY_SMI_INTERFACE_ENT smiIf;
    GT_U32                     regAddr;
    GT_U16                     regData;
}PHY_board_RD_88E1512_STC;

static PHY_board_RD_88E1512_STC prv_board_RD_phy881512[] =
{
     { CPSS_PHY_SMI_INTERFACE_3_E, 0x16, 0x0012 } /* goto page 18d */
    ,{ CPSS_PHY_SMI_INTERFACE_3_E, 0x14, 0x0201 } /* reserved 0x4 + SGMII to Copper mode */
    ,{ CPSS_PHY_SMI_INTERFACE_3_E, 0x14, 0x8201 } /* reset + reserved 0x4 + SGMII to Copper mode */
    ,{ CPSS_PHY_SMI_INTERFACE_3_E, 0x16, 0x0000 } /* return to page 0 */
};

static GT_STATUS PHY_Board_RD_88E1512_RevA0_Init
(
    IN  GT_U8                 devNum
)
{
    GT_STATUS rc;
    GT_U32 i;
    GT_U32 smiAddr;
    PHY_board_RD_88E1512_STC * phyDataPtr;

    for(smiAddr = 0; smiAddr < 2; smiAddr++)
    {
        #if (APP_DEMO_PHY_CONFIG_DEBUG == 1)
            cpssOsPrintf("\n    PHY_Board_RD_88E1512_RevA0_Init() smiaddr [%2d] ... ",smiAddr);
        #endif

        phyDataPtr = &prv_board_RD_phy881512[0];
        for (i = 0 ; i < sizeof(prv_board_RD_phy881512)/sizeof(prv_board_RD_phy881512[0]); i++, phyDataPtr++)
        {
                                                     /*       smi-intface, smi-addr,            reg-addr, data */
            rc = cpssSmiRegisterWriteShort(devNum, 0x1, phyDataPtr->smiIf,  smiAddr, phyDataPtr->regAddr,phyDataPtr->regData);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssSmiRegisterWriteShort", rc);
            if(rc != GT_OK)
            {
                cpssOsPrintf("--> ERROR : PHY 88E1512 failure: smiAddr [%d] Reg[0x%2X] Data[0x%04X]\n", smiAddr, phyDataPtr->regAddr, phyDataPtr->regData);
                return rc;
            }
        }
        #if (APP_DEMO_PHY_CONFIG_DEBUG == 1)
            cpssOsPrintf("Done");
        #endif

    }
    return GT_OK;
}

/*******************************************************************************
* boardPhyConfigRDBoardPollingSet
*
* DESCRIPTION:
*       board Phy RD Board Polling Set
*
* INPUTS:
*       devNum          - device number
*       portNum         - portNum
*       pollingStatus   - TRUE/FALSE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS boardPhyConfigRDBoardPollingSet
(
    IN GT_U8       devNum,
    IN GT_U32      portNum,
    IN GT_BOOL     pollingStatus
)
{
    /*-------------------------------------------------------*/
    /* SMI IF 0  <--> ports  0-15                            */
    /* SMI IF 1  <--> ports 16-23                            */
    /* SMI IF 2  <--> ports 24-39                            */
    /* SMI IF 3  <--> ports 40-47 + ports 82/83 at default mapping, see mapping for details */
    /*                                   IF(62/63)           */
    /*-------------------------------------------------------*/
    GT_PHYSICAL_PORT_NUM  firstPortOnSMIIFArr[] = {  0, 16, 24, 40 };
    GT_U32                portIdx;
    GT_STATUS             rc = GT_OK; /* return code */
    GT_PHYSICAL_PORT_NUM  bc2PortNum; /* PP first port number for quad PHY configuration */
    #if (APP_DEMO_PHY_CONFIG_POLLING_DEBUG == 1)
        GT_CHAR * boolStr[] = { "FALSE","TRUE" };
    #endif

    /* for (bc2PortNum = 0; bc2PortNum < portNum; bc2PortNum++) */
    /* stop polling per SMI IF */
    for (portIdx = 0 ; portIdx < sizeof(firstPortOnSMIIFArr)/sizeof(firstPortOnSMIIFArr[0]); portIdx++)
    {
        bc2PortNum = firstPortOnSMIIFArr[portIdx];
        if (bc2PortNum >= portNum) /* don't process after max number of ports */
        {
            break;
        }
        #if (APP_DEMO_PHY_CONFIG_POLLING_DEBUG == 1)
            cpssOsPrintf("\n        cpssDxChPhyAutonegSmiSet() bc2PortNum=%2d %s... ",bc2PortNum,boolStr[pollingStatus]);
        #endif
        rc= cpssDxChPhyAutonegSmiSet(devNum,bc2PortNum,pollingStatus);
        if(rc != GT_OK)
        {
            cpssOsPrintf("\n        -->ERROR : rc=%d\n", rc);
            return rc;
        }
        #if (APP_DEMO_PHY_CONFIG_POLLING_DEBUG == 1)
            cpssOsPrintf("Done");
        #endif
    }
    return GT_OK;
}



GT_STATUS bobcat2BoardPhyConfigRDBoardInitSequenceGet
(
    IN  GT_U32             bc2BoardType,
    OUT GT_U32            *numOfConfigsPtr,  /* number of configurations */
    OUT QUAD_PHY_CFG_STC **configArrayPtrPtr /* pointer to PHY configuration array */

)
{
    switch (bc2BoardType)
    {
        case APP_DEMO_BC2_BOARD_RD_MSI_CNS:
            *numOfConfigsPtr   = sizeof(macSec_PTP_disable_PHY_88E1680M_RevA0_RD_MSI_Init_Array)/sizeof(macSec_PTP_disable_PHY_88E1680M_RevA0_RD_MSI_Init_Array[0]);
            *configArrayPtrPtr = &macSec_PTP_disable_PHY_88E1680M_RevA0_RD_MSI_Init_Array[0];
        break;
        case APP_DEMO_BC2_BOARD_RD_MTL_CNS:
            *numOfConfigsPtr    = sizeof(macSec_PTP_disable_PHY_88E1680M_RevA0_RD_MTL_Init_Array)/sizeof(macSec_PTP_disable_PHY_88E1680M_RevA0_RD_MTL_Init_Array[0]);
            *configArrayPtrPtr  = &macSec_PTP_disable_PHY_88E1680M_RevA0_RD_MTL_Init_Array[0];
        break;
        default:
        {
            return GT_NOT_SUPPORTED;
        }
    }
    return GT_OK;
}

/*******************************************************************************
* bobcat2BoardPhyConfig
*
* DESCRIPTION:
*       Board specific PHY configurations after board initialization.
*
* INPUTS:
*       boardRevId      - The board revision Id.
*       devNum          - device number
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
GT_STATUS bobcat2BoardPhyConfig
(
    IN  GT_U8       boardRevId,
    IN  GT_U8       devNum
)
{
    GT_STATUS             rc = GT_OK; /* return code */
    GT_PHYSICAL_PORT_NUM  bc2PortNum; /* PP first port number for quad PHY configuration */
    GT_U32                portNum;
    GT_U32                bc2BoardType;
    GT_U32                numOfConfigs;  /* number of configurations */
    QUAD_PHY_CFG_STC     *configArrayPtr; /* pointer to PHY configuration array */


    #if (APP_DEMO_PHY_CONFIG_DEBUG == 1)
        cpssOsPrintf("\nBoard Phy Config revID = %d",boardRevId);
    #endif

    /* configure QSGMII ports PHY registers */
    if (appDemoTmScenarioMode==CPSS_TM_2 || appDemoTmScenarioMode==CPSS_TM_3 || appDemoTmScenarioMode==CPSS_TM_8)
        portNum=10;
    else
        portNum=48;

    if (   boardRevId == BOARD_REV_ID_DB_E     || boardRevId == BOARD_REV_ID_DB_TM_E
        || boardRevId == BOARD_REV_ID_RDMTL_E  || boardRevId == BOARD_REV_ID_RDMTL_TM_E
        || boardRevId == BOARD_REV_ID_RDMSI_E  || boardRevId == BOARD_REV_ID_RDMSI_TM_E)
    {
        rc = bobcat2BoardTypeGet(devNum,boardRevId, /*OUT*/&bc2BoardType);
        if(rc != GT_OK)
        {
            return rc;
        }


        if ((bc2BoardType == APP_DEMO_BC2_BOARD_DB_CNS) ||
            (bc2BoardType == APP_DEMO_CAELUM_BOARD_DB_CNS))
        {/* DB board - Quad PHYs */

            if(PRV_CPSS_DXCH_PP_MAC(devNum)->genInfo.devFamily == CPSS_PP_FAMILY_DXCH_BOBCAT3_E)
            {
                cpssOsPrintf("\n skip PHY CONFIG DB board ... \n");
                /* 1. the ports are 10G so no real meaning to init PHY of GIG ports */
                /* 2. The function macSecPHY_88E1548P_RevA0_Init() fail when
                      GM init only 12 ports of DP 0 */
                return GT_OK;
            }


            cpssOsPrintf("\nPHY CONFIG DB board ...");
            /* configure first 48 ports */
            rc = macSecPHY_88E1548P_RevA0_Init(devNum, portNum);
            if(rc != GT_OK)
            {
                cpssOsPrintf("\n--->ERROR : bobcat2BoardPhyConfig() at  macSecPHY_88E1548P_RevA0_Init()");
                return rc;
            }
            cpssOsPrintf("Done\n");
        }
        else if (bc2BoardType == APP_DEMO_BC2_BOARD_RD_MSI_CNS || bc2BoardType == APP_DEMO_BC2_BOARD_RD_MTL_CNS)
        {/* RD board - Octal PHYs */
            GT_BOOL status;

            if (bc2BoardType == APP_DEMO_BC2_BOARD_RD_MSI_CNS)
            {
                cpssOsPrintf("\nPHY CONFIG RD MSI board ...");
            }
            else
            {
                cpssOsPrintf("\nPHY CONFIG RD MTL board ...");
            }


            rc = bobcat2BoardPhyConfigRDBoardInitSequenceGet(bc2BoardType,/*OUT*/&numOfConfigs,&configArrayPtr);
            if(rc != GT_OK)
            {
                cpssOsPrintf("\n-->ERROR : bobcat2BoardPhyConfigRDBoardInitSequenceGet() failure\n");
                return rc;
            }

            /* Get current polling status */
            rc= cpssDxChPhyAutonegSmiGet(devNum,0,&status);
            if(rc != GT_OK)
            {
                cpssOsPrintf("\n-->ERROR : cpssDxChPhyAutonegSmiGet() failure\n");
                return rc;
            }

            if(status)
            {
                /* stop polling */
                #if (APP_DEMO_PHY_CONFIG_POLLING_DEBUG == 1)
                    cpssOsPrintf("\n    boardPhyConfigRDBoardPollingSet(GT_FALSE) ... ");
                #endif
                #if (APP_DEMO_PHY_CONFIG_POLLING_WRITE_DEBUG == 1)
                    appDemoTraceHwAccessEnable(devNum,APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E,GT_TRUE);
                #endif

                rc= boardPhyConfigRDBoardPollingSet(devNum,portNum,GT_FALSE);
                if(rc != GT_OK)
                {
                    cpssOsPrintf("\n-->ERROR : boardPhyConfigRDBoardPollingSet(GT_FALSE)\n");
                    return rc;
                }
                #if (APP_DEMO_PHY_CONFIG_POLLING_WRITE_DEBUG == 1)
                    appDemoTraceHwAccessEnable(devNum,APP_DEMO_TRACE_HW_ACCESS_TYPE_WRITE_E,GT_FALSE);
                #endif

                #if (APP_DEMO_PHY_CONFIG_POLLING_DEBUG == 1)
                    cpssOsPrintf("\n    Done");
                #endif
            }

            /* configure first 48 ports */
            for (bc2PortNum = 0; bc2PortNum < portNum; bc2PortNum +=8)
            {
                CPSS_ENABLER_PORT_SKIP_CHECK(devNum, bc2PortNum);
                rc = macSec_PTP_disable_PHY_88E1680M_RevA0_Init(devNum, bc2PortNum,numOfConfigs,configArrayPtr);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("macSec_PTP_disable_PHY_88E1680M_RevA0_Init", rc);
                if(rc != GT_OK)
                {
                    cpssOsPrintf("\n-->ERROR : boardPhyConfig() failure: PP port[%d]\n", bc2PortNum);
                    return rc;
                }
            }

            if (bc2BoardType == APP_DEMO_BC2_BOARD_RD_MSI_CNS)
            {
                rc = PHY_Board_RD_88E1512_RevA0_Init(devNum);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("PHY_Board_RD_88E1512_RevA0_Init", rc);
                if(rc != GT_OK)
                {
                    cpssOsPrintf("\n-->ERROR : boardPhyConfig() failure: PHY_Board_RD_88E1512_RevA0_Init()\n");
                    return rc;
                }
            }
            if(status)
            {
                /* restart polling */
                #if (APP_DEMO_PHY_CONFIG_POLLING_DEBUG == 1)
                    cpssOsPrintf("\n    boardPhyConfigRDBoardPollingSet(GT_TRUE) ... ");
                #endif
                rc= boardPhyConfigRDBoardPollingSet(devNum,portNum,GT_TRUE);
                if(rc != GT_OK)
                {
                    cpssOsPrintf("\n-->ERROR : boardPhyConfigRDBoardPollingSet(GT_TRUE)\n");
                    return rc;
                }
                #if (APP_DEMO_PHY_CONFIG_POLLING_DEBUG == 1)
                    cpssOsPrintf("\n    Done");
                #endif
            }
            #if (APP_DEMO_BOARD_TYPE_GET_DEBUG == 1)
                    cpssOsPrintf("\n");
            #endif

            cpssOsPrintf("Done\n");
            /* wait 1 second for PHYs reset */
            cpssOsTimerWkAfter(1000);
        }
        else if (APP_DEMO_CETUS_BOARD_LEWIS_RD_CNS == bc2BoardType)
        {
            /* According to MTL here is too late to initialize Lewis' PHY 
             * Initialized by mv_3240_init at beginning of configBoardAfterPhase2
             */
        } 
        else 
        {
            cpssOsPrintf("\n-->ERROR : boardPhyConfig() failure: undefined board type%d\n",bc2BoardType);
            return GT_NOT_SUPPORTED;
        }
    }
    else
    {
        cpssOsPrintf("\n-->ERROR : boardPhyConfig() : revId % is not supported\n", boardRevId);
        return GT_NOT_SUPPORTED;
    }
    return rc;
}


/*******************************************************************************
* bobkExternalBoardPhyConfig
*
* DESCRIPTION:
*       External PHY configuration.
*
* INPUTS:
*       devNum          - device number
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
GT_STATUS bobkExternalBoardPhyConfig
(
    IN  GT_U8       devNum,
    IN  GT_U32      phyType,
    IN  GT_U32      smiInterface,
    IN  GT_U32      smiStartAddr
)
{
    GT_STATUS rc;           /* return code */
    GT_U32 i, j, phaseId;   /* loop iterators */
    GT_U32 numOfConfigs;    /* number of PHY init sequence configurations */
    GT_U32 phyRegAddr;      /* PHY address */
    GT_U16 phyRegData;      /* PHY data */
    QUAD_PHY_CFG_STC *currentArrayPtr;
    QUAD_PHY_CFG_STC_PHASE_STC * phasePtr;
    

    if(phyType == 0)/* PHY1680M */
    {
        numOfConfigs = sizeof(macSec_PTP_disable_PHY_88E1680M_RevA0_RD_MSI_Init_Array)/sizeof(macSec_PTP_disable_PHY_88E1680M_RevA0_RD_MSI_Init_Array[0]);

        for(i = 0; i < PHY_1680M_NUM_OF_PORTS_CNS; i++)
        {
            for(j = 0; j < numOfConfigs; j++)
            {
                phyRegAddr = macSec_PTP_disable_PHY_88E1680M_RevA0_RD_MSI_Init_Array[j].phyRegAddr;
                phyRegData = macSec_PTP_disable_PHY_88E1680M_RevA0_RD_MSI_Init_Array[j].phyRegData;

                cpssOsPrintf("SMI_[%d]  SMI_addr_[%d]  PHY_REG=0x%04x  PHY_DATA=0x%04x\r\n", smiInterface, (smiStartAddr + i), phyRegAddr, phyRegData);

                rc = cpssSmiRegisterWriteShort(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, smiInterface, (smiStartAddr + i), phyRegAddr, phyRegData);
                if(rc != GT_OK)
                {
                    cpssOsPrintf("\r\nEXT_PHY_ERROR: error in SMI write for SMI_[%d]  SMI_addr_[%d]  PHY_REG 0x%x \r\n", smiInterface, (smiStartAddr + i), phyRegAddr);
                    return rc;
                }
            }

            cpssOsPrintf("\r\n\r\n");
        }
    }
    else if(phyType == 1)/* PHY1548P */
    {
        for (phaseId = 1, phasePtr = &macSecPHY_88E1548P_RevA0_Init_Array[0]; phasePtr->seqPtr != NULL; phaseId++, phasePtr++)
        {
            currentArrayPtr = phasePtr->seqPtr;
            for (i = 0; currentArrayPtr->allPorts != AD_BAD_PORT; i++, currentArrayPtr++)  /* loop over commands */
            {
                for (j = 0; j < PHY_1548M_NUM_OF_PORTS_CNS; j++)
                {
                    phyRegAddr = (GT_U8)currentArrayPtr->phyRegAddr;
                    phyRegData = currentArrayPtr->phyRegData;

                    cpssOsPrintf("SMI_[%d]  SMI_addr_[%d]  PHY_REG=0x%04x  PHY_DATA=0x%04x\r\n", smiInterface, (smiStartAddr + j), phyRegAddr, phyRegData);

                    rc = cpssSmiRegisterWriteShort(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, smiInterface, (smiStartAddr + j), phyRegAddr, phyRegData);
                    CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssDxChPhyPortSmiRegisterWrite", rc);
                    if(rc != GT_OK)
                    {
                        cpssOsPrintf("\n-->ERROR : bobkExternalBoardPhyConfig() failure: phase = %2d PP port[%d] Reg[%d] Data[%04X]\n", phaseId,i, phyRegAddr, phyRegData);
                        return rc;
                    }
                }
            }
            cpssOsPrintf("\r\n\r\n");

            /* whether there is a need for a delay */
            if (phasePtr->sleepTimeAfter_ms > 0)
            {
                cpssOsTimerWkAfter(phasePtr->sleepTimeAfter_ms);
            }
        }
    }
    else
    {
        cpssOsPrintf("\r\nEXT_PHY_ERROR: illegal PHY type - %d\r\n", phyType);
        return GT_BAD_PARAM;
    }

    return GT_OK;
}
