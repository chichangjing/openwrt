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
#ifndef __COMM_DB_IF_H_
#define __COMM_DB_IF_H_

#include "Globals.h"

//except for system, all other groups are now handled as an array
typedef union
{
    t_config            Config;
    t_system            System;
    t_chip              Chip;
    t_port_commands     PortCommands;
    t_port_counters     PortCounters;
    t_port_statuses     PortStatuses;
    t_port_measurements PortMeasurements;
    t_port_layer2       PortLayer2;
    t_irq               Irq;
    U32 DebugReg[1];
    U32 OtherReg[1];
} t_Comm_db;


//the actual register memory
extern t_Comm_db DB_for_Comm;

//****************************************************************************
//                         DEFINITIONS                                       
//****************************************************************************
#define DRAGONITE_MNGR_DB DB_for_Comm
#define CONFIG_REGS     DRAGONITE_MNGR_DB.Config
#define SYSTEM_REGS     DRAGONITE_MNGR_DB.System
#define CHIP_REGS       DRAGONITE_MNGR_DB.Chip
#define PORT_CMD_REGS   DRAGONITE_MNGR_DB.PortCommands
#define PORT_CNT_REGS   DRAGONITE_MNGR_DB.PortCounters
#define PORT_STAT_REGS  DRAGONITE_MNGR_DB.PortStatuses
#define PORT_MSR_REGS   DRAGONITE_MNGR_DB.PortMeasurements
#define PORT_L2_REGS    DRAGONITE_MNGR_DB.PortLayer2
#define DEBUG_REGS      DRAGONITE_MNGR_DB.DebugReg
#define OTHER_REGS      DRAGONITE_MNGR_DB.OtherReg

#define IRQ_ERROR     DRAGONITE_MNGR_DB.System.IRQErr_Counter   
#define COMM_ERROR(x) (U32*)(&DRAGONITE_MNGR_DB.System.CheckSumErr_Counter)[x]

#endif
