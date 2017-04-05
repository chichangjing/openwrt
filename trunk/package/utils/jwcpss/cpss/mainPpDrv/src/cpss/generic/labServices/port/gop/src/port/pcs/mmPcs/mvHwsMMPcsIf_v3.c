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
* mvHwsMMPcsIf_v3.c
*
* DESCRIPTION:
*       MMPCS V3.0 interface API for puma3B0
*
* FILE REVISION NUMBER:
*       $Revision: 10 $
*
*******************************************************************************/

#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <port/private/mvPortModeElements.h>
#include <pcs/mmPcs/mvHwsMMPcsIf.h>
#include <pcs/mmPcs/mvHwsMMPcsV2If.h>
#include <pcs/mmPcs/mvHwsMMPcsV3If.h>
#include <pcs/mmPcs/mvHwsMMPcsDbRev3.h>

#ifdef RUN_PCS_DEBUG
#include <common/os/gtOs.h>
#define DEBUG_PCS(s)              osPrintf s
#else
#define DEBUG_PCS(s)
#endif



static char* mvHwsPcsTypeGetFunc(void)
{
    return "MMPCS V3.0";
}

static  GT_U32 indArr[4] = {0, 0x400, 0x600, 0x800};
static GT_BOOL hwsInitDone = GT_FALSE;

static GT_U32 mmPcsFabWa = 10;

/*******************************************************************************
* mvHwsMMPcsV3IfInit
*
* DESCRIPTION:
*       Init MMPCS configuration sequences and IF functions.
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
GT_STATUS mvHwsMMPcsV3IfInit(MV_HWS_PCS_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsMMPscSeqInit());
        CHECK_STATUS(hwsMMPscRev3SeqInit());
    }

    funcPtrArray[MMPCS].pcsLbCfgFunc = mvHwsMMPcsLoopBack;
    funcPtrArray[MMPCS].pcsTestGenFunc = mvHwsMMPcsTestGenCfg;
    funcPtrArray[MMPCS].pcsTestGenStatusFunc = mvHwsMMPcsTestGenStatus;
    funcPtrArray[MMPCS].pcsTypeGetFunc = mvHwsPcsTypeGetFunc;
    funcPtrArray[MMPCS].pcsFecCfgFunc = mvHwsMMPcsFecConfig;
    funcPtrArray[MMPCS].pcsFecCfgGetFunc = mvHwsMMPcsFecConfigGet;
    funcPtrArray[MMPCS].pcsActiveStatusGetFunc = mvHwsMMPcsActiveStatusGet;

    funcPtrArray[MMPCS].pcsRxResetFunc = mvHwsMMPcsV2RxReset;
    funcPtrArray[MMPCS].pcsExtPllCfgFunc = mvMmPcsV2ExtPllCfg;
    funcPtrArray[MMPCS].pcsCheckGearBoxFunc = mvHwsMMPcsV2CheckGearBox;

    funcPtrArray[MMPCS].pcsResetFunc = mvHwsMMPcsV3Reset;
    funcPtrArray[MMPCS].pcsModeCfgFunc = mvHwsMMPcsV3Mode;
    funcPtrArray[MMPCS].pcsSignalDetectMaskEn = mvHwsMMPcsV3SignalDetectMaskSet;

    hwsInitDone = GT_TRUE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsV3Reset
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
GT_STATUS mvHwsMMPcsV3Reset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_RESET            action
)
{
    GT_U32 regAddr;
    GT_U32 data;
    GT_U32 countVal;
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    MV_CFG_SEQ *seq;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    /* read PCS mode */
    regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    countVal = (data >> 8) & 3;
    countVal = (countVal == 0) ? 1 : countVal * 2;

    if (action == RESET)
    {
        if (pcsNum < 16)
        {
            switch(countVal)
            {
            case 1:
                seq = &hwsMmPscSeqDbRev3[MMPCS_RESET_1_LANE_SEQ];
                break;
            case 2:
                seq = &hwsMmPscSeqDbRev3[MMPCS_RESET_2_LANE_SEQ];
                break;
            case 4:
                seq = &hwsMmPscSeqDbRev3[MMPCS_RESET_4_LANE_SEQ];
                break;
            default:
                return GT_BAD_PARAM;
            }
        }
        else
        {
            switch(countVal)
            {
            case 1:
                seq = &hwsMmPscSeqDbRev3[MMPCS_RESET_1_LANE_SEQ];
                break;
            case 2:
                seq = &hwsMmPscSeqDbRev3[MMPCS_FABRIC_RESET_2_LANE_SEQ];
                break;
            case 4:
                seq = &hwsMmPscSeqDbRev3[MMPCS_FABRIC_RESET_4_LANE_SEQ];
                break;
            default:
                return GT_BAD_PARAM;
            }
        }
    }
    else
    {
        if (pcsNum < 16)
        {
            switch(countVal)
            {
            case 1:
                seq = &hwsMmPscSeqDbRev3[MMPCS_UNRESET_1_LANE_SEQ];
                break;
            case 2:
                seq = &hwsMmPscSeqDbRev3[MMPCS_UNRESET_2_LANE_SEQ];
                break;
            case 4:
                seq = &hwsMmPscSeqDbRev3[MMPCS_UNRESET_4_LANE_SEQ];
                break;
            default:
                return GT_BAD_PARAM;
            }
        }
        else
        {
            switch(countVal)
            {
            case 1:
                seq = &hwsMmPscSeqDbRev3[MMPCS_UNRESET_1_LANE_SEQ];
                break;
            case 2:
                seq = &hwsMmPscSeqDbRev3[MMPCS_FABRIC_UNRESET_2_LANE_SEQ];
                break;
            case 4:
                seq = &hwsMmPscSeqDbRev3[MMPCS_FABRIC_UNRESET_4_LANE_SEQ];
                break;
            default:
                return GT_BAD_PARAM;
            }
        }
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    if ((action == UNRESET) && (pcsNum >= 16) && (countVal == 4))
    {
        GT_U32 i;

        /* read interrupts */
        regAddr = PCS40G_COMMON_INTERRUPT_CAUSE + unitAddr + pcsNum * unitIndex;
        CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
        for (i = 0; i < mmPcsFabWa; i++)
        {
            CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
            if (data & (1 << 5))
            {
                DEBUG_PCS(("\nRun MMPCS WA on device %d, pcs %d (i = %d).", devNum, pcsNum, i));
                /* reset / unreset */
                CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, hwsMmPscSeqDbRev3[MMPCS_FABRIC_RESET_4_LANE_SEQ].cfgSeq,
                                          hwsMmPscSeqDbRev3[MMPCS_FABRIC_RESET_4_LANE_SEQ].cfgSeqSize));
                hwsOsExactDelayPtr(devNum, portGroup, 1);
                CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, hwsMmPscSeqDbRev3[MMPCS_FABRIC_UNRESET_4_LANE_SEQ].cfgSeq,
                                          hwsMmPscSeqDbRev3[MMPCS_FABRIC_UNRESET_4_LANE_SEQ].cfgSeqSize));
                hwsOsExactDelayPtr(devNum, portGroup, 1);
                continue;
            }
            else
            {
                break;
            }
        }

        /* unmask tx_gb_fifo_full interrupt on lane 0 only */
        regAddr = PCS40G_COMMON_INTERRUPT_MASK + unitAddr + pcsNum * unitIndex;
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, (1 << 5), (1 << 5)));
        if (data & (1 << 5))
        {
            return GT_FAIL;
        }
    }

    return GT_OK;
}


