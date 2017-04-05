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
//=============================================================================
//      File name : Globals.h
//      Description : this file holds all the global definitions for the
//                    project, including some global function prototypes
//                    which are implemented in Globals.c
//      Author : Avital Netzer
//=============================================================================
#ifndef __GLOBALS_H_
#define __GLOBALS_H_

#include <string.h>

#ifndef DRAGONITE_TYPE_A1

#include "RTOS.H"
#include <stdbool.h>
#include <global_def.h>
#include <hal_cpu_reg.h>
#include <hal_global_def.h>
#include <hal_gen.h>
#include <hal_spi.h>
//#include "mvSpi.h"
#include "Rotem_Espi.h"
#include "TaskMisc.h"
#include "Rotem_IF.h"
#include "watchdog.h"
#include "Rotem_sw_defines_customer.h"
#include "TaskMaster.h"

#else

#include "InsteadOfOS.h"

#endif 

#include "chip_sw_defines.h"
#include "config_sw_defines.h"
#include "irq_sw_defines.h"
#include "Comm_irq.h"
#include "port_commands_sw_defines.h"
#include "port_counters_sw_defines.h"
#include "port_layer2_sw_defines.h"
#include "port_measurements_sw_defines.h"
#include "port_statuses_sw_defines.h"
#include "system_sw_defines.h"
#include "Mngr_Host_if.h"
#include "MCom.h"

#ifdef DRAGONITE_TYPE_A1
#include "Comm_db_if.h"
#endif 

#define TRUE 	1
#define FALSE	0

#ifndef DRAGONITE_TYPE_A1

extern const U8 BuildDate[];
extern const U8 BuildTime[];


typedef union
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
}t_SlaveStates;

//chip state enumeration
enum{
	e_ChipDoesNotExist,
	e_ChipExist,
	e_Reserved,
	e_ChipError
};

//PM calc method
enum{
	e_Static_PM,
	e_Dinamic_PM
};
//active slave list structure
extern t_SlaveStates _ActiveSlaveList;

#define MAX_SLAVES              8
extern U8   NumOfActiveSlaves;
#define NUM_ACTIVE_SLAVES NumOfActiveSlaves
#define SLAVE_DET_WORD      0xAA55
#define ESPI_BROADCAST_ADD  0xE
#define MODE_CHANGE_KEY_WORD 0xDC00
#define MANUAL_MODE_INDEX   15

//SPI config definitions
//set SPI clock divider to division by 8 (200mHz / 8 = 25mHz)
//and set the SPI clock prescaler to divide by 20 (25mHz / 20 = 1.25mHz)
#define XCAT_SPI_INIT_VAL   0x0000181a
#define XCAT_SPI_INIT_MASK  0x0000181f
//DRAGONITE GPP constants
#define	MV_DRAGONITE_GPP_CTRL_REG				0x00000010
#define	MV_DRAGONITE_GPP0_CTRL_MASK  			0x00000001
#define	MV_DRAGONITE_GPP1_CTRL_MASK  			0x00000002
#define	MV_DRAGONITE_GPP0_OUT_MASK  			0x00000004
#define	MV_DRAGONITE_GPP1_OUT_MASK  			0x00000008
//task misc cycle counter clear
#define TASK_MISC_CYCLE_CLEAR   10

#define NUM_PORTS_FUSE_MASK	0x0010
extern U8 ChipNumberOfPorts[MAX_SLAVES];
#define CHIP_NUM_PORTS(x)	ChipNumberOfPorts[x]

enum
{
    e_StandAlone = 0,
    e_MacroMode,
    e_ManualMode,
    e_ConfigMode
};

//flag indicating wheather the problem ports list should be cleared or not
extern bool ClearProblemPortsFlag;
#define CLEAR_PROBLEM_PORTS_FLAG  ClearProblemPortsFlag

//global master parameters
extern t_config	gConfig;
extern t_system    gSystem;

//PM mode field
#define PM_MODE	gConfig.t_PM_Mode.Bits.PM_CalcMode
//system power budgets
typedef union
{
  U32 u32;
  struct
  {
      U32 Available_Power : 16;
      U32 PowerSourceType : 2;
      U32 reserved : 14;
  }Bits;
}t_PowerBudget;
//power budgets array
#define NUM_BUDGETS         8
#define SYS_POWER_BUDGET(x) ((t_PowerBudget*)&gSystem.t_PowerBudget0)[x].Bits.Available_Power
//message maximum values
#define MAX_PORT_POWER		320		//32W
#define MAX_ICUT_VAL		177		//863mA
#define MIN_TEMP_VAL		0x184	//155 DegC
#define MAX_VMAIN_VAL		0x3d8	//60V
#define MIN_VMAIN_VAL		0x28f	//40V
#define MAX_GB_VAL			255		//25.5W
#define STARTUP_CR_MASK 	0x3f
#define PS_TYPE_RESERVED	3
//current budget variable read from slave
#define CURRENT_BUDGET  gSystem.t_ActiveBudget.Bits.ActiveBudget
typedef enum
{
    e_PriorityCritical = 0,
    e_PriorityHigh,
    e_PriorityLow
}e_PortPriority;
#define NUM_PRIORITY_LEVELS 3
//slaves power consumption (for master use) - index is slave number and priority level
#define SLAVE_POWER_CONS(slave,priority) (SlaveParams[slave].PowerCons.PriorityCons[priority])
//slave total power consumption for the RLPM
extern U16 SlaveTotalPowerConsArr[MAX_SLAVES];
#define TOTAL_SLAVE_POWER_CONS(x)   SlaveTotalPowerConsArr[x]
//slave total power consumption
#define SLAVE_TOTAL_REAL_CONS(x)   (SlaveParams[x].PowerCons.RealCons)
#define SLAVE_TOTAL_CALC_CONS(x)   (SlaveParams[x].PowerCons.CalcCons)
extern U16 SlaveBudgets[NUM_BUDGETS][MAX_SLAVES];
#define SLAVE_BUDGET(BudgetNo,SlaveNo)   SlaveBudgets[BudgetNo][SlaveNo]
//external Sync constants
#define EXT_SYNC_DETECTION              0x01
#define EXT_SYNC_STARTUP                0x02
#define EXT_SYNC_UPDATE_PB              0x04
#define EXT_SYNC_READ_INDICATIONS       0x08
#define EXT_SYNC_MACRO                  0x10
#define EXT_SYNC_MODE                   0x20
#define EXT_SYNC_INT_OUT                0x40
#define EXT_SYNC_READ_PM_INDICATIONS    0x80
#define EXT_SYNC_MASTER_SYNC            0x100
//temp word for spi
extern volatile U16 SpiTempWord;
#define SPI_TEMP_WORD   SpiTempWord

