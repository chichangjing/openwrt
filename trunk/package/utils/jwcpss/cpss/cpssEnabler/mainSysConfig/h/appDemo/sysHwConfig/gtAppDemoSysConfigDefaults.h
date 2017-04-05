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
* gtAppDemoSysConfigDefaults.h
*
* DESCRIPTION:
*       System configuration and initialization structres default values.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 46$
*
*******************************************************************************/
#ifndef __gtAppDemoSysConfigDefaultsh
#define __gtAppDemoSysConfigDefaultsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <appDemo/os/appOs.h>
#include <appDemo/sysHwConfig/gtAppDemoPciConfig.h>


/*******************************************************************************
 * Defualt values for the different configuration structs.
 ******************************************************************************/

/****************************************************************************/
/* GT_PP_PHASE1_INIT_PARAMS struct default                                  */
/****************************************************************************/
#define INT_LINE_DEFAULT GT_PCI_INT_D


#define CPSS_SRAM_DEFAULT                                       \
    {                                                           \
        CPSS_SRAM_SIZE_2MB_E,      /*sramSize;       */         \
        CPSS_SRAM_FREQ_250_MHZ_E,  /*sramFrequency;  */         \
        CPSS_SRAM_RATE_DDR_E,      /*sramRate;       */         \
        CPSS_EXTERNAL_MEMORY_0_E,  /*externalMemory; */         \
        CPSS_SRAM_TYPE_NARROW_E    /*sramType;       */         \
    }

#define CPSS_RLDRAM_DEFAULT                                     \
    {                                                           \
        CPSS_SRAM_SIZE_2MB_E,      /*dramSize      */           \
        GT_FALSE,                  /*overwriteDef  */           \
        11,                        /*mrs           */           \
        1536,                      /*refreshLimit  */           \
        {9,8,8,6},                 /*dramTiming    */           \
        80000,                     /*pwrUpDelay    */           \
        2048                       /*pwrUpRefDelay */           \
    }

#define CPSS_EXMXPM_SRAM_DEFAULT                                \
    {                                                           \
        CPSS_SRAM_SIZE_2MB_E       /*sramSize        */         \
    }

#define CPSS_EXMXPM_EXT_DRAM_LU_DUNIT_DEFAULT                   \
    {                                                           \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                \
     {GT_TRUE},                                                \
     {GT_TRUE}                                                 \
   }/* dunitCfgArray */

    #define CPSS_EXMXPM_EXT_DRAM_FWD_DUNIT_DEFAULT              \
    {                                                           \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
     {GT_TRUE},                                                 \
   }/* dunitCfgArray */
#define CPSS_EXMXPM_PP_PHASE1_DEFAULT                           \
    GT_FALSE,      /*ingressBufferMemoryEnableScrambling    */  \
    CPSS_EXMXPM_SRAM_DEFAULT,        /*ingressBufferSramCfg */  \
    APPDEMO_EXMXPM_BUFFER_SIZE_256_E,/*ingressBufferSize*/      \
    GT_FALSE,      /*egressBufferMemoryEnableScrambling     */  \
    APPDEMO_EXMXPM_RAM_TYPE_SRAM_E,  /*egressBufferRamType  */  \
    CPSS_EXMXPM_SRAM_DEFAULT,        /*egressBufferSramCfg  */  \
    CPSS_RLDRAM_DEFAULT,             /*egressBufferRldramCfg*/  \
    APPDEMO_EXMXPM_BUFFER_SIZE_512_E,/*egressBufferSize*/       \
    {GT_FALSE,                                                  \
    GT_FALSE},/*controlSramUsedArray */                         \
    {CPSS_EXMXPM_SRAM_DEFAULT,                                  \
    CPSS_EXMXPM_SRAM_DEFAULT},/*controlSramCfgArray*/           \
    {APPDEMO_EXMXPM_CSU_PROTECT_ECC_E,                          \
    APPDEMO_EXMXPM_CSU_PROTECT_ECC_E},/*controlSramProtectArray*/ \
    {                                                           \
    CPSS_DRAM_512MB_E,                /* Lookup DRAM size */           \
    CPSS_DRAM_512MB_E,                /* forward DRAM size */           \
    GT_TRUE,                         /* external dram in use*/  \
    CPSS_EXMXPM_EXT_DRAM_LU_DUNIT_DEFAULT, /*ext LU dunit default */ \
    CPSS_EXMXPM_EXT_DRAM_FWD_DUNIT_DEFAULT,/*ext FWD dunit default */\
    APPDEMO_EXMXPM_EXT_MEMORY_LOGICAL_MODE_8BANKS_E,/*dramLogicalMode*/\
    NULL,                                  /*luDramStaticParams*/\
    0,                                     /*luDramStaticNumOfCfg*/\
    NULL,                                  /*fwrDramStaticParams*/\
    0                                      /*fwrDramStaticNumOfCfg*/\
    },                                                           \
    GT_FALSE/*externalTcamUsed*/

#define CPSS_PP_PHASE1_DEFAULT                                  \
{                                                               \
    0,                              /* devNum           */      \
    0,                              /* baseAddr         */      \
    0,                              /* Internal base    */      \
    0,                              /* resetAndInitControllerBase   */      \
    { 0, 0, 0 },                    /* hwAddr           */      \
    0,                              /* deviceId         */      \
    0,                              /* revision         */      \
    0,                              /* intVecNum        */      \
    0,                              /* intMask          */      \
    166,                            /* coreClk          */      \
    /* flowDramCfg   , dram parameters of SAMSUNG E-Ver
    (K4D261638E-TC40) at 250MHz      */                         \
    {                                                           \
        0,                          /* flowDramSize     */      \
        CPSS_DRAM_FREQ_250_MHZ_E,   /* dramFrequency    */      \
        GT_TRUE,                    /* overwriteDef     */      \
        0x31,                       /* mrs              */      \
        2,                          /* emrs             */      \
        /* struct dramTiming */                                 \
        {                                                       \
            0,                      /* tdqss            */      \
            3,                      /* trcd             */      \
            1,                      /* trp              */      \
            2,                      /* twr              */      \
            3,                      /* twtr             */      \
            7,                      /* tras             */      \
            1,                      /* trrd             */      \
            0,                      /* tmrd             */      \
            12,                     /* trfc             */      \
            0,                      /* rd2rdDelay       */      \
            0,                      /* rd2wrDelay       */      \
            7,                      /* writeCount       */      \
            7                       /* readCount        */      \
        },                                                      \
        1850,                       /* refreshCounter   */      \
        2,                          /* trCdWr           */      \
        0                           /* operation        */      \
    },                                                          \
    /* bufferDramCfg, dram parameters of
    SAMSUNG E-Ver (K4D263238E-GC33) at 300MHz*/                 \
    {                                                           \
        0,                          /* flowDramSize     */      \
        CPSS_DRAM_FREQ_300_MHZ_E,   /* dramFrequency    */      \
        GT_TRUE,                    /* overwriteDef     */      \
        4,                          /* mrs              */      \
        2,                          /* emrs             */      \
        /* struct dramTiming */                                 \
        {                                                       \
            0,                      /* tdqss            */      \
            3,                      /* trcd             */      \
            1,                      /* trp              */      \
            2,                      /* twr              */      \
            3,                      /* twtr             */      \
            7,                      /* tras             */      \
            1,                      /* trrd             */      \
            0,                      /* tmrd             */      \
            5,                      /* trfc             */      \
            0,                      /* rd2rdDelay       */      \
            1,                      /* rd2wrDelay       */      \
            7,                      /* writeCount       */      \
            7                       /* readCount        */      \
        },                                                      \
        2240,                       /* refreshCounter   */      \
        0,                          /* trCdWr           */      \
        0                           /* operation        */      \
    },                                                          \
    /* bufferMgmtCfg        */                                  \
    {                                                           \
        GT_FALSE,                   /* overwriteDef     */      \
        0,                          /* numPpBuffers     */      \
        0                           /* numUplinkBuffers */      \
    },                                                          \
    CPSS_PP_UPLINK_BACK_TO_BACK_E,  /* uplinkCfg        */      \
    CPSS_BUFFER_DRAM_USE_DEFAULT_DRAM_CONF_E, /* bufferDram */  \
    CPSS_FLOW_DRAM_USE_DEFAULT_DRAM_CONF_E,   /* flowDram */    \
    CPSS_CHANNEL_PCI_E,             /* mngInterfaceType */      \
    CPSS_TWIST_INTERNAL_E,          /* routingSramCfgType */    \
    0,                              /* numOfSramsCfg */         \
    {                               /* sram struct */           \
        {0,0,0,0,0},                                            \
        {0,0,0,0,0},                                            \
        {0,0,0,0,0},                                            \
        {0,0,0,0,0},                                            \
        {0,0,0,0,0}                                             \
    },                                                          \
    CPSS_BUF_1536_E,                /* maxBufSize           */  \
    CPSS_SYS_HA_MODE_ACTIVE_E,      /* sysHAState           */  \
    {                                                           \
        {0,0}                       /* powerDownPortsBmp */     \
    },                                                          \
    APPDEMO_SERDES_REF_CLOCK_EXTERNAL_25_SINGLE_ENDED_E,        \
                                    /*serdesRefClock*/          \
    GT_TRUE,                       /* initSerdesDefaults */    \
    GT_FALSE,                      /* isExternalCpuConnected */ \
    CPSS_EXMXPM_PP_PHASE1_DEFAULT,  /* extra ExMxPm info */     \
    CPSS_PP_PHASE1_PORT_GROUPS_DEFAULT_MAC,/* port groups info */ \
    GT_FALSE,/*tcamParityCalcEnable*/                           \
    /* relevant only to 8-region address completion mechanism*/ \
    2,/*isrAddrCompletionRegionsBmp*/  /* region 1 */           \
    0xFC, /*appAddrCompletionRegionsBmp*/  /* regions 2-7 */    \
    GT_FALSE                        /*enableLegacyVplsModeSupport */  \
}

/* default for the port groups in the CPSS_PP_PHASE1_INIT_PARAMS */
#define CPSS_PP_PHASE1_PORT_GROUPS_DEFAULT_MAC                  \
    0,                          /*numOfPortGroups*/             \
    {                                                           \
        {0,0,{0,0,0},0,0},                                      \
        {0,0,{0,0,0},0,0},                                      \
        {0,0,{0,0,0},0,0},                                      \
        {0,0,{0,0,0},0,0},                                      \
        {0,0,{0,0,0},0,0},                                      \
        {0,0,{0,0,0},0,0},                                      \
        {0,0,{0,0,0},0,0},                                      \
        {0,0,{0,0,0},0,0}                                       \
    }/*portGroupsInfo[APP_DEMO_CPSS_MAX_NUM_PORT_GROUPS_CNS]*/

/****************************************************************************/
/* GT_REG_VALUE_INFO list used by corePpHwStartInit()                       */
/****************************************************************************/
#define GT_TWIST_C_REG_VAL_INFO_LIST                                          \
{                                                                           \
    {0x00000000, 0x00000000, 0x00000000, 0},                                \
    {0x00000001, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0x0000006C, 0xFFFFFFFF, 0x0000A861, 1},    /* W C2C            */      \
    {0x00000070, 0xFFFFFFFF, 0x00002861, 1},    /* N C2C            */      \
    {0x02E00290, 0xFFFFFFFF, 0x00000000, 1},    /* srAddressReg     */      \
    {0x02E00294, 0xFFFFFFFF, 0x0001E785, 64},   /* srDataReg        */      \
    {0x01900190, 0xFFFFFFFF, 0x00000000, 1},    /* bdAddressReg     */      \
    {0x01900194, 0xFFFFFFFF, 0x000150C3, 64},   /* bdDataReg        */      \
    {0x02E00190, 0xFFFFFFFF, 0x00000000, 1},    /* fdAddressReg     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0001C008, 1},    /* fdDataReg 1      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0005C048, 1},    /* fdDataReg 2      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0009C088, 1},    /* fdDataReg 3      */      \
    {0x02E00194, 0xFFFFFFFF, 0x000DC0C8, 1},    /* fdDataReg 4      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0011C108, 1},    /* fdDataReg 5      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0015C148, 1},    /* fdDataReg 6      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0019C188, 1},    /* fdDataReg 7      */      \
    {0x02E00194, 0xFFFFFFFF, 0x001DC1C8, 1},    /* fdDataReg 8      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0021C208, 1},    /* fdDataReg 9      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0025C248, 1},    /* fdDataReg 10     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0029C288, 1},    /* fdDataReg 11     */      \
    {0x02E00194, 0xFFFFFFFF, 0x002DC2C8, 1},    /* fdDataReg 12     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0031C308, 1},    /* fdDataReg 13     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0035C348, 1},    /* fdDataReg 14     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0039C388, 1},    /* fdDataReg 15     */      \
    {0x02E00194, 0xFFFFFFFF, 0x003DC3C8, 1},    /* fdDataReg 16     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0041C408, 1},    /* fdDataReg 17     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0045C448, 1},    /* fdDataReg 18     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0049C488, 1},    /* fdDataReg 19     */      \
    {0x02E00194, 0xFFFFFFFF, 0x004DC4C8, 1},    /* fdDataReg 20     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0051C508, 1},    /* fdDataReg 21     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0055C548, 1},    /* fdDataReg 22     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0059C588, 1},    /* fdDataReg 23     */      \
    {0x02E00194, 0xFFFFFFFF, 0x005DC5C8, 1},    /* fdDataReg 24     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0061C608, 1},    /* fdDataReg 25     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0065C648, 1},    /* fdDataReg 26     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0069C688, 1},    /* fdDataReg 27     */      \
    {0x02E00194, 0xFFFFFFFF, 0x006DC6C8, 1},    /* fdDataReg 28     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0071C708, 1},    /* fdDataReg 29     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0075C748, 1},    /* fdDataReg 30     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0079C788, 1},    /* fdDataReg 31     */      \
    {0x02E00194, 0xFFFFFFFF, 0x007DC7C8, 1},    /* fdDataReg 32     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0081C808, 1},    /* fdDataReg 33     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0085C848, 1},    /* fdDataReg 34     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0089C888, 1},    /* fdDataReg 35     */      \
    {0x02E00194, 0xFFFFFFFF, 0x008DC8C8, 1},    /* fdDataReg 36     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0091C908, 1},    /* fdDataReg 37     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0095C948, 1},    /* fdDataReg 38     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0099C988, 1},    /* fdDataReg 39     */      \
    {0x02E00194, 0xFFFFFFFF, 0x009DC9C8, 1},    /* fdDataReg 40     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00A1CA08, 1},    /* fdDataReg 41     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00A5CA48, 1},    /* fdDataReg 42     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00A9CA88, 1},    /* fdDataReg 43     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00ADCAC8, 1},    /* fdDataReg 44     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00B1CB08, 1},    /* fdDataReg 45     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00B5CB48, 1},    /* fdDataReg 46     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00B9CB88, 1},    /* fdDataReg 47     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00BDCBC8, 1},    /* fdDataReg 48     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00C1CC08, 1},    /* fdDataReg 49     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00C5CC48, 1},    /* fdDataReg 50     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00C9CC88, 1},    /* fdDataReg 51     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00CDCCC8, 1},    /* fdDataReg 52     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00D1CD08, 1},    /* fdDataReg 53     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00D5CD48, 1},    /* fdDataReg 54     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00D9CD88, 1},    /* fdDataReg 55     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00DDCDC8, 1},    /* fdDataReg 56     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00E1CE08, 1},    /* fdDataReg 57     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00E5CE48, 1},    /* fdDataReg 58     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00E9CE88, 1},    /* fdDataReg 59     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00EDCEC8, 1},    /* fdDataReg 60     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00F1CF08, 1},    /* fdDataReg 61     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00F5CF48, 1},    /* fdDataReg 62     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00F9CF88, 1},    /* fdDataReg 63     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00FDCFC8, 1},    /* fdDataReg 64     */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0x00000002, 0x00000000, 0x00000000, 0},                                \
    {0x00000003, 0x00000000, 0x00000000, 0},                                \
    {0x00000004, 0x00000000, 0x00000000, 0},                                \
    {0x00000005, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0x0000006C, 0x00020000, 0x00000000, 1},    /* W C2C CMOS       */      \
    {0x0000006C, 0x00001000, 0x00001000, 1},    /* W C2C By DLL     */      \
    {0x00000070, 0x00001000, 0x00001000, 1},    /* N C2C By DLL     */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0x0000006C, 0xFFFFFFFF, 0x00001861, 1},    /* W C2C            */      \
    {0x00000070, 0xFFFFFFFF, 0x00001861, 1},    /* N C2C            */      \
    {0x02E00290, 0xFFFFFFFF, 0x00000000, 1},    /* srAddressReg     */      \
    {0x02E00294, 0xFFFFFFFF, 0x13456505, 64},   /* srDataReg        */      \
    {0x01900190, 0xFFFFFFFF, 0x00000000, 1},    /* bdAddressReg     */      \
    {0x01900194, 0xFFFFFFFF, 0x000080C1, 64},   /* bdDataReg        */      \
    {0x02E00190, 0xFFFFFFFF, 0x00000000, 1},    /* fdAddressReg     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0001E206, 64},   /* fdDataReg        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}     /* Delimiter        */      \
}

#define GT_TWIST_D_REG_VAL_INFO_LIST                                        \
{                                                                           \
    {0x00000000, 0x00000000, 0x00000000, 0},                                \
    {0x00000001, 0x00000000, 0x00000000, 0},                                \
    {0x00000002, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0x0000006C, 0x00200000, 0x00200000, 1},    /* W C2C            */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
}

