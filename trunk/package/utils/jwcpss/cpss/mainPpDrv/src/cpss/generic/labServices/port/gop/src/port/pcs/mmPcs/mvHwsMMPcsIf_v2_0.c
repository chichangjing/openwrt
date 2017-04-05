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
* mvHwsMMPcsIf_v2_0.c
*
* DESCRIPTION:
*       MMPCS V2.0 interface API
*
* FILE REVISION NUMBER:
*       $Revision: 31 $
*
*******************************************************************************/

#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <pcs/mmPcs/mvHwsMMPcsIf.h>
#include <pcs/mmPcs/mvHwsMMPcsV2If.h>
#include <pcs/mmPcs/mvHwsMMPcsDb.h>


#define SYNC_CHANGED_INTERRUPT     0x6


static char* mvHwsPcsTypeGetFunc(void)
{
    return "MMPCS V2.0";
}

static  GT_U32 indArr[4] = {0, 0x400, 0x600, 0x800};
static GT_BOOL hwsInitDone = GT_FALSE;



/*******************************************************************************
* mvHwsMMPcsV2IfInit
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
GT_STATUS mvHwsMMPcsV2IfInit(MV_HWS_PCS_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsMMPscSeqInit());
    }

    funcPtrArray[MMPCS].pcsResetFunc = mvHwsMMPcsV2Reset;
    funcPtrArray[MMPCS].pcsModeCfgFunc = mvHwsMMPcsV2Mode;
    funcPtrArray[MMPCS].pcsRxResetFunc = mvHwsMMPcsV2RxReset;
    funcPtrArray[MMPCS].pcsExtPllCfgFunc = mvMmPcsV2ExtPllCfg;
    funcPtrArray[MMPCS].pcsSignalDetectMaskEn = mvHwsMMPcsV2SignalDetectMaskSet;
    funcPtrArray[MMPCS].pcsCheckGearBoxFunc = mvHwsMMPcsV2CheckGearBox;

    funcPtrArray[MMPCS].pcsLbCfgFunc = mvHwsMMPcsLoopBack;
    funcPtrArray[MMPCS].pcsTestGenFunc = mvHwsMMPcsTestGenCfg;
    funcPtrArray[MMPCS].pcsTestGenStatusFunc = mvHwsMMPcsTestGenStatus;
    funcPtrArray[MMPCS].pcsTypeGetFunc = mvHwsPcsTypeGetFunc;
    funcPtrArray[MMPCS].pcsFecCfgFunc = mvHwsMMPcsFecConfig;
    funcPtrArray[MMPCS].pcsFecCfgGetFunc = mvHwsMMPcsFecConfigGet;
    funcPtrArray[MMPCS].pcsActiveStatusGetFunc = mvHwsMMPcsActiveStatusGet;

    hwsInitDone = GT_TRUE;

    return GT_OK;
}

static GT_STATUS mvHwsMMPcsV2Unreset
(
  GT_U8  devNum,
  GT_U32 portGroup,
  GT_U32 pcsNum,
  GT_U32 countVal
)
{
  GT_U32 regAddr;
  GT_U32 i;
  GT_U32 unitAddr;
  GT_U32 unitIndex;

  mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

  for (i = 0; i < countVal; i++)
  {
    if (pcsNum < 16)
    {
      /* access to network PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
    }
    else
    {
      /* access to fabric PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum * unitIndex) + indArr[i];
    }
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 1, 1));
  }
  for (i = 0; i < countVal; i++)
  {
    if (pcsNum < 16)
    {
      /* access to network PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
    }
    else
    {
      /* access to fabric PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum * unitIndex) + indArr[i];
    }
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x2, 0x2));
  }
  for (i = 0; i < countVal; i++)
  {
    if (pcsNum < 16)
    {
      /* access to network PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
    }
    else
    {
      /* access to fabric PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum * unitIndex) + indArr[i];
    }
    /* only MMPCS 0 are relevant; other lanes keep reset set 0 */
    if (i == 0)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x4, 0x4));
    }
    else
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x0, 0x4));
    }
  }
  for (i = 0; i < countVal; i++)
  {
    if (pcsNum < 16)
    {
      /* access to network PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
    }
    else
    {
      /* access to fabric PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum * unitIndex) + indArr[i];
    }
    /* only MMPCS 0 are relevant; other lanes keep reset set 0 */
    if (i == 0)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x20, 0x20));
    }
    else
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x0, 0x20));
    }
  }
  for (i = 0; i < countVal; i++)
  {
    if (pcsNum < 16)
    {
      /* access to network PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
    }
    else
    {
      /* access to fabric PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum * unitIndex) + indArr[i];
    }
    /* only MMPCS 0 are relevant; other lanes keep reset set 0 */
    if (i == 0)
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x40, 0x40));
    }
    else
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x0, 0x40));
    }
  }
  for (i = 0; i < countVal; i++)
  {
    if (pcsNum < 16)
    {
      /* access to network PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
    }
    else
    {
      /* access to fabric PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum * unitIndex) + indArr[i];
    }
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0x10, 0x10));
  }
  for (i = 0; i < countVal; i++)
  {
    if (pcsNum < 16)
    {
      /* access to network PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
    }
    else
    {
      /* access to fabric PCS */
      regAddr = PCS_RESET_REG + unitAddr + (pcsNum * unitIndex) + indArr[i];
    }
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 8, 8));
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsV2Reset
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
GT_STATUS mvHwsMMPcsV2Reset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_RESET            action
)
{
    GT_U32 regAddr;
    GT_U32 data, i;
    GT_U32 countVal;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    countVal = (data >> 8) & 3;

	/* Check if the port is in AP mode. 
	   If true don't perform any PCS operation */
	if (countVal == 3)
	{
		return GT_BAD_STATE;
	}

    countVal = (countVal == 0) ? 1 : countVal * 2;

    if (action == RESET)
    {
        for (i = 0; i < countVal; i++)
        {
            if (pcsNum < 16)
            {
              /* access to network PCS */
              regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
            }
            else
            {
              /* access to fabric PCS */
              regAddr = PCS_RESET_REG + unitAddr + (pcsNum * unitIndex) + indArr[i];
            }
            CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, 0x7F));
        }
    }
    else
    {
        CHECK_STATUS(mvHwsMMPcsV2Unreset(devNum,portGroup,pcsNum,countVal));
    }

    return GT_OK;
}


