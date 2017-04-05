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
* mvHwsGeMacSgIf.c
*
* DESCRIPTION:
*       GE MAC interface
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <mac/geMac/mvHwsGeMacSgIf.h>
#include <mac/geMac/mvHwsGeMacDb.h>

static char* mvHwsMacTypeGet(void)
{
  return "GEMAC_SG";
}
static GT_BOOL hwsInitDone = GT_FALSE;

extern GT_STATUS mvHwsGeMacLoopbackStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_LB_TYPE     *lbType
);

GT_STATUS mvHwsGeNetMacSgReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_RESET            action
);

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
GT_STATUS hwsGeMacSgIfInit(GT_U8 devNum, MV_HWS_MAC_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsGeMacSgSeqInit(devNum));
    }

    funcPtrArray[GEMAC_SG].macRestFunc = mvHwsGeMacSgReset;
    funcPtrArray[GEMAC_SG].macModeCfgFunc = mvHwsGeMacSgModeCfg;
    funcPtrArray[GEMAC_SG].macLinkGetFunc = mvHwsGeMacSgLinkStatus;
    funcPtrArray[GEMAC_SG].macLbCfgFunc = mvHwsGeMacSgLoopbackCfg;
    funcPtrArray[GEMAC_SG].macTypeGetFunc = mvHwsMacTypeGet;
    funcPtrArray[GEMAC_SG].macFcStateCfgFunc = NULL;

    funcPtrArray[GEMAC_NET_SG].macRestFunc = mvHwsGeNetMacSgReset;       /* Network port implementation */
    funcPtrArray[GEMAC_NET_SG].macModeCfgFunc = mvHwsGeNetMacSgModeCfg;  /* Network port implementation */
    funcPtrArray[GEMAC_NET_SG].macLinkGetFunc = mvHwsGeMacSgLinkStatus;
    funcPtrArray[GEMAC_NET_SG].macLbCfgFunc = mvHwsGeMacSgLoopbackCfg;
    funcPtrArray[GEMAC_NET_SG].macTypeGetFunc = mvHwsMacTypeGet;
    funcPtrArray[GEMAC_NET_SG].macFcStateCfgFunc = NULL;
#ifndef CO_CPU_RUN
    funcPtrArray[GEMAC_SG].macActiveStatusFunc = mvHwsGeMacSgActiveStatusGet;
    funcPtrArray[GEMAC_SG].macLbStatusGetFunc = mvHwsGeMacLoopbackStatusGet;

    funcPtrArray[GEMAC_NET_SG].macActiveStatusFunc = mvHwsGeMacSgActiveStatusGet;
    funcPtrArray[GEMAC_NET_SG].macLbStatusGetFunc = mvHwsGeMacLoopbackStatusGet;
#endif
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
GT_STATUS mvHwsGeMacSgReset
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

    seq = (action == RESET) ? &hwsGeMacSeqDb[GEMAC_RESET_SEQ]: &hwsGeMacSeqDb[GEMAC_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));
   
    if((HWS_DEV_SILICON_TYPE(devNum) != BobcatA0) && 
                                (HWS_DEV_SILICON_TYPE(devNum) != Alleycat3A0))
    {
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
GT_STATUS mvHwsGeMacSgModeCfg
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

    if((HWS_DEV_SILICON_TYPE(devNum) != BobcatA0) && 
                                (HWS_DEV_SILICON_TYPE(devNum) != Alleycat3A0))
    {
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
    }

    seq = &hwsGeMacSeqDb[GEMAC_SG_MODE_SEQ];      
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsGeMacLoopbackCfg
*
* DESCRIPTION:
*       .
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
GT_STATUS mvHwsGeMacSgLoopbackCfg
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
    case RX_2_TX_LB:
      seq = &hwsGeMacSeqDb[GEMAC_LPBK_RX2TX_SEQ];
      break;
    case TX_2_RX_LB:
      seq = &hwsGeMacSeqDb[GEMAC_LPBK_TX2RX_SEQ];
      break;
    default:
      return GT_NOT_SUPPORTED;
    }      

    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));
    
    return GT_OK;
}

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
GT_STATUS mvHwsGeMacSgLinkStatus
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

  mvUnitExtInfoGet(devNum, GEMAC_UNIT, macNum, &baseAddr, &unitIndex , &macNum);
  accessAddr = baseAddr + GIG_Port_Status_Reg + macNum * unitIndex;
  CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));

  *linkStatus = data & 1;

  return GT_OK;
}

/*******************************************************************************
* mvHwsGeMacSgActiveStatusGet
*
* DESCRIPTION:
*       Return number of MAC active lanes or 0, if current MAC isn't active.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*       macType   - MAC type according to port configuration
*
* OUTPUTS:
*       numOfLanes - number of lanes agregated in PCS
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsGeMacSgActiveStatusGet
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

    mvUnitExtInfoGet(devNum, GEMAC_UNIT, macNum, &baseAddr, &unitIndex, &macNum);

    *numOfLanes = 0;
    accessAddr = baseAddr + GIG_PORT_MAC_CONTROL_REGISTER0 + macNum * unitIndex;
    /* port MAC type bit 1 */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    if (((data >> 1) & 1) != 0)
    {
        /* 0x0 = SGMII */
        return GT_OK;
    }

    accessAddr = baseAddr + GIG_PORT_MAC_CONTROL_REGISTER2 + macNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    if ((data >> 6) & 1)
    {
        /* unit is under RESET */
        return GT_OK;
    }
    *numOfLanes = 1;

    return GT_OK;
}

/*******************************************************************************
* mvHwsGeNetMacSgReset
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
GT_STATUS mvHwsGeNetMacSgReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_RESET            action
)
{
    MV_CFG_SEQ *seq;

    seq = (action == RESET) ? &hwsGeMacSeqDb[GEMAC_RESET_SEQ]: &hwsGeMacSeqDb[GEMAC_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

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
GT_STATUS mvHwsGeNetMacSgModeCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_U32                  numOfLanes
)
{
    MV_CFG_SEQ *seq;

    numOfLanes = numOfLanes;

    seq = &hwsGeMacSeqDb[GEMAC_NET_SG_MODE_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}