/*******************************************************************************
* mvHwsMMPcsV3Mode
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
GT_STATUS mvHwsMMPcsV3Mode
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_U32                  numOfLanes
)
{
    MV_CFG_SEQ *seq;
    GT_U32 regAddr;
    GT_U32 data;
    GT_U32 mask;
    GT_U32 i;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    /* EXTERNAL_CONTROL - Select recovered clock 0 per pair of cores */
    mvUnitInfoGet(devNum, XLGMAC_UNIT, &unitAddr, &unitIndex);
    regAddr = unitAddr + unitIndex * pcsNum + EXTERNAL_CONTROL;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, 3));

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    /* clear register used to prevent WA function execution */
    regAddr = FEC_DEC_DMA_WR_DATA + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, 0));

    if ((pcsNum >= 16) && (numOfLanes == 4))
    {
        /* mask tx_gb_fifo_full interrupt on lane 0 only */
        regAddr = PCS40G_COMMON_INTERRUPT_MASK + unitAddr + pcsNum * unitIndex;
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, (1 << 5)));
        /* clear interrupts */
        regAddr = PCS40G_COMMON_INTERRUPT_CAUSE + unitAddr + pcsNum * unitIndex;
        CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    }
    /* config FEC */
    for (i = 0; i < numOfLanes; i++)
    {
        if (pcsNum < 16)
        {
            /* access to network PCS */
            regAddr = PCS40G_COMMON_CONTROL + unitAddr + (pcsNum+i) * unitIndex;
        }
        else
        {
            /* access to fabric PCS */
            regAddr = PCS40G_COMMON_CONTROL + unitAddr + (pcsNum * unitIndex) + indArr[i];
        }
        data = (fecCtrl == FEC_ON) ? (1 << 10) : 0;
        mask = (1 << 10) + (1 << 7); /* always set bit 7 to 0; */
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, data, mask));
    }

    /* reduce the probability of the link loss to about once in ~1000 times */
    regAddr = CHANNEL0_RX_GB_FIFO_CONFIGURATION + unitAddr + pcsNum * unitIndex;
    for (i = 0; i < 4; i++)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr + i*4, 0x18, 0));
    }

    if (pcsNum < 16)
    {
        switch (numOfLanes)
        {
        case 1:
            seq = &hwsMmPscSeqDbRev3[MMPCS_MODE_1_LANE_SEQ];
            break;
        case 2:
            seq = &hwsMmPscSeqDbRev3[MMPCS_MODE_2_LANE_SEQ];
            break;
        case 4:
            seq = &hwsMmPscSeqDbRev3[MMPCS_MODE_4_LANE_SEQ];
            break;
        default:
            return GT_NOT_SUPPORTED;
        }
    }
    else
    {
        switch (numOfLanes)
        {
        case 1:
            seq = &hwsMmPscSeqDbRev3[MMPCS_FABRIC_MODE_1_LANE_SEQ];
            break;
        case 2:
            seq = &hwsMmPscSeqDbRev3[MMPCS_FABRIC_MODE_2_LANE_SEQ];
            break;
        case 4:
            seq = &hwsMmPscSeqDbRev3[MMPCS_FABRIC_MODE_4_LANE_SEQ];
            break;
        default:
            return GT_NOT_SUPPORTED;
        }
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}