#define GT_PHOENIX_REG_VAL_INFO_LIST                                        \
{                                                                           \
    {0x00000000, 0x00000000, 0x00000000, 0},                                \
    {0x00000001, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},                                \
    {0x0190010c, 0x07000000, 0x06000000, 1},                                \
    {0x01900180, 0xFFFFFFFF, 0x00400000, 1},                                \
    {0x01900184, 0xFFFFFFFF, 0x10d80180, 1},                                \
    {0x02e00280, 0xFFFFFFFF, 0x00420000, 1},                                \
    {0x1900190 , 0xFFFFFFFF, 0x0       , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x0       , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x1040    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x2041    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x3081    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x40C2    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x50C2    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x6102    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x7103    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x8143    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x9184    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0xA184    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0xB1C4    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0xC205    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0xD205    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0xE246    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0xF246    , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x11287   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x122C7   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x132C7   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x14308   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x15348   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x16349   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x17389   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x18389   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x193CA   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x1A40A   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x1B40B   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x1C44B   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x1D48B   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x1E48C   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x1F4CC   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x204CD   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x2150D   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x2254D   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x2354E   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x2458E   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x255CF   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x265CF   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x2760F   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x27610   , 1},                                \
    {0x1900194 , 0xFFFFFFFF, 0x27650   , 1},                                \
    {0x1900000 , 0x00300000, 0x00300000, 1},                                \
    {0x03800054, 0xFFFFFFFF, 0x2880CE2 , 1},                                \
    {0x03800040, 0xFFFFFFFF, 0x0000001 , 150},                              \
    {0x03800054, 0xFFFFFFFF, 0x2890CE2 , 1},                                \
    {0x03800040, 0xFFFFFFFF, 0x0000002 , 150},                              \
    {0x03800054, 0xFFFFFFFF, 0x28A0CE2 , 1},                                \
    {0x03800040, 0xFFFFFFFF, 0x0000004 , 150},                              \
    {0x03800054, 0xFFFFFFFF, 0x28B0CE2 , 1},                                \
    {0x03800040, 0xFFFFFFFF, 0x0000008 , 150},                              \
    {0x03800054, 0xFFFFFFFF, 0x89140   , 1},                                \
    {0x03800040, 0xFFFFFFFF, 0x0001000 , 150},                              \
    {0x03800054, 0xFFFFFFFF, 0x99140   , 1},                                \
    {0x03800040, 0xFFFFFFFF, 0x0002000 , 150},                              \
    {0x03800054, 0xFFFFFFFF, 0xA9140   , 1},                                \
    {0x03800040, 0xFFFFFFFF, 0x0004000 , 150},                              \
    {0x03800054, 0xFFFFFFFF, 0xB9140   , 1},                                \
    {0x03800040, 0xFFFFFFFF, 0x0008000 , 150},                              \
    {0x03800040, 0xFFFFFFFF, 0x0000000 , 1},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}                                 \
}

/* Twist-D-XG, includes info for revision 0-5 */
#define GT_TWIST_D_XG_REG_VAL_INFO_LIST                                   \
{                                                                         \
    {0x00000000, 0x00000000, 0x00000000,  0},  /* revision 0 */           \
    {0x00000001, 0x00000000, 0x00000000,  0},  /* revision 1 */           \
    {0xFFFFFFFF, 0x00000000, 0x00000000,  0},  /* Delimiter  */           \
    {0xFFFFFFFF, 0x00000000, 0x00000000,  0},  /* Delimiter  */           \
    {0x0000006C, 0xFFFFFFFF, 0x00A61861,  1},  /* C2C ByPass Dll,HSTL Rec mod*/\
    {0x00000070, 0xFFFFFFFF, 0x00001861,  1},  /* N C2C*/                 \
    {0x01900100, 0xFFFFFFFF, 0x77      ,  1},  /* DRAM E-Ver Parameters */\
    {0x01900108, 0xFFFFFFFF, 0x2       ,  1},                             \
    {0x0190010c, 0xFFFFFFFF, 0x170006E0,  1},                             \
    {0x01900110, 0xFFFFFFFF, 0x1732130 ,  1},                             \
    {0x01900114, 0xFFFFFFFF, 0x45      ,  1},                             \
    {0x01900118, 0xFFFFFFFF, 0x3       ,  1},  /* MRS operation start */  \
    {0x01900118, 0xFFFFFFFF, 0x4       ,  1},  /* EMRS operation start */ \
    {0x01900180, 0xFFFFFFFF, 0x00400000,  1},  /* DFCDL Adjust */         \
    {0x01900184, 0xFFFFFFFF, 0x10d80180,  1},                             \
    {0x02e00280, 0xFFFFFFFF, 0x10420000,  1},                             \
    {0x02e00290, 0xFFFFFFFF, 0x0       ,  1},  /* PP C2C + Narrow Sram Address DFCDL 200MHz*/ \
    {0x02e00294, 0xFFFFFFFF, 0x0       ,  1},  /* PP C2C + Narrow Sram value DFCDL */   \
    {0x02e00294, 0xFFFFFFFF, 0x1041001 ,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1042002 ,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x2083003 ,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x30C4004 ,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x30C6005 ,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x4107006 ,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x5148007 ,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x5149008 ,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x618A00A,   1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x71CB00B,   1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x71CC00C,   1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x820D00D,   1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x924E00E,   1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x925000F,   1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xA291010,   1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xB2D2011,   1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xB2D3012,   1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xC314013,   1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xD355014,   1},                             \
    {0x02e00294, 0xFFFFFFF,  0xD356015,   1},                             \
    {0x02e00294, 0xFFFFFFF,  0xE397016,   1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xF3D8017,   1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xF3DA018,   1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1041B019,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1145C01A,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1145D01B,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1249E01D,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x134DF01E,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x134E001F,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x14521020,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x15562021,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x15564022,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x165A5023,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x175E6024,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x175E7025,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x18627026,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x19667027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x19667027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1A6A7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1B6E7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1B6E7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1C727027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1D767027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1D767027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1E7A7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1F7E7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1F7E7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x20827027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x21867027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x21867027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x228A7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x238E7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x238E7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x24927027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x25967027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x25967027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x269A7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x279E7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x279E7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x279E7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x279E7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x279E7027,  1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x279E7027,  1},                             \
    {0x01900190, 0xFFFFFFFF, 0x0       ,  1}, /* PP Address SDRAM DFCDL 250MHz */\
    {0x01900194, 0xFFFFFFFF, 0x0       ,  1}, /* PP SDRAM Values*/        \
    {0x01900194, 0xFFFFFFFF, 0x1040    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x2041    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x3081    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x4082    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x50c2    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x60c3    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x7103    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x8143    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x9144    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x9184    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0xA185    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0xB1C5    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0xC205    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0xD206    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0xE246    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0xF247    ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x10287   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x11288   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x122C8   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x13308   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x14309   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x15349   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1634A   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1738A   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1838B   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x193CB   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1A40B   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1B40C   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1B44C   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1C44D   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1D48D   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1E4CD   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1F4CE   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x2050E   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x2150F   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x2254F   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x23550   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x24590   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x255D0   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x265D1   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27611   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27612   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27652   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27653   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27693   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x276D3   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x276D4   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27714   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27715   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27755   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27795   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27796   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x277D6   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x277D7   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27817   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27818   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27858   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27898   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27899   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x278D9   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x278DA   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x2791A   ,  1},                             \
    {0x01900194, 0xFFFFFFFF, 0x2791B   ,  1},                             \
    {0xFFFFFFFF, 0x00000000, 0x00000000,  0},/* Delimiter */              \
    {0x00000002, 0x00000000, 0x00000000, 0}, /* revisions 2,3,4,5     */ \
    {0x00000003, 0x00000000, 0x00000000, 0}, /* currently NOT relevant*/ \
    {0x00000004, 0x00000000, 0x00000000, 0},                             \
    {0x00000005, 0x00000000, 0x00000000, 0},                             \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},/* Delimiter        */       \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},/* Delimiter        */       \
    {0x02e00290, 0xFFFFFFFF, 0x0       , 1},/* PP C2C + Narrow Sram Address DFCDL 200MHz*/ \
    {0x02e00294, 0xFFFFFFFF, 0x0       , 1},/* PP C2C + Narrow Sram value DFCDL*/\
    {0x02e00294, 0xFFFFFFFF, 0x1041001 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1042002 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x2083003 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x30C4004 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x30C6005 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x4107006 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x5148007 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x5149008 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x618A00A , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x71CB00B , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x71CC00C , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x820D00D , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x924E00E , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x925000F , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xA291010 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xB2D2011 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xB2D3012 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xC314013 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xD355014 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xD356015 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xE397016 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xF3D8017 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0xF3DA018 , 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1041B019, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1145C01A, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1145D01B, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1249E01D, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x134DF01E, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x134E001F, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x14521020, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x15562021, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x15564022, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x165A5023, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x175E6024, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x175E7025, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x18627026, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x19667027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x19667027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1A6A7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1B6E7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1B6E7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1C727027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1D767027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1D767027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1E7A7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1F7E7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x1F7E7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x20827027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x21867027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x21867027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x228A7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x238E7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x238E7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x24927027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x25967027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x25967027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x269A7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x279E7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x279E7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x279E7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x279E7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x279E7027, 1},                             \
    {0x02e00294, 0xFFFFFFFF, 0x279E7027, 1},                             \
    {0x01900190, 0xFFFFFFFF, 0x0       , 1},/*PP Address SDRAM DFCDL 250MHz*/ \
    {0x01900194, 0xFFFFFFFF, 0x0       , 1},/*PP SDRAM Values */         \
    {0x01900194, 0xFFFFFFFF, 0x1040    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x2041    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x3081    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x4082    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x50c2    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x60c3    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x7103    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x8143    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x9144    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x9184    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0xA185    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0xB1C5    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0xC205    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0xD206    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0xE246    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0xF247    , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x10287   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x11288   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x122C8   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x13308   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x14309   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x15349   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1634A   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1738A   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1838B   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x193CB   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1A40B   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1B40C   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1B44C   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1C44D   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1D48D   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1E4CD   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x1F4CE   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x2050E   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x2150F   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x2254F   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x23550   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x24590   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x255D0   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x265D1   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27611   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27612   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27652   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27653   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27693   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x276D3   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x276D4   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27714   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27715   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27755   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27795   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27796   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x277D6   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x277D7   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27817   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27818   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27858   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27898   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x27899   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x278D9   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x278DA   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x2791A   , 1},                             \
    {0x01900194, 0xFFFFFFFF, 0x2791B   , 1},                             \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}  /* Delimiter  */            \
}

#define GT_TIGER_XG_REG_VAL_INFO_LIST_PART1 \
    /* section 1 - revision 0,1 */             \
    { 0x00000000, 0x00000000, 0x00000000, 0  },\
    { 0x00000001, 0x00000000, 0x00000000, 0  },\
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0  },\
    /* after phase 1 before start init */      \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0  },\
    /* after start init before phase 2 */      \
    { 0xEEEEEEEE, 0x00000000, 0x00000000, 0  },\
    { 0x03800008, 0xFFFFFFFF, 0x0000FF28, 1  },\
    { 0x03800004, 0xFFFFFFFF, 0x14A18000, 1  },\
    { 0x03800008, 0xFFFFFFFF, 0x0000FF28, 1  },\
    { 0x03800004, 0xFFFFFFFF, 0x14A1DD00, 1  },\
    { 0x03800008, 0xFFFFFFFF, 0x0000FF29, 1  },\
    { 0x03800004, 0xFFFFFFFF, 0x14A18000, 1  },\
    { 0x03800008, 0xFFFFFFFF, 0x0000FF29, 1  },\
    { 0x03800004, 0xFFFFFFFF, 0x14A1DD00, 1  },\
    { 0x03800008, 0xFFFFFFFF, 0x0000FF2a, 1  },\
    { 0x03800004, 0xFFFFFFFF, 0x14A18000, 1  },\
    { 0x03800008, 0xFFFFFFFF, 0x0000FF2a, 1  },\
    { 0x03800004, 0xFFFFFFFF, 0x14A1DD00, 1  },\
    { 0x03800008, 0xFFFFFFFF, 0x0000FF2b, 1  },\
    { 0x03800004, 0xFFFFFFFF, 0x14A18000, 1  },\
    { 0x03800008, 0xFFFFFFFF, 0x0000FF2b, 1  },\
    { 0xEEEEEEEE, 0x00000000, 0x00000000, 0  },\
    /*Errata and defualt values */             \
    /*RegAddr     Mask        Value     Times*/\
    { 0x03800000, 0x00200000, 0x00200000, 1  },\
    { 0x0000006C, 0x00800001, 0x00800001, 1  },\
    { 0x00000070, 0x00000001, 0x00000001, 1  },\
    { 0x01900000, 0xFFFFFFFF, 0x00000029, 1  },\
    { 0x02E00000, 0x00000FFF, 0x00000333, 1  },\
    { 0x02E00020, 0x00000FFF, 0x00000333, 1  },\
    { 0x02E00030, 0x00000FFF, 0x00000333, 1  },\
    { 0x02E00010, 0x00000F00, 0x00000100, 1  },\
    { 0x02E00014, 0x0000FFFF, 0x0000F93B, 1  },\
    { 0x01800004, 0xFFFFFFFF, 0x1000220A, 1  },\
    /* Buffer mode - 2K */                     \
    { 0x0190010c, 0x0F000FFF, 0x000008C0, 1  },\
    { 0x01800184, 0x00000010, 0x00000010, 1  },\
    { 0x01840000, 0x00000080, 0x00000080, 1  },\
    { 0x00804000, 0x000003FF, 0x00000044, 1  },\
    { 0x01004000, 0x000003FF, 0x00000044, 1  },\
    { 0x03800014, 0x00000180, 0x00000180, 1  },\
    { 0x02800198, 0x00002000, 0x00002000, 1  },\
    { 0x02E0000C, 0x00000007, 0x00000003, 1  },\
    { 0x02E0002C, 0x00000007, 0x00000003, 1  },\
    { 0x0000006c, 0x3F000000, 0x0F000000, 1  },\
    { 0x03004020, 0x10000000, 0x10000000, 1  },\
    { 0x03024018, 0x20000000, 0x00000000, 1  },\
    { 0x03020000, 0x04000000, 0x00000000, 1  },\
    { 0x03034078, 0x00000005, 0x00000005, 1  },

    /* FTDLL SDRAM FUNCTION 300MHz */
#define GT_TIGER_XG_REG_VAL_INFO_LIST_PART2  \
    { 0x01900190, 0xFFFFFFFF, 0x0     , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x44183 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x48184 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x4C184 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x50204 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x54205 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x58205 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x5C206 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x60286 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x64286 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x68287 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x6C307 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x70307 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x74308 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x78388 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x7C388 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x80409 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x84409 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x8848A , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x8C50B , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x9058C , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x9460D , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x9468E , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x9868F , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x9C70F , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xA0790 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xA4811 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xA8892 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xAC913 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xB0914 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xB4995 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xB8A16 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xB8A97 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xB8B18 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xBCB99 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xC0C1A , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xC4C9B , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xC8D1C , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xCCD9D , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xD0E1E , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xD4E9F , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xD8F20 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xDCFA1 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xE1022 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xE50A3 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xE9124 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xED1A5 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xF1226 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xF52A7 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xF9328 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xFD3A9 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x10142A, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1054AB, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x10952C, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x10D5AD, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x11162E, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1156AF, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x119730, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x11D7B1, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x121832, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1258B3, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x129934, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x12D9B5, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x131A36, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x135AB7, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x139B38, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x13DBB9, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x141C3A, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x145CBB, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x149D3C, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x14DDBD, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x151E3E, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x155EBF, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x159F40, 1 },

#define GT_TIGER_XG_REG_VAL_INFO_LIST_PART3  \
    { 0x01900194, 0xFFFFFFFF, 0x15DFC1, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x162042, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1660C3, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x16A144, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x16E1C5, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x172246, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1762C7, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x17A348, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x17E3C9, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x18244A, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1864CB, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x18A54C, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x18E5CD, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x19264E, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1966CF, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x19A750, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x19E7D1, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1A2852, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1A68D3, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1AA954, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1AE9D5, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1B2A56, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1B6AD7, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1BAB58, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1BEBD9, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1C2C5A, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1C6CDB, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1CAD5C, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1CEDDD, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1D2E5E, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1D6EDF, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1DAF60, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1DEFE1, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1E3062, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1E70E3, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1EB164, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1EF1E5, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1F3266, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1F72E7, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FB368, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF3E9, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF46A, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF4EB, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF56C, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF5ED, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF66E, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF6EF, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF770, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF7F1, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF872, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF8F3, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF974, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF9F5, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FFA76, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FFAF7, 1 },

    /* FTDLL SDRAM FUNCTION */

    /* FTDLL SRAM1 FUNCTION */

#define GT_TIGER_XG_REG_VAL_INFO_LIST_PART4  \
    { 0x02E00110, 0xFFFFFFFF, 0x0     , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x104   , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x4104  , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x8185  , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xC185  , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x10205 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x14206 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x18286 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1C286 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x20307 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x24387 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x28407 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x2C488 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x30508 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x34589 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x38609 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x3C68A , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x4070A , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x4478B , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x4880B , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x4C88C , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x5090D , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x5498E , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x58A0F , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x5CA90 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x60B11 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x64B92 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x68C13 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x6CC94 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x70D15 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x74D96 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x78E17 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x7CE98 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x80F19 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x84F9A , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x8901B , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x8D09C , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x9111D , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x9519E , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x9921F , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x9D2A0 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xA1321 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xA53A2 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xA9423 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xAD4A4 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xB1525 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xB55A6 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xB9627 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xBD6A8 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xC1729 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xC57AA , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xC982B , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xCD8AC , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xD192D , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xD59AE , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xD9A2F , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xDDAB0 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xE1B31 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xE5BB2 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xE9C33 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xEDCB4 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xF1D35 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xF5DB6 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xF9E37 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xFDEB8 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x101F39, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x105FBA, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x10A03B, 1 },

