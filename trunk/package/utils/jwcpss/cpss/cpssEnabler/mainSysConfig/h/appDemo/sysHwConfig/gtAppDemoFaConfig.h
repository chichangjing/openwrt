/*******************************************************************************
*              Copyright 2002, GALILEO TECHNOLOGY, LTD.
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
* gtAppDemoFaConfig.h
*
* DESCRIPTION:
*       Application demo for the Fabric Adapter initialization and
*       configuration.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.3 $
*
*******************************************************************************/
#ifndef __gtAppDemoFaConfigh
#define __gtAppDemoFaConfigh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <cpssFa/generic/cpssFaHwInit/gtCoreFaHwInit.h>
#include <appDemo/os/appOs.h>


/****************************************************************/
/* Fa Phase1 initialization parameters default values.        */
/****************************************************************/
#define FA_PHASE1_DEFAULT                                       \
{                                                               \
    0,                              /* devNum           */      \
    133,                            /* coreClk          */      \
    /* bufferDramCfg, dram parameters of
    SAMSUNG E-Ver (K4D263238E-GC33) at 300MHz*/                 \
    {                                                           \
        0,                          /* flowDramSize     */      \
        GT_DRAM_FREQ_300_MHZ,       /* dramFrequency    */      \
        GT_TRUE,                    /* overwriteDef     */      \
        0x41,                       /* mrs              */      \
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
        0,                          /* operation        */      \
        GT_DRAM_PAGE_SIZE_512B      /* dramPageSize     */      \
    },                                                          \
    {                                                           \
        GT_INT_2_PP_GPP0,             /* interrupt type */      \
        0,                            /* intVecNum      */      \
        0                             /* intMask        */      \
    },                                                          \
    CPSS_FA_BUFFER_DRAM_USE_DEFAULT_DRAM_CONF_E, /* bufferDram ??  is it correct default? [SL...]*/ \
    CPSS_FA_SYS_HA_MODE_ACTIVE_E,      /*  devHAState    */     \
    CPSS_EXTDRV_MGMT_VIA_PP_E,         /* management interface - default */\
    0xB0                                /* base address         */\
}

/****************************************************************************/
/* GT_REG_VALUE_INFO list used by coreFaHwStartInit()                       */
/****************************************************************************/

#define GT_FOX_REG_VAL_INFO_LIST                                            \
{                                                                           \
    {0x00000000, 0x00000000, 0x00000000, 0},                                \
    {0x00000001, 0x00000000, 0x00000000, 0},                                \
    {0x00000002, 0x00000000, 0x00000000, 0},                                \
    {0x00000003, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0x42800190, 0xFFFFFFFF, 0x00000000, 1},    /* bdAddressReg     */      \
    {0x42800194, 0xFFFFFFFF, 0x000060C2, 64},   /* bdDataReg        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}     /* Delimiter        */      \
}

/* In XG board, the board frequency was changed so FA needs a different
   configuration. Note that the device type is just an indication! */
#define GT_FOX_REG_XG_VAL_INFO_LIST                                         \
{                                                                           \
    {0x00000000, 0x00000000, 0x00000000, 0},                                \
    {0x00000001, 0x00000000, 0x00000000, 0},                                \
    {0x00000002, 0x00000000, 0x00000000, 0},                                \
    {0x00000003, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0x42800190, 0xFFFFFFFF, 0x00000000, 1},    /* bdAddressReg     */      \
    {0x42800194, 0xFFFFFFFF, 0x00000000, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00002041, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00005041, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00007082, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0000A0C2, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0000C0C3, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0000E103, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00011144, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00013144, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00016185, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000181C5, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0001A1C6, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0001D206, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0001F247, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00022247, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00024288, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000262C9, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000272C9, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002730A, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002734A, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002734B, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002738B, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000273CC, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000273CC, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002740D, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002744D, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002744E, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002748E, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000274CF, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000274CF, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00027510, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00027551, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00027551, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00027592, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000275D2, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000275D3, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00027613, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00027654, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00027654, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00027695, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000276D5, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000276D6, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00027716, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00027757, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00027757, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x00027798, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000277D9, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000277D9, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002781A, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002785A, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002785B, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002789B, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000278DC, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000278DC, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002791D, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002795D, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002795E, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x0002799E, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000279DF, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000279DF, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000279E0, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000279E1, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000279E1, 1},    /* bdDataReg        */      \
    {0x42800194, 0xFFFFFFFF, 0x000279E2, 1},    /* bdDataReg        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}     /* Delimiter        */      \
}

/* In Tiger XG board, the board frequency was changed so FA needs a different
   configuration. Note that the device type is just an indication! */
 
