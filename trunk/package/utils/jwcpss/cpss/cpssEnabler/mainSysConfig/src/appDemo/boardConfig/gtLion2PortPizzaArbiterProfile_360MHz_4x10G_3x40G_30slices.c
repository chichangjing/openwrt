/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortPizzaArbiterTablesLionB0_360MHz_30slices.c 
*
* DESCRIPTION:
*       Pizza arbiter Table for Lion2 B0 240 MHz clock (7 ports 7 slices)
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterTables.h>

/* 120G / 30 Slices ==> 4G per Slice */
/* Config    Real                    */
/* 40G   --> 16G                     */
/* 10    --> 12G                     */

CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC portSpeed2SliceNumListLionB0_360Mhz_30Slices_NoAlloc[] = 
{     /* port speed        ,     slices N */
     {   CPSS_PORT_SPEED_NA_E,        CPSS_INVALID_SLICE_NUM }
};

CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC portSpeed2SliceNumListLionB0_360Mhz_30Slices[] = 
{     /* port speed        ,     slices N */
     {   CPSS_PORT_SPEED_10_E,        3 }
    ,{   CPSS_PORT_SPEED_100_E,       3 }
    ,{   CPSS_PORT_SPEED_1000_E,      3 }
    ,{   CPSS_PORT_SPEED_2500_E,      3 }
    ,{   CPSS_PORT_SPEED_10000_E,     3 }
    ,{   CPSS_PORT_SPEED_40000_E,     4 }  /* 16G */
    ,{   CPSS_PORT_SPEED_NA_E,        CPSS_INVALID_SLICE_NUM }
};


#if 0
    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC portSpeed2SliceNumListLionB0_360Mhz_30Slices_40G_4Slices_10G_1Slice[] = 
    {     /* port speed        ,     slices N */
         {   CPSS_PORT_SPEED_10000_E,     1 }
        ,{   CPSS_PORT_SPEED_40000_E,     4 }  /* 16G */
        ,{   CPSS_PORT_SPEED_NA_E,        CPSS_INVALID_SLICE_NUM }
    };

    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC portSpeed2SliceNumListLionB0_360Mhz_30Slices_10G_1Slices[] = 
    {     /* port speed        ,     slices N */
         {   CPSS_PORT_SPEED_10000_E,     1 }
        ,{   CPSS_PORT_SPEED_NA_E,        CPSS_INVALID_SLICE_NUM }
    };


    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC portSpeed2SliceNumListLionB0_360Mhz_30Slices_40G_4Slices_10G_2Slice[] = 
    {     /* port speed        ,     slices N */
         {   CPSS_PORT_SPEED_10000_E,     2 }
        ,{   CPSS_PORT_SPEED_40000_E,     4 }  /* 16G */
        ,{   CPSS_PORT_SPEED_NA_E,        CPSS_INVALID_SLICE_NUM }
    };

    CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC portSpeed2SliceNumListLionB0_360Mhz_30Slices_10G_2Slices[] = 
    {     /* port speed        ,     slices N */
         {   CPSS_PORT_SPEED_10000_E,     2 }
        ,{   CPSS_PORT_SPEED_NA_E,        CPSS_INVALID_SLICE_NUM }
    };
#endif

const CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC portGroupPortSpeed2SliceNum_360MHz_30Slices = 
{
    /* 12 */
    {
         { 0 , &portSpeed2SliceNumListLionB0_360Mhz_30Slices[0] }
        ,{ 1 , &portSpeed2SliceNumListLionB0_360Mhz_30Slices[0] }
        ,{ 2 , &portSpeed2SliceNumListLionB0_360Mhz_30Slices[0] }
        ,{ 3 , &portSpeed2SliceNumListLionB0_360Mhz_30Slices[0] }
        ,{ 4 , &portSpeed2SliceNumListLionB0_360Mhz_30Slices[0] } /* &portSpeed2SliceNumListLionB0_360Mhz_30Slices_40G_4Slices_10G_1Slice[0] } */
        ,{ 5 , &portSpeed2SliceNumListLionB0_360Mhz_30Slices_NoAlloc[0] } /* &portSpeed2SliceNumListLionB0_360Mhz_30Slices_10G_1Slices[0] }  */
        ,{ 6 , &portSpeed2SliceNumListLionB0_360Mhz_30Slices_NoAlloc[0] } /* &portSpeed2SliceNumListLionB0_360Mhz_30Slices_10G_1Slices[0] }  */
        ,{ 7 , &portSpeed2SliceNumListLionB0_360Mhz_30Slices_NoAlloc[0] } /* &portSpeed2SliceNumListLionB0_360Mhz_30Slices_10G_1Slices[0] }  */
        ,{ 8 , &portSpeed2SliceNumListLionB0_360Mhz_30Slices[0] } /* &portSpeed2SliceNumListLionB0_360Mhz_30Slices_40G_4Slices_10G_2Slice[0] }  */
        ,{ 9 , &portSpeed2SliceNumListLionB0_360Mhz_30Slices[0] } /* &portSpeed2SliceNumListLionB0_360Mhz_30Slices_40G_4Slices_10G_2Slice[0] }  */
        ,{10 , &portSpeed2SliceNumListLionB0_360Mhz_30Slices_NoAlloc[0] } /* &portSpeed2SliceNumListLionB0_360Mhz_30Slices_10G_2Slices[0] }  */
        ,{11 , &portSpeed2SliceNumListLionB0_360Mhz_30Slices_NoAlloc[0] } /* &portSpeed2SliceNumListLionB0_360Mhz_30Slices_10G_2Slices[0] }  */
        ,{CPSS_PA_INVALID_PORT  , NULL     }
    }
};