#define GT_TIGER_XG_REG_VAL_INFO_LIST_PART5  \
    { 0x02E00114, 0xFFFFFFFF, 0x10E0BC, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x11213D, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1161BE, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x11A23F, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x11E2C0, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x122341, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1263C2, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x12A443, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x12E4C4, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x132545, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1365C6, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x13A647, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x13E6C8, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x142749, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1467CA, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x14A84B, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x14E8CC, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x15294D, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1569CE, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x15AA4F, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x15EAD0, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x162B51, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x166BD2, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x16AC53, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x16ECD4, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x172D55, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x176DD6, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x17AE57, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x17EED8, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x182F59, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x186FDA, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x18B05B, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x18F0DC, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x19315D, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1971DE, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x19B25F, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x19F2E0, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1A3361, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1A73E2, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1AB463, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1AF4E4, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1B3565, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1B75E6, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1BB667, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1BF6E8, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1C3769, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1C77EA, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1CB86B, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1CF8EC, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1D396D, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1D79EE, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1DBA6F, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1DFAF0, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1E3B71, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1E7BF2, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1EBC73, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1EFCF4, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1F3D75, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1F7DF6, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1FBE77, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1FFEF8, 1 },

    /* FTDLL SRAM1 FUNCTION */

    /* FTDLL SRAM2 FUNCTION */
#define GT_TIGER_XG_REG_VAL_INFO_LIST_PART6  \
    { 0x02E00150, 0xFFFFFFFF, 0x0     , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x10282 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x14283 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x18303 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1C303 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x20384 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x24384 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x28404 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x2C405 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x30485 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x34506 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x38507 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x3C588 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x40609 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x4468A , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x4868B , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x4C70C , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x5070D , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x5478E , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x5880F , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x5C890 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x60911 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x64992 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x68993 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x6CA14 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x70A15 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x74A96 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x74B17 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x78B98 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x7CC19 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x80C9A , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x80D1B , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x84D9C , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x88D9D , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x8CE1E , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x90E9F , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x94F20 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x94FA1 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x99022 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x9D0A3 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xA1124 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xA51A5 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xA9226 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xAD2A7 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xB1328 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xB53A9 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xB942A , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xBD4AB , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xC152C , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xC55AD , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xC962E , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xCD6AF , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xD1730 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xD57B1 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xD9832 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xDD8B3 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xE1934 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xE59B5 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xE9A36 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xEDAB7 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xF1B38 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xF5BB9 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xF9C3A , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xFDCBB , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x101D3C, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x105DBD, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x109E3E, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x10DEBF, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x111F40, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x115FC1, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x11A042, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x11E0C3, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x122144, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1261C5, 1 },

#define GT_TIGER_XG_REG_VAL_INFO_LIST_PART7  \
    { 0x02E00154, 0xFFFFFFFF, 0x12A246, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x12E2C7, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x132348, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1363C9, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x13A44A, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x13E4CB, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x14254C, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1465CD, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x14A64E, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x14E6CF, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x152750, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1567D1, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x15A852, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x15E8D3, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x162954, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1669D5, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x16AA56, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x16EAD7, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x172B58, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x176BD9, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x17AC5A, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x17ECDB, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x182D5C, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x186DDD, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x18AE5E, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x18EEDF, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x192F60, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x196FE1, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x19B062, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x19F0E3, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1A3164, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1A71E5, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1AB266, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1AF2E7, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1B3368, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1B73E9, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1BB46A, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1BF4EB, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1C356C, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1C75ED, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1CB66E, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1CF6EF, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1D3770, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1D77F1, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1DB872, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1DF8F3, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1E3974, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1E79F5, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1EBA76, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1EFAF7, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1F3B78, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1F7BF9, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1FBC7A, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1FFCFB, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x203D7C, 1 },

    /* FTDLL SRAM2 FUNCTION */
    /* --------AN 146-------- */
    /* Max Descriptors */
#define GT_TIGER_XG_REG_VAL_INFO_LIST_PART8  \
    { 0x1800100,  0xFFF,      0xE4,     1 }, \
    { 0x1800200,  0xFFF,      0x1C,     1 }, \
    { 0x1800210,  0xFFF,      0x1C,     1 }, \
    { 0x1800220,  0xFFF,      0x1C,     1 }, \
    { 0x1800230,  0xFFF,      0x1C,     1 }, \
    { 0x1800240,  0xFFF,      0x1C,     1 }, \
    { 0x1800250,  0xFFF,      0x1C,     1 }, \
    { 0x1800260,  0xFFF,      0x1C,     1 }, \
    { 0x1800270,  0xFFF,      0x1C,     1 }, \
    /* Max buffers */                        \
    { 0x3011000,  0xFFF,      0xFFF,    1 }, \
    /* Delimiter */                          \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 }

#define GT_TIGER_XG_REG_VAL_INFO_LIST                                   \
{                                                                       \
GT_TIGER_XG_REG_VAL_INFO_LIST_PART1 \
GT_TIGER_XG_REG_VAL_INFO_LIST_PART2 \
GT_TIGER_XG_REG_VAL_INFO_LIST_PART3 \
GT_TIGER_XG_REG_VAL_INFO_LIST_PART4 \
GT_TIGER_XG_REG_VAL_INFO_LIST_PART5 \
GT_TIGER_XG_REG_VAL_INFO_LIST_PART6 \
GT_TIGER_XG_REG_VAL_INFO_LIST_PART7 \
GT_TIGER_XG_REG_VAL_INFO_LIST_PART8 \
}

#define GT_TWIST_C_B2B_REG_VAL_INFO_LIST                                      \
{                                                                           \
    {0x00000000, 0x00000000, 0x00000000, 0},                                \
    {0x00000001, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0x0000006C, 0xFFFFFFFF, 0x0000A861, 1},    /* W C2C            */      \
    {0x00000070, 0xFFFFFFFF, 0x00002861, 1},    /* N C2C            */      \
    {0x02E00290, 0xFFFFFFFF, 0x00000000, 1},    /* srAddressReg     */      \
    {0x02E00294, 0xFFFFFFFF, 0x0001E785, 64},   /* srDataReg        */      \
    {0x01900190, 0xFFFFFFFF, 0x00000000, 1},    /* bdAddressReg     */      \
    {0x01900194, 0xFFFFFFFF, 0x000150C3, 64},   /* bdDataReg        */      \
    {0x02E00190, 0xFFFFFFFF, 0x00000000, 1},    /* fdAddressReg     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0001C008, 1},    /* fdDataReg 1      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0005C048, 1},    /* fdDataReg 2      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0009C088, 1},    /* fdDataReg 3      */      \
    {0x02E00194, 0xFFFFFFFF, 0x000DC0C8, 1},    /* fdDataReg 4      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0011C108, 1},    /* fdDataReg 5      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0015C148, 1},    /* fdDataReg 6      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0019C188, 1},    /* fdDataReg 7      */      \
    {0x02E00194, 0xFFFFFFFF, 0x001DC1C8, 1},    /* fdDataReg 8      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0021C208, 1},    /* fdDataReg 9      */      \
    {0x02E00194, 0xFFFFFFFF, 0x0025C248, 1},    /* fdDataReg 10     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0029C288, 1},    /* fdDataReg 11     */      \
    {0x02E00194, 0xFFFFFFFF, 0x002DC2C8, 1},    /* fdDataReg 12     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0031C308, 1},    /* fdDataReg 13     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0035C348, 1},    /* fdDataReg 14     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0039C388, 1},    /* fdDataReg 15     */      \
    {0x02E00194, 0xFFFFFFFF, 0x003DC3C8, 1},    /* fdDataReg 16     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0041C408, 1},    /* fdDataReg 17     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0045C448, 1},    /* fdDataReg 18     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0049C488, 1},    /* fdDataReg 19     */      \
    {0x02E00194, 0xFFFFFFFF, 0x004DC4C8, 1},    /* fdDataReg 20     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0051C508, 1},    /* fdDataReg 21     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0055C548, 1},    /* fdDataReg 22     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0059C588, 1},    /* fdDataReg 23     */      \
    {0x02E00194, 0xFFFFFFFF, 0x005DC5C8, 1},    /* fdDataReg 24     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0061C608, 1},    /* fdDataReg 25     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0065C648, 1},    /* fdDataReg 26     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0069C688, 1},    /* fdDataReg 27     */      \
    {0x02E00194, 0xFFFFFFFF, 0x006DC6C8, 1},    /* fdDataReg 28     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0071C708, 1},    /* fdDataReg 29     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0075C748, 1},    /* fdDataReg 30     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0079C788, 1},    /* fdDataReg 31     */      \
    {0x02E00194, 0xFFFFFFFF, 0x007DC7C8, 1},    /* fdDataReg 32     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0081C808, 1},    /* fdDataReg 33     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0085C848, 1},    /* fdDataReg 34     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0089C888, 1},    /* fdDataReg 35     */      \
    {0x02E00194, 0xFFFFFFFF, 0x008DC8C8, 1},    /* fdDataReg 36     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0091C908, 1},    /* fdDataReg 37     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0095C948, 1},    /* fdDataReg 38     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0099C988, 1},    /* fdDataReg 39     */      \
    {0x02E00194, 0xFFFFFFFF, 0x009DC9C8, 1},    /* fdDataReg 40     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00A1CA08, 1},    /* fdDataReg 41     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00A5CA48, 1},    /* fdDataReg 42     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00A9CA88, 1},    /* fdDataReg 43     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00ADCAC8, 1},    /* fdDataReg 44     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00B1CB08, 1},    /* fdDataReg 45     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00B5CB48, 1},    /* fdDataReg 46     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00B9CB88, 1},    /* fdDataReg 47     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00BDCBC8, 1},    /* fdDataReg 48     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00C1CC08, 1},    /* fdDataReg 49     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00C5CC48, 1},    /* fdDataReg 50     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00C9CC88, 1},    /* fdDataReg 51     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00CDCCC8, 1},    /* fdDataReg 52     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00D1CD08, 1},    /* fdDataReg 53     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00D5CD48, 1},    /* fdDataReg 54     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00D9CD88, 1},    /* fdDataReg 55     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00DDCDC8, 1},    /* fdDataReg 56     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00E1CE08, 1},    /* fdDataReg 57     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00E5CE48, 1},    /* fdDataReg 58     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00E9CE88, 1},    /* fdDataReg 59     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00EDCEC8, 1},    /* fdDataReg 60     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00F1CF08, 1},    /* fdDataReg 61     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00F5CF48, 1},    /* fdDataReg 62     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00F9CF88, 1},    /* fdDataReg 63     */      \
    {0x02E00194, 0xFFFFFFFF, 0x00FDCFC8, 1},    /* fdDataReg 64     */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0x00000002, 0x00000000, 0x00000000, 0},                                \
    {0x00000003, 0x00000000, 0x00000000, 0},                                \
    {0x00000004, 0x00000000, 0x00000000, 0},                                \
    {0x00000005, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0x0000006C, 0x00020000, 0x00000000, 1},    /* W C2C CMOS       */      \
    {0x0000006C, 0x00001000, 0x00001000, 1},    /* W C2C By DLL     */      \
    {0x00000070, 0x00001000, 0x00001000, 1},    /* N C2C By DLL     */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0x0000006C, 0xFFFFFFFF, 0x00021861, 1},    /* W C2C            */      \
    {0x00000070, 0xFFFFFFFF, 0x00001861, 1},    /* N C2C            */      \
    {0x02E00290, 0xFFFFFFFF, 0x00000000, 1},    /* srAddressReg     */      \
    {0x02E00294, 0xFFFFFFFF, 0x1B59A685, 64},   /* srDataReg        */      \
    {0x01900190, 0xFFFFFFFF, 0x00000000, 1},    /* bdAddressReg     */      \
    {0x01900194, 0xFFFFFFFF, 0x000080C1, 64},   /* bdDataReg        */      \
    {0x02E00190, 0xFFFFFFFF, 0x00000000, 1},    /* fdAddressReg     */      \
    {0x02E00194, 0xFFFFFFFF, 0x0001E206, 64},   /* fdDataReg        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}     /* Delimiter        */      \
}

#define GT_TWIST_D_B2B_REG_VAL_INFO_LIST                                      \
{                                                                           \
    {0x00000000, 0x00000000, 0x00000000, 0},                                \
    {0x00000001, 0x00000000, 0x00000000, 0},                                \
    {0x00000002, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0x0000006C, 0x00200000, 0x00200000, 1},    /* W C2C            */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
}


#define GT_SAMBA_6X8_52_REG_VAL_INFO_LIST                                \
{                                                                              \
    { 0x00000000, 0x00000000, 0x00000000, 0 },                     \
    { 0x00000001, 0x00000000, 0x00000000, 0 },                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                                \
    { 0x01834000, 0xFFFFFFFF, 0x0000F7DF, 1 },                                \
    { 0x01834000, 0x00000600, 0x00000000, 1 },                                \
    { 0x01900100, 0xFFFFFFFF, 0x00000077, 1 },                                \
    { 0x01900104, 0xFFFFFFFF, 0x00000041, 1 },                                \
    { 0x01900108, 0xFFFFFFFF, 0x00000002, 1 },                                \
    { 0x0190010c, 0xFFFFFFFF, 0x070008c0, 1 },                                \
    { 0x01900110, 0xFFFFFFFF, 0x01732130, 1 },                                \
    { 0x01900114, 0xFFFFFFFF, 0x00000045, 1 },                                \
    { 0x01900118, 0xFFFFFFFF, 0x00000003, 1 },                                \
    { 0x01900118, 0xFFFFFFFF, 0x00000004, 1 },                                \
    { 0x02e00008, 0xFFFFFFFE, 0x0000001b, 1 },                                \
    { 0x02e00018, 0xFFFFFFFE, 0x0000000b, 1 },                                \
    { 0x02e00280, 0xFFFFFFFF, 0x10420000, 1 },                                \
    { 0x02e00284, 0xFFFFFFFF, 0x10d80100, 1 },                                \
                                          \
      /* DFCDL for Uplink &  SRAM */                                           \
    { 0x02e00290, 0xFFFFFFFF, 0x0    , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0       , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x00000041, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x01041042, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x01041082, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x01041083, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x010820C4, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x020820C5, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x02083106, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x020C3146, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x020C3147, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x030C4188, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x030C4189, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x031041C9, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0310520A, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0410520B, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0414624C, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0414624D, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0414628D, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0518728E, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x051872CF, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x05187310, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x061C8311, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x061C8351, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x061C8352, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x06209393, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x07209394, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x7020A3D5, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0724A415, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0724A416, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0824B457, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0824B458, 1 },                     \
    { 0x02e00294, 0xFFFFFFFF, 0x0828B498, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0828C4D9, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0928C4DA, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x092CD51B, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x092CD51C, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x092CD55C, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0A30E55D, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0A30E59E, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0A30E5DF, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0B34F5E0, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0B34F620, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0B34F621, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0B390662, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0C390663, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0C3916A4, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0C3D16E4, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0C3D16E5, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0D3D2726, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0D3D2727, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0D412767, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0D4137A7, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0E4137A7, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0E4547E7, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0E4547E7, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0E454827, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0F495827, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0F495867, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0F4958A7, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x104D68A7, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x104D68E7, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x104D68E7, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x10517927, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x11517927, 1 },                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                                \
                                          \
     /*ByPass Dll ,HSTL Recive mode, Clear Meassage to Narrow */              \
    { 0x0000006c, 0xFFFFFFFF, 0x00a61861, 1 },                                \
    { 0x00000070, 0xFFFFFFFF, 0x00001861, 1 },                                \
    { 0x01900110, 0xFFFFFFFF, 0x01732130, 1 },                                \
    { 0x01900114, 0xFFFFFFFF, 0x00000045, 1 },                                \
    { 0x0190010c, 0xFFFFFFFF, 0x0700073A, 1 },                                \
    { 0x1900104 , 0xFFFFFFFF, 0x50      , 1 },                                \
    { 0x1900108 , 0xFFFFFFFF, 0x2       , 1 },                                \
    { 0x1900118 , 0xFFFFFFFF, 0x3       , 1 },                                \
    { 0x1900118 , 0xFFFFFFFF, 0x4       , 1 },                                \
                                                                 \
      /* DFCDL Adjust */                                                      \
    { 0x01900180, 0xFFFFFFFF, 0x00400000, 1 },                                \
    { 0x01900184, 0xFFFFFFFF, 0x10d80180, 1 },                                \
      /* Flow SDRAM Adjust */                                      \
    { 0x02e00180, 0xFFFFFFFF, 0x00400000, 1 },                                \
    { 0x02e00184, 0xFFFFFFFF, 0x10d80180, 1 },                                \
      /* Phy address */                                                       \
    { 0x03800060, 0xFFFFFFFF, 0x00B50128, 1 },                                \
      /* Flow DRAM Parameters */                                              \
    { 0x02e00114, 0x0000FF00, 0x7700    , 1 },                                \
    { 0x02e00118, 0xFFFFFFFF, 0x3       , 1 },                                \
    { 0x02e00118, 0xFFFFFFFF, 0x4       , 1 },                                \
    { 0x02e0010c, 0xFFFFF000, 0xFA710000, 1 },                                \
      /* DFCDL for Buffer S,dram */                                           \
      /* value = ( (dqsDel),|(clkOutDel<<6)|(rxClkDel<<12)|(sysClkDel<<18));*/\
    { 0x01900190, 0xFFFFFFFF, 0x00000000, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00000000, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00002041, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00003041, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00005082, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00006082, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x000080c3, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x000090C3, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0000B104, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0000C104, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0000E145, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00010185, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00011186, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x000131C6, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x000141C7, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00016207, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00017208, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00019248, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0001A249, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0001C289, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0001D28A, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0001F2CA, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0002130B, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0002230B, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0002434C, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0002534C, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0002738D, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0002738D, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x000273CE, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x000273CE, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0002740F, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x0002744f, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00027450, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00027490, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00027491, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x000274D1, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x000274D2, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00027512, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00027513, 1 },                     \
    { 0x01900194, 0xFFFFFFFF, 0x00027553, 1 },                                \
    { 0x01900194, 0xFFFFFFFF, 0x00027554, 1 },                                \
      /* DFCDL for Flow Sdram */                                              \
    { 0x2e00190,  0xFFFFFFFF, 0x0    , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x0    , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x1040    , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x3041    , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x4081    , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x5082    , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x70C2    , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x80C3    , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x9103    , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0xA103    , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0xC144    , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0xD144    , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0xE185    , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x10185   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x111C6   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x121C6   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x14207   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x15247   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x16247   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x17288   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x19288   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x1A2C9   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x1B2C9   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x1D30A   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x1E30A   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x1F34A   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x2134B   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x2238B   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x2338C   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x243CC   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x263CD   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x2740D   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x2744D   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x2744E   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x2748E   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x2748F   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x274CF   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x274D0   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27510   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27510   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27551   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27551   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27592   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27592   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x275D3   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x275D3   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27614   , 1 },                     \
    { 0x2e00194,  0xFFFFFFFF, 0x27654   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27654   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27695   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27695   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x276D6   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x276D6   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27717   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27717   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27757   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27758   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27798   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x27799   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x277D9   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x277DA   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x2781A   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x2785A   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x2785B   , 1 },                                \
    { 0x2e00194,  0xFFFFFFFF, 0x2789B   , 1 },                                \
      /* Phy Tx Delay mode */                                                 \
    { 0x03800054, 0xFFFFFFFF, 0x2880CE2, 1},/* Phy Tx Delay mode */           \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x2890CE2, 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x28A0CE2, 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x28B0CE2, 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x28C0CE2, 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x28D0CE2, 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x28E0CE2, 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x28F0CE2, 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x2900CE2, 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x2910CE2, 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x2920CE2, 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x2930CE2, 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x89140  , 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x99140  , 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0xA9140  , 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0xB9140  , 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0xC9140  , 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0xD9140  , 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0xE9140  , 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0xF9140  , 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x109140 , 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x119140 , 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x129140 , 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000001, 200},/* delay function Led Override*/\
    { 0x03800054, 0xFFFFFFFF, 0x139140 , 1},                                  \
    { 0x03800040, 0xFFFFFFFF, 0x0000000, 200},/* delay function Led Override*/\
    /*  Delimiter  */                                                        \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0}                                  \
}