#define GT_FOX_REG_TIGER_XG_VAL_INFO_LIST                                   \
{                                                                           \
    /* section 1 - revision 0,1 */                                          \
    { 0x00000000, 0x00000000, 0x00000000, 0 },                              \
    { 0x00000001, 0x00000000, 0x00000000, 0 },                              \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                              \
    /* after phase 1 before start init */                                   \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                              \
    /* after start init before phase 2 */                                   \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                              \
    /* section 2 - revisions 2,3,4,5 */                                     \
    { 0x00000002, 0x00000000, 0x00000000, 0 },                              \
    { 0x00000003, 0x00000000, 0x00000000, 0 },                              \
    { 0x00000004, 0x00000000, 0x00000000, 0 },                              \
    { 0x00000005, 0x00000000, 0x00000000, 0 },                              \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                              \
    /* pre start init */                                                    \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 },                              \
    /* post start init */                                                   \
    /* FA SDRAM parameters (G-ver) */                                       \
    { 0x4280010C, 0xFFFFFFFF, 0x070008C0, 1 },                              \
    { 0x42800100, 0xFFFFFFFF, 0x77,       1 },                              \
    { 0x42800104, 0xFFFFFFFF, 0x40,       1 },                              \
    { 0x42800108, 0xFFFFFFFF, 0x2,        1 },                              \
    { 0X42800110, 0XFFFFFFFF, 0x1714130,  1 },                              \
    { 0X42800114, 0XFFFFFFFF, 0x45,       1 },                              \
    { 0X42800118, 0XFFFFFFFF, 0x3,        1 },                              \
    { 0X42800118, 0XFFFFFFFF, 0x4,        1 },                              \
    /* DFSDL Adjust */                                                      \
    { 0x42800180, 0xFFFFFFFF, 0x00400000, 1 },                              \
    { 0x42800184, 0xFFFFFFFF, 0x10D80180, 1 },                              \
    /* DFCDL for FOX SDRAM */                                               \
    { 0x42800190, 0xFFFFFFFF, 0x0    ,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x0    ,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x1041 ,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x3041 ,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x4082 ,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x60C2 ,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x70C3 ,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x9104 ,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0xA144 ,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0xB145 ,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0xD185 ,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0xE186 ,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x101C7,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x11207,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x13208,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x14249,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x15289,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x1728A,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x182CA,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x1A30B,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x1B30C,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x1D34C,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x1E38D,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x1F38D,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x213CE,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x223CF,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x2440F,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x25450,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27450,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27491,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x274D2,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x274D2,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27513,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27553,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27554,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27595,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x275D5,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x275D6,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27616,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27617,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27658,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27698,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27699,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x276DA,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x2771A,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x2771B,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x2775B,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x2779C,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x2779D,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x277DD,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x2781E,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x2781E,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x2785F,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27860,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x278A0,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x278E1,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x278E1,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27922,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27963,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x27963,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x279A4,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x279E4,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x279E5,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x279E6,    1 },                              \
    { 0x42800194, 0xFFFFFFFF, 0x279E6,    1 },                              \
    /* General Config */                                                    \
    { 0x42080050, 0xFFFFFFFF, 0xFFFFFFFF, 1 },                              \
    { 0x42088050, 0xFFFFFFFF, 0xFFFFFFFF, 1 },                              \
    { 0x42090050, 0xFFFFFFFF, 0xFFFFFFFF, 1 },                              \
    { 0x42180050, 0xFFFFFFFF, 0xFFFFFFFF, 1 },                              \
    { 0x4208003C, 0x00003FF0, 0x1290    , 1 },                              \
    { 0x4208803C, 0x00003FF0, 0x1290    , 1 },                              \
    { 0x4209003C, 0x00003FF0, 0x1290    , 1 },                              \
    { 0x4218003C, 0x00003FF0, 0x1290    , 1 },                              \
    { 0x4200005C, 0x0000FFFF, 0x423C    , 1 },                              \
    { 0x4200805C, 0x0000FFFF, 0x423C    , 1 },                              \
    { 0x4201005C, 0x0000FFFF, 0x423C    , 1 },                              \
    { 0x4210005C, 0x0000FFFF, 0x423C    , 1 },                              \
    { 0x4210805C, 0x0000FFFF, 0x423C    , 1 },                              \
    { 0x420C0000, 0xFFFFFFFF, 0x3F973E0 , 1 },                              \
    { 0x420C0004, 0xFFFFFFFF, 0x3F973E0 , 1 },                              \
    { 0x420C0008, 0xFFFFFFFF, 0x3F973E0 , 1 },                              \
    { 0x420C000C, 0xFFFFFFFF, 0x3F973E0 , 1 },                              \
    { 0x420C0010, 0xFFFFFFFF, 0x3F973E0 , 1 },                              \
    { 0x420C0014, 0xFFFFFFFF, 0x3F973E0 , 1 },                              \
    { 0x420C2200, 0xFFFFFFFF, 0x3F973E0 , 1 },                              \
    { 0x420C2204, 0xFFFFFFFF, 0x3F973E0 , 1 },                              \
    { 0x420C2208, 0xFFFFFFFF, 0x3F973E0 , 1 },                              \
    { 0x420C220C, 0xFFFFFFFF, 0x3F973E0 , 1 },                              \
    { 0x420C2210, 0xFFFFFFFF, 0x3F973E0 , 1 },                              \
    { 0x420C2214, 0xFFFFFFFF, 0x3F973E0 , 1 },                              \
    { 0xFFFFFFFF, 0x00000000, 0x00000000, 0 }                               \
}

