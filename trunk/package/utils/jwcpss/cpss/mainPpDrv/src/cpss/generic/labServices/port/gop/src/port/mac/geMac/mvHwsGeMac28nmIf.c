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
* mvHwsGeMac28nmIf.c
*
* DESCRIPTION:
*       GE MAC interface
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <mac/geMac/mvHwsGeMac28nmIf.h>
#include <mac/geMac/mvHwsGeMacDb.h>
#include <mac/xlgMac/mvHwsXlgMacDb.h>

#ifndef CO_CPU_RUN
#include <mac/geMac/mvHwsGeMacIf.h>

GT_STATUS mvHwsGeNetMac28nmReset
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       macNum,
    MV_HWS_RESET  action
);

GT_STATUS mvHwsQsgmiiMac28nmReset
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       macNum,
    MV_HWS_RESET  action
);

static char* mvHwsMacTypeGet(void)
{
  return "GEMAC_X_28nm";
}
#endif

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
GT_STATUS hwsGeMac28nmIfInit(GT_U8 devNum, MV_HWS_MAC_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsGeMacSeqInit(devNum));
    }

    funcPtrArray[GEMAC_X].macRestFunc = mvHwsGeMac28nmReset;
    funcPtrArray[GEMAC_X].macModeCfgFunc = mvHwsGeMac28nmModeCfg;
    funcPtrArray[GEMAC_X].macLinkGetFunc = mvHwsGeMac28nmLinkStatus;
    funcPtrArray[GEMAC_X].macFcStateCfgFunc = NULL;

    funcPtrArray[GEMAC_NET_X].macRestFunc = mvHwsGeNetMac28nmReset;      /* Network port implementation */
    funcPtrArray[GEMAC_NET_X].macModeCfgFunc = mvHwsGeNetMac28nmModeCfg; /* Network port implementation */
    funcPtrArray[GEMAC_NET_X].macLinkGetFunc = mvHwsGeMac28nmLinkStatus;
    funcPtrArray[GEMAC_NET_X].macFcStateCfgFunc = NULL;

    funcPtrArray[QSGMII_MAC].macRestFunc = mvHwsQsgmiiMac28nmReset;
    funcPtrArray[QSGMII_MAC].macModeCfgFunc = mvHwsQsgmiiMac28nmModeCfg;
    funcPtrArray[QSGMII_MAC].macLinkGetFunc = mvHwsGeMac28nmLinkStatus;
    funcPtrArray[QSGMII_MAC].macFcStateCfgFunc = NULL;

#ifndef CO_CPU_RUN
    funcPtrArray[GEMAC_X].macLbCfgFunc = mvHwsGeMacLoopbackCfg;
    funcPtrArray[GEMAC_X].macLbStatusGetFunc = mvHwsGeMac28nmLoopbackStatusGet;
    funcPtrArray[GEMAC_X].macActiveStatusFunc = mvHwsGeMac28nmActiveStatusGet;
    funcPtrArray[GEMAC_X].macTypeGetFunc = mvHwsMacTypeGet;

    funcPtrArray[GEMAC_NET_X].macLbCfgFunc = mvHwsGeMacLoopbackCfg;
    funcPtrArray[GEMAC_NET_X].macLbStatusGetFunc = mvHwsGeMac28nmLoopbackStatusGet;
    funcPtrArray[GEMAC_NET_X].macActiveStatusFunc = mvHwsGeMac28nmActiveStatusGet;
    funcPtrArray[GEMAC_NET_X].macTypeGetFunc = mvHwsMacTypeGet;

    funcPtrArray[QSGMII_MAC].macLbCfgFunc = mvHwsGeMacLoopbackCfg;
    funcPtrArray[QSGMII_MAC].macLbStatusGetFunc = mvHwsGeMac28nmLoopbackStatusGet;
    funcPtrArray[QSGMII_MAC].macActiveStatusFunc = mvHwsQsgmiiMac28nmActiveStatusGet;
    funcPtrArray[QSGMII_MAC].macTypeGetFunc = mvHwsMacTypeGet;
#endif

    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsGeMac28nmReset
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
GT_STATUS mvHwsGeMac28nmReset
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       macNum,
    MV_HWS_RESET  action
)
{
    MV_CFG_SEQ *seq;

    seq = (action == RESET) ? &hwsGeMacSeqDb[GEMAC_RESET_SEQ]: &hwsGeMacSeqDb[GEMAC_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsGeMac28nmModeCfg
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
GT_STATUS mvHwsGeMac28nmModeCfg
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       macNum,
    GT_UOPT       numOfLanes
)
{
    MV_CFG_SEQ *seq;

    numOfLanes = numOfLanes;

    seq = &hwsGeMacSeqDb[GEMAC_X_MODE_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));
		
	/* In Alleycat3 there is a MUX in DMA unit (connected to Serdes #10) for selecting between two modes in XLGMAC unit with MAC #29.
	   The selector (port RX interface) exists in External_control register and the configuration can be changed
	   between these two modes:
	   - Switch CPU port #31 (as Ethernet port in MII mode): External_Control_1 value=0, port RX interface is connected to 
			the internal MAC #31 of CPU port.
	   - SDMA (PEX mode): External_Control_1 bit value=1, port RX interface is connected to the external client 3 */
	if (HWS_DEV_SILICON_TYPE(devNum) == Alleycat3A0)
	{
		if (macNum == 31)
		{
			/* For Switch CPU port #31 mode: set External_Control_1 bit to work with internal MAC #31 of CPU port */
			CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, 29, EXTERNAL_CONTROL, 0, 0x2));
		}
	}

    return GT_OK;
}

/*******************************************************************************
* mvHwsGeMac28nmLinkStatus
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
GT_STATUS mvHwsGeMac28nmLinkStatus
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       macNum,
    GT_BOOL       *linkStatus
)
{
  GT_UREG_DATA data;

  CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, GEMAC_UNIT, macNum, GIG_Port_Status_Reg, &data, 0));

  *linkStatus = data & 1;

  return GT_OK;
}

/*******************************************************************************
* mvHwsGeNetMac28nmReset
*
* DESCRIPTION:
*       Set the GE MAC to reset or exit from reset (network port).
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
GT_STATUS mvHwsGeNetMac28nmReset
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       macNum,
    MV_HWS_RESET  action
)
{
    MV_CFG_SEQ *seq;

    seq = (action == RESET) ? &hwsGeMacSeqDb[GEMAC_RESET_SEQ]: &hwsGeMacSeqDb[GEMAC_UNRESET_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsGeNetMac28nmModeCfg
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
GT_STATUS mvHwsGeNetMac28nmModeCfg
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       macNum,
    GT_UOPT       numOfLanes
)
{
    MV_CFG_SEQ *seq;

    numOfLanes = numOfLanes;

    seq = &hwsGeMacSeqDb[GEMAC_NET_X_MODE_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, macNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsGeMac28nmLoopbackStatusGet
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
GT_STATUS mvHwsGeMac28nmLoopbackStatusGet
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

/*******************************************************************************
* mvHwsGeMac28nmActiveStatusGet
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
GT_STATUS mvHwsGeMac28nmActiveStatusGet
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
* mvHwsQsgmiiMac28nmActiveStatusGet
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
*       numOfLanes - number of lanes agregated in MAC
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsQsgmiiMac28nmActiveStatusGet
(
    GT_U8       devNum,
    GT_U32      portGroup,
    GT_U32      macNum,
    GT_U32      *numOfLanes
)
{
    GT_U32 data;

    if (numOfLanes == NULL)
    {
        return GT_BAD_PARAM;
    }

    *numOfLanes = 0;

    /* check if MAC unit is under RESET */
    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, GEMAC_UNIT, macNum, GIG_PORT_MAC_CONTROL_REGISTER2, &data, 0));
    if ((data >> 6) & 1)
    {
        return GT_OK;
    }

    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, GEMAC_UNIT, macNum, GIG_PORT_MAC_CONTROL_REGISTER4, &data, 0));
    /* if bit #7==0, port type is QSGMII --> number of active lanes is 4 */
    if (((data >> 7) & 1) == 0)
    {
        *numOfLanes = 4;
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsQsgmiiMac28nmReset
*
* DESCRIPTION:
*       Set the QSGMII MAC to reset or exit from reset.
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
GT_STATUS mvHwsQsgmiiMac28nmReset
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       macNum,
    MV_HWS_RESET  action
)
{
	GT_U8 macId;
	GT_U8 numOfMac = 4;
	GT_UREG_DATA data;

	data = (action == RESET) ? (1 << 7) : 0;

	for(macId = (GT_U8)macNum; macId < (((GT_U8)macNum) + numOfMac); macId++)
	{
		/* set MUX to SGMII */
		CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, macId, EXTERNAL_CONTROL, data, (1 << 7)));

		/* reset MAC */
		CHECK_STATUS(mvHwsGeNetMac28nmReset(devNum, portGroup, macId, action));
	}

	return GT_OK;
}

/*******************************************************************************
* mvHwsQsgmiiMac28nmModeCfg
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
GT_STATUS mvHwsQsgmiiMac28nmModeCfg
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       macNum,
    GT_UOPT       numOfLanes
)
{
    GT_UOPT macId;
    GT_UOPT numOfMac = 4;

    numOfLanes = numOfLanes; /* avoid warnings */

    /* for AC3/BC2 no need to do anything */
    if (HWS_DEV_GOP_REV(devNum) < GOP_28NM_REV3)
    {
        return GT_OK;
    }

    /* for BobK (and above) */
    for(macId = macNum; macId < (macNum + numOfMac); macId++)
    {
        /* enable Inband_Auto_Neg */
        CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, macId, GIG_Port_Auto_Negotiation_Configuration, (1<<2), (1 << 2)));

        /* XLGMAC is not supported for port number <= 47 */
        if ((HWS_DEV_SILICON_TYPE(devNum) == BobK) && (macNum > 47))
        {
            /* Change mac_mode_select to 0 (GEMAC) */
            CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, macId, MSM_PORT_MAC_CONTROL_REGISTER3, 0, (7 << 13)));
        }
    }

    return GT_OK;
}

/*******************************************************************************
* hwsGeMac28nmIfClose
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
void hwsGeMac28nmIfClose(void)
{
  hwsGeMacSeqFree();
  hwsInitDone = GT_FALSE;
}
#endif



