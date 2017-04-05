/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtDb98ExMxPm3Config.c
*
* DESCRIPTION:
*       Initialization functions for the ExMxPm3 boards.
*
*
* FILE REVISION NUMBER:
*       $Revision: 121 $
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
#include <cpss/exMxPm/exMxPmGen/port/PizzaArbiter/cpssExMxPmPortPizzaArbiter.h>
#include <cpss/exMxPm/exMxPmGen/port/PizzaArbiter/private/prvCpssExMxPmPortPizzaArbiterInterface.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricConfig.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricVoq.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricXbar.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricHGLink.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricTxqQueueMap.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricTxqDevMap.h>
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricTxqHeaderMerger.h>
#include <cpss/exMxPm/exMxPmGen/fabric/private/prvCpssExMxPmFabricInfo.h>
#include <cpss/exMxPm/exMxPmGen/cpssHwInit/private/prvCpssExMxPmHwInit.h>
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortCtrl.h>
#include <cpss/exMxPm/exMxPmGen/intTcam/cpssExMxPmIntTcam.h>
#include <cpss/exMxPm/exMxPmGen/phy/cpssExMxPmPhySmi.h>
#include <cpss/exMxPm/exMxPmGen/cpssHwInit/cpssExMxPmLedCtrl.h>
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgGen.h>
#include <cpss/exMxPm/exMxPmGen/vb/cpssExMxPmVb.h>
#include <cpss/exMxPm/exMxPmGen/diag/cpssExMxPmDiag.h>
#include <cpssDriver/pp/config/prvCpssDrvPpCfgInit.h>
#include <cpss/generic/systemRecovery/cpssGenSystemRecovery.h>
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortPfc.h>

/* pizza arbiter files */
#include <cpss/exMxPm/exMxPmGen/port/PizzaArbiter/cpssExMxPmPortPizzaArbiter.h>

#include <appDemo/utils/extTcam/appDemoExMxPmExternalTcamConfig.h>
#include <appDemo/utils/port/appDemoExMxPmPortUtils.h>
#include <mvHwsPortInitIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <serdes/mvHwsSerdesIf.h>
#include <private/mvHwsPortMiscIf.h>
#include <private/mvPortModeElements.h>

#include <ddr3/mvHwsDdr3InitIf.h>
#include <ddr3/private/mvDdr3PrvIf.h>
#include <ddr3/private/mvDdr3Regs.h>
#include <ddr3/private/mvDdr3StaticCfg.h>
#include <cpss/exMxPm/exMxPmGen/phy/cpssExMxPmPhySmi.h>
#include <cpss/generic/smi/cpssGenSmi.h>

#ifdef ASIC_SIMULATION
#include <asicSimulation/SCIB/scib.h>
#endif /*ASIC_SIMULATION*/


#define TC_MAX  8 /* original code used 7 */

#define CPSS_EXMXPM3_FABRIC_PORTS_BMP_INDEX   1 /* Ports 32,36,40,41,48,52,56,57 are in index 1 of array*/

#define CPSS_EXMXPM3_PORT_TX_HIGH_SPEED_PROFILE     7

/* Set custom configuration according to revision before configuration starts*/
#define CPSS_EXMXPM3_CUSTOM_CONFIG(devNum,boardRevId) ((PRV_CPSS_PP_MAC(devNum)->revision > 0)?                      \
                                                        appDemoExMxPmCustomBoardConfigurationsRevB[boardRevId - 1]:      \
                                                        appDemoExMxPmCustomBoardConfigurationsRevA[boardRevId - 1])



#ifdef GM_USED
#ifdef  INCLUDE_UTF
GT_STATUS simulateExternalLoopbackPortSet
(
    IN  GT_U8       devNum,
    IN  GT_PHYSICAL_PORT_NUM portNum,
    IN GT_BOOL                   enable
);
#endif /*INCLUDE_UTF*/
#endif /*GM_USED*/

/*******************************************************************************
 * Internal variables
 ******************************************************************************/
/*
 * Typedef: enum APPDEMO_BOARD_TYPE
 *
 * Description: Defines the different target borad type values.
 *
 */
typedef enum
{
    APPDEMO_BOARD_TYPE_SLM_1230_E,/* Modular */
    APPDEMO_BOARD_TYPE_SLM_1236_E /* Stand Alone */
} APPDEMO_BOARD_TYPE;

/*
 * Typedef: enum APPDEMO_EMC_TYPE
 *
 * Description: Defines the different memory types.
 *
 */
typedef enum
{
   APPDEMO_EMC_TYPE_LOOKUP_E,
   APPDEMO_EMC_TYPE_FORWARD_E

}APPDEMO_EMC_TYPE;


/* static configuration data for dunit of LU IF, board SLM 1230, 800Mhg */
static APPDEMO_EXMXPM_EXT_DRAM_STATIC_CFG dunit_LU_SLM1230_800[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,        0xD,       0xF,        0x113},
    {1,        0,        0xD,       0x11,       0x313},
    {2,        0,        0xD,       0xF,        0x10E},
    {3,        0,        0xD,       0x11,       0x30E},
    {4,        0,        0xD,       0xF,        0x10E},
    {5,        0,        0xD,       0x11,       0x30E},
    {6,        0,        0xD,       0xF,        0x115},
    {7,        0,        0xD,       0x11,       0x315},
    {8,        0,        0xD,       0xF,        0x10E},
    {9,        0,        0xD,       0x11,       0x30E},
    {10,       0,        0xD,       0xF,        0x106},
    {11,       0,        0xD,       0x11,       0x306},
    {12,       0,        0xD,       0xF,        0x102},
    {13,       0,        0xD,       0x11,       0x302}
};

/* static configuration data for dunit of FWD IF, board SLM 1230, 800Mhg */
static APPDEMO_EXMXPM_EXT_DRAM_STATIC_CFG dunit_FWD_SLM1230_800[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,      0xD,         0xF,        0x100}, /* 0 */
    {0,        1,      0xD,         0xF,        0x100}, /* 0 */
    {1,        0,      0xD,         0x11,        0x21F}, /* 1 */
    {1,        1,      0xD,         0x11,        0x300}, /* 1 */
    {2,        0,      0xD,         0xF,        0x103}, /* 2 */
    {2,        1,      0xD,         0xF,        0x104}, /* 2 */
    {3,        0,      0xD,         0x11,        0x303}, /* 3 */
    {3,        1,      0xD,         0x11,        0x304}, /* 3 */
    {4,        0,      0xD,         0xF,        0x113},  /* 4 */
    {4,        1,      0xD,         0xF,        0x113},  /* 4 */
    {5,        0,      0xD,         0x11,        0x312},  /* 5 */
    {5,        1,      0xD,         0x11,        0x312},  /* 5 */
    {6,        0,      0xD,         0xF,        0x116},  /* 6 */
    {6,        1,      0xD,         0xF,        0x117},  /* 6 */
    {7,        0,      0xD,         0x11,        0x317},  /* 7 */
    {7,        1,      0xD,         0x11,        0x317},  /* 7 */
};

/* static configuration data for dunit of LU IF, board SLM 1236, 800Mhg */
static APPDEMO_EXMXPM_EXT_DRAM_STATIC_CFG dunit_LU_SLM1236_800[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,        0xC,       0x10,        0x30B},
    {1,        0,        0xD,       0xF,        0x10B},
    {2,        0,        0xC,       0x10,        0x307},
    {3,        0,        0xD,       0xF,        0x107},
    {4,        0,        0xC,       0x10,        0x309},
    {5,        0,        0xD,       0xF,        0x108},
    {6,        0,        0xC,       0x10,        0x310},
    {7,        0,        0xD,       0xF,        0x10E},
    {8,        0,        0xC,       0x10,        0x312},
    {9,        0,        0xD,       0xF,        0x113},
    {10,       0,        0xC,       0x10,        0x30C},
    {11,       0,        0xD,       0xF,        0x10B},
    {12,       0,        0xC,       0x10,        0x306},
    {13,       0,        0xD,       0xF,        0x106}
};

/* static configuration data for dunit of FWD IF, board SLM 1236, 800Mhg */
static APPDEMO_EXMXPM_EXT_DRAM_STATIC_CFG dunit_FWD_SLM1236_800[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,      0xC,         0x10,        0x307}, /* 0 */
    {0,        1,      0xC,         0x10,        0x305}, /* 0 */
    {1,        0,      0xD,         0xF,        0x106}, /* 1 */
    {1,        1,      0xD,         0xF,        0x105}, /* 1 */
    {2,        0,      0xC,         0x10,        0x310}, /* 2 */
    {2,        1,      0xC,         0x10,        0x30F}, /* 2 */
    {3,        0,      0xD,         0xF,        0x10F}, /* 3 */
    {3,        1,      0xD,         0xF,        0x10E}, /* 3 */
    {4,        0,      0xC,         0x10,        0x31D},  /* 4 */
    {4,        1,      0xC,         0x10,        0x31A},  /* 4 */
    {5,        0,      0xD,         0xF,        0x11A},  /* 5 */
    {5,        1,      0xD,         0xF,        0x118},  /* 5 */
    {6,        0,      0xC,         0x10,        0x314},  /* 6 */
    {6,        1,      0xC,         0x10,        0x311},  /* 6 */
    {7,        0,      0xD,         0xF,        0x112},  /* 7 */
    {7,        1,      0xD,         0xF,        0x112},  /* 7 */
};

/* static configuration data for dunit of LU IF, board SLM 1230, 667Mhg */
static APPDEMO_EXMXPM_EXT_DRAM_STATIC_CFG dunit_LU_SLM1230_667[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,        0xB,       0xD,        0x15},
    {1,        0,        0xB,       0xF,        0x215},
    {2,        0,        0xB,       0xD,        0X11},
    {3,        0,        0xB,       0xF,        0x211},
    {4,        0,        0xB,       0xD,        0x11},
    {5,        0,        0xB,       0xF,        0x211},
    {6,        0,        0xB,       0xD,        0x17},
    {7,        0,        0xB,       0xF,        0x217},
    {8,        0,        0xB,       0xD,        0x10},
    {9,        0,        0xB,       0xF,        0x210},
    {10,       0,        0xB,       0xD,        0xA},
    {11,       0,        0xB,       0xF,        0x20A},
    {12,       0,        0xB,       0xD,        0x7},
    {13,       0,        0xB,       0xF,        0x207}
};

/* static configuration data for dunit of FWD IF, board SLM 1230, 667Mhg */
static APPDEMO_EXMXPM_EXT_DRAM_STATIC_CFG dunit_FWD_SLM1230_667[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,      0xB,         0xD,        0x5}, /* 0 */
    {0,        1,      0xB,         0xD,        0x5}, /* 0 */
    {1,        0,      0xB,         0xF,        0x204}, /* 1 */
    {1,        1,      0xB,         0xF,        0x204}, /* 1 */
    {2,        0,      0xB,         0xD,        0x8}, /* 2 */
    {2,        1,      0xB,         0xD,        0x8}, /* 2 */
    {3,        0,      0xB,         0xF,        0x208}, /* 3 */
    {3,        1,      0xB,         0xF,        0x208}, /* 3 */
    {4,        0,      0xB,         0xD,        0x14},  /* 4 */
    {4,        1,      0xB,         0xD,        0x15},  /* 4 */
    {5,        0,      0xB,         0xF,        0x214},  /* 5 */
    {5,        1,      0xB,         0xF,        0x214},  /* 5 */
    {6,        0,      0xB,         0xD,        0x17},  /* 6 */
    {6,        1,      0xB,         0xD,        0x18},  /* 6 */
    {7,        0,      0xB,         0xF,        0x218},  /* 7 */
    {7,        1,      0xB,         0xF,        0x218},  /* 7 */
};

/* static configuration data for dunit of LU IF, board SLM 1236, 667Mhg */
static APPDEMO_EXMXPM_EXT_DRAM_STATIC_CFG dunit_LU_SLM1236_667[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,        0xA,       0xE,        0x219},
    {1,        0,        0xB,       0xD,        0x19},
    {2,        0,        0xA,       0xE,        0x215},
    {3,        0,        0xB,       0xD,        0x15},
    {4,        0,        0xA,       0xE,        0x217},
    {5,        0,        0xB,       0xD,        0x16},
    {6,        0,        0xA,       0xE,        0x21D},
    {7,        0,        0xB,       0xD,        0x1B},
    {8,        0,        0xA,       0xE,        0x21F},
    {9,        0,        0xB,       0xD,        0x1F},
    {10,       0,        0xA,       0xE,        0x21A},
    {11,       0,        0xB,       0xD,        0x19},
    {12,       0,        0xA,       0xE,        0x215},
    {13,       0,        0xB,       0xD,        0x14}
};

/* static configuration data for dunit of FWD IF, board SLM 1236, 667Mhg */
static APPDEMO_EXMXPM_EXT_DRAM_STATIC_CFG dunit_FWD_SLM1236_667[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,      0xA,         0xE,        0x215}, /* 0 */
    {0,        1,      0xA,         0xE,        0x214}, /* 0 */
    {1,        0,      0xB,         0xD,        0x14}, /* 1 */
    {1,        1,      0xB,         0xD,        0x13}, /* 1 */
    {2,        0,      0xA,         0xE,        0x21D}, /* 2 */
    {2,        1,      0xA,         0xE,        0x21C}, /* 2 */
    {3,        0,      0xB,         0xD,        0x1C}, /* 3 */
    {3,        1,      0xB,         0xD,        0x1B}, /* 3 */
    {4,        0,      0xA,         0xE,        0x308},  /* 4 */
    {4,        1,      0xA,         0xE,        0x305},  /* 4 */
    {5,        0,      0xB,         0xD,        0x105},  /* 5 */
    {5,        1,      0xB,         0xD,        0x104},  /* 5 */
    {6,        0,      0xA,         0xE,        0x300},  /* 6 */
    {6,        1,      0xA,         0xE,        0x21D},  /* 6 */
    {7,        0,      0xB,         0xD,        0x1F},  /* 7 */
    {7,        1,      0xB,         0xD,        0x1E},  /* 7 */
};


/*
 * typedef: struct APPDEMO_EXMXPM_QUEUE_MAPPER_CUSTOM_CFG_STC
 *
 * Description: Defines binding of a physical port to txq ports.
 *
 * Fields:
 *    physicalPort      - physical port number.
 *    downstreamTxqPort - upstream txq port.
 *    downWC            - upstream txq port WC mode (Puma3 B0 and above).
 *    upstreamTxqPort   - downstream txq port.
 *    upWC              - downstream txq port WC mode (Puma3 B0 and above).
 *    downDMA           - downstream DMA channel.
 *    upDMA             - downstream DMA channel.
 *    virtualPort       - virtual port number.
 */

typedef struct
{
    GT_U8   physicalPort;
    GT_U8   downstreamTxqPort;
    GT_BOOL downWC;
    GT_U8   upstreamTxqPort;
    GT_BOOL upWC;
    GT_U8   downDMA;
    GT_U8   upDMA;
    GT_U8   virtualPort;
} APPDEMO_EXMXPM_QUEUE_MAPPER_CUSTOM_CFG_STC;

/*
 * typedef: struct APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC
 *
 * Description: Defines custom pizza and queue mapper configuration.
 *
 * Fields:
 *    txqPizza      - (ptr to)Txq pizza array.
 *    pizzaSizes - sizes of each pizza array.
 *    queueMapperCfg   - queue mapper configuration.
 *
 */

typedef struct
{
    GT_U8  *txqPizza[PRV_CPSS_EXMXPM_PUMA3_PROCCESSING_CORES_NUM_CNS];    /* 4 TxQ pizzas*/
    const GT_U32 pizzaSizes[PRV_CPSS_EXMXPM_PUMA3_PROCCESSING_CORES_NUM_CNS];   /* 4 TxQ pizzas*/
    const APPDEMO_EXMXPM_QUEUE_MAPPER_CUSTOM_CFG_STC  *queueMapperCfg;
    GT_U8 *highSpeedArray;
    const GT_U8 highSpeedArraySize;
} APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC;



/**********************************************************************************
*         Init system 4
**********************************************************************************/
static const APPDEMO_EXMXPM_QUEUE_MAPPER_CUSTOM_CFG_STC INIT_SYSTEM_4_Queue_mapper[]=
{  /*Phy Downstream     WC      Upstream   WC         downDMA   upDMA   Virtual          */
    {0      , 0     , GT_FALSE , 32    , GT_TRUE  ,    0   ,    32   , 0xFF },
    {16     , 16    , GT_FALSE , 38    , GT_TRUE  ,   16   ,    32   , 0xFF },
    {31     , 31    , GT_TRUE  , 46    , GT_TRUE  ,   31   ,    32   , 0xFF },
    {15     , 15    , GT_FALSE , 47    , GT_FALSE ,   PRV_CPSS_EXMXPM_CPU_PORT_NUM_CNS   ,    32   , 0xFF },  /* CPU port*/
    {0xFF   ,0xFF   ,     0xFF ,0xFF   ,     0xFF , 0xFF   ,  0xFF   , 0xFF },
};

/* CP4R100G Pizza */
static GT_U8 INIT_SYSTEM_4_PIZZA_CORE_0 [] = {0,0,15,15}; /* 0-2, 15-2  cpu port is a high speed port*/
static GT_U8 INIT_SYSTEM_4_PIZZA_CORE_1 [] = {16,16,31,31}; /* 16-2 , 31-2   lb port is high speed port*/
static GT_U8 INIT_SYSTEM_4_PIZZA_CORE_2 [] = {32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,
                                         32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,
                                         32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,
                                         32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,
                                         32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,32,32,38,38,0xFF,0xFF,46,46,47,47}; /*32-96, 33-96, X-2, 46-2, 47-2*/

/* High speed array */
static GT_U8 INIT_SYSTEM_4_High_Speed_Arr[] = {0,15,16,31,32,38};

/* Register queue mapper and pizza configuration to board*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC INIT_SYSTEM_4= {
    {&INIT_SYSTEM_4_PIZZA_CORE_0[0],&INIT_SYSTEM_4_PIZZA_CORE_1[0],&INIT_SYSTEM_4_PIZZA_CORE_2[0],NULL},          /* Pizza configuration arrays*/
    {sizeof(INIT_SYSTEM_4_PIZZA_CORE_0),sizeof(INIT_SYSTEM_4_PIZZA_CORE_1),sizeof(INIT_SYSTEM_4_PIZZA_CORE_2),0}, /* Pizza configuration array sizes*/
    &INIT_SYSTEM_4_Queue_mapper[0],  &INIT_SYSTEM_4_High_Speed_Arr[0], sizeof(INIT_SYSTEM_4_High_Speed_Arr)       /*Queue mapper configuration and high speed ports array and size*/
};

/* CP4R100G Pizza Rev B*/
static GT_U8 INIT_SYSTEM_4_PIZZA_CORE_0_Rev_B [] = {0,0,0,0,0,0,0,0,0,0,15}; /* 0-10, 15-1  */
static GT_U8 INIT_SYSTEM_4_PIZZA_CORE_1_Rev_B [] = {16}; /* 16-1 */
static GT_U8 INIT_SYSTEM_4_PIZZA_CORE_2_Rev_B [] = {32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,
                                                    32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,32,38,47}; /*32,38-77, 47-1*/

/* High speed array Rev B*/
static GT_U8 INIT_SYSTEM_4_High_Speed_Arr_Rev_B[] = {0,16,31,32,38,46};


/* Register queue mapper and pizza configuration to board for Rev B0 and above*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC INIT_SYSTEM_4_RevB= {
    {&INIT_SYSTEM_4_PIZZA_CORE_0_Rev_B[0],&INIT_SYSTEM_4_PIZZA_CORE_1_Rev_B[0],&INIT_SYSTEM_4_PIZZA_CORE_2_Rev_B[0],NULL},          /* Pizza configuration arrays*/
    {sizeof(INIT_SYSTEM_4_PIZZA_CORE_0_Rev_B),sizeof(INIT_SYSTEM_4_PIZZA_CORE_1_Rev_B),sizeof(INIT_SYSTEM_4_PIZZA_CORE_2_Rev_B),0}, /* Pizza configuration array sizes*/
    &INIT_SYSTEM_4_Queue_mapper[0],  &INIT_SYSTEM_4_High_Speed_Arr_Rev_B[0], sizeof(INIT_SYSTEM_4_High_Speed_Arr_Rev_B)       /*Queue mapper configuration and high speed ports array and size*/
};


/**********************************************************************************
*         Init system 7
**********************************************************************************/
static const APPDEMO_EXMXPM_QUEUE_MAPPER_CUSTOM_CFG_STC INIT_SYSTEM_7_Queue_mapper[]=
{  /*Phy Downstream     WC      Upstream   WC         downDMA   upDMA   Virtual         */
    {0      , 0     , GT_FALSE , 32    , GT_FALSE ,    0   ,    32   , 0xFF },
    {16     , 16    , GT_FALSE , 38    , GT_FALSE ,   16   ,    32   , 0xFF },
    {31     , 31    , GT_TRUE  , 46    , GT_TRUE  ,   31   ,    32   , 0xFF },
    {15     , 15    , GT_FALSE , 47    , GT_FALSE ,   PRV_CPSS_EXMXPM_CPU_PORT_NUM_CNS   ,    32  , 0xFF }, /* CPU port*/
    {0xFF   ,0xFF   ,     0xFF ,0xFF   ,     0xFF , 0xFF   ,  0xFF   , 0xFF },
};

/* CP4R100G Pizza*/
static GT_U8 INIT_SYSTEM_7_PIZZA_CORE_0 [] = {0,0,15,15}; /* 0-2, 15-2  cpu port is a high speed port*/
static GT_U8 INIT_SYSTEM_7_PIZZA_CORE_1 [] = {16,16,31,31}; /* 16-2 , 31-2   lb port is high speed port*/
static GT_U8 INIT_SYSTEM_7_PIZZA_CORE_2 [] = {32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,
                                              40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,
                                              36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,
                                              32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,
                                              40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,32,32,36,36,40,40,47,47};/*32-64, 36-64, 40-64, 47-2*/
static GT_U8 INIT_SYSTEM_7_PIZZA_CORE_3 [] = {48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,
                                              56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,
                                              52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,
                                              48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,
                                              56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,48,48,52,52,56,56,63,63};/*48-64, 52-64, 56-64, 63-2*/

/* High speed array */
static GT_U8 INIT_SYSTEM_7_High_Speed_Arr[] = {0,15,16,31};

/* Register queue mapper and pizza configuration to board*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC INIT_SYSTEM_7= {
    {&INIT_SYSTEM_7_PIZZA_CORE_0[0],&INIT_SYSTEM_7_PIZZA_CORE_1[0],&INIT_SYSTEM_7_PIZZA_CORE_2[0],&INIT_SYSTEM_7_PIZZA_CORE_3[0]},          /* Pizza configuration arrays*/
    {sizeof(INIT_SYSTEM_7_PIZZA_CORE_0),sizeof(INIT_SYSTEM_7_PIZZA_CORE_1),sizeof(INIT_SYSTEM_7_PIZZA_CORE_2),sizeof(INIT_SYSTEM_7_PIZZA_CORE_3)}, /* Pizza configuration array sizes*/
    &INIT_SYSTEM_7_Queue_mapper[0],  &INIT_SYSTEM_7_High_Speed_Arr[0], sizeof(INIT_SYSTEM_7_High_Speed_Arr)       /*Queue mapper configuration and high speed ports array and size*/
};

/* CP4R100G Pizza Rev B*/
static GT_U8 INIT_SYSTEM_7_PIZZA_CORE_0_Rev_B [] = {0,0,0,0,0,0,0,0,0,0,15}; /* 0-10, 15-1  */
static GT_U8 INIT_SYSTEM_7_PIZZA_CORE_1_Rev_B [] = {16}; /* 16-1 */
static GT_U8 INIT_SYSTEM_7_PIZZA_CORE_2_Rev_B [] = {32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,
                                                    32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,32,36,40,47};/*32-40, 36-40, 40-40, 47-1*/
static GT_U8 INIT_SYSTEM_7_PIZZA_CORE_3_Rev_B [] = {48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,
                                                    48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,48,52,56,63};/*48-40, 52-40, 56-40, 63-1*/

/* High speed array Rev B */
static GT_U8 INIT_SYSTEM_7_High_Speed_Arr_Rev_B[] = {0,16,31,32,36,40,46,48,52,56};

/* Register queue mapper and pizza configuration to board for Rev B0 and above*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC INIT_SYSTEM_7_RevB= {
    {&INIT_SYSTEM_7_PIZZA_CORE_0_Rev_B[0],&INIT_SYSTEM_7_PIZZA_CORE_1_Rev_B[0],&INIT_SYSTEM_7_PIZZA_CORE_2_Rev_B[0],&INIT_SYSTEM_7_PIZZA_CORE_3_Rev_B[0]},          /* Pizza configuration arrays*/
    {sizeof(INIT_SYSTEM_7_PIZZA_CORE_0_Rev_B),sizeof(INIT_SYSTEM_7_PIZZA_CORE_1_Rev_B),sizeof(INIT_SYSTEM_7_PIZZA_CORE_2_Rev_B),sizeof(INIT_SYSTEM_7_PIZZA_CORE_3_Rev_B)}, /* Pizza configuration array sizes*/
    &INIT_SYSTEM_7_Queue_mapper[0],  &INIT_SYSTEM_7_High_Speed_Arr_Rev_B[0], sizeof(INIT_SYSTEM_7_High_Speed_Arr_Rev_B)       /*Queue mapper configuration and high speed ports array and size*/
};



/**********************************************************************************
*         Init system 8
**********************************************************************************/
static const APPDEMO_EXMXPM_QUEUE_MAPPER_CUSTOM_CFG_STC INIT_SYSTEM_8_Queue_mapper[]=
{  /*Phy Downstream     WC      Upstream   WC         downDMA   upDMA   Virtual         */
    {0      , 0     , GT_FALSE , 32    , GT_FALSE ,    0   ,    32   , 0xFF },
    {16     , 16    , GT_FALSE , 48    , GT_FALSE ,   16   ,    48   , 0xFF },
    {31     , 31    , GT_TRUE  , 63    , GT_TRUE  ,   31   ,    48   , 0xFF },
    {15     , 15    , GT_FALSE , 47    , GT_FALSE ,   PRV_CPSS_EXMXPM_CPU_PORT_NUM_CNS   ,    32 , 0xFF }, /* CPU port*/
    {0xFF   ,0xFF   ,     0xFF ,0xFF   ,     0xFF , 0xFF   ,  0xFF   , 0xFF },
};