#define GT_FOX_REG_SAMBA_VAL_INFO_LIST                                         \
{                                                                              \
    {0x00000000, 0x00000000, 0x00000000, 0},     /* section 1 - revision 0,1 */\
    {0x00000001, 0x00000000, 0x00000000, 0},                                   \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},     /* Delimiter */               \
    /* after phase 1 before start init */                                      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},     /* Delimiter */               \
    /* after start init before phase 2 */                                      \
    {0x42800194, 0xFFFFFFFF, 0x279E0,    1},                                   \
    {0x420C0000, 0xFFFFFFFF, 0x3E1C7E0,  1},     /* Serdes Config */           \
    {0x420C0004, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C0008, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C000C, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C0010, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C0014, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C2200, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C2204, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C2208, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C220C, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C2210, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C2214, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C2218, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C221C, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C2220, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C2224, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C2228, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0x420C222C, 0xFFFFFFFF, 0x3E1C7E0,  1},                                   \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},     /* Delimiter */               \
    /* section 2 - revisions 2,3,4,5 */                                        \
    {0x00000002, 0x00000000, 0x00000000, 0},                                   \
    {0x00000003, 0x00000000, 0x00000000, 0},                                   \
    {0x00000004, 0x00000000, 0x00000000, 0},                                   \
    {0x00000005, 0x00000000, 0x00000000, 0},                                   \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter */                \
    /* pre start init */                                                       \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter */                \
    /* post start init */                                                      \
    /* 0x42800190 0xFFFFFFFF 0x00000000 1   # SDRAM address DFCDL */           \
    /* 0x42800194 0xFFFFFFFF 0x0000E207 64  # SDRAM DFCDL Value */             \
    /* DFCDL for FOX SDRAM */                                                  \
    {0x42800190, 0xFFFFFFFF, 0x0    , 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x0    , 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x2041 , 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x3041 , 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x5082 , 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x70C2 , 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x8103 , 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0xA103 , 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0xC144 , 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0xD184 , 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0xF185 , 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x111C6, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x12206, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x14207, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x15247, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x17288, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x192C8, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x1A2C9, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x1C309, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x1E34A, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x1F34A, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x2138B, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x233CC, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x243CC, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x2640D, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x2744D, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x2748E, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x2748E, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x274CF, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x2750F, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x27510, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x27551, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x27591, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x27592, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x275D2, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x27613, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x27653, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x27654, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x27694, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x276D5, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x276D5, 1},                                      \
    {0x42800194, 0xFFFFFFFF, 0x27716, 1},                                      \
/* */                                                                          \
    {0x420C0000, 0xFFFFFFFF, 0x3E1C7E0, 1},       /* Serdes Config */          \
    {0x420C0004, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C0008, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C000C, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C0010, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C0014, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C2200, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C2204, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C2208, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C220C, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C2210, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C2214, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C2218, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C221C, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C2220, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C2224, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C2228, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0x420C222C, 0xFFFFFFFF, 0x3E1C7E0, 1},                                    \
    {0xFFFFFFFF, 0x00000000, 0x0000000, 0}             /* Delimiter */         \
}

