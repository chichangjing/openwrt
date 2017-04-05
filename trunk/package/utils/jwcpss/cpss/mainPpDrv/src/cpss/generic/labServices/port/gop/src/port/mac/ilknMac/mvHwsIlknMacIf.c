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
* mvHwsIlknMacIf.c
*
* DESCRIPTION:
*       CG MAC reset interface
*
* FILE REVISION NUMBER:
*       $Revision: 18 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <mac/ilknMac/mvHwsIlknMacIf.h>
#include <mac/ilknMac/mvHwsIlknMacDb.h>

#ifndef CO_CPU_RUN
static char* mvHwsMacTypeGet(void)
{
  return "ILKN_MAC";
}
#endif

static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* hwsIlknMacIfInit
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
GT_STATUS hwsIlknMacIfInit(MV_HWS_MAC_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsIlknMacSeqInit());
    }

    funcPtrArray[INTLKN_MAC].macRestFunc = mvHwsIlknMacReset;
    funcPtrArray[INTLKN_MAC].macModeCfgFunc = mvHwsIlknMacModeCfg;
    funcPtrArray[INTLKN_MAC].macLinkGetFunc = mvHwsIlknMacLinkStatus;
#ifndef CO_CPU_RUN
    funcPtrArray[INTLKN_MAC].macLbCfgFunc = mvHwsIlknMacLoopbackCfg;
    funcPtrArray[INTLKN_MAC].macLbStatusGetFunc = mvHwsIlknMacLoopbackStatusGet;
    funcPtrArray[INTLKN_MAC].macTypeGetFunc = mvHwsMacTypeGet;
#endif
    funcPtrArray[INTLKN_MAC].macFcStateCfgFunc = NULL;

    hwsInitDone = GT_TRUE;

    return GT_OK;
}


/*******************************************************************************
* mvHwsIlknMacReset
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
GT_STATUS mvHwsIlknMacReset
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 macNum,
    MV_HWS_RESET            action
)
{
    MV_CFG_SEQ *seq;

    seq = (action == RESET) ? &hwsIlknMacSeqDb[INTLKN_MAC_RESET_SEQ] : &hwsIlknMacSeqDb[INTLKN_MAC_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsIlknMacModeCfg
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
GT_STATUS mvHwsIlknMacModeCfg
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 macNum,
    GT_UOPT                 numOfLanes
)
{
    MV_CFG_SEQ *seq;

    switch(numOfLanes)
    {
	case 4:
        seq = &hwsIlknMacSeqDb[INTLKN_MAC_MODE_4_LANES_SEQ];
        break;
	case 8:
        seq = &hwsIlknMacSeqDb[INTLKN_MAC_MODE_8_LANES_SEQ];
        break;
    case 12:
        seq = &hwsIlknMacSeqDb[INTLKN_MAC_MODE_12_LANES_SEQ];
        break;
    case 16:
        seq = &hwsIlknMacSeqDb[INTLKN_MAC_MODE_16_LANES_SEQ];
        break;
    case 24:
        seq = &hwsIlknMacSeqDb[INTLKN_MAC_MODE_24_LANES_SEQ];
        break;
    default:
        return GT_BAD_PARAM;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    if (HWS_DEV_SILICON_TYPE(devNum) == Puma3B0)
    {
    	CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, INTLKN_UNIT, macNum, TXDMA_converter_control_0, (1 << 15), (1 << 15)));

		/* set the field in the RxDMA converter packet size restriction register 
		  (WA to avoid packets corruption Puma3  FE-820601) */
		CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, INTLKN_UNIT, macNum, 0x4C, (0x3FFF << 14), (0x3FFF << 14)));
    }

	return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsIlknMacLoopbackCfg
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
GT_STATUS mvHwsIlknMacLoopbackCfg
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 macNum,
    MV_HWS_PORT_LB_TYPE     lbType
)
{
    MV_CFG_SEQ *seq;

    switch (lbType)
    {
    case DISABLE_LB:
      seq = &hwsIlknMacSeqDb[INTLKN_MAC_LPBK_NORMAL_SEQ];
      break;
    case TX_2_RX_LB:
      seq = &hwsIlknMacSeqDb[INTLKN_MAC_LPBK_TX2RX_SEQ];
      break;
    default:
      return GT_NOT_SUPPORTED;
    }

    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsIlknMacLoopbackStatusGet
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
GT_STATUS mvHwsIlknMacLoopbackStatusGet
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 macNum,
    MV_HWS_PORT_LB_TYPE     *lbType
)
{
    GT_UREG_DATA data;

    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, INTLKN_UNIT, macNum, RXDMA_converter_control_0, &data, 0));

    switch ((data >> 5) & 3)
    {
    case 0:
        *lbType = DISABLE_LB;
      break;
    case 1:
        *lbType = TX_2_RX_LB;
      break;
    default:
      return GT_NOT_SUPPORTED;
    }

    return GT_OK;
}
#endif

/*******************************************************************************
* mvHwsIlknMacLinkStatus
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
GT_STATUS mvHwsIlknMacLinkStatus
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 macNum,
    GT_BOOL                 *linkStatus
)
{
  GT_UREG_DATA data;
  MV_HWS_UNITS_ID unitId;

  /* check if unit not in RESET */
  CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, INTLKN_UNIT, macNum, ILKN_RESETS, &data, 0));

  if (((data >> 1) & 1) == 0)
  {
      /* unit under reset */
      *linkStatus = GT_FALSE;
      return GT_OK;
  }

  /* get link status */
  if ((HWS_DEV_SILICON_TYPE(devNum) == Puma3A0) || (HWS_DEV_SILICON_TYPE(devNum) == Puma3B0))
  {
	unitId = INTLKN_UNIT;
  }
  else
  {
	unitId = INTLKN_RF_UNIT;
  }

  CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, unitId, macNum, ILKN_0_ALIGN_STATUS, &data, 0));

  *linkStatus = (((data >> 1) & 3) == 3) ? GT_TRUE : GT_FALSE;

  if (*linkStatus)
  {
      return GT_OK;
  }
  return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* hwsIlknMacIfClose
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
void hwsIlknMacIfClose(void)
{
  hwsIlknMacSeqFree();
  hwsInitDone = GT_FALSE;
}
#endif