/*******************************************************************************
* mvHwsMMPcsV2RxReset
*
* DESCRIPTION:
*       Set the selected RX PCS type and number to reset or exit from reset.
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
GT_STATUS mvHwsMMPcsV2RxReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_RESET            action
)
{
    GT_U32 regAddr;
    GT_U32 data, i;
    GT_U32 countVal;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    countVal = (data >> 8) & 3;

	/* Check if the port is in AP mode. 
	   If true don't perform any PCS operation */
	if (countVal == 3)
	{
		return GT_BAD_STATE;
	}

    countVal = (countVal == 0) ? 1 : countVal * 2;

    if (action == RESET)
    {
        /* reset PCS Rx only */
        /* on first lane reset bits 5,4,3 */
        regAddr = PCS_RESET_REG + unitAddr + pcsNum * unitIndex;
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, 0x38));
        for (i = 1; i < countVal; i++)
        {
            /* on other lanes reset bits 4,3 only */
            if (pcsNum < 16)
            {
              /* access to network PCS */
              regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
            }
            else
            {
              /* access to fabric PCS */
              regAddr = PCS_RESET_REG + unitAddr + (pcsNum * unitIndex) + indArr[i];
            }
            CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, 0x18));
        }
    }
    else
    {
        /* un reset PCS Rx only */
        regAddr = PCS_RESET_REG + unitAddr + pcsNum * unitIndex;
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, (1 << 5), (1 << 5)));
        for (i = 0; i < countVal; i++)
        {
            if (pcsNum < 16)
            {
              /* access to network PCS */
              regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
            }
            else
            {
              /* access to fabric PCS */
              regAddr = PCS_RESET_REG + unitAddr + (pcsNum * unitIndex) + indArr[i];
            }
            CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, (1 << 4), (1 << 4)));
        }
        for (i = 0; i < countVal; i++)
        {
            if (pcsNum < 16)
            {
              /* access to network PCS */
              regAddr = PCS_RESET_REG + unitAddr + (pcsNum+i) * unitIndex;
            }
            else
            {
              /* access to fabric PCS */
              regAddr = PCS_RESET_REG + unitAddr + (pcsNum * unitIndex) + indArr[i];
            }
            CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, (1 << 3), (1 << 3)));
        }
    }
    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsV2Mode
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
GT_STATUS mvHwsMMPcsV2Mode
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
    /* clear register used for WA disable */
    regAddr = FEC_DEC_DMA_WR_DATA + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, 0));

    /* by default don't use external PLL */
    regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 0, (1 << 14)));

    /* config FEC and lanes number; set MAC clock disable */
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

    for (i = 0; i < numOfLanes; i++)
    {
        if (pcsNum < 16)
        {
            regAddr = CHANNEL0_RX_GB_FIFO_CONFIGURATION + unitAddr + (pcsNum + i) * unitIndex;
        }
        else
        {
            /* access to fabric PCS */
            regAddr = CHANNEL0_RX_GB_FIFO_CONFIGURATION + unitAddr + (pcsNum * unitIndex) + indArr[i];
        }
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, 26, 0));
    }

    if (pcsNum < 16)
    {
        switch (numOfLanes)
        {
        case 1:
            seq = &hwsMmPscSeqDb[MMPCS_MODE_1_LANE_SEQ];
            break;
        case 2:
            seq = &hwsMmPscSeqDb[MMPCS_MODE_2_LANE_SEQ];
            break;
        case 4:
            seq = &hwsMmPscSeqDb[MMPCS_MODE_4_LANE_SEQ];
            break;
        default:
            return GT_NOT_SUPPORTED;
        }

        /* Set interrupt signal detect change interrupt to be SYNC;
           set then FEC DEC status bit 1 to be rx_decoder error */
        for (i = 0; i < numOfLanes; i++)
        {
            regAddr = FEC_DEC_DMA_CONTROL + unitAddr + (pcsNum + i) * unitIndex;
            CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, SYNC_CHANGED_INTERRUPT, SYNC_CHANGED_INTERRUPT));
        }

        if ((Lion2B0 == HWS_DEV_SILICON_TYPE(devNum)) || (HooperA0 == HWS_DEV_SILICON_TYPE(devNum)))
        {
             /* JIRA MSPEEDPCS-553 - mux of signal detect or sync_change interrupt
                    controlled by fdec_dma_read_s of MSM0 affecting all ports */
            if (pcsNum % 4 != 0)
            {
                regAddr = FEC_DEC_DMA_CONTROL + unitAddr + (pcsNum & 0xFFFFFFFC) * unitIndex;
                CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, SYNC_CHANGED_INTERRUPT, SYNC_CHANGED_INTERRUPT));
            }

            /* for Lion change interrupt for extended/regular PCS of ports 9 and 11 */
            switch(pcsNum)
            {
                case 9:
                    for (i = 12; i < 12 + numOfLanes; i++)
                    {
                        regAddr = FEC_DEC_DMA_CONTROL + unitAddr + i * unitIndex;
                        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, SYNC_CHANGED_INTERRUPT, SYNC_CHANGED_INTERRUPT));
                    }
                    break;

                case 12:
                    regAddr = FEC_DEC_DMA_CONTROL + unitAddr + 9 * unitIndex;
                    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, SYNC_CHANGED_INTERRUPT, SYNC_CHANGED_INTERRUPT));
                    break;

                case 11:
                    for (i = 14; i < 14 + numOfLanes; i++)
                    {
                        regAddr = FEC_DEC_DMA_CONTROL + unitAddr + i * unitIndex;
                        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, SYNC_CHANGED_INTERRUPT, SYNC_CHANGED_INTERRUPT));
                    }
                    break;

                case 14:
                    regAddr = FEC_DEC_DMA_CONTROL + unitAddr + 11 * unitIndex;
                    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, SYNC_CHANGED_INTERRUPT, SYNC_CHANGED_INTERRUPT));
                    break;

                default:
                    break;
            }
        }
    }
    else
    {
        switch (numOfLanes)
        {
        case 1:
            seq = &hwsMmPscSeqDb[MMPCS_FABRIC_MODE_1_LANE_SEQ];
            break;
        case 2:
            seq = &hwsMmPscSeqDb[MMPCS_FABRIC_MODE_2_LANE_SEQ];
            break;
        case 4:
            seq = &hwsMmPscSeqDb[MMPCS_FABRIC_MODE_4_LANE_SEQ];
            break;
        default:
            return GT_NOT_SUPPORTED;
        }
    }
    CHECK_STATUS(mvCfgSeqExec(devNum, portGroup, pcsNum, seq->cfgSeq, seq->cfgSeqSize));

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsV2CheckGearBox
*
* DESCRIPTION:
*       Check Gear Box Status on related lanes.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       numOfLanes - number of lanes agregated in PCS
*
* OUTPUTS:
*       laneLock  - true or false.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsMMPcsV2CheckGearBox
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    GT_U32                  numOfLanes,
    GT_BOOL                 *laneLock
)
{
    GT_U32 regAddr;
    GT_U32 data;
    GT_U32 mask;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    /* Check Gear Box Status */
    regAddr = unitAddr + pcsNum*unitIndex + GEARBOX_STATUS;
    /* gear box status can show lock in not used bits, so read only
       required bits */
    mask = ((1 << numOfLanes) - 1) << 1;
    genRegisterGet(devNum, portGroup, regAddr, &data, mask);

    *laneLock = (data == mask) ? GT_TRUE : GT_FALSE;

    return GT_OK;
}