#define GT_FOX_REG_SAMBA_XG_VAL_INFO_LIST                                      \
{                                                                              \
    {0x00000000, 0x00000000, 0x00000000, 0},/*section 1 - revision 0,1*/       \
    {0x00000001, 0x00000000, 0x00000000, 0},                                   \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},/*Delimiter*/                      \
                                            /*after phase 1 before start init*/\
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},/* Delimiter */                    \
                                            /*after start init before phase 2*/\
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},/*Delimiter*/                      \
                                            /*section 2 - revisions 2,3,4,5*/  \
    {0x00000002, 0x00000000, 0x00000000, 0},                                   \
    {0x00000003, 0x00000000, 0x00000000, 0},                                   \
    {0x00000004, 0x00000000, 0x00000000, 0},                                   \
    {0x00000005, 0x00000000, 0x00000000, 0},                                   \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},/*Delimiter*/                      \
                                            /*pre start init*/                 \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},/*Delimiter*/                      \
                                            /*post start init*/                \
    {0x42800100, 0xFFFFFFFF, 0x77      , 1},/*Read Count =7, Write Count =7*/  \
    {0x4280010C, 0xF7000000, 0x06000000, 1},/*Address decode =6 , TrcdWr = 0*/ \
    {0x42800114, 0xFFFFFFFF, 0x45      , 1},/* trfc =5, rd2wrdelay = 1*/       \
    /* -- Dram DFCDL */                                                        \
    /* value = ( (dqsDel)|(clkOutDel<<6)|(rxClkDel<<12)|(sysClkDel<<18)); */   \
    /*  Entry         DQS     TxClk   RxClk */                                 \
    {0x42800190, 0xFFFFFFFF, 0x00000000, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00000000, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00001040, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00003041, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00004081, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000060C2, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00007102, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00009103, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0000A143, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0000B183, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0000D184, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0000E1C4, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00010205, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00011245, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00013246, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00014286, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000152C6, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000172C7, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00018307, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0001A348, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0001B388, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0001D389, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0001E3C9, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0001F409, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0002140A, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0002244A, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0002448B, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000254CB, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000274CC, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0002750C, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0002754C, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0002754D, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0002758D, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000275CE, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0002760E, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0002760F, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0002764F, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x0002768F, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00027690, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000276D0, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00027711, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00027751, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00027752, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00027792, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000277D2, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000277D3, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00027813, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00027854, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00027894, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00027895, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000278D5, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00027915, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00027916, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00027956, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x00027997, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000279D7, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000279D8, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000279D8, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000279D8, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000279D9, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000279D9, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000279DA, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000279DA, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000279DB, 1},                                   \
    {0x42800194, 0xFFFFFFFF, 0x000279DB, 1},                                   \
    {0x42080050, 0xFFFFFFFF, 0xFFFFFFFF, 1},/*level flow control*/             \
    {0x42088050, 0xFFFFFFFF, 0xFFFFFFFF, 1},/*level flow control*/             \
    {0x42090050, 0xFFFFFFFF, 0xFFFFFFFF, 1},/*level flow control*/             \
    {0x42180050, 0xFFFFFFFF, 0xFFFFFFFF, 1},/*level flow control*/             \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0} /*Delimiter*/                      \
}                                                                              


                                                                               
#define FA_98FX902A_24G_REG_VAL_INFO_LIST                                   \
{                                                                           \
    {   /********** first device *************/                             \
        {0x00000000, 0x00000000, 0x00000000, 0},    /* A1               */  \
        {0x00000001, 0x00000000, 0x00000000, 0},    /*                  */  \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */  \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter - Init */  \
        {0x40000058, 0x00000001, 0x00000001, 1},    /*                  */  \
        {0x40000058, 0x00000002, 0x00000002, 1},    /*                  */  \
        {0x40800010, 0x00000FFF, 0x00000FFF, 1},    /*                  */  \
        {0x40800020, 0x00000FFF, 0x00000FFF, 1},    /*                  */  \
        {0x40800060, 0x000F0000, 0x000F0000, 1},    /*                  */  \
        {0x42800190, 0xFFFFFFFF, 0x00000000, 1},    /*                  */  \
        {0x42800194, 0xFFFFFFFF, 0x000090C3, 20},   /*                  */  \
        {0x42800110, 0x000F0000, 0x00040000, 1},    /*                  */  \
        {0x42800100, 0x000000FF, 0x00000077, 1},    /*                  */  \
        {0x4280010C, 0xF0000000, 0x30000000, 1},    /*                  */  \
        {0x42800110, 0x000F0000, 0x00040000, 1},    /*                  */  \
        {0x4280010C, 0x07000000, 0x06000000, 1},    /*                  */  \
        {0x41000010, 0x0000000E, 0x0000000C, 1},    /*                  */  \
        {0x41800000, 0x01000000, 0x00000000, 1},    /*                  */  \
        {0x40806004, 0xFFFFFFFF, 0x00000001, 1},    /*                  */  \
        {0x40806000, 0xFFFFFFFF, 0x00000000, 1},    /*                  */  \
        {0x40800010, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800014, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800018, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x4080001C, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800020, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800024, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x42000000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42008000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42010000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42100000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42108000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42800110, 0x000F0000, 0x00040000, 1},    /*                  */  \
        {0x4208003C, 0x00000001, 0x00000001, 1},    /*                  */  \
        {0x4208003C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x4208803C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x4209003C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x4218003C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x40800000, 0x08000000, 0x08000000, 1},    /*                  */  \
        {0x42140000, 0xFFFFFFFF, 0x00000020, 1},    /*                  */  \
        {0x42040000, 0xFFFFFFFF, 0x00000200, 1},    /*                  */  \
        {0x42004000, 0xFFFFFFFF, 0x00000129, 1},    /*                  */  \
        {0x4210C000, 0xFFFFFFFF, 0x00000021, 1},    /*                  */  \
        {0x40000078, 0x00000100, 0x00000100, 1},    /*                  */  \
        {0x40000078, 0x00000008, 0x00000000, 1},    /*                  */  \
        {0x40000078, 0x00000100, 0x00000000, 1},    /*                  */  \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */  \
        {0x00000002, 0x00000000, 0x00000000, 0},    /* A3               */  \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */  \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */  \
        {0x40000058, 0x00000001, 0x00000001, 1},    /*                  */  \
        {0x40000058, 0x00000002, 0x00000002, 1},    /*                  */  \
        {0x43000010, 0xffffffff, 0x00000000, 1},    /*                  */  \
        {0x43000014, 0xffffffff, 0x000004d6, 22},   /*                  */  \
        {0x42800190, 0xFFFFFFFF, 0x00000000, 1},    /*                  */  \
        {0x42800194, 0xFFFFFFFF, 0x0000F203, 25},   /*                  */  \
        {0x40800010, 0x00000FFF, 0x00000FFF, 1},    /*                  */  \
        {0x40800020, 0x00000FFF, 0x00000FFF, 1},    /*                  */  \
        {0x40800060, 0x000F0000, 0x000F0000, 1},    /*                  */  \
        {0x42800110, 0xFFFFFFFF, 0x01732130, 1},    /*                  */  \
        {0x42800100, 0x000000FF, 0x00000077, 1},    /*                  */  \
        {0x4280010C, 0xFFFFFFFF, 0x17000850, 1},    /*                  */  \
        {0x42800108, 0x00000002, 0x00000002, 1},    /*                  */  \
        {0x42800114, 0x0000000F, 0x00000005, 1},    /*                  */  \
        {0x42800104, 0x00000041, 0x00000041, 1},    /*                  */  \
        {0x42800118, 0xFFFFFFFF, 0x00000003, 1},    /*                  */  \
        {0x42800118, 0xFFFFFFFF, 0x00000004, 1},    /*                  */  \
        {0x4280010C, 0x07000000, 0x06000000, 1},    /*                  */  \
        {0x41000010, 0x0000000E, 0x0000000C, 1},    /*                  */  \
        {0x41800000, 0x01000000, 0x00000000, 1},    /*                  */  \
        {0x40806004, 0xFFFFFFFF, 0x00000001, 1},    /*                  */  \
        {0x40806000, 0xFFFFFFFF, 0x00000004, 1},    /*                  */  \
        {0x40800010, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800014, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800018, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x4080001C, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800020, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x42000000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42008000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42010000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42100000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42108000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42800110, 0x000F0000, 0x00040000, 1},    /*                  */  \
        {0x4208003C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x4208803C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x4209003C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x4218003C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x40800000, 0x08000000, 0x08000000, 1},    /*                  */  \
        {0x40000038, 0xf0000000, 0x10000000, 1},    /*                  */  \
        {0x40000078, 0x00000100, 0x00000100, 1},    /*                  */  \
        {0x40000078, 0x00000008, 0x00000000, 1},    /*                  */  \
        {0x40000078, 0x00000100, 0x00000000, 1},    /*                  */  \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}     /* Delimiter        */  \
    },                                                                      \
    {   /********** second device ************/                             \
        {0x00000000, 0x00000000, 0x00000000, 0},    /* A1               */  \
        {0x00000001, 0x00000000, 0x00000000, 0},    /*                  */  \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */  \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter - Init */  \
        {0x40000058, 0x00000001, 0x00000001, 1},    /*                  */  \
        {0x40000058, 0x00000002, 0x00000002, 1},    /*                  */  \
        {0x40800010, 0x00000FFF, 0x00000FFF, 1},    /*                  */  \
        {0x40800020, 0x00000FFF, 0x00000FFF, 1},    /*                  */  \
        {0x40800060, 0x000F0000, 0x000F0000, 1},    /*                  */  \
        {0x42800190, 0xFFFFFFFF, 0x00000000, 1},    /*                  */  \
        {0x42800194, 0xFFFFFFFF, 0x000090C3, 20},   /*                  */  \
        {0x42800110, 0x000F0000, 0x00040000, 1},    /*                  */  \
        {0x42800100, 0x000000FF, 0x00000077, 1},    /*                  */  \
        {0x4280010C, 0xF0000000, 0x30000000, 1},    /*                  */  \
        {0x42800110, 0x000F0000, 0x00040000, 1},    /*                  */  \
        {0x4280010C, 0x07000000, 0x06000000, 1},    /*                  */  \
        {0x41000010, 0x0000000E, 0x0000000C, 1},    /*                  */  \
        {0x41800000, 0x01000000, 0x00000000, 1},    /*                  */  \
        {0x40806004, 0xFFFFFFFF, 0x00000001, 1},    /*                  */  \
        {0x40806000, 0xFFFFFFFF, 0x00000000, 1},    /*                  */  \
        {0x40800010, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800014, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800018, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x4080001C, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800020, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800024, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x42000000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42008000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42010000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42100000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42108000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42800110, 0x000F0000, 0x00040000, 1},    /*                  */  \
        {0x4208003C, 0x00000001, 0x00000001, 1},    /*                  */  \
        {0x4208003C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x4208803C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x4209003C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x4218003C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x40800000, 0x08000000, 0x08000000, 1},    /*                  */  \
        {0x42140000, 0xFFFFFFFF, 0x00000020, 1},    /*                  */  \
        {0x42040000, 0xFFFFFFFF, 0x00000200, 1},    /*                  */  \
        {0x42004000, 0xFFFFFFFF, 0x00000129, 1},    /*                  */  \
        {0x4210C000, 0xFFFFFFFF, 0x00000021, 1},    /*                  */  \
        {0x40000078, 0x00000100, 0x00000100, 1},    /*                  */  \
        {0x40000078, 0x00000008, 0x00000000, 1},    /*                  */  \
        {0x40000078, 0x00000100, 0x00000000, 1},    /*                  */  \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */  \
        {0x00000002, 0x00000000, 0x00000000, 0},    /* A3               */  \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */  \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */  \
        {0x40000058, 0x00000001, 0x00000001, 1},    /*                  */  \
        {0x40000058, 0x00000002, 0x00000002, 1},    /*                  */  \
        {0x43000010, 0xffffffff, 0x00000000, 1},    /*                  */  \
        {0x43000014, 0xffffffff, 0x00000517, 22},   /*                  */  \
        {0x42800190, 0xFFFFFFFF, 0x00000000, 1},    /*                  */  \
        {0x42800194, 0xFFFFFFFF, 0x00011186, 25},   /*                  */  \
        {0x40800010, 0x00000FFF, 0x00000FFF, 1},    /*                  */  \
        {0x40800020, 0x00000FFF, 0x00000FFF, 1},    /*                  */  \
        {0x40800060, 0x000F0000, 0x000F0000, 1},    /*                  */  \
        {0x42800110, 0xFFFFFFFF, 0x01732130, 1},    /*                  */  \
        {0x42800100, 0x000000FF, 0x00000077, 1},    /*                  */  \
        {0x4280010C, 0xFFFFFFFF, 0x17000850, 1},    /*                  */  \
        {0x42800108, 0x00000002, 0x00000002, 1},    /*                  */  \
        {0x42800114, 0x0000000F, 0x00000005, 1},    /*                  */  \
        {0x42800104, 0x00000041, 0x00000041, 1},    /*                  */  \
        {0x42800118, 0xFFFFFFFF, 0x00000003, 1},    /*                  */  \
        {0x42800118, 0xFFFFFFFF, 0x00000004, 1},    /*                  */  \
        {0x4280010C, 0x07000000, 0x06000000, 1},    /*                  */  \
        {0x41000010, 0x0000000E, 0x0000000C, 1},    /*                  */  \
        {0x41800000, 0x01000000, 0x00000000, 1},    /*                  */  \
        {0x40806000, 0xFFFFFFFF, 0x00000000, 1},    /*                  */  \
        {0x40806004, 0xFFFFFFFF, 0x00000004, 1},    /*                  */  \
        {0x40800010, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800014, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800018, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x4080001C, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x40800020, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                  */  \
        {0x42000000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42008000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42010000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42100000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42108000, 0x00000003, 0x00000003, 1},    /*                  */  \
        {0x42800110, 0x000F0000, 0x00040000, 1},    /*                  */  \
        {0x4208003C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x4208803C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x4209003C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x4218003C, 0x00003FF0, 0x00001AD0, 1},    /*                  */  \
        {0x40800000, 0x08000000, 0x08000000, 1},    /*                  */  \
        {0x40000038, 0xf0000000, 0x10000000, 1},    /*                  */  \
        {0x40000078, 0x00000100, 0x00000100, 1},    /*                  */  \
        {0x40000078, 0x00000008, 0x00000000, 1},    /*                  */  \
        {0x40000078, 0x00000100, 0x00000000, 1},    /*                  */  \
        {0xFFFFFFFF, 0x00000000, 0x00000000, 0}     /*                  */  \
    }                                                                       \
}

