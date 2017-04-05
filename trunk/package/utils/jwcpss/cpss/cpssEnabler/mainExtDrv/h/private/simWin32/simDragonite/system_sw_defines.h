/*===============================================================================================*/
/*  THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF                                               */
/*  MICROSEMI COP.- ANALOG MIXED SIGNAL GROUP LTD. ("AMSG")                                      */
/*  AND IS SUBJECT TO A NON-DISCLOSURE AGREEMENT                                                 */
/*  NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT,                                               */
/*  MASK WORK RIGHT OR COPYRIGHT OF AMSG OR ANY THIRD PARTY.                                     */
/*  AMSG RESERVES THE RIGHT AT ITS SOLE                                                          */
/*  DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO AMSG.                        */
/*  THIS CODE IS PROVIDED "AS IS". AMSG MAKES NO WARRANTIES, EXPRESSED,                          */
/*  IMPLIEDOR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.                    */
/*  THIS SOURCE CODE, MAY NOT BE DISCLOSED TO ANY THIRD PARTY OR USED IN ANY OTHER MANNER,       */
/*  AND KNOWLEDGE DERIVED THEREFROM OR CANNOT BE USED TO WRITE ANY PROGRAM OR CODE.              */
/*  USE IS PERMITTED ONLY PURSUANT TO WRITTEN AGREEMENT SIGNED BY AMSG.                          */
/*  KNOWLEDGE OF THIS FILE MAY UNDER NO CIRCUMSTANCES BE USED TO WRITE ANY PROGRAM.              */
/*===============================================================================================*/
//-----------------------------------------------------------------------------
// Title      :  system_sw_defines.h
// Project    :  system
// File        : system_sw_defines 
// Author      : YakiD 
// Created     : Mon Jan  1 09:15:41     2007
// Last update : //
//-----------------------------------------------------------------------------

//#############################################################################
//                   !!!!!!!!! DO NOT CHANGE !!!!!!!!!
//#############################################################################
#ifndef _SYSTEM_DEF_H
#define _SYSTEM_DEF_H

#ifdef _VISUALC
    /* Disable warning messages */
    #pragma warning( disable : 4214 )
#endif /* _VISUALC */

typedef struct
{
// #############################################################
// Register SystemMask0
// 
// #############################################################
U32 SystemMask0;
// #############################################################
// Register ActiveBudget
// reset value : ActiveBudget = 00000000000000
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 ActiveBudget : 4;
      U32 Vmain : 10;
      U32 reserved : 18;
  }Bits;
}t_ActiveBudget;
// #############################################################
// Register PowerBudget0
// reset value : PowerBudget0 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget0;
// #############################################################
// Register PowerBudget1
// reset value : PowerBudget1 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget1;
// #############################################################
// Register PowerBudget2
// reset value : PowerBudget2 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget2;
// #############################################################
// Register PowerBudget3
// reset value : PowerBudget3 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget3;
// #############################################################
// Register PowerBudget4
// reset value : PowerBudget4 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget4;
// #############################################################
// Register PowerBudget5
// reset value : PowerBudget5 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget5;
// #############################################################
// Register PowerBudget6
// reset value : PowerBudget6 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget6;
// #############################################################
// Register PowerBudget7
// reset value : PowerBudget7 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget7;
// #############################################################
// Register PowerBudget8
// reset value : PowerBudget8 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget8;
// #############################################################
// Register PowerBudget9
// reset value : PowerBudget9 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget9;
// #############################################################
// Register PowerBudget10
// reset value : PowerBudget10 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget10;
// #############################################################
// Register PowerBudget11
// reset value : PowerBudget11 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget11;
// #############################################################
// Register PowerBudget12
// reset value : PowerBudget12 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget12;
// #############################################################
// Register PowerBudget13
// reset value : PowerBudget13 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget13;
// #############################################################
// Register PowerBudget14
// reset value : PowerBudget14 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget14;
// #############################################################
// Register PowerBudget15
// reset value : PowerBudget15 = 011111111111111111
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget15;
// #############################################################
// Register SystemErrorFlags
// reset value : SystemErrorFlags = 000000
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 VmainHigh : 1;
      U32 VmainLowAT : 1;
      U32 VmainLowAF : 1;
      U32 TempAlarm : 1;
      U32 OverTemp : 1;
      U32 DisablePortsActive : 1;
      U32 reserved : 26;
  }Bits;
}t_SystemErrorFlags;
// #############################################################
// Register OSstatus
// 
// #############################################################
U32 OSstatus;
// #############################################################
// Register SystemChipExist
// reset value : SystemChipExist = 000000000000000000000000
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Chip0 : 2;
      U32 Chip1 : 2;
      U32 Chip2 : 2;
      U32 Chip3 : 2;
      U32 Chip4 : 2;
      U32 Chip5 : 2;
      U32 Chip6 : 2;
      U32 Chip7 : 2;
      U32 Chip8 : 2;
      U32 Chip9 : 2;
      U32 Chip10 : 2;
      U32 Chip11 : 2;
      U32 reserved : 8;
  }Bits;
}t_SystemChipExist;
// #############################################################
// Register SysTotalCriticalCons
// 
// #############################################################
U32 SysTotalCriticalCons;
// #############################################################
// Register SysTotalHighCons
// 
// #############################################################
U32 SysTotalHighCons;
// #############################################################
// Register SysTotalLowCons
// 
// #############################################################
U32 SysTotalLowCons;
// #############################################################
// Register SysTotalCriticalReq
// 
// #############################################################
U32 SysTotalCriticalReq;
// #############################################################
// Register SysTotalHighReq
// 
// #############################################################
U32 SysTotalHighReq;
// #############################################################
// Register SysTotalLowReq
// 
// #############################################################
U32 SysTotalLowReq;
// #############################################################
// Register SysTotalCalcPowerCons
// 
// #############################################################
U32 SysTotalCalcPowerCons;
// #############################################################
// Register SysTotalPowerRequest
// 
// #############################################################
U32 SysTotalPowerRequest;
// #############################################################
// Register SysTotalDeltaPower
// 
// #############################################################
U32 SysTotalDeltaPower;
// #############################################################
// Register SysTotalRealPowerCons
// 
// #############################################################
U32 SysTotalRealPowerCons;
// #############################################################
// Register ProductInfo
// reset value : ProductInfo = 
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 Minor : 16;
      U32 Major : 16;
  }Bits;
}t_ProductInfo;
// #############################################################
// Register BuildDateandTime0
// 
// #############################################################
U32 BuildDateandTime0;
// #############################################################
// Register BuildDateandTime1
// 
// #############################################################
U32 BuildDateandTime1;
// #############################################################
// Register BuildDateandTime2
// 
// #############################################################
U32 BuildDateandTime2;
// #############################################################
// Register BuildDateandTime3
// 
// #############################################################
U32 BuildDateandTime3;
// #############################################################
// Register BuildDateandTime4
// 
// #############################################################
U32 BuildDateandTime4;
// #############################################################
// Register CheckSumErr_Counter
// 
// #############################################################
U32 CheckSumErr_Counter;
// #############################################################
// Register LengthErr_Counter
// 
// #############################################################
U32 LengthErr_Counter;
// #############################################################
// Register StructVerErr_Counter
// 
// #############################################################
U32 StructVerErr_Counter;
// #############################################################
// Register TypeErr_Counter
// 
// #############################################################
U32 TypeErr_Counter;
// #############################################################
// Register ConfigErr_Counter
// 
// #############################################################
U32 ConfigErr_Counter;
// #############################################################
// Register IRQErr_Counter
// 
// #############################################################
U32 IRQErr_Counter;

}t_system;

#endif
