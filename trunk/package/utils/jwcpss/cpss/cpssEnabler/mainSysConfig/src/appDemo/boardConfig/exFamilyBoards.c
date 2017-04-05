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
* exFamilyBoards.c
*
* DESCRIPTION:
*       Includes board specific initialization definitions and data-structures.
*       Ex Family baord types.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#include <appDemo/boardConfig/gtBoardsConfigFuncs.h>

/*******************************************************************************
 * External variables
 ******************************************************************************/
/* entry for not defined yet board */
#define NO_BOARD_PLACE_HOLDER_CNS \
    {NULL, "", 0, {""}}

/* ExMx boards */
#if (defined EX_FAMILY) || (defined MX_FAMILY)
    #define __gtDbMx620E1041BoardReg                    gtDbMx620E1041BoardReg
    #define __gtRdEx12024GBoardReg                      gtRdEx12024GBoardReg
    #define __gtDbMx61048F4GsBoardReg                   gtDbMx61048F4GsBoardReg
    #define __gtDbMx62010GsBoardReg                     gtDbMx62010GsBoardReg
    #define __gtDbFx900Mx620ConfigBoardReg              gtDbFx900Mx620ConfigBoardReg
    #define __gtDbFx910Ex120ConfigBoardReg              gtDbFx910Ex120ConfigBoardReg
    #define __gtDbFx910Fx9210Ex120CapConfigBoardReg     gtDbFx910Fx9210Ex120CapConfigBoardReg
    #define __gtRdEx120D024GBoardReg                    gtRdEx120D024GBoardReg
    #define __gtDbFx910Mx62010GsBoardReg                gtDbFx910Mx62010GsBoardReg
    #define __gtDbFx910Mx61048F4GsBoardReg              gtDbFx910Mx61048F4GsBoardReg
    #define __gtDbEx24248GBoardReg                      gtDbEx24248GBoardReg
    #define __gtDbDX13xXGBoardReg                       gtDbDX13xXGBoardReg
    #define __gtDbFx910Mx62012GsBoardReg                gtDbFx910Mx62012GsBoardReg
    #define __gtDbFx910Mx61548F4GsBoardReg              gtDbFx910Mx61548F4GsBoardReg
    #define __gtDbMX635XGBoardReg                       gtDbMX635XGBoardReg
    #define __gtFsFx902Ex125_24GBoardReg                gtFsFx902Ex125_24GBoardReg
    #define __gtFsFx902Ex115_48F4GBoardReg              gtFsFx902Ex115_48F4GBoardReg
    #define __gtDbEx126_12G_BoardReg                    gtDbEx126_12G_BoardReg
    #define __gtDbEx100_24F4G_BoardReg                  gtDbEx100_24F4G_BoardReg
    #define __gtDbEx126Dx240BoardReg                    gtDbEx126Dx240BoardReg
    #define __gtDbEx126Dx160BoardReg                    gtDbEx126Dx160BoardReg
    #define __gtDbEx106Ex108BoardReg                    gtDbEx106Ex108BoardReg
    #define __gtDbEx136Dx270BoardReg                    gtDbEx136Dx270BoardReg
    #define __gtDb2Ex126_3Dx246BoardReg                 gtDb2Ex126_3Dx246BoardReg
#else /* ! (defined EX_FAMILY || defined MX_FAMILY)*/
    #define __gtDbMx620E1041BoardReg                    NULL
    #define __gtRdEx12024GBoardReg                      NULL
    #define __gtDbMx61048F4GsBoardReg                   NULL
    #define __gtDbMx62010GsBoardReg                     NULL
    #define __gtDbFx900Mx620ConfigBoardReg              NULL
    #define __gtDbFx910Ex120ConfigBoardReg              NULL
    #define __gtDbFx910Fx9210Ex120CapConfigBoardReg     NULL
    #define __gtRdEx120D024GBoardReg                    NULL
    #define __gtDbFx910Mx62010GsBoardReg                NULL
    #define __gtDbFx910Mx61048F4GsBoardReg              NULL
    #define __gtDbEx24248GBoardReg                      NULL
    #define __gtDbDX13xXGBoardReg                       NULL
    #define __gtDbFx910Mx62012GsBoardReg                NULL
    #define __gtDbFx910Mx61548F4GsBoardReg              NULL
    #define __gtDbMX635XGBoardReg                       NULL
    #define __gtFsFx902Ex125_24GBoardReg                NULL
    #define __gtFsFx902Ex115_48F4GBoardReg              NULL
    #define __gtDbEx126_12G_BoardReg                    NULL
    #define __gtDbEx100_24F4G_BoardReg                  NULL
    #define __gtDbEx126Dx240BoardReg                    NULL
    #define __gtDbEx126Dx160BoardReg                    NULL
    #define __gtDbEx106Ex108BoardReg                    NULL
    #define __gtDbEx136Dx270BoardReg                    NULL
    #define __gtDb2Ex126_3Dx246BoardReg                 NULL