#define GT_SAMBA_6X8_12_REG_VAL_INFO_LIST                                \
{                                                                        \
        /* section 1 - revision 0,1 */                                      \
        { 0x00000000, 0x00000000, 0x00000000, 0 },                          \
        { 0x00000001, 0x00000000, 0x00000000, 0 },                          \
        { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                          \
        /* after phase 1 before start init */                               \
        /* SRAM parameters */                                               \
        { 0x02e00008, 0xFFFFFFFE, 0x0000001B, 1 },                          \
        { 0x02e00018, 0xFFFFFFFE, 0x0000000B, 1 },                          \
        { 0x02e00280, 0xFFFFFFFF, 0x10420000, 1 },                          \
        { 0x02e00284, 0xFFFFFFFF, 0x10d80100, 1 },                          \
        /* DFCDL for Uplink &  SRAM */                                      \
        { 0x02e00290, 0xFFFFFFFF, 0x0,        1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x0,        1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x41,       1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x1040002,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x1040002,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x1080003,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x2080004,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x2080005,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x20C0005,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x20C0006,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x3100007,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x3100008,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x3100008,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x4140009,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x414000A,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x414000B,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x518000B,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x518000C,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x51C000D,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x61C000E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x61C000E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x620000F,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x6200010,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x7240011,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x7240011,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x7240012,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x8280013,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x8280014,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x82C0014,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x92C0015,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x92C0016,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x9300017,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0x9300017,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xA300018,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xA340019,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xA34001A,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xB38001A,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xB38001B,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xB38001C,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC3C001D,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC3C001D,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0x02e00294, 0xFFFFFFFF, 0xC41001E,  1 },                          \
        { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                          \
        /* after start init before phase 2 */                               \
        /* ByPass Dll ,HSTL Recive mode, Clear Meassage to Narrow */        \
        { 0x01834000, 0xFFFFFFFF, 0x0000F1DF, 1 },                          \
        { 0x0000006c, 0xFFFFFFFF, 0x00a41861, 1 },                          \
        { 0x00000070, 0xFFFFFFFF, 0x00001861, 1 },                          \
        /* DRAM E-Ver Parameters */                                         \
        { 0x01900104, 0xFFFFFFFF, 0x40,       1 },                          \
        { 0x0190010c, 0x0F000000, 0x07000000, 1 },                          \
        { 0x01900118, 0xFFFFFFFF, 0x3,        1 },                          \
        { 0x01900118, 0xFFFFFFFF, 0x4,        1 },                          \
        /* Buffer SDRAM DFCDL Adjust */                                     \
        { 0x01900180, 0xFFFFFFFF, 0x00400000, 1 },                          \
        { 0x01900184, 0xFFFFFFFF, 0x10d80180, 1 },                          \
        /* PHY address */                                                   \
        { 0x03800064, 0xFFFFFFFF, 0x14944653, 1 },                          \
        { 0x03800060, 0xFFFFFFFF, 0x16c6c1ee, 1 },                          \
        /* LED configuration */                                             \
        { 0x0380003c, 0xFFFFFFFF, 0x1600,     1 },                          \
        /* Flow Dram Adjust */                                              \
        { 0x02e00180, 0xFFFFFFFF, 0x00400000, 1 },                          \
        { 0x02e00184, 0xFFFFFFFF, 0x10d80180, 1 },                          \
        /* Flow DRAM Parameters */                                          \
        { 0x02e00114, 0x0000FF00, 0x7700,     1 },                          \
        { 0x02e00118, 0xFFFFFFFF, 0x3,        1 },                          \
        { 0x02e00118, 0xFFFFFFFF, 0x4,        1 },                          \
        { 0x02e0010c, 0xFFFFF000, 0xFA710000, 1 },                          \
        /* DFCDL for Buffer Sdram */                                        \
        { 0x1900190,  0xFFFFFFFF, 0x0,        1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x0,        1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x2040,     1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x4041,     1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x6081,     1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x80C2,     1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0xA0C2,     1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0xB103,     1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0xD103,     1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0xF143,     1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x11184,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x13184,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x151C5,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x17205,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x19206,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x1B246,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x1D287,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x1E287,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x202C7,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x222C8,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x24308,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x26349,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27349,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x2738A,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x273CA,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x273CA,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x2740B,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x2740B,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x2744C,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x2748C,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x2748D,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x274CD,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x2750D,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x2750E,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x2754E,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x2758F,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x2758F,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x275D0,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x275D0,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27610,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        { 0x1900194,  0xFFFFFFFF, 0x27651,    1 },                          \
        /* DFCDL for Flow Sdram */                                          \
        { 0x2e00190,  0xFFFFFFFF, 0x0,        1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x0,        1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x1000,     1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x3041,     1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x4041,     1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x5082,     1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x7082,     1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x80C2,     1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x90C3,     1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0xB103,     1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0xC104,     1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0xD144,     1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0xF144,     1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x10185,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x12185,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x131C5,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x141C6,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x16206,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x17207,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x18247,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x1A247,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x1B288,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x1C288,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x1E2C9,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x1F2C9,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x202C9,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x2230A,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x2330A,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x2434B,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x2634B,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x2738B,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x2738C,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x273CC,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x273CD,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x2740D,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x2740D,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x2744E,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x2744E,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x2748E,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x2748F,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274CF,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        { 0x2e00194,  0xFFFFFFFF, 0x274D0,    1 },                          \
        /* -- Phy Tx Delay mode */                                          \
        { 0x03800054, 0xFFFFFFFF, 0x2880CE2,  1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x2890CE2,  1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x28A0CE2,  1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x28B0CE2,  1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x28C0CE2,  1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x28D0CE2,  1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x28E0CE2,  1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x28F0CE2,  1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x2900CE2,  1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x2910CE2,  1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x2920CE2,  1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x2930CE2,  1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x89140,    1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x99140,    1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0xA9140,    1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0xB9140,    1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0xC9140,    1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0xD9140,    1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0xE9140,    1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0xF9140,    1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x109140,   1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x119140,   1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x129140,   1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000001,  200 },                        \
        { 0x03800054, 0xFFFFFFFF, 0x139140,   1 },                          \
        { 0x03800040, 0xFFFFFFFF, 0x0000000,  200 },                        \
        /*-------------AN 146-----------------*/                            \
        /* Max Descriptors */                                               \
        /* Port 0 */                                                        \
        { 0x1800100,  0xFFF,      0x12,       1 },                          \
        { 0x1800200,  0xFFF,      0x2,        1 },                          \
        { 0x1800210,  0xFFF,      0x2,        1 },                          \
        { 0x1800220,  0xFFF,      0x2,        1 },                          \
        { 0x1800230,  0xFFF,      0x2,        1 },                          \
        { 0x1800240,  0xFFF,      0x2,        1 },                          \
        { 0x1800250,  0xFFF,      0x2,        1 },                          \
        { 0x1800260,  0xFFF,      0x3,        1 },                          \
        { 0x1800270,  0xFFF,      0x3,        1 },                          \
        /* Port 1 */                                                        \
        { 0x1801100,  0xFFF,      0x12,       1 },                          \
        { 0x1801200,  0xFFF,      0x2,        1 },                          \
        { 0x1801210,  0xFFF,      0x2,        1 },                          \
        { 0x1801220,  0xFFF,      0x2,        1 },                          \
        { 0x1801230,  0xFFF,      0x2,        1 },                          \
        { 0x1801240,  0xFFF,      0x2,        1 },                          \
        { 0x1801250,  0xFFF,      0x2,        1 },                          \
        { 0x1801260,  0xFFF,      0x3,        1 },                          \
        { 0x1801270,  0xFFF,      0x3,        1 },                          \
        /* Port 2 */                                                        \
        { 0x1802100,  0xFFF,      0x12,       1 },                          \
        { 0x1802200,  0xFFF,      0x2,        1 },                          \
        { 0x1802210,  0xFFF,      0x2,        1 },                          \
        { 0x1802220,  0xFFF,      0x2,        1 },                          \
        { 0x1802230,  0xFFF,      0x2,        1 },                          \
        { 0x1802240,  0xFFF,      0x2,        1 },                          \
        { 0x1802250,  0xFFF,      0x2,        1 },                          \
        { 0x1802260,  0xFFF,      0x3,        1 },                          \
        { 0x1802270,  0xFFF,      0x3,        1 },                          \
        /* Port 3 */                                                        \
        { 0x1803100,  0xFFF,      0x12,       1 },                          \
        { 0x1803200,  0xFFF,      0x2,        1 },                          \
        { 0x1803210,  0xFFF,      0x2,        1 },                          \
        { 0x1803220,  0xFFF,      0x2,        1 },                          \
        { 0x1803230,  0xFFF,      0x2,        1 },                          \
        { 0x1803240,  0xFFF,      0x2,        1 },                          \
        { 0x1803250,  0xFFF,      0x2,        1 },                          \
        { 0x1803260,  0xFFF,      0x3,        1 },                          \
        { 0x1803270,  0xFFF,      0x3,        1 },                          \
        /* Port 4 */                                                        \
        { 0x1804100,  0xFFF,      0x12,       1 },                          \
        { 0x1804200,  0xFFF,      0x2,        1 },                          \
        { 0x1804210,  0xFFF,      0x2,        1 },                          \
        { 0x1804220,  0xFFF,      0x2,        1 },                          \
        { 0x1804230,  0xFFF,      0x2,        1 },                          \
        { 0x1804240,  0xFFF,      0x2,        1 },                          \
        { 0x1804250,  0xFFF,      0x2,        1 },                          \
        { 0x1804260,  0xFFF,      0x3,        1 },                          \
        { 0x1804270,  0xFFF,      0x3,        1 },                          \
        /* Port 5 */                                                        \
        { 0x1805100,  0xFFF,      0x12,       1 },                          \
        { 0x1805200,  0xFFF,      0x2,        1 },                          \
        { 0x1805210,  0xFFF,      0x2,        1 },                          \
        { 0x1805220,  0xFFF,      0x2,        1 },                          \
        { 0x1805230,  0xFFF,      0x2,        1 },                          \
        { 0x1805240,  0xFFF,      0x2,        1 },                          \
        { 0x1805250,  0xFFF,      0x2,        1 },                          \
        { 0x1805260,  0xFFF,      0x3,        1 },                          \
        { 0x1805270,  0xFFF,      0x3,        1 },                          \
        /* Port 6 */                                                        \
        { 0x1806100,  0xFFF,      0x12,       1 },                          \
        { 0x1806200,  0xFFF,      0x2,        1 },                          \
        { 0x1806210,  0xFFF,      0x2,        1 },                          \
        { 0x1806220,  0xFFF,      0x2,        1 },                          \
        { 0x1806230,  0xFFF,      0x2,        1 },                          \
        { 0x1806240,  0xFFF,      0x2,        1 },                          \
        { 0x1806250,  0xFFF,      0x2,        1 },                          \
        { 0x1806260,  0xFFF,      0x3,        1 },                          \
        { 0x1806270,  0xFFF,      0x3,        1 },                          \
        /* Port 7 */                                                        \
        { 0x1807100,  0xFFF,      0x12,       1 },                          \
        { 0x1807200,  0xFFF,      0x2,        1 },                          \
        { 0x1807210,  0xFFF,      0x2,        1 },                          \
        { 0x1807220,  0xFFF,      0x2,        1 },                          \
        { 0x1807230,  0xFFF,      0x2,        1 },                          \
        { 0x1807240,  0xFFF,      0x2,        1 },                          \
        { 0x1807250,  0xFFF,      0x2,        1 },                          \
        { 0x1807260,  0xFFF,      0x3,        1 },                          \
        { 0x1807270,  0xFFF,      0x3,        1 },                          \
        /* Port 8 */                                                        \
        { 0x1808100,  0xFFF,      0x12,       1 },                          \
        { 0x1808200,  0xFFF,      0x2,        1 },                          \
        { 0x1808210,  0xFFF,      0x2,        1 },                          \
        { 0x1808220,  0xFFF,      0x2,        1 },                          \
        { 0x1808230,  0xFFF,      0x2,        1 },                          \
        { 0x1808240,  0xFFF,      0x2,        1 },                          \
        { 0x1808250,  0xFFF,      0x2,        1 },                          \
        { 0x1808260,  0xFFF,      0x3,        1 },                          \
        { 0x1808270,  0xFFF,      0x3,        1 },                          \
        /* Port 9 */                                                        \
        { 0x1809100,  0xFFF,      0x12,       1 },                          \
        { 0x1809200,  0xFFF,      0x2,        1 },                          \
        { 0x1809210,  0xFFF,      0x2,        1 },                          \
        { 0x1809220,  0xFFF,      0x2,        1 },                          \
        { 0x1809230,  0xFFF,      0x2,        1 },                          \
        { 0x1809240,  0xFFF,      0x2,        1 },                          \
        { 0x1809250,  0xFFF,      0x2,        1 },                          \
        { 0x1809260,  0xFFF,      0x3,        1 },                          \
        { 0x1809270,  0xFFF,      0x3,        1 },                          \
        /* Port 10 */                                                       \
        { 0x180A100,  0xFFF,      0x12,       1 },                          \
        { 0x180A200,  0xFFF,      0x2,        1 },                          \
        { 0x180A210,  0xFFF,      0x2,        1 },                          \
        { 0x180A220,  0xFFF,      0x2,        1 },                          \
        { 0x180A230,  0xFFF,      0x2,        1 },                          \
        { 0x180A240,  0xFFF,      0x2,        1 },                          \
        { 0x180A250,  0xFFF,      0x2,        1 },                          \
        { 0x180A260,  0xFFF,      0x3,        1 },                          \
        { 0x180A270,  0xFFF,      0x3,        1 },                          \
        /* Port 11 */                                                       \
        { 0x180B100,  0xFFF,      0x12,       1 },                          \
        { 0x180B200,  0xFFF,      0x2,        1 },                          \
        { 0x180B210,  0xFFF,      0x2,        1 },                          \
        { 0x180B220,  0xFFF,      0x2,        1 },                          \
        { 0x180B230,  0xFFF,      0x2,        1 },                          \
        { 0x180B240,  0xFFF,      0x2,        1 },                          \
        { 0x180B250,  0xFFF,      0x2,        1 },                          \
        { 0x180B260,  0xFFF,      0x3,        1 },                          \
        { 0x180B270,  0xFFF,      0x3,        1 },                          \
        /* buffers mode - 2k */                                             \
        { 0x0301400C, 0x30000000, 0x10000000, 1 },                          \
        /* Max buffers */                                                   \
        { 0x3011000,  0xFFF,      0xFFF,      1 },                          \
        { 0x3011004,  0xFFF,      0xFFF,      1 },                          \
        { 0x3011008,  0xFFF,      0xFFF,      1 },                          \
        { 0x301100C,  0xFFF,      0xFFF,      1 },                          \
        { 0x3011010,  0xFFF,      0xFFF,      1 },                          \
        { 0x3011014,  0xFFF,      0xFFF,      1 },                          \
        { 0x3011018,  0xFFF,      0xFFF,      1 },                          \
        { 0x301101C,  0xFFF,      0xFFF,      1 },                          \
        { 0x3011020,  0xFFF,      0xFFF,      1 },                          \
        { 0x3011024,  0xFFF,      0xFFF,      1 },                          \
        { 0x3011028,  0xFFF,      0xFFF,      1 },                          \
        { 0x301102C,  0xFFF,      0xFFF,      1 },                          \
        /* Delimiter */                                                     \
        { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 }                           \
}

#define GT_SAMBA_REG_VAL_INFO_LIST                                             \
{                                                                              \
    {0x00000000, 0x00000000, 0x00000000, 0}, /* Section 1 - revision 0,1 */    \
    {0x00000001, 0x00000000, 0x00000000, 0},                                   \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /*  Delimiter   */                \
    /*  after phase, 1, before start init  */                                  \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /*  Delimiter   */                \
    /*  after start init before phase 2  */                                    \
    /* ByPass Dll ,HSTL Recive mode, Clear Meassage to Narrow  */              \
    {0x0000006c, 0xFFFFFFFF, 0x00a61861, 1},                                   \
    {0x00000070, 0xFFFFFFFF, 0x00001861, 1},                                   \
    /* -- DRAM E-Ver Parameters */                                             \
    {0x01900100, 0xFFFFFFFF, 0x77      , 1},/*  DRAM A-Ver Parameters  */      \
    {0x01900104, 0xFFFFFFFF, 0x41      , 1},                                   \
    {0x01900108, 0xFFFFFFFF, 0x2       , 1},                                   \
    {0x0190010c, 0xFFFFFFFF, 0x17000850, 1},                                   \
    {0x01900110, 0xFFFFFFFF, 0x1732130 , 1},                                   \
    {0x01900114, 0xFFFFFFFF, 0x45      , 1},                                   \
    {0x01900118, 0xFFFFFFFF, 0x3       , 1},                                   \
    {0x01900118, 0xFFFFFFFF, 0x4       , 1},                                   \
    /* -- DFCDL Adjust */                                                      \
    {0x01900180, 0xFFFFFFFF, 0x00400000, 1},                                   \
    {0x01900184, 0xFFFFFFFF, 0x10d80180, 1},                                   \
    /*  SRAM parameters */                                                     \
    {0x02e00008, 0xFFFFFFFF, 0x0000001B, 1},                                   \
    {0x02e00018, 0xFFFFFFFF, 0x0000000B, 1},                                   \
    {0x02e00280, 0xFFFFFFFF, 0x10420000, 1},                                   \
    {0x02e00284, 0xFFFFFFFF, 0x10d80100, 1},                                   \
    {0x02e00180, 0xFFFFFFFF, 0x00400000, 1},/* Flow Dram Adjust  */            \
    {0x02e00184, 0xFFFFFFFF, 0x10d80180, 1},                                   \
    {0x02e0010c, 0xFFFFFFFF, 0xfa310e70, 1},                                   \
    /*  Phy address  */                                                        \
    {0x03800064, 0xFFFFFFFF, 0x1AC5A928, 1},                                   \
    {0x03800060, 0xFFFFFFFF, 0x2728C1EE, 1},                                   \
    /* Flow DRAM Parameters  */                                                \
    {0x02e00100, 0xFFFFFFFF, 0x33      , 1},                                   \
    {0x02e00104, 0xFFFFFFFF, 0x31      , 1},                                   \
    {0x02e00108, 0xFFFFFFFF, 0x2       , 1},                                   \
    {0x02e00110, 0xFFFFFFFF, 0x00722240, 1},                                   \
    {0x02e00114, 0xFFFFFFFF, 0x3306    , 1},                                   \
    {0x02e00118, 0xFFFFFFFF, 0x3       , 1},                                   \
    {0x02e00118, 0xFFFFFFFF, 0x4       , 1},                                   \
    {0x02e0010c, 0xFFFFFFFF, 0xFA3106E0, 1},                                   \
    /*  DFCDL for Buffer Sdram  */                                             \
    /*  value = ( (dqsDel)|(clkOutDel<<6)|(rxClkDel<<12)|(sysClkDel<<18)); */  \
    {0x1900190, 0xFFFFFFFF, 0x0    , 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x0    , 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x2040 , 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x4041 , 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x6081 , 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x80C2 , 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0xA0C2 , 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0xB103 , 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0xD103 , 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0xF143 , 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x11184, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x13184, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x151C5, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x17205, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x19206, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x1B246, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x1D287, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x1E287, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x202C7, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x222C8, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x24308, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x26349, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x27349, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x2738A, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x273CA, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x273CA, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x2740B, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x2740B, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x2744C, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x2748C, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x2748D, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x274CD, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x2750D, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x2750E, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x2754E, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x2758F, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x2758F, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x275D0, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x275D0, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x27610, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x27651, 1},                                       \
    {0x1900194, 0xFFFFFFFF, 0x27651, 1},                                       \
    /*  DFCDL for Flow Sdram  */                                               \
    {0x2e00190, 0xFFFFFFFF, 0x0    , 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x0    , 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x1000 , 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x3041 , 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x4041 , 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x5082 , 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x7082 , 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x80C2 , 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x90C3 , 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0xB103 , 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0xC104 , 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0xD144 , 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0xF144 , 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x10185, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x12185, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x131C5, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x141C6, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x16206, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x17207, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x18247, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x1A247, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x1B288, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x1C288, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x1E2C9, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x1F2C9, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x202C9, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x2230A, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x2330A, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x2434B, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x2634B, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x2738B, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x2738C, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x273CC, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x273CD, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x2740D, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x2740D, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x2744E, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x2744E, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x2748E, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x2748F, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x274CF, 1},                                       \
    {0x2e00194, 0xFFFFFFFF, 0x274D0, 1},                                       \
    /*  DFCDL for C22 &  SRAM  */                                              \
    {0x02e00290, 0xFFFFFFFF, 0x0      , 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x0      , 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x41     , 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x1041042, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x1041082, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x1082083, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x20820C4, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x2083105, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x20C3105, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x20C4146, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x3104147, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x3105188, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x3105188, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x41461C9, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x414620A, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x414720B, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x518724B, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x518824C, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x51C828D, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x61C92CE, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x61C92CE, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x620930F, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x620A310, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x724A351, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x724B351, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x724B392, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x828C3D3, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x828C3D4, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x82CD414, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x92CD415, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x92CE456, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x930E497, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0x930F497, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0xA30F4D8, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0xA3504D9, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0xA35051A, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0xB39151A, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0xB39155B, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0xB39159C, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0xC3D259D, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0xC3D25DD, 1},                                    \
    {0x02e00294, 0xFFFFFFFF, 0xC4135DE, 1},                                    \
    /*  -- Phy Tx Delay mode  */                                               \
    {0x03800054, 0xFFFFFFFF, 0x2880CE2, 1},/*  Phy Tx Delay mode */            \
    {0x03800040, 0xFFFFFFFF, 0x0000001,  200},/*  delay function Led Override*/\
    {0x03800054, 0xFFFFFFFF, 0x2890CE2, 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x28A0CE2, 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x28B0CE2, 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x28C0CE2, 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x28D0CE2, 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x28E0CE2, 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x28F0CE2, 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x2900CE2, 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x2910CE2, 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x2920CE2, 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x2930CE2, 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x89140  , 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x99140  , 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0xA9140  , 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0xB9140  , 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0xC9140  , 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0xD9140  , 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0xE9140  , 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0xF9140  , 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x109140 , 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x119140 , 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x129140 , 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000001, 200},/*  delay function Led Override */\
    {0x03800054, 0xFFFFFFFF, 0x139140 , 1},                                    \
    {0x03800040, 0xFFFFFFFF, 0x0000000, 200},/*  delay function Led Override */\
    /*  Delimiter  */                                                          \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}                                    \
}

#define GT_SAMBA_XG_REG_VAL_INFO_LIST                                          \
{                                                                              \
        {0x00000000, 0x00000000, 0x00000000, 0}, /* section 1 - revision 0,1 */    \
        {0x00000001, 0x00000000, 0x00000000, 0},                                   \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* Delimiter */                   \
                                                /* after phase 1 before start init */                       \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* Delimiter */                   \
                                                /* after start init before phase 2 */                  \
        {0x0000006C, 0xFFFFFFFF, 0x00A41861, 1}, /* W C2C ByPass Dll ,        */   \
                                                                                         /* CMOS Recive mode,         */   \
                                                                                         /* Clear Meassage to Narrow  */   \
        {0x00000070, 0xFFFFFFFF, 0x00001861, 1}, /* N C2C */                       \
        {0x0190010c, 0x07000000, 0x06000000, 1}, /* mask changed from         */   \
                                                                                         /* 0xffffffff  Addr Decode   */   \
                                                                                         /* changed to from 7         */   \
        /* SRAM parameters */                                                      \
        {0x02e00008, 0xFFFFFFFF, 0x0000001B, 1}, /* external Narrow SRAM enable */ \
        {0x02e00018, 0xFFFFFFFF, 0x0000000B, 1}, /* external Wide SRAM enable */   \
        {0x02e00280, 0xFFFFFFFF, 0x10420000, 1},                                   \
        {0x02e00284, 0xFFFFFFFF, 0x10d80100, 1},                                   \
        /* Flow Dram Adjust */                                                     \
        /*{0x02e00180, 0xFFFFFFFF, 0x00400000, 1},*/                                   \
        /*{0x02e00184, 0xFFFFFFFF, 0x10d80180, 1},*/                               \
        /* Flow DRAM Parameters */                                                 \
        {0x02e00114, 0x0000FF00, 0x7700    , 1}, /* read counter, write counter */ \
        {0x02e00118, 0xFFFFFFFF, 0x3       , 1},                                   \
        {0x02e00118, 0xFFFFFFFF, 0x4       , 1},                                   \
        {0x02e0010c, 0xFFFFF000, 0xFA710000, 1}, /* Addr decoding = 2 */           \
        /* Buffer SDRAM DFCDL Adjust */                                            \
        {0x01900180, 0xFFFFFFFF, 0x00400000, 1}, /* DFCDL Adjust */                \
        {0x01900184, 0xFFFFFFFF, 0x10d80180, 1},                                   \
        /* Uplink and Sram DFCDL */                                                \
        /* DFCDL for C22 &  SRAM */                                                \
        {0x02e00290, 0xFFFFFFFF, 0x0       , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x0       , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x1041041 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x1041042 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x2082083 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x20C3083 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x30C30C4 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x31040C5 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x4145106 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x4145107 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x5186148 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x61C7188 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x61C7189 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x72081CA , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x72491CB , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x824920C , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x828A20D , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x92CB24D , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x92CB24E , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0xA30C28F , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0xB34D2D0 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0xB34D2D1 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0xC38E312 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0xC3CF312 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0xD3CF353 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0xD410354 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0xE451395 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0xE451396 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0xF4923D7 , 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x104D3417, 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x104D3418, 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x11514459, 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x1155545A, 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x1255549B, 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x1259649C, 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x135D74DC, 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x135D74DD, 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x1461851E, 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x1565955F, 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x15659560, 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x1669A5A1, 1},                                   \
        {0x02e00294, 0xFFFFFFFF, 0x166DB5A1, 1},                                   \
        {0x1900190 , 0xFFFFFFFF, 0x0           , 1}, /* SDRAM DFCDL address */         \
                                                 /* register=0          */         \
        {0x1900194 , 0xFFFFFFFF, 0x0           , 1}, /* SDRAM Data DFCDL fill */       \
        {0x1900194 , 0xFFFFFFFF, 0x1041           , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x3041           , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x4082           , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x60C3           , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x7104           , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x9104           , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0xA145           , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0xC186           , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0xD1C7           , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0xE1C7           , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x10208   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x11249   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x13249   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x1428A   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x162CB   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x1730C   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x1930C   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x1A34D   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x1B38E   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x1D38E   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x1E3CF   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x20410   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x21451   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x23451   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x24492   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x264D3   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x27514   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x27514   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x27555   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x27596   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x27596   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x275D7   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x27618   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x27659   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x27659   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x2769A   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x276DB   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x276DB   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x2771C   , 1},                                   \
        {0x1900194 , 0xFFFFFFFF, 0x2775D   , 1},                                   \
        {0x2e00190 , 0xFFFFFFFF, 0x0           , 1}, /* Flow SDRAM address DFCDL */    \
        {0x2e00194 , 0xFFFFFFFF, 0x0           , 1}, /* Flow SDRAM Data DFCDL */       \
        {0x2e00194 , 0xFFFFFFFF, 0x42      , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x1083    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x10C5    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x2107    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x2148    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x318A    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x31CC    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x424D    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x428F    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x52D1    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x5312    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x6354    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x6396    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x73D7    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x7419    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x745B    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x849C    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x84DE    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x9520    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x9561    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0xA5A3    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0xA5E5    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0xB666    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0xB6A7    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0xC6E7    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0xC727    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0xD767    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0xD7A7    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0xE7E7    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0xE827    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0xE867    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0xF8A7    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0xF8E7    , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x10927   , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x10967   , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x119A7   , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x119E7   , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x129E7   , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x129E7   , 1},                                   \
        {0x2e00194 , 0xFFFFFFFF, 0x139E7   , 1},                                   \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* Delimiter */                   \
                    /* section 2 - revisions 2,3,4,5 */                               \
        {0x00000002, 0x00000000, 0x00000000, 0},                                   \
        {0x00000003, 0x00000000, 0x00000000, 0},                                   \
        {0x00000004, 0x00000000, 0x00000000, 0},                                   \
        {0x00000005, 0x00000000, 0x00000000, 0},                                   \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* Delimiter */                   \
                                /* pre start init */                                       \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* Delimiter */                   \
                                /* post start init */                                      \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}  /* Delimiter */                   \
}

#define GT_SAMBA_6X8_XG_REG_VAL_INFO_LIST                                     \
{                                                                             \
    { 0x00000000, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000001, 0x00000000, 0x00000000, 0 },                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                                \
    { 0x01834000, 0xFFFFFFFF, 0x0000F1DF, 1 },                                \
    { 0x0000006C, 0xFFFFFFFF, 0x00A41863, 1 },                                \
    { 0x00000070, 0xFFFFFFFF, 0x00001863, 1 },                                \
    { 0x0190010c, 0x07000000, 0x06000000, 1 },                                \
    { 0x02e00008, 0xFFFFFFFF, 0x0000001B, 1 },                                \
    { 0x02e00018, 0xFFFFFFFF, 0x0000000B, 1 },                                \
    { 0x02e00280, 0xFFFFFFFF, 0x10420000, 1 },                                \
    { 0x02e00284, 0xFFFFFFFF, 0x10d80100, 1 },                                \
    { 0x02e00114, 0x0000FF00, 0x7700    , 1 },                                \
    { 0x02e00118, 0xFFFFFFFF, 0x3       , 1 },                                \
    { 0x02e00118, 0xFFFFFFFF, 0x4       , 1 },                                \
    { 0x02e0010c, 0xFFFFF000, 0xFA710000, 1 },                                \
    { 0x01900180, 0xFFFFFFFF, 0x00400000, 1 },                                \
    { 0x01900184, 0xFFFFFFFF, 0x10d80180, 1 },                                \
    { 0x02e00290, 0xFFFFFFFF, 0x0       , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x0       , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x1041041 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x1041042 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x2082083 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x20C3083 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x30C30C4 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x31040C5 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x4145106 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x4145107 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x5186148 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x61C7188 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x61C7189 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x72081CA , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x72491CB , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x824920C , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x828A20D , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x92CB24D , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x92CB24E , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0xA30C28F , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0xB34D2D0 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0xB34D2D1 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0xC38E312 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0xC3CF312 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0xD3CF353 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0xD410354 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0xE451395 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0xE451396 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0xF4923D7 , 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x104D3417, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x104D3418, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x11514459, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x1155545A, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x1255549B, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x1259649C, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x135D74DC, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x135D74DD, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x1461851E, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x1565955F, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x15659560, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x1669A5A1, 1 },                                \
    { 0x02e00294, 0xFFFFFFFF, 0x166DB5A1, 1 },                                \
    { 0x1900190 , 0xFFFFFFFF, 0x0       , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x0       , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x1041    , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x3041    , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x4082    , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x60C3    , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x7104    , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x9104    , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0xA145    , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0xC186    , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0xD1C7    , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0xE1C7    , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x10208   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x11249   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x13249   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x1428A   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x162CB   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x1730C   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x1930C   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x1A34D   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x1B38E   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x1D38E   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x1E3CF   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x20410   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x21451   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x23451   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x24492   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x264D3   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x27514   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x27514   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x27555   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x27596   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x27596   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x275D7   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x27618   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x27659   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x27659   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x2769A   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x276DB   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x276DB   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x2771C   , 1 },                                \
    { 0x1900194 , 0xFFFFFFFF, 0x2775D   , 1 },                                \
    { 0x2e00190 , 0xFFFFFFFF, 0x0       , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x0       , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x42      , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x1083    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x10C5    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x2107    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x2148    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x318A    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x31CC    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x424D    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x428F    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x52D1    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x5312    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x6354    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x6396    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x73D7    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x7419    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x745B    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x849C    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x84DE    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x9520    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x9561    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0xA5A3    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0xA5E5    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0xB666    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0xB6A7    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0xC6E7    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0xC727    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0xD767    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0xD7A7    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0xE7E7    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0xE827    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0xE867    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0xF8A7    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0xF8E7    , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x10927   , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x10967   , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x119A7   , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x119E7   , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x129E7   , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x129E7   , 1 },                                \
    { 0x2e00194 , 0xFFFFFFFF, 0x139E7   , 1 },                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000002, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000003, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000004, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000005, 0x00000000, 0x00000000, 0 },                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 }                                 \
}

