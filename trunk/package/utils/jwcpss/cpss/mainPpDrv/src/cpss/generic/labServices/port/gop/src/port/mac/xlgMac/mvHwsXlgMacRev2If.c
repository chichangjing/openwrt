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
* mvHwsXglMacRev2If.c
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

#include <mac/xlgMac/mvHwsXlgMacRev2If.h>
#include <mac/xlgMac/mvHwsXlgMacDb.h>

static char* mvHwsMacTypeGet(void)
{
  return "XLGMAC_Rev2";
}
static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* hwsXlgMacRev2IfInit
*
* DESCRIPTION:
*       Init XLG MAC configuration sequences and IF functions.
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
GT_STATUS hwsXlgMacRev2IfInit(MV_HWS_MAC_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsXlgMacSeqInit());
    }

    funcPtrArray[XLGMAC].macRestFunc = mvHwsXlgMacReset;
    funcPtrArray[XLGMAC].macLinkGetFunc = mvHwsXlgMacLinkStatus;
    funcPtrArray[XLGMAC].macLbCfgFunc = mvHwsXlgMacLoopbackCfg;
    funcPtrArray[XLGMAC].macTypeGetFunc = mvHwsMacTypeGet;
#ifndef CO_CPU_RUN
    funcPtrArray[XLGMAC].macLbStatusGetFunc = mvHwsXlgMacLoopbackStatusGet;
    funcPtrArray[XLGMAC].macActiveStatusFunc = mvHwsXlgMacActiveStatusGet;
#endif
    funcPtrArray[XLGMAC].macFcStateCfgFunc = NULL;

    /* specific function */
    funcPtrArray[XLGMAC].macModeCfgFunc = mvHwsXlgMacRev2ModeCfg;
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
GT_STATUS mvHwsXlgMacRev2ModeCfg
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
    GT_U32 unitIndex, i;

    mvUnitInfoGet(devNum, XLGMAC_UNIT, &baseAddr, &unitIndex);

    switch(numOfLanes)
    {
    case 1:
      seq = &hwsXlgMacSeqDb[XLGMAC_MODE_1_Lane_SEQ];
      accessAddr  = baseAddr + MACRO_CONTROL + 8 * unitIndex;
      if (macNum == 12)
      {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 5), (1 << 5)));
      }
      if (macNum == 14)
      {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 7), (1 << 7)));
      }

      /*Rx XLG to RxDMA interface is 64 bits wide.*/
      accessAddr  = baseAddr + MSM_PORT_MAC_CONTROL_REGISTER4 + macNum * unitIndex;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 9), (1 << 9)));
      break;
    case 2:
      seq = &hwsXlgMacSeqDb[XLGMAC_MODE_2_Lane_SEQ];
      accessAddr  = baseAddr + MACRO_CONTROL + 8 * unitIndex;
      if (macNum == 12)
      {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 5), (1 << 5)));
      }
      if (macNum == 14)
      {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 7), (1 << 7)));
      }

      /*Rx XLG to RxDMA interface is 64 bits wide.*/
      accessAddr  = baseAddr + MSM_PORT_MAC_CONTROL_REGISTER4 + macNum * unitIndex;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 9), (1 << 9)));
      break;
    case 4:
      seq = &hwsXlgMacSeqDb[XLGMAC_MODE_4_Lane_SEQ];
      accessAddr  = baseAddr + MACRO_CONTROL + 8 * unitIndex;
      if (macNum == 12)
      {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 5), (1 << 5)));
      }

      /*Rx XLG to RxDMA interface is 256 bits wide.*/
      accessAddr  = baseAddr + MSM_PORT_MAC_CONTROL_REGISTER4 + macNum * unitIndex;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, 0, (1 << 9)));
      break;
    default:
        return GT_BAD_PARAM;
        break;
    }

    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    /* keep in reset all unused MACs */
    for (i = 1; (i < numOfLanes); i++)
    {
        if ((macNum+i) < 12)
        {
            /* 0x010C0000 + 0x1000*p: where p (16-19) represents fabric port
               0x010C0000 + 0x1000*p: where p (0-11) represents network port */
	        accessAddr = baseAddr + MSM_PORT_MAC_CONTROL_REGISTER0 + (macNum+i) * unitIndex;
	        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, 0, (1 << 1)));
	    }
    }
    return GT_OK;
}

/*******************************************************************************
* hwsXlgMacRev2IfClose
*
* DESCRIPTION:
*       Release all system resources allocated by XLG MAC IF functions.
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
void hwsXlgMacRev2IfClose(void)
{
  hwsXlgMacSeqFree();
  hwsInitDone = GT_FALSE;
}