//=============================================================================
//  global macro's
//=============================================================================
#define SetBit(BitNo,DestWord)   (*DestWord |= (0x0001 << BitNo))
#define ClearBit(BitNo,DestWord) (*DestWord &= ~(0x0001 << BitNo))
#define GetBit(BitNo,SrcWord) (SrcWord & (0x0001U << BitNo))

//=============================================================================
//  Internal Port Parameters
//=============================================================================
#define MAX_NUM_PORTS   96
#define MAX_PORTS_PER_CHIP	12
//Port Control Register
typedef union
{
  U16 Word;
  struct
  {
      U16 pse_enable : 2;
      U16 paircontrol : 2;
      U16 portmode : 2;
      U16 portpriority : 2;
      U16 reserved : 8;
  }Bits;
}tPort_CR;
//Port Status Register
typedef union
{
  U16 Word;
  struct
  {
      U16 internalstatus : 8;
      U16 externalstatus : 3;
      U16 port_at_behavior : 1;
      U16 reserved : 4;
  }Bits;
}tPort_SR;
//Port Class
typedef union
{
  U16 Word;
  struct
  {
      U16 classin1 : 4;
      U16 classin2 : 4;
      U16 classout : 8;
  }Bits;
}tPort_Class;
typedef union
{
  U32 u32;
  struct
  {
      U32 PhysicalPortNumber : 8;
      U32 ChipNumber : 4;
      U32 reserved : 20;
  }Bits;
}t_Matrix_LogicalPort;

extern const t_Matrix_LogicalPort* Matrix;
#define GetLogicalPortNo(x)	Matrix[x].Bits.PhysicalPortNumber
#define	GetLogicalChipNo(x)	Matrix[x].Bits.ChipNumber
//*********************************************
//	Port Parameters structure
//	DO NOT CHANGE THE ORDER OF THESE FIELDS
//	THEY ARE ALIGNED WITH THE ROTEM REG MAP
//*********************************************
typedef struct{
    tPort_CR    CR[MAX_PORTS_PER_CHIP];
    U16         PPL[MAX_PORTS_PER_CHIP];
    U16         TPPL[MAX_PORTS_PER_CHIP];
    tPort_SR    SR[MAX_PORTS_PER_CHIP];
    tPort_Class Class[MAX_PORTS_PER_CHIP];
	U16			LastDisconnect[MAX_PORTS_PER_CHIP];
    U16         InvalidSig_Cnt[MAX_PORTS_PER_CHIP];
    U16         PowerDenied_Cnt[MAX_PORTS_PER_CHIP];
    U16         OVL_Cnt[MAX_PORTS_PER_CHIP];
    U16         UDL_Cnt[MAX_PORTS_PER_CHIP];
    U16         SC_Cnt[MAX_PORTS_PER_CHIP];
    U16         ClassError_Cnt[MAX_PORTS_PER_CHIP];
    U16         Indications[MAX_PORTS_PER_CHIP];
    U16         PD_Request[MAX_PORTS_PER_CHIP];
    U16         PSE_Allocated[MAX_PORTS_PER_CHIP];
    U16         Voltage[MAX_PORTS_PER_CHIP];
    U16         Current[MAX_PORTS_PER_CHIP];
    U16         Power[MAX_PORTS_PER_CHIP];
}tPortsParams;
//Slave power consumption
typedef struct
{
    U16 RealCons;
    U16 CalcCons;
    U16 PriorityCons[NUM_PRIORITY_LEVELS];
}t_SlavePowerCons;
//Slave parameters struct
typedef struct{
    U16					ChipInfo;
    U16					ID;										
    t_SlavePowerCons    PowerCons;
    U16                 Rpr_Ind;
    t_SystemErrorFlags  ErrorFlags;
    U16                 MeasuredTemp;
    U16                 MaxMeasuredTemp;
    U16                 TemperatureAlarm;
	tPortsParams		Ports;
}t_SlaveParams;
extern t_SlaveParams    SlaveParams[MAX_SLAVES];
extern U16 SlavePowerReq[MAX_SLAVES][NUM_PRIORITY_LEVELS];

extern bool PortCommandsReceived;
#define PORT_COMMANDS_RECEIVED PortCommandsReceived

#endif 

#endif