#define GT_PUMA_REG_VAL_INFO_LIST                                             \
{                                                                             \
    { 0x00000000, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000001, 0x00000000, 0x00000000, 0 },                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* Delimiter */                 \
        /* Before start init */                                               \
        /* External Memories IO config */                                     \
    { 0x01800000, 0x06000000, 0x00047857, 1},  /* add_dq_delay: csu (csu0) */ \
    { 0x01900000, 0x06000000, 0x00043857, 1},  /* add_dq_delay: lpm1(csu1) */ \
    { 0x01A00000, 0x06000000, 0x00043857, 1},  /* add_dq_delay: lpm2(csu2) */ \
    { 0x01B00000, 0x06000000, 0x00043857, 1},  /* add_dq_delay: lpm3(csu3) */ \
    { 0x01600008, 0x00000180, 0x80000099, 1},  /* add_dq_delay: i_dsu */      \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0},  /* Delimiter */                \
        /* After start init */                                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0}  /* Delimiter */                 \
}

#define GT_PUMA_QDR_REG_VAL_INFO_LIST                                         \
{                                                                             \
    { 0x00000000, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000001, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000002, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000003, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000004, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000005, 0x00000000, 0x00000000, 0 },                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* Delimiter */                 \
        /* Before start init */                                               \
    /* set DSU and CSU Rx clock scheme for parallel expansion */              \
    { 0x0000005c, 0x00240002, 0x000000a2, 1},                                 \
        /* External Memories IO config */                                     \
    /* IDSU Control 1: SramRdLatencyInType = NonInteger */                    \
    { 0x01600008, 0x80000070, 0x80000099, 1},                                 \
    /* DSU Control1 Control 1: SramRdLatencyInType = NonInteger */            \
    { 0x01700008, 0x80000070, 0x80000099, 1},                                 \
    { 0x01800000, 0x00000044, 0x00047857, 1},  /* add_dq_delay: csu (csu0) */ \
    { 0x01900000, 0x00000044, 0x00043857, 1},  /* add_dq_delay: lpm1(csu1) */ \
    { 0x01A00000, 0x00000044, 0x00043857, 1},  /* add_dq_delay: lpm2(csu2) */ \
    { 0x01B00000, 0x00000044, 0x00043857, 1},  /* add_dq_delay: lpm3(csu3) */ \
    /* TX_SM_control */                                                       \
    { 0x01800014, 0x00000038, 0x00000458, 1},                                 \
    { 0x01900014, 0x00000038, 0x00000458, 1},                                 \
    { 0x01A00014, 0x00000038, 0x00000458, 1},                                 \
    { 0x01B00014, 0x00000038, 0x00000458, 1},                                 \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0},  /* Delimiter */                \
        /* After start init */                                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0}  /* Delimiter */                 \
}

