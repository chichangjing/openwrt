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

/////////////////////////////////////////////
// This file handles the interrupt data, 
// sent from Dragonite to the host.
////////////////////////////////////////////

#include "Comm_irq.h"

t_irq IrqData;

U32 IRQTOHOST;
t_irq IrqEvent;
t_irq IrqCause;

#ifdef DRAGONITE_TYPE_A1

#include "Globals.h"

extern unsigned int simOsInterruptSet
(
    U32 intr
);

U32 intLine;

#endif 

//init dragonite variables  
void InitCommIrq(void)
{
    memset(&IrqEvent, 0, sizeof(t_irq));
    memset(&IrqCause, 0, sizeof(t_irq));

}

void HandleInterruptToHost(void)
{
  //protect from dragonite other tasks
  _ENTER_CRITICAL_SECTION_;
  //save int so far accumulated
  IrqCause.t_IRQ_Cause_Out.u32 |= IrqEvent.t_IRQ_Cause_Out.u32;
  //release critical section
  _EXIT_CRITICAL_SECTION_;
  //clear events
  IrqEvent.t_IRQ_Cause_Out.u32 = 0;
  //is host ready for new irq
  if(INT_MO_FLAG)
  {
    if(IRQ_TO_HOST == INT_POLARITY)
    {
      //in here, irq i/o should be deactivated
      IRQ_TO_HOST = !IRQ_TO_HOST;
      //check that host cleard the register
      if(INT_LIST4HOST & ~0x80000000) 
      {
        //inc error cntr
        IRQ_ERROR++;
      }
    }
    else
    {
	//in here, ready to send new irq data
	//protect from dragonite other tasks
	_ENTER_CRITICAL_SECTION_;
	//copy new events to host data register
      INT_LIST4HOST |= (COMM_INT_LIST & ~0x80000000);
	//clear cause register
	IrqCause.t_IRQ_Cause_Out.u32 = 0;
	//release critical section
	_EXIT_CRITICAL_SECTION_;
	//clear imo if new data is available
	  INT_MO_FLAG = 0;
	//update irq flag
      if(INT_LIST4HOST & (INT_MASK & ~0x80000000))
		INT_FLAG = 1;
	else
		INT_FLAG = 0;
	//irq i/o is !(irq_polarity xor irq_flag)
	IRQ_TO_HOST = !(INT_POLARITY ^ INT_FLAG);
#ifdef DRAGONITE_TYPE_A1
        /* simulate HW interrupt */
        if(IRQ_TO_HOST)
        {
            simOsInterruptSet(intLine + 1); /* increment intLine to fix Host bug */
        }
#endif 
    }
  }
} 
