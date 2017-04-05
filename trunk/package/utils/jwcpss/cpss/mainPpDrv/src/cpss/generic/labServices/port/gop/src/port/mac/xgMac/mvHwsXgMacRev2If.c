/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*
* This software file (the "File") is owned and distributed by Marvell
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.
* If you received this File from Marvell, you may opt to use, redistribute
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.
*  -   Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mvHwsXgMacRev2If.c
*
* DESCRIPTION:
*       XGL MAC revision 2 interface (for puma3)
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <mac/xgMac/mvHwsXgMacRev2If.h>
#include <mac/xlgMac/mvHwsXlgMacDb.h>
#include <mac/xgMac/mvHwsXgMacDb.h>


static char* mvHwsMacTypeGet(void)
{
  return "XGMAC_Rev2";
}
static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* hwsXgMacIfInit
*
* DESCRIPTION:
*       Init XG MAC configuration sequences and IF functions.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS hwsXgMacRev2IfInit(GT_U8 devNum, MV_HWS_MAC_FUNC_PTRS *funcPtrArray)
{
  if (hwsInitDone == GT_FALSE)
  {
      /* init sequences DB */
      CHECK_STATUS(hwsXgMacSeqInit(devNum));
  }

  funcPtrArray[XGMAC].macRestFunc = mvHwsXgMacReset;
  funcPtrArray[XGMAC].macLinkGetFunc = mvHwsXgMacLinkStatus;
  funcPtrArray[XGMAC].macLbCfgFunc = mvHwsXgMacLoopbackCfg;
  funcPtrArray[XGMAC].macTypeGetFunc = mvHwsMacTypeGet;

#ifndef CO_CPU_RUN
  funcPtrArray[XGMAC].macLbStatusGetFunc = mvHwsXgMacLoopbackStatusGet;
  funcPtrArray[XGMAC].macActiveStatusFunc = mvHwsXgMacActiveStatusGet;
#endif

  /* specific function */
  funcPtrArray[XGMAC].macModeCfgFunc = mvHwsXgMacRev2ModeCfg;
  funcPtrArray[XGMAC].macFcStateCfgFunc = NULL;

  hwsInitDone = GT_TRUE;

  return GT_OK;
}

/*******************************************************************************
* mvHwsXlgMacRev2ModeCfg
*
* DESCRIPTION:
*       Set the internal mux's to the required MAC in the GOP.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*       numOfLanes - number of lanes aggregated by current mac/port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsXgMacRev2ModeCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_U32                  numOfLanes
)
{
    MV_CFG_SEQ *seq;
    GT_U32 accessAddr;
    GT_U32 baseAddr;
    GT_U32 unitIndex;

    numOfLanes = numOfLanes;

    mvUnitInfoGet(devNum, XLGMAC_UNIT, &baseAddr, &unitIndex);

    accessAddr  = baseAddr + MACRO_CONTROL + 8 * unitIndex;
    if (macNum == 12)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 5), (1 << 5)));
    }

    if (macNum == 14)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 7), (1 << 7)));
    }

    seq = &hwsXgMacSeqDb[XGMAC_MODE_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    /* Rx XLG to RxDMA interface is 64 bits wide.*/
    accessAddr  = baseAddr + MSM_PORT_MAC_CONTROL_REGISTER4 + macNum * unitIndex;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 9), (1 << 9)));


    return GT_OK;
}

/*******************************************************************************
* hwsXgMacRev2IfClose
*
* DESCRIPTION:
*       Release all system resources allocated by XG MAC IF functions.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
void hwsXgMacRev2IfClose(void)
{
  hwsXgMacSeqFree();
  hwsInitDone = GT_FALSE;
}

