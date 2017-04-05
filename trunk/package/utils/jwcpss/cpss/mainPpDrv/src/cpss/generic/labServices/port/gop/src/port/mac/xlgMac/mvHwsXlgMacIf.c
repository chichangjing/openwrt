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
* mvHwsXglMacIf.c
*
* DESCRIPTION:
*       XGL MAC reset interface
*
* FILE REVISION NUMBER:
*       $Revision: 30 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <mac/xlgMac/mvHwsXlgMacIf.h>
#include <mac/xlgMac/mvHwsXlgMacDb.h>

static char* mvHwsMacTypeGet(void)
{
  return "XLGMAC";
}
static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* hwsXlgMacIfInit
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
GT_STATUS hwsXlgMacIfInit(MV_HWS_MAC_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsXlgMacSeqInit());
    }

    funcPtrArray[XLGMAC].macRestFunc = mvHwsXlgMacReset;
    funcPtrArray[XLGMAC].macModeCfgFunc = mvHwsXlgMacModeCfg;
    funcPtrArray[XLGMAC].macLinkGetFunc = mvHwsXlgMacLinkStatus;
    funcPtrArray[XLGMAC].macLbCfgFunc = mvHwsXlgMacLoopbackCfg;
#ifndef CO_CPU_RUN
    funcPtrArray[XLGMAC].macLbStatusGetFunc = mvHwsXlgMacLoopbackStatusGet;
    funcPtrArray[XLGMAC].macActiveStatusFunc = mvHwsXlgMacActiveStatusGet;
#endif
    funcPtrArray[XLGMAC].macTypeGetFunc = mvHwsMacTypeGet;
    funcPtrArray[XLGMAC].macFcStateCfgFunc = NULL;

    hwsInitDone = GT_TRUE;

    return GT_OK;
}


/*******************************************************************************
* mvHwsXlgMacReset
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
GT_STATUS mvHwsXlgMacReset
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
GT_STATUS mvHwsXlgMacModeCfg
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

    accessAddr  = baseAddr + MACRO_CONTROL + 8 * unitIndex;

    switch(numOfLanes)
    {
    case 1:
      seq = &hwsXlgMacSeqDb[XLGMAC_MODE_1_Lane_SEQ];
      if (macNum == 12)
      {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 5), (1 << 5)));
      }
      if (macNum == 14)
      {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 7), (1 << 7)));
      }
      break;
    case 2:
      seq = &hwsXlgMacSeqDb[XLGMAC_MODE_2_Lane_SEQ];
      if (macNum == 12)
      {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 5), (1 << 5)));
      }
      if (macNum == 14)
      {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 7), (1 << 7)));
      }
      break;
    case 4:
      seq = &hwsXlgMacSeqDb[XLGMAC_MODE_4_Lane_SEQ];
      if (macNum == 12)
      {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, (1 << 5), (1 << 5)));
      }
      break;
    default:
        return GT_BAD_PARAM;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    /* keep in reset all unused MACs */
    for (i = 1; (i < numOfLanes); i++)
    {
        /*0x088C0000 + p * 0x1000: where p (14-14) represents Port Num
          0x088C0000 + p * 0x1000: where p (0-12) represents Port Num*/
        if (((macNum+i) < 15) && ((macNum+i) != 13))
        {
            accessAddr = baseAddr + MSM_PORT_MAC_CONTROL_REGISTER0 + (macNum+i) * unitIndex;
            CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, 0, (1 << 1)));
        }
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsXlgMacLoopbackCfg
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
GT_STATUS mvHwsXlgMacLoopbackCfg
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
      return GT_NOT_SUPPORTED;
    }

    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsXlgMacLoopbackStatusGet
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
GT_STATUS mvHwsXlgMacLoopbackStatusGet
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
* mvHwsXlgMacLinkStatus
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
GT_STATUS mvHwsXlgMacLinkStatus
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

/*******************************************************************************
* mvHwsXlgMacModeSelectDefaultCfg
*
* DESCRIPTION:
*       MAC mode select default value configuration (needed for AP mechanism).
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
GT_STATUS mvHwsXlgMacModeSelectDefaultCfg
(
    GT_U8                 devNum,
    GT_U32                portGroup,
    GT_U32                macNum
)
{
	/* change to genUnitRegisterSet since this functionality is also used for 28nm devices */
	CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, macNum, MSM_PORT_MAC_CONTROL_REGISTER3, (1 << 13),(7 << 13)));

    return GT_OK;
}

/*******************************************************************************
* mvHwsXlgMacActiveStatusGet
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
GT_STATUS mvHwsXlgMacActiveStatusGet
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

    accessAddr = baseAddr + MSM_PORT_MAC_CONTROL_REGISTER0 + macNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    if (!((data >> 1) & 1))
    {
        /* unit is under RESET */
        return GT_OK;
    }

    *numOfLanes = 0;
    /* get number of active lanes */
    accessAddr = baseAddr + MSM_PORT_MAC_CONTROL_REGISTER3 + macNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    switch ((data >> 13) & 7)
    {
    case 1:
        *numOfLanes = 1;
        break;
    case 2:
        *numOfLanes = 2;
        break;
    case 3:
        *numOfLanes = 4;
        break;
    case 7:
    case 0:
    default:
        *numOfLanes = 0;
        break;
    }

    return GT_OK;
}

/*******************************************************************************
* hwsXlgMacIfClose
*
* DESCRIPTION:
*       Release all system resources allocated by MAC IF functions.
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
void hwsXlgMacIfClose(void)
{
  hwsXlgMacSeqFree();
  hwsInitDone = GT_FALSE;
}

