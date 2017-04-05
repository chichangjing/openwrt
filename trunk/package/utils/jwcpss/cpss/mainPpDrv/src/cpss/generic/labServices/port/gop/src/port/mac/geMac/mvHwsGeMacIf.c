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
* mvHwsGeMacIf.c
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

#include <mac/geMac/mvHwsGeMacIf.h>
#include <mac/geMac/mvHwsGeMacDb.h>
#include <mac/xlgMac/mvHwsXlgMacDb.h>

static char* mvHwsMacTypeGet(void)
{
  return "GEMAC_X";
}
static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* hwsGeMacIfInit
*
* DESCRIPTION:
*       Init GE MAC configuration sequences and IF functions.
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
GT_STATUS hwsGeMacIfInit(GT_U8 devNum, MV_HWS_MAC_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsGeMacSeqInit(devNum));
    }

    funcPtrArray[GEMAC_X].macRestFunc = mvHwsGeMacReset;
    funcPtrArray[GEMAC_X].macModeCfgFunc = mvHwsGeMacModeCfg;
    funcPtrArray[GEMAC_X].macLinkGetFunc = mvHwsGeMacLinkStatus;
    funcPtrArray[GEMAC_X].macLbCfgFunc = mvHwsGeMacLoopbackCfg;
#ifndef CO_CPU_RUN
    funcPtrArray[GEMAC_X].macLbStatusGetFunc = mvHwsGeMacLoopbackStatusGet;
    funcPtrArray[GEMAC_X].macActiveStatusFunc = mvHwsGeMacActiveStatusGet;
#endif
    funcPtrArray[GEMAC_X].macTypeGetFunc = mvHwsMacTypeGet;
    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsGeMacReset
*
* DESCRIPTION:
*       Set the GE MAC to reset or exit from reset.
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
GT_STATUS mvHwsGeMacReset
(
    GT_U8         devNum,
    GT_U32        portGroup,
    GT_U32        macNum,
    MV_HWS_RESET  action
)
{
    MV_CFG_SEQ *seq;
    GT_U32 accessAddr;
    GT_U32 baseAddr;
    GT_U32 unitIndex;
	GT_U32 data;

    seq = (action == RESET) ? &hwsGeMacSeqDb[GEMAC_RESET_SEQ]: &hwsGeMacSeqDb[GEMAC_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    mvUnitInfoGet(devNum, XLGMAC_UNIT, &baseAddr, &unitIndex);
    accessAddr  = baseAddr + MACRO_CONTROL + 8 * unitIndex;

    data = (action == RESET) ? 0 : 1;
    if (macNum == 12)
    {
		CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (data << 5), (1 << 5)));
    }
    if (macNum == 14)
    {
		CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (data << 7), (1 << 7)));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsGeMacModeCfg
*
* DESCRIPTION:
*       Set the internal mux's to the required MAC in the GOP.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsGeMacModeCfg
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

    seq = &hwsGeMacSeqDb[GEMAC_X_MODE_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsGeMacLoopbackCfg
*
* DESCRIPTION:
*       Configure MAC loopback.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*       lbType    - supported loopback type
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsGeMacLoopbackCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_LB_TYPE     lbType
)
{
    MV_CFG_SEQ *seq;

    switch (lbType)
    {
    case DISABLE_LB:
      seq = &hwsGeMacSeqDb[GEMAC_LPBK_NORMAL_SEQ];
      break;
    case TX_2_RX_LB:
      seq = &hwsGeMacSeqDb[GEMAC_LPBK_TX2RX_SEQ];
      break;
    case RX_2_TX_LB:
    default:
      return GT_NOT_SUPPORTED;
    }

    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsGeMacLoopbackStatusGet
*
* DESCRIPTION:
*       Retrive MAC loopback status.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*
* OUTPUTS:
*       lbType    - supported loopback type
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsGeMacLoopbackStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_LB_TYPE     *lbType
)
{
    GT_U32 data;

    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, GEMAC_UNIT, macNum, GIG_PORT_MAC_CONTROL_REGISTER1, &data, 0));

    *lbType = ((data >> 5) & 1) ? TX_2_RX_LB : DISABLE_LB;

    return GT_OK;
}
#endif

/*******************************************************************************
* mvHwsGeMacLinkStatus
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
GT_STATUS mvHwsGeMacLinkStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_BOOL                 *linkStatus
)
{
  GT_U32 data;

  CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, GEMAC_UNIT, macNum, GIG_Port_Status_Reg, &data, 0));

  *linkStatus = data & 1;

  return GT_OK;
}

/*******************************************************************************
* mvHwsGeMacActiveStatusGet
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
*       numOfLanes - number of lanes agregated in PCS
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsGeMacActiveStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_U32                  *numOfLanes
)
{
    GT_U32 data;

    if (numOfLanes == NULL)
    {
        return GT_BAD_PARAM;
    }

    *numOfLanes = 0;
    /* port MAC type bit 1 */
    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, GEMAC_UNIT, macNum, GIG_PORT_MAC_CONTROL_REGISTER0, &data, 0));
    if (((data >> 1) & 1) != 1)
    {
        /* 0x1 = 1000Base-X_mode */
        return GT_OK;
    }

    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, GEMAC_UNIT, macNum, GIG_PORT_MAC_CONTROL_REGISTER2, &data, 0));
    if ((data >> 6) & 1)
    {
        /* unit is under RESET */
        return GT_OK;
    }
    *numOfLanes = 1;

    return GT_OK;
}

/*******************************************************************************
* hwsGeMacIfClose
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
void hwsGeMacIfClose(void)
{
  hwsGeMacSeqFree();
  hwsInitDone = GT_FALSE;
}
