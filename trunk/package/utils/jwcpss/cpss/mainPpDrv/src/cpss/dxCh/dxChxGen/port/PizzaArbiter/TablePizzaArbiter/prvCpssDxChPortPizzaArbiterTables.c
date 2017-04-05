/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvCpssDxChPortPizzaArbiterTables.c
*
* DESCRIPTION:
*       Pizza arbiter Table for Lion2 A0 360/480 GHz clock (12/16 slices)
                                      B0 360/480 GHz clock (24/32 slices)
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*******************************************************************************/

#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/cpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiter.h>
#include <cpss/dxCh/dxChxGen/port/PizzaArbiter/TablePizzaArbiter/prvCpssDxChPortPizzaArbiterTables.h>



CPSS_DXCH_PORT_SPEED_TO_SLICE_NUM_STC portSpeed2SliceNumListDefault[] =
{     /* port speed        ,     slices N */
     {   CPSS_PORT_SPEED_10_E,        1 }
    ,{   CPSS_PORT_SPEED_100_E,       1 }
    ,{   CPSS_PORT_SPEED_1000_E,      1 }
    ,{   CPSS_PORT_SPEED_2500_E,      1 }
    ,{   CPSS_PORT_SPEED_10000_E,     1 }
    ,{   CPSS_PORT_SPEED_11800_E,     1 }
    ,{   CPSS_PORT_SPEED_12000_E,     2 }
    ,{   CPSS_PORT_SPEED_16000_E,     2 }
    ,{   CPSS_PORT_SPEED_15000_E,     2 }
    ,{   CPSS_PORT_SPEED_20000_E,     2 }
    ,{   CPSS_PORT_SPEED_40000_E,     4 }
    ,{   CPSS_PORT_SPEED_47200_E,     4 }
    ,{   CPSS_PORT_SPEED_NA_E,        CPSS_INVALID_SLICE_NUM }
};

const CPSS_DXCH_PORT_PIZZA_PORTGROUP_SPEED_TO_SLICENUM_STC prvPortGroupPortSpeed2SliceNumDefault =
{
    /* 12 */
    {
         { 0 , &portSpeed2SliceNumListDefault[0] }
        ,{ 1 , &portSpeed2SliceNumListDefault[0] }
        ,{ 2 , &portSpeed2SliceNumListDefault[0] }
        ,{ 3 , &portSpeed2SliceNumListDefault[0] }
        ,{ 4 , &portSpeed2SliceNumListDefault[0] }
        ,{ 5 , &portSpeed2SliceNumListDefault[0] }
        ,{ 6 , &portSpeed2SliceNumListDefault[0] }
        ,{ 7 , &portSpeed2SliceNumListDefault[0] }
        ,{ 8 , &portSpeed2SliceNumListDefault[0] }
        ,{ 9 , &portSpeed2SliceNumListDefault[0] }
        ,{10 , &portSpeed2SliceNumListDefault[0] }
        ,{11 , &portSpeed2SliceNumListDefault[0] }
        ,{CPSS_PA_INVALID_PORT  , NULL     }
    }
};


/*---------------------------------------------------------------------------
**  Port Priority tables
**---------------------------------------------------------------------------
*/

const CPSS_DXCH_PORT_PRIORITY_MATR_STC portPriorityLion2_12ports_12_24_slices =
{
/* GT_U32        portN; */12
/* portPriorityList  */  ,{
        /* port  0*/           { 1,  2,  3, CPSS_PA_INVALID_PORT }
        /* port  1*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  2*/          ,{ 3, CPSS_PA_INVALID_PORT }
        /* port  3*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  4*/          ,{ 5, 6, 7, CPSS_PA_INVALID_PORT }
        /* port  5*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  6*/          ,{ 7, CPSS_PA_INVALID_PORT }
        /* port  7*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  8*/          ,{ 9, 10, 11, CPSS_PA_INVALID_PORT }
        /* port  9*/          ,{ CPSS_PA_INVALID_PORT }
        /* port 10*/          ,{ 11, CPSS_PA_INVALID_PORT }
        /* port 11*/          ,{ CPSS_PA_INVALID_PORT }
                          }
};


