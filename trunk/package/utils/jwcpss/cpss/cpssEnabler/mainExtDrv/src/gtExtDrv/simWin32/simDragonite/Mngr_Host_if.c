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


//////////////////////////////////////////
// This file holds definitions that should
// be shared between Dragonite & Host.
// It defines the shared memory of the
// communication msg & msg types version
// & length.
//////////////////////////////////////////
#include "mngr_host_if.h"
//****************************************************************************
//                         GLOBAL VARS                                       
//****************************************************************************
t_HostMsg InputMsg;


//this const array has to be updated for each msg type
//first field is the size of the message without the header 
//and the second one is the version
const TypeData MsgTypeData[] = 
{
  {sizeof(t_config), 0},//Config
  {sizeof(t_system), 0},//System
  {sizeof(t_chip), 0},//Chip
  {sizeof(t_port_commands), 0},//Port commands
  {sizeof(t_port_counters), 0}, //Port counters
  {sizeof(PORT_STAT_REGS), 0}, //Port statuses
  {sizeof(PORT_MSR_REGS), 0}, //Port measurments
  {sizeof(PORT_L2_REGS), 0}, //Layer 2
  {sizeof(DEBUG_REGS), 0}, //Debug
  {sizeof(OTHER_REGS), 0} //Other
};


