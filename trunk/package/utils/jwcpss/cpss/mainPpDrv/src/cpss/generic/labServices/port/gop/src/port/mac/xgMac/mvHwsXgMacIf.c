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
* mvHwsXgMacIf.c
*
* DESCRIPTION:
*       XGL MAC reset interface
*
* FILE REVISION NUMBER:
*       $Revision: 20 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <mac/xgMac/mvHwsXgMacIf.h>
#include <mac/xlgMac/mvHwsXlgMacDb.h>
#include <mac/xgMac/mvHwsXgMacDb.h>


static char* mvHwsMacTypeGet(void)
{
  return "XGMAC";
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
GT_STATUS hwsXgMacIfInit(GT_U8 devNum, MV_HWS_MAC_FUNC_PTRS *funcPtrArray)
{
  if (hwsInitDone == GT_FALSE)
  {
      /* init sequences DB */
      hwsXgMacSeqInit(devNum);
  }

  funcPtrArray[XGMAC].macRestFunc = mvHwsXgMacReset;
  funcPtrArray[XGMAC].macModeCfgFunc = mvHwsXgMacModeCfg;
  funcPtrArray[XGMAC].macLinkGetFunc = mvHwsXgMacLinkStatus;
  funcPtrArray[XGMAC].macLbCfgFunc = mvHwsXgMacLoopbackCfg;
#ifndef CO_CPU_RUN
  funcPtrArray[XGMAC].macLbStatusGetFunc = mvHwsXgMacLoopbackStatusGet;
  funcPtrArray[XGMAC].macActiveStatusFunc = mvHwsXgMacActiveStatusGet;
#endif
  funcPtrArray[XGMAC].macTypeGetFunc = mvHwsMacTypeGet;
  funcPtrArray[XGMAC].macFcStateCfgFunc = NULL;
  hwsInitDone = GT_TRUE;

  return GT_OK;
}


/*******************************************************************************
* mvHwsXgMacReset
*
* DESCRIPTION:
*       Set the XG MAC to reset or exit from reset.
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
GT_STATUS mvHwsXgMacReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_RESET            action
)
{
    MV_CFG_SEQ *seq;
    GT_U32 accessAddr;
    GT_U32 baseAddr;
    GT_U32 unitIndex;
	GT_U32 data;

    seq = (action == RESET) ? &hwsXlgMacSeqDb[XLGMAC_RESET_SEQ] : &hwsXlgMacSeqDb[XLGMAC_UNRESET_SEQ];
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
* mvHwsXlgMacModeCfg
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
GT_STATUS mvHwsXgMacModeCfg
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

    return GT_OK;
}

/*******************************************************************************
* mvHwsXgMacLoopbackCfg
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
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsXgMacLoopbackCfg
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
      seq = &hwsXlgMacSeqDb[XLGMAC_LPBK_NORMAL_SEQ];
      break;
    case RX_2_TX_LB:
      seq = &hwsXlgMacSeqDb[XLGMAC_LPBK_RX2TX_SEQ];
      break;
    case TX_2_RX_LB:
      seq = &hwsXlgMacSeqDb[XLGMAC_LPBK_TX2RX_SEQ];
      break;
    default:
      return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsXgMacLoopbackStatusGet
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
GT_STATUS mvHwsXgMacLoopbackStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_LB_TYPE     *lbType
)
{
    GT_U32 data;
    GT_U32 accessAddr;
    GT_U32 unitIndex;
    GT_U32 baseAddr;

    mvUnitInfoGet(devNum, XLGMAC_UNIT, &baseAddr, &unitIndex);
    accessAddr  = baseAddr + MSM_PORT_MAC_CONTROL_REGISTER1 + macNum * unitIndex;

    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));

    switch ((data >> 13) & 3)
    {
    case 0:
        *lbType = DISABLE_LB;
      break;
    case 1:
        *lbType = TX_2_RX_LB;
      break;
    case 2:
        *lbType = RX_2_TX_LB;
      break;
    default:
      return GT_NOT_SUPPORTED;
    }

    return GT_OK;
}
#endif

/*******************************************************************************
* mvHwsXgMacLinkStatus
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
GT_STATUS mvHwsXgMacLinkStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_BOOL                 *linkStatus
)
{
  GT_U32 data;
  GT_U32 accessAddr;
  GT_U32 unitIndex;
  GT_U32 baseAddr;

  mvUnitInfoGet(devNum, XLGMAC_UNIT, &baseAddr, &unitIndex);
  accessAddr  = baseAddr + MSM_PORT_STATUS + macNum * unitIndex;

  CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));

  *linkStatus = data & 1;
  return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsXgMacActiveStatusGet
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
GT_STATUS mvHwsXgMacActiveStatusGet
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
    accessAddr = baseAddr + MSM_PORT_MAC_CONTROL_REGISTER0 + macNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    if (((data >> 1) & 1) == 0)
    {
        /* unit is under RESET */
        return GT_OK;
    }
    /* check if XPCS used */
    accessAddr  = baseAddr + MSM_PORT_MAC_CONTROL_REGISTER4 + macNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    if (((data >> 8) & 1) == 0)
    {
        /* XPCS doesn't used */
        return GT_OK;
    }

    *numOfLanes = 1;

    return GT_OK;
}
#endif

/*******************************************************************************
* hwsXgMacIfClose
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
void hwsXgMacIfClose(void)
{
  hwsXgMacSeqFree();
  hwsInitDone = GT_FALSE;
}
