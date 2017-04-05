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
* mvHwsHglMacRev2If.c
*
* DESCRIPTION:
*       HGL MAC interface revision 2 (for puma3)
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <mac/hglMac/mvHwsHglMacRev2If.h>
#include <mac/hglMac/mvHwsHglMacDb.h>

static char* mvHwsMacTypeGet(void)
{
  return "HGLMAC_Rev2";
}
static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* hwsHglMacRev2IfInit
*
* DESCRIPTION:
*       Init HGL MAC configuration sequences and IF functions.
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
GT_STATUS hwsHglMacRev2IfInit(MV_HWS_MAC_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsHglMacSeqInit());
    }

    funcPtrArray[HGLMAC].macRestFunc = mvHwsHglMacReset;
    funcPtrArray[HGLMAC].macLinkGetFunc = mvHwsHglMacLinkStatus;
    funcPtrArray[HGLMAC].macLbCfgFunc = mvHwsHglMacLoopbackCfg;
    funcPtrArray[HGLMAC].macTypeGetFunc = mvHwsMacTypeGet;
#ifndef CO_CPU_RUN
    funcPtrArray[HGLMAC].macLbStatusGetFunc = mvHwsHglMacLoopbackStatusGet;
    funcPtrArray[HGLMAC].macActiveStatusFunc = mvHwsHglMacActiveStatusGet;
#endif
    funcPtrArray[HGLMAC].macFcStateCfgFunc = NULL;

    /* specific function */
    funcPtrArray[HGLMAC].macModeCfgFunc = mvHwsHglMacRev2ModeCfg;

    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsHglMacRev2ModeCfg
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
GT_STATUS mvHwsHglMacRev2ModeCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_U32                  numOfLanes
)
{
    MV_CFG_SEQ *seq;
    GT_U32 baseAddr;
    GT_U32 unitIndex;
    GT_U32 accessAddr;

    numOfLanes = numOfLanes;

    seq = &hwsHglMacSeqDb[HGLMAC_MODE_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    /* Rx XLG to RxDMA interface is 64 bits wide.*/
    mvUnitInfoGet(devNum, XLGMAC_UNIT, &baseAddr, &unitIndex);
    accessAddr  = baseAddr + MSM_PORT_MAC_CONTROL_REGISTER4 + macNum * unitIndex;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 9), (1 << 9)));

    return GT_OK;
}

/*******************************************************************************
* hwsHglMacRev2IfClose
*
* DESCRIPTION:
*       Release all system resources allocated by Hgl MAC IF functions.
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
void hwsHglMacRev2IfClose(void)
{
  hwsHglMacSeqFree();
  hwsInitDone = GT_FALSE;
}

