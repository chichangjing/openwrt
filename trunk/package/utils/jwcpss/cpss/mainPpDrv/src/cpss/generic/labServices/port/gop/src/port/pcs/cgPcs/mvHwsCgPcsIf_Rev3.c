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
* mvHwsCgPcsIf_Rev3.c
*
* DESCRIPTION:
*       CGPCS interface API
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <pcs/cgPcs/mvHwsCgPcsIf.h>
#include <pcs/cgPcs/mvHwsCgPcsRev3If.h>
#include <pcs/cgPcs/mvHwsCgPcsDb.h>


static char* mvHwsPcsTypeGetFunc(void)
{
  return "CGPCS Rev3";
}
static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* mvHwsGPcsIfInit
*
* DESCRIPTION:
*       Init GPCS configuration sequences and IF functions.
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
GT_STATUS mvHwsCgPcsRev3IfInit(MV_HWS_PCS_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsCgPcsSeqInit());
    }

    funcPtrArray[CGPCS].pcsResetFunc = NULL; /* all functionality moved to CG MAC */
    funcPtrArray[CGPCS].pcsModeCfgFunc = mvHwsCgPcsMode;
    funcPtrArray[CGPCS].pcsLbCfgFunc = mvHwsCgPcsLoopBack;
    funcPtrArray[CGPCS].pcsTypeGetFunc = mvHwsPcsTypeGetFunc;
    funcPtrArray[CGPCS].pcsFecCfgFunc = mvHwsCgPcsFecConfig;
    funcPtrArray[CGPCS].pcsFecCfgGetFunc = mvHwsCgPcsFecConfigGet;

    funcPtrArray[CGPCS].pcsSignalDetectMaskEn = mvHwsCgPcsSignalDetectMaskRev3Set;

    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsCgPcsSignalDetectMaskRev3Set
*
* DESCRIPTION:
*       Set all related PCS with Signal Detect Mask value (1/0).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       maskEn    - if true, enable signal detect mask bit
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsCgPcsSignalDetectMaskRev3Set
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_BOOL                 maskEn
)
{
    GT_U32 regAddr, data, mask;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    mvUnitInfoGet(devNum, CG_UNIT, &unitAddr, &unitIndex);

    regAddr = CG_CONTROL_0 + unitAddr + pcsNum*unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    if (((data >> 22) & 1) == 1)
    {
        /* supports 12 lanes */
        data = (maskEn == GT_TRUE) ? (0xFFF << 6) : 0;
        mask = 0xFFF << 6;
    }
    else
    {
        /* supports 10 lanes */
        data = (maskEn == GT_TRUE) ? (0x3FF << 6) : 0;
        mask = 0xFFF << 6;
    }
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, data, mask));

    return GT_OK;
}

/*******************************************************************************
* hwsCgPcsRev3IfClose
*
* DESCRIPTION:
*       Release all system resources allocated by PCS IF functions.
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
void hwsCgPcsRev3IfClose(void)
{
  hwsCgPcsSeqFree();
  hwsInitDone = GT_FALSE;
}