#define GT_PUMA_RLD_REG_VAL_INFO_LIST                                         \
{                                                                             \
    { 0x00000000, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000001, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000002, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000003, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000004, 0x00000000, 0x00000000, 0 },                                \
    { 0x00000005, 0x00000000, 0x00000000, 0 },                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* Delimiter */                 \
        /* Before start init */                                               \
    /* set DSU and CSU Rx clock scheme for parallel expansion */              \
    { 0x0000005c, 0x00240003, 0x000000ab, 1},                                 \
    /* set TX DMA config 1 register for RLD RAM */                            \
    { 0x01000004, 0x01000000, 0x01259844, 1},                                 \
    /* RLD MRS Configurations */                                              \
    { 0x02300004, 0x00000380, 0x000000038B, 1},                               \
    /* Set RLD Address Decoding register */                                   \
    { 0x0230000C, 0x00040000, 0x00023488, 1},                                 \
    /* IDSU Control 1: SramRdLatencyInType = NonInteger */                    \
    { 0x01600008, 0x80000070, 0x80000099, 1},                                 \
    /* IDSU Control 0: SBLatency = 2, RdLatency =3 */                         \
    { 0x01600004, 0xfe000000, 0x883fe108, 1},                                 \
    /* IDSU Control 1: SramRdLatencyInType = NonInteger */                    \
    { 0x01600008, 0x80000070, 0x80000099, 1},                                 \
    /* External Memories IO config */                                         \
    { 0x01800000, 0x00000044, 0x00047857, 1},  /* add_dq_delay: csu (csu0) */ \
    { 0x01900000, 0x00000044, 0x00043857, 1},  /* add_dq_delay: lpm1(csu1) */ \
    { 0x01A00000, 0x00000044, 0x00043857, 1},  /* add_dq_delay: lpm2(csu2) */ \
    { 0x01B00000, 0x00000044, 0x00043857, 1},  /* add_dq_delay: lpm3(csu3) */ \
    /* TX_SM_control */                                                       \
    { 0x01800014, 0x00000038, 0x00000458, 1},  /* csu0 read to write delay */ \
    { 0x01900014, 0x00000038, 0x00000458, 1},  /* lpm1 read to write delay */ \
    { 0x01A00014, 0x00000038, 0x00000458, 1},  /* lpm2 read to write delay */ \
    { 0x01B00014, 0x00000038, 0x00000458, 1},  /* lpm3 read to write delay */ \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0},  /* Delimiter */                \
        /* After start init */                                                \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0}  /* Delimiter */                 \
}



/* Dummy for competability, for init process. */
#define GT_DUMMY_REG_VAL_INFO_LIST                                          \
{                                                                           \
    {0x00000000, 0x00000000, 0x00000000, 0},                                \
    {0x00000001, 0x00000000, 0x00000000, 0},                                \
    {0x00000002, 0x00000000, 0x00000000, 0},                                \
    {0x00000003, 0x00000000, 0x00000000, 0},                                \
    {0x00000004, 0x00000000, 0x00000000, 0},                                \
    {0x00000005, 0x00000000, 0x00000000, 0},                                \
    {0x00000006, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}     /* Delimiter        */      \
}

/* Dummy for competability, for init process. */
#define GT_SAPPHIRE_REG_VAL_INFO_LIST                                       \
{                                                                           \
    {0x00000000, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}     /* Delimiter        */      \
}

#define  GT_TWIST_D_24G_FS_REG_VAL_INFO_LIST                           \
{                                                                           \
    {   /* device 0 */                                                      \
        {0x00000000, 0x00000000, 0x00000000, 0},  /* revision D0 */         \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0},  /* delimiter   */         \
        {0x0000006C, 0xFFFFFFFF, 0x00001860, 1},                            \
        {0x00000070, 0xFFFFFFFF, 0x00001860, 1},                            \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0},  /* delimiter   */         \
        {0x01800004, 0xFFFF0000, 0x0B000000, 1},                            \
        {0x0190010C, 0x00000EFF, 0x00000E00, 1},                            \
        {0x03014010, 0x0000FFFF, 0x00002000, 1},                            \
        {0x00804014, 0x007FE000, 0xFFFF0000, 1},                            \
        {0x01004014, 0x007FE000, 0xFFFF0000, 1},                            \
        {0x0190010C, 0x0F000000, 0x07000000, 1},                            \
        {0x01900110, 0xFFFFFFFF, 0x01A32250, 1},                            \
        {0x01900120, 0xFFFFFFFF, 0x00000077, 1},                            \
        {0x0190012C, 0xFFFFFFFF, 0x37000850, 1},                            \
        {0x0190010C, 0x0F000000, 0x07000000, 1},                            \
        {0x01900190, 0xFFFFFFFF, 0x00000000, 1},                            \
        {0x01900194, 0xFFFFFFFF, 0x000142C7, 21},                           \
        {0x03014008, 0x00000001, 0x00000001, 1},                            \
        {0x00000058, 0x00010000, 0x00000000, 1},                            \
        {0x0000006C, 0x0003FFFF, 0x00001861, 1},                            \
        {0x00000070, 0x0001FFFF, 0x00001861, 1},                            \
        {0x03004020, 0xFFFFFFFF, 0x00000001, 1},                            \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* delimiter   */          \
        {0x00000001, 0x00000000, 0x00000000, 0}, /* revision D1 */          \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* delimiter   */          \
        {0x0000006C, 0xFFFFFFFF, 0x00a61860, 1},                            \
        {0x00000070, 0xFFFFFFFF, 0x00001860, 1},                            \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* delimiter   */          \
        {0x01900190, 0xFFFFFFFF, 0x00000000, 1},                            \
        {0x01900194, 0xFFFFFFFF, 0x0000F207, 64},                           \
        {0x02e00290, 0xFFFFFFFF, 0x00000000, 1},                            \
        {0x02e00294, 0xFFFFFFFF, 0x0C252013, 64},                           \
        {0x01800004, 0xFFFF0000, 0x0B000000, 1},                            \
        {0x0190010C, 0x00000EFF, 0x00000E00, 1},                            \
        {0x03014010, 0x0000FFFF, 0x00002000, 1},                            \
        {0x00804014, 0x007FE000, 0xFFFF0000, 1},                            \
        {0x01004014, 0x007FE000, 0xFFFF0000, 1},                            \
        {0x0190010C, 0x0F000000, 0x07000000, 1},                            \
        {0x01900110, 0xFFFFFFFF, 0x01732130, 1},                            \
        {0x01900114, 0xFFFFFFFF, 0x24928845, 1},                            \
        {0x01900100, 0xFFFFFFFF, 0x00000077, 1},                            \
        {0x0190010C, 0xFFFFFFFF, 0x17000850, 1},                            \
        {0x01900108, 0x00000002, 0x00000002, 1},                            \
        {0x01900104, 0x00000051, 0x00000041, 1},                            \
        {0x01900118, 0xFFFFFFFF, 0x00000003, 1},                            \
        {0x01900118, 0xFFFFFFFF, 0x00000004, 1},                            \
        {0x03014008, 0x00000001, 0x00000001, 1},                            \
        {0x0301400C, 0x30000000, 0x10000000, 1}, /*buffer size = 2K */      \
        {0x00000058, 0x00010000, 0x00000000, 1},                            \
        {0x0000006C, 0x00200000, 0x00200000, 1},                            \
        {0x0000006C, 0x0001FFFF, 0x00001861, 1},                            \
        {0x00000070, 0x0001FFFF, 0x00001861, 1},                            \
        {0x00000060, 0x00000840, 0x00000840, 1},                            \
        {0x03004020, 0xFFFFFFFF, 0x00000001, 1},                            \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}  /* delimiter   */          \
    }, /* end of device 0 */                                                \
    {   /* device 1 */                                                      \
        {0x00000000, 0x00000000, 0x00000000, 0}, /* revision D0 */          \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* delimiter   */          \
        {0x0000006C, 0xFFFFFFFF, 0x00001860, 1},                            \
        {0x00000070, 0xFFFFFFFF, 0x00001860, 1},                            \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* delimiter   */          \
        {0x01800004, 0xFFFF0000, 0x0B000000, 1},                            \
        {0x0190010C, 0x00000EFF, 0x00000E00, 1},                            \
        {0x03014010, 0x0000FFFF, 0x00002000, 1},                            \
        {0x00804014, 0x007FE000, 0xFFFF0000, 1},                            \
        {0x01004014, 0x007FE000, 0xFFFF0000, 1},                            \
        {0x0190010C, 0x0F000000, 0x07000000, 1},                            \
        {0x01900110, 0xFFFFFFFF, 0x01A32250, 1},                            \
        {0x01900120, 0xFFFFFFFF, 0x00000077, 1},                            \
        {0x0190012C, 0xFFFFFFFF, 0x37000850, 1},                            \
        {0x0190010C, 0x0F000000, 0x07000000, 1},                            \
        {0x01900190, 0xFFFFFFFF, 0x00000000, 1},                            \
        {0x01900194, 0xFFFFFFFF, 0x000142C7, 21},                           \
        {0x03014008, 0x00000001, 0x00000001, 1},                            \
        {0x00000058, 0x00010000, 0x00000000, 1},                            \
        {0x0000006C, 0x0003FFFF, 0x00001861, 1},                            \
        {0x00000070, 0x0001FFFF, 0x00001861, 1},                            \
        {0x03004020, 0xFFFFFFFF, 0x00000001, 1},                            \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* delimiter   */          \
        {0x00000001, 0x00000000, 0x00000000, 0}, /* revision D1 */          \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* delimiter   */          \
        {0x0000006C, 0xFFFFFFFF, 0x00a61860, 1},                            \
        {0x00000070, 0xFFFFFFFF, 0x00001860, 1},                            \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}, /* delimiter   */          \
        {0x01900190, 0xFFFFFFFF, 0x00000000, 1},                            \
        {0x01900194, 0xFFFFFFFF, 0x0000D205, 64},                           \
        {0x02e00290, 0xFFFFFFFF, 0x00000000, 1},                            \
        {0x02e00294, 0xFFFFFFFF, 0x0A1D2013, 64},                           \
        {0x01800004, 0xFFFF0000, 0x0B000000, 1},                            \
        {0x0190010C, 0x00000EFF, 0x00000E00, 1},                            \
        {0x03014010, 0x0000FFFF, 0x00002000, 1},                            \
        {0x00804014, 0x007FE000, 0xFFFF0000, 1},                            \
        {0x01004014, 0x007FE000, 0xFFFF0000, 1},                            \
        {0x0190010C, 0x0F000000, 0x07000000, 1},                            \
        {0x01900110, 0xFFFFFFFF, 0x01732130, 1},                            \
        {0x01900114, 0xFFFFFFFF, 0x24928845, 1},                            \
        {0x01900100, 0xFFFFFFFF, 0x00000077, 1},                            \
        {0x0190010C, 0xFFFFFFFF, 0x17000850, 1},                            \
        {0x01900108, 0x00000002, 0x00000002, 1},                            \
        {0x01900104, 0x00000051, 0x00000041, 1},                            \
        {0x01900118, 0xFFFFFFFF, 0x00000003, 1},                            \
        {0x01900118, 0xFFFFFFFF, 0x00000004, 1},                            \
        {0x03014008, 0x00000001, 0x00000001, 1},                            \
        {0x0301400C, 0x30000000, 0x10000000, 1}, /*buffer size = 2K */      \
        {0x00000058, 0x00010000, 0x00000000, 1},                            \
        {0x0000006C, 0x00200000, 0x00200000, 1},                            \
        {0x0000006C, 0x0001FFFF, 0x00001861, 1},                            \
        {0x00000070, 0x0001FFFF, 0x00001861, 1},                            \
        {0x00000060, 0x00000840, 0x00000840, 1},                            \
        {0x03004020, 0xFFFFFFFF, 0x00000001, 1},                            \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}  /* delimiter   */          \
    }/* end of device 1 */                                                  \
}

#define  GT_TWIST_D_48F4G_FS_REG_VAL_INFO_LIST                         \
{                                                                           \
    {0x00000000, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},                                \
    {0x0000006C, 0xFFFFFFFF, 0x00001860, 1},                                \
    {0x00000070, 0xFFFFFFFF, 0x00001860, 1},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},                                \
    {0x01800004, 0xFFFF0000, 0x0B000000, 1},                                \
    {0x0190010C, 0x00000EFF, 0x00000E00, 1},                                \
    {0x03014010, 0x0000FFFF, 0x00002000, 1},                                \
    {0x00804014, 0x007FE000, 0xFFFF0000, 1},                                \
    {0x01004014, 0x007FE000, 0xFFFF0000, 1},                                \
    {0x0190010C, 0x0F000000, 0x07000000, 1},                                \
    {0x01900110, 0xFFFFFFFF, 0x01A32250, 1},                                \
    {0x01900120, 0xFFFFFFFF, 0x00000077, 1},                                \
    {0x0190012C, 0xFFFFFFFF, 0x37000850, 1},                                \
    {0x02e00290, 0xFFFFFFFF, 0x00000000, 1}, /*start the dfcdl config*/     \
    {0x02e00294, 0xFFFFFFFF, 0x0935A685, 25},                               \
    {0x01900190, 0xFFFFFFFF, 0x00000000, 1}, /*dram dfcdl*/                 \
    {0x01900194, 0xFFFFFFFF, 0x000142C7, 22}, /*change from 21 to 22*/      \
    {0x03014008, 0x00000001, 0x00000001, 1},                                \
    {0x00000058, 0x0002FFFF, 0x00028001, 1}, /*uplink not back to back*/    \
    {0x0000006C, 0x0003FFFF, 0x00001861, 1},                                \
    {0x00000070, 0x0001FFFF, 0x00001861, 1},                                \
    {0x03004020, 0xFFFFFFFF, 0x00000001, 1}, /*change from 0x00300001 to 0x1*/ \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}                                 \
}

#define GT_TIGER_12_REG_VAL_INFO_LIST_PART1 \
    {0x00000000, 0x00000000, 0x00000000, 0},\
    {0x00000001, 0x00000000, 0x00000000, 0},\
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},\
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},\
    {0x03800064, 0xFFFFFFFF, 0x14944653, 1},\
    {0x03800060, 0xFFFFFFFF, 0x16c6c1ee, 1},\
    {0x2040208 , 0xffffffff, 0x00001fff, 1},\
    {0xEEEEEEEE, 0x00000000, 0x00000000, 0},\
    /* PhyReg       Mask        Data     */ \
    {0x03800054, 0xFFFFFFFF, 0x2880CE2 , 1},\
    {0x03800054, 0xFFFFFFFF, 0x2890CE2 , 1},\
    {0x03800054, 0xFFFFFFFF, 0x28A0CE2 , 1},\
    {0x03800054, 0xFFFFFFFF, 0x28B0CE2 , 1},\
    {0x03800054, 0xFFFFFFFF, 0x28C0CE2 , 1},\
    {0x03800054, 0xFFFFFFFF, 0x28D0CE2 , 1},\
    {0x03800054, 0xFFFFFFFF, 0x28E0CE2 , 1},\
    {0x03800054, 0xFFFFFFFF, 0x28F0CE2 , 1},\
    {0x03800054, 0xFFFFFFFF, 0x2900CE2 , 1},\
    {0x03800054, 0xFFFFFFFF, 0x2910CE2 , 1},\
    {0x03800054, 0xFFFFFFFF, 0x2920CE2 , 1},\
    {0x03800054, 0xFFFFFFFF, 0x2930CE2 , 1},\
    /*# phy auto media select disable    */ \
    {0x03800054, 0xFFFFFFFF, 0x370808B , 1},\
    {0x03800054, 0xFFFFFFFF, 0x371808B , 1},\
    {0x03800054, 0xFFFFFFFF, 0x372808B , 1},\
    {0x03800054, 0xFFFFFFFF, 0x373808B , 1},\
    /* phy reset                         */ \
    {0x03800054, 0xFFFFFFFF, 0x89140   , 1},\
    {0x03800054, 0xFFFFFFFF, 0x99140   , 1},\
    {0x03800054, 0xFFFFFFFF, 0xA9140   , 1},\
    {0x03800054, 0xFFFFFFFF, 0xB9140   , 1},\
    {0x03800054, 0xFFFFFFFF, 0xC9140   , 1},\
    {0x03800054, 0xFFFFFFFF, 0xD9140   , 1},\
    {0x03800054, 0xFFFFFFFF, 0xE9140   , 1},\
    {0x03800054, 0xFFFFFFFF, 0xF9140   , 1},\
    {0x03800054, 0xFFFFFFFF, 0x109140  , 1},\
    {0x03800054, 0xFFFFFFFF, 0x119140  , 1},\
    {0x03800054, 0xFFFFFFFF, 0x129140  , 1},\
    {0x03800054, 0xFFFFFFFF, 0x139140  , 1},\
    {0xEEEEEEEE, 0x00000000, 0x00000000, 0},\
    /*      Errata and defualt values    */ \
    /*RegAddr     Mask      Value   Times*/ \
    {0x0000006C, 0x00800001, 0x00800001, 1},\
    {0x00000070, 0x00000001, 0x00000001, 1},\
    {0x01900000, 0xFFFFFFFF, 0x00000029, 1},\
    {0x02E00000, 0x00000FFF, 0x00000333, 1},\
    {0x02E00020, 0x00000FFF, 0x00000333, 1},\
    {0x02E00030, 0x00000FFF, 0x00000333, 1},\
    {0x02E00010, 0x00000F00, 0x00000100, 1},\
    {0x02E00014, 0x0000FFFF, 0x0000F93B, 1},\
    /* don't change MTU bits 14:15 */       \
    {0x01800004, 0xFFFF3FFF, 0x1000220A, 1},\
    {0x0190010c, 0x0F000000, 0x00000000, 1},\
    {0x01800184, 0x00000010, 0x00000010, 1},\
    {0x01840000, 0x00000080, 0x00000080, 1},\
    {0x00804000, 0x000003FF, 0x00000044, 1},\
    {0x01004000, 0x000003FF, 0x00000044, 1},\
    {0x03800014, 0x00000180, 0x00000180, 1},\
    {0x02800198, 0x00002000, 0x00002000, 1},\
    {0x02E0000C, 0x00000007, 0x00000003, 1},\
    {0x02E0002C, 0x00000007, 0x00000003, 1},\
                                            \
    /* FTDLL SDRAM FUNCTION 300MHz */       \
                                            \
    { 0x01900190, 0xFFFFFFFF, 0x0     , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x44183 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x48184 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x4C184 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x50204 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x54205 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x58205 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x5C206 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x60286 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x64286 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x68287 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x6C307 , 1 },

#define GT_TIGER_12_REG_VAL_INFO_LIST_PART2 \
    { 0x01900194, 0xFFFFFFFF, 0x70307 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x74308 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x78388 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x7C388 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x80409 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x84409 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x8848A , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x8C50B , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x9058C , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x9460D , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x9468E , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x9868F , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x9C70F , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xA0790 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xA4811 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xA8892 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xAC913 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xB0914 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xB4995 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xB8A16 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xB8A97 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xB8B18 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xBCB99 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xC0C1A , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xC4C9B , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xC8D1C , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xCCD9D , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xD0E1E , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xD4E9F , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xD8F20 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xDCFA1 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xE1022 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xE50A3 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xE9124 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xED1A5 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xF1226 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xF52A7 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xF9328 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0xFD3A9 , 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x10142A, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1054AB, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x10952C, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x10D5AD, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x11162E, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1156AF, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x119730, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x11D7B1, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x121832, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1258B3, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x129934, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x12D9B5, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x131A36, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x135AB7, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x139B38, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x13DBB9, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x141C3A, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x145CBB, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x149D3C, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x14DDBD, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x151E3E, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x155EBF, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x159F40, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x15DFC1, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x162042, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1660C3, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x16A144, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x16E1C5, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x172246, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1762C7, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x17A348, 1 },

#define GT_TIGER_12_REG_VAL_INFO_LIST_PART3 \
    { 0x01900194, 0xFFFFFFFF, 0x17E3C9, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x18244A, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1864CB, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x18A54C, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x18E5CD, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x19264E, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1966CF, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x19A750, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x19E7D1, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1A2852, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1A68D3, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1AA954, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1AE9D5, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1B2A56, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1B6AD7, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1BAB58, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1BEBD9, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1C2C5A, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1C6CDB, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1CAD5C, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1CEDDD, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1D2E5E, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1D6EDF, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1DAF60, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1DEFE1, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1E3062, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1E70E3, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1EB164, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1EF1E5, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1F3266, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1F72E7, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FB368, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FF3E9, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FF46A, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FF4EB, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FF56C, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FF5ED, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FF66E, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FF6EF, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FF770, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FF7F1, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FF872, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FF8F3, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FF974, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FF9F5, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FFA76, 1 },\
    { 0x01900194, 0xFFFFFFFF, 0x1FFAF7, 1 },

    /* FTDLL SDRAM FUNCTION */

    /* FTDLL SRAM1 FUNCTION */

#define GT_TIGER_12_REG_VAL_INFO_LIST_PART4 \
    { 0x02E00110, 0xFFFFFFFF, 0x0     , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x104   , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x4104  , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x8185  , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xC185  , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x10205 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x14206 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x18286 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1C286 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x20307 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x24387 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x28407 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x2C488 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x30508 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x34589 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x38609 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x3C68A , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x4070A , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x4478B , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x4880B , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x4C88C , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x5090D , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x5498E , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x58A0F , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x5CA90 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x60B11 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x64B92 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x68C13 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x6CC94 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x70D15 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x74D96 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x78E17 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x7CE98 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x80F19 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x84F9A , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x8901B , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x8D09C , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x9111D , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x9519E , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x9921F , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x9D2A0 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xA1321 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xA53A2 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xA9423 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xAD4A4 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xB1525 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xB55A6 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xB9627 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xBD6A8 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xC1729 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xC57AA , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xC982B , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xCD8AC , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xD192D , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xD59AE , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xD9A2F , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xDDAB0 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xE1B31 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xE5BB2 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xE9C33 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xEDCB4 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xF1D35 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xF5DB6 , 1 },


#define GT_TIGER_12_REG_VAL_INFO_LIST_PART5 \
    { 0x02E00114, 0xFFFFFFFF, 0xF9E37 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0xFDEB8 , 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x101F39, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x105FBA, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x10A03B, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x10E0BC, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x11213D, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1161BE, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x11A23F, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x11E2C0, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x122341, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1263C2, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x12A443, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x12E4C4, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x132545, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1365C6, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x13A647, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x13E6C8, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x142749, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1467CA, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x14A84B, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x14E8CC, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x15294D, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1569CE, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x15AA4F, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x15EAD0, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x162B51, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x166BD2, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x16AC53, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x16ECD4, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x172D55, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x176DD6, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x17AE57, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x17EED8, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x182F59, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x186FDA, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x18B05B, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x18F0DC, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x19315D, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1971DE, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x19B25F, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x19F2E0, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1A3361, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1A73E2, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1AB463, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1AF4E4, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1B3565, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1B75E6, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1BB667, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1BF6E8, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1C3769, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1C77EA, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1CB86B, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1CF8EC, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1D396D, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1D79EE, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1DBA6F, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1DFAF0, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1E3B71, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1E7BF2, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1EBC73, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1EFCF4, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1F3D75, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1F7DF6, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1FBE77, 1 },\
    { 0x02E00114, 0xFFFFFFFF, 0x1FFEF8, 1 },

    /* FTDLL SRAM1 FUNCTION */

    /* FTDLL SRAM2 FUNCTION */
#define GT_TIGER_12_REG_VAL_INFO_LIST_PART6 \
    { 0x02E00150, 0xFFFFFFFF, 0x0     , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x10282 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x14283 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x18303 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1C303 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x20384 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x24384 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x28404 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x2C405 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x30485 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x34506 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x38507 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x3C588 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x40609 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x4468A , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x4868B , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x4C70C , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x5070D , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x5478E , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x5880F , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x5C890 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x60911 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x64992 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x68993 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x6CA14 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x70A15 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x74A96 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x74B17 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x78B98 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x7CC19 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x80C9A , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x80D1B , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x84D9C , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x88D9D , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x8CE1E , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x90E9F , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x94F20 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x94FA1 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x99022 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x9D0A3 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xA1124 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xA51A5 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xA9226 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xAD2A7 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xB1328 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xB53A9 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xB942A , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xBD4AB , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xC152C , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xC55AD , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xC962E , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xCD6AF , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xD1730 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xD57B1 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xD9832 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xDD8B3 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xE1934 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xE59B5 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xE9A36 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xEDAB7 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xF1B38 , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xF5BB9 , 1 },

#define GT_TIGER_12_REG_VAL_INFO_LIST_PART7 \
    { 0x02E00154, 0xFFFFFFFF, 0xF9C3A , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0xFDCBB , 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x101D3C, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x105DBD, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x109E3E, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x10DEBF, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x111F40, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x115FC1, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x11A042, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x11E0C3, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x122144, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1261C5, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x12A246, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x12E2C7, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x132348, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1363C9, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x13A44A, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x13E4CB, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x14254C, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1465CD, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x14A64E, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x14E6CF, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x152750, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1567D1, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x15A852, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x15E8D3, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x162954, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1669D5, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x16AA56, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x16EAD7, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x172B58, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x176BD9, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x17AC5A, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x17ECDB, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x182D5C, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x186DDD, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x18AE5E, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x18EEDF, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x192F60, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x196FE1, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x19B062, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x19F0E3, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1A3164, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1A71E5, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1AB266, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1AF2E7, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1B3368, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1B73E9, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1BB46A, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1BF4EB, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1C356C, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1C75ED, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1CB66E, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1CF6EF, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1D3770, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1D77F1, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1DB872, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1DF8F3, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1E3974, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1E79F5, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1EBA76, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1EFAF7, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1F3B78, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1F7BF9, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1FBC7A, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x1FFCFB, 1 },\
    { 0x02E00154, 0xFFFFFFFF, 0x203D7C, 1 },\
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}

#define GT_TIGER_12_REG_VAL_INFO_LIST  \
{                                      \
   GT_TIGER_12_REG_VAL_INFO_LIST_PART1 \
   GT_TIGER_12_REG_VAL_INFO_LIST_PART2 \
   GT_TIGER_12_REG_VAL_INFO_LIST_PART3 \
   GT_TIGER_12_REG_VAL_INFO_LIST_PART4 \
   GT_TIGER_12_REG_VAL_INFO_LIST_PART5 \
   GT_TIGER_12_REG_VAL_INFO_LIST_PART6 \
   GT_TIGER_12_REG_VAL_INFO_LIST_PART7 \
}

#define GT_TIGER_52_REG_VAL_INFO_LIST_PART1  \
    {0x0       , 0x0       , 0x0       , 0}, \
    {0x1       , 0x0       , 0x0       , 0}, \
    {0xFFFFFFFF, 0x0       , 0x0       , 0}, \
    {0xFFFFFFFF, 0x0       , 0x0       , 0}, \
    {0x3800060 , 0xFFFFFFFF, 0x1A84B16A, 1}, \
    {0x0       , 0xFFFFFFFF, 0x1B      , 1}, \
    {0x1900100 , 0xFFFFFFFF, 0x77      , 1}, \
    {0x1900104 , 0xFFFFFFFF, 0x41      , 1}, \
    {0x1900108 , 0xFFFFFFFF, 0x2       , 1}, \
    {0x190010C , 0xFFFFFFFF, 0x700073A , 1}, \
    {0x1900110 , 0xFFFFFFFF, 0x1732130 , 1}, \
    {0x1900114 , 0xFFFFFFFF, 0x45      , 1}, \
    {0x1900118 , 0xFFFFFFFF, 0x3       , 1}, \
    {0x1900118 , 0xFFFFFFFF, 0x4       , 1}, \
    {0x2040208 , 0xFFFFFFFF, 0x0       , 1}, \
    {0x2040190 , 0x1       , 0x1       , 1}, \
    {0x68      , 0xFFF10000, 0xFFF10000, 1}, \
    {0xEEEEEEEE, 0x0       , 0x0       , 0}, \
    {0x3800054 , 0xFFFFFFFF, 0x2880CE2 , 1}, \
    {0x3800054 , 0xFFFFFFFF, 0x2890CE2 , 1}, \
    {0x3800054 , 0xFFFFFFFF, 0x28A0CE2 , 1}, \
    {0x3800054 , 0xFFFFFFFF, 0x28B0CE2 , 1}, \
    {0x3800054 , 0xFFFFFFFF, 0x89140   , 1}, \
    {0x3800054 , 0xFFFFFFFF, 0x99140   , 1}, \
    {0x3800054 , 0xFFFFFFFF, 0xA9140   , 1}, \
    {0x3800054 , 0xFFFFFFFF, 0xB9140   , 1}, \
    {0xEEEEEEEE, 0x0       , 0x0       , 0}, \
    {0x6C      , 0xFFFFFFFF, 0x1FA40861, 1}, \
    {0x70      , 0xFFFFFFFF, 0x1861    , 1}, \
    {0x1900180 , 0xFFFFFFFF, 0x400000  , 1}, \
    {0x1900184 , 0xFFFFFFFF, 0x10D80180, 1}, \
    {0x70      , 0x2       , 0x2       , 1}, \
    {0x1900000 , 0xFFFFFFFF, 0x29      , 1}, \
    {0x2E00000 , 0xFFF     , 0x333     , 1}, \
    {0x2E00020 , 0xFFF     , 0x333     , 1}, \
    {0x2E00030 , 0xFFF     , 0x333     , 1}, \
    {0x2E00010 , 0xF00     , 0x100     , 1}, \
    {0x2E00014 , 0xFFFF    , 0xF93B    , 1}, \
    {0x1800004 , 0xFFFFFFFF, 0x1000220A, 1}, \
    {0x01800184, 0x00000010, 0x00000010, 1}, \
    {0x01840000, 0x00000080, 0x00000080, 1}, \
    {0x00804000, 0x000003FF, 0x00000044, 1}, \
    {0x01004000, 0x000003FF, 0x00000044, 1}, \
    {0x03800014, 0x00000180, 0x00000180, 1}, \
    {0x02800198, 0x00002000, 0x00002000, 1}, \
    {0x02E0000C, 0x00000007, 0x00000003, 1}, \
    {0x02E0002C, 0x00000007, 0x00000003, 1}, \
                                             \
    /* FTDLL SDRAM FUNCTION 300MHz */        \
                                             \
    { 0x01900190, 0xFFFFFFFF, 0x0     , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x44183 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x48184 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x4C184 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x50204 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x54205 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x58205 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x5C206 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x60286 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x64286 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x68287 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x6C307 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x70307 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x74308 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x78388 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x7C388 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x80409 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x84409 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x8848A , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x8C50B , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x9058C , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x9460D , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x9468E , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x9868F , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x9C70F , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xA0790 , 1 },

#define GT_TIGER_52_REG_VAL_INFO_LIST_PART2  \
    { 0x01900194, 0xFFFFFFFF, 0xA4811 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xA8892 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xAC913 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xB0914 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xB4995 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xB8A16 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xB8A97 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xB8B18 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xBCB99 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xC0C1A , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xC4C9B , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xC8D1C , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xCCD9D , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xD0E1E , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xD4E9F , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xD8F20 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xDCFA1 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xE1022 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xE50A3 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xE9124 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xED1A5 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xF1226 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xF52A7 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xF9328 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0xFD3A9 , 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x10142A, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1054AB, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x10952C, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x10D5AD, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x11162E, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1156AF, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x119730, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x11D7B1, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x121832, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1258B3, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x129934, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x12D9B5, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x131A36, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x135AB7, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x139B38, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x13DBB9, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x141C3A, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x145CBB, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x149D3C, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x14DDBD, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x151E3E, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x155EBF, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x159F40, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x15DFC1, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x162042, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1660C3, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x16A144, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x16E1C5, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x172246, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1762C7, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x17A348, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x17E3C9, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x18244A, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1864CB, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x18A54C, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x18E5CD, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x19264E, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1966CF, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x19A750, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x19E7D1, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1A2852, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1A68D3, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1AA954, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1AE9D5, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1B2A56, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1B6AD7, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1BAB58, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1BEBD9, 1 },