/*******************************************************************************
* mvHwsMMPcsV3SignalDetectMaskSet
*
* DESCRIPTION:
*       Set all related PCS with Signal Detect Mask value (1/0).
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
GT_STATUS mvHwsMMPcsV3SignalDetectMaskSet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_BOOL                 maskEn
)
{
    GT_U32 regAddr, data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 countVal, i;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum*unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    countVal = (data >> 8) & 3;
    countVal = (countVal == 0) ? 1 : countVal * 2;

    for (i = 0; i < countVal; i++)
    {
        if (pcsNum < 16)
        {
            /* access to network PCS */
            regAddr = CHANNEL_CONFIGURATION + unitAddr + (pcsNum + i)*unitIndex;
        }
        else
        {
            /* access to fabric PCS */
            regAddr = CHANNEL_CONFIGURATION + unitAddr + (pcsNum * unitIndex) + indArr[i];
        }
        data = (maskEn == GT_TRUE) ? (1 << 14) : 0;
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, data, (1 << 14)));
    }

    return GT_OK;
}

/*******************************************************************************
* hwsMMPcsRev3IfClose
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
void hwsMMPcsRev3IfClose(void)
{
  hwsMMPcsSeqFree();
  hwsMMPcsRev3SeqFree();
  hwsInitDone = GT_FALSE;
}

GT_STATUS mvHwsMmPcs40GBackWa
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum
)
{
    GT_U32 regAddr, data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 countVal;
    GT_U32 gearVal;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    regAddr = FEC_DEC_DMA_WR_DATA + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    if (data != 0x8000)
    {
        /* unit not initalize or under TRx training, delete */
        DEBUG_PCS(("\nRun mvHwsMmPcs40GBackWa PORT NOT CREATED on device %d, pcs %d", devNum, pcsNum));
        return GT_OK;
    }
    hwsOsTimerWkFuncPtr(2);

    regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    countVal = (data >> 8) & 3;
    countVal = (countVal == 0) ? 1 : countVal * 2;

    if (countVal != 4)
    {
        /* not 40G */
        DEBUG_PCS(("\nRun mvHwsMmPcs40GBackWa PORT NOT CREATED (4) on device %d, pcs %d", devNum, pcsNum));
        return GT_OK;
    }

    DEBUG_PCS(("\nRun mvHwsMmPcs40GBackWa on device %d, pcs %d", devNum, pcsNum));
    /* Check Gear Box Status */
    regAddr = unitAddr + pcsNum*unitIndex + GEARBOX_STATUS;
    genRegisterGet(devNum, portGroup, regAddr, &data, 0);
    gearVal = (data >> 1) & 0xF;
    if (gearVal != 0xF)
    {
        DEBUG_PCS(("\nCheck Gear Box Status on device %d, pcs %d (data 0x%x)", devNum, pcsNum, data));
        if (gearVal == 0)
        {
            /* no connector in port */
            return GT_OK;
        }
        else
        {
          return GT_FAIL;
        }
    }
    else
    {
        regAddr = PCS40G_COMMON_STATUS + unitAddr + pcsNum*unitIndex;
        CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
        if (!(data & 1))
        {
            DEBUG_PCS(("\nRun RESET on device %d, pcs %d", devNum, pcsNum));
            /* one SD sq_detect = 1 perform pcs reset sequence */
            mvHwsMMPcsV3Reset(devNum,portGroup,pcsNum,RESET);
            hwsOsTimerWkFuncPtr(1);
            mvHwsMMPcsV3Reset(devNum,portGroup,pcsNum,UNRESET);
        }
    }

    return GT_OK;
}

