/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsPortApIf.c
*
* DESCRIPTION:
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>

#include <private/mvHwsPortApInitIfPrv.h>
#include <private/mvHwsPortApCoCpuIf.h>
#include <private/mvPortModeElements.h>
#include <serdes/mvHwsSerdesPrvIf.h>

extern void mvGetZ80CodeAttr(GT_U32 **code, GT_U32 *size);

MV_HWS_GET_Z80_CODE_FUNC_PTR mvHwsZ80CodeGetFunc = mvGetZ80CodeAttr;


#ifdef DD_SIM  /* run AP engine on main CPU */
#include <common/os/gtOs.h>
#include <gtOs/gtOsTask.h>

GT_U8 taskParams;
extern unsigned __TASKCONV apMainLoop(void *);
extern unsigned __TASKCONV mytimerIsrTask(void *);

static GT_TASK      timerTid;
static GT_TASK      apLoopTid;
#else
#endif

GT_UINTPTR mvApGetSharedMemPtr(GT_U8 devNum, GT_U32 portGroup)
{
  GT_U32 data;
  /* read address of Global Data Structure from Z80 header in Confi Processor memory at 0x100 */
  genRegisterGet(devNum, portGroup, CO_PROC_MEMORY_BASE+0x100, &data, 0);
  data = 0x7000;
  return (GT_UINTPTR)data;
}

void mvApSetSharedMem(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 data)
{
  genRegisterSet(devNum, portGroup, CO_PROC_MEMORY_BASE+address, data, 0);
}

void mvApGetSharedMem(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 *data)
{
  genRegisterGet(devNum, portGroup, CO_PROC_MEMORY_BASE+address, data, 0);
}

void mvApFwRun(GT_U8 devNum, GT_U32 portGroup)
{
  GT_U32 data;

#ifndef DD_SIM
  unsigned int i;
  GT_U32 z80MemBase = CO_PROC_MEMORY_BASE;
  GT_U32 regAddr;
    GT_U32 size;
    GT_U32 *apZ80Code;
#endif

  /* enable Confi Processor memory */
  genRegisterGet(devNum, portGroup, Confi_Processor_Global_Cfg, &data, 0);
  U32_SET_FIELD(data, 19, 1, 0);
  genRegisterSet(devNum, portGroup, Confi_Processor_Global_Cfg, data, 0);
  /* enable Confi Processor */
  genRegisterGet(devNum, portGroup, Extended_Global_Control, &data, 0);
  U32_SET_FIELD(data, 6, 2, 2);
  genRegisterSet(devNum, portGroup, Extended_Global_Control, data, 0);

#ifdef DD_SIM
  taskParams = devNum;
  osTaskCreate("z80Loop", 80, 0, apMainLoop, devNum, &apLoopTid);
  osTaskCreate("timerIsr", 90, 0, mytimerIsrTask, devNum, &timerTid);

  return;
#else

  /* copy code to memory */
  mvHwsZ80CodeGetFunc(&apZ80Code, &size);
    
  for (i = 0; i < size; i++)
  {
    regAddr = z80MemBase + i*4;
    genRegisterSet(devNum, portGroup, regAddr, apZ80Code[i], 0);
  }

  /* start Confi Processor */
  genRegisterGet(devNum, portGroup, Confi_Processor_Global_Cfg, &data, 0);
  U32_SET_FIELD(data, 21, 1, 1);
  genRegisterSet(devNum, portGroup, Confi_Processor_Global_Cfg, data, 0);
#endif
}

void mvApFwStop(GT_U8 devNum, GT_U32 portGroup)
{
#ifdef DD_SIM
  osTaskDelete(timerTid);
  osTaskDelete(apLoopTid);
  return;
#else
  GT_U32 data;

  /* stop Confi Processor */
  genRegisterGet(devNum, portGroup, Confi_Processor_Global_Cfg, &data, 0);
  U32_SET_FIELD(data, 21, 1, 0);
  genRegisterSet(devNum, portGroup, Confi_Processor_Global_Cfg, data, 0);

#endif
}

/* for DEBUG only */
void mvApFwZ80Run(GT_U8 devNum)
{
  unsigned int i;
  GT_U32 z80MemBase = CO_PROC_MEMORY_BASE;
  GT_U32 regAddr, data;
    GT_U32 size;
    GT_U32 *apZ80Code;

  /* enable Confi Processor memory */
  genRegisterGet(devNum, devNum, Confi_Processor_Global_Cfg, &data, 0);
  U32_SET_FIELD(data, 19, 1, 0);
  genRegisterSet(devNum, devNum, Confi_Processor_Global_Cfg, data, 0);

  /* enable Confi Processor */
  genRegisterGet(devNum, devNum, Extended_Global_Control, &data, 0);
  U32_SET_FIELD(data, 6, 2, 2);
  genRegisterSet(devNum, devNum, Extended_Global_Control, data, 0);

  /* copy code to memory */
  mvHwsZ80CodeGetFunc(&apZ80Code, &size);
 
  for (i = 0; i < size; i++)
  {
    regAddr = z80MemBase + i*4;
    genRegisterSet(devNum, devNum, regAddr, apZ80Code[i], 0);
  }

  /* start Confi Processor */
  genRegisterGet(devNum, devNum, Confi_Processor_Global_Cfg, &data, 0);
  U32_SET_FIELD(data, 21, 1, 1);
  genRegisterSet(devNum, devNum, Confi_Processor_Global_Cfg, data, 0);
}
