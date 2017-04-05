//////////////////////////////////////////////////////////////////////////////////////////////////////////
//THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MICROSEMI COP.- ANALOG MIXED SIGNAL GROUP LTD. ("AMSG") 
//AND IS SUBJECT TO A NON-DISCLOSURE AGREEMENT
//NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF AMSG OR ANY THIRD PARTY. 
//AMSG RESERVES THE RIGHT AT ITS SOLE 
//DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO AMSG.  
//THIS CODE IS PROVIDED "AS IS". AMSG MAKES NO WARRANTIES, EXPRESSED, IMPLIEDOR OTHERWISE, 
//REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.  
//THIS SOURCE CODE, MAY NOT BE DISCLOSED TO ANY THIRD PARTY OR USED IN ANY OTHER MANNER, 
//AND KNOWLEDGE DERIVED THEREFROM OR CANNOT BE USED TO WRITE ANY PROGRAM OR CODE. 
//USE IS PERMITTED ONLY PURSUANT TO WRITTEN AGREEMENT SIGNED BY AMSG.
//KNOWLEDGE OF THIS FILE MAY UNDER NO CIRCUMSTANCES BE USED TO WRITE ANY PROGRAM.
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef _COMM_IRQ_H_
#define _COMM_IRQ_H_

#ifdef _VISUALC
    /* Disable warning messages */
    #pragma warning( disable : 4214 )
#endif /* _VISUALC */

#include "Globals.h"

//****************************************************************************
//                         DEFINITIONS                                       
//****************************************************************************
#define COMM_WRITE_FLAG     IrqEvent.t_IRQ_Cause_Out.Bits.IRQ_Write_Event
#define COMM_READ_FLAG      IrqEvent.t_IRQ_Cause_Out.Bits.IRQ_Read_Event
#define INT_MO_FLAG         IrqData.t_IRQ_Cause_Out.Bits.IMO_bit
#define INT_POLARITY        IrqData.t_IRQ_Mask.Bits.IRQ_Polarity
#define INT_FLAG            IrqData.IRQ_Flag
#define INT_MASK            IrqData.t_IRQ_Mask.u32
#define COMM_INT_LIST       IrqCause.t_IRQ_Cause_Out.u32
#define INT_LIST4HOST       IrqData.t_IRQ_Cause_Out.u32

//actual irq shared memory
extern t_irq IrqData;
//instead of output pin@@@@@@
extern  U32                 IRQTOHOST;
//for use of dragonite
extern t_irq                IrqEvent;
extern t_irq                IrqCause;
//****************************************************************************
//                         PROTOTYPES                                       
//****************************************************************************
void HandleInterruptToHost(void); 
void InitCommIrq(void);
#endif