#endif /* !(defined EX_FAMILY || defined MX_FAMILY) */

/* ExMxPm boards */
#if defined EXMXPM_FAMILY
    #ifndef IMPL_DUNE
        #define __gtDb98Ex240BoardReg                       gtDb98Ex240BoardReg
        #define __gtDb98ExMxPm3BoardReg                     gtDb98ExMxPm3BoardReg
        #define __gtDbFx950Ex240BoardReg                    NULL
        #define __gtDbFx9130BoardReg                          NULL
        #if defined CHX_FAMILY
            #define __gtDb98Ex240_3Dx246BoardReg            gtDb98Ex240_3Dx246BoardReg
            #define __gtDb98Ex240_DxLion48XGBoardReg        gtDb98Ex240_DxLion48XGBoardReg
        #else
            #define __gtDb98Ex240_3Dx246BoardReg            NULL
            #define __gtDb98Ex240_DxLion48XGBoardReg        NULL
        #endif
    #else
        /* EXMXPM with Dune */
        #define __gtDb98Ex240BoardReg                       gtDb98Ex240BoardReg
        #define __gtDb98ExMxPm3BoardReg                     gtDb98ExMxPm3BoardReg
        #define __gtDbFx950Ex240BoardReg                    gtDbFx950Ex240BoardReg
        #define __gtDbFx9130BoardReg                        gtDbFx9130BoardReg
    #endif /* EXMXPM with Dune */
#else /* ! defined EXMXPM_FAMILY */
    #define __gtDb98Ex240BoardReg                       NULL
    #define __gtDb98ExMxPm3BoardReg                     NULL
    #define __gtDb98Ex240_3Dx246BoardReg                NULL
    #define __gtDbFx950Ex240BoardReg                    NULL
    #define __gtDbFx9130BoardReg                        NULL
    #define __gtDb98Ex240_DxLion48XGBoardReg            NULL
#endif /* ! defined EXMXPM_FAMILY */

/*******************************************************************************
 * Local usage variables
 ******************************************************************************/