#ifndef ASIC_SIMULATION
static GT_U32 delay10GConnectWa = 10;
#endif
static GT_U32 readSyncStatus = 1000;
static GT_U32 readSyncCount = 30;

static GT_STATUS mvHwsMmPcs10GConnectWa
(
 GT_U8                   devNum,
 GT_U32                  portGroup,
 GT_U32                  pcsNum
)
{
    GT_U32 regAddr, data;
    GT_U32 unitAddr;
    GT_U32 unitIndex, i;
    GT_BOOL errorDetect;
    GT_U32 syncDataPrev, syncDataCurr;
    GT_U32 runCount;
    GT_BOOL noSyncData;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);
    errorDetect = GT_FALSE;

    /* Check Sync Down/Up  bit[3]*/
    regAddr = unitAddr + pcsNum*unitIndex + FEC_DEC_STATUS_REG;

    runCount = 0;
    do
    {
#ifndef ASIC_SIMULATION
        hwsOsTimerWkFuncPtr(delay10GConnectWa);
#endif /*ASIC_SIMULATION*/
        noSyncData = GT_FALSE;
        genRegisterGet(devNum, portGroup, regAddr, &syncDataPrev, 0);
        syncDataPrev &= (1 << 3);
        for (i = 1; i < readSyncStatus; i++)
        {
            genRegisterGet(devNum, portGroup, regAddr, &syncDataCurr, 0);
            syncDataCurr &= (1 << 3);

            /* The Sync-Detection indication is stable only if there is a pattern of 111... */
            if (syncDataCurr == 0)
            {
                noSyncData = GT_TRUE;
                break;
            }
            syncDataPrev = syncDataCurr;
        }
        runCount++;
    }while ((runCount < readSyncCount) && (noSyncData));

