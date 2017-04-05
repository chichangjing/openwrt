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
* mvHwsHglMacIf.c
*
* DESCRIPTION:
*       GE MAC interface
*
* FILE REVISION NUMBER:
*       $Revision: 17 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <mac/hglMac/mvHwsHglMacIf.h>
#include <mac/hglMac/mvHwsHglMacDb.h>
 
static char* mvHwsMacTypeGet(void)
{
  return "HGLMAC";
}
static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* hwsHglMacIfInit
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
GT_STATUS hwsHglMacIfInit(MV_HWS_MAC_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsHglMacSeqInit());
    }

    funcPtrArray[HGLMAC].macRestFunc = mvHwsHglMacReset;
    funcPtrArray[HGLMAC].macModeCfgFunc = mvHwsHglMacModeCfg;
    funcPtrArray[HGLMAC].macLinkGetFunc = mvHwsHglMacLinkStatus;
    funcPtrArray[HGLMAC].macLbCfgFunc = mvHwsHglMacLoopbackCfg;
#ifndef CO_CPU_RUN
    funcPtrArray[HGLMAC].macLbStatusGetFunc = mvHwsHglMacLoopbackStatusGet;
    funcPtrArray[HGLMAC].macActiveStatusFunc = mvHwsHglMacActiveStatusGet;
#endif
    funcPtrArray[HGLMAC].macTypeGetFunc = mvHwsMacTypeGet;
    funcPtrArray[HGLMAC].macFcStateCfgFunc = NULL;
    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsHglMacReset
*
* DESCRIPTION:
*       Set the HGL MAC to reset or exit from reset.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*       action    - normal or reset
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsHglMacReset
(
    GT_U8         devNum,
    GT_U32        portGroup,
    GT_U32        macNum,
    MV_HWS_RESET  action
)
{
    devNum      = devNum;
    portGroup   = portGroup;
    macNum      = macNum;
    action      = action;

    return GT_OK;
}

/*******************************************************************************
* mvHwsHglMacModeCfg
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
GT_STATUS mvHwsHglMacModeCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_U32                  numOfLanes
)
{
    MV_CFG_SEQ *seq;

    numOfLanes = numOfLanes;

    seq = &hwsHglMacSeqDb[HGLMAC_MODE_SEQ];      
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsHglMacLinkStatus
*
* DESCRIPTION:
*       Get MAC link status.
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
GT_STATUS mvHwsHglMacLinkStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_BOOL                 *linkStatus
)
{
  GT_U32 data;
  GT_U32 baseAddr;
  GT_U32 unitIndex;
  GT_U32 accessAddr;

  mvUnitInfoGet(devNum, XPCS_UNIT, &baseAddr, &unitIndex);
  accessAddr  = baseAddr + XPCS_Global_Status_Reg + (macNum/2) * unitIndex;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));

  *linkStatus = data & 1;
  return GT_OK;
}

/*******************************************************************************
* mvHwsHglMacLoopbackCfg
*
* DESCRIPTION:
*       Loopback not supported.
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
GT_STATUS mvHwsHglMacLoopbackCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_LB_TYPE     lbType
)
{
    (GT_VOID)devNum;
    (GT_VOID)portGroup;
    (GT_VOID)macNum;
    (GT_VOID)lbType;

    return GT_NOT_SUPPORTED;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsHglMacLoopbackStatusGet
*
* DESCRIPTION:
*       Loopback not supported.
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
GT_STATUS mvHwsHglMacLoopbackStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_LB_TYPE     *lbType
)
{
    (GT_VOID)devNum;
    (GT_VOID)portGroup;
    (GT_VOID)macNum;

    lbType = lbType;

    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* mvHwsHglMacActiveStatusGet
*
* DESCRIPTION:
*       Return number of MAC active lanes or 0, if current MAC isn't active.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*
* OUTPUTS:
*       numOfLanes - number of lanes agregated
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsHglMacActiveStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_U32                  *numOfLanes
)
{
    GT_U32 data;
    GT_U32 baseAddr;
    GT_U32 unitIndex;
    GT_U32 accessAddr;

    if (numOfLanes == NULL)
    {
        return GT_BAD_PARAM;
    }

    mvUnitInfoGet(devNum, XLGMAC_UNIT, &baseAddr, &unitIndex);
    *numOfLanes = 0;
    /* get number of active lanes */
    accessAddr = baseAddr + MSM_PORT_MAC_CONTROL_REGISTER3 + macNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    switch ((data >> 13) & 7)
    {
    case 7:
        *numOfLanes = 6;
        break;
    default:
        *numOfLanes = 0;
        break;
    }

    return GT_OK;
}

#endif

/*******************************************************************************
* hwsHglMacIfClose
*
* DESCRIPTION:
*       Release all system resources allocated by GE MAC IF functions.
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
void hwsHglMacIfClose(void)
{
  hwsHglMacSeqFree();
  hwsInitDone = GT_FALSE;
}