const CPSS_DXCH_PORT_PRIORITY_MATR_STC portPriorityLion2_12ports_16_32_slices =
{
/* GT_U32        portN; */12
/* portPriorityList  */  ,{
        /* port  0*/           { 1,  2,  3, CPSS_PA_INVALID_PORT }
        /* port  1*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  2*/          ,{ 3, CPSS_PA_INVALID_PORT }
        /* port  3*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  4*/          ,{ 5, 6, 7, CPSS_PA_INVALID_PORT }
        /* port  5*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  6*/          ,{ 7, CPSS_PA_INVALID_PORT }
        /* port  7*/          ,{ CPSS_PA_INVALID_PORT }
        /* port  8*/          ,{ 10, 11, CPSS_PA_INVALID_PORT }
        /* port  9*/          ,{ 11, CPSS_PA_INVALID_PORT }
        /* port 10*/          ,{ 11, CPSS_PA_INVALID_PORT }
        /* port 11*/          ,{ CPSS_PA_INVALID_PORT }
                            }
};


/*-------------------------------------------------------
** Lion 2 A0 Pizzas
**-------------------------------------------------------
*/


/*-------------------------------------------------------
** Lion 2 A0 360MHz 12 slices configs
**-------------------------------------------------------
*/

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2A0_12SlicesConfBy1Slice =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */       { 0  , CPSS_PA_INVALID_SLICE }
/* PORT 1  */      ,{ 3  , CPSS_PA_INVALID_SLICE }
/* PORT 2  */      ,{ 6  , CPSS_PA_INVALID_SLICE }
/* PORT 3  */      ,{ 9  , CPSS_PA_INVALID_SLICE }
/* PORT 4  */      ,{ 2  , CPSS_PA_INVALID_SLICE }
/* PORT 5  */      ,{ 5  , CPSS_PA_INVALID_SLICE }
/* PORT 6  */      ,{ 8  , CPSS_PA_INVALID_SLICE }
/* PORT 7  */      ,{ 11 , CPSS_PA_INVALID_SLICE }
/* PORT 8  */      ,{ 1  , CPSS_PA_INVALID_SLICE }
/* PORT 9  */      ,{ 4  , CPSS_PA_INVALID_SLICE }
/* PORT 10 */      ,{ 7  , CPSS_PA_INVALID_SLICE }
/* PORT 11 */      ,{ 10 , CPSS_PA_INVALID_SLICE }
                }
};

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2A0_12SlicesConfBy2Slices =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */         { 0  , 3  , CPSS_PA_INVALID_SLICE }
/* PORT 1  */        ,{ CPSS_PA_INVALID_SLICE }
/* PORT 2  */        ,{ 6  , 9  , CPSS_PA_INVALID_SLICE }
/* PORT 3  */        ,{ CPSS_PA_INVALID_SLICE }
/* PORT 4  */        ,{ 2  , 5  , CPSS_PA_INVALID_SLICE }
/* PORT 5  */        ,{ CPSS_PA_INVALID_SLICE }
/* PORT 6  */        ,{ 8  , 11 , CPSS_PA_INVALID_SLICE }
/* PORT 7  */        ,{ CPSS_PA_INVALID_SLICE }
/* PORT 8  */        ,{ 1  , 4  , CPSS_PA_INVALID_SLICE }
/* PORT 9  */        ,{ CPSS_PA_INVALID_SLICE }
/* PORT 10 */        ,{ 7  , 10 , CPSS_PA_INVALID_SLICE }
/* PORT 11 */        ,{ CPSS_PA_INVALID_SLICE }
                }
};

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2A0_12SlicesConfBy4Slices =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */        { 0  , 3  , 6  , 9  , CPSS_PA_INVALID_SLICE }
/* PORT 1  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 2  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 3  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 4  */       ,{ 2  , 5  , 8  , 11 , CPSS_PA_INVALID_SLICE }
/* PORT 5  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 6  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 7  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 8  */       ,{ 1  , 4  , 7  , 10 , CPSS_PA_INVALID_SLICE }
/* PORT 9  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 10 */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 11 */       ,{ CPSS_PA_INVALID_SLICE }
                }
};

const CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC g_lion2_A0_12Slices =
{
/* portNum */       12
/* maxSliceNum */  ,12
/* pizzaCfg */     ,{
                         { 1, (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2A0_12SlicesConfBy1Slice  }
                        ,{ 2, (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2A0_12SlicesConfBy2Slices }
                        ,{ 4, (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2A0_12SlicesConfBy4Slices }
                        ,{ 0, NULL                                                              }
                    }
};

const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2A0Clock360MHz12Ports12Sllices =
{
    /* portNum        */  12
    /* maxSliceNum    */ ,12
    /* txQDef         */ ,{
    /* TXQ_repetition */     10
    /*    TxQDef      */    ,{
    /*       CPU TYPE */         CPSS_DXCH_CPUPORT_TYPE_EXTERNAL_E
    /*       CPU Port */        ,15
    /*       Slices   */        ,{CPSS_PA_INVALID_SLICE, 0, 0, 0, 0 }
    /*                */     }
    /*                */  }
    ,(CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)&g_lion2_A0_12Slices
    ,NULL
    ,(CPSS_DXCH_PORT_PRIORITY_MATR_STC *)&portPriorityLion2_12ports_12_24_slices
};


/*-------------------------------------------------------
** Lion 2 A0 480 MHz 16 Slices Pizza
**-------------------------------------------------------
*/

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2A0_16SlicesConfRegModeBy1Slice =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */         { 0  , CPSS_PA_INVALID_SLICE }
/* PORT 1  */        ,{ 8  , CPSS_PA_INVALID_SLICE }
/* PORT 2  */        ,{ 4  , CPSS_PA_INVALID_SLICE }
/* PORT 3  */        ,{ 12 , CPSS_PA_INVALID_SLICE }
/* PORT 4  */        ,{ 2  , CPSS_PA_INVALID_SLICE }
/* PORT 5  */        ,{ 10 , CPSS_PA_INVALID_SLICE }
/* PORT 6  */        ,{ 14 , CPSS_PA_INVALID_SLICE }
/* PORT 7  */        ,{ 6  , CPSS_PA_INVALID_SLICE }
/* PORT 8  */        ,{ 1  , CPSS_PA_INVALID_SLICE }
/* PORT 9  */        ,{ 3  , CPSS_PA_INVALID_SLICE }
/* PORT 10 */        ,{ 5  , CPSS_PA_INVALID_SLICE }
/* PORT 11 */        ,{ 13 , CPSS_PA_INVALID_SLICE }
                }
};

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2A0_16SlicesConfExtModeBy1Slice =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */         { 0  , CPSS_PA_INVALID_SLICE }
/* PORT 1  */        ,{ 8  , CPSS_PA_INVALID_SLICE }
/* PORT 2  */        ,{ 4  , CPSS_PA_INVALID_SLICE }
/* PORT 3  */        ,{ 12 , CPSS_PA_INVALID_SLICE }
/* PORT 4  */        ,{ 2  , CPSS_PA_INVALID_SLICE }
/* PORT 5  */        ,{ 10 , CPSS_PA_INVALID_SLICE }
/* PORT 6  */        ,{ 14 , CPSS_PA_INVALID_SLICE }
/* PORT 7  */        ,{ 6  , CPSS_PA_INVALID_SLICE }
/* PORT 8  */        ,{ 1  , CPSS_PA_INVALID_SLICE }
/* PORT 9  */        ,{ 3  , CPSS_PA_INVALID_SLICE }
/* PORT 10 */        ,{ 5  , CPSS_PA_INVALID_SLICE }
/* PORT 11 */        ,{ 7  , CPSS_PA_INVALID_SLICE }
                }
};