#ifdef ASIC_SIMULATION
    noSyncData = GT_FALSE;
#endif

    /* Check if there Serdes is Synced according to the Data pattern (only when the pattern is 111...) */
    if (!noSyncData)
    {
        /* sync UP, checks if an RX Decoder Error is detected */
        for (i = 0; (i < 20); i++)
        {
            errorDetect = GT_FALSE;
            regAddr = unitAddr + pcsNum*unitIndex + FEC_DEC_STATUS_REG;
            genRegisterGet(devNum, portGroup, regAddr, &data, 0);
            if (((data >> 1) & 1) == 1)
            {
                errorDetect = GT_TRUE;
            }
        }

        if (!errorDetect)
        {
            /* disable force link down on port */
            CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, pcsNum, MSM_PORT_MAC_CONTROL_REGISTER0, 0, (1 << 2)));
        }
        else /* in case of Link up state: if the Sync-Detection interrupt raised and there are errors on the line --> Link down */
        {
            /* enable force link down on port */
            CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, pcsNum, MSM_PORT_MAC_CONTROL_REGISTER0, (1 << 2), (1 << 2)));
        }
    }
    else
    {
        /* enable force link down on port */
        CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, XLGMAC_UNIT, pcsNum, MSM_PORT_MAC_CONTROL_REGISTER0, (1 << 2), (1 << 2)));
    }

    return GT_OK;
}


/*
Upon interrupt of the above call to HW services function that implements the bellow WA:
1.  Check GB lock indication  status  (/Cider/EBU/Lion2/Lion2_B0 {Current}/Lion2_B0 Units/GOP/<MMPCS> MMPCS MMPCS MPCS_IP %p Units/Gear Box Status):
a.  If at least one GB lock indication is in-active do nothing.
b.  Else check all GB lock is active and align done is  in-active set PCS rx reset and then un-reset (may try several times)
c.  If align done is active do nothing.
CPSS/VIPS On port delete return mask to this interrupt.

*/
GT_STATUS mvHwsMmPcs40GConnectWa
(
 GT_U8                   devNum,
 GT_U32                  portGroup,
 GT_U32                  pcsNum
)
{
    GT_U32 regAddr, data, gearVal;
    GT_U32 unitAddr;
    GT_U32 unitIndex;
    GT_U32 countVal;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    regAddr = FEC_DEC_DMA_WR_DATA + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    if (data != 0x8000)
    {
        /* unit not initalize or under TRx training, delete */
        return GT_OK;
    }

    regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum * unitIndex;
    CHECK_STATUS(genRegisterGet(devNum, portGroup, regAddr, &data, 0));
    countVal = (data >> 8) & 3;
    countVal = (countVal == 0) ? 1 : countVal * 2;

    if (countVal == 1)
    {
        /* 10G WA */
        return mvHwsMmPcs10GConnectWa(devNum,portGroup,pcsNum);
    }

    /* 40G WA */
    hwsOsTimerWkFuncPtr(10);
    /* Check Gear Box Status */
    regAddr = unitAddr + pcsNum*unitIndex + GEARBOX_STATUS;
    genRegisterGet(devNum, portGroup, regAddr, &data, 0);
    gearVal = (data >> 1) & 0xF;
    /*osPrintf("\nPCS reset WA on port %d(device %d). Gear Box is 0x%x.", pcsNum, devNum, gearVal);*/
    if (gearVal != 0xF)
    {
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
            /* run RX Reset */
            mvHwsMMPcsV2RxReset(devNum,portGroup,pcsNum,RESET);
            mvHwsMMPcsV2RxReset(devNum,portGroup,pcsNum,UNRESET);
        }
    }

    return GT_OK;
}

GT_STATUS mvMmPcsV2ExtPllCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum
)
{
    GT_U32 regAddr;
    GT_U32 unitAddr;
    GT_U32 unitIndex;

    mvUnitInfoGet(devNum, MMPCS_UNIT, &unitAddr, &unitIndex);

    regAddr = PCS40G_COMMON_CONTROL + unitAddr + pcsNum*unitIndex;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, (1 << 14), (1 << 14)));

    return GT_OK;
}

/*******************************************************************************
* mvHwsMMPcsV2SignalDetectMaskSet
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
GT_STATUS mvHwsMMPcsV2SignalDetectMaskSet
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

	/* Check if the port is in AP mode. 
	   If true don't perform any PCS operation */
	if (countVal == 3)
	{
		return GT_BAD_STATE;
	}

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
        data = (maskEn == GT_TRUE) ? (1 << 15) : 0;
        CHECK_STATUS(genRegisterSet(devNum, portGroup, regAddr, data, (1 << 15)));
    }

    return GT_OK;
}

/*******************************************************************************
* hwsMMPcsRev2IfClose
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
void hwsMMPcsRev2IfClose(void)
{
  hwsMMPcsSeqFree();
  hwsInitDone = GT_FALSE;
}