#ifndef IMPL_DUNE
GT_BOARD_LIST_ELEMENT   boardsList[] =
{
    {/*__gtDbMx620E1041BoardReg*/NULL,                "DB-MX620-E1041",
     1,                                     {"Rev 4.2"}},

    {/*__gtRdEx12024GBoardReg*/NULL,                  "RD-EX120-24G",
     1,                                     {"C0 & Later"}},

    {/*__gtDbMx61048F4GsBoardReg*/NULL,               "DB-MX610-48F4GS",
     3,                                     {/*1*/"C0",
                                             /*2*/"Rev 4.1",
                                             /*3*/"D1"}},

    {/*__gtDbMx62010GsBoardReg*/NULL,                 "DB-MX620-10/12GS",
     4,                                     {/*1*/"10G",
                                             /*2*/"12G",
                                             /*3*/"10G-D1",
                                             /*4*/"12G-D1"}},

    {/*__gtDbFx900Mx620ConfigBoardReg*/NULL,          "DB-FX900-MX620",
     2,                                     {/*1*/"3x6Serdes",
                                             /*2*/"4x4Serdes"}},

    {__gtDbFx910Ex120ConfigBoardReg,          "DB-FX910-EX120(12G)",  /* 6,4,0,1    - already exists in cpss */
     4,                                     {/*1*/"Cx - 3x6Serdes",
                                             /*2*/"Cx - 4x4Serdes",
                                             /*3*/"D1 - 3x6Serdes",
                                             /*4*/"D1 - 4x4Serdes"}},

    {__gtDbFx910Fx9210Ex120CapConfigBoardReg, "DB-FX910-FX9210-EX120(12G)",
     4,                                     {/*1*/"3x6Serdes",
                                             /*2*/"4x4Serdes",
                                             /*3*/"FX9210",
                                             /*4*/"FX9206"}},

    {__gtRdEx120D024GBoardReg,                "RD-EX120-D0-24G",      /* 8,1,0,1    ;  8,2,0,1 */
     2,                                     {/*1*/"C0 & Later",
                                             /*2*/"D1"}},

    {/*__gtDbFx910Mx62010GsBoardReg*/NULL,            "DB-FX910-MX620-10/12GS",
     4,                                     {/*1*/"10G - 3x6Serdes",
                                             /*2*/"10G - 4x4Serdes",
                                             /*3*/"12G - 3x6Serdes",
                                             /*4*/"12G - 4x4Serdes"}},

    {__gtDbFx910Mx61048F4GsBoardReg,          "DB-FX910-MX610-48F4GS",    /* 10,4,0,1 ;  10,6,0,1   - already in cpss */
     6,                                     {/*1*/"C0 - 3x6Serdes",
                                             /*2*/"C0 - 4x4Serdes",
                                             /*3*/"Rev 4.1 - 3x6Serdes",
                                             /*4*/"Rev 4.1 - 4x4Serdes",
                                             /*5*/"D1 - 3x6Serdes",
                                             /*6*/"D1 - 4x4Serdes"}},

    {/*__gtDbEx24248GBoardReg*/NULL,                  "DB-EX242-48G",
     1,                                     {"Rev 0.9"}},

    {__gtDbDX13xXGBoardReg,                   "DB-DX13x-XG",          /* 12,1,0,1  - already in cpss */
     1,                                     {"Rev 0.1"}},

    {/*__gtDbFx910Mx62012GsBoardReg*/NULL,            "DB-FX910-MX625-12GS",
     1,                                     {"Rev 0.1"}},

    {__gtDbFx910Mx61548F4GsBoardReg,          "DB-FX910-MX615-48F4GS",    /* 14,1,0,0   Samba/Rumba  DB-FX910-MX615-48F4GS - 3 boards*/
     1,                                     {"Rev 0.1"}},

    {/*__gtDbMX635XGBoardReg*/NULL,                   "DB-MX635-XG",
     1,                                     {"Rev 0.1"}},

    {/*__gtFsFx902Ex125_24GBoardReg*/NULL,            "Fs-EX125-FX902-24G",
     2,                                     {"Rev x00",
                                             "Rev x01"}},

    {/*__gtFsFx902Ex115_48F4GBoardReg*/NULL,          "Fs-EX115-FX902-48F4G",
     1,                                     {"D0"}},

    {__gtDbEx126_12G_BoardReg,                "DB-EX126-12G",     /*  18,1,0,0  - already in cpss; 3 boards  */
     4,                                     {/*1*/"Rev 0.1",
                                             /*2*/"Twist Drop In",
                                             /*3*/"Tiger On Dune board",
                                             /*4*/"Tiger Only on VB Board"}},

    {/*__gtDbEx100_24F4G_BoardReg*/NULL,              "DB-EX100-24Fa-4G",
     1,                                     {"Rev 0.1"}},

    {/*__gtDbEx126Dx240BoardReg*/NULL,                "EV-EX126-VB48G",   /* 20,1,0,0  - ValueBlade */
     1,                                     {"Rev 1.0"}},

    {/*__gtDbEx126Dx160BoardReg*/NULL,                "EV-EX126-VB20G",
     1,                                     {"Rev 1.0"}},

    {__gtDbEx106Ex108BoardReg,                "DB-EX106-EX108",   /* 22,1,0,0  */
     1,                                     {"Rev 1.0"}},

    {/*__gtDbEx136Dx270BoardReg*/NULL,     "EV-EX136-VB4XG",
     1,                                     {"Rev 1.0"}},

    {/*__gtDb2Ex126_3Dx246BoardReg*/NULL,  "EV-2-EX126-VB48G",
     1,                                     {"Rev 1.0"}},

    {__gtDb98Ex240BoardReg,                "DB-98EX240",          /* 25,1,0,0  */
     4,                                     {/*1*/"Rev 1.0",
                                             /*2*/"Rev 1.0 with external TCAM disabled",
                                             /*3*/"Rev 1.0 with RLD RAM and external TCAM disabled",
                                             /*4*/"Rev 1.0 with RLD RAM"}},

    {__gtDb98Ex240_3Dx246BoardReg,         "DB-EX240-VB48G",      /* 26,1,0,0  */
     2,                                     {/*1*/"Rev 1.0",
                                             /*2*/"Rev 1.0 with external TCAM disabled"}},

    {__gtDb98Ex240_DxLion48XGBoardReg,     "DB-EX240-VB-DXLion48G", /* 27,1,0,0  */
     1,                                     {/*1*/"Rev 1.0"}},

    {__gtDb98ExMxPm3BoardReg        ,      "DB-EX-Puma3",       /* 28,1,0,0  */
     13,                                     {/*1*/"Rev 1.0 - 22X10G + 8X40G  XLHGL     ",
                                              /*2*/"Rev 1.0 - 22X10G + 100G   ILKN24    ",
                                              /*3*/"Rev 1.0 - 6X40G  + 100G   ILKN24    ",
                                              /*4*/"Rev 1.0 - 2X100G + 100G   ILKN24    ",
                                              /*5*/"Rev 1.0 - 24X10G + 2X100G  ILKN16   ",
                                              /*6*/"Rev 1.0 - 6X40G  + 2X100G  ILKN16   ",
                                              /*7*/"Rev 1.0 - 2X100G + 8X40G  XLHGL     ",
                                              /*8*/"Rev 1.0 - 2X100G + 2X100G CHGL      ",
                                              /*9*/"Rev 1.0 - H3C XP48 12X10G + 100G ILKN24 (1,2,5,7,8,11,19,22,23,25,26,27,32,10) ",
                                             /*10*/"Rev 1.0 - H3C XLP16R 4X40G + 100G ILKN24 (0,4,16,20,32,10) ",
                                             /*11*/"Rev 1.0 - H3C XP40R 20X10G + 2X100G ILKN16 (0-9,16-25,32,48,10) ",
                                             /*12*/"Rev 1.0 - H3C CP4R 2X100G + 100G ILKN24 (0,16,32,10)    ",
                                             /*13*/"Rev 1.0 - H3C CP4R 100G+40G + 100G ILKN24 (4,16,32,10)  "} },



};
#else /* IMPL_DUNE */
GT_BOARD_LIST_ELEMENT   boardsList[] =
{
    {/* gtDuneBrd0012aBoardReg*/ NULL,                "",
     1,                                     {"Rev 1.0"}},

    {   gtDbFx9130BoardReg ,                "DUNE-BRD0002A",
     2,                                     {"Rev 1.0 in NEGEV",
                                             "Rev 1.0 in GOBY"}},

    {/*gtDuneEx136Dx270BoardReg*/ NULL,              "",
     2,                                     {"Rev 1.0",
                                             "Virtual Board - One VB only"}},

    {/*gtDuneEx126BoardReg*/ NULL,                   "",
     1,                                     {"Rev 0.1"}},

    {__gtDbFx950Ex240BoardReg,                   "FX950-EX240-2XG",
     2,                                     {/*1*/"Rev 1.0",
                                             /*2*/"Rev 1.0 with external TCAM disabled",}},
    NO_BOARD_PLACE_HOLDER_CNS, /* 6, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 7, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 8, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 9, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 10, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 11, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 12, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 13, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 14, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 15, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 16, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 17, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 18, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 19, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 20, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 21, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 22, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 23, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 24, x, 0 */
    {__gtDb98Ex240BoardReg,                "DB-98EX240",   /* 25,1,0,0  */
     4,                                     {/*1*/"Rev 1.0",
                                             /*2*/"Rev 1.0 with external TCAM disabled",
                                             /*3*/"Rev 1.0 with RLD RAM and external TCAM disabled",
                                             /*4*/"Rev 1.0 with RLD RAM"}}

    NO_BOARD_PLACE_HOLDER_CNS, /* 26, x, 0 */
    NO_BOARD_PLACE_HOLDER_CNS, /* 27, x, 0 */
    {__gtDb98ExMxPm3BoardReg        ,      "DB-EX-Puma3", /* 28,1,0,0  */
     1,                                     {/*1*/"Rev 1.0"}},

};
#endif /* IMPL_DUNE */


const GT_U32 boardListLen = sizeof(boardsList) / sizeof(GT_BOARD_LIST_ELEMENT);