#define FA_98FX902A_48F4G_REG_VAL_INFO_LIST                                   \
{                                                                           \
    {0x00000000, 0x00000000, 0x00000000, 0},    /* A1               */      \
    {0x00000001, 0x00000000, 0x00000000, 0},    /*                  */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter        */      \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},    /* Delimiter - Init */      \
    {0x40000058, 0x00000003, 0x00000003, 1},    /* init foxtrot     */      \
    {0x40000058, 0x00007F00, 0x00000000, 1},    /*                  */      \
    {0x42800190, 0xFFFFFFFF, 0x00000000, 1},    /*DFCDL foxtrot sdram 300*/ \
    {0x42800194, 0xFFFFFFFF, 0x000090C3, 24},    /*                  */     \
    {0x42800110, 0x000F0000, 0x00040000, 1},    /* LD TWTR foxtrot   */     \
    {0x41800000, 0x01000000, 0x00000000, 1},    /* Disable E2E Foxtrot*/    \
    {0x40800000, 0x80000000, 0x80000000, 1},    /*                    */    \
    {0x4200005C, 0x0000FFFF, 0x0000321E, 1},    /* Gport Tx FIFO Flow Control Threshold*/   \
    {0x4200805C, 0x0000FFFF, 0x0000321E, 1},    /*                    */    \
    {0x4201005C, 0x0000FFFF, 0x0000321E, 1},    /*                    */    \
    {0x4210005C, 0x0000FFFF, 0x0000321E, 1},    /*                    */    \
    {0x4210805C, 0x0000FFFF, 0x0000321E, 1},    /*                    */    \
    {0x40806000, 0xFFFFFFFF, 0x00000000, 1},    /*  Device ID and Fabric Port*/ \
    {0x40806004, 0xFFFFFFFF, 0x00000001, 1},    /*                     */   \
    {0x40800010, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                     */   \
    {0x40800020, 0xFFFFFFFF, 0xFFFFFFFF, 1},    /*                     */   \
    {0x42008000, 0x00000003, 0x00000003, 1},    /*Enable ALL Gport and UC table*/   \
    {0x42000000, 0x00000003, 0x00000003, 1},    /*                      */  \
    {0x42108000, 0x00000003, 0x00000003, 1},    /*                      */  \
    {0x42010000, 0x00000003, 0x00000003, 1},    /*                      */  \
    {0x42100000, 0x00000003, 0x00000003, 1},    /*                      */  \
    {0x4210C000, 0xFFFFFFFF, 0x00000009, 1},    /*                      */  \
    {0x40800000, 0x00000000, 0x00000000, 1},    /*Set MC Forwarding Table*/ \
    {0x42140000, 0xFFFFFFFF, 0x00000020, 1},    /*                      */  \
    {0x42040000, 0xFFFFFFFF, 0x00000200, 1},    /*                      */  \
    {0x420C2200, 0xFFFFFFFF, 0x03E1C7E0, 1},    /* SERDES CONFIG        */  \
    {0x420C2204, 0xFFFFFFFF, 0x03E1C7E0, 1},    /*                      */  \
    {0x420C2208, 0xFFFFFFFF, 0x03E1C7E0, 1},    /*                      */  \
    {0x420C220C, 0xFFFFFFFF, 0x03E1C7E0, 1},    /*                      */  \
    {0x420C2210, 0xFFFFFFFF, 0x03E1C7E0, 1},    /*                      */  \
    {0x420C221C, 0xFFFFFFFF, 0x03E1C7E0, 1},    /*                      */  \
    {0x420C2218, 0xFFFFFFFF, 0x03E1C7E0, 1},    /*                      */  \
    {0x420C221C, 0xFFFFFFFF, 0x03E1C7E0, 1},    /*                      */  \
    {0x420C2220, 0xFFFFFFFF, 0x03E1C7E0, 1},    /*                      */  \
    {0x420C2224, 0xFFFFFFFF, 0x03E1C7E0, 1},    /*                      */  \
    {0x420C2228, 0xFFFFFFFF, 0x03E1C7E0, 1},    /*                      */  \
    {0x420C222C, 0xFFFFFFFF, 0x03E1C7E0, 1},    /*                      */  \
    {0x4210C000, 0xFFFFFFFF, 0x00000009, 1},    /* Foxtrot 0 UC Route   */  \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}     /* Delimiter        */      \
}

