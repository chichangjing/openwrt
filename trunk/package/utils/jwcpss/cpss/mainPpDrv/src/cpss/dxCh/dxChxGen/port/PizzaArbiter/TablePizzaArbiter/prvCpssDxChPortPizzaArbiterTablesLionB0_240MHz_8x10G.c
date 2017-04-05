/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortPizzaArbiterTablesLionB0_240MHz_8x10G.c 
*
* DESCRIPTION:
*       Pizza arbiter Table for Lion2 B0 240 MHz clock (8x10G ports 8 slices)
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiterProfile.h>

CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC portSpeed2SliceNumListLionB0_240Mhz_8x10G[] = 
{     /* port speed        ,     slices N */
     {   CPSS_PORT_SPEED_10_E,        1 }
    ,{   CPSS_PORT_SPEED_100_E,       1 }
    ,{   CPSS_PORT_SPEED_1000_E,      1 }
    ,{   CPSS_PORT_SPEED_2500_E,      1 }
    ,{   CPSS_PORT_SPEED_10000_E,     1 }
    ,{   CPSS_PORT_SPEED_NA_E,        CPSS_INVALID_SLICE_NUM }
};

const CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC prvPortGroupPortSpeed2SliceNumLionB0_240Mhz_8x10G = 
{
    /* 12 */
    {
         { 0 , &portSpeed2SliceNumListLionB0_240Mhz_8x10G[0] }
        ,{ 1 , &portSpeed2SliceNumListLionB0_240Mhz_8x10G[0] }
        ,{ 2 , &portSpeed2SliceNumListLionB0_240Mhz_8x10G[0] }
        ,{ 3 , &portSpeed2SliceNumListLionB0_240Mhz_8x10G[0] }
        ,{ 4 , &portSpeed2SliceNumListLionB0_240Mhz_8x10G[0] }
        ,{ 5 , &portSpeed2SliceNumListLionB0_240Mhz_8x10G[0] }
        ,{ 6 , &portSpeed2SliceNumListLionB0_240Mhz_8x10G[0] }
        ,{ 7 , &portSpeed2SliceNumListLionB0_240Mhz_8x10G[0] }
        ,{CPSS_PA_INVALID_PORT  , NULL     }
    }
};


/*---------------------------------------------------------------------------
**  Port Priority tables
**---------------------------------------------------------------------------
*/

const CPSS_DXCH_PORT_PRIORITY_MATR_STC portPriorityLion2B0_240Mhz_8x10G = 
{
/* GT_U32        portN; */8
/* portPriorityList  */  ,{
        /* port  0*/           { CPSS_PA_INVALID_PORT }
        /* port  1*/          ,{ CPSS_PA_INVALID_PORT }     
        /* port  2*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  3*/          ,{ CPSS_PA_INVALID_PORT }   
        /* port  4*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  5*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  6*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  7*/          ,{ CPSS_PA_INVALID_PORT }
                          }
};

/*-------------------------------------------------------
** Lion 2 B0 210 MHz 7 ports 7 slices configs
**-------------------------------------------------------
*/

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2B0_240MHz_8x10G_ConfBy1Slice = 
{
/* maxPortNum*/ 8
/* portArr   */,{
/* PORT 0  */       { 0  , CPSS_PA_INVALID_SLICE }
/* PORT 1  */      ,{ 1  , CPSS_PA_INVALID_SLICE }
/* PORT 2  */      ,{ 2  , CPSS_PA_INVALID_SLICE }
/* PORT 3  */      ,{ 3  , CPSS_PA_INVALID_SLICE }
/* PORT 4  */      ,{ 4  , CPSS_PA_INVALID_SLICE }
/* PORT 5  */      ,{ 5  , CPSS_PA_INVALID_SLICE }
/* PORT 6  */      ,{ 6  , CPSS_PA_INVALID_SLICE }
/* PORT 7  */      ,{ 7  , CPSS_PA_INVALID_SLICE }
                }
};


const CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC lion2B0_240MHz_8x10G = 
{
/* portNum */       8
/* maxSliceNum */  ,8
/* pizzaCfg */     ,{
                         { 1, (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_240MHz_8x10G_ConfBy1Slice  }
                        ,{ 0, NULL                                                                                     }
                    }
};

const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2B0Clock240MHz8x10G  = 
{ 
    /* portNum        */  8
    /* maxSliceNum    */ ,8
    /* txQDef         */ ,{ 
    /* TXQ_repetition */     10
    /*    TxQDef      */    ,{
    /*       CPU TYPE */         CPSS_DXCH_CPUPORT_TYPE_EXTERNAL_E
    /*       CPU Port */        ,15                 
    /*       Slices   */        ,{CPSS_PA_INVALID_SLICE, 0, 0, 0, 0 }
    /*                */     }                    
    /*                */  }
    ,(CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)&lion2B0_240MHz_8x10G
    ,NULL
    ,(CPSS_DXCH_PORT_PRIORITY_MATR_STC *)&portPriorityLion2B0_240Mhz_8x10G
};


