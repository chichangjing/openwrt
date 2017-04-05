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
* mvHwsXglMac28nmIf.c
*
* DESCRIPTION:
*       XGL MAC interface (for bobcat2)
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <mac/xlgMac/mvHwsXlgMac28nmDb.h>
#include <mac/xlgMac/mvHwsXlgMac28nmIf.h>

#ifndef CO_CPU_RUN
static char* mvHwsMacTypeGet(void)
{
  return "XLGMAC_28nm";
}
#endif
static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* hwsXlgMac28nmIfInit
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
GT_STATUS hwsXlgMac28nmIfInit(GT_U8 devNum, MV_HWS_MAC_FUNC_PTRS *funcPtrArray)
{
    /* TBD - Need to fix */
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsXlgMac28nmSeqInit(devNum));
    }

    funcPtrArray[XLGMAC].macRestFunc = mvHwsXlgMac28nmReset;
    funcPtrArray[XLGMAC].macLinkGetFunc = mvHwsXlgMac28nmLinkStatus;
    funcPtrArray[XLGMAC].macModeCfgFunc = mvHwsXlgMac28nmModeCfg;
#ifndef CO_CPU_RUN
    funcPtrArray[XLGMAC].macTypeGetFunc = mvHwsMacTypeGet;
    funcPtrArray[XLGMAC].macLbCfgFunc = mvHwsXlgMac28nmLoopbackCfg;
    funcPtrArray[XLGMAC].macLbStatusGetFunc = mvHwsXlgMac28nmLoopbackStatusGet;
    funcPtrArray[XLGMAC].macActiveStatusFunc = mvHwsXlgMac28nmActiveStatusGet;
    funcPtrArray[XLGMAC].macClearChannelCfgFunc = NULL;
#endif
    funcPtrArray[XLGMAC].macFcStateCfgFunc = mvHwsXlgMac28nmFcStateCfg;

    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsXlgMac28nmReset
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
GT_STATUS mvHwsXlgMac28nmReset
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 macNum,
    MV_HWS_RESET            action
)
{
    MV_CFG_SEQ *seq;

    /* For BobcatA0 only: ENV_SPEED_MAC_369 - XLG MAC cannot be reset */
    if ((HWS_DEV_GOP_REV(devNum) == GOP_28NM_REV1) && (HWS_DEV_SILICON_TYPE(devNum) == BobcatA0) && (action == RESET))
    {
        return GT_OK;
    }

    if(FULL_RESET == action)
    {
        seq = &hwsXlgMac28nmSeqDb[XLGMAC_POWER_DOWN_SEQ];
    }
    else if(RESET == action)
    {
        seq = &hwsXlgMac28nmSeqDb[XLGMAC_RESET_SEQ];
    }
    else
    {
        seq = &hwsXlgMac28nmSeqDb[XLGMAC_UNRESET_SEQ];
    }

    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsXlgMac28nmModeCfg
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
GT_STATUS mvHwsXlgMac28nmModeCfg
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
    case 1:
      seq = &hwsXlgMac28nmSeqDb[XLGMAC_MODE_1_Lane_SEQ];
      break;
    case 2:
      seq = &hwsXlgMac28nmSeqDb[XLGMAC_MODE_2_Lane_SEQ];
      break;
    case 4:
      seq = &hwsXlgMac28nmSeqDb[XLGMAC_MODE_4_Lane_SEQ];
      break;
    default:
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

	/* In Alleycat3 there is a Mux for DMA unit between regular and extended Stacking ports 25,27-29 in MSMS4 unit
	   The selector (port RX interface) for changing the configuration exists 
	   in XLGMAC unit with MAC number 24 in External_control register:
	      External_Control_0 (bit 0) - select between regular port 25 (value=0, internal MAC) and extended port 28 (value=1, external client 1)
		  External_Control_1 (bit 1) - select between regular port 27 (value=0, internal MAC) and extended port 29 (value=1, external client 3) */
	if (HWS_DEV_SILICON_TYPE(devNum) == Alleycat3A0)
	{
		if (macNum == 28)
		{
			/* for extended Stacking port 28: set External_Control_0 bit to work with external client 1 */
			CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, 24, EXTERNAL_CONTROL, 1, 0x1));
		}
		else if(macNum == 25)
		{
			/* for regular Stacking port 25: set External_Control_0 bit to work with internal MAC 24 */
			CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, 24, EXTERNAL_CONTROL, 0, 0x1));
		}

		if (macNum == 29)
		{
			/* for extended Stacking port 29: set External_Control_1 bit to work with external client 3 */
			CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, 24, EXTERNAL_CONTROL, (1 << 1), (1 << 1)));
		}
		else if(macNum == 27)
		{
			/* for regular Stacking port 27: set External_Control_1 bit to work with internal MAC 24 */
			CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, 24, EXTERNAL_CONTROL, 0, (1 << 1)));
		}		
	}

    if (HWS_DEV_GOP_REV(devNum) >= GOP_28NM_REV3)
    {
        /* Clear chicken bit of 10G false link-up mechanism */
        CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, macNum, MSM_PORT_MAC_CONTROL_REGISTER4, 0, (1 << 14)));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsXlgMac28nmLoopbackCfg
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
GT_STATUS mvHwsXlgMac28nmLoopbackCfg
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
      seq = &hwsXlgMac28nmSeqDb[XLGMAC_LPBK_NORMAL_SEQ];
      break;
    case RX_2_TX_LB:
      seq = &hwsXlgMac28nmSeqDb[XLGMAC_LPBK_RX2TX_SEQ];
      break;
    case TX_2_RX_LB:
      seq = &hwsXlgMac28nmSeqDb[XLGMAC_LPBK_TX2RX_SEQ];
      break;
    default:
      return GT_NOT_SUPPORTED;
    }

    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsXlgMac28nmFcStateCfg
*
* DESCRIPTION:
*       Configure Flow Control state.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*       fcState    - flow control state
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsXlgMac28nmFcStateCfg
(
    GT_U8                           devNum,
    GT_UOPT                         portGroup,
    GT_UOPT                         macNum,
    MV_HWS_PORT_FLOW_CONTROL_ENT    fcState
)
{
    MV_CFG_SEQ *seq;

    switch (fcState)
    {
        case MV_HWS_PORT_FLOW_CONTROL_DISABLE_E:
            seq = &hwsXlgMac28nmSeqDb[XLGMAC_FC_DISABLE_SEQ];
            break;
        case MV_HWS_PORT_FLOW_CONTROL_RX_TX_E:
            seq = &hwsXlgMac28nmSeqDb[XLGMAC_FC_BOTH_SEQ];
            break;
        case MV_HWS_PORT_FLOW_CONTROL_RX_ONLY_E:
            seq = &hwsXlgMac28nmSeqDb[XLGMAC_FC_RX_ONLY_SEQ];
            break;
        case MV_HWS_PORT_FLOW_CONTROL_TX_ONLY_E:
            seq = &hwsXlgMac28nmSeqDb[XLGMAC_FC_TX_ONLY_SEQ];
            break;
        default:
            return GT_NOT_SUPPORTED;
    }

    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsXlgMac28nmLoopbackStatusGet
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
GT_STATUS mvHwsXlgMac28nmLoopbackStatusGet
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 macNum,
    MV_HWS_PORT_LB_TYPE     *lbType
)
{
    GT_UREG_DATA data;

    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, XLGMAC_UNIT, macNum, MSM_PORT_MAC_CONTROL_REGISTER1, &data, 0));

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

/*******************************************************************************
* mvHwsXlgMac28nmActiveStatusGet
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
GT_STATUS mvHwsXlgMac28nmActiveStatusGet
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 macNum,
    GT_UOPT                 *numOfLanes
)
{
    GT_U32 data;

    if (numOfLanes == NULL)
    {
        return GT_BAD_PARAM;
    }

    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, XLGMAC_UNIT, macNum, MSM_PORT_MAC_CONTROL_REGISTER0, &data, 0));
    if (!((data >> 1) & 1))
    {
        /* unit is under RESET */
        return GT_OK;
    }

    *numOfLanes = 0;
    /* get number of active lanes */
    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, XLGMAC_UNIT, macNum, MSM_PORT_MAC_CONTROL_REGISTER3, &data, 0));
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
#endif

/*******************************************************************************
* mvHwsXlgMac28nmLinkStatus
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
GT_STATUS mvHwsXlgMac28nmLinkStatus
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 macNum,
    GT_BOOL                 *linkStatus
)
{
  GT_UREG_DATA data;

  CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, XLGMAC_UNIT, macNum, MSM_PORT_STATUS, &data, 0));

  *linkStatus = data & 1;
  return GT_OK;
}


#ifndef CO_CPU_RUN
/*******************************************************************************
* hwsXlgMac28nmIfClose
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
void hwsXlgMac28nmIfClose(void)
{
  hwsXlgMac28nmSeqFree();
  hwsInitDone = GT_FALSE;
}
#endif