const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2A0_16SlicesConfBy2Slices =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */       { 0  , 8  , CPSS_PA_INVALID_SLICE }
/* PORT 1  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 2  */      ,{ 4  , 12 , CPSS_PA_INVALID_SLICE }
/* PORT 3  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 4  */      ,{ 2  , 10 , CPSS_PA_INVALID_SLICE }
/* PORT 5  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 6  */      ,{ 6  , 14 , CPSS_PA_INVALID_SLICE }
/* PORT 7  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 8  */      ,{ 1  , 9  , CPSS_PA_INVALID_SLICE }
/* PORT 9  */      ,{ 3  , 11 , CPSS_PA_INVALID_SLICE }
/* PORT 10 */      ,{ 5  , 13 , CPSS_PA_INVALID_SLICE }
/* PORT 11 */      ,{ 7  , 15 , CPSS_PA_INVALID_SLICE }
                }
};

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2A0_16SlicesConfBy4Slices =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */       { 0  , 4  , 8  , 12 , CPSS_PA_INVALID_SLICE }
/* PORT 1  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 2  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 3  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 4  */      ,{ 2  , 6  , 10 , 14 , CPSS_PA_INVALID_SLICE }
/* PORT 5  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 6  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 7  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 8  */      ,{ 1  , 5  , 9  , 13 , CPSS_PA_INVALID_SLICE }
/* PORT 9  */      ,{ 3  , 7  , 11 , 15 , CPSS_PA_INVALID_SLICE }
/* PORT 10 */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 11 */      ,{ CPSS_PA_INVALID_SLICE }
                }
};

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2A0_16SlicesConfBy10Slices =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */       { 0  , 1  , 2  , 4  , 6  , 8  , 9  , 10 , 12 , 14, CPSS_PA_INVALID_SLICE }
/* PORT 1  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 2  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 3  */      ,{ CPSS_PA_INVALID_SLICE }
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


const CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC g_lion2_A0_16Slices_RegMode =
{
/* portNum     */     12
/* maxSliceNum */    ,16
/* pizzaCfg    */    ,{
                            {1, (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2A0_16SlicesConfRegModeBy1Slice }
                           ,{2, (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2A0_16SlicesConfBy2Slices       }
                           ,{4, (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2A0_16SlicesConfBy4Slices       }
                           ,{0,  NULL                                                                   }
                      }
};

const CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC g_lion2_A0_16Slices_ExtMode =
{
/* portNum     */     12
/* maxSliceNum */    ,16
/* pizzaCfg    */    ,{
                             {1 , (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2A0_16SlicesConfExtModeBy1Slice}
                            ,{2 , (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2A0_16SlicesConfBy2Slices      }
                            ,{4 , (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2A0_16SlicesConfBy4Slices      }
                            ,{0 ,  NULL                                                                  }
                      }
};

const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2A0Clock480MHz12Ports16Sllices =
{
    /* portNum        */  12
    /* maxSliceNum    */ ,16
    /* txQDef         */ ,{
    /* TXQ_repetition */     10
    /*    TxQDef      */    ,{
    /*       CPU TYPE */         CPSS_DXCH_CPUPORT_TYPE_EXTERNAL_E
    /*       CPU Port */        ,15
    /*       Slices   */        ,{CPSS_PA_INVALID_SLICE, 0, 0, 0, 0 }
    /*                */     }
    /*                */  }
    ,(CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)&g_lion2_A0_16Slices_RegMode
    ,(CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)&g_lion2_A0_16Slices_ExtMode
    ,(CPSS_DXCH_PORT_PRIORITY_MATR_STC *)&portPriorityLion2_12ports_16_32_slices
};

/*-------------------------------------------------------
** Lion 2 B0 360MHz 24 Slices configs
**-------------------------------------------------------
*/

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2B0_24SlicesConfBy1Slice =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */       { 0  , CPSS_PA_INVALID_SLICE }
/* PORT 1  */      ,{ 6  , CPSS_PA_INVALID_SLICE }
/* PORT 2  */      ,{ 12 , CPSS_PA_INVALID_SLICE }
/* PORT 3  */      ,{ 18 , CPSS_PA_INVALID_SLICE }
/* PORT 4  */      ,{ 4  , CPSS_PA_INVALID_SLICE }
/* PORT 5  */      ,{ 10 , CPSS_PA_INVALID_SLICE }
/* PORT 6  */      ,{ 16 , CPSS_PA_INVALID_SLICE }
/* PORT 7  */      ,{ 22 , CPSS_PA_INVALID_SLICE }
/* PORT 8  */      ,{ 2  , CPSS_PA_INVALID_SLICE }
/* PORT 9  */      ,{ 8  , CPSS_PA_INVALID_SLICE }
/* PORT 10 */      ,{ 14 , CPSS_PA_INVALID_SLICE }
/* PORT 11 */      ,{ 20 , CPSS_PA_INVALID_SLICE }
                }
};

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2B0_24SlicesConfBy2Slices =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */         { 0  , 6  , CPSS_PA_INVALID_SLICE }
/* PORT 1  */        ,{ CPSS_PA_INVALID_SLICE }
/* PORT 2  */        ,{ 12 , 18 , CPSS_PA_INVALID_SLICE }
/* PORT 3  */        ,{ CPSS_PA_INVALID_SLICE }
/* PORT 4  */        ,{ 4  , 10 , CPSS_PA_INVALID_SLICE }
/* PORT 5  */        ,{ CPSS_PA_INVALID_SLICE }
/* PORT 6  */        ,{ 16 , 22 , CPSS_PA_INVALID_SLICE }
/* PORT 7  */        ,{ CPSS_PA_INVALID_SLICE }
/* PORT 8  */        ,{ 2  , 8  , CPSS_PA_INVALID_SLICE }
/* PORT 9  */        ,{ CPSS_PA_INVALID_SLICE }
/* PORT 10 */        ,{ 14 , 20 , CPSS_PA_INVALID_SLICE }
/* PORT 11 */        ,{ CPSS_PA_INVALID_SLICE }
                }
};

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2B0_24SlicesConfBy4Slices =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */        { 0  , 6  , 12 , 18 , CPSS_PA_INVALID_SLICE }
/* PORT 1  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 2  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 3  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 4  */       ,{ 4  , 10 , 16 , 22 , CPSS_PA_INVALID_SLICE }
/* PORT 5  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 6  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 7  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 8  */       ,{ 2  , 8  , 14 , 20 , CPSS_PA_INVALID_SLICE }
/* PORT 9  */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 10 */       ,{ CPSS_PA_INVALID_SLICE }
/* PORT 11 */       ,{ CPSS_PA_INVALID_SLICE }
                }
};


const CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC g_lion2_B0_24Slices =
{
/* portNum     */  12
/* maxSliceNum */ ,24
/* pizzaCfg    */ ,{
                        {1 , (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_24SlicesConfBy1Slice  }
                       ,{2 , (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_24SlicesConfBy2Slices }
                       ,{4 , (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_24SlicesConfBy4Slices }
                       ,{0 ,  NULL                                                              }
                   }
};

const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2B0Clock360MHz12Ports24Sllices =
{
    /* portNum        */  12
    /* maxSliceNum    */ ,12
    /* txQDef         */ ,{
    /* TXQ_repetition */     10
    /*    TxQDef      */    ,{
    /*       CPU TYPE */         CPSS_DXCH_CPUPORT_TYPE_FIXED_E
    /*       CPU Port */        ,15
    /*       Slices   */        ,{44, CPSS_PA_INVALID_SLICE, 0, 0, 0 } /* */
    /*                */     }
    /*                */  }
    ,(CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)&g_lion2_A0_12Slices  /*&g_lion2_B0_24Slices*/
    ,NULL
    ,(CPSS_DXCH_PORT_PRIORITY_MATR_STC *)&portPriorityLion2_12ports_12_24_slices
};

/*-------------------------------------------------------
** Lion 2 B0 480MHz 32 Slices configs
**-------------------------------------------------------
*/

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2B0_32SlicesConfRegModeBy1Slice =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */         {  0 , CPSS_PA_INVALID_SLICE }
/* PORT 1  */        ,{ 16 , CPSS_PA_INVALID_SLICE }
/* PORT 2  */        ,{  8 , CPSS_PA_INVALID_SLICE }
/* PORT 3  */        ,{ 24 , CPSS_PA_INVALID_SLICE }
/* PORT 4  */        ,{  4 , CPSS_PA_INVALID_SLICE }
/* PORT 5  */        ,{ 20 , CPSS_PA_INVALID_SLICE }
/* PORT 6  */        ,{ 28 , CPSS_PA_INVALID_SLICE }
/* PORT 7  */        ,{ 12 , CPSS_PA_INVALID_SLICE }
/* PORT 8  */        ,{  2 , CPSS_PA_INVALID_SLICE }
/* PORT 9  */        ,{ 6  , CPSS_PA_INVALID_SLICE }
/* PORT 10 */        ,{ 10 , CPSS_PA_INVALID_SLICE }
/* PORT 11 */        ,{ 26 , CPSS_PA_INVALID_SLICE }
                }
};

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2B0_32SlicesConfExtModeBy1Slice =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */         {  0 , CPSS_PA_INVALID_SLICE }
/* PORT 1  */        ,{ 16 , CPSS_PA_INVALID_SLICE }
/* PORT 2  */        ,{  8 , CPSS_PA_INVALID_SLICE }
/* PORT 3  */        ,{ 24 , CPSS_PA_INVALID_SLICE }
/* PORT 4  */        ,{  4 , CPSS_PA_INVALID_SLICE }
/* PORT 5  */        ,{ 20 , CPSS_PA_INVALID_SLICE }
/* PORT 6  */        ,{ 28 , CPSS_PA_INVALID_SLICE }
/* PORT 7  */        ,{ 12 , CPSS_PA_INVALID_SLICE }
/* PORT 8  */        ,{  2 , CPSS_PA_INVALID_SLICE }
/* PORT 9  */        ,{  6 , CPSS_PA_INVALID_SLICE }
/* PORT 10 */        ,{ 10 , CPSS_PA_INVALID_SLICE }
/* PORT 11 */        ,{ 14 , CPSS_PA_INVALID_SLICE }
                }
};