#define GT_TIGER_52_REG_VAL_INFO_LIST_PART3  \
    { 0x01900194, 0xFFFFFFFF, 0x1C2C5A, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1C6CDB, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1CAD5C, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1CEDDD, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1D2E5E, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1D6EDF, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1DAF60, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1DEFE1, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1E3062, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1E70E3, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1EB164, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1EF1E5, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1F3266, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1F72E7, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FB368, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF3E9, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF46A, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF4EB, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF56C, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF5ED, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF66E, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF6EF, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF770, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF7F1, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF872, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF8F3, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF974, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FF9F5, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FFA76, 1 }, \
    { 0x01900194, 0xFFFFFFFF, 0x1FFAF7, 1 }, \
                                             \
    /* FTDLL SDRAM FUNCTION */               \
                                             \
    /* FTDLL SRAM1 FUNCTION */               \
                                             \
    { 0x02E00110, 0xFFFFFFFF, 0x0     , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x104   , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x4104  , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x8185  , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xC185  , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x10205 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x14206 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x18286 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1C286 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x20307 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x24387 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x28407 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x2C488 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x30508 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x34589 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x38609 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x3C68A , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x4070A , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x4478B , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x4880B , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x4C88C , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x5090D , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x5498E , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x58A0F , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x5CA90 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x60B11 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x64B92 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x68C13 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x6CC94 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x70D15 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x74D96 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x78E17 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x7CE98 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x80F19 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x84F9A , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x8901B , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x8D09C , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x9111D , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x9519E , 1 },

#define GT_TIGER_52_REG_VAL_INFO_LIST_PART4  \
    { 0x02E00114, 0xFFFFFFFF, 0x9921F , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x9D2A0 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xA1321 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xA53A2 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xA9423 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xAD4A4 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xB1525 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xB55A6 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xB9627 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xBD6A8 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xC1729 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xC57AA , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xC982B , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xCD8AC , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xD192D , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xD59AE , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xD9A2F , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xDDAB0 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xE1B31 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xE5BB2 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xE9C33 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xEDCB4 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xF1D35 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xF5DB6 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xF9E37 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0xFDEB8 , 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x101F39, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x105FBA, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x10A03B, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x10E0BC, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x11213D, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1161BE, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x11A23F, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x11E2C0, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x122341, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1263C2, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x12A443, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x12E4C4, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x132545, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1365C6, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x13A647, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x13E6C8, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x142749, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1467CA, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x14A84B, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x14E8CC, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x15294D, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1569CE, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x15AA4F, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x15EAD0, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x162B51, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x166BD2, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x16AC53, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x16ECD4, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x172D55, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x176DD6, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x17AE57, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x17EED8, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x182F59, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x186FDA, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x18B05B, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x18F0DC, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x19315D, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1971DE, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x19B25F, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x19F2E0, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1A3361, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1A73E2, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1AB463, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1AF4E4, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1B3565, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1B75E6, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1BB667, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1BF6E8, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1C3769, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1C77EA, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1CB86B, 1 },

#define GT_TIGER_52_REG_VAL_INFO_LIST_PART5  \
    { 0x02E00114, 0xFFFFFFFF, 0x1CF8EC, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1D396D, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1D79EE, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1DBA6F, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1DFAF0, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1E3B71, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1E7BF2, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1EBC73, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1EFCF4, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1F3D75, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1F7DF6, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1FBE77, 1 }, \
    { 0x02E00114, 0xFFFFFFFF, 0x1FFEF8, 1 }, \
                                             \
    /* FTDLL SRAM1 FUNCTION */               \
                                             \
    /* FTDLL SRAM2 FUNCTION */               \
    { 0x02E00150, 0xFFFFFFFF, 0x0     , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x10282 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x14283 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x18303 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1C303 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x20384 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x24384 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x28404 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x2C405 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x30485 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x34506 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x38507 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x3C588 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x40609 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x4468A , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x4868B , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x4C70C , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x5070D , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x5478E , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x5880F , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x5C890 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x60911 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x64992 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x68993 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x6CA14 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x70A15 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x74A96 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x74B17 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x78B98 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x7CC19 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x80C9A , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x80D1B , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x84D9C , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x88D9D , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x8CE1E , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x90E9F , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x94F20 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x94FA1 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x99022 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x9D0A3 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xA1124 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xA51A5 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xA9226 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xAD2A7 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xB1328 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xB53A9 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xB942A , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xBD4AB , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xC152C , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xC55AD , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xC962E , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xCD6AF , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xD1730 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xD57B1 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xD9832 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xDD8B3 , 1 },

#define GT_TIGER_52_REG_VAL_INFO_LIST_PART6  \
    { 0x02E00154, 0xFFFFFFFF, 0xE1934 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xE59B5 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xE9A36 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xEDAB7 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xF1B38 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xF5BB9 , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xF9C3A , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0xFDCBB , 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x101D3C, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x105DBD, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x109E3E, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x10DEBF, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x111F40, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x115FC1, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x11A042, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x11E0C3, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x122144, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1261C5, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x12A246, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x12E2C7, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x132348, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1363C9, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x13A44A, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x13E4CB, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x14254C, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1465CD, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x14A64E, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x14E6CF, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x152750, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1567D1, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x15A852, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x15E8D3, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x162954, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1669D5, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x16AA56, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x16EAD7, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x172B58, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x176BD9, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x17AC5A, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x17ECDB, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x182D5C, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x186DDD, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x18AE5E, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x18EEDF, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x192F60, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x196FE1, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x19B062, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x19F0E3, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1A3164, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1A71E5, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1AB266, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1AF2E7, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1B3368, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1B73E9, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1BB46A, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1BF4EB, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1C356C, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1C75ED, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1CB66E, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1CF6EF, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1D3770, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1D77F1, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1DB872, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1DF8F3, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1E3974, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1E79F5, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1EBA76, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1EFAF7, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1F3B78, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1F7BF9, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1FBC7A, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x1FFCFB, 1 }, \
    { 0x02E00154, 0xFFFFFFFF, 0x203D7C, 1 }, \
    {0xFFFFFFFF, 0x0       , 0x0       , 0},


#define GT_TIGER_52_REG_VAL_INFO_LIST   \
{                                       \
    GT_TIGER_52_REG_VAL_INFO_LIST_PART1 \
    GT_TIGER_52_REG_VAL_INFO_LIST_PART2 \
    GT_TIGER_52_REG_VAL_INFO_LIST_PART3 \
    GT_TIGER_52_REG_VAL_INFO_LIST_PART4 \
    GT_TIGER_52_REG_VAL_INFO_LIST_PART5 \
    GT_TIGER_52_REG_VAL_INFO_LIST_PART6 \
}

/****************************************************************************/
/* CPSS_PP_PHASE2_INIT_PARAMS struct default                                  */
/****************************************************************************/
#define RX_DESC_NUM_DEF         200
#define TX_DESC_NUM_DEF         1000
#define AU_DESC_NUM_DEF         2048
#define RX_BUFF_SIZE_DEF        1548
#define RX_BUFF_ALLIGN_DEF      1
#define APP_DEMO_RX_BUFF_ALLIGN_BITS_DEF  7
#define APP_DEMO_RX_BUFF_ALLIGN_DEF      (1 << APP_DEMO_RX_BUFF_ALLIGN_BITS_DEF)
#define APP_DEMO_RX_BUFF_SIZE_DEF        800

#define CPSS_PP_PHASE2_DEFAULT                                  \
{                                                               \
    0,                              /* devNum           */      \
    0,                              /* baseAddr         */      \
    CPSS_98MX610B_CNS,              /* deviceId         */      \
    /* struct netIfCfg  */                                      \
    {                                                           \
        NULL,                       /* *txDescBlock     */      \
        4096,                       /* txDescBlockSize  */      \
        NULL,                       /* *rxDescBlock     */      \
        4096,                       /* rxDescBlockSize  */      \
        /* rxBufInfo        */                                  \
        {                                                       \
            CPSS_RX_BUFF_STATIC_ALLOC_E, /* allocMethod */      \
            { 13, 13, 13, 13,           /* bufferPercentage */  \
              12, 12, 12, 12 },                                 \
            RX_BUFF_SIZE_DEF,           /* rxBufSize    */      \
            0,                          /* headerOffset */      \
            GT_FALSE,                   /* buffersInCachedMem */\
            /* union buffData   */                              \
            {                                                   \
                /* struct staticAlloc   */                      \
                {                                               \
                    NULL,           /* *rxBufBlock      */      \
                    16000           /* rxBufBlockSize   */      \
                }                                               \
            }                                                   \
        }                                                       \
    },                                                          \
    /* struct auqCfg        */                                  \
    {                                                           \
        NULL,                       /* *auDescBlock     */      \
        4096                        /* auDescBlockSize  */      \
    },                                                          \
    0,                              /* hwDevNum         */      \
    /* struct fuqCfg        */                                  \
    {                                                           \
        NULL,                       /* *fuDescBlock     */      \
        0                           /* fuDescBlockSize  */      \
    },                                                          \
    GT_FALSE,                       /* fuqUseSeparate  */       \
    RX_BUFF_SIZE_DEF,               /* rxBufSize       */       \
    0,                              /* headerOffset    */       \
    0,                              /* netifSdmaPortGroupId */  \
    CPSS_AU_MESSAGE_LENGTH_4_WORDS_E,/* auMessageLength */       \
    GT_FALSE,                       /* useSecondaryAuq */       \
    GT_FALSE                        /* useDoubleAuq */          \
}


#define GT_PCL_ACTION_SIZE_16_BYTE 16

/* 7 trunks supported by : EXMXTC , EXMXSAM  */
#define APP_DEMO_7_TRUNKS_CNS      7

/* 31 trunks supported by : DXSAL , EXMXTD , EXMXTG */
#define APP_DEMO_31_TRUNKS_CNS      31

/* 127 trunks supported by : DXCH , DXCH2, DXCH3 */
#define APP_DEMO_127_TRUNKS_CNS     127

/* 127 trunks supported by : EXMXPM */
#define APP_DEMO_255_TRUNKS_CNS     255

#define PP_LOGICAL_CONFIG_FDB_DEFAULT                                         \
{                                                                             \
    0,/*fdbTableMode = INTERNAL_SPLIT */                                      \
    0,/*fdbToLutRatio -- don't care for internal memory */                    \
    0,/*fdbHashSize    -- don't care for internal memory */                   \
    0 /*extFdbHashMode -- don't care for internal memory */                   \
}

#define PP_LOGICAL_CONFIG_EXTERNAL_TCAM_DEFAULT                               \
{                                                                             \
    0,/* actionLinesInMemoryControl0 */                                       \
    0,/* actionLinesInMemoryControl1 */                                       \
    NULL,/*extTcamRuleWriteFuncPtr   */                                       \
    NULL,/*extTcamRuleReadFuncPtr    */                                       \
    NULL,/*extTcamActionWriteFuncPtr */                                       \
    NULL /*extTcamActionReadFuncPtr  */                                       \
}

#define APP_DEMO_CPSS_LPM_RAM_MEM_BLOCKS_DEFAULT                               \
{                                                                              \
    0,                                                                             /* numOfBlocks*/ \
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},                                     /* blocksSizeArray*/ \
    APP_DEMO_CPSS_LPM_RAM_BLOCKS_ALLOCATION_METHOD_DYNAMIC_WITHOUT_BLOCK_SHARING_E /* blocksAllocationMethod */  \
}
    
/****************************************************************************/
/* GT_PP_CONFIG struct default                                              */
/****************************************************************************/
#define PP_LOGICAL_CONFIG_DEFAULT                                            \
{                                                                    \
    500,                            /* maxNumOfLifs              */  \
    APP_DEMO_CPSS_INLIF_PORT_MODE_PORT_E, /* inlifPortMode       */  \
    50,                             /* maxNumOfPolicerEntries    */  \
    GT_TRUE,                        /* policerConformCountEn     */  \
    1000,                           /* maxNumOfNhlfe             */  \
    100,                            /* maxNumOfMplsIfs           */  \
    APP_DEMO_CPSS_MEMORY_LOCATION_EXTERNAL_E,/*ipMplsMemLocation */  \
    1024,                           /* maxNumOfPclAction         */  \
    GT_PCL_ACTION_SIZE_16_BYTE,     /* pclActionSize             */  \
    0,                              /* maxNumOfPceForIpPrefixes  */  \
    GT_FALSE,                       /* usePolicyBasedRouting     */  \
    GT_FALSE,                       /* usePolicyBasedDefaultMc   */  \
    0,                              /* maxNumOfPbrEntries        */  \
    4095,                           /* maxVid                    */  \
    1024,                           /* maxMcGroups               */  \
    5,                              /* maxNumOfVirtualRouters    */  \
    100,                            /* maxNumOfIpNextHop         */  \
    1000,                           /* maxNumOfIpv4Prefixes      */  \
    100,                            /* maxNumOfIpv4McEntries     */  \
    500,                            /* maxNumOfMll               */  \
    100,                            /* maxNumOfIpv6Prefixes      */  \
    50,                             /* maxNumOfIpv6McGroups      */  \
    500,                            /* maxNumOfTunnelEntries     */  \
    8,                              /* maxNumOfIpv4TunnelTerms   */  \
    100,                            /* maxNumOfTunnelTerm        */  \
    0/*GT_VLAN_INLIF_ENTRY_TYPE_REGULAR_E*/, /* vlanInlifEntryType  */  \
    GT_FALSE,                       /* ipMemDivisionOn           */  \
    APP_DEMO_CPSS_DXCH_TCAM_ROUTER_BASED_E,  /* routingMode      */  \
    0,                              /* ipv6MemShare              */  \
    APP_DEMO_31_TRUNKS_CNS,         /* numOfTrunks               */  \
    0,                              /* defIpv6McPclRuleIndex     */  \
    0,                              /* vrIpv6McPclId             */  \
    GT_TRUE,                        /* lpmDbPartitionEnable      */  \
    100,                            /* lpmDbFirstTcamLine        */  \
    1003,                           /* lpmDbLastTcamLine         */  \
    GT_TRUE,                        /* lpmDbSupportIpv4          */  \
    GT_TRUE,                        /* lpmDbSupportIpv6          */  \
    0,                              /* lpmDbNumOfMemCfg          */  \
    NULL,                           /* lpmDbMemCfgArray          */  \
    CPSS_DRAM_16MB_E,               /* extLpmMemSize             */  \
    APP_DEMO_CPSS_LPM_STAGES_MEM_ASSOCIATION_ALL_INTERNAL_E, /*lpmStagesMemAssociation*/\
    APP_DEMO_CPSS_LPM_RAM_MEM_BLOCKS_DEFAULT,/*lpmRamMemoryBlocksCfg*/\
    APP_DEMO_CPSS_MEMORY_LOCATION_EXTERNAL_E,/* ditMemLocation   */  \
    APP_DEMO_CPSS_DIT_SHARED_MEM_E, /* ditMemoryMode             */  \
    APP_DEMO_CPSS_DIT_IP_MC_DUAL_MODE_E, /* ditIpMcMode          */  \
    50,                             /* maxNumOfDitUcIpEntries    */  \
    50,                             /* maxNumOfDitUcMplsEntries  */  \
    50,                             /* maxNumOfDitUcOtherEntries */  \
    50,                             /* maxNumOfDitMcIpEntries    */  \
    50,                             /* maxNumOfDitMcMplsEntries  */  \
    50,                             /* maxNumOfDitMcOtherEntries */  \
    PP_LOGICAL_CONFIG_FDB_DEFAULT,  /* fdbTableInfo              */  \
    PP_LOGICAL_CONFIG_EXTERNAL_TCAM_DEFAULT,/* externalTcamInfo  */  \
    0,                              /* miiNumOfTxDesc */             \
    0,                              /* miiTxInternalBufBlockSize */  \
    {13, 13, 13, 13, 12, 12, 12, 12},/*miiBufferPercentage   */      \
    0,                              /*miiRxBufSize    */             \
    0,                              /*miiHeaderOffset */             \
    0                               /*miiRxBufBlockSize  */          \
}

/****************************************************************************/
/* APP_DEMO_LIB_INIT_PARAMS struct default                                  */
/****************************************************************************/
#define LIB_INIT_PARAMS_DEFAULT                                 \
{                                                               \
    GT_TRUE,                        /* initBridge       */      \
    GT_TRUE,                        /* initClassifier   */      \
    GT_TRUE,                        /* initCos          */      \
    GT_TRUE,                        /* initIpv4         */      \
    GT_FALSE,                       /* initIpv6         */      \
    GT_TRUE,                        /* initIpv4Filter   */      \
    GT_TRUE,                        /* initIpv4Tunnel   */      \
    GT_TRUE,                        /* initLif          */      \
    GT_TRUE,                        /* initMirror       */      \
    GT_TRUE,                        /* initMpls         */      \
    GT_TRUE,                        /* initMplsTunnel   */      \
    GT_TRUE,                        /* initNetworkIf    */      \
    GT_FALSE,                       /* initI2c          */      \
    GT_TRUE,                        /* initPhy          */      \
    GT_TRUE,                        /* initPort         */      \
    GT_TRUE,                        /* initTc           */      \
    GT_TRUE,                        /* initTrunk        */      \
    GT_TRUE,                        /* initPcl          */      \
    GT_FALSE,                       /* initTcam         */      \
    GT_TRUE,                        /* initPolicer      */      \
    GT_TRUE,                        /* Init action      */      \
    GT_FALSE                  /* sFlow, only for D1 and D-XG*/  \
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtAppDemoSysConfigDefaultsh */

