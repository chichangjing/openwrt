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
* mvHwsXgMac28nmIf.c
*
* DESCRIPTION:
*       XGL MAC (for bobcat2)
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <mac/xgMac/mvHwsXgMac28nmIf.h>
#include <mac/xlgMac/mvHwsXlgMac28nmDb.h>
#include <mac/xgMac/mvHwsXgMacDb.h>


#ifndef CO_CPU_RUN
static char* mvHwsMacTypeGet(void)
{
  return "XGMAC_28nm";
}
#endif

static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* hwsXgMac28nmIfInit
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
GT_STATUS hwsXgMac28nmIfInit(GT_U8 devNum, MV_HWS_MAC_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsXgMacSeqInit(devNum));
    }

    funcPtrArray[XGMAC].macRestFunc = mvHwsXgMac28nmReset;
    funcPtrArray[XGMAC].macLinkGetFunc = mvHwsXgMac28nmLinkStatus;
    funcPtrArray[XGMAC].macModeCfgFunc = mvHwsXgMac28nmModeCfg;

#ifndef CO_CPU_RUN
    funcPtrArray[XGMAC].macTypeGetFunc = mvHwsMacTypeGet;
    funcPtrArray[XGMAC].macLbCfgFunc = mvHwsXgMac28nmLoopbackCfg;
    funcPtrArray[XGMAC].macLbStatusGetFunc = mvHwsXgMac28nmLoopbackStatusGet;
    funcPtrArray[XGMAC].macActiveStatusFunc = mvHwsXgMac28nmActiveStatusGet;
#endif
    funcPtrArray[XGMAC].macFcStateCfgFunc = NULL;

    /* specific function */
    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsXgMac28nmReset
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
GT_STATUS mvHwsXgMac28nmReset
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

    seq = (action == RESET)? &hwsXlgMac28nmSeqDb[XLGMAC_RESET_SEQ] : &hwsXlgMac28nmSeqDb[XLGMAC_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsXgMac28nmModeCfg
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
GT_STATUS mvHwsXgMac28nmModeCfg
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 macNum,
    GT_UOPT                 numOfLanes
)
{
    MV_CFG_SEQ *seq;

    numOfLanes = numOfLanes;

    seq = &hwsXgMacSeqDb[XGMAC_MODE_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    /* Rx XLG to RxDMA interface is 64 bits wide.*/
    CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, macNum, MSM_PORT_MAC_CONTROL_REGISTER4, (1 << 9), (1 << 9)));

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

    return GT_OK;
}

/*******************************************************************************
* mvHwsXgMac28nmLoopbackCfg
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
GT_STATUS mvHwsXgMac28nmLoopbackCfg
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
      return GT_BAD_PARAM;
    }

    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsXgMac28nmLoopbackStatusGet
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
GT_STATUS mvHwsXgMac28nmLoopbackStatusGet
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
#endif

/*******************************************************************************
* mvHwsXgMac28nmLinkStatus
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
GT_STATUS mvHwsXgMac28nmLinkStatus
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
* mvHwsXgMac28nmActiveStatusGet
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
GT_STATUS mvHwsXgMac28nmActiveStatusGet
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 macNum,
    GT_UOPT                 *numOfLanes
)
{
    GT_UREG_DATA data;

    if (numOfLanes == NULL)
    {
        return GT_BAD_PARAM;
    }

    *numOfLanes = 0;
    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, XLGMAC_UNIT, macNum, MSM_PORT_MAC_CONTROL_REGISTER0, &data, 0));
    if (((data >> 1) & 1) != 0)
    {
        /* unit is under RESET */
        return GT_OK;
    }
    /* check if XPCS used */
    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, XLGMAC_UNIT, macNum, MSM_PORT_MAC_CONTROL_REGISTER4, &data, 0));
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
* hwsXgMac28nmIfClose
*
* DESCRIPTION:
*       Release all system resources allocated by XG MAC IF functions.
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
void hwsXgMac28nmIfClose(void)
{
#ifndef CO_CPU_RUN
  hwsXgMacSeqFree();
#endif
  hwsInitDone = GT_FALSE;
}