const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2B0_32SlicesConfBy2Slices =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */       { 0  , 16  , CPSS_PA_INVALID_SLICE }
/* PORT 1  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 2  */      ,{ 8  , 24 , CPSS_PA_INVALID_SLICE }
/* PORT 3  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 4  */      ,{ 4  , 20 , CPSS_PA_INVALID_SLICE }
/* PORT 5  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 6  */      ,{ 12 , 28 , CPSS_PA_INVALID_SLICE }
/* PORT 7  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 8  */      ,{ 2  , 18 , CPSS_PA_INVALID_SLICE }
/* PORT 9  */      ,{ 6  , 22 , CPSS_PA_INVALID_SLICE }
/* PORT 10 */      ,{ 10 , 26 , CPSS_PA_INVALID_SLICE }
/* PORT 11 */      ,{ 14 , 30 , CPSS_PA_INVALID_SLICE }
                }
};

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2B0_32SlicesConfBy4Slices =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */       { 0  , 8  , 16 , 24 , CPSS_PA_INVALID_SLICE }
/* PORT 1  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 2  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 3  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 4  */      ,{ 4  , 12 , 20 , 28 , CPSS_PA_INVALID_SLICE }
/* PORT 5  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 6  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 7  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 8  */      ,{ 2  , 10 , 18 , 26 , CPSS_PA_INVALID_SLICE }
/* PORT 9  */      ,{ 6  , 14 , 22 , 30 , CPSS_PA_INVALID_SLICE }
/* PORT 10 */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 11 */      ,{ CPSS_PA_INVALID_SLICE }
                }
};

const CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC pizzaLion2B0_32SlicesConfBy10Slices =
{
/* maxPortNum*/ 12
/* portArr   */,{
/* PORT 0  */       { 0  , 1  , 2  , 4  , 6  , 8  , 9  , 10 , 12 , 14, CPSS_PA_INVALID_SLICE }
/* PORT 1  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 2  */      ,{ CPSS_PA_INVALID_SLICE }
/* PORT 3  */      ,{ CPSS_PA_INVALID_SLICE }
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



/*-------------------------------------------------------------------------------*/
/*  Lion2 B0 Pizza configuration                                                 */
/*-------------------------------------------------------------------------------*/

const CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC g_lion2_B0_32Slices_RegMode =
{
/* portNum     */  12
/* maxSliceNum */ ,32
/* pizzaCfg    */ ,{
                       {1 , (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_32SlicesConfRegModeBy1Slice }
                      ,{2 , (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_32SlicesConfBy2Slices       }
                      ,{4 , (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_32SlicesConfBy4Slices       }
                      ,{0 , NULL                                                                    }
                   }
};


const CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC g_lion2_B0_32Slices_ExtMode =
{
/* portNum     */  12
/* maxSliceNum */ ,32
/* pizzaCfg    */ ,{
                       {1 , (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_32SlicesConfExtModeBy1Slice }
                      ,{2 , (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_32SlicesConfBy2Slices       }
                      ,{4 , (CPSS_DXCH_PORT_PIZZA_CFG_PORT_SLICE_LIST_STC *)&pizzaLion2B0_32SlicesConfBy4Slices       }
                      ,{0 , NULL                                                                    }
                   }
};


const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2B0Clock480MHz12Ports32Sllices =
{
    /* portNum        */  12
    /* maxSliceNum    */ ,16
    /* txQDef         */ ,{
    /* TXQ_repetition */     7
    /*    TxQDef      */    ,{
    /*       CPU TYPE */         CPSS_DXCH_CPUPORT_TYPE_EXTERNAL_E
    /*       CPU Port */        ,15
    /*       Slices   */        ,{CPSS_PA_INVALID_SLICE, 0, 0, 0, 0 }
    /*                */     }
    /*                */  }
    ,(CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)&g_lion2_A0_16Slices_RegMode /* &g_lion2_B0_32Slices_RegMode, */
    ,(CPSS_DXCH_PORT_PIZZA_SLICENUM_TO_SLICE_LIST_STC *)&g_lion2_A0_16Slices_ExtMode /* &g_lion2_B0_32Slices_ExtMode, */
    ,(CPSS_DXCH_PORT_PRIORITY_MATR_STC *)&portPriorityLion2_12ports_16_32_slices
};



/*------------------------------------------------*/
/*  default profile tables                        */
/*------------------------------------------------*/
extern const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2A0Clock360MHz12Ports12Sllices;
extern const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2A0Clock480MHz12Ports16Sllices;
extern const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2B0Clock210MHz7x10G;
extern const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2B0Clock240MHz8x10G;
extern const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2B0Clock270MHz9x10G;
extern const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2B0Clock360MHz12Ports24Sllices;
extern const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2B0Clock480MHz12Ports32Sllices;
extern const CPSS_DXCH_PIZZA_PROFILE_STC  pizzaProfileLion2B0Clock360MHz12Ports30Sllices;


const DevRevTypeClockPizza_STC prvDevRevClockPizzaArr[] =
{
      /* family                        rev  clock             PizzaConfig                                   */
      { CPSS_PP_FAMILY_DXCH_LION2_E , PRV_CPSS_DXCH_LION2_REV_A0, 360,  /* out*/ (CPSS_DXCH_PIZZA_PROFILE_STC *)&pizzaProfileLion2A0Clock360MHz12Ports12Sllices }
     ,{ CPSS_PP_FAMILY_DXCH_LION2_E , PRV_CPSS_DXCH_LION2_REV_A0, 480,  /* out*/ (CPSS_DXCH_PIZZA_PROFILE_STC *)&pizzaProfileLion2A0Clock480MHz12Ports16Sllices }
     ,{ CPSS_PP_FAMILY_DXCH_LION2_E , PRV_CPSS_DXCH_LION2_REV_B0, 210,  /* out*/ (CPSS_DXCH_PIZZA_PROFILE_STC *)&pizzaProfileLion2B0Clock210MHz7x10G            }
     ,{ CPSS_PP_FAMILY_DXCH_LION2_E , PRV_CPSS_DXCH_LION2_REV_B0, 240,  /* out*/ (CPSS_DXCH_PIZZA_PROFILE_STC *)&pizzaProfileLion2B0Clock240MHz8x10G            }
     ,{ CPSS_PP_FAMILY_DXCH_LION2_E , PRV_CPSS_DXCH_LION2_REV_B0, 270,  /* out*/ (CPSS_DXCH_PIZZA_PROFILE_STC *)&pizzaProfileLion2B0Clock270MHz9x10G            }
     ,{ CPSS_PP_FAMILY_DXCH_LION2_E , PRV_CPSS_DXCH_LION2_REV_B0, 360,  /* out*/ (CPSS_DXCH_PIZZA_PROFILE_STC *)&pizzaProfileLion2B0Clock360MHz12Ports24Sllices }
     ,{ CPSS_PP_FAMILY_DXCH_LION2_E , PRV_CPSS_DXCH_LION2_REV_B0, 480,  /* out*/ (CPSS_DXCH_PIZZA_PROFILE_STC *)&pizzaProfileLion2B0Clock480MHz12Ports32Sllices }
};


const ListDevRevTypeClockPizza_STC prvListDevRevTypeClock2PizzaDefault =
{
     sizeof(prvDevRevClockPizzaArr)/sizeof(prvDevRevClockPizzaArr[0])
    ,(DevRevTypeClockPizza_STC *)&prvDevRevClockPizzaArr[0]
};