#define GT_FOX_REG_TIGER_12_VAL_INFO_LIST                                   \
{                                                                           \
    {0x00000000, 0x00000000, 0x00000000, 0},                                \
    {0x00000001, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},                                \
    {0x00000002, 0x00000000, 0x00000000, 0},                                \
    {0x00000003, 0x00000000, 0x00000000, 0},                                \
    {0x00000004, 0x00000000, 0x00000000, 0},                                \
    {0x00000005, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0},                                \
    /*    DFCDL  for FOX SDRAM           */                                 \
    {0x42800190, 0xFFFFFFFF, 0x0       , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x0       , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x1040    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2041    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x3081    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x4082    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x50C2    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x6102    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x7103    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x9143    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0xA144    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0xB184    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0xC1C4    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0xD1C5    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0xE205    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0xF206    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x10246   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x11286   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x12287   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x132C7   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x142C8   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x15308   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x16348   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x17349   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x19389   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x1A38A   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x1B3CA   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x1C40A   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x1D40B   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x1E44B   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x1F44C   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2048C   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x214CC   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x224CD   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2350D   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2450E   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2554E   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2658E   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2758F   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x275CF   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x275D0   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27610   , 1},                                \
    {0x42080050, 0xFFFFFFFF, 0xFFFFFFFF, 1},                                \
    {0x42088050, 0xFFFFFFFF, 0xFFFFFFFF, 1},                                \
    {0x42090050, 0xFFFFFFFF, 0xFFFFFFFF, 1},                                \
    {0x42180050, 0xFFFFFFFF, 0xFFFFFFFF, 1},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}                                 \
}