/* CP4R100G Pizza*/
static GT_U8 INIT_SYSTEM_8_PIZZA_CORE_0 [] = {0,0,15,15}; /* 0-2, 15-2  cpu port is a high speed port*/
static GT_U8 INIT_SYSTEM_8_PIZZA_CORE_1 [] = {16,16,31,31}; /* 16-2 , 31-2   lb port is high speed port*/
static GT_U8 INIT_SYSTEM_8_PIZZA_CORE_2 [] = {32,32,47,47}; /* 32-2, 47-2  cpu port is a high speed port*/
static GT_U8 INIT_SYSTEM_8_PIZZA_CORE_3 [] = {48,48,63,63}; /* 48-2 , 63-2   lb port is high speed port*/

/* High speed array */
static GT_U8 INIT_SYSTEM_8_High_Speed_Arr[] = {0,15,16,31,32,47,48,63};

/* Register queue mapper and pizza configuration to board*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC INIT_SYSTEM_8= {
    {&INIT_SYSTEM_8_PIZZA_CORE_0[0],&INIT_SYSTEM_8_PIZZA_CORE_1[0],&INIT_SYSTEM_8_PIZZA_CORE_2[0],&INIT_SYSTEM_8_PIZZA_CORE_3[0]},          /* Pizza configuration arrays*/
    {sizeof(INIT_SYSTEM_8_PIZZA_CORE_0),sizeof(INIT_SYSTEM_8_PIZZA_CORE_1),sizeof(INIT_SYSTEM_8_PIZZA_CORE_2),sizeof(INIT_SYSTEM_8_PIZZA_CORE_3)}, /* Pizza configuration array sizes*/
    &INIT_SYSTEM_8_Queue_mapper[0],  &INIT_SYSTEM_8_High_Speed_Arr[0], sizeof(INIT_SYSTEM_8_High_Speed_Arr)       /*Queue mapper configuration and high speed ports array and size*/
};

/* CP4R100G Pizza Rev B*/
static GT_U8 INIT_SYSTEM_8_PIZZA_CORE_0_Rev_B [] = {0,0,0,0,0,0,0,0,0,0,15}; /* 0-10, 15-1  */
static GT_U8 INIT_SYSTEM_8_PIZZA_CORE_1_Rev_B [] = {16}; /* 16-1 */
static GT_U8 INIT_SYSTEM_8_PIZZA_CORE_2_Rev_B [] = {32,32,32,32,32,32,32,32,32,32,47}; /* 32-10, 15-1  */
static GT_U8 INIT_SYSTEM_8_PIZZA_CORE_3_Rev_B [] = {48}; /* 48-1 */

/* High speed array Rev B */
static GT_U8 INIT_SYSTEM_8_High_Speed_Arr_Rev_B[] = {0,16,31,32,48,63};

/* Register queue mapper and pizza configuration to board for Rev B*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC INIT_SYSTEM_8_RevB= {
    {&INIT_SYSTEM_8_PIZZA_CORE_0_Rev_B[0],&INIT_SYSTEM_8_PIZZA_CORE_1_Rev_B[0],&INIT_SYSTEM_8_PIZZA_CORE_2_Rev_B[0],&INIT_SYSTEM_8_PIZZA_CORE_3_Rev_B[0]},          /* Pizza configuration arrays*/
    {sizeof(INIT_SYSTEM_8_PIZZA_CORE_0_Rev_B),sizeof(INIT_SYSTEM_8_PIZZA_CORE_1_Rev_B),sizeof(INIT_SYSTEM_8_PIZZA_CORE_2_Rev_B),sizeof(INIT_SYSTEM_8_PIZZA_CORE_3_Rev_B)}, /* Pizza configuration array sizes*/
    &INIT_SYSTEM_8_Queue_mapper[0],  &INIT_SYSTEM_8_High_Speed_Arr_Rev_B[0], sizeof(INIT_SYSTEM_8_High_Speed_Arr_Rev_B)       /*Queue mapper configuration and high speed ports array and size*/
};

/**********************************************************************************
*         XP48L
**********************************************************************************/
static const APPDEMO_EXMXPM_QUEUE_MAPPER_CUSTOM_CFG_STC XP48L_Queue_mapper[]=
{  /*Phy Downstream     WC      Upstream   WC        downDMA   upDMA    Virtual  */
    {1      , 0     , GT_FALSE , 38    , GT_FALSE ,    1   ,    32   ,    8 },
    {2      , 1     , GT_FALSE , 39    , GT_FALSE ,    2   ,    32   ,    9 },
    {7      , 2     , GT_FALSE , 40    , GT_FALSE ,    7   ,    32   ,   10 },
    {5      , 3     , GT_FALSE , 41    , GT_FALSE ,    5   ,    32   ,   11 },
    {8      , 4     , GT_FALSE , 42    , GT_FALSE ,    8   ,    32   ,   12 },
    {11     , 5     , GT_FALSE , 43    , GT_FALSE ,   11   ,    32   ,   13 },
    {10     , 6     , GT_FALSE , 44    , GT_FALSE ,   10   ,    32   ,    1 },
    {19     , 20    , GT_FALSE , 36    , GT_FALSE ,   19   ,    32   ,    6 },
    {22     , 21    , GT_FALSE , 37    , GT_FALSE ,   22   ,    32   ,    7 },
    {23     , 18    , GT_FALSE , 34    , GT_FALSE ,   23   ,    32   ,    4 },
    {25     , 19    , GT_FALSE , 35    , GT_FALSE ,   25   ,    32   ,    5 },
    {26     , 16    , GT_FALSE , 32    , GT_FALSE ,   26   ,    32   ,    2 },
    {27     , 17    , GT_FALSE , 33    , GT_FALSE ,   27   ,    32   ,    3 },
    {31     , 31    , GT_TRUE  , 46    , GT_TRUE  ,   31   ,    32   ,    0 },
    {15     , 15    , GT_FALSE , 47    , GT_FALSE ,   PRV_CPSS_EXMXPM_CPU_PORT_NUM_CNS   ,    32   , 0xFF },  /* CPU port*/
    {32     ,0xFF   , GT_FALSE , 0xFF  , GT_FALSE , 0xFF   ,  0xFF   ,   14 },
    {0xFF   ,0xFF   ,    0xFF  , 0xFF  ,    0xFF  , 0xFF   ,  0xFF   , 0xFF },
};

/* XP48L Pizza*/
static GT_U8 XP48L_PIZZA_CORE_0 [] = {0,0,1,1,2,2,3,3,4,4,5,5,0,0,1,1,2,2,3,3,4,4,5,5,0,0,1,1,2,2,3,3,4,4,5,5,0,0,1,1,
                                      2,2,3,3,4,4,5,5,0,0,1,1,2,2,3,3,4,4,5,5,0,0,1,1,2,2,3,3,4,4,5,5,0,0,1,1,2,2,3,3,
                                      4,4,5,5,0,0,1,1,2,2,3,3,4,4,5,5,0,0,1,1,2,2,3,3,4,4,5,5,0,0,1,1,2,2,3,3,4,4,5,5,
                                      6,6,15,15};
static GT_U8 XP48L_PIZZA_CORE_1 [] = {16,16,17,17,31,31,18,18,19,19,31,31,20,20,21,21,31,31,0xFF,0xFF,0xFF,0xFF,31,31}; /* Added 4 unused slices to comply with restrictions, can be improved by using total of 72 slices (NW - 6X8 + 24 lb)*/
static GT_U8 XP48L_PIZZA_CORE_2 [] = {32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,42,42,43,43,32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,
                                      40,40,41,41,42,42,43,43,32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,42,42,43,43,32,32,33,33,34,34,35,35,
                                      36,36,37,37,38,38,39,39,40,40,41,41,42,42,43,43,32,32,33,33,46,46,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,42,42,
                                      43,43,32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,42,42,43,43,32,32,33,33,34,34,35,35,36,36,37,37,38,38,
                                      39,39,40,40,41,41,42,42,43,43,32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,42,42,43,43,44,44,46,46,47,47};

/* Register queue mapper and pizza configuration to board*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC XP48L= {
    {&XP48L_PIZZA_CORE_0[0],&XP48L_PIZZA_CORE_1[0],&XP48L_PIZZA_CORE_2[0], NULL},         /* Pizza configuration arrays*/
    {sizeof(XP48L_PIZZA_CORE_0),sizeof(XP48L_PIZZA_CORE_1),sizeof(XP48L_PIZZA_CORE_2),0}, /* Pizza configuration array sizes*/
    &XP48L_Queue_mapper[0], NULL,0                                                        /*Queue mapper configuration and high speed ports array and size*/
};


/* XP48L Pizza for Rev B*/
static GT_U8 XP48L_PIZZA_CORE_0_Rev_B [] = {0,1,2,3,4,5,6,15}; /* 0,1,2,3,4,5 - 1   6,15-1 */
static GT_U8 XP48L_PIZZA_CORE_1_Rev_B [] = {16,17,18,19,20,21};  /* 16,17,18,19,20,21 - 1 */
static GT_U8 XP48L_PIZZA_CORE_2_Rev_B [] = {32,33,34,35,36,37,38,39,40,41,42,43,32,33,34,35,36,37,38,39,40,41,42,43,32,33,34,35,36,37,38,39,40,41,42,43,32,33,34,35,36,37,38,39,40,41,42,43,32,33,34,35,36,37,38,39,40,41,42,43,
                                            32,33,34,35,36,37,38,39,40,41,42,43,32,33,34,35,36,37,38,39,40,41,42,43,32,33,34,35,36,37,38,39,40,41,42,43,32,33,34,35,36,37,38,39,40,41,42,43,32,33,34,35,36,37,38,39,40,41,42,43,44,47};  /* 32,33,34,35,36,37,38,39,40,41,42,43 - 10   44,47 -1*/

static GT_U8 XP48L_High_Speed_Arr_Rev_B[] = {31,46};

/* Register queue mapper and pizza configuration to board for Rev B*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC XP48L_RevB= {
    {&XP48L_PIZZA_CORE_0_Rev_B[0],&XP48L_PIZZA_CORE_1_Rev_B[0],&XP48L_PIZZA_CORE_2_Rev_B[0], NULL},         /* Pizza configuration arrays*/
    {sizeof(XP48L_PIZZA_CORE_0_Rev_B),sizeof(XP48L_PIZZA_CORE_1_Rev_B),sizeof(XP48L_PIZZA_CORE_2_Rev_B),0}, /* Pizza configuration array sizes*/
    &XP48L_Queue_mapper[0], &XP48L_High_Speed_Arr_Rev_B[0], sizeof(XP48L_High_Speed_Arr_Rev_B)              /*Queue mapper configuration and high speed ports array and size*/
};


/**********************************************************************************
*         XLP16R
**********************************************************************************/
static const APPDEMO_EXMXPM_QUEUE_MAPPER_CUSTOM_CFG_STC XLP16R_Queue_mapper[]=
{  /*Phy Downstream     WC      Upstream   WC        downDMA   upDMA   Virtual         */
    {0      ,  0    , GT_TRUE  , 34    , GT_TRUE  ,    0   ,    32  ,    4 },
    {4      ,  1    , GT_TRUE  , 35    , GT_TRUE  ,    4   ,    32  ,    5 },
    {16     , 17    , GT_TRUE  , 33    , GT_TRUE  ,   16   ,    32  ,    3 },
    {20     , 16    , GT_TRUE  , 32    , GT_TRUE  ,   20   ,    32  ,    2 },
    {31     , 31    , GT_TRUE  , 36    , GT_TRUE  ,   31   ,    32  ,    0 },
    {15     , 15    , GT_FALSE , 47    , GT_FALSE ,   PRV_CPSS_EXMXPM_CPU_PORT_NUM_CNS   ,    32  , 0xFF },  /* CPU port*/
    {32     ,0xFF   , GT_FALSE ,0xFF   , GT_FALSE , 0xFF   ,  0xFF   ,   6 },
    {0xFF   ,0xFF   ,   0xFF   ,0xFF   ,   0xFF   , 0xFF   ,  0xFF  , 0xFF },
};

/* XLP16R Pizza*/
static GT_U8 XLP16R_PIZZA_CORE_0 [] = {0,0,1,1,2,2,0,0,1,1,15,15,0,0,1,1,2,2,0,0,1,1,15,15}; /* duplicated, can use only 12 slices*/
static GT_U8 XLP16R_PIZZA_CORE_1 [] = {16,16,17,17,31,31,16,16,17,17,31,31,16,16,17,17,31,31,16,16,17,17,31,31}; /* duplicated twice, can use only 6 slices*/
static GT_U8 XLP16R_PIZZA_CORE_2 [] = {32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,
                                       32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,
                                       32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,36,36,32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,
                                       35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,
                                       35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,32,32,33,33,34,34,35,35,36,36,47,47};

/* Register queue mapper and pizza configuration to board*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC XLP16R= {
    {&XLP16R_PIZZA_CORE_0[0],&XLP16R_PIZZA_CORE_1[0],&XLP16R_PIZZA_CORE_2[0], NULL},         /* Pizza configuration arrays*/
    {sizeof(XLP16R_PIZZA_CORE_0),sizeof(XLP16R_PIZZA_CORE_1),sizeof(XLP16R_PIZZA_CORE_2),0}, /* Pizza configuration array sizes*/
    &XLP16R_Queue_mapper[0],  NULL,0                                                         /*Queue mapper configuration and high speed ports array and size*/
};

/* XLP16R Pizza for Rev B*/
static GT_U8 XLP16R_PIZZA_CORE_0_Rev_B [] = {0,1,2,0,1,15}; /* 0,1-2  2,15-1 */
static GT_U8 XLP16R_PIZZA_CORE_1_Rev_B [] = {16,17,0xFF};     /* 16,17 -1 0xFF-1 for pizza constraints*/
static GT_U8 XLP16R_PIZZA_CORE_2_Rev_B [] = {32,33,34,35,32,33,34,35,32,33,34,35,32,33,34,35,32,33,34,35,32,33,34,35,32,33,34,35,36,47}; /* 32,33,34,35-7  36,47-1*/


static GT_U8 XLP16R_High_Speed_Arr_Rev_B[] = {0,1,16,17,32,33,34,35,36};

/* Register queue mapper and pizza configuration to board for Rev B*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC XLP16R_RevB= {
    {&XLP16R_PIZZA_CORE_0_Rev_B[0],&XLP16R_PIZZA_CORE_1_Rev_B[0],&XLP16R_PIZZA_CORE_2_Rev_B[0], NULL},         /* Pizza configuration arrays*/
    {sizeof(XLP16R_PIZZA_CORE_0_Rev_B),sizeof(XLP16R_PIZZA_CORE_1_Rev_B),sizeof(XLP16R_PIZZA_CORE_2_Rev_B),0}, /* Pizza configuration array sizes*/
    &XLP16R_Queue_mapper[0],  &XLP16R_High_Speed_Arr_Rev_B[0], sizeof(XLP16R_High_Speed_Arr_Rev_B)              /*Queue mapper configuration and high speed ports array and size*/
};



/**********************************************************************************
*         XP40R
**********************************************************************************/
static const APPDEMO_EXMXPM_QUEUE_MAPPER_CUSTOM_CFG_STC XP40R_Queue_mapper[]=
{  /*Phy Downstream     WC      Upstream   WC         downDMA   upDMA   Virtual         */
    {0      , 1     , GT_FALSE , 33    , GT_FALSE ,    0   ,    32   ,  13 },
    {1      , 0     , GT_FALSE , 32    , GT_FALSE ,    1   ,    32   ,  12 },
    {2      , 3     , GT_FALSE , 35    , GT_FALSE ,    2   ,    32   ,  15 },
    {3      , 2     , GT_FALSE , 34    , GT_FALSE ,    3   ,    32   ,  14 },
    {4      , 5     , GT_FALSE , 37    , GT_FALSE ,    4   ,    32   ,  17 },
    {5      , 4     , GT_FALSE , 36    , GT_FALSE ,    5   ,    32   ,  16 },
    {6      , 7     , GT_FALSE , 39    , GT_FALSE ,    6   ,    32   ,  19 },
    {7      , 6     , GT_FALSE , 38    , GT_FALSE ,    7   ,    32   ,  18 },
    {8      , 9     , GT_FALSE , 41    , GT_FALSE ,    8   ,    32   ,  21 },
    {9      , 8     , GT_FALSE , 40    , GT_FALSE ,    9   ,    32   ,  20 },
    {10     , 10    , GT_FALSE , 42    , GT_FALSE ,   10   ,    32   ,   1 },
    {16     , 24    , GT_FALSE , 56    , GT_FALSE ,   16   ,    48   ,  10 },
    {17     , 25    , GT_FALSE , 57    , GT_FALSE ,   17   ,    48   ,  11 },
    {18     , 22    , GT_FALSE , 54    , GT_FALSE ,   18   ,    48   ,   8 },
    {19     , 23    , GT_FALSE , 55    , GT_FALSE ,   19   ,    48   ,   9 },
    {20     , 20    , GT_FALSE , 52    , GT_FALSE ,   20   ,    48   ,   6 },
    {21     , 21    , GT_FALSE , 53    , GT_FALSE ,   21   ,    48   ,   7 },
    {22     , 18    , GT_FALSE , 50    , GT_FALSE ,   22   ,    48   ,   4 },
    {23     , 19    , GT_FALSE , 51    , GT_FALSE ,   23   ,    48   ,   5 },
    {24     , 16    , GT_FALSE , 48    , GT_FALSE ,   24   ,    48   ,   2 },
    {25     , 17    , GT_FALSE , 49    , GT_FALSE ,   25   ,    48   ,   3 },
    {31     , 31    , GT_TRUE  , 63    , GT_TRUE  ,   31   ,    48   ,   0 },
    {15     , 15    , GT_FALSE , 47    , GT_FALSE ,   PRV_CPSS_EXMXPM_CPU_PORT_NUM_CNS   ,    32  , 0xFF },  /* CPU port*/
    {32     ,0xFF   , GT_FALSE ,0xFF   , GT_FALSE , 0xFF   ,  0xFF   ,  22 },
    {48     ,0xFF   , GT_FALSE ,0xFF   , GT_FALSE , 0xFF   ,  0xFF   ,  23 },
    {0xFF   ,0xFF   ,     0xFF ,0xFF   ,     0xFF , 0xFF   ,  0xFF  , 0xFF },
};

/* XP40R Pizza*/
static GT_U8 XP40R_PIZZA_CORE_0 [] = {0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,
                                      0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,
                                      0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,15,15};
static GT_U8 XP40R_PIZZA_CORE_1 [] = {16,16,17,17,18,18,19,19,20,20,31,31,21,21,22,22,23,23,24,24,25,25,31,31,16,16,17,17,18,18,19,19,20,20,31,31,21,21,22,22,
                                      23,23,24,24,25,25,31,31,16,16,17,17,18,18,19,19,20,20,31,31,21,21,22,22,23,23,24,24,25,25,31,31,16,16,17,17,18,18,19,19,
                                      20,20,31,31,21,21,22,22,23,23,24,24,25,25,31,31,16,16,17,17,18,18,19,19,20,20,31,31,21,21,22,22,23,23,24,24,25,25,31,31}; /* duplicated five times, can use only 24 slices*/
static GT_U8 XP40R_PIZZA_CORE_2 [] = {32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,
                                      32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,
                                      32,32,33,33,34,34,35,35,36,36,37,37,38,38,39,39,40,40,41,41,42,42,47,47};
static GT_U8 XP40R_PIZZA_CORE_3 [] = {48,48,49,49,50,50,51,51,52,52,53,53,54,54,55,55,56,56,57,57,48,48,49,49,50,50,51,51,52,52,53,53,54,54,55,55,56,56,57,57,
                                      48,48,49,49,50,50,51,51,52,52,53,53,54,54,55,55,56,56,57,57,48,48,49,49,50,50,51,51,52,52,53,53,54,54,55,55,56,56,57,57,
                                      48,48,49,49,50,50,51,51,52,52,53,53,54,54,55,55,56,56,57,57,63,63};

/* Register queue mapper and pizza configuration to board*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC XP40R= {
    {&XP40R_PIZZA_CORE_0[0],&XP40R_PIZZA_CORE_1[0],&XP40R_PIZZA_CORE_2[0], XP40R_PIZZA_CORE_3},                    /* Pizza configuration arrays*/
    {sizeof(XP40R_PIZZA_CORE_0),sizeof(XP40R_PIZZA_CORE_1),sizeof(XP40R_PIZZA_CORE_2),sizeof(XP40R_PIZZA_CORE_3)}, /* Pizza configuration array sizes*/
    &XP40R_Queue_mapper[0],  NULL,0                                                                                /*Queue mapper configuration and high speed ports array and size*/
};


/* XP40R Pizza for Rev B*/
static GT_U8 XP40R_PIZZA_CORE_0_Rev_B [] = {0,1,2,3,4,5,6,7,8,9,10,15}; /* 0,1,2,3,4,5,6,7,8,9 - 1 10,15-1*/
static GT_U8 XP40R_PIZZA_CORE_1_Rev_B [] = {16,17,18,19,20,21,22,23,24,25}; /* 16,17,18,19,20,21,22,23,24,25 - 1  */
static GT_U8 XP40R_PIZZA_CORE_2_Rev_B [] = {32,33,34,35,36,37,38,39,40,41,47}; /* 32,33,34,35,36,37,38,39,40,41 - 1  47-1*/
static GT_U8 XP40R_PIZZA_CORE_3_Rev_B [] = {48,49,50,51,52,53,54,55,56,57}; /* 48,49,50,51,52,53,54,55,56,57 - 1  */

static GT_U8 XP40R_High_Speed_Arr_Rev_B[] = {31,63};

/* Register queue mapper and pizza configuration to board for Rev B*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC XP40R_RevB= {
    {&XP40R_PIZZA_CORE_0_Rev_B[0],&XP40R_PIZZA_CORE_1_Rev_B[0],&XP40R_PIZZA_CORE_2_Rev_B[0], &XP40R_PIZZA_CORE_3_Rev_B[0]},                    /* Pizza configuration arrays*/
    {sizeof(XP40R_PIZZA_CORE_0_Rev_B),sizeof(XP40R_PIZZA_CORE_1_Rev_B),sizeof(XP40R_PIZZA_CORE_2_Rev_B),sizeof(XP40R_PIZZA_CORE_3_Rev_B)}, /* Pizza configuration array sizes*/
    &XP40R_Queue_mapper[0], &XP40R_High_Speed_Arr_Rev_B[0], sizeof(XP40R_High_Speed_Arr_Rev_B)                   /*Queue mapper configuration and high speed ports array and size*/
};

/**********************************************************************************
*         CP4R - 2X100G
**********************************************************************************/
static const APPDEMO_EXMXPM_QUEUE_MAPPER_CUSTOM_CFG_STC CP4R100G_Queue_mapper[]=
{  /*Phy Downstream     WC      Upstream   WC         downDMA   upDMA  Virtual          */
    {0      , 0     , GT_TRUE  , 32    , GT_TRUE  ,    0   ,    32   ,   3  },
    {16     , 16    , GT_TRUE  , 33    , GT_TRUE  ,   16   ,    32   ,   2  },
    {10     , 1     , GT_FALSE , 35    , GT_FALSE ,   10   ,    32   ,   1  },
    {31     , 31    , GT_TRUE  , 34    , GT_TRUE  ,   31   ,    32   ,   0  },
    {15     , 15    , GT_FALSE , 47    , GT_FALSE ,   PRV_CPSS_EXMXPM_CPU_PORT_NUM_CNS   ,    32  , 0xFF },  /* CPU port*/
    {32     ,0xFF   , GT_FALSE ,0xFF   , GT_FALSE , 0xFF   ,  0xFF   ,   4  },
    {0xFF   ,0xFF   , 0xFF     ,0xFF   , 0xFF     , 0xFF   ,  0xFF   , 0xFF },
};


/* CP4R100G Pizza*/
static GT_U8 CP4R100G_PIZZA_CORE_0 [] = {0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,
                                         0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,
                                         0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,
                                         0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,
                                         0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0,0,2,2,0xFF,0xFF,1,1,15,15}; /* 0-96, 2-96 , X-2, 1-2, 15-2*/

static GT_U8 CP4R100G_PIZZA_CORE_1 [] = {16,16,31,31}; /* 16-2 , 31-2   lb port is high speed port*/

static GT_U8 CP4R100G_PIZZA_CORE_2 [] = {32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,
                                         32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,
                                         32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,
                                         32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,
                                         32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,0xFF,0xFF,34,34,35,35,47,47}; /*32-96, 33-96, X-2, 34-2, 35-2, 47-2*/


/* High speed array */
static GT_U8 CP4R100G_High_Speed_Arr[] = {0,2,16,31,32,33};


/* Register queue mapper and pizza configuration to board*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC CP4R100G= {
    {&CP4R100G_PIZZA_CORE_0[0],&CP4R100G_PIZZA_CORE_1[0],&CP4R100G_PIZZA_CORE_2[0],NULL},          /* Pizza configuration arrays*/
    {sizeof(CP4R100G_PIZZA_CORE_0),sizeof(CP4R100G_PIZZA_CORE_1),sizeof(CP4R100G_PIZZA_CORE_2),0}, /* Pizza configuration array sizes*/
    &CP4R100G_Queue_mapper[0],  &CP4R100G_High_Speed_Arr[0], sizeof(CP4R100G_High_Speed_Arr)       /*Queue mapper configuration and high speed ports array and size*/
};


/* CP4R100G Pizza for Rev B*/
static GT_U8 CP4R100G_PIZZA_CORE_0_Rev_B [] = {0,0,0,0,0,0,0,0,0,0,1,15}; /* 0-10, 1,15-1*/
static GT_U8 CP4R100G_PIZZA_CORE_1_Rev_B [] = {16,16,16,16,16,31,16,16,16,16,16,31}; /* 16-10 31-2 */
static GT_U8 CP4R100G_PIZZA_CORE_2_Rev_B [] = {32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,
                                               32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,32,33,34,32,33,35,47}; /* 32,33-47 34-23  35,47-1  */

/* High speed array for Rev B */
static GT_U8 CP4R100G_High_Speed_Arr_Rev_B[] = {0,16,32,33};


/* Register queue mapper and pizza configuration to board for Rev B*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC CP4R100G_RevB= {
    {&CP4R100G_PIZZA_CORE_0_Rev_B[0],&CP4R100G_PIZZA_CORE_1_Rev_B[0],&CP4R100G_PIZZA_CORE_2_Rev_B[0],NULL},          /* Pizza configuration arrays*/
    {sizeof(CP4R100G_PIZZA_CORE_0_Rev_B),sizeof(CP4R100G_PIZZA_CORE_1_Rev_B),sizeof(CP4R100G_PIZZA_CORE_2_Rev_B),0}, /* Pizza configuration array sizes*/
    &CP4R100G_Queue_mapper[0],  &CP4R100G_High_Speed_Arr_Rev_B[0], sizeof(CP4R100G_High_Speed_Arr_Rev_B)       /*Queue mapper configuration and high speed ports array and size*/
};

/**********************************************************************************
*         CP4R - 100G + 40G
**********************************************************************************/
static const APPDEMO_EXMXPM_QUEUE_MAPPER_CUSTOM_CFG_STC CP4R40G_Queue_mapper[]=
{  /*Phy Downstream     WC      Upstream   WC         downDMA   upDMA  Virtual  */
    {4      , 0     , GT_FALSE , 33    , GT_FALSE ,    4   ,    32   ,   3  },
    {16     , 16    , GT_FALSE , 32    , GT_TRUE  ,   16   ,    32   ,   2  },
    {10     , 1     , GT_FALSE , 34    , GT_FALSE ,   10   ,    32   ,   1  },
    {31     , 31    , GT_TRUE  , 46    , GT_TRUE  ,   31   ,    32   ,   0  },
    {15     , 15    , GT_FALSE , 47    , GT_FALSE ,   PRV_CPSS_EXMXPM_CPU_PORT_NUM_CNS   ,    32   , 0xFF },  /* CPU port*/
    {32     ,0xFF   , GT_FALSE ,0xFF   , GT_FALSE , 0xFF   ,  0xFF   ,   4 },
    {0xFF   ,0xFF   , 0xFF     ,0xFF   , 0xFF     , 0xFF   ,  0xFF   , 0xFF },
};

/* CP4R40G Pizza*/
static GT_U8 CP4R40G_PIZZA_CORE_0 [] = {0,0,1,1,15,15}; /* 0-2 , 1-2  15-2*/

static GT_U8 CP4R40G_PIZZA_CORE_1 [] = {16,16,31,31}; /* 16-2 , 31-2   lb port is high speed port*/

static GT_U8 CP4R40G_PIZZA_CORE_2 [] = {32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,
                                        32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,
                                        32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,
                                        32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,
                                        32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,32,32,33,33,0xFF,0xFF,34,34,46,46,47,47}; /*32-96, 33-96, X-2, 34-2, 46-2, 47-2 */

/* High speed array */
static GT_U8 CP4R40G_High_Speed_Arr[] = {16,31,32,33};

/* Register queue mapper and pizza configuration to board*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC CP4R40G= {
    {&CP4R40G_PIZZA_CORE_0[0],&CP4R40G_PIZZA_CORE_1[0],&CP4R40G_PIZZA_CORE_2[0],NULL},          /* Pizza configuration arrays*/
    {sizeof(CP4R40G_PIZZA_CORE_0),sizeof(CP4R40G_PIZZA_CORE_1),sizeof(CP4R40G_PIZZA_CORE_2),0}, /* Pizza configuration array sizes*/
    &CP4R40G_Queue_mapper[0], &CP4R40G_High_Speed_Arr[0], sizeof(CP4R40G_High_Speed_Arr)        /*Queue mapper configuration and high speed ports array and size*/
};

/* CP4R40G Pizza for Rev B*/
static GT_U8 CP4R40G_PIZZA_CORE_0_Rev_B [] = {0,1,15}; /* 0-1 , 1,15-1*/
static GT_U8 CP4R40G_PIZZA_CORE_1_Rev_B [] = {16}; /* 16-1*/
static GT_U8 CP4R40G_PIZZA_CORE_2_Rev_B [] = {32,32,32,33,32,32,33,32,32,33,32,32,33,32,32,32,33,32,32,33,32,32,33,32,32,33,32,32,32,33,32,32,33,32,32,33,32,32,32,33,32,32,33,32,32,33,32,32,33,32,32,32,33,32,32,33,32,32,33,32,32,33,32,32,32,33,32,32,32,33,32,32,33,32,32,33,32,32,32,33,
                                              32,32,32,33,32,32,33,32,32,33,32,32,32,33,32,32,32,33,32,32,33,32,32,33,32,32,33,32,32,32,33,32,32,33,32,32,33,32,32,32,33,32,32,33,32,32,33,32,32,33,32,32,32,33,34,47}; /* 32-94 33-40  34,47-1*/

/* High speed array for Rev B */
static GT_U8 CP4R40G_High_Speed_Arr_Rev_B[] = {16,31,32,46};

/* Register queue mapper and pizza configuration to board for Rev B*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC CP4R40G_RevB= {
    {&CP4R40G_PIZZA_CORE_0_Rev_B[0],&CP4R40G_PIZZA_CORE_1_Rev_B[0],&CP4R40G_PIZZA_CORE_2_Rev_B[0],NULL},          /* Pizza configuration arrays*/
    {sizeof(CP4R40G_PIZZA_CORE_0_Rev_B),sizeof(CP4R40G_PIZZA_CORE_1_Rev_B),sizeof(CP4R40G_PIZZA_CORE_2_Rev_B),0}, /* Pizza configuration array sizes*/
    &CP4R40G_Queue_mapper[0], &CP4R40G_High_Speed_Arr_Rev_B[0], sizeof(CP4R40G_High_Speed_Arr_Rev_B)        /*Queue mapper configuration and high speed ports array and size*/
};


/*      Register all custom boards*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC *appDemoExMxPmCustomBoardConfigurationsRevA[]={
    NULL, NULL, NULL, &INIT_SYSTEM_4, NULL, NULL, &INIT_SYSTEM_7, &INIT_SYSTEM_8,   /* init system 1-8*/
    &XP48L, &XLP16R, &XP40R, &CP4R100G, &CP4R40G}; /* init system 9-13*/

/*      Register all custom boards for revision B and above*/
static const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC *appDemoExMxPmCustomBoardConfigurationsRevB[]={
    NULL, NULL, NULL, &INIT_SYSTEM_4_RevB, NULL, NULL, &INIT_SYSTEM_7_RevB, &INIT_SYSTEM_8_RevB,   /* init system 1-8*/
    &XP48L_RevB, &XLP16R_RevB, &XP40R_RevB, &CP4R100G_RevB, &CP4R40G_RevB}; /* init system 9-13*/



static GT_STATUS getDramStaticCfg
(
    IN APPDEMO_DRAM_FREQUENCY_ENT               targetFreq,
    IN APPDEMO_EMC_TYPE                         memType,
    IN APPDEMO_BOARD_TYPE                       board,
    OUT APPDEMO_EXMXPM_EXT_DRAM_STATIC_CFG      **staticParamsPtr,
    OUT GT_U32                                  *numOfCfgPtr
);

static GT_STATUS prvQueueMapperConfig
(
    IN GT_U8  devNum,
    IN GT_U8  txqPortNum,
    IN GT_U8  portNum
);


/*******************************************************************************
 * External variables
 ******************************************************************************/

/* User configured bitmap for SERDES power down */
extern GT_U32 appDemoExMxPmPowerDownBmp;
GT_U32 appDemoExMxPmPowerDownBmp1 = 0;

static CPSS_REG_VALUE_INFO_STC   regValInfoList[]= GT_DUMMY_REG_VAL_INFO_LIST;

/* number of 'devices' that we see on the PEX for single Puma3 device */
#define PUMA3_MANAGEMENT_BUS_NUM_CNS    2

/* The second core interlaken 24 uses */
#define CPSS_EXMXPM_ILKN_24_SECOND_CORE             3

static GT_U8    ppCounter = 0; /* Number of Packet processors. */
static GT_U32   numOfActivePortGroups = 0; /* number of port groups -- assume single device */

static GT_U8    currentDevNum = 0;

static GT_BOOL  isModularBoard[PRV_CPSS_MAX_PP_DEVICES_CNS] = {GT_FALSE};
static GT_BOOL  isLoopbackDisabled[PRV_CPSS_MAX_PP_DEVICES_CNS] = {GT_FALSE};

/* enable External TCAM for HW only */
#ifndef ASIC_SIMULATION
GT_BOOL appDemoPuma3ExtTcamEnabled = GT_TRUE;
#else
GT_BOOL appDemoPuma3ExtTcamEnabled = GT_FALSE;
#endif

/* virtual port number for loopback port */
#define LOOPBACK_PORT_VIRTUAL_PORT_NUM_CNS          56

/* Physical port number for loopback port */
#define CPSS_EXMXPM3_LOOPBACK_PORT_PHYSICAL_PORT_NUM_CNS    31
/* physical port number for non exist port */
#define NON_EXIST_PHYSICAL_PORT_NUM_CNS             13



#define CORE_NUM                    2
#define REG_WRITE_MAX_DATA_WORDS    16
static GT_STATUS    extraSystemConfigSet(boardRevId)
{
    GT_U32 phyPort;
    GT_U32 i;

    CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_KEY_TABLE_PACKET_TYPE_ENT packetType;
    GT_U8                                                      srcPortGroupId;
    GT_BOOL                                                    trgDevIsOwn;
    GT_U32                                                     trgDevsBmpArr[4];
    CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_KEY_INDEX_UNT             keyIndex;
    CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_ENTRY_STC   tableEntry;

    GT_STATUS rc;

    /* /Cider/EBU/puma3/Puma3 {Current}/Puma3 Units/Global Units/EGF EGF_IP_sht%i/Egress Tables/Mapping Type Table*/

    /* Mapping Type table*/
    /* Upstream Queue Mapping tables
    All the packet types (UC,MC) from source cores 0 and 1 with TargetIsOwn true and false are mapped
    to Table 0 with Key 1 (pkt_type, VirtualSrcPort, FabricTc).
    */
    for (i = 0; i < 16; i++)
    {
        packetType = ((i >> 3) & 0x1) ? CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_KEY_TABLE_PACKET_TYPE_MC_FORWARD_E : CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_KEY_TABLE_PACKET_TYPE_UC_FORWARD_E;
        srcPortGroupId = (i >> 1) & 0x3;
        trgDevIsOwn = BIT2BOOL_MAC((i & 1));
        rc = cpssExMxPmFabricTxqQueueMapKeyTypeTableEntryWrite(currentDevNum,
                                                               CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_DIRECTION_UPSTREAM_E,
                                                               packetType,
                                                               srcPortGroupId,
                                                               trgDevIsOwn,
                                                               (boardRevId == 7)?0:1,
                                                               0);
        if(GT_OK != rc)
        {
            return rc;
        }
    }


    /* Use work conserving for Puma 3 B0 lb port when there is no custom config*/
    if ((PRV_CPSS_PP_MAC(currentDevNum)->revision > 0) && (CPSS_EXMXPM3_CUSTOM_CONFIG(currentDevNum,boardRevId) == NULL))
    {
        rc = cpssExMxPmPortPizzaArbiterWorkConservingEnableSet(currentDevNum, 31, GT_FALSE, GT_TRUE);
        if (rc != GT_OK)
        {
            return rc;
        }
    }


    /* EGF SHT configuration*/
    /* /Cider/EBU/puma3/Puma3 {Current}/Puma3 Units/Global Units/EGF EGF_IP_sht%i/Egress Tables/Mapping Type Table*/

    /* Mapping Type table*/
    /* key#2 : table 2*/
    /* Downstream Queue Mapping Tables :
    All the packet types (UC,MC) from source cores 2 and 3 with TargetIsOwn true and false are mapped to
    Table 2 with Key 2 (TargetVirtualPort, TargetTC).
    */
    for (i = 0; i < 16; i++)
    {
        packetType = ((i >> 3) & 0x1) ? CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_KEY_TABLE_PACKET_TYPE_MC_FORWARD_E : CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_KEY_TABLE_PACKET_TYPE_UC_FORWARD_E;
        srcPortGroupId = (i >> 1) & 0x3;
        trgDevIsOwn = BIT2BOOL_MAC((i & 1));
        rc = cpssExMxPmFabricTxqQueueMapKeyTypeTableEntryWrite(currentDevNum,
                                                               CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_DIRECTION_DOWNSTREAM_E,
                                                               packetType,
                                                               srcPortGroupId,
                                                               trgDevIsOwn,
                                                               2,
                                                               2);
        if(GT_OK != rc)
        {
            return rc;
        }
    }


    /*Downstream Queue Mapping Tables :
    Key Type Table 0x3a00000:
    All the packet types (UC,MC) from source cores 2 and 3 with TargetIsOwn true and false are mapped to Table 2 with Key 2 (TargetVirtualPort, TargetTC).
    Table2 Configuration 0x3b10000:
    Entries targeted to ports 0-11 are sent to queues 0-95 where {Port0, Tc0} is sent to queue 0 and {Port11,Tc7} is sent to queue 95.
    Entries targeted to ports 16-27 are sent to queues 128-223 where {Port16,Tc0} is sent to queue 128 and {Port27,Tc7} is sent to queue 223.
    Entries corresponding to the CPU Port (63) are sent to queues 120-127 according to the TC and entries corresponding to the Loopback port (56) are sent to queues 248-255 according to the TC.
    */

    /* /Cider/EBU/puma3/Puma3 {Current}/Puma3 Units/Global Units/EGF EGF_IP_queue_agg/Distributor/Mapping Table 2*/
    tableEntry.forceDp = GT_FALSE;
    tableEntry.dp = CPSS_DP_GREEN_E;

    {
        /* Mapping table 2 cfg*/
        for(i = 0; i < 512; i++)
        {
            keyIndex.indexKey2.targetPort = (i / TC_MAX);
            keyIndex.indexKey2.tc = i % TC_MAX;

            /* map (back) virtual port to physical port :
              1. virtual ports  0..11 --> physical  0..11
              2. virtual ports 12..15 --> physical 13 (drop packet)
              3. virtual ports 16..27 --> physical 16..27
              4. virtual ports 28..31 --> physical 13 (drop packet)
              5. virtual ports 32..55,57..62 --> physical 13 (drop packet)
              6. virtual port 56  --> physical 31
              7. virtual port 63  --> physical 15
            */
            if(0 == CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(currentDevNum)->existingPorts,keyIndex.indexKey2.targetPort) ||
               keyIndex.indexKey2.targetPort >= 32) /* not for fabric ports */
            {
                if(keyIndex.indexKey2.targetPort == CPSS_CPU_PORT_NUM_CNS)
                {
                    /* CPU physical port */
                    phyPort = 15 + (16 * PRV_CPSS_CPU_PORT_PORT_GROUP_ID_MAC(currentDevNum));
                }
                else if(keyIndex.indexKey2.targetPort == LOOPBACK_PORT_VIRTUAL_PORT_NUM_CNS)/*loopback port*/
                {
                    /* loopback physical port */
                    phyPort = PRV_CPSS_EXMXPM_LBP_PHY_PORT_NUM_CNS(currentDevNum);
                }
                else
                {
                    phyPort = NON_EXIST_PHYSICAL_PORT_NUM_CNS;/* physical port that is not exist */
                }
            }
            else
            {
                phyPort = keyIndex.indexKey2.targetPort;
            }


            tableEntry.queueNum = (i%TC_MAX) | (phyPort << 3);

            rc = cpssExMxPmFabricTxqQueueMapMappingTableEntryWrite(currentDevNum,2,2,&keyIndex,&tableEntry);
            if(GT_OK != rc)
            {
                return rc;
            }
        }
    }

    /* enable EGF system devices */
    trgDevsBmpArr[0] = 0xffffffff;
    trgDevsBmpArr[1] = 0xffffffff;
    trgDevsBmpArr[2] = 0xffffffff;
    trgDevsBmpArr[3] = 0xffffffff;

    rc = cpssExMxPmFabricTxqTargetDeviceFilterTableSet(currentDevNum, trgDevsBmpArr);
    if(GT_OK != rc)
    {
        return rc;
    }

    /* the GM do not simulate FDB BCAM */
#ifdef ASIC_SIMULATION
    rc = cpssExMxPmBrgFdbTcamEnableSet(currentDevNum, GT_FALSE);
    if(GT_OK != rc)
    {
        return rc;
    }
#endif



    /*enable txq mapping for portgroup 2 & 3*/
    rc = cpssExMxPmFabricTxqQueueMapToDmaMappingEnableSet(currentDevNum,0xC, GT_TRUE);
    if( GT_OK != rc )
        return rc;

    /* Map each virtual port to a matching pfc counter*/
    for (i=0;i<64;i++)
    {
        rc = cpssExMxPmPortPfcSourcePortToPfcCounterSet(currentDevNum, (GT_U8)i, i);
        if (rc != GT_OK)
            return rc;
    }

    return  GT_OK;
}


static GT_STATUS prvQueueMapperConfig
(
    IN GT_U8  devNum,
    IN GT_U8  txqPortNum,
    IN GT_U8  portNum
)
{
    CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_ENTRY_STC   tableEntry;
    CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_KEY_INDEX_UNT             keyIndex;
    GT_STATUS rc;
    GT_U32  i,k;

    tableEntry.forceDp = GT_FALSE;
    tableEntry.dp = CPSS_DP_GREEN_E;

    if (txqPortNum >= 32)
    {
        for (i = 0; i < 8; i++)  /* packet type[11:9] index*/
        {

            switch (i)
              {
                case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_FROM_CPU_E:
                case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_TO_CPU_E:
                case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_INGRESS_MIRROR_E:
                case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_EGRESS_MIRROR_E:
                case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_FORWARD_E:
                    break;
                default:
                    continue;
              }

              keyIndex.indexKey1.packetType = i;
              keyIndex.indexKey1.localDevSrcPort =portNum;

              for (k = 0; k < 8; k++) /* TC[2:0] index*/
              {
                  keyIndex.indexKey1.fabricTc = k;
                  tableEntry.queueNum = txqPortNum*8 + k;

                  /* GM TxQ port 41-47,57-63 does not send traffic*/
                  #ifdef GM_USED
                  if (txqPortNum>=41)
                      tableEntry.queueNum = 256 + k;

                  if (txqPortNum>=57)
                      tableEntry.queueNum = 384 + k;
                  #endif

                  rc = cpssExMxPmFabricTxqQueueMapMappingTableEntryWrite(devNum,1,0,&keyIndex,&tableEntry);
                  if(GT_OK != rc)
                  {
                    return rc;
                  }
              }/* tc*/
        }
    }
    else    /* Downstream*/
    {
        tableEntry.forceDp = GT_FALSE;
        tableEntry.dp = CPSS_DP_GREEN_E;


        keyIndex.indexKey2.targetPort = portNum;

        for (k = 0; k < 8; k++) /* TC[2:0] index*/
        {
            keyIndex.indexKey2.tc = k;
            tableEntry.queueNum = txqPortNum*8+k;

            rc = cpssExMxPmFabricTxqQueueMapMappingTableEntryWrite(0,2,2,&keyIndex,&tableEntry);
            if(GT_OK != rc)
            {
              return rc;
            }
        }/* tc*/
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoBoardCustomConfigSet
*
* DESCRIPTION:
*       The API configures queue mapping, DMA mapping, pizza allocation and high
*       speed port according to a custom configuration.
*
* INPUTS:
*       devNum    - device number
*       boardCustomConfig - (ptr to) A custom board configuration.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS appDemoBoardCustomConfigSet
(
    IN GT_U8  devNum,
    IN const APPDEMO_EXMXPM_PIZZA_AND_QUEUE_MAPPER_CUSTOM_CFG_STC *boardCustomConfig
)
{
    GT_STATUS rc;
    GT_U32  i;
    const APPDEMO_EXMXPM_QUEUE_MAPPER_CUSTOM_CFG_STC *queueMapperParams;

    CPSS_PORTS_BMP_STC portsMembers, portsTagging;
    CPSS_EXMXPM_BRG_VLAN_INFO_STC   vlanInfo;
    GT_BOOL                         isValid;


    /* If there is no custom config*/
    if (boardCustomConfig == NULL)
    {
        return GT_OK;
    }

    /* Get the vlan entry for port members modifying*/
    rc = cpssExMxPmBrgVlanEntryRead(devNum, 1, &portsMembers, &portsTagging, &vlanInfo, &isValid);
    if (rc != GT_OK)
    {
        return rc;
    }
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsMembers);

    /*enable txq mapping for all portgroups */
    rc = cpssExMxPmFabricTxqQueueMapToDmaMappingEnableSet(devNum,0xF, GT_TRUE);
    if( GT_OK != rc )
        return rc;


    /* Queue mapper configuration*/
    if (boardCustomConfig != NULL)
    {
        queueMapperParams = boardCustomConfig->queueMapperCfg;

        /* All non existing ports are mapped to port 13 - dropped*/
        for (i=0;i<PRV_CPSS_EXMXPM3_QUEUE_GROUPS_NUM_CNS;i++)
        {
            rc = prvQueueMapperConfig(devNum, (GT_U8)i , NON_EXIST_PHYSICAL_PORT_NUM_CNS );
            if (rc != GT_OK)
            {
                return rc;
            }
        }


        while (queueMapperParams->physicalPort != 0xFF)
        {
            /* Vlan contains only network ports*/
            if ((queueMapperParams->physicalPort < PRV_CPSS_EXMXPM3_LBP_PHY_PORT_NUM_CNS(devNum)) && (queueMapperParams->virtualPort != 0xFF))
                CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembers,queueMapperParams->virtualPort);


            /* Configure virtual port to downstream txq port*/
            if ((queueMapperParams->downstreamTxqPort != 0xFF) && (queueMapperParams->virtualPort != 0xFF))
            {
                rc = prvQueueMapperConfig(devNum, queueMapperParams->downstreamTxqPort ,
                                      queueMapperParams->virtualPort );
                if (rc != GT_OK)
                {
                    return rc;
                }

                if (PRV_CPSS_PP_MAC(devNum)->revision > 0)
                {
                    rc = cpssExMxPmPortPizzaArbiterWorkConservingEnableSet(devNum, queueMapperParams->downstreamTxqPort,
                                                                           GT_FALSE, queueMapperParams->downWC);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }

            }


            /* Configure virtual port to upstream txq port*/
            if ((queueMapperParams->upstreamTxqPort != 0xFF) && (queueMapperParams->virtualPort != 0xFF))
            {
                rc = prvQueueMapperConfig(devNum, queueMapperParams->upstreamTxqPort ,
                                      queueMapperParams->virtualPort );
                if (rc != GT_OK)
                {
                    return rc;
                }

                if (PRV_CPSS_PP_MAC(devNum)->revision > 0)
                {
                    rc = cpssExMxPmPortPizzaArbiterWorkConservingEnableSet(devNum, queueMapperParams->upstreamTxqPort,
                                                                           GT_FALSE, queueMapperParams->upWC);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }
            }

            /* Map downstream txq port to physical port*/
            if ((queueMapperParams->downstreamTxqPort != 0xFF) && (queueMapperParams->downDMA != 0xFF))
            {
                rc = cpssExMxPmFabricTxqQueueMapPortToDmaTableWrite(devNum, queueMapperParams->downstreamTxqPort, GT_FALSE,
                                                                queueMapperParams->downDMA);
                if(GT_OK != rc)
                    return rc;
            }


            /* Map upstream txq port to physical port*/
            if ((queueMapperParams->upstreamTxqPort != 0xFF) && (queueMapperParams->upDMA != 0xFF))
            {
                rc = cpssExMxPmFabricTxqQueueMapPortToDmaTableWrite(devNum, queueMapperParams->upstreamTxqPort, GT_FALSE,
                                                                queueMapperParams->upDMA);
                if(GT_OK != rc)
                    return rc;
            }

            /* Virtual to TxQ port - for pfc , only network ports*/
            if ((queueMapperParams->downstreamTxqPort != 0xFF) && (queueMapperParams->virtualPort != 0xFF))
            {
                rc = cpssExMxPmPortTxVirtualToTxQPortMapSet(devNum, queueMapperParams->virtualPort, queueMapperParams->downstreamTxqPort);
                if(GT_OK != rc)
                    return rc;
            }


            queueMapperParams++;
        }


        /* Configure VLAN only if at least one port is in the custom vlan config*/
        if (CPSS_PORTS_BMP_IS_ZERO_MAC(&portsMembers) == 0)
        {
            /* Set vlan entry with custom configuration*/
            rc = cpssExMxPmBrgVlanEntryWrite(devNum, 1, &portsMembers, &portsTagging, &vlanInfo);
            if (rc != GT_OK)
            {
                return rc;
            }
        }



        /* Pizza configuration - Set all existing custom txq_dq pizzas*/
        for (i=0;i<PRV_CPSS_EXMXPM_PUMA3_PROCCESSING_CORES_NUM_CNS;i++)
        {
            if (boardCustomConfig->txqPizza[i] != NULL)
            {
                rc = cpssExMxPmTxQPizzaArbiterSet(devNum, 1 << i, boardCustomConfig->pizzaSizes[i] , boardCustomConfig->txqPizza[i] );
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }


        if (boardCustomConfig->highSpeedArray != NULL)
        {
            rc = cpssExMxPmPortTxHighSpeedPortSet(devNum,boardCustomConfig->highSpeedArraySize,&(boardCustomConfig->highSpeedArray[0]));
            if (rc != GT_OK)
            {
                return rc;
            }

            if (PRV_CPSS_PP_MAC(devNum)->revision > 0)
            {
                /* Set scheduler profile as high speed profile for high speed ports*/
                rc = cpssExMxPmPortTxSchedulerHighSpeedEnableSet(devNum, CPSS_EXMXPM3_PORT_TX_HIGH_SPEED_PROFILE, GT_TRUE);
                if (rc != GT_OK)
                {
                    return rc;
                }

                /* Set all high speed ports to use the high speed scheduler profile*/
                for (i=0; i < boardCustomConfig->highSpeedArraySize; i++)
                {
                    rc = cpssExMxPmPortTxSchedulerProfileIndexSet(devNum, boardCustomConfig->highSpeedArray[i],
                                                                  GT_FALSE, CPSS_EXMXPM3_PORT_TX_HIGH_SPEED_PROFILE);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }
            }
        }
    }

    return GT_OK;
}

/* setting for connecting with Aslan device */
static GT_STATUS    extraSystemConfigSet_AslanConnectionPorts
(
    IN GT_U8            devNum,
    IN GT_U8            boardRevId,
    IN GT_U32           fabricPortBitmap
)
{
    GT_STATUS rc;
    GT_U32    j,i,k;
    CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_KEY_INDEX_UNT             keyIndex;
    CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_ENTRY_STC   tableEntry;
    CPSS_PORT_INTERFACE_MODE_ENT    ifMode;

    /* Fabric Port configuration */
    for(j = 32; j < 64; j++) {
        if ((fabricPortBitmap & (1 << (j - 32))) == 0)
        {
            continue;
        }

        /* configure Fabric Descriptor to be 24 bytes and Fabric DSA tag disabled.
          This mode is used for Puma3 to Puma3 connections. */
        rc = cpssExMxPmFabricTxqHeaderMergerPortDescriptorConfigSet(
            devNum, (GT_U8)j, CPSS_EXPMPM_FABRIC_TXQ_HEADER_MERGER_DESCRIPTOR_TYPE_24B_DSA_DISABLE_E);
        if(GT_OK != rc)
        {
            return rc;
        }


        /* currently the Puma3 GM must not be set to internal mac loopback on fabric port ,
           because this damage the 'upstream' ability to duplicate descriptors (like for Rx Mirroring).
           so we only do 'external loopback' with the help of the simulation */
#if defined (ASIC_SIMULATION) && defined (GM_USED) && defined (INCLUDE_UTF)
        simulateExternalLoopbackPortSet(devNum, j, GT_TRUE);
#endif

        /* Check if port exists*/
        rc = cpssExMxPmPortInterfaceModeGet(devNum,(GT_U8)j, &ifMode);
        if(GT_OK != rc)
            return rc;

        /* Do not use per port APIs if port does not exist*/
        if (ifMode == CPSS_PORT_INTERFACE_MODE_NA_E)
            continue;


#ifndef ASIC_SIMULATION
        /* enable MAC loopback */
        if (isLoopbackDisabled[devNum] == GT_FALSE)
        {
            /* CG mac serdes loopback*/
            if (boardRevId == 8)
                rc = cpssExMxPmPortSerdesLoopbackModeSet(devNum, (GT_U8)j, CPSS_EXMXPM_PORT_SERDES_LOOPBACK_ANALOG_TX2RX_E);
            else
                rc = cpssExMxPmPortInternalLoopbackEnableSet(devNum, (GT_U8)j, GT_TRUE);

            if(GT_OK != rc)
            {
                return rc;
            }
        }
#endif

        /* configure MRU to be maximal value on initialized ports*/
        if (ifMode != CPSS_PORT_INTERFACE_MODE_NA_E)
        rc = cpssExMxPmPortMruSet(devNum, (GT_U8)j, (_10K + 64));
        if(GT_OK != rc)
        {
            return rc;
        }
    }

         /*Table 0 Configuration 0x3b08000:
    Entries corresponding to source ports 0-11 should be sent to queues 256-351 where {Port0,Tc0} is sent to queue 256 and {Port11,Tc7} is sent to queue 351.
    Entries corresponding to source ports 16-27 should be sent to queues 384-479 where {Port16,Tc0} is sent to queue 384 and {Port27,Tc7} is sent to queue 479.
    Entries corresponding to the CPU Port (63) are sent to queues 376-383 according to the TC and entries corresponding to the Loopback port (28) are sent to queues 504-511 according to the TC.
    */
    tableEntry.forceDp = GT_FALSE;
    tableEntry.dp = CPSS_DP_GREEN_E;

    for (i = 0; i < 8; i++){ /* packet type[11:9] index*/

      switch (i)
      {
        case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_FROM_CPU_E:
        case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_TO_CPU_E:
        case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_INGRESS_MIRROR_E:
        case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_EGRESS_MIRROR_E:
        case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_FORWARD_E:
            break;
        default:
            continue;
      }
      keyIndex.indexKey1.packetType = i;

      for (j = 0; j <= 63; j++)  /* Local device source port[8:3] index*/
      {
          if (((j>=12)&& (j<16)) || ((j>27)&& (j!=CPSS_CPU_PORT_NUM_CNS) && (j!=LOOPBACK_PORT_VIRTUAL_PORT_NUM_CNS))) /* cpu 63 and lbp 56*/
              continue;

          keyIndex.indexKey1.localDevSrcPort =(GT_U8) j;

          for (k = 0; k < 8; k++) /* TC[2:0] index*/
          {
              keyIndex.indexKey1.fabricTc = k;

              if (j == CPSS_CPU_PORT_NUM_CNS)
                  tableEntry.queueNum = 376 + k;
              else if (j == LOOPBACK_PORT_VIRTUAL_PORT_NUM_CNS)
                  tableEntry.queueNum = 504 + k;
              else
                  tableEntry.queueNum = 256 + j*8+k;

              /* GM TxQ port 41-47,57-63 does not send traffic*/
              #ifdef GM_USED
              if (j>=9)
                  tableEntry.queueNum = 256 + k;

              if (j>=25)
                  tableEntry.queueNum = 384 + k;

              #endif
              rc = cpssExMxPmFabricTxqQueueMapMappingTableEntryWrite(devNum,1,0,&keyIndex,&tableEntry);
              if(GT_OK != rc)
              {
                return rc;
              }
          }/* tc*/
      }/* src port*/
    }

    /* Use work conserving for Puma 3 B0 upstraem lb port when there is no custom config*/
    if ((PRV_CPSS_PP_MAC(currentDevNum)->revision > 0) && (CPSS_EXMXPM3_CUSTOM_CONFIG(currentDevNum,boardRevId) == NULL))
    {
        rc = cpssExMxPmPortPizzaArbiterWorkConservingEnableSet(currentDevNum, 63, GT_FALSE, GT_TRUE);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    /*Port to TxDma Mapping:
        This mapping provides the link between the queue and the fabric port through which the packets of the queue will be transmitted.
        */
    for (i=2;i<=3;i++)
    {
        for (j=0;j<12;j++)
        {
            rc = cpssExMxPmFabricTxqQueueMapPortToDmaTableWrite(devNum,(GT_U8)( i*16+j), GT_FALSE,(GT_U8)( i*16+((j<9)?((j/3)*4):9)));
            if(GT_OK != rc)
            {
                return rc;
            }
        }
    }

    rc = cpssExMxPmFabricTxqQueueMapPortToDmaTableWrite(devNum, 63, GT_FALSE, 57);/*(Packets from Loopbackport)  */
    if(GT_OK != rc)
    {
        return rc;
    }

    rc = cpssExMxPmFabricTxqQueueMapPortToDmaTableWrite(devNum, 47, GT_FALSE, 41);/*(Packets from CPU Port)  */
    if(GT_OK != rc)
    {
        return rc;
    }

    return  GT_OK;
}


static GT_STATUS     extraSystemConfigSet_PetraConnectionPorts
(
    IN GT_U8  devNum,
    IN GT_U8  boardRevId,
    IN GT_U32 fabricPortBitmap
)
{
    GT_STATUS rc;
    GT_U32    j,i,k;
    CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_KEY_INDEX_UNT             keyIndex;
    CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_ENTRY_STC   tableEntry;
    CPSS_EXMXPM_FABRIC_TXQ_HEADER_MERGER_TARGET_DEV_INFO_ENTRY_STC tempStc;
    GT_BOOL     isInterlaken24 = GT_TRUE;

    isInterlaken24 = ((boardRevId== 5) || (boardRevId== 6) || (boardRevId== 11))?GT_FALSE:GT_TRUE;

    /* Descriptor Merger and Fabric Header Port%p (MG0 only):
    Merger Enabled, Merger data size is  22B (MFPD), ITMH, enabled (will be suppressed by Metal Fix),  No ByteSwap,
    Descriptor at head of the packet and no Fabric DSA sent.*/
    rc = cpssExMxPmFabricTxqHeaderMergerPortDescriptorConfigSet(devNum,32,(PRV_CPSS_PP_MAC(devNum)->revision > 0)?
                                                                    CPSS_EXPMPM_FABRIC_TXQ_HEADER_MERGER_DESCRIPTOR_TYPE_25B_ILKN_ITMH_DISABLE_E:
                                                                    CPSS_EXPMPM_FABRIC_TXQ_HEADER_MERGER_DESCRIPTOR_TYPE_22B_ILKN_ITMH_ENABLE_E);
    if(GT_OK != rc)
        return rc;

    if (isInterlaken24 == GT_FALSE)
    {
        rc = cpssExMxPmFabricTxqHeaderMergerPortDescriptorConfigSet(devNum,48,(PRV_CPSS_PP_MAC(devNum)->revision > 0)?
                                                                    CPSS_EXPMPM_FABRIC_TXQ_HEADER_MERGER_DESCRIPTOR_TYPE_25B_ILKN_ITMH_DISABLE_E:
                                                                    CPSS_EXPMPM_FABRIC_TXQ_HEADER_MERGER_DESCRIPTOR_TYPE_22B_ILKN_ITMH_ENABLE_E);
        if(GT_OK != rc)
        return rc;
    }


#ifdef ASIC_SIMULATION
    /*
       currently the Puma3 GM must not be set to internal mac loopback on fabric port ,
       because this damage the 'upstream' ability to duplicate descriptors (like for Rx Mirroring).

       so we only do 'external loopback' with the help of the simulation
   */
    #ifdef  INCLUDE_UTF
        simulateExternalLoopbackPortSet(devNum, 32, GT_TRUE);
        if (isInterlaken24 == GT_FALSE)
            simulateExternalLoopbackPortSet(devNum, 48, GT_TRUE);
    #endif /*INCLUDE_UTF */
#endif
    /*When working in ILKN24 mode, only Core2 is active and all the queueing is performed to queues 256-383.
    Because there aren't enough queues for allocating one queue per {source port,tc},
    aggregation of two TCs to the same queue is performed. Two TCs of the same port are enqueued in the same queue
    instead of enqueueing the same TC of two ports because it has an impact on the Channelized Flow control:
    a channel will stop two ports instead of stopping two TCs of the same port.*/

    /*Entries corresponding to source ports 0-11 should be sent to queues 256-303. Every two TCs of the same source port are sent to the same queue, where {Port0,Tc0} and{Port0,Tc1} are sent to queue 256 and {Port11,Tc6} and {Port11,Tc7} are sent to queue 303.
    Entries corresponding to source ports 16-27 should be sent to queues 304-351. Every two TCs of the same source port are sent to the same queue, where {Port16,Tc0} and {Port16,Tc1} are sent to queue 304 and {Port27,Tc6} and {Port27,Tc7} are sent to queue 351.
    Entries corresponding to the CPU Port (63) are sent to queues 376-383 according to the TC and entries corresponding to the Loopback port (28) are sent to queues 368-375 according to the TC.
    */
    tableEntry.forceDp = GT_FALSE;
    tableEntry.dp = CPSS_DP_GREEN_E;

    for (i = 0; i < 8; i++){ /* packet type[11:9] index*/
          switch (i)
          {
            case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_FROM_CPU_E:
            case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_TO_CPU_E:
            case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_INGRESS_MIRROR_E:
            case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_EGRESS_MIRROR_E:
            case CPSS_EXMXPM_FABRIC_TXQ_QUEUE_MAP_MAPPING_TABLE_PACKET_TYPE_FORWARD_E:
                break;
            default:
                continue;
          }
          keyIndex.indexKey1.packetType = i;

          for (j = 0; j <= 63; j++)  /* Local device source port[8:3] index*/
          {
              if (((j>=12)&& (j<16)) || ((j>28)&& (j!=CPSS_CPU_PORT_NUM_CNS)&& (j != LOOPBACK_PORT_VIRTUAL_PORT_NUM_CNS))) /* cpu 63 and lbp 56*/
                  continue;

              keyIndex.indexKey1.localDevSrcPort =(GT_U8) j;

              for (k = 0; k < 8; k++) /* TC[2:0] index*/
              {
                  keyIndex.indexKey1.fabricTc = k;

                  if (isInterlaken24 == GT_TRUE) /* Every two tcs are queued in the same queue, all in core 2*/
                  {
                  if (j == CPSS_CPU_PORT_NUM_CNS)
                      tableEntry.queueNum = 376 + k;
                  else if (j==LOOPBACK_PORT_VIRTUAL_PORT_NUM_CNS)
                      tableEntry.queueNum = 368 + k;
                  else if (j<12)
                      tableEntry.queueNum = 256 + j*4+k/2;
                  else
                      tableEntry.queueNum = 304 + (j-16)*4+k/2;
                  }
                  else /* ILKN12&16 are similar to 40G, queue per tc, both cores are used*/
                  {
                      if (j == CPSS_CPU_PORT_NUM_CNS)
                          tableEntry.queueNum = 376 + k;
                      else if (j == LOOPBACK_PORT_VIRTUAL_PORT_NUM_CNS)
                          tableEntry.queueNum = 504 + k;
                      else
                          tableEntry.queueNum = 256 + j*8+k;
                  }


                  rc = cpssExMxPmFabricTxqQueueMapMappingTableEntryWrite(devNum,1,0,&keyIndex,&tableEntry);
                  if(GT_OK != rc)
                  {
                    return rc;
                  }
              }/* tc*/
          }/* src port*/
    }

    /* Use work conserving for Puma 3 B0 when there is no custom config*/
    if ((PRV_CPSS_PP_MAC(devNum)->revision > 0) && (CPSS_EXMXPM3_CUSTOM_CONFIG(devNum,boardRevId) == NULL))
    {
        if (isInterlaken24 == GT_TRUE)
        {
            for (j=32;j<44;j++)
            {
                rc = cpssExMxPmPortPizzaArbiterWorkConservingEnableSet(devNum, (GT_U8)j, GT_FALSE, GT_TRUE);
                if (rc != GT_OK)
                {
                    return rc;
                }
            }
        }

        /* Use work conserving for Puma 3 B0 upstraem lb port when there is no custom config*/
        rc = cpssExMxPmPortPizzaArbiterWorkConservingEnableSet(currentDevNum, (GT_U8)((isInterlaken24 == GT_TRUE)?46:63), GT_FALSE, GT_TRUE);
        if (rc != GT_OK)
        {
            return rc;
        }
    }




    /*Port to TxDma Mapping:
        This mapping provides the link between the queue and the fabric port through which the packets of the queue will be transmitted.
        The ILKN port is Port0 of the fabric, therefore all the entries are configured to 0.
        The TxQ to Dma mapping should be enabled for the upstream packets therefore the following register should be configured:
        */
    if (isInterlaken24 == GT_TRUE)
    {
        for (j=0;j<12;j++)
        {
            rc = cpssExMxPmFabricTxqQueueMapPortToDmaTableWrite(devNum, (GT_U8)(32+j), GT_FALSE, 32);
            if(GT_OK != rc)
                    return rc;
        }

        rc = cpssExMxPmFabricTxqQueueMapPortToDmaTableWrite(devNum, 46, GT_FALSE, 32);/* (Packets from the loopback port)*/
        if(GT_OK != rc)
            return rc;

        rc = cpssExMxPmFabricTxqQueueMapPortToDmaTableWrite(devNum, 47, GT_FALSE, 32);/* (Packets from CPU Port)*/
        if(GT_OK != rc)
            return rc;

    }
    else /* Map all network0 ports to fabric 0 and all network 1 ports to fabric 1*/
    {
        for (i=0;i<2;i++)
        {
            for (j=0;j<12;j++)
            {
                rc = cpssExMxPmFabricTxqQueueMapPortToDmaTableWrite(devNum, (GT_U8)(32+j+i*16), GT_FALSE, (GT_U8)(32+i*16));
                if(GT_OK != rc)
                    return rc;
            }
        }

        rc = cpssExMxPmFabricTxqQueueMapPortToDmaTableWrite(devNum, 63, GT_FALSE, 48);/* (Packets from the loopback port)*/
        if(GT_OK != rc)
            return rc;

        rc = cpssExMxPmFabricTxqQueueMapPortToDmaTableWrite(devNum, 47, GT_FALSE, 32);/* (Packets from CPU Port)*/
        if(GT_OK != rc)
            return rc;
    }


    /* Initialize merger tables to 0*/
    cpssOsMemSet(&tempStc,0,sizeof(tempStc));
    for (i=0;i<PRV_CPSS_MAX_PP_DEVICES_CNS;i++)
    {
        cpssExMxPmFabricTxqHeaderMergerTargetDeviceInfoWrite(currentDevNum,(GT_U8)i,&tempStc);
        cpssExMxPmFabricTxqHeaderMergerCpuCodeMappingWrite(currentDevNum,(GT_U8)i,0);
    }

    /*TxQ Ports Arbiter Configuration:
        Since in ILKN24 two ports are configured to the same set of queues of a specific TxQ port, one of the port is enqueued in the lower queues while the other is enqueued in the higher queues which normally have higher priority,
    giving advantage to the port enqueued to the higher queues. To avoid this advantage, the queues work using weighted round robin, when the queues used for the same TCs have the same weight, and the weight is higher for the queues allocating higher TCs.
    */
    if (isInterlaken24 == GT_TRUE) /* Two ports are mapped to one TxQ port, give them equal priority*/
    {
        /* Set priorities: 7&3 - 8,  6&2 - 4,  5&1 - 2, 4&0 - 1   on profile 1*/
        for (i=0;i<TC_MAX;i++)
        {
            rc =  cpssExMxPmPortTxSchedulerWrrProfileSet(devNum, 1, i, 1<<(i%4) );
            if (rc != GT_OK)
            {
                return rc;
            }
        }

        /* Set port 32 to profile 1*/
        rc = cpssExMxPmPortTxSchedulerProfileIndexSet(devNum, 32, GT_FALSE, 1);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/* debug function : print utility */
GT_STATUS prvCpssExMxPmVbDbVirtualPortToGlobalPortConvertGet_print(
    IN GT_U8    devNum
)
{
    GT_STATUS   rc;
    GT_U32  ii;
    GT_U8   globalPhysicalPort;

    osPrintf(" the CPSS DB of virtual port to physical port \n");
    osPrintf("\n");
    osPrintf(" virtual port |   physical port  \n");
    osPrintf(" ------------------------------- \n");

    for(ii = 0 ; ii < 64 ; ii++)
    {
        rc = prvCpssExMxPmVbDbVirtualPortToGlobalPortConvertGet(devNum,(GT_U8)ii,&globalPhysicalPort);
        if(rc != GT_OK)
        {
            osPrintf(" %2.2d   |   error(rc = 0x%x) \n" , ii,rc);
            continue;
        }

        osPrintf(" %2.2d   |   %2.2d \n" , ii , globalPhysicalPort);
    }
    osPrintf("\n");

    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmPowerDownBmp1Set
*
* DESCRIPTION:
*       Debug function that sets AppDemo power down SERDES bitmask.
*   ---> set word[1] in the bmp ... see function appDemoExMxPmPowerDownBmpSet for word[0]
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
GT_STATUS appDemoExMxPmPowerDownBmp1Set
(
    IN GT_U32   powerDownBmp1
)
{
    appDemoExMxPmPowerDownBmp1 = powerDownBmp1;
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
    GT_U8       i;                      /* Loop index.                  */
    GT_STATUS   pciStat;
    GT_U32      usedDevices[APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS]={0};
    GT_U32      index=0,devIndex0,activeGroupsNum;
    GT_U32      value;
    GT_BOOL     foundFirstDev;
    GT_U8       firstDev;
    GT_U32      activePortGroupsBmp = 0x3;

    /* ability to set different active port group bitmap */
    if(appDemoDbEntryGet((GT_CHAR*)"activePortGropsBmp", &value) == GT_OK)
    {
        activePortGroupsBmp = value;
    }

    pciStat = appDemoSysGetPciInfo();

    if(pciStat != GT_OK)
    {
        return GT_NOT_FOUND;
    }

    foundFirstDev = GT_FALSE;
    firstDev = 0;
    for(i = 0; i < APP_DEMO_PP_CONFIG_SIZE_CNS; i++)
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

            if(((1<<(i-firstDev)) & activePortGroupsBmp) == 0)
            {
                appDemoPpConfigList[i].valid = GT_FALSE;
                continue;
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

    /* set the first device to hold info about all the port groups */
    for(i = 0, index = 0, activeGroupsNum=0; i < PUMA3_MANAGEMENT_BUS_NUM_CNS; i++, index++)
    {
        /*invalid all the devices , the first device will be validated after the loop */
        appDemoPpConfigList[usedDevices[index]].valid = GT_FALSE;

        if(((1<<i) & activePortGroupsBmp) == 0)
        {
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupActive = GT_FALSE;
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupPciInfo.pciBaseAddr = CPSS_PARAM_NOT_USED_CNS;
            appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupPciInfo.internalPciBase = CPSS_PARAM_NOT_USED_CNS;
            continue;
        }

        appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupActive = GT_TRUE;

        appDemoPpConfigList[devIndex0].portGroupsInfo[i].portGroupPciInfo =
            appDemoPpConfigList[usedDevices[index]].pciInfo;

        activeGroupsNum++;
    }

    /* the Puma3 device has 4 port groups - each MG controls network port group and fabric port group */
    appDemoPpConfigList[devIndex0].numOfPortGroups = PUMA3_MANAGEMENT_BUS_NUM_CNS * 2;

    /* duplicate the first 2 port groups to another 2 port groups */
    for(i = 0 ; i < PUMA3_MANAGEMENT_BUS_NUM_CNS; i++)
    {
        appDemoPpConfigList[devIndex0].portGroupsInfo[i + PUMA3_MANAGEMENT_BUS_NUM_CNS] =
            appDemoPpConfigList[devIndex0].portGroupsInfo[i];
    }

    /* validate the first device */
    appDemoPpConfigList[devIndex0].valid = GT_TRUE;

    appDemoPpConfigList[devIndex0].pciInfo.pciBaseAddr = CPSS_PARAM_NOT_USED_CNS;
    appDemoPpConfigList[devIndex0].pciInfo.internalPciBase =  CPSS_PARAM_NOT_USED_CNS;


    if(activeGroupsNum != numOfActivePortGroups)
    {
        /* the bmp of active port groups filtered one of the existing port groups */
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
    GT_U32  ii;
    GT_U32                        value;
    APP_DEMO_CPSS_LPM_MEM_CFG_ENT lpmMemoryMode=APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E;

    APPDEMO_DRAM_FREQUENCY_ENT          targetFreqLookup;
    APPDEMO_DRAM_FREQUENCY_ENT          targetFreqFwr;
    GT_U32                              data,regData;
    GT_STATUS                           rc = GT_OK;
    APPDEMO_EXMXPM_EXT_DRAM_STATIC_CFG  *lookupStaticParamsPtr;
    APPDEMO_EXMXPM_EXT_DRAM_STATIC_CFG  *fwdStaticParamsPtr;
    GT_U32                              lookupNumOfCfg=0;
    GT_U32                              fwdNumOfCfg=0;
    APPDEMO_BOARD_TYPE                  boardType;
    GT_BOOL                             doByteSwap=GT_FALSE;

    localPpPh1Config.devNum             = devIdx;
    localPpPh1Config.baseAddr           =
    appDemoPpConfigList[devIdx].pciInfo.pciBaseAddr;
    localPpPh1Config.internalPciBase    =
    appDemoPpConfigList[devIdx].pciInfo.internalPciBase;
    localPpPh1Config.deviceId           =
    ((appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.devId << 16) |
     (appDemoPpConfigList[devIdx].pciInfo.pciDevVendorId.vendorId));

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

        if( ii < PUMA3_MANAGEMENT_BUS_NUM_CNS)
        {
            extDrvGetPciIntVec(GT_PCI_INT_D - ii , &intVecNum);
            localPpPh1Config.portGroupsInfo[ii].intVecNum = (GT_U32)((GT_UINTPTR)intVecNum);
            extDrvGetIntMask(GT_PCI_INT_D - ii , &localPpPh1Config.portGroupsInfo[ii].intMask);
        }
        else
        {
            localPpPh1Config.portGroupsInfo[ii].intVecNum =
                localPpPh1Config.portGroupsInfo[ii - PUMA3_MANAGEMENT_BUS_NUM_CNS].intVecNum;
            localPpPh1Config.portGroupsInfo[ii].intMask =
                localPpPh1Config.portGroupsInfo[ii - PUMA3_MANAGEMENT_BUS_NUM_CNS].intMask;
        }
    }

    localPpPh1Config.mngInterfaceType                      = CPSS_CHANNEL_PEX_E;



    /* set all external configuration */
    if(appDemoDbEntryGet("allExternal", &value) == GT_OK)
    {
        switch(value)
        {
        case 0:
            /* use defaults - do nothing */
            break;
        case 1:
            lpmMemoryMode = APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;
            break;
        default:
            return GT_BAD_PARAM;
        }
    }
    /* LPM memory configuration */
    if(appDemoDbEntryGet("lpmMemoryMode", &value) == GT_OK)
    {
        switch(value)
        {
        case 0:
            lpmMemoryMode = APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E;
            break;
        case 1:
            lpmMemoryMode = APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;
            break;
        case 2:
            lpmMemoryMode = APP_DEMO_CPSS_EXMXPM_LPM_SINGLE_INTERNAL_E;
            break;
        case 3:
            lpmMemoryMode = APP_DEMO_CPSS_EXMXPM_LPM_DOUBLE_INTERNAL_E;
            break;
        default:
            return GT_BAD_PARAM;
        }
    }

    switch (lpmMemoryMode)
    {
    case APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E:
        localPpPh1Config.routingSramCfgType = CPSS_TWIST_EXTERNAL_E;
        localPpPh1Config.numOfSramsCfg = 0;
        break;

    case APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E:
        localPpPh1Config.routingSramCfgType = CPSS_TWIST_INTERNAL_E;
        localPpPh1Config.numOfSramsCfg = 0;
        break;
    case APP_DEMO_CPSS_EXMXPM_LPM_SINGLE_INTERNAL_E:
        localPpPh1Config.routingSramCfgType = CPSS_ONE_INTERNAL_E;
        localPpPh1Config.numOfSramsCfg = 0;
        break;
    case APP_DEMO_CPSS_EXMXPM_LPM_DOUBLE_INTERNAL_E:
        localPpPh1Config.routingSramCfgType = CPSS_TWO_INTERNAL_E;
        localPpPh1Config.numOfSramsCfg = 0;
        break;
    default:
        return GT_BAD_PARAM;
    }


    /* retrieve PP Core Clock from HW */
    localPpPh1Config.coreClk = APP_DEMO_CPSS_AUTO_DETECT_CORE_CLOCK_CNS;

    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&localPpPh1Config.powerDownPortsBmp);
    localPpPh1Config.powerDownPortsBmp.ports[0] = appDemoExMxPmPowerDownBmp;
    localPpPh1Config.powerDownPortsBmp.ports[1] = appDemoExMxPmPowerDownBmp1;

    localPpPh1Config.externalTcamUsed = GT_FALSE;
    if (appDemoPuma3ExtTcamEnabled == GT_TRUE)
    {
        localPpPh1Config.externalTcamUsed = GT_TRUE;
    }

    localPpPh1Config.ingressBufferSize = APPDEMO_EXMXPM_BUFFER_SIZE_256_E;
    localPpPh1Config.egressBufferSize = APPDEMO_EXMXPM_BUFFER_SIZE_512_E;

    /* our boards have 2Gbit DRAM size */
    localPpPh1Config.extDramCfg.luDunitDramSize = CPSS_DRAM_256MB_E;
    localPpPh1Config.extDramCfg.fwdDunitDramSize = CPSS_DRAM_256MB_E;

     /* need to create the dramStaticParams according to board type */
    if(localPpPh1Config.extDramCfg.externalDramInUse==GT_TRUE)

    {
#ifdef ASIC_SIMULATION
        data = 0;
        regData = 0;
        targetFreqLookup = APPDEMO_DRAM_FREQ_800_MHZ_E;
        targetFreqFwr = APPDEMO_DRAM_FREQ_800_MHZ_E;
        doByteSwap = doByteSwap; /* Compiler warning*/
#else

        /* find if byte swap is needed */
        rc = cpssExMxPmDiagRegRead(
            appDemoPpConfigList[devIdx].portGroupsInfo[0].portGroupPciInfo.pciBaseAddr,
            CPSS_DIAG_PP_REG_INTERNAL_E,
            0x50,
            &regData,
            doByteSwap);

        if( GT_OK != rc )
        {
            return rc;
        }

        if( 0x11AB != regData)
        {
            doByteSwap = GT_TRUE;
        }


        /* Get bits from Functional SAR Status LSB[30:22]  */
        /* Bits[23:22] -->DDR WIDE0 PLL, Bits[25:24] -->DDR WIDE1 PLL
           Bits[27:26] -->DDR NARROW0 PLL, Bits[29:28] --> DDR NARROW1 PLL
           Bits[31:30] -->DDR NARROW2 PLL.
           Data: 0x0-->800MHz, 0x1 --> 667MHz, 0x2 --> 400MHz, 0x3-->PLL bypass*/

        rc = cpssExMxPmDiagRegRead(
            appDemoPpConfigList[devIdx].portGroupsInfo[0].portGroupPciInfo.pciBaseAddr,
            CPSS_DIAG_PP_REG_INTERNAL_E,
            0x044f8200,/*functionalSARStatusLSB*/
            &regData,
            doByteSwap);

        if( GT_OK != rc )
        {
            return rc;
        }

        /* only bits 22 to 31 is relevant for target frequence */
        data = ((regData & 0xFFC00000) >> 22);
        switch(data)
        {
            case 0:/* bits 22,24,26,28,30 are 0 */
                    targetFreqLookup = APPDEMO_DRAM_FREQ_800_MHZ_E;
                    targetFreqFwr = APPDEMO_DRAM_FREQ_800_MHZ_E;
                    break;
            case 341:/* bits 22,24,26,28,30 are 1 */
                    targetFreqLookup = APPDEMO_DRAM_FREQ_667_MHZ_E;
                    targetFreqFwr = APPDEMO_DRAM_FREQ_667_MHZ_E;
                    break;
            default:/* any other variation is not supported at the moment */
                return GT_BAD_PARAM;
        }
#endif

        if(isModularBoard[devIdx]==GT_TRUE)
        {
            boardType = APPDEMO_BOARD_TYPE_SLM_1230_E;
        }
        else
        {
            boardType = APPDEMO_BOARD_TYPE_SLM_1236_E;
        }

        /* get lookup static params to be configured */
        rc = getDramStaticCfg(targetFreqLookup,
                                           APPDEMO_EMC_TYPE_LOOKUP_E,
                                           boardType,
                                           &lookupStaticParamsPtr,
                                           &lookupNumOfCfg);
        if(rc!=GT_OK)
            return rc;


        /* set new parameters */
        localPpPh1Config.extDramCfg.luDramStaticParams = lookupStaticParamsPtr;
        localPpPh1Config.extDramCfg.luDramStaticNumOfCfg = lookupNumOfCfg;

        /* get forward static params to be configured */
        rc = getDramStaticCfg(targetFreqFwr,
                                           APPDEMO_EMC_TYPE_FORWARD_E,
                                           boardType,
                                           &fwdStaticParamsPtr,
                                           &fwdNumOfCfg);

        if(rc!=GT_OK)
            return rc;

        localPpPh1Config.extDramCfg.fwdDramStaticParams = fwdStaticParamsPtr;
        localPpPh1Config.extDramCfg.fwdDramStaticNumOfCfg = fwdNumOfCfg;
    }

    osMemCpy(phase1Params,&localPpPh1Config,sizeof(CPSS_PP_PHASE1_INIT_PARAMS));
    return GT_OK;
}


/*
 * Typedef: struct PRV_PUMA3_EXT_TCAM_CFG_STC
 *
 * Description:
 *        This structure contains info about NL TCAM Interface configuration
 *
 * Fields:
 *        phyRegAddr  - NL TCAM MDIO register address
 *        phyRegData  - NL TCAM MDIO register data
 *        sleepTime  - sleep time after regsister write
 *
 * Comments:
 *
 */
typedef struct
{
    GT_U32 phyRegAddr;
    GT_U16 phyRegData;
    GT_U32 sleepTime;
} PRV_PUMA3_EXT_TCAM_CFG_STC;

/*
 * Typedef: struct PRV_CPSS_EXMXPM_PUMA3_TCAM_SERDES_POLARITY_INVERT_STC
 *
 * Description:
 *        This structure contains info about serdeses which require
 *          polarity invert on any direction (TX/RX)
 *
 * Fields:
 *        serdesNum - number of serdes in core
 *        invertTx  - polarity invert required for TX
 *        invertRx  - polarity invert required for RX
 *
 * Comments:
 *
 */
typedef struct PRV_CPSS_EXMXPM_PUMA3_TCAM_SERDES_POLARITY_INVERT_STC
{
    GT_U32  serdesNum;
    GT_BOOL invertTx;
    GT_BOOL invertRx;
} PRV_CPSS_EXMXPM_PUMA3_TCAM_SERDES_POLARITY_INVERT_STC;

static PRV_CPSS_EXMXPM_PUMA3_TCAM_SERDES_POLARITY_INVERT_STC appDemoInvertArr[] =
{
        /* serdes invTx invRx */
         {34, GT_TRUE, GT_FALSE}
        ,{35, GT_TRUE, GT_FALSE}
        ,{37, GT_TRUE, GT_FALSE}
        ,{39, GT_TRUE, GT_FALSE}
        ,{41, GT_TRUE, GT_FALSE}
        ,{46, GT_TRUE, GT_FALSE}
        ,{48, GT_TRUE, GT_FALSE}
        ,{50, GT_TRUE, GT_FALSE}
        ,{52, GT_TRUE, GT_FALSE}
        ,{55, GT_TRUE, GT_FALSE}
        ,{0xFFFF, GT_FALSE, GT_FALSE}
};

/*
Tx lanes: 2; 3; 5; 7; 11; 13
Rx lanes: 5
First lane of TCAM ILNK is 32.
*/
static PRV_CPSS_EXMXPM_PUMA3_TCAM_SERDES_POLARITY_INVERT_STC appDemoModularInvertArr[] =
{
        /* serdes invTx invRx */
         {34, GT_TRUE, GT_FALSE}
        ,{35, GT_TRUE, GT_FALSE}
        ,{37, GT_TRUE, GT_TRUE}
        ,{39, GT_TRUE, GT_FALSE}
        ,{43, GT_TRUE, GT_FALSE}
        ,{45, GT_TRUE, GT_FALSE}
        /* disable SA boards configurations to be on safe side */
        ,{41, GT_FALSE, GT_FALSE}
        ,{46, GT_FALSE, GT_FALSE}
        ,{48, GT_FALSE, GT_FALSE}
        ,{50, GT_FALSE, GT_FALSE}
        ,{52, GT_FALSE, GT_FALSE}
        ,{55, GT_FALSE, GT_FALSE}
        ,{0xFFFF, GT_FALSE, GT_FALSE}
};

/*******************************************************************************
* prvPuma3TcamIlnkInit
*
* DESCRIPTION:
*       Init Puma3 TCAM ILNK interface.
*
* INPUTS:
*       devNum      - device number
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
GT_STATUS prvPuma3TcamIlnkInit
(
    GT_U8 devNum
)
{
    GT_U32  i = 0; /* iterator */
    PRV_CPSS_EXMXPM_PUMA3_TCAM_SERDES_POLARITY_INVERT_STC * invertArr = appDemoInvertArr;
    GT_STATUS rc;

    rc = mvHwsPortInit(
        devNum, 0 /*portGroup*/,
        60 /*phyPortNum*/, TCAM /*portMode*/, GT_FALSE /*lbPort*/,
        MHz_156 /*refClock*/,
        PRIMARY_LINE_SRC  /*refClockSource*/);
    if (rc != GT_OK)
    {
        cpssOsPrintf("mvHwsPortInit fail: %d\n", rc);
        return rc;
    }

    if (isModularBoard[devNum]== GT_TRUE)
    {
        invertArr = appDemoModularInvertArr;
    }

    while (invertArr[i].serdesNum != 0xFFFF)
    {
        rc = mvHwsSerdesPolarityConfig(devNum,
                                       0 /*portGroup*/,
                                       invertArr[i].serdesNum, /* serdesNum */
                                       COM_PHY_H_REV2,
                                       invertArr[i].invertTx,
                                       invertArr[i].invertRx);
        if(rc != GT_OK)
        {
            cpssOsPrintf("mvHwsSerdesPolarityConfig fail: serdes %d %d\n", invertArr[i].serdesNum, rc);
            return rc;
        }

        i++;
    }

    return GT_OK;
}

static PRV_PUMA3_EXT_TCAM_CFG_STC  appDemoNlCfgArr[] =
{{0x8108, 0x3, 50},
 {0x811b, 0x40, 50},
 {0x811b, 0x0   , 50},
 {0x8117, 0xfff , 50},
 {0x8118, 0xffff, 50},
 {0x8119, 0xff  , 50},
 {0x810c, 0x0   , 50},
 {0x811c, 0x7   , 50},
 {0x811c, 0x0   , 100},
 {0x810c, 0x3   , 50},
 {0x8119, 0x0   , 50},
 {0x8118, 0x0   , 50},
 {0x8117, 0x0   , 50},
 {0x811b, 0x40  , 50},
 {0x811b, 0x0   , 50},
 {0x811e, 0x7   , 50}};

static PRV_PUMA3_EXT_TCAM_CFG_STC  appDemoModularNlCfgArr[] =
{ {0x8117, 0xfff , 50},
  {0x810c, 0x0   , 50},
  {0x811c, 0x7   , 50},
  {0x8108, 0x3, 50},
  {0x811c, 0x0   , 100},
  {0x810c, 0x3   , 50},
  {0x8117, 0x0   , (50+100)}};

static PRV_PUMA3_EXT_TCAM_CFG_STC  appDemoModularNlCfg2Arr[] =
{ {0x811b, 0x40  , 50},
  {0x811b, 0x0   , 50}};

/*******************************************************************************
* prvPuma3TcamNlInit
*
* DESCRIPTION:
*       Init NL TCAM ILNK interface.
*
* INPUTS:
*       devNum      - device number
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
GT_STATUS prvPuma3TcamNlInit
(
    GT_U8 devNum
)
{
    GT_U32      ii;     /* iterator */
    GT_U32      cfgSize; /* table size */
    GT_STATUS   rc;      /* return code */

    cfgSize = sizeof(appDemoNlCfgArr) / sizeof(appDemoNlCfgArr[0]);

    for (ii = 0; ii < cfgSize; ii++)
    {
        rc = cpssExMxPmPhyPort10GSmiRegisterWrite(devNum, 31, (GT_U16)appDemoNlCfgArr[ii].phyRegAddr, 1, appDemoNlCfgArr[ii].phyRegData);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPort10GSmiRegisterWrite", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        cpssOsTimerWkAfter(appDemoNlCfgArr[ii].sleepTime);
    }

    return GT_OK;
}

/*******************************************************************************
* prvPuma3ModularTcamNlInit
*
* DESCRIPTION:
*       Init NL TCAM ILNK interface.
*
* INPUTS:
*       devNum      - device number
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
GT_STATUS prvPuma3ModularTcamNlInit
(
    GT_U8 devNum
)
{
    GT_U32      ii;     /* iterator */
    GT_U32      cfgSize; /* table size */
    GT_STATUS   rc;      /* return code */

    cfgSize = sizeof(appDemoModularNlCfgArr) / sizeof(appDemoModularNlCfgArr[0]);

    for (ii = 0; ii < cfgSize; ii++)
    {
        rc = cpssExMxPmPhyPort10GSmiRegisterWrite(devNum, 31, (GT_U16)appDemoModularNlCfgArr[ii].phyRegAddr, 1, appDemoModularNlCfgArr[ii].phyRegData);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPort10GSmiRegisterWrite", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        cpssOsTimerWkAfter(appDemoModularNlCfgArr[ii].sleepTime);
    }

    return GT_OK;
}

/*******************************************************************************
* prvPuma3ModularTcamNlInit2
*
* DESCRIPTION:
*       Init NL TCAM ILNK interface.
*
* INPUTS:
*       devNum      - device number
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
GT_STATUS prvPuma3ModularTcamNlInit2
(
    GT_U8 devNum
)
{
    GT_U32      ii;     /* iterator */
    GT_U32      cfgSize; /* table size */
    GT_STATUS   rc;      /* return code */

    cfgSize = sizeof(appDemoModularNlCfg2Arr) / sizeof(appDemoModularNlCfg2Arr[0]);

    for (ii = 0; ii < cfgSize; ii++)
    {
        rc = cpssExMxPmPhyPort10GSmiRegisterWrite(devNum, 31, (GT_U16)appDemoModularNlCfg2Arr[ii].phyRegAddr, 1, appDemoModularNlCfg2Arr[ii].phyRegData);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPhyPort10GSmiRegisterWrite", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        cpssOsTimerWkAfter(appDemoModularNlCfg2Arr[ii].sleepTime);
    }

    return GT_OK;
}

/*******************************************************************************
* prvPuma3NlTcamInit
*
* DESCRIPTION:
*       Init NL TCAM and Puma3 TCAM ILNK interface.
*
* INPUTS:
*       devNum      - device number
*       initPort    - GT_TRUE init Puma3 ILNK
*                     GT_FALSE do not init Puma3 ILNK
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
GT_STATUS prvPuma3NlTcamInit
(
    GT_U8 devNum,
    GT_U32 initPort
)
{
    GT_STATUS rc;

    rc = prvPuma3ModularTcamNlInit(devNum);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("prvPuma3ModularTcamNlInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (initPort)
    {
        /* wait for NL SERDES to be ready */
        cpssOsTimerWkAfter(100);

        /* init Puma3 ILNK interface including SERDES */
        rc = prvPuma3TcamIlnkInit(devNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvPuma3TcamIlnkInit", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    cpssOsTimerWkAfter(100);
    rc = prvPuma3ModularTcamNlInit2(devNum);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("prvPuma3ModularTcamNlInit2", rc);
    return rc;
}

/*******************************************************************************
* prvPuma3NlXsmiRegWrite
*
* DESCRIPTION:
*       Write NL TCAM MDIO register
*
* INPUTS:
*       devNum     - device number
*       phyDev     - MDIO device
*       phyReg     - MDIO register address
*       data       - MDIO register data
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
GT_STATUS prvPuma3NlXsmiRegWrite
(
    GT_U8  devNum,
    GT_U32 phyDev,
    GT_U32 phyReg,
    GT_U16 data
)
{
    GT_STATUS rc;

    rc = cpssExMxPmPhyPort10GSmiRegisterWrite(devNum, 31, (GT_U16)phyReg, (GT_U8)phyDev, data);
    if (rc != GT_OK)
    {
        cpssOsPrintf(" XSMI error dev %d reg %X rc %d\n", phyDev, phyReg, rc);
    }

    return rc;
}

/*******************************************************************************
* prvPuma3NlXsmiRegRead
*
* DESCRIPTION:
*       Read NL TCAM MDIO register
*
* INPUTS:
*       devNum     - device number
*       phyDev     - MDIO device
*       phyReg     - MDIO register address
*
* OUTPUTS:
*       dataPtr    - (pointer to) MDIO register data
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvPuma3NlXsmiRegRead
(
    GT_U8 devNum,
    GT_U32 phyDev,
    GT_U32 phyReg,
    GT_U16 *dataPtr
)
{
    GT_STATUS rc;

    rc = cpssExMxPmPhyPort10GSmiRegisterRead(devNum, 31, (GT_U16)phyReg, (GT_U8)phyDev, dataPtr);
    return rc;
}

GT_U32  nlSerdesCntr;


/*******************************************************************************
* prvPuma3TcamNlInitNew
*
* DESCRIPTION:
*       Init NL TCAM ILNK interface. Use pooling instead of sleep.
*
* INPUTS:
*       devNum      - device number
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
GT_STATUS prvPuma3TcamNlInitNew
(
    GT_U8 devNum
)
{
    GT_U32 ii;
    GT_U32 cfgSize;
    GT_U16 data;
    GT_STATUS rc;

    cfgSize = sizeof(appDemoNlCfgArr) / sizeof(appDemoNlCfgArr[0]);

    nlSerdesCntr = 0;

    for (ii = 0; ii < cfgSize; ii++)
    {
        rc = prvPuma3NlXsmiRegWrite(devNum, 1, appDemoNlCfgArr[ii].phyRegAddr, appDemoNlCfgArr[ii].phyRegData);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvPuma3NlXsmiRegWrite", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        if (appDemoNlCfgArr[ii].sleepTime == 200)
        {
            do
            {
                rc = prvPuma3NlXsmiRegRead(devNum, 1, 0x8183, &data);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("prvPuma3NlXsmiRegRead", rc);
                if (rc != GT_OK)
                {
                    return rc;
                }
                nlSerdesCntr++;
            } while ( (data & BIT_3) == 0 );
        }
    }
    return GT_OK;
}

/*******************************************************************************
* prvPuma3NlTcamInitNew
*
* DESCRIPTION:
*       Init NL TCAM and Puma3 TCAM ILNK interface.
*       Use pooling instead of sleep.
*
* INPUTS:
*       devNum      - device number
*       initPort    - GT_TRUE init Puma3 ILNK
*                     GT_FALSE do not init Puma3 ILNK
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
GT_STATUS prvPuma3NlTcamInitNew
(
    GT_U8 devNum,
    GT_U32 initPort
)
{
    GT_STATUS rc;

    rc = prvPuma3TcamNlInitNew(devNum);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("prvPuma3TcamNlInit", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (initPort)
    {
        rc = prvPuma3TcamIlnkInit(devNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvPuma3TcamIlnkInit", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    cpssOsPrintf("prvPuma3NlTcamInit done, ILNK %d .\n", initPort);
    return GT_OK;
}

/*******************************************************************************
* prvPuma3NlXsmiRegDump
*
* DESCRIPTION:
*       Dump NL TCAM MDIO register
*
* INPUTS:
*       phyDev     - MDIO device
*       phyReg     - MDIO register address
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
GT_STATUS prvPuma3NlXsmiRegDump
(
    GT_U32 phyDev,
    GT_U32 phyReg
)
{
    GT_STATUS rc;
    GT_U16 data;

    rc = prvPuma3NlXsmiRegRead(0, phyDev, phyReg, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsPrintf(" NL dev[%d] reg[0x%04X] data 0x%04X\n", phyDev, phyReg, data);
    return GT_OK;
}

/*******************************************************************************
* prvPuma3NlReadyCheck
*
* DESCRIPTION:
*       Check that NL TCAM ILNK is ready.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       isReadyPtr  - GT_TRUE - ready
*                     GT_FALSE - not ready
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS prvPuma3NlReadyCheck
(
  IN  GT_U8   devNum,
  OUT GT_BOOL *isReadyPtr
)
{
    GT_STATUS rc;
    GT_U16 data;
    GT_U32  phyReg;

    *isReadyPtr = GT_FALSE;

    phyReg = 0x8180;
    /* read common status register to check if some errors exist
      Read first time to clean errors. */
    rc = prvPuma3NlXsmiRegRead(devNum, 1, phyReg, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    /* get clean status */
    rc = prvPuma3NlXsmiRegRead(devNum, 1, phyReg, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (data != 0)
    {
        /* some errors are exist */
        return GT_OK;
    }

    /* check general purpose status */
    phyReg = 0x8183;
    rc = prvPuma3NlXsmiRegRead(devNum, 1, phyReg, &data);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (data != 0xE)
    {
        /* some errors are exist */
        return GT_OK;
    }

    *isReadyPtr = GT_TRUE;
    return GT_OK;
}

/*******************************************************************************
* prvPuma3NlTcamInitCheckAndFix
*
* DESCRIPTION:
*       Check that NL TCAM ILNK is ready and re-init if not ready.
*
* INPUTS:
*       devNum      - device number
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
GT_STATUS prvPuma3NlTcamInitCheckAndFix
(
    GT_U8 devNum
)
{
    GT_U32  initLimit;
    GT_BOOL tcamReady = GT_FALSE;
    GT_STATUS  rc;

    initLimit = 20;

    do
    {
        rc = prvPuma3NlReadyCheck(devNum, &tcamReady);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvPuma3NlReadyCheck", rc);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (tcamReady == GT_TRUE)
        {
            cpssOsPrintf("NL11 TCAM Ready\n");
            return rc;
        }

        cpssOsPrintf("NL11 TCAM Not Ready\n");

        /* try to init TCAM ILNK again */
        prvPuma3NlTcamInit(devNum, 1);

        initLimit--;
    }while ((initLimit) && (tcamReady == GT_FALSE) );

    rc = GT_HW_ERROR;
    CPSS_ENABLER_DBG_TRACE_RC_MAC("prvPuma3NlTcamInitCheckAndFix", rc);
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
        rc = prvCpssExMxPmMetalFixRev2RegisterConfig(appDemoPpConfigList[devId].devNum);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("prvCpssExMxPmMetalFixRev2RegisterConfig", rc);
        if (rc != GT_OK)
        {
            return rc;
        }

        currentDevNum = appDemoPpConfigList[devId].devNum;
        rc = extraSystemConfigSet(boardRevId);
        if (rc != GT_OK)
            return rc;

        if (appDemoPuma3ExtTcamEnabled == GT_TRUE)
        {
            rc = prvPuma3NlTcamInit(devId, 0);
            if (rc != GT_OK)
            {
                return rc;
            }
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
    localPpPh2Config.fuqUseSeparate = GT_TRUE;
    localPpPh2Config.baseAddr   =
    appDemoPpConfigList[oldDevNum].pciInfo.pciBaseAddr;
    rc = appDemoAllocateDmaMem(localPpPh2Config.deviceId,
                               RX_DESC_NUM_DEF,
                               RX_BUFF_SIZE_DEF,
                               RX_BUFF_ALLIGN_DEF,
                               TX_DESC_NUM_DEF,
                               AU_DESC_NUM_DEF*numOfActivePortGroups,
                               &localPpPh2Config);
    osMemCpy(phase2Params,&localPpPh2Config,sizeof(CPSS_PP_PHASE2_INIT_PARAMS));

    return rc;
}


static GT_STATUS configurePorts
(
    IN  GT_U8   devNum,
    IN  GT_U8   boardRevId
)
{
    GT_U8  i;
    GT_STATUS rc;
    CPSS_PORT_INTERFACE_MODE_ENT mode;


    /* Fabric*/
    switch (boardRevId)
    {
        /* 8X 40G XLHGL*/
        case 1:
        case 7:{
                GT_U8 portsArray[]={32,36,40,41,48,52,56,57};
                mode = (isLoopbackDisabled[devNum] == GT_TRUE)?CPSS_PORT_INTERFACE_MODE_HGL_E:CPSS_PORT_INTERFACE_MODE_KR_E;
                for (i=0;i<sizeof(portsArray);i++)
                {
                    rc = gtAppDemoPuma3PortModeSpeedSet(devNum, portsArray[i], mode, CPSS_PORT_SPEED_40000_E);
                    if (rc != GT_OK)
                        return rc;
                }
            }
            break;

        /* 2X 100G ILKN16*/
        case 11:
        case 5:
        case 6:
            rc = gtAppDemoPuma3PortModeSpeedSet(devNum, 32, CPSS_PORT_INTERFACE_MODE_ILKN16_E, CPSS_PORT_SPEED_100G_E);
            if (rc != GT_OK)
                return rc;
            rc = gtAppDemoPuma3PortModeSpeedSet(devNum, 48, CPSS_PORT_INTERFACE_MODE_ILKN16_E, CPSS_PORT_SPEED_100G_E);
            if (rc != GT_OK)
                return rc;
            break;

        /* 100G ILKN24*/
        case 9:
        case 10:
        case 12:
        case 13:
        case 2:
        case 3:
        case 4:
            rc = gtAppDemoPuma3PortModeSpeedSet(devNum, 32, CPSS_PORT_INTERFACE_MODE_ILKN24_E, CPSS_PORT_SPEED_100G_E);
            if (rc != GT_OK)
                return rc;
            break;
        /* 2X 100G CHGL*/
        case 8:
            rc = gtAppDemoPuma3PortModeSpeedSet(devNum, 32, CPSS_PORT_INTERFACE_MODE_CHGL_12_E, CPSS_PORT_SPEED_100G_E);
            if (rc != GT_OK)
                return rc;
            rc = gtAppDemoPuma3PortModeSpeedSet(devNum, 48, CPSS_PORT_INTERFACE_MODE_CHGL_12_E, CPSS_PORT_SPEED_100G_E);
            if (rc != GT_OK)
                return rc;

            break;
    }

    /* Network*/
    switch (boardRevId)
    {
        /* 24 X 10G SR_LR*/
        case 5:{
                GT_U8 portsArray[]={0,1,2,3,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23,24,25,26,27};
                for (i=0;i<sizeof(portsArray);i++)
                {
                    rc = gtAppDemoPuma3PortModeSpeedSet(devNum, portsArray[i], CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_10000_E);
                    if (rc != GT_OK)
                        return rc;
                }
            }
            break;
            /* 22 X 10G SR_LR*/
        case 1:
        case 2:{
                GT_U8 portsArray[]={0,1,2,3,4,5,6,7,8,9,10,11,16,17,18,19,20,21,22,23,24,25};
                for (i=0;i<sizeof(portsArray);i++)
                {
                    rc = gtAppDemoPuma3PortModeSpeedSet(devNum, portsArray[i], CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_10000_E);
                    if (rc != GT_OK)
                        return rc;
                }
            }
            break;

        /* 2 X 100G SR_LR*/
        case 4:
        case 7:
        case 8:
            rc = gtAppDemoPuma3PortModeSpeedSet(devNum, 0, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_100G_E);
            if (rc != GT_OK)
                return rc;
            rc = gtAppDemoPuma3PortModeSpeedSet(devNum, 16, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_100G_E);
            if (rc != GT_OK)
                return rc;
            break;

        /* 6 X 40G SR_LR */
        case 3:
        case 6:{
            GT_U8 portsArray[]={0,4,8,16,20,24};
            for (i=0;i<sizeof(portsArray);i++)
            {
                rc = gtAppDemoPuma3PortModeSpeedSet(devNum, portsArray[i], CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_40000_E);
                if (rc != GT_OK)
                    return rc;
            }

        }
        break;

        case 9: {
                GT_U8 portsArray[]={1,2,5,7,8,11,19,22,23,25,26,27};
                for (i=0;i<sizeof(portsArray);i++)
                {
                    rc = gtAppDemoPuma3PortModeSpeedSet(devNum, portsArray[i], CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_10000_E);
                    if (rc != GT_OK)
                        return rc;
                }
            }
            break;
        case 10:{
                GT_U8 portsArray[]={0,4,16,20};
                for (i=0;i<sizeof(portsArray);i++)
                {
                    rc = gtAppDemoPuma3PortModeSpeedSet(devNum, portsArray[i], CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_40000_E);
                    if (rc != GT_OK)
                        return rc;
                }

            }
            break;
        case 11: {
                GT_U8 portsArray[]={0,1,2,3,4,5,6,7,8,9,16,17,18,19,20,21,22,23,24,25};
                for (i=0;i<sizeof(portsArray);i++)
                {
                    rc = gtAppDemoPuma3PortModeSpeedSet(devNum, portsArray[i], CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_10000_E);
                    if (rc != GT_OK)
                        return rc;
                }
            }
            break;
        case 12:
            rc = gtAppDemoPuma3PortModeSpeedSet(devNum, 0, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_100G_E);
            if (rc != GT_OK)
                return rc;
            rc = gtAppDemoPuma3PortModeSpeedSet(devNum, 16, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_100G_E);
            if (rc != GT_OK)
                return rc;
            break;
        case 13:
            rc = gtAppDemoPuma3PortModeSpeedSet(devNum, 4, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_40000_E);
            if (rc != GT_OK)
                return rc;
            rc = gtAppDemoPuma3PortModeSpeedSet(devNum, 16, CPSS_PORT_INTERFACE_MODE_SR_LR_E, CPSS_PORT_SPEED_100G_E);
            if (rc != GT_OK)
                return rc;
        break;

    }

    /* Service port*/
    if (boardRevId > 8)
    {
        rc = gtAppDemoPuma3PortModeSpeedSet(devNum, 10, CPSS_PORT_INTERFACE_MODE_SGMII_E, CPSS_PORT_SPEED_1000_E);
        if (rc != GT_OK)
            return rc;

        rc = cpssExMxPmPortInbandAutoNegEnableSet(devNum, 10, GT_FALSE);
        if (rc != GT_OK)
            return rc;

        rc = cpssExMxPmPortDuplexAutoNegEnableSet(devNum, 10, GT_FALSE);
        if (rc != GT_OK)
            return rc;

        rc = cpssExMxPmPortDuplexModeSet(devNum, 10, CPSS_PORT_FULL_DUPLEX_E);
        if (rc != GT_OK)
            return rc;

        rc = cpssExMxPmPortFlowControlAutoNegEnableSet(devNum, 10, GT_FALSE, GT_FALSE);
        if (rc != GT_OK)
            return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmFirstPacketWA
*
* DESCRIPTION:
*       This function performs the sequence required for the first packet WA.
*
* INPUTS:
*       devNum      - The device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       The WA initializes all ports on a default configuration,
*       calls the WA, powers down all ports and initializes them
*       according to the boardRevId.
*
*******************************************************************************/
#ifndef ASIC_SIMULATION
static GT_STATUS appDemoExMxPmFirstPacketWA
(
    GT_U8   devNum
)
{
    GT_U8       i;
    GT_STATUS   rc = GT_OK;

    if (PRV_CPSS_PP_MAC(devNum)->revision == 0)
    {
        /* Power up all serdes for the WA*/
        for(i = 0; i < PRV_CPSS_PP_MAC(devNum)->numOfPorts; i++)
        {
            if(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&(PRV_CPSS_PP_MAC(devNum)->existingPorts), i))
            {
                rc = gtAppDemoPuma3PortModeSpeedSet(devNum, i, CPSS_PORT_INTERFACE_MODE_SR_LR_E,
                                                   (i<32)?CPSS_PORT_SPEED_10000_E:CPSS_PORT_SPEED_40000_E);
                if (rc != GT_OK)
                    return rc;
            }
        }


        /* Call WA*/
        rc = prvCpssExMxPmHwFirstPacketErrataWa(devNum);
        if (rc != GT_OK)
        {
            return rc;
        }

        /* disable all ports before configuring according to boardRevId*/
        rc = cpssExMxPmPortModeSpeedSet(devNum, PRV_CPSS_PP_MAC(devNum)->existingPorts, GT_FALSE,
                                        /*don't care*/CPSS_PORT_INTERFACE_MODE_SR_LR_E,/*don't care*/CPSS_PORT_SPEED_10000_E);
    }

    return rc;
}
#endif
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
    GT_STATUS   rc = GT_OK;
    GT_U8       devId;
    GT_U8       devNum;
    GT_U32      value;

    for (devId = 0; devId < PRV_CPSS_MAX_PP_DEVICES_CNS ; devId++)
    {
        if ((GT_FALSE == appDemoPpConfigList[devId].valid) ||
            (CPSS_IS_EXMXPM_FAMILY_MAC(appDemoPpConfigList[devId].devFamily) == 0))
        {
            continue;
        }

        devNum = appDemoPpConfigList[devId].devNum;

        /* Init pizza arbiter */
        if (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_HSU_E)
        {
            rc = prvCpssExMxPmPortPizzaArbiterIfInit(devNum);
            if (rc != GT_OK)
            {
                return rc;
            }
        }
        rc = appDemoPuma3BoardTypeSet(devNum, isModularBoard[devId]);
        if (rc != GT_OK)
        {
            return rc;
        }
        if (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_HSU_E)
        {
            /* initSerdesDefaults */
            rc = appDemoDbEntryGet("initSerdesDefaults", &value);

            if ((rc != GT_OK) || (value == 0))
            {
#ifndef ASIC_SIMULATION
                /* First packet WA*/
                rc = appDemoExMxPmFirstPacketWA(devNum);
                if (rc != GT_OK)
                    return rc;

                /* ExtTCAM Interlaken check and fix */
                if(appDemoPuma3ExtTcamEnabled == GT_TRUE)
                {
                    rc = prvPuma3NlTcamInitCheckAndFix(devNum);
                    if (rc != GT_OK)
                    {
                        return rc;
                    }
                }
#endif

                /* Init ports*/
                rc = configurePorts(devNum,boardRevId);
                if (rc != GT_OK)
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
    GT_U32                          value;
    CPSS_PP_CONFIG_INIT_STC         localPpCfgParams = PP_LOGICAL_CONFIG_DEFAULT;
    APP_DEMO_CPSS_LPM_MEM_CFG_ENT   lpmMemoryMode=APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E;

    /* note lpmDbMemCfg is static in order to save memory allocation and free... */
    static APP_DEMO_CPSS_IP_MEM_CFG_STC  lpmDbMemCfg[1];

    GT_U32  mplsInterfaceMemoryNeeded; /* memory needed for MPLS interface tables */

    /* set default values for lpmInternalSramSizeArray in Klines */
    lpmDbMemCfg[0].lpmInternalSramSizeArray[0]=8;
    lpmDbMemCfg[0].lpmInternalSramSizeArray[1]=16;
    lpmDbMemCfg[0].lpmInternalSramSizeArray[2]=32;
    lpmDbMemCfg[0].lpmInternalSramSizeArray[3]=64;
    lpmDbMemCfg[0].useLpmInternalSram=GT_FALSE;

    localPpCfgParams.numOfTrunks = APP_DEMO_255_TRUNKS_CNS;
    localPpCfgParams.fdbTableInfo.fdbTableMode = CPSS_EXMXPM_PP_CONFIG_FDB_AND_ARP_TABLE_MODE_INTERNAL_SPLIT_E;
    localPpCfgParams.fdbTableInfo.fdbToLutRatio = 0;/* don't care -- internal memory */
    localPpCfgParams.fdbTableInfo.fdbHashSize = CPSS_EXMXPM_PP_CONFIG_FDB_HASH_SIZE_32K_E;
    localPpCfgParams.fdbTableInfo.extFdbHashMode = CPSS_EXMXPM_PP_CONFIG_EXT_FDB_HASH_MODE_PUMA2_E;

    /* override MPLS parameters */
    localPpCfgParams.maxNumOfMplsIfs = 10;
    localPpCfgParams.maxNumOfNhlfe = 100;

    /* set maxNumOfIpNextHop according to Puma3 routerNextHopTable numOfEntries \
       minus numOfEntries used for MPLS NHLFE */
    localPpCfgParams.maxNumOfIpNextHop = 0x8000 - localPpCfgParams.maxNumOfNhlfe;

    /* each MPLS entry takes 72 bits of memory (16 bytes in memory address space) */
    mplsInterfaceMemoryNeeded = localPpCfgParams.maxNumOfMplsIfs * 9;

    /* dit configuration */
    localPpCfgParams.ditMemLocation = APP_DEMO_CPSS_MEMORY_LOCATION_INTERNAL_E;

    /* set all features to use external memory configuration */
    if(appDemoDbEntryGet("allExternal", &value) == GT_OK)
    {
        switch(value)
        {
        case 0:
            /* use defaults - do nothing */
            break;
        case 1:/* set all featues to use external memory */
            lpmMemoryMode = APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;

            /* External Memory configuration :
            1. to enable the bridge for external memory configure
               the <FDB Mem Mode> field in the FDB Control0 register
            2. to enable the router for external memory configure the
               <NH Memory Mode> field in the Router GlobalControl0 register,
               the <SRAM TYPE> field in the General Control Register
               and the <LPM Stages Memory Assosiation> field in the Route
               LPM Control Reg1 register
            3. to enable the DIT for External memory, configure the
               <DIT Memory Mode> field in the DIT Global Control register */

            /* 1. FDB External Memory Config */
            localPpCfgParams.fdbTableInfo.fdbTableMode = CPSS_EXMXPM_PP_CONFIG_FDB_TABLE_MODE_EXTERNAL_E;

            if(appDemoDbEntryGet("fdbTableInfo.fdbHashSize", &value) == GT_OK)
            {
                localPpCfgParams.fdbTableInfo.fdbHashSize = value;
            }
            else
            {
                localPpCfgParams.fdbTableInfo.fdbHashSize = CPSS_EXMXPM_PP_CONFIG_FDB_HASH_SIZE_512K_E;
            }

            if(appDemoDbEntryGet("fdbTableInfo.extFdbHashMode", &value) == GT_OK)
            {
                localPpCfgParams.fdbTableInfo.extFdbHashMode = value;
            }
            else
            {
                localPpCfgParams.fdbTableInfo.extFdbHashMode = CPSS_EXMXPM_PP_CONFIG_EXT_FDB_HASH_MODE_PUMA2_E;
            }


            /* 2. Router External Memory Config
                <NH Memory Mode> is configured in ipMplsTablesAddrInit according
                               to moduleCfgPtr->ipCfg.ipMemLocation
                <SRAM TYPE> - General Control register should have the value
                              0x43091 - RM# 4613848
                              TBD - need to verify this issue curently it is in internal mode
                <LPM Stages Memory Assosiation>  will be configured according to
                              localPpCfgParams.lpmStagesMemAssociation */
            localPpCfgParams.lpmStagesMemAssociation=APP_DEMO_CPSS_LPM_STAGES_MEM_ASSOCIATION_ALL_EXTERNAL_E;

            /* 3. DIT External Memory Config */
            localPpCfgParams.ditMemLocation = APP_DEMO_CPSS_MEMORY_LOCATION_EXTERNAL_E;


           /* if we want all external lookup memory for LPM
              for 2Gig memory we will define extLpmMemSize to be (CPSS_DRAM_32MB_E - mplsMemoryNeeded)
              for 4Gig memory we will define extLpmMemSize to be (CPSS_DRAM_64MB_E - mplsMemoryNeeded)
              otherwise the extLpmMemSize should take into consideration the FDB memory needed */

            if(appDemoDbEntryGet("extLpmMemSize", &value) == GT_OK)
                localPpCfgParams.extLpmMemSize = value;


            break;
        default:
            return GT_BAD_PARAM;
        }
    }

    /* LPM memory configuration */
    if(appDemoDbEntryGet("lpmMemoryMode", &value) == GT_OK)
    {
        switch(value)
        {
        case 0:
            lpmMemoryMode = APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E;
            break;
        case 1:
            lpmMemoryMode = APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;
            localPpCfgParams.lpmStagesMemAssociation=APP_DEMO_CPSS_LPM_STAGES_MEM_ASSOCIATION_ALL_EXTERNAL_E;
            break;
        case 2:
            lpmMemoryMode = APP_DEMO_CPSS_EXMXPM_LPM_SINGLE_INTERNAL_E;
            break;
        case 3:
            lpmMemoryMode = APP_DEMO_CPSS_EXMXPM_LPM_DOUBLE_INTERNAL_E;
            break;
        default:
            return GT_BAD_PARAM;
        }

        if(value!=0)
        {
            /* if we want all external lookup memory for LPM
              for 2Gig memory we will define extLpmMemSize to be (CPSS_DRAM_32MB_E - mplsMemoryNeeded)
              for 4Gig memory we will define extLpmMemSize to be (CPSS_DRAM_64MB_E - mplsMemoryNeeded)
              otherwise the extLpmMemSize should take into consideration the FDB memory needed */

            if(appDemoDbEntryGet("extLpmMemSize", &value) == GT_OK)
                localPpCfgParams.extLpmMemSize = value;
        }
    }
     /* set LPM DB parameters; note that MPLS interface table is shared with                           */
     /* IP LPM memory and  it resides in LPM SRAM 3 in case of internal LPM config or in external DRAM */
     /* when external LPM memory config is applied                                                     */
    switch (lpmMemoryMode)
    {
    case APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E:
        localPpCfgParams.ipMplsMemLocation = APP_DEMO_CPSS_MEMORY_LOCATION_EXTERNAL_E;
        lpmDbMemCfg[0].routingSramCfgType = APP_DEMO_CPSS_EXMXPM_LPM_ALL_EXTERNAL_E;
        lpmDbMemCfg[0].numOfSramsSizes    = 1;
        lpmDbMemCfg[0].sramsSizeArray[0] = localPpCfgParams.extLpmMemSize;
        lpmDbMemCfg[0].extMemBankMode = APPDEMO_EXMXPM_EXT_MEMORY_LOGICAL_MODE_8BANKS_E;
        lpmDbMemCfg[0].extMemSize = CPSS_DRAM_256MB_E;
        break;
    case APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E:
        localPpCfgParams.ipMplsMemLocation = APP_DEMO_CPSS_MEMORY_LOCATION_INTERNAL_E;
        lpmDbMemCfg[0].routingSramCfgType = APP_DEMO_CPSS_EXMXPM_LPM_ALL_INTERNAL_E;
        lpmDbMemCfg[0].numOfSramsSizes    = 4;

        if(PRV_CPSS_PP_MAC(devNum)->revision >= 1)
        {
            /* Puma3 B0 and above */

            lpmDbMemCfg[0].useLpmInternalSram=GT_TRUE;

            if(appDemoDbEntryGet("lpmInternalSramSizeArray_elem0", &value) == GT_OK)
                lpmDbMemCfg[0].lpmInternalSramSizeArray[0] = value;
            if(appDemoDbEntryGet("lpmInternalSramSizeArray_elem1", &value) == GT_OK)
                lpmDbMemCfg[0].lpmInternalSramSizeArray[1] = value;
            if(appDemoDbEntryGet("lpmInternalSramSizeArray_elem2", &value) == GT_OK)
                lpmDbMemCfg[0].lpmInternalSramSizeArray[2] = value;
            if(appDemoDbEntryGet("lpmInternalSramSizeArray_elem3", &value) == GT_OK)
                lpmDbMemCfg[0].lpmInternalSramSizeArray[3] = value;

             /* sum of all sram can not be bigger then 120K Lines.
                each sram must have a value bigger then 0, and a multiple of 2 */
             if(((lpmDbMemCfg[0].lpmInternalSramSizeArray[0]+
                 lpmDbMemCfg[0].lpmInternalSramSizeArray[1]+
                 lpmDbMemCfg[0].lpmInternalSramSizeArray[2]+
                 lpmDbMemCfg[0].lpmInternalSramSizeArray[3]) > 120) ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[0]==0)     ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[1]==0)     ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[2]==0)     ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[3]==0)     ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[0]%2 != 0) ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[1]%2 != 0) ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[2]%2 != 0) ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[3]%2 != 0))
             {
                 return GT_BAD_PARAM;
             }

            /* sramsSizeArray will not be used in creation of lpmDb intead
               lpmInternalSramSizeArray will be used */
        }
        else
        {
            lpmDbMemCfg[0].sramsSizeArray[0]  = 33 * _1K;               /* LPM 0 */
            lpmDbMemCfg[0].sramsSizeArray[1]  = 66 * _1K;                /* LPM 1 */
            lpmDbMemCfg[0].sramsSizeArray[2]  = 132 * _1K;               /* LPM 2 */
            lpmDbMemCfg[0].sramsSizeArray[3]  = (264 * _1K)- mplsInterfaceMemoryNeeded;/* LPM 3 */

            if(appDemoDbEntryGet("lpmStagesMemAssociation", &value) == GT_OK)
            {
                localPpCfgParams.lpmStagesMemAssociation = (APP_DEMO_CPSS_LPM_STAGES_MEM_ASSOCIATION_ENT)value;
                if(localPpCfgParams.lpmStagesMemAssociation==APP_DEMO_CPSS_LPM_STAGES_MEM_ASSOCIATION_ALL_EXTERNAL_E)
                {
                     /* if LPM Stages Memory Association is All_External the sram0
                       size should be swapped with sram3 size otherwise LPM Stages Memory
                       Association is All_Internal and the values for the srams are
                       LPM0 = 33K, LPM1 = 66K, LPM2 = 132K, LPM3 = 264K */

                    lpmDbMemCfg[0].sramsSizeArray[0]  = (264 * _1K)- mplsInterfaceMemoryNeeded;  /* LPM 0 */
                    lpmDbMemCfg[0].sramsSizeArray[3]  = 33 * _1K;/* LPM 3 */
                }
            }
        }

        break;
    case APP_DEMO_CPSS_EXMXPM_LPM_SINGLE_INTERNAL_E:
        localPpCfgParams.ipMplsMemLocation = APP_DEMO_CPSS_MEMORY_LOCATION_EXTERNAL_E;
        lpmDbMemCfg[0].routingSramCfgType = APP_DEMO_CPSS_EXMXPM_LPM_SINGLE_INTERNAL_E;
        lpmDbMemCfg[0].numOfSramsSizes    = 2;
        lpmDbMemCfg[0].sramsSizeArray[0] = localPpCfgParams.extLpmMemSize;
        lpmDbMemCfg[0].extMemBankMode = APPDEMO_EXMXPM_EXT_MEMORY_LOGICAL_MODE_8BANKS_E;
        lpmDbMemCfg[0].extMemSize = CPSS_DRAM_256MB_E;

        if(PRV_CPSS_PP_MAC(devNum)->revision >= 1)
        {
            /* Puma3 B0 and above */

            lpmDbMemCfg[0].useLpmInternalSram=GT_TRUE;

            /* only lpmInternalSramSizeArray[0] is used in this mode */
            if(appDemoDbEntryGet("lpmInternalSramSizeArray_elem0", &value) == GT_OK)
                lpmDbMemCfg[0].lpmInternalSramSizeArray[0] = value;

             /* lpmInternalSramSizeArray[0] can not be bigger then 120K Lines.
                lpmInternalSramSizeArray[0] must have a value bigger then 0,
                and a multiple of 2 */
             if((lpmDbMemCfg[0].lpmInternalSramSizeArray[0] > 120) ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[0]==0)     ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[0]%2 != 0))
             {
                 return GT_BAD_PARAM;
             }

            /* sramsSizeArray will not be used in creation of lpmDb intead
               lpmInternalSramSizeArray will be used */
        }
        else
        {
            lpmDbMemCfg[0].sramsSizeArray[1]  = 264 * _1K;               /* LPM 1 */
        }
        break;
    case APP_DEMO_CPSS_EXMXPM_LPM_DOUBLE_INTERNAL_E:
        localPpCfgParams.ipMplsMemLocation = APP_DEMO_CPSS_MEMORY_LOCATION_EXTERNAL_E;
        lpmDbMemCfg[0].routingSramCfgType = APP_DEMO_CPSS_EXMXPM_LPM_DOUBLE_INTERNAL_E;
        lpmDbMemCfg[0].numOfSramsSizes    = 3;
        lpmDbMemCfg[0].sramsSizeArray[0] = localPpCfgParams.extLpmMemSize;
        lpmDbMemCfg[0].extMemBankMode = APPDEMO_EXMXPM_EXT_MEMORY_LOGICAL_MODE_8BANKS_E;
        lpmDbMemCfg[0].extMemSize = CPSS_DRAM_256MB_E;

        if(PRV_CPSS_PP_MAC(devNum)->revision >= 1)
        {
            /* Puma3 B0 and above */

            lpmDbMemCfg[0].useLpmInternalSram=GT_TRUE;

            /* only lpmInternalSramSizeArray[0] and lpmInternalSramSizeArray[1] are used in this mode */
            if(appDemoDbEntryGet("lpmInternalSramSizeArray_elem0", &value) == GT_OK)
                lpmDbMemCfg[0].lpmInternalSramSizeArray[0] = value;
            /* only lpmInternalSramSizeArray[0] is used in this mode */
            if(appDemoDbEntryGet("lpmInternalSramSizeArray_elem1", &value) == GT_OK)
                lpmDbMemCfg[0].lpmInternalSramSizeArray[1] = value;


             /* sum of lpmInternalSramSizeArray[0] and lpmInternalSramSizeArray[1]
                can not be bigger then 120K Lines.
                each sram must have a value bigger then 0, and a multiple of 2 */
             if(((lpmDbMemCfg[0].lpmInternalSramSizeArray[0]+
                 lpmDbMemCfg[0].lpmInternalSramSizeArray[1]) > 120) ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[0]==0)     ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[1]==0)     ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[0]%2 != 0) ||
                (lpmDbMemCfg[0].lpmInternalSramSizeArray[1]%2 != 0))
             {
                 return GT_BAD_PARAM;
             }

            /* sramsSizeArray will not be used in creation of lpmDb intead
               lpmInternalSramSizeArray will be used */
        }
        else
        {
            return GT_BAD_PARAM;
        }
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
    GT_BOOL                                 autoLearn;
    GT_U8                                   virtualPort;/*virtual port*/

    /* allow processing of AA messages */
    /* because the multi-port group device can't be configured to 'age with delete'
       so the 'CPU' must to the aging */
    appDemoSysConfig.supportAaMessage = GT_TRUE;

    /* no support for auto learn - ExMxPm3 not need it.*/
    autoLearn = GT_FALSE;

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

        /* create vlan 1 */
        numOfPorts = PRV_CPSS_PP_MAC(dev)->numOfVirtPorts;

        /* remove from the vlan the virtual ports of the FABRIC !
        to prevent loops.--> we dont want traffic from the FABRIC to return to the FABRIC.
        (this vlan entry used by the 'down stream' and not effect the 'upstream' !)

        since this is NOT valueBlade :
        remove from the vlan the virtual ports of non existing physical ports.
        */
        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsMembers);
        portsMembers.ports[0] = PRV_CPSS_PP_MAC(dev)->existingPorts.ports[0];

        CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsTagging);

        /* clear also fields not supported by this command */
        cpssOsMemSet(&vlanInfo, 0, sizeof(vlanInfo));

        vlanInfo.cpuMember = GT_FALSE;
        vlanInfo.ipv4IpmBrgEn = GT_FALSE;
        vlanInfo.ipv4IpmBrgMode = CPSS_BRG_IPM_SGV_E;
        vlanInfo.ipv6IpmBrgEn = GT_FALSE;
        vlanInfo.ipv6IpmBrgMode = CPSS_BRG_IPM_SGV_E;
        vlanInfo.mirrorToTxAnalyzerEn = GT_FALSE;
        vlanInfo.egressUnregMcFilterCmd = CPSS_UNREG_MC_VLAN_FRWD_E;
        vlanInfo.stgId = 0;

        rc = cpssExMxPmBrgVlanUntaggedMruProfileValueSet(dev, 0, 0x5f2/*1522*/);
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
        /* get full bmp for virtual ports */
        prvCpssDrvPortsFullMaskGet(numOfPorts,&vidxPortsMembers);

        rc = cpssExMxPmBrgMcEntryWrite(dev, 0xFFF, GT_TRUE, &vidxPortsMembers);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgMcEntryWrite", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        /* Port InLif configuration for Auto Learning */
        for (virtualPort = 0 ; virtualPort < numOfPorts; virtualPort++)
        {
            /* set PVID = 1*/
            rc = cpssExMxPmBrgVlanPortPvidSet(dev, virtualPort, 1);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgVlanPortPvidSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            inlifIndex = virtualPort;

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
            rc = cpssExMxPmInlifPortDefaultInlifIdSet(dev,virtualPort,inlifIndex);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmInlifPortDefaultInlifIdSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Enable Auto Learning in Ingress Port Bridge Entry */
            rc = cpssExMxPmBrgFdbPortAutoLearnEnableSet(dev, virtualPort, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbPortAutoLearnEnableSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            /* Enable NA To CPU message in Ingress Port Bridge Entry */
            rc = cpssExMxPmBrgFdbPortNaToCpuEnableSet(dev, virtualPort, GT_TRUE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmBrgFdbPortNaToCpuEnableSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

#ifdef ASIC_SIMULATION
        if(numOfPorts == 0)   CPSS_TBD_BOOKMARK_PUMA3
        {

            /* because function cpssExMxPmPortCrcCheckEnableSet is not
               supporting puma3 (yet) -->fatal error

               we need to skip calling it */

            /* Port MAC configuration */
            for (port = 0 ; port < numOfPorts; port++)
            {
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
#if 0 /* supported by puma2 but not by puma3 */
        /* Disable Txq resource sharing */
        rc = cpssExMxPmPortTxSharingResourcesEnableSet(dev, GT_FALSE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmPortTxSharingResourcesEnableSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
#endif

        if(numOfPorts == 0)   CPSS_TBD_BOOKMARK_PUMA3
        {
            /* the CPSS not implemented yet cpssExMxPmPortTxTailDropProfileSet
            so skip it */

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
        }

        /* Perform the custom configuration if exists, else perform the default config*/
        if (( CPSS_EXMXPM3_CUSTOM_CONFIG(dev,boardRevId) != NULL) &&
            (CPSS_EXMXPM3_CUSTOM_CONFIG(dev,boardRevId)->queueMapperCfg->virtualPort != 0xFF))
        {
            const APPDEMO_EXMXPM_QUEUE_MAPPER_CUSTOM_CFG_STC *queueMapperParams =
                CPSS_EXMXPM3_CUSTOM_CONFIG(dev,boardRevId)->queueMapperCfg;

            while (queueMapperParams->physicalPort != 0xFF)
            {
                /* Physical to virtual  - only if a custom value was assigned, otherwise default remains*/
                if (queueMapperParams->virtualPort != 0xFF)
                {
                    rc = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, queueMapperParams->physicalPort, queueMapperParams->virtualPort);
                    if(GT_OK != rc)
                        return rc;

                    if (queueMapperParams->physicalPort == CPSS_EXMXPM3_LOOPBACK_PORT_PHYSICAL_PORT_NUM_CNS)
                    {
                        rc = cpssExMxPmVbLoopbackPortVirtualPortNumSet(dev, queueMapperParams->virtualPort);
                        if(rc != GT_OK)
                        {
                            return rc;
                        }
                    }
                }

                queueMapperParams++;
            }
        }
        /* Default virtual ports config*/
        else
        {

            /* network cores : set default ingress physical to virtual port mapping */
            for (port = 0; port < 32; port++)
            {
                if( !(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(dev)->existingPorts,port)))
                    continue;

                virtualPort = port;

                rc = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, port, virtualPort);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            /* fabric cores : set default ingress physical to virtual port mapping */
            /*
               use the virtual port of the previous port
               since only 4 out of the 12 ports of each fabric core should be used

               so by this configuration I state :
               physical port >=32 and port < 36     --> virtual port 48
               physical port >=36 and port < 40     --> virtual port 49
               physical port ==40                   --> virtual port 50
               physical port >=41 and port < 43     --> virtual port 51
               physical port >=48 and port < 52     --> virtual port 52
               physical port >=52 and port < 56     --> virtual port 53
               physical port ==56                   --> virtual port 54
               physical port ==57 and port < 59     --> virtual port 61 --> can't use 55 as it is "egress VLAN mirroring target port"
            */


            /* physical ports 32, 36, 40, 41, 48, 52, 56, 57 */
            for (/* continue */; port < 64; port++)
            {
                if( !(CPSS_PORTS_BMP_IS_PORT_SET_MAC(&PRV_CPSS_PP_MAC(dev)->existingPorts,port)))
                    continue;

                switch(port)
                {
                    /* core 2 */
                    case 32:
                        virtualPort = 48;
                        break;
                    case 36:
                        virtualPort = 49;
                        break;
                    case 40:
                        virtualPort = 50;
                        break;
                    case 41:
                        virtualPort = 51;
                        break;
                    /* core 3 */
                    case 48:
                        virtualPort = 52;
                        break;
                    case 52:
                        virtualPort = 53;
                        break;
                    case 56:
                        virtualPort = 54;
                        break;
                    case 57:
                        virtualPort = 61;/* can't use 55 as it is "egress VLAN mirroring target port"*/
                        break;
                    default:
                        continue; /*should not get here --> non exist port */
                }


                rc = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev, port, virtualPort);
                CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet", rc);
                if(rc != GT_OK)
                {
                    return rc;
                }
            }

            /* set LBP ingress physical to virtual port mapping */
            port = PRV_CPSS_EXMXPM_LBP_PHY_PORT_NUM_CNS(dev);
            virtualPort = LOOPBACK_PORT_VIRTUAL_PORT_NUM_CNS;
            rc = cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet(dev,port, virtualPort);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmVbIngressNonCscdVirtualPortIdAssignmentSet", rc);
            if(rc != GT_OK)
            {
                return rc;
            }

            rc = cpssExMxPmVbLoopbackPortVirtualPortNumSet(dev, virtualPort);
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* Set internal TCAM write format */
        rc = cpssExMxPmIntTcamWriteModeSet(dev,
                    CPSS_EXMXPM_INT_TCAM_DATA_FORMAT_X_Y_E);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("cpssExMxPmIntTcamWriteModeSet", rc);
        if(rc != GT_OK)
        {
            return rc;
        }
        if (systemRecoveryInfo.systemRecoveryProcess != CPSS_SYSTEM_RECOVERY_PROCESS_HSU_E)
        {
            /* Fabric configuration*/
            if ((boardRevId == 1) || (boardRevId == 7) || (boardRevId == 8))
            {
                /* Puma3 connection with Aslan */
                rc = extraSystemConfigSet_AslanConnectionPorts(dev, boardRevId,
                               PRV_CPSS_PP_MAC(dev)->existingPorts.ports[CPSS_EXMXPM3_FABRIC_PORTS_BMP_INDEX]);
            }
            else
            {
                /* Puma3 connection with Petra */
                rc = extraSystemConfigSet_PetraConnectionPorts(dev, boardRevId,
                               PRV_CPSS_PP_MAC(dev)->existingPorts.ports[CPSS_EXMXPM3_FABRIC_PORTS_BMP_INDEX]);
            }
            if(rc != GT_OK)
            {
                return rc;
            }
        }

        /* Custom boards configuration - override queuemapper, pizza and dma mappng configurations*/
        rc = appDemoBoardCustomConfigSet(dev, CPSS_EXMXPM3_CUSTOM_CONFIG(dev,boardRevId));
        if(rc != GT_OK)
        {
            return rc;
        }

    }

    return rc;
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
    if (PRV_CPSS_PP_MAC(devNum)->devFamily != CPSS_PP_FAMILY_PUMA3_E)
    {
        return GT_NOT_APPLICABLE_DEVICE;
    }

    *regCfgList     = regValInfoList;
    *regCfgListSize = (sizeof(regValInfoList) /
                       sizeof(CPSS_REG_VALUE_INFO_STC));

    return GT_OK;

}

/*******************************************************************************
* gtDb98ExMxPm3BoardReg
*
* DESCRIPTION:
*       Registration function for the DB-98 Puma3 board. -- EXMXPM3
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
GT_STATUS gtDb98ExMxPm3BoardReg
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

GT_STATUS appDemoExMxPmCheckCoreClock(GT_U8 devNum, GT_U32 portGroup)
{
    GT_U32 readRegAddr = 0x100298; /* some TTI reg */
    GT_U32 timerReg = PRV_CPSS_EXMXPM_PP_MAC(devNum)->regsAddr.MG.globalConfig.lastReadTimeStamp;
    GT_U32 regData, firstRead, secondRead;
    GT_U32 diff;
    GT_STATUS rc;

    /* first read */
    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroup, readRegAddr, &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroup, timerReg, &firstRead);
    if (rc != GT_OK)
    {
        return rc;
    }

    cpssOsTimerWkAfter(10000);

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroup, readRegAddr, &regData);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroup, timerReg, &secondRead);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (firstRead > secondRead)
    {
        diff = 0xFFFFFFFF - firstRead + secondRead;
    }
    else diff = secondRead - firstRead;

    cpssOsPrintf("\n-- Diff: %u\n", diff);
    return GT_OK;
}


/*******************************************************************************
* gtAppDemoPuma3PortModeSpeedSet
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
*
*******************************************************************************/
GT_STATUS gtAppDemoPuma3PortModeSpeedSet
(
    IN  GT_U8     devNum,
    IN  GT_U8     portNum,
    IN  CPSS_PORT_INTERFACE_MODE_ENT   ifMode,
    IN  CPSS_PORT_SPEED_ENT  speed
)
{
    GT_STATUS   rc; /* return code */
    CPSS_PORTS_BMP_STC initPortsBmp,/* bitmap of ports to init */
                        *initPortsBmpPtr;/* pointer to bitmap */
    GT_U32      sdVecSize,i;
    GT_U32      *sdVectorPtr;
    GT_U32      portGroupId;
    GT_U32      rxPolarity,txPolarity;
    CPSS_PORT_INTERFACE_MODE_ENT   prevIfMode;
    CPSS_PORT_SPEED_ENT            prevSpeed;

    PRV_CPSS_EXMXPM_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_PUMA_E);
    PRV_CPSS_EXMXPM_PHY_PORT_OR_CPU_PORT_CHECK_MAC(devNum,portNum);

    if((ifMode >= CPSS_PORT_INTERFACE_MODE_NA_E) ||
            (ifMode < CPSS_PORT_INTERFACE_MODE_REDUCED_10BIT_E))
        return GT_BAD_PARAM;

    if((speed >= CPSS_PORT_SPEED_NA_E) || (speed < CPSS_PORT_SPEED_10_E))
        return GT_BAD_PARAM;


    initPortsBmpPtr = &initPortsBmp;
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(initPortsBmpPtr);
    CPSS_PORTS_BMP_PORT_SET_MAC(initPortsBmpPtr,portNum);


    /* Get previous port speed and interface port*/
    rc = cpssExMxPmPortInterfaceModeGet(devNum, portNum, &prevIfMode);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("gtAppDemoPuma3PortModeSpeedSet:cpssExMxPmPortInterfaceModeGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssExMxPmPortSpeedGet(devNum, portNum, &prevSpeed);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("gtAppDemoPuma3PortModeSpeedSet:cpssExMxPmPortSpeedGet", rc);
    if (rc != GT_OK)
    {
        return rc;
    }

    if (prevIfMode == ifMode)
    {
        if ((speed == prevSpeed) || ((speed <= CPSS_PORT_SPEED_1000_E) && (prevSpeed <= CPSS_PORT_SPEED_1000_E)))
        {
            return GT_OK;
        }
    }

    /* Disable port*/
    if (prevIfMode != CPSS_PORT_INTERFACE_MODE_NA_E)
    {
        /* Do not disable ILKN ports*/
        if ((prevIfMode != CPSS_PORT_INTERFACE_MODE_ILKN12_E) && (prevIfMode != CPSS_PORT_INTERFACE_MODE_ILKN16_E) &&
            (prevIfMode != CPSS_PORT_INTERFACE_MODE_ILKN24_E))
        {
            rc = cpssExMxPmPortEnableSet(devNum,portNum,GT_FALSE);
            CPSS_ENABLER_DBG_TRACE_RC_MAC("gtAppDemoPuma3PortModeSpeedSet:cpssExMxPmPortEnableSet(FALSE)", rc);

            if(rc!= GT_OK)
                return rc;
        }
    }


    rc = cpssEventDeviceMaskSet(devNum, CPSS_PP_PORT_LINK_STATUS_CHANGED_E, CPSS_EVENT_MASK_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("gtAppDemoPuma3PortModeSpeedSet:cpssEventDeviceMaskSet(mask)", rc);
    if(rc!= GT_OK)
    {
        return rc;
    }

    rc = cpssExMxPmPortModeSpeedSet(devNum, initPortsBmp, GT_TRUE, ifMode, speed);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("gtAppDemoPuma3PortModeSpeedSet:cpssExMxPmPortModeSpeedSet", rc);
    if(rc!= GT_OK)
    {
        return rc;
    }

    rc = cpssExMxPmPortSerdesInfoGet(devNum, portNum, ifMode, speed, &sdVecSize,
                                     &sdVectorPtr);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("gtAppDemoPuma3PortModeSpeedSet:cpssExMxPmPortSerdesInfoGet", rc);
    if(rc!= GT_OK)
    {
        return rc;
    }

    portGroupId = PRV_CPSS_GLOBAL_PORT_TO_PORT_GROUP_ID_CONVERT_MAC(devNum, portNum);

    /* translate polarity to first serdes lane*/
    rxPolarity = appDemoPpConfigList[devNum].portGroupsInfo[portGroupId].rxPolarityBmp  >> (sdVectorPtr[0] & 0xF);
    txPolarity = appDemoPpConfigList[devNum].portGroupsInfo[portGroupId].txPolarityBmp  >> (sdVectorPtr[0] & 0xF);


    if (ifMode == CPSS_PORT_INTERFACE_MODE_ILKN24_E)
    {
        rxPolarity |= (appDemoPpConfigList[devNum].portGroupsInfo[CPSS_EXMXPM_ILKN_24_SECOND_CORE].rxPolarityBmp >> 8) << 16 ;
        txPolarity |= (appDemoPpConfigList[devNum].portGroupsInfo[CPSS_EXMXPM_ILKN_24_SECOND_CORE].txPolarityBmp >> 8) << 16 ;
    }
    else
    {
        GT_U32 tmpRx = rxPolarity, tmpTx = txPolarity;
        /* Fixe the BMP according to activated lanes (for if-modes with unused serdes such as CHGL10)*/
        for (i=0;i<sdVecSize;i++)
        {
            if (sdVectorPtr[0] + i != sdVectorPtr[i])
            {
                rxPolarity = (rxPolarity & (~(0x1 << i))) | (((tmpRx >> (sdVectorPtr[i] & 0xF)) & 0x1) << i);
                txPolarity = (txPolarity & (~(0x1 << i))) | (((tmpTx >> (sdVectorPtr[i] & 0xF)) & 0x1) << i);
            }
        }
    }

    /* Set 40G network port's PPM */
    if ((PRV_CPSS_PP_MAC(devNum)->revision > 0) && (speed == CPSS_PORT_SPEED_40000_E) && (portNum < 32))
    {
        rc = cpssExMxPmPortSerdesPpmSet(devNum, portNum, 30);
        if (rc != GT_OK)
            return rc;
    }


#ifndef ASIC_SIMULATION
    rc = cpssExMxPmPortSerdesPolaritySet(devNum,portNum, txPolarity, rxPolarity);
    if (rc != GT_OK)
        return rc;
#endif

    rc = cpssEventDeviceMaskSet(devNum, CPSS_PP_PORT_LINK_STATUS_CHANGED_E, CPSS_EVENT_UNMASK_E);
    CPSS_ENABLER_DBG_TRACE_RC_MAC("gtAppDemoPuma3PortModeSpeedSet:cpssEventDeviceMaskSet(unmask)", rc);
    if(rc!= GT_OK)
    {
        return rc;
    }

    /* enable port*/
    if ((ifMode != CPSS_PORT_INTERFACE_MODE_ILKN12_E) && (ifMode != CPSS_PORT_INTERFACE_MODE_ILKN16_E)  &&
        (ifMode != CPSS_PORT_INTERFACE_MODE_ILKN24_E))
    {
        rc = cpssExMxPmPortEnableSet(devNum,portNum,GT_TRUE);
        CPSS_ENABLER_DBG_TRACE_RC_MAC("gtAppDemoPuma3PortModeSpeedSet:cpssExMxPmPortEnableSet(TRUE)", rc);
        if(rc!= GT_OK)
            return rc;
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoPuma3LoopbackEnableSet
*
* DESCRIPTION:
*       A pre-init API defining whether the fabric ports are initialized with a
*       loopback or not.
*
* INPUTS:
*       devNum          - device number
*       enableLoopBack  - GT_TRUE  - initialize fabric ports with loopback.
*                       - GT_FALSE - initialize fabric ports without loopback.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoPuma3LoopbackEnableSet
(
    GT_U8   devNum,
    GT_BOOL enableLoopBack
)
{
    if (devNum >=PRV_CPSS_MAX_PP_DEVICES_CNS)
        return GT_BAD_PARAM;

    if ((enableLoopBack != GT_TRUE) && (enableLoopBack != GT_FALSE))
        return GT_BAD_PARAM;

    isLoopbackDisabled[devNum] = (enableLoopBack==GT_TRUE)?GT_FALSE:GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* appDemoPuma3BoardTypeSet
*
* DESCRIPTION:
*       A pre-init API defining whether the board is a modular or a
*       standalone board.
*
* INPUTS:
*       devNum    - device number
*       isModular - GT_TRUE  - The board is a modular board.
*                 - GT_FALSE - The board is a standalone board.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - otherwise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoPuma3BoardTypeSet
(
    GT_U8   devNum,
    GT_BOOL isModular
)
{
    if (devNum >=PRV_CPSS_MAX_PP_DEVICES_CNS)
        return GT_BAD_PARAM;

    if ((isModular != GT_TRUE) && (isModular != GT_FALSE))
        return GT_BAD_PARAM;

    isModularBoard[devNum] = isModular;

    if (isModularBoard[devNum]== GT_TRUE)
    {
        appDemoPpConfigList[devNum].portGroupsInfo[0].rxPolarityBmp = 0x0800;
        appDemoPpConfigList[devNum].portGroupsInfo[0].txPolarityBmp = 0x0980;
        appDemoPpConfigList[devNum].portGroupsInfo[1].rxPolarityBmp = 0x0340;
        appDemoPpConfigList[devNum].portGroupsInfo[1].txPolarityBmp = 0x0211;
        appDemoPpConfigList[devNum].portGroupsInfo[2].rxPolarityBmp = 0xEC80;
        appDemoPpConfigList[devNum].portGroupsInfo[2].txPolarityBmp = 0x4C10;
        appDemoPpConfigList[devNum].portGroupsInfo[3].rxPolarityBmp = 0xDC40;
        appDemoPpConfigList[devNum].portGroupsInfo[3].txPolarityBmp = 0x0000;
    }
    else
    {
        appDemoPpConfigList[devNum].portGroupsInfo[0].rxPolarityBmp = 0x0CEC;
        appDemoPpConfigList[devNum].portGroupsInfo[0].txPolarityBmp = 0x0000;
        appDemoPpConfigList[devNum].portGroupsInfo[1].rxPolarityBmp = 0x0FDF;
        appDemoPpConfigList[devNum].portGroupsInfo[1].txPolarityBmp = 0x0040;
        appDemoPpConfigList[devNum].portGroupsInfo[2].rxPolarityBmp = 0x0CE4;
        appDemoPpConfigList[devNum].portGroupsInfo[2].txPolarityBmp = 0x5709;
        appDemoPpConfigList[devNum].portGroupsInfo[3].rxPolarityBmp = 0xA28F;
        appDemoPpConfigList[devNum].portGroupsInfo[3].txPolarityBmp = 0x7600;
    }

    return GT_OK;
}


/*******************************************************************************
* appDemoPuma3ExtTcamEnableSet
*
* DESCRIPTION:
*       A pre-init API defining whether the board has external TCAM.
*
* INPUTS:
*       enabled - GT_TRUE  - The board has external TCAM.
*                 - GT_FALSE - The board hasn't external TCAM.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       old state
*
* COMMENTS:
*
*******************************************************************************/
GT_BOOL appDemoPuma3ExtTcamEnableSet
(
    GT_BOOL enabled
)
{
    GT_BOOL oldState = appDemoPuma3ExtTcamEnabled;

    /* update state */
    appDemoPuma3ExtTcamEnabled = enabled;

    return oldState;
}

/*******************************************************************************
**   Pizza Dump Print
**
*******************************************************************************/
static void PizzaSim_ProfilePriorityMatrPrint(
    cpssExMxPmPortPrioritiesMatrix_STC * pPortPriorityMatrPtr,
    char * msg
)
{
    CPSS_EXMXPM_PIZZA_PORT_ID              * portPriorityListPtr;
    GT_U32 p;
    GT_U32 v;

    printf("\n    %s",msg);
    printf("\n        portNum  = %d",(int)(pPortPriorityMatrPtr->portN));
    for (p = 0 ; p < pPortPriorityMatrPtr->portN ; p++)
    {
        portPriorityListPtr = &pPortPriorityMatrPtr->portPriorityList[p][0];
        printf("\n        port %2d :",(int)p);
        for (v = 0 ; portPriorityListPtr[v] != CPSS_EXMXPM_PIZZA_INVALID_PORT_CNS ; v ++ )
        {
            printf(" %2d",portPriorityListPtr[v]);
        }
    }
}

static void PizzaSim_ProfilePizzaPrint(
    cpssExMxPmPortPizzaCfgSliceNum2SlicesList_STC * pPizzaCfgPtr,
    char * msg
)
{
    GT_U32 p; /* port */
    GT_U32 slIdx;
    GT_U32 cfg;
    cpssExMxPmPortPizzaCfgSlicesList_STC * pPizzaCfgSliceList;
    CPSS_EXMXPM_PIZZA_SLICE_ID  * pSliceList;

    printf("\n    %s",msg);
    if (NULL == pPizzaCfgPtr)
    {
        printf("\n        ------------");
        return;
    }
    printf("\n        portNum  = %d",(int)pPizzaCfgPtr->portNum);
    printf("\n        sliceNum = %d",(int)pPizzaCfgPtr->totalSlices);
    for (cfg = 0 ; pPizzaCfgPtr->pizzaCfg[cfg].sliceNum2Config != 0 ; cfg++)
    {
        printf("\n        pizza %d slices",(int)pPizzaCfgPtr->pizzaCfg[cfg].sliceNum2Config);

        pPizzaCfgSliceList = pPizzaCfgPtr->pizzaCfg[cfg].pizzaPtr;
        for (p = 0 ; p < pPizzaCfgSliceList->maxPortNum; p++)
        {
            pSliceList = &pPizzaCfgSliceList->portConfArr[p][0];
            printf("\n              %2d :",(int)p);
            for (slIdx = 0; pSliceList[slIdx] != CPSS_EXMXPM_PIZZA_INVALID_SLICE_CNS; slIdx++)
            {
                printf(" %2d",pSliceList[slIdx]);
            }
        }
    }
}

static void PizzaSim_ProfilePrint(cpssExMxPmPizzaProfile_STC * pProfilePtr)
{
    cpssExMxPmPortPrioritiesMatrix_STC * pPortPriorityMatrPtr;
    char * cpuTypeStr;

    switch (pProfilePtr->txQDef.cpuPortDef.type)
    {
        case CPSS_EXMXPM_CPUPORT_TYPE_UNDEFINED_E:
            cpuTypeStr = "No CPU";
        break;
        case CPSS_EXMXPM_CPUPORT_TYPE_REGULAR_E:
            cpuTypeStr = "Regular";
        break;
        case CPSS_EXMXPM_CPUPORT_TYPE_EXTERNAL_E:
            cpuTypeStr = "External";
        break;
        default:
        {
            cpuTypeStr = "Error in CPU type";
        }
    }

    pPortPriorityMatrPtr = pProfilePtr->pPortPriorityMatrPtr;
    printf("\n    portNum  = %d",(int)pProfilePtr->portNum);
    printf("\n    sliceNum = %d",(int)pProfilePtr->maxSliceNum);
    printf("\n    TxQ Def");
    printf("\n       rep  = %d",(int)pProfilePtr->txQDef.pizzaRepetitionNum);
    printf("\n       CPU port");
    printf("\n          type = %s", cpuTypeStr);
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
    sprintf(&str[0],"%d",(int)speed);
    return &str[0];
}

static void PizzaSim_Speed2SlaceConvTblPrint(cpssExMxPmPortGroupPortSpeed2SliceNum_STC * table2UsePtr)
{
    GT_U32 portIdx;
    GT_U32 speedIdx;
    /*CPSS_PORT_SPEED_INVALIDE_E*/
    cpssExMxPmPortGroupPortSpeed2SliceNumListItem_STC * pPortEntryPtr;
    cpssExMxPmPortSpeed2SliceNum_STC                  * pSpeedEntryPtr;
    char * pSpeedStr;


    pPortEntryPtr = &table2UsePtr->arr[0];
    for (portIdx = 0 ; pPortEntryPtr->portN != CPSS_EXMXPM_PIZZA_INVALID_PORT_CNS ; portIdx++, pPortEntryPtr++)
    {
        printf("\n    Port %2d : ",(int)pPortEntryPtr->portN);
        pSpeedEntryPtr = pPortEntryPtr->portSpeed2SliceNumListPtr;
        for (speedIdx = 0 ; pSpeedEntryPtr->slicesNum != CPSS_EXMXPM_PIZZA_INVALID_SLICE_NUM_CNS; speedIdx++,pSpeedEntryPtr++)
        {
            pSpeedStr = PizzaSim_SpeedStrGet(pSpeedEntryPtr->speed);
            printf("\n        %-10s : %2d",pSpeedStr, (int)pSpeedEntryPtr->slicesNum);
        }
    }
}

/*******************************************************************************
* cpssExMxPmPortPizzaArbiterIfTablesDump
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
GT_STATUS appDemoPuma3PortPizzaArbiterIfTablesDump
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId
)
{
    GT_STATUS rc;
    cpssExMxPmPizzaProfile_STC * pPizzaProfilePtr;
    cpssExMxPmPortGroupPortSpeed2SliceNum_STC * portGroupPortSpeed2SliceNumPtr;

    rc = cpssExMxPmPortPizzaArbiterIfUserTableGet(/*IN*/devNum,
                                                  /*IN*/portGroupId,
                                                  /*OUT*/&pPizzaProfilePtr,
                                                  /*OUT*/&portGroupPortSpeed2SliceNumPtr);
    if (rc != GT_OK)
    {
        return rc;
    }
    printf("\ndevNum = %2d portGroupId = %2d",(int)devNum,(int)portGroupId);
    printf("\nPizza Profile");
    PizzaSim_ProfilePrint(pPizzaProfilePtr);
    printf("\nSpeed Conv Table");
    PizzaSim_Speed2SlaceConvTblPrint(portGroupPortSpeed2SliceNumPtr);
    printf("\n");
    return GT_OK;
}

/*-------------------------------------------------------------------------------
**     Pizza Arbiter State Print Function :
**         prints configuration and state of all slice for each unit
**         <Rx/Tx DMA DMA_CTU  BM TxQ
**-------------------------------------------------------------------------------*/


#ifdef WIN32
    #define cpssOsPrintf printf
#endif


static void appDemoPuma3PortPizzaArbiterIfDumpSliceStatePrint
(
    GT_BOOL isEnable, GT_U32 occupiedBy
)
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

static void appDemoPuma3PortPizzaArbiterIfDumpUnitStatePrint
(
    cpssExMxPmUnit32SlicesPizzaArbiterState_STC * pUnitPtr, char * msg
)
{
    GT_U32      sliceId;
    GT_U32      slicesNumberPerGop;


    slicesNumberPerGop = sizeof(pUnitPtr->slice_enable)/sizeof(pUnitPtr->slice_enable[0]); /* pUnitPtr->totalSlices; */
    cpssOsPrintf("\n    %s",msg);
    cpssOsPrintf("\n       Total Slices       = %d",pUnitPtr->totalSlicesConfigured);
    cpssOsPrintf("\n       StrictPriority Bit = %d",pUnitPtr->strictPriorityBit);
    cpssOsPrintf("\n       Slice to port map : slice :");
    for (sliceId = 0 ; sliceId < slicesNumberPerGop ; sliceId++)
    {
        cpssOsPrintf(" %2d",sliceId);
    }
    cpssOsPrintf("\n       Slice to port map : port  :");
    for (sliceId = 0 ; sliceId < pUnitPtr->totalSlicesInHW ; sliceId++)
    {
        appDemoPuma3PortPizzaArbiterIfDumpSliceStatePrint(pUnitPtr->slice_enable[sliceId],
                                                                  pUnitPtr->slice_occupied_by[sliceId]);
    }
}


/*******************************************************************************
* appDemoPuma3PortPizzaArbiterIfStateDump
*
* DESCRIPTION:
*       Dump Pizza Arbiter Registers
*
* APPLICABLE DEVICES:
*        Puma3.
*
* NOT APPLICABLE DEVICES:
*        Puma2.
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
GT_STATUS appDemoPuma3PortPizzaArbiterIfStateDump
(
    IN  GT_U8  devNum,
    IN  GT_U32 portGroupId
)
{
    GT_STATUS   rc;         /* return code */
    GT_U32      i;
    GT_U32      sliceId;
    PRV_CPSS_GEN_PP_CONFIG_STC* pDev;
    static cpssExMxPmDevicePizzaArbiterState_STC  g_pizzaDeviceState;

    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_PUMA_E);

    pDev = PRV_CPSS_PP_MAC(devNum);

    rc = cpssExMxPmPortPizzaArbiterDevStateGet(devNum,portGroupId, /*OUT*/&g_pizzaDeviceState);
    if (rc != GT_OK)
    {
        return rc;
    }


    cpssOsPrintf("\nPizza Arbiter State device = %2d port group = %2d", devNum,portGroupId);
    appDemoPuma3PortPizzaArbiterIfDumpUnitStatePrint(&g_pizzaDeviceState.rxDMA,    "RxDMA Pizza Arbiter:");
    appDemoPuma3PortPizzaArbiterIfDumpUnitStatePrint(&g_pizzaDeviceState.txDMA,    "TxDMA Pizza Arbiter:");
    appDemoPuma3PortPizzaArbiterIfDumpUnitStatePrint(&g_pizzaDeviceState.BM,       "BM Pizza Arbiter:");

    cpssOsPrintf("\n    TxQ Pizza Arbiter:");
    cpssOsPrintf("\n       Total Slices       = %d",g_pizzaDeviceState.TxQ.totalSlicesConfigured);
    cpssOsPrintf("\n       Slices in GOP      = %d",g_pizzaDeviceState.TxQ.slicesNumInGop);
    cpssOsPrintf("\n       Slice to port map : slice :");

    for (sliceId = 0 ; sliceId < CPSS_EXMXPM_PIZZA_UNIT32_MAX_SLICES_CNS ; sliceId++)
    {
        cpssOsPrintf(" %2d",sliceId);
    }

    for (sliceId = 0 ; sliceId < g_pizzaDeviceState.TxQ.totalSlicesInHW ; sliceId++)
    {
        if (0 == sliceId % CPSS_EXMXPM_PIZZA_UNIT32_MAX_SLICES_CNS)
        {
            cpssOsPrintf("\n       Slice %3d : %3d   : port  :",sliceId, sliceId + CPSS_EXMXPM_PIZZA_UNIT32_MAX_SLICES_CNS -1 );
        }
        appDemoPuma3PortPizzaArbiterIfDumpSliceStatePrint(g_pizzaDeviceState.TxQ.slice_enable[sliceId],
                                                                      g_pizzaDeviceState.TxQ.slice_occupied_by[sliceId]);
    }
    switch (g_pizzaDeviceState.TxQ.cpu_port_type)
    {
        case CPSS_EXMXPM_CPUPORT_TYPE_UNDEFINED_E:
            cpssOsPrintf("\n       No CPU port defined");
        break;
        case CPSS_EXMXPM_CPUPORT_TYPE_REGULAR_E:
            cpssOsPrintf("\n       CPU port type regular <loopBackPort>:");
            cpssOsPrintf("\n           SliceId :");
            for (i = 0 ; g_pizzaDeviceState.TxQ.cpu_port_sliceId[i] != CPSS_EXMXPM_PIZZA_INVALID_SLICE_CNS ; i++)
            {
                cpssOsPrintf("%3d",g_pizzaDeviceState.TxQ.cpu_port_sliceId[i]);
            }
            cpssOsPrintf("\n           PortId  :");
            for (i = 0 ; g_pizzaDeviceState.TxQ.cpu_port_sliceId[i] != CPSS_EXMXPM_PIZZA_INVALID_SLICE_CNS ; i++)
            {
                appDemoPuma3PortPizzaArbiterIfDumpSliceStatePrint(g_pizzaDeviceState.TxQ.cpu_port_slice_is_enable[i],
                                                                   g_pizzaDeviceState.TxQ.cpu_port_slice_occupied_by[i]);
            }
        break;
        case CPSS_EXMXPM_CPUPORT_TYPE_EXTERNAL_E:
            cpssOsPrintf("\n       CPU port type external Slice %3d :",g_pizzaDeviceState.TxQ.cpu_port_sliceId[0]);
            appDemoPuma3PortPizzaArbiterIfDumpSliceStatePrint(g_pizzaDeviceState.TxQ.cpu_port_slice_is_enable[0],
                                                               g_pizzaDeviceState.TxQ.cpu_port_slice_occupied_by[0]);

        break;
        default:
        {
            cpssOsPrintf("\n       Error in CPU type definition");
        }

    }
    cpssOsPrintf("\n");
    return GT_OK;
}



/*******************************************************************************
* cpssExMxPmPortPizzaArbiterIfRegistersDump
*
* DESCRIPTION:
*       Dump Pizza Arbiter Registers
*
* APPLICABLE DEVICES:
*        Puma3.
*
* NOT APPLICABLE DEVICES:
*        Puma2.
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
#if 0
    GT_STATUS cpssExMxPmPortPizzaArbiterIfRegistersDump(IN  GT_U8  devNum,
                                             IN  GT_U32 portGroupId)
    {
        GT_STATUS   rc;         /* return code */
        GT_U32      regValue;   /* current value of register */
        GT_U32      regAddr;    /* register address */
        GT_U32      i;          /* iterator */
        GT_U32      cpuPortPortGroupId,
                    txQSlicesNum;
        PizzaDevInfo_STC     pizzaDevInfo;
        PizzaProfile_STC  * pPizzaProfile;

        PRV_CPSS_EXMXPM_DEV_CHECK_MAC(devNum);
        PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(devNum, CPSS_PUMA_E);

        /* number of slices in RX DMA pizza arbiter */
        regAddr = PRV_CPSS_EXMXPM_DEV_REGS_MAC(devNum)->bufferMng.rxdmaPZCtrlReg;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
        if(rc != GT_OK)
        {
            return rc;
        }

        cpssOsPrintf("rxdmaPZCtrlReg:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                        devNum, portGroupId, regAddr, regValue);

        /* number of slices in TX DMA pizza arbiter */
        regAddr = PRV_CPSS_EXMXPM_DEV_REGS_MAC(devNum)->bufferMng.txdmaPZCtrlReg;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
        cpssOsPrintf("txdmaPZCtrlReg:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                        devNum, portGroupId, regAddr, regValue);

        regAddr = PRV_CPSS_EXMXPM_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.
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


        rc = prvCpssExMxPmPortPizzaArbiterIfDevInfoBuild(/*OUT*/&pizzaDevInfo, devNum);

        if (rc != GT_OK)
        {
            return rc;
        }

        /* get the pizza definition by device parameters */
        rc = prvCpssExMxPmPortPizzaArbiterIfProfileGet(/*IN*/devNum,/*IN*/portGroupId,/*OUT*/&pPizzaProfile);
        if (GT_OK != rc )
        {
            return rc;
        }

        cpuPortPortGroupId = (PRV_CPSS_PP_MAC(devNum)->cpuPortMode !=
                                CPSS_NET_CPU_PORT_MODE_SDMA_E) ?
                                    PRV_CPSS_CPU_PORT_PORT_GROUP_ID_MAC(devNum) :
                                        PRV_CPSS_NETIF_SDMA_PORT_GROUP_ID_MAC(devNum);

        txQSlicesNum = pPizzaProfile->maxSliceNum;
        if(portGroupId == cpuPortPortGroupId)
        {/* to minimize tx queue time wasted on CPU port lets multiply number of
            slices on GOP where CPU connected, and add one slice for CPU;
            CPU port txq slice mapping already defined at init */
           txQSlicesNum = (txQSlicesNum*pPizzaProfile->TXQ_repetition)+1;
        }

        cpssOsPrintf("devType=0x%X,slicesNumberPerGop=%d,cpuPortPortGroupId=%d,txQSlicesNum=%d\n",
                     PRV_CPSS_PP_MAC(devNum)->devType, pPizzaProfile->maxSliceNum,
                     cpuPortPortGroupId, txQSlicesNum);

        regAddr = PRV_CPSS_EXMXPM_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.
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
            regAddr = PRV_CPSS_EXMXPM_DEV_REGS_MAC(devNum)->txqVer1.dq.scheduler.
                                                    portArbiter.pizzaArbiterMap[i];
            rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
            if(rc != GT_OK)
            {
                return rc;
            }
            cpssOsPrintf("pizzaArbiterMap[%d]:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                            i, devNum, portGroupId, regAddr, regValue);
        }

        regAddr = PRV_CPSS_EXMXPM_DEV_REGS_MAC(devNum)->bufferMng.bufMngPZGenConfiglReg;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
        cpssOsPrintf("bufMngPZGenConfiglReg:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                        devNum, portGroupId, regAddr, regValue);

        regAddr = PRV_CPSS_EXMXPM_DEV_REGS_MAC(devNum)->bufferMng.bufMngPZSlotEnableReg;
        rc = prvCpssDrvHwPpPortGroupReadRegister(devNum, portGroupId, regAddr, &regValue);
        if(rc != GT_OK)
        {
            return rc;
        }
        cpssOsPrintf("bufMngPZSlotEnableReg:devNum=%d,portGroupId=%d,regAddr=0x%X,data=0x%X\n",
                        devNum, portGroupId, regAddr, regValue);

        for(i = 0; i < 8; i++)
        {
            regAddr = PRV_CPSS_EXMXPM_DEV_REGS_MAC(devNum)->bufferMng.rxdmaPZSliceMapReg[i];
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
            regAddr = PRV_CPSS_EXMXPM_DEV_REGS_MAC(devNum)->bufferMng.txdmaPZSliceMapReg[i];
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
            regAddr = PRV_CPSS_EXMXPM_DEV_REGS_MAC(devNum)->bufferMng.bufMngPZSlotConfigReg[i];
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
#endif

/*******************************************************************************
* getDramStaticCfg
*
* DESCRIPTION:
*       Get DDR3 static configuration
*
* APPLICABLE DEVICES:
*        Puma3.
*
* NOT APPLICABLE DEVICES:
*        Puma2.
*
* INPUTS:
*       targetFreq      - The board target frequency
*       memType         - board memory type: LOOKUP / FORWARD
*       board           - board type
*
* OUTPUTS:
*       staticParamsPtr - pointer to the static parameters
*       numOfCfgPtr     - pointer to number of static parameters
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_STATUS getDramStaticCfg
(
    IN APPDEMO_DRAM_FREQUENCY_ENT               targetFreq,
    IN APPDEMO_EMC_TYPE                         memType,
    IN APPDEMO_BOARD_TYPE                       board,
    OUT APPDEMO_EXMXPM_EXT_DRAM_STATIC_CFG      **staticParamsPtr,
    OUT GT_U32                                  *numOfCfgPtr
)
{
    GT_STATUS rc=GT_OK;

    switch (memType)
    {
    case APPDEMO_EMC_TYPE_LOOKUP_E:
        switch(board)
        {
        case APPDEMO_BOARD_TYPE_SLM_1230_E:
            switch(targetFreq)
            {
            case APPDEMO_DRAM_FREQ_667_MHZ_E:
                *numOfCfgPtr = sizeof(dunit_LU_SLM1230_667)/ sizeof(HWS_DRAM_STATIC_CFG);
                *staticParamsPtr = dunit_LU_SLM1230_667;
                break;
            case APPDEMO_DRAM_FREQ_800_MHZ_E:
                *numOfCfgPtr = sizeof(dunit_LU_SLM1230_800)/ sizeof(HWS_DRAM_STATIC_CFG);
                *staticParamsPtr = dunit_LU_SLM1230_800;
                break;
            default:
                return GT_BAD_PARAM;
            }
            break;
        case APPDEMO_BOARD_TYPE_SLM_1236_E:
            switch(targetFreq)
            {
            case APPDEMO_DRAM_FREQ_667_MHZ_E:
                *numOfCfgPtr = sizeof(dunit_LU_SLM1236_667)/ sizeof(HWS_DRAM_STATIC_CFG);
                *staticParamsPtr = dunit_LU_SLM1236_667;
                break;
            case APPDEMO_DRAM_FREQ_800_MHZ_E:
                *numOfCfgPtr = sizeof(dunit_LU_SLM1236_800)/ sizeof(HWS_DRAM_STATIC_CFG);
                *staticParamsPtr = dunit_LU_SLM1236_800;
                break;
            default:
                return GT_BAD_PARAM;
            }
            break;
        default:
            return GT_BAD_PARAM;
        }
        break;
    case APPDEMO_EMC_TYPE_FORWARD_E:
        switch(board)
        {
        case APPDEMO_BOARD_TYPE_SLM_1230_E:
            switch(targetFreq)
            {
            case APPDEMO_DRAM_FREQ_667_MHZ_E:
                *numOfCfgPtr = sizeof(dunit_FWD_SLM1230_667)/ sizeof(HWS_DRAM_STATIC_CFG);
                *staticParamsPtr = dunit_FWD_SLM1230_667;
                break;
            case APPDEMO_DRAM_FREQ_800_MHZ_E:
                *numOfCfgPtr = sizeof(dunit_FWD_SLM1230_800)/ sizeof(HWS_DRAM_STATIC_CFG);
                *staticParamsPtr = dunit_FWD_SLM1230_800;
                break;
            default:
                return GT_BAD_PARAM;
            }
            break;
        case APPDEMO_BOARD_TYPE_SLM_1236_E:
            switch(targetFreq)
            {
            case APPDEMO_DRAM_FREQ_667_MHZ_E:
                *numOfCfgPtr = sizeof(dunit_FWD_SLM1236_667)/ sizeof(HWS_DRAM_STATIC_CFG);
                *staticParamsPtr = dunit_FWD_SLM1236_667;
                break;
            case APPDEMO_DRAM_FREQ_800_MHZ_E:
                *numOfCfgPtr = sizeof(dunit_FWD_SLM1236_800)/ sizeof(HWS_DRAM_STATIC_CFG);
                *staticParamsPtr = dunit_FWD_SLM1236_800;
                break;
            default:
                return GT_BAD_PARAM;
            }
            break;
        default:
            return GT_BAD_PARAM;
        }
        break;
    default:
        return GT_BAD_PARAM;
    }

    return rc;
}


/*******************************************************************************
* appDemoPuma3PortInterlakenRegSet
*
* DESCRIPTION:
*       Function sets Interlaken write only register.
*
* APPLICABLE DEVICES:
*       Puma3.
*
* NOT APPLICABLE DEVICES:
*       Puma2.
*
*
* INPUTS:
*       devNum      - device number
*       portGroup   - Interlaken port group id
*       regAddr     - Interlaken register address
*       mask        - register mask
*       data        - date value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoPuma3PortInterlakenRegSet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroup,
    IN GT_U32 regAddr,
    IN GT_U32 mask,
    IN GT_U32 data
)
{
    return genInterlakenRegSet(devNum, portGroup, regAddr, data, mask);
}


/*******************************************************************************
* appDemoPuma3PortInterlakenRegGet
*
* DESCRIPTION:
*       Function prints Interlaken write only register.
*
* APPLICABLE DEVICES:
*       Puma3.
*
* NOT APPLICABLE DEVICES:
*       Puma2.
*
*
* INPUTS:
*       devNum      - device number
*       portGroup   - Interlaken port group id
*       regAddr     - Interlaken register address
*       mask        - register mask
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoPuma3PortInterlakenRegGet
(
    IN GT_U8  devNum,
    IN GT_U32 portGroup,
    IN GT_U32 regAddr,
    IN GT_U32 mask
)
{
    GT_STATUS rc;
    GT_U32 data;


    rc = genInterlakenRegGet(devNum, portGroup, regAddr, &data, mask);
    if(GT_OK != rc)
    {
        return rc;
    }

    cpssOsPrintf("cpssExMxPmPortInterlakenRegGet: data = 0x%x\r\n", data);

    return GT_OK;
}

GT_STATUS showTable()
{
    GT_U8 i,j;
    GT_BOOL rxPolarity,txPolarity;
    GT_STATUS rc;
    GT_U32 buff,desc;

    for (i=0;i<2;i++)
    {
        for (j=0;j<32;j++)
        {
            cpssOsPrintf("%2d ",PRV_CPSS_EXMXPM_DEV_SERDES_INFO_MAC(0)->serdesTable[i][j]);
        }
        cpssOsPrintf("\n");
    }

    cpssOsPrintf("\n\n");
    for (i=0;i<4;i++)
    {
        for (j=0;j<16;j++)
        {
            if ((i==3) && (j>=12))
                continue;
            cpssOsPrintf("%d ",PRV_CPSS_EXMXPM_PORT_TYPE(0, i*16+j));

        }
        cpssOsPrintf("\n");
    }


    for (i=0;i<63;i++)
    {
        for (j=0;j<=24;j++)
        {
            rc = cpssExMxPmPortSerdesPolarityGet(0,i, j, &txPolarity, &rxPolarity);
            if (rc==GT_OK)
            {
                cpssOsPrintf("\nPort:\t%d\tLane:\t%d\tRx:\t%d\tTx:\t%d",i,j,rxPolarity,txPolarity);
            }
        }

    }

    cpssOsPrintf("\n\nTxq port buff/desc\n");

    for (i=0;i<16;i++)
    {
        for (j=0;j<4;j++)
        {
            rc = cpssExMxPmPortTxPortBuffDescNumberGet(0,(GT_U8)(j*16+i),GT_FALSE,&buff,&desc);
            if (rc != GT_OK)
            {
                return rc;
            }
            cpssOsPrintf("%2d:%2d/%2d\t",j*16+i,buff,desc);
        }
        cpssOsPrintf("\n");
    }

    return GT_OK;
}


GT_STATUS testXsmiRead(GT_U32 portGroup, GT_U16 xsmiAddr,GT_U16 phyDev ,GT_U16 regAddr)
{
    GT_STATUS           rc           ;
    GT_U8               devNum = 0   ;
    GT_U16              data         ;
    GT_PORT_GROUPS_BMP  portGroupsBmp;

    portGroupsBmp = (1 << portGroup);
    cpssOsPrintf(" read XSMI devNum=%d portGroupsBmp=%d XsmiAddr=%x regAddr=%x phyDev=%d \n ",
                 devNum, portGroupsBmp,xsmiAddr, regAddr, phyDev );

    /* input line testXsmiRead  1, 31, 1 , 0x8100*/
    rc = cpssXsmiPortGroupRegisterRead(devNum, portGroupsBmp, CPSS_PHY_XSMI_INTERFACE_0_E, 
                                       xsmiAddr, regAddr, phyDev, &data );

    cpssOsPrintf(" read XSMI devNum=%d portGroupsBmp=%d XsmiAddr=%x regAddr=%x phyDev=%d Data= %x\n ",
                 devNum, portGroupsBmp,xsmiAddr, regAddr, phyDev, data );
    return rc;
}

GT_STATUS testXsmiWrite(GT_U32 portGroup, GT_U16 xsmiAddr,GT_U16 phyDev ,GT_U16 regAddr, GT_U16 data)
{
    GT_STATUS           rc           ;
    GT_U8               devNum = 0   ;
    GT_PORT_GROUPS_BMP  portGroupsBmp;

    portGroupsBmp = (1 << portGroup);

    cpssOsPrintf(" Write XSMI devNum=%d portGroupsBmp=%d XsmiAddr=%x regAddr=%x phyDev=%d Data= %x\n ",
                 devNum, portGroupsBmp,xsmiAddr, regAddr, phyDev, data );
    rc = cpssXsmiPortGroupRegisterWrite(devNum, portGroupsBmp, CPSS_PHY_XSMI_INTERFACE_0_E, 
                                        xsmiAddr, regAddr, phyDev, data );
    cpssOsPrintf(" Write XSMI devNum=%d portGroupsBmp=%d XsmiAddr=%x regAddr=%x phyDev=%d Data= %x\n ",
                 devNum, portGroupsBmp,xsmiAddr, regAddr, phyDev, data );
    return rc;


}

/*******************************************************************************
* appDemoExMxPmExtTcamSpeedTest
*
* DESCRIPTION:
*       Function measures and prints time for
*       External TCAM read or write operations.
*
* APPLICABLE DEVICES:
*       Puma3, Puma2.
*
* NOT APPLICABLE DEVICES:
*       None.
*
*
* INPUTS:
*       devNum      - device number
*       startIdx    - first rule index
*       numOfEntries  - number of entries to access starting from startIdx
*       cycles        - number of cycles to access TCAM entries
*       ruleSize      - size of rules
*       dataFormat    - format of rules
*       readOrWrite   - GT_TRUE - write operations, GT_FALSE read operations
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExtTcamSpeedTest(
    IN GT_U8 devNum,
    IN GT_U32 startIdx,
    IN GT_U32 numOfEntries,
    IN GT_U32 cycles,
    IN CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_ENT   dataFormat,
    IN GT_U32 readOrWrite)
{
    GT_U32 entry;
    GT_U32 cycle;
    GT_U32 numOfOper = 0;
    GT_U32 dataSize;
    GT_U32 patternOrX[30];
    GT_U32 maskOrY[30];
    GT_BOOL valid,ageEnable;
    GT_STATUS rc;
    GT_U32  secondsStart, secondsEnd;
    GT_U32  nanoStart, nanoEnd;
    double  outTime, rate;
    GT_CHAR *operatPtr;

    cpssOsMemSet(patternOrX, 0, sizeof(patternOrX));
    cpssOsMemSet(maskOrY, 0, sizeof(maskOrY));

    switch(ruleSize)
    {
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E: dataSize = 80; break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E: dataSize = 160; break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E: dataSize = 320; break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E: dataSize = 640; break;
        default: return GT_BAD_PARAM;
    }

    cpssOsTimeRT(&secondsStart,&nanoStart);

    if (readOrWrite)
    {
        for (cycle = 0; cycle < cycles;cycle++)
        {
            for (entry = startIdx; entry < (startIdx + numOfEntries); entry++)
            {
                rc = cpssExMxPmExternalTcamRuleWrite(devNum, 0, entry, ruleSize,
                                                     dataSize,GT_TRUE,GT_FALSE,
                                                     dataFormat, patternOrX,maskOrY);
                if (rc != GT_OK)
                {
                    cpssOsPrintf("cpssExMxPmExternalTcamRuleWrite: failure %d, entry %d", rc, entry);
                    return rc;
                }

                numOfOper++;
            }
        }
    }
    else
    {
        for (cycle = 0; cycle < cycles;cycle++)
        {
            for (entry = startIdx; entry < (startIdx + numOfEntries); entry++)
            {
                rc = cpssExMxPmExternalTcamRuleRead(devNum, 0,entry, ruleSize,
                                                    dataSize, dataFormat,&valid,
                                                    &ageEnable,patternOrX,maskOrY);
                if (rc != GT_OK)
                {
                    cpssOsPrintf("cpssExMxPmExternalTcamRuleRead: failure %d, entry %d", rc, entry);
                    return rc;
                }

                numOfOper++;
            }
        }
    }
    cpssOsTimeRT(&secondsEnd,&nanoEnd);
    operatPtr = (readOrWrite)? "Write" : "Read";

    outTime = (((double)secondsEnd * 1000000000) + (double)nanoEnd) -
              (((double)secondsStart * 1000000000) + (double)nanoStart);
    rate = outTime / numOfOper;
    cpssOsPrintf(" Number of %s: %d, elapsed time in nanosec %lf\n", operatPtr, numOfOper, outTime);
    cpssOsPrintf("   each operation takes %lf nanoseconds.\n\n", rate);
    return GT_OK;
}