/*---------------------------------------------------------------------------
**  Port Priority tables
**---------------------------------------------------------------------------
*/

const CPSS_DXCH_PORT_PRIORITY_MATR_STC portPriorityLion2_12ports_360MHz_30Slices = 
{
/* GT_U32        portN; */12
/* portPriorityList  */  ,{
        /* port  0*/           { CPSS_PA_INVALID_PORT }
        /* port  1*/          ,{ CPSS_PA_INVALID_PORT }     
        /* port  2*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  3*/          ,{ CPSS_PA_INVALID_PORT }   
        /* port  4*/          ,{ 5, 6, 7, CPSS_PA_INVALID_PORT }
        /* port  5*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  6*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  7*/          ,{ CPSS_PA_INVALID_PORT }   
        /* port  8*/          ,{ 10, CPSS_PA_INVALID_PORT }
        /* port  9*/          ,{ 11, CPSS_PA_INVALID_PORT }
        /* port 10*/          ,{ CPSS_PA_INVALID_PORT }
        /* port 11*/          ,{ CPSS_PA_INVALID_PORT }  
                          }
};

/*-------------------------------------------------------
** Lion 2 B0 210 MHz 7 ports 7 slices configs
**-------------------------------------------------------
*/

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2B0_360MHz_30Slices_ConfBy1Slice = 
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */       { CPSS_PA_INVALID_SLICE }
/* PORT 1  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 2  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 3  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 4  */      ,{  0,  CPSS_PA_INVALID_SLICE }
/* PORT 5  */      ,{  7,  CPSS_PA_INVALID_SLICE }
/* PORT 6  */      ,{ 14,  CPSS_PA_INVALID_SLICE }
/* PORT 7  */      ,{ 21,  CPSS_PA_INVALID_SLICE }
/* PORT 8  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 9  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 10 */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 11 */      ,{ CPSS_PA_INVALID_SLICE }
                }
};

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2B0_360MHz_30Slices_ConfBy2Slice = 
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */       { CPSS_PA_INVALID_SLICE }
/* PORT 1  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 2  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 3  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 4  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 5  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 6  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 7  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 8  */      ,{  1,  8, CPSS_PA_INVALID_SLICE }
/* PORT 9  */      ,{  2,  9, CPSS_PA_INVALID_SLICE }
/* PORT 10 */      ,{ 15, 22, CPSS_PA_INVALID_SLICE }
/* PORT 11 */      ,{ 16, 23, CPSS_PA_INVALID_SLICE }
                }
};


const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2B0_360MHz_30Slices_ConfBy3Slice = 
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */       {  3, 10, 17, CPSS_PA_INVALID_SLICE }
/* PORT 1  */      ,{  4, 11, 18, CPSS_PA_INVALID_SLICE }
/* PORT 2  */      ,{  5, 12, 19, CPSS_PA_INVALID_SLICE }
/* PORT 3  */      ,{  6, 13, 20, CPSS_PA_INVALID_SLICE }
/* PORT 4  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 5  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 6  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 7  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 8  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 9  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 10 */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 11 */      ,{ CPSS_PA_INVALID_SLICE }
                } 
};


const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2B0_360MHz_30Slices_ConfBy4Slice = 
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */       { CPSS_PA_INVALID_SLICE }
/* PORT 1  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 2  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 3  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 4  */      ,{  0,  7, 14, 21, CPSS_PA_INVALID_SLICE }
/* PORT 5  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 6  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 7  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 8  */      ,{  1,  8, 15, 22, CPSS_PA_INVALID_SLICE }
/* PORT 9  */      ,{  2,  9, 16, 23, CPSS_PA_INVALID_SLICE }
/* PORT 10 */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 11 */      ,{ CPSS_PA_INVALID_SLICE }
                }
};


const CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC lion2B0_360MHz_30Slices = 
{
/* portNum */       12
/* maxSliceNum */  ,30
/* pizzaCfg */     ,{
                         { 1, (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_360MHz_30Slices_ConfBy1Slice }
                        ,{ 2, (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_360MHz_30Slices_ConfBy2Slice }
                        ,{ 3, (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_360MHz_30Slices_ConfBy3Slice }
                        ,{ 4, (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_360MHz_30Slices_ConfBy4Slice }
                        ,{ 0, NULL                                                                     }
                    }
};

const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2B0Clock360MHz12Ports30Sllices = 
{ 
    /* portNum        */  12
    /* maxSliceNum    */ ,30
    /* txQDef         */ ,{ 
    /* TXQ_repetition */     4
    /*    TxQDef      */    ,{
    /*       CPU TYPE */         CPSS_DXCH_CPUPORT_TYPE_EXTERNAL_E
    /*       CPU Port */        ,15                 
    /*       Slices   */        ,{CPSS_PA_INVALID_SLICE, 0, 0, 0, 0 }
    /*                */     }                    
    /*                */  }
    ,(CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)&lion2B0_360MHz_30Slices
    ,(CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)&lion2B0_360MHz_30Slices
    ,(CPSS_DXCH_PORT_PRIORITY_MATR_STC *)&portPriorityLion2_12ports_360MHz_30Slices
};