#define GT_FOX_REG_TIGER_52_VAL_INFO_LIST                                   \
{                                                                           \
    {0x0       , 0x0       , 0x0       , 0},                                \
    {0x1       , 0x0       , 0x0       , 0},                                \
    {0xFFFFFFFF, 0x0       , 0x0       , 0},                                \
    {0xFFFFFFFF, 0x0       , 0x0       , 0},                                \
    {0xFFFFFFFF, 0x0       , 0x0       , 0},                                \
    {0x2       , 0x0       , 0x0       , 0},                                \
    {0x3       , 0x0       , 0x0       , 0},                                \
    {0x4       , 0x0       , 0x0       , 0},                                \
    {0x5       , 0x0       , 0x0       , 0},                                \
    {0xFFFFFFFF, 0x0       , 0x0       , 0},                                \
    {0xFFFFFFFF, 0x0       , 0x0       , 0},                                \
    {0x4280010C, 0xFF000000, 0x6000000 , 1},                                \
    {0x42800190, 0xFFFFFFFF, 0x00000000, 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x0       , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2040    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x3041    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x5081    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x70C2    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x9102    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0xA102    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0xC143    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0xE183    , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x101C4   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x111C4   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x13204   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x15245   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x17285   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x18286   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x1A2C6   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x1C306   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x1D307   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x1F347   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x21388   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x233C8   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x243C8   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x26409   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27449   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2748A   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2748A   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x274CA   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2750B   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2754B   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2754C   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2758C   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x275CC   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x275CD   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2760D   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2764E   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2768E   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2768E   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x276CF   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x2770F   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27750   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27750   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27790   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x277D1   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27811   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27812   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27852   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27892   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27893   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x278D3   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27914   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27954   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27954   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x27995   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x279D5   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x279D6   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x279D6   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x279D6   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x279D7   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x279D7   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x279D8   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x279D8   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x279D8   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x279D9   , 1},                                \
    {0x42800194, 0xFFFFFFFF, 0x279D9   , 1},                                \
    {0x42080050, 0xFFFFFFFF, 0xFFFFFFFF, 1},                                \
    {0x42088050, 0xFFFFFFFF, 0xFFFFFFFF, 1},                                \
    {0x42090050, 0xFFFFFFFF, 0xFFFFFFFF, 1},                                \
    {0x42180050, 0xFFFFFFFF, 0xFFFFFFFF, 1},                                \
    {0xFFFFFFFF, 0x00000000, 0x00000000, 0}                                 \
}
/* Dummy for competability, for init process. */
#define GT_DUMMY_FA_REG_VAL_INFO_LIST                                          \
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

#define GT_FOX_REG_NULL_VAL_INFO_LIST               {}                         
                                                                               
/*******************************************************************************
* appDemoSetFaPhase1Params                                                     
*                                                                              
* DESCRIPTION:                                                                 
*       Initialize core level Fa phase1 initialization function.               
*                                                                              
* INPUTS:                                                                      
*       faPhase1Params  - Fa phase1 initialization params.                     
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
GT_STATUS appDemoSetFaPhase1Params
(
    IN GT_FA_PHASE1_INIT_PARAMS     *faPhase1Params
);


/*******************************************************************************
* appDemoSetFaPhase2Params
*
* DESCRIPTION:
*       Initialize core level Fa phase2 initialization function.
*
* INPUTS:
*       faPhase2Params  - Fa phase2 initialization params.
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
GT_STATUS appDemoSetFaPhase2Params
(
    IN GT_FA_PHASE2_INIT_PARAMS     *faPhase2Params
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtAppDemoFaConfigh */


