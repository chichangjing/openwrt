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
* mvHwsXPcsIf.h
*
* DESCRIPTION:
*       XPCS interface API
*
* FILE REVISION NUMBER:
*       $Revision: 32 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <port/private/mvPortModeElements.h>
#include <pcs/xPcs/mvHwsXPcsIf.h>
#include <pcs/xPcs/mvHwsXPcsDb.h>

#ifdef RUN_XPCS_DEBUG
#include <common/os/gtOs.h>
#define DEBUG_XPCS(s)              osPrintf s
#else
#define DEBUG_XPCS(s)
#endif

#ifndef CO_CPU_RUN
static char* mvHwsPcsTypeGetFunc(void)
{
  return "XPCS";
}
#endif

static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* mvHwsXPcsIfInit
*
* DESCRIPTION:
*       Init XPCS configuration sequences and IF functions.
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
GT_STATUS mvHwsXPcsIfInit(MV_HWS_PCS_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsXPscSeqInit());
    }

    funcPtrArray[XPCS].pcsResetFunc = mvHwsXPcsReset;
    funcPtrArray[XPCS].pcsModeCfgFunc = mvHwsXPcsMode;
#ifndef CO_CPU_RUN
    funcPtrArray[XPCS].pcsLbCfgFunc = mvHwsXPcsLoopBack;
    funcPtrArray[XPCS].pcsLbCfgGetFunc = mvHwsXPcsLoopBackGet;
    funcPtrArray[XPCS].pcsTestGenFunc = mvHwsXPcsTestGenCfg;
    funcPtrArray[XPCS].pcsTestGenStatusFunc = mvHwsXPcsTestGenStatus;
    funcPtrArray[XPCS].pcsActiveStatusGetFunc = mvHwsXPcsActiveStatusGet;
    funcPtrArray[XPCS].pcsSignalDetectMaskEn = mvHwsXPcsSignalDetectBypassEn;
    funcPtrArray[XPCS].pcsTypeGetFunc = mvHwsPcsTypeGetFunc;
#endif
    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsXPcsReset
*
* DESCRIPTION:
*       Set the selected PCS type and number to reset or exit from reset.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       action    - reset / unreset
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsXPcsReset
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_RESET            action
)
{
    GT_UREG_DATA data;

    data = (action == RESET) ? 0 : 1;
    CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XPCS_UNIT, pcsNum, XPCS_Global_Configuration_Reg0, data, 1));

    return GT_OK;

}

/*******************************************************************************
* mvHwsXPcsMode
*
* DESCRIPTION:
*       Set the internal mux's to the required PCS in the PI.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       fecCtrl   - FEC control
*       numOfLanes - number of lanes agregated in PCS
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsXPcsMode
(
    GT_U8                   devNum,
    GT_UOPT                 portGroup,
    GT_UOPT                 pcsNum,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_UOPT                 numOfLanes
)
{
    MV_CFG_SEQ *seq;

    fecCtrl = fecCtrl;

    CHECK_STATUS(mvHwsXPcsReset(devNum,portGroup,pcsNum,RESET));
    seq = &hwsXpcsPscSeqDb[XPCS_MODE_MISC_SEQ];
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));
    switch(numOfLanes)
    {
    case 1:
      seq = &hwsXpcsPscSeqDb[XPCS_MODE_1_LANE_SEQ];
      break;
    case 2:
      seq = &hwsXpcsPscSeqDb[XPCS_MODE_2_LANE_SEQ];
      break;
    case 4:
      seq = &hwsXpcsPscSeqDb[XPCS_MODE_4_LANE_SEQ];
      break;
    default:
      return GT_NOT_SUPPORTED;
      break;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

	if (HWS_DEV_SILICON_TYPE(devNum) != Alleycat3A0)
	{
		/* clear register used to prevent WA function execution */
		CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XPCS_UNIT, pcsNum, XPCS_Internal_Metal_Fix, 0, 0));
	}

    CHECK_STATUS(mvHwsXPcsReset(devNum,portGroup,pcsNum,UNRESET));
    return GT_OK;

}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsXPcsLoopback
*
* DESCRIPTION:
*       Set PCS loop back.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
*       lbType    - loop back type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsXPcsLoopBack
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_LB_TYPE     lbType
)
{
    MV_CFG_SEQ *seq;

    switch(lbType)
    {
    case DISABLE_LB:
      seq = &hwsXpcsPscSeqDb[XPCS_LPBK_NORMAL_SEQ];
      break;
    case RX_2_TX_LB:
      seq = &hwsXpcsPscSeqDb[XPCS_LPBK_RX2TX_SEQ];
      break;
    case TX_2_RX_LB:
      seq = &hwsXpcsPscSeqDb[XPCS_LPBK_TX2RX_SEQ];
      break;
    default:
      return GT_NOT_SUPPORTED;
      break;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsXPcsLoopBackGet
*
* DESCRIPTION:
*       Get the PCS loop back mode state.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       lbType    - loop back type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsXPcsLoopBackGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_LB_TYPE     *lbType
)
{
	GT_UREG_DATA	data;

	CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, XPCS_UNIT, pcsNum, XPCS_Global_Configuration_Reg1, &data, (0x3 << 1)));

	switch(data>>1)
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
* mvHwsXPcsTestGenCfg
*
* DESCRIPTION:
*       Set PCS internal test generator mechanisms.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pattern   - pattern to generate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsXPcsTestGenCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern
)
{
    MV_CFG_SEQ *seq;

    CHECK_STATUS(mvHwsXPcsReset(devNum,portGroup,pcsNum,RESET));

    switch (pattern)
    {
    case TEST_GEN_CJPAT:
      seq = &hwsXpcsPscSeqDb[XPCS_GEN_CJPAT_SEQ];
      break;
    case TEST_GEN_CRPAT:
      seq = &hwsXpcsPscSeqDb[XPCS_GEN_CRPAT_SEQ];
      break;
    case TEST_GEN_PRBS7:
      seq = &hwsXpcsPscSeqDb[XPCS_GEN_PRBS7_SEQ];
      break;
    case TEST_GEN_PRBS23:
      seq = &hwsXpcsPscSeqDb[XPCS_GEN_PRBS23_SEQ];
      break;
    case TEST_GEN_Normal:
      seq = &hwsXpcsPscSeqDb[XPCS_GEN_NORMAL_SEQ];
      break;
    default:
      return GT_NOT_SUPPORTED;
      break;
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    CHECK_STATUS(mvHwsXPcsReset(devNum,portGroup,pcsNum,UNRESET));

    return GT_OK;
}

/*******************************************************************************
* mvHwsXPcsTestGenStatus
*
* DESCRIPTION:
*       Get PCS internal test generator mechanisms error counters and status.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pattern   - pattern to generate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsXPcsTestGenStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern,
    MV_HWS_TEST_GEN_STATUS        *status
)
{
  GT_U32 data, tmp;
  GT_U32 accessAddr;
  GT_U32 baseAddr;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  if (status == 0)
  {
    return GT_BAD_PARAM;
  }

  mvUnitExtInfoGet(devNum, XPCS_UNIT, pcsNum, &unitAddr, &unitIndex , &pcsNum);
  baseAddr = unitAddr + pcsNum * unitIndex;
  if ((pattern == TEST_GEN_CJPAT) || (pattern == TEST_GEN_CRPAT))
  {
    accessAddr = XPCS_Tx_Packets_Cntr_LSB + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->goodFrames = data;
    accessAddr = XPCS_Tx_Packets_Cntr_MSB + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->goodFrames += (data << 16);

    accessAddr = XPCS_Rx_Bad_Packets_Cntr_LSB_Lane0 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->totalErrors = data;
    accessAddr = XPCS_Rx_Bad_Packets_Cntr_MSB_Lane0 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->totalErrors += (data << 16);

    accessAddr = XPCS_Rx_Bad_Packets_Cntr_LSB_Lane1 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    tmp = data;
    accessAddr = XPCS_Rx_Bad_Packets_Cntr_MSB_Lane1 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    tmp += (data << 16);
    status->totalErrors += tmp;

    accessAddr = XPCS_Rx_Bad_Packets_Cntr_LSB_Lane2 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    tmp = data;
    accessAddr = XPCS_Rx_Bad_Packets_Cntr_MSB_Lane2 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    tmp += (data << 16);
    status->totalErrors += tmp;

    accessAddr = XPCS_Rx_Bad_Packets_Cntr_LSB_Lane3 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    tmp = data;
    accessAddr = XPCS_Rx_Bad_Packets_Cntr_MSB_Lane3 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    tmp += (data << 16);
    status->totalErrors += tmp;

    accessAddr = XPCS_Rx_Bad_Packets_Cntr_LSB_Lane4 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    tmp = data;
    accessAddr = XPCS_Rx_Bad_Packets_Cntr_MSB_Lane4 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    tmp += (data << 16);
    status->totalErrors += tmp;

    accessAddr = XPCS_Rx_Bad_Packets_Cntr_LSB_Lane5 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    tmp = data;
    accessAddr = XPCS_Rx_Bad_Packets_Cntr_MSB_Lane5 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    tmp += (data << 16);
    status->totalErrors += tmp;

    status->checkerLock = 0;
  }
  else if ((pattern == TEST_GEN_PRBS7) || (pattern == TEST_GEN_PRBS23))
  {
    accessAddr = XPCS_PRBS_Error_Cntr_Lane0 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->totalErrors = data;
    accessAddr = XPCS_PRBS_Error_Cntr_Lane1 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->totalErrors += data;
    accessAddr = XPCS_PRBS_Error_Cntr_Lane2 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->totalErrors += data;
    accessAddr = XPCS_PRBS_Error_Cntr_Lane3 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->totalErrors += data;
    accessAddr = XPCS_PRBS_Error_Cntr_Lane4 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->totalErrors += data;
    accessAddr = XPCS_PRBS_Error_Cntr_Lane5 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->totalErrors += data;

    accessAddr = XPCS_PRBS_Error_Cntr_Lane0 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->checkerLock = data & 1;
    accessAddr = XPCS_PRBS_Error_Cntr_Lane1 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->checkerLock += (data & 1) << 1;
    accessAddr = XPCS_PRBS_Error_Cntr_Lane2 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->checkerLock += (data & 1) << 2;
    accessAddr = XPCS_PRBS_Error_Cntr_Lane3 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->checkerLock += (data & 1) << 3;
    accessAddr = XPCS_PRBS_Error_Cntr_Lane4 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->checkerLock += (data & 1) << 4;
    accessAddr = XPCS_PRBS_Error_Cntr_Lane5 + baseAddr;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &data, 0));
    status->checkerLock += (data & 1) << 5;
  }
  else
  {
    return GT_BAD_PARAM;
  }
  return GT_OK;
}

/*******************************************************************************
* mvHwsXPcsActiveStatusGet
*
* DESCRIPTION:
*       Return number of PCS active lanes or 0, if current PCS isn't active.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*
* OUTPUTS:
*       numOfLanes - number of lanes agregated in PCS
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsXPcsActiveStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_U32                  *numOfLanes
)
{
    GT_U32 data;

    if (numOfLanes == NULL)
    {
      return GT_BAD_PARAM;
    }

    CHECK_STATUS(genUnitRegisterGet(devNum, portGroup, XPCS_UNIT, pcsNum, XPCS_Global_Configuration_Reg0, &data, 0));

    *numOfLanes = 0;
    /* check unreset bit status */
    if (!(data & 1))
    {
        /* unit is under RESET */
        return GT_OK;
    }
    /* get number of active lanes */
    switch ((data >> 5) & 3)
    {
    case 0:
        *numOfLanes = 1;
        break;
    case 1:
        *numOfLanes = 2;
        break;
    case 2:
        *numOfLanes = 4;
        break;
    case 3:
        *numOfLanes = 6;
        break;
    default:
        *numOfLanes = 0;
        break;
    }

    return GT_OK;
}
#endif

#ifndef CO_CPU_RUN
/*******************************************************************************
* hwsXPcsIfClose
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
void hwsXPcsIfClose(void)
{
  hwsXPcsSeqFree();
  hwsInitDone = GT_FALSE;
}
#endif

#ifndef CO_CPU_RUN
static GT_U32 rxauiTryNum = 5;
static int rxauiFilterDelay = 3;
/*
1.  check sync indication for each lane /Cider/EBU/Lion2/Lion2_B1 {B1 freeze}/Lion2_B1 Units/GOP/XPCS %p Units/Lane 0 Registers/Lane Status and Interrupt/Lane Status bit 4
2.  If not all are synced then perform rx reset/unreset for each lane /Cider/EBU/Lion2/Lion2_B1 {B1 freeze}/Lion2_B1 Units/GOP/XPCS %p Units/Lane 0 Registers/Lane Configuration/Lane Configuration 0 bit 1
3.  Wait on signal detect change interrupt /Cider/EBU/Lion2/Lion2_B1 {B1 freeze}/Lion2_B1 Units/GOP/MMPCS_IP %P Units/PCS40G Common Interrupt Cause bit 12 (make sure it's not set for sync change interrupt)
4.  Upon interrupt check Cider/EBU/Lion2/Lion2_B1 {B1 freeze}/Lion2_B1 Units/GOP/COMPHY_H %t Registers/<COMPHY_H(r2p1)> COMPHY_H Units/Page 1/Squelch and FFE Setting bit 14
5.  If 1 then goto 3
6.  If 0 then goto 1
*/
GT_STATUS mvHwsXPcsConnectWa
(
 GT_U8                   devNum,
 GT_U32                  portGroup,
 GT_U32                  portNum
)
{
    GT_U32 regAddr, data, i;
    GT_BOOL syncOk;
    GT_U32 unitAddr, baseAddr;
    GT_U32 unitIndex;
    GT_U32 macNum, pcsNum, pcsNumMaster = 0, sdSize, *sdVec;
    GT_U32 firstLaneNum, currSqlSignal, tmp;
    GT_U32 tryNum;

    hwsPortsParamsCfgGet(devNum, portGroup, portNum, RXAUI, &macNum, &pcsNum, &sdSize, &sdVec);
    mvUnitExtInfoGet(devNum, XPCS_UNIT, pcsNum, &unitAddr, &unitIndex , &pcsNum);
    regAddr = XPCS_Internal_Metal_Fix + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    if (data != 0x8000)
    {
        /* unit not initalize or under TRx training, delete */
        return GT_OK;
    }

    DEBUG_XPCS(("\nEnter mvHwsXPcsConnectWa for port %d. (device %d)", portNum, devNum));
    firstLaneNum = sdVec[0];

    /* check if it is RXAUI_MEDIA */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, MG_Media_Interface_Reg, &data, 0));
    if (!((data >> firstLaneNum) & 1))
    {
        return GT_OK;
    }

    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

    /* check Signal on both lanes */
    tmp = 0;
    baseAddr = unitAddr + unitIndex * firstLaneNum;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x18, &data, 0));
    currSqlSignal = (data & 1);
    baseAddr = unitAddr + unitIndex * (firstLaneNum+1);
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x18, &data, 0));
    currSqlSignal += (data & 1) << 1;

    tryNum = 0;
    for (i = 0; (i < 10) && (tryNum < 2); i++)
    {
        hwsOsExactDelayPtr(devNum, portGroup, rxauiFilterDelay);
        baseAddr = unitAddr + unitIndex * firstLaneNum;
        CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x18, &data, 0));
        tmp = (data & 1);
        baseAddr = unitAddr + unitIndex * (firstLaneNum+1);
        CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x18, &data, 0));
        tmp += (data & 1) << 1;
        if (tmp != currSqlSignal)
        {
            tmp = currSqlSignal;
            i = 0;
            tryNum++;
            continue;
        }
    }
    if (currSqlSignal != 0)
    {
        DEBUG_XPCS(("\nSignal detect problem on device %d, pcs %d, data is %d.", devNum, pcsNum, currSqlSignal));
        return GT_OK;
    }

    DEBUG_XPCS(("\nWANT check XPCS Rx synchronization"));
    /* check XPCS Rx synchronization on all (two) lanes [4] */
    mvUnitExtInfoGet(devNum, XPCS_UNIT, pcsNum, &unitAddr, &unitIndex , &pcsNum);
    syncOk = GT_TRUE;
    for (i = 0; (i < 4) && (syncOk); i++)
    {
        regAddr = XPCS_Lane_Status_Lane0 + i*0x44 + unitAddr + pcsNum * unitIndex;

        CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
        syncOk = ((data >> 4) & 1) ? GT_TRUE : GT_FALSE;
    }

    if (syncOk)
    {
        /* check Deskew acquired */
        regAddr = XPCS_Global_Status_Reg + unitAddr + pcsNum * unitIndex;
        CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
        if ((data & 3) == 3)
        {
            /* check Global Deskew Error Counter */
            regAddr = XPCS_Global_Deskew_Error_Counter + unitAddr + pcsNum * unitIndex;
            CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
            CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
            if (data == 0)
            {
                DEBUG_XPCS(("\nDeskew acquired on device %d, pcs %d.", devNum, pcsNum));
                return GT_OK;
            }
        }
    }

    DEBUG_XPCS(("\nEnter WA loop"));
    tryNum = 0;
    /* run WA - reset current port, if (portNum%4 == 0) or port = portNum-2 */
    if ((portNum != 9) && ((portNum % 4) != 0))
    {
        hwsPortsParamsCfgGet(devNum, portGroup, portNum - 2, RXAUI, &macNum, &pcsNumMaster, &sdSize, &sdVec);
        firstLaneNum = sdVec[0];
    }

    do
    {
        /* reset lanes */
        if ((portNum != 9) && ((portNum % 4) != 0))
        {
            /* mask interrupt of master PCS */
            regAddr = 0x64 + unitAddr + pcsNumMaster * unitIndex;
            CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, (1 << 5)));

            /* reset master PCS */
            for (i = 0; i < 4; i++)
            {
                regAddr = XPCS_Lane0_configuration_Reg + i*0x44 + unitAddr + pcsNumMaster * unitIndex;

                CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, 2));
            }
            hwsOsExactDelayPtr(devNum, portGroup, 1);
        }
        /* reset current PCS */
        for (i = 0; i < 4; i++)
        {
            regAddr = XPCS_Lane0_configuration_Reg + i*0x44 + unitAddr + pcsNum * unitIndex;

            CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, 2));
        }
        hwsOsExactDelayPtr(devNum, portGroup, 1);
        /* unreset current PCS lanes */
        for (i = 0; i < 4; i++)
        {
            regAddr = XPCS_Lane0_configuration_Reg + i*0x44 + unitAddr + pcsNum * unitIndex;

            CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 2, 2));
        }
        /* unreset master PCS lanes */
        if (((portNum != 9) && ((portNum % 4) != 0)))
        {
            /* clean interrupt again */
            regAddr = 0x60 + unitAddr + pcsNumMaster * unitIndex;
            CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));

            hwsOsExactDelayPtr(devNum, portGroup, 1);
            for (i = 0; i < 4; i++)
            {
                regAddr = XPCS_Lane0_configuration_Reg + i*0x44 + unitAddr + pcsNumMaster * unitIndex;

                CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 2, 2));
            }
            /* unmask interrupt of master PCS */
            regAddr = 0x64 + unitAddr + pcsNumMaster * unitIndex;
            CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, (1 << 5), (1 << 5)));
        }

        hwsOsExactDelayPtr(devNum, portGroup, 1);
        /* chek sync */
        syncOk = GT_TRUE;
        for (i = 0; (i < 4) && (syncOk); i++)
        {
            regAddr = XPCS_Lane_Status_Lane0 + i*0x44 + unitAddr + pcsNum * unitIndex;

            CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
            syncOk = ((data >> 4) & 1) ? GT_TRUE : GT_FALSE;
        }
        /* check Deskew acquired */
        if (syncOk)
        {
            regAddr = XPCS_Global_Status_Reg + unitAddr + pcsNum * unitIndex;
            CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
            syncOk = ((data & 3) == 3) ? GT_TRUE : GT_FALSE;
            DEBUG_XPCS(("\nRun WA on XPCS %d. Check DESKEW is %d (syncOk = %d).", pcsNum, data, syncOk));

            if (syncOk)
            {
                /* check Global Deskew Error Counter */
                regAddr = XPCS_Global_Deskew_Error_Counter + unitAddr + pcsNum * unitIndex;
                CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
                CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
                syncOk = (data == 0) ? GT_TRUE : GT_FALSE;
                DEBUG_XPCS(("\nRun WA on XPCS %d. Check DESKEW ERROR is %d (syncOk = %d).", pcsNum, data, syncOk));
            }
        }

        DEBUG_XPCS(("\nRun WA on XPCS %d. (device %d, tryNum %d)", pcsNum, devNum, tryNum));
        tryNum++;

    }while ((tryNum < rxauiTryNum) && (syncOk == GT_FALSE));

    /* clean interrupt again */
    regAddr = 0x60 + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));

    if ((tryNum == rxauiTryNum) && (syncOk == GT_FALSE))
    {
        DEBUG_XPCS(("\n WA FAILED on XPCS %d. (device %d)", pcsNum, devNum));
        return GT_FAIL;
    }
    return GT_OK;
}

GT_U32 DeskewAcquired = 0;

GT_STATUS mvHwsXPcsConnect
(
 GT_U8                   devNum,
 GT_U32                  portGroup,
 GT_U32                  portNum
)
{
    GT_U32 regAddr, data, i;
    GT_BOOL syncOk;
    GT_U32 unitAddr, baseAddr;
    GT_U32 unitIndex;
    GT_U32 macNum, pcsNum, pcsNumMaster = 0, sdSize, *sdVec;
    GT_U32 firstLaneNum, currSqlSignal, tmp;
    GT_U32 tryNum;

    DEBUG_XPCS(("\nEnter mvHwsXPcsConnect for port %d. (device %d)", portNum, devNum));
    hwsPortsParamsCfgGet(devNum, portGroup, portNum, RXAUI, &macNum, &pcsNum, &sdSize, &sdVec);
    firstLaneNum = sdVec[0];

    /* check if it is RXAUI_MEDIA */
    CHECK_STATUS(genRegisterGet(devNum, portGroup, MG_Media_Interface_Reg, &data, 0));
    if (!((data >> firstLaneNum) & 1))
    {
        return GT_OK;
    }

    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);

    /* check Squelch Detected [14] on both lanes */
    tmp = 0;
    baseAddr = unitAddr + unitIndex * firstLaneNum + SERDES_PHY_REGS;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x18, &data, 0));
    currSqlSignal = ((data >> 14) & 1);
    baseAddr = unitAddr + unitIndex * (firstLaneNum+1) + SERDES_PHY_REGS;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x18, &data, 0));
    currSqlSignal += ((data >> 14) & 1) << 1;

    tryNum = 0;
    for (i = 0; (i < 10) && (tryNum < 2); i++)
    {
        baseAddr = unitAddr + unitIndex * firstLaneNum + SERDES_PHY_REGS;
        CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x18, &data, 0));
        tmp = ((data >> 14) & 1);
        baseAddr = unitAddr + unitIndex * (firstLaneNum+1) + SERDES_PHY_REGS;
        CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + 0x18, &data, 0));
        tmp += ((data >> 14) & 1) << 1;
        if (tmp != currSqlSignal)
        {
            tmp = currSqlSignal;
            i = 0;
            tryNum++;
            continue;
        }
    }
    if (currSqlSignal != 0)
    {
        DEBUG_XPCS(("\nSignal detect problem on device %d, pcs %d, data is %d.", devNum, pcsNum, currSqlSignal));
        return GT_OK;
    }

    DEBUG_XPCS(("\nWANT check XPCS Rx synchronization"));
    /* check XPCS Rx synchronization on all (two) lanes [4] */
    mvUnitExtInfoGet(devNum, XPCS_UNIT, pcsNum, &unitAddr, &unitIndex , &pcsNum);
    syncOk = GT_TRUE;
    for (i = 0; (i < 4) && (syncOk); i++)
    {
        regAddr = XPCS_Lane_Status_Lane0 + i*0x44 + unitAddr + pcsNum * unitIndex;

        CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
        syncOk = ((data >> 4) & 1) ? GT_TRUE : GT_FALSE;
    }

    if (syncOk)
    {
        /* check Deskew acquired */
        regAddr = XPCS_Global_Status_Reg + unitAddr + pcsNum * unitIndex;
        CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
        if ((data & 3) == 3)
        {
            hwsOsExactDelayPtr(devNum, portGroup, DeskewAcquired);
            /* check Global Deskew Error Counter */
            regAddr = XPCS_Global_Deskew_Error_Counter + unitAddr + pcsNum * unitIndex;
            CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
            CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
            if (data == 0)
            {
                DEBUG_XPCS(("\nDeskew acquired on device (no errors) %d, pcs %d.", devNum, pcsNum));
                return GT_OK;
            }
        }
    }

    DEBUG_XPCS(("\nEnter WA loop"));
    tryNum = 0;
    do
    {
        /* run WA - reset current port, if (portNum%4 == 0) or port = portNum-2 */
        if ((portNum != 9) && ((portNum % 4) != 0))
        {
            hwsPortsParamsCfgGet(devNum, portGroup, portNum - 2, RXAUI, &macNum, &pcsNumMaster, &sdSize, &sdVec);
            firstLaneNum = sdVec[0];
        }
        /* reset lanes */
        if ((portNum != 9) && ((portNum % 4) != 0))
        {
            /* reset master PCS */
            for (i = 0; i < 4; i++)
            {
                regAddr = XPCS_Lane0_configuration_Reg + i*0x44 + unitAddr + pcsNumMaster * unitIndex;

                CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, 2));
            }
            hwsOsExactDelayPtr(devNum, portGroup, 1);
        }
        /* reset current PCS */
        for (i = 0; i < 4; i++)
        {
            regAddr = XPCS_Lane0_configuration_Reg + i*0x44 + unitAddr + pcsNum * unitIndex;

            CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, 2));
        }
        hwsOsExactDelayPtr(devNum, portGroup, 1);
        /* unreset current PCS lanes */
        for (i = 0; i < 4; i++)
        {
            regAddr = XPCS_Lane0_configuration_Reg + i*0x44 + unitAddr + pcsNum * unitIndex;

            CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 2, 2));
        }
        /* unreset master PCS lanes */
        if (((portNum != 9) && ((portNum % 4) != 0)))
        {
            hwsOsExactDelayPtr(devNum, portGroup, 1);
            for (i = 0; i < 4; i++)
            {
                regAddr = XPCS_Lane0_configuration_Reg + i*0x44 + unitAddr + pcsNumMaster * unitIndex;

                CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 2, 2));
            }
        }

        hwsOsExactDelayPtr(devNum, portGroup, 1);
        /* chek sync */
        syncOk = GT_TRUE;
        for (i = 0; (i < 4) && (syncOk); i++)
        {
            regAddr = XPCS_Lane_Status_Lane0 + i*0x44 + unitAddr + pcsNum * unitIndex;

            CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
            syncOk = ((data >> 4) & 1) ? GT_TRUE : GT_FALSE;
        }
        /* check Deskew acquired */
        if (syncOk)
        {
            regAddr = XPCS_Global_Status_Reg + unitAddr + pcsNum * unitIndex;
            CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
            syncOk = ((data & 3) == 3) ? GT_TRUE : GT_FALSE;
            DEBUG_XPCS(("\nRun WA on XPCS %d. Check DESKEW is %d (syncOk = %d).", pcsNum, data, syncOk));

            if (syncOk)
            {
                /* check Global Deskew Error Counter */
                regAddr = XPCS_Global_Deskew_Error_Counter + unitAddr + pcsNum * unitIndex;
                CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
                CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
                syncOk = (data == 0) ? GT_TRUE : GT_FALSE;
                DEBUG_XPCS(("\nRun WA on XPCS %d. Check DESKEW ERROR is %d (syncOk = %d).", pcsNum, data, syncOk));
            }
        }

        DEBUG_XPCS(("\nRun WA on XPCS %d. (device %d, tryNum %d)", pcsNum, devNum, tryNum));
        tryNum++;

    }while ((tryNum < rxauiTryNum) && (syncOk == GT_FALSE));

    return GT_OK;
}

/*******************************************************************************
* mvHwsXPcsSignalDetectBypassEn
*
* DESCRIPTION:
*       Set all related PCS with Signal Detect Bypass value (1/0).
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
GT_STATUS mvHwsXPcsSignalDetectBypassEn
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_BOOL                 maskEn
)
{
    GT_U32 i, data, regAddr;


    data = (maskEn == GT_TRUE) ? 1 : 0;
    for (i = 0; i < 4; i++)
    {
        regAddr = XPCS_Lane0_configuration_Reg + i*0x44;
        CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XPCS_UNIT, pcsNum, regAddr, (data << 6), (1 << 6)));
    }

    return GT_OK;
}
#endif
