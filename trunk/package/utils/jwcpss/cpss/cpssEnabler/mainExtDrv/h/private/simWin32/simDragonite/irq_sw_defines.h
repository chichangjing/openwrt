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
// Title      :  irq_sw_defines.h
// Project    :  irq
// File        : irq_sw_defines 
// Author      : YakiD 
// Created     : Mon Jan  1 09:15:41     2007
// Last update : //
//-----------------------------------------------------------------------------

//#############################################################################
//                   !!!!!!!!! DO NOT CHANGE !!!!!!!!!
//#############################################################################
#ifndef _IRQ_DEF_H
#define _IRQ_DEF_H


#ifdef _VISUALC
    /* Disable warning messages */
    #pragma warning( disable : 4214 )
#endif /* _VISUALC */

typedef struct
{
// #############################################################
// Register IRQ_Cause_Out
// reset value : IRQ_Cause_Out = 
// #############################################################

union
{
  U32 u32;
  U32 List4host :31;
  struct
  {
      U32 PortOn : 1;
      U32 PortOff : 1;
      U32 DetectionFailed : 1;
      U32 PortFault : 1;
      U32 PortUDL : 1;
      U32 PortFaultDuringSU : 1;
      U32 PortPM : 1;
      U32 PowerDenied : 1;
      U32 Port_Spare0 : 1;
      U32 Port_Spare1 : 1;
      U32 OverTemp : 1;
      U32 TempAlarm : 1;
      U32 DRAGONITEDeviceFault : 1;
      U32 Device_Spare0 : 1;
      U32 Device_Spare1 : 1;
      U32 OverConsumption : 1;
      U32 VmainLowAF : 1;
      U32 VmainLowAT : 1;
      U32 VmainHigh : 1;
      U32 System_Spare0 : 1;
      U32 System_Spare1 : 1;
      U32 IRQ_Read_Event : 1;
      U32 reserved0 : 3;
      U32 IRQ_Write_Event : 1;
      U32 reserved1 : 5;
      U32 IMO_bit : 1;
  }Bits;
}t_IRQ_Cause_Out;
// #############################################################
// Register IRQ_Mask
// reset value : IRQ_Mask = 
// #############################################################

union
{
  U32 u32;
  struct
  {
      U32 PortOn_Mask : 1;
      U32 PortOff_Mask : 1;
      U32 DetectionFailed_Mask : 1;
      U32 PortFault_Mask : 1;
      U32 PortUDL_Mask : 1;
      U32 PortFaultDuringSU_Mask : 1;
      U32 PortPM_Mask : 1;
      U32 PowerDenied_Mask : 1;
      U32 Port_Spare0_Mask : 1;
      U32 Port_Spare1_Mask : 1;
      U32 OverTemp_Mask : 1;
      U32 TempAlarm_Mask : 1;
      U32 DRAGONITEDeviceFault_Mask : 1;
      U32 Device_Spare0_Mask : 1;
      U32 Device_Spare1_Mask : 1;
      U32 OverConsumption_Mask : 1;
      U32 VmainLowAF_Mask : 1;
      U32 VmainLowAT_Mask : 1;
      U32 VmainHigh_Mask : 1;
      U32 System_Spare0_Mask : 1;
      U32 System_Spare1_Mask : 1;
      U32 IRQ_Read_Event_Mask : 1;
      U32 reserved0_Mask : 3;
      U32 IRQ_Write_Event_Mask : 1;
      U32 reserved1_Mask : 5;
      U32 IRQ_Polarity : 1;
  }Bits;
}t_IRQ_Mask;
// #############################################################
// Register IRQ_Flag
// 
// #############################################################
U32 IRQ_Flag;

}t_irq;

#endif
