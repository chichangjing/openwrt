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
* mvComPhyHIf.c
*
* DESCRIPTION:
*         Com PHY H (serdes) EOM implementation
*
* FILE REVISION NUMBER:
*       $Revision: 18 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <serdes/mvHwsSerdesIf.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <serdes/comPhyH/mvComPhyHDb.h>
#include <serdes/comPhyH/mvComPhyHIf.h>
#include <serdes/comPhyH/mvComPhyEomIf.h>

/*#define HWS_EOM_DEBUG*/

#ifdef  HWS_EOM_DEBUG
#define EOM_DBG(s) 	osPrintf s
#else
#define EOM_DBG(s)
#endif /* HWS_EOM_DEBUG */

GT_BOOL	eomDebug = GT_FALSE;
GT_BOOL	eomPrintEye = GT_TRUE;

#define GET_PHASE_RANGE(ui) ((ui < 160) ? 64 : 128)

/************************************************************************/

static GT_STATUS mvEomSetVerticalResolution(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum)
{
  GT_U32 baseAddr;
  GT_U32 data;
  GT_U32 unitAddr;
  GT_U32 unitIndex;
	
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;

  /* check analog reset */
  CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + COM_H_Adapted_DFE_Coefficient0_Reg, &data, 0));
  if(((data >> 10) & 0x3F) > 40)
  {
    /* Decision Feedback Equalization (DFE) Resolution */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_DFE_Resolution_Control_Reg, (1 << 8), (3 << 8)));
  }

  return GT_OK;
}

static GT_STATUS mvEomInit(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum)
{
	GT_U32 baseAddr;
	GT_U32 data = 0;
	GT_U32 unitAddr;
	GT_U32 unitIndex;
	
	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
	baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;
	/* EOM disable */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_EOM_Circuit_Control_Reg, (0 << 15), (1 << 15)));
	
	/* check EOM ready */
	do
	{
		CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + COM_H_EOM_Circuit_Control_Reg, &data, 0));
	} while((data >> 13) & 1);
	
	/*****************************/
	
	/* EOM enable */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_EOM_Circuit_Control_Reg, (1 << 15), (1 << 15)));
	
	/* check EOM ready */
	do
	{
		CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + COM_H_EOM_Circuit_Control_Reg, &data, 0));
	} while(!((data >> 13) & 1));
	
	/*****************************/
	
	/* clear DFE_FX_FORCE[0] */
	if ((data >> 13) & 1)
	{
		/* Use external tap override register values */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_EOM_DFE_Update_Control_Reg, 0, 0x1));
		return GT_OK;
	}
	
	return GT_FAIL;
}

static GT_STATUS mvEomSetValid(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesBaseAddr)
{
	GT_U32 data;

	/* clear EOM stop,sample,error fields */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Control_Reg0, (0 << 13), (1 << 13)));
	CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Control_Reg0, (1 << 13), (1 << 13)));

	/* check EOM ready */
	do
	{
        /* check EOM ready */
    	CHECK_STATUS(genRegisterGet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Control_Reg1, &data, 0));
	} while((data & 0x8000) == 0);

	if (data & 0x8000)
	{
		/* set EOM valid */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Control_Reg0, 0, (1 << 13)));
		return GT_OK;
	}

	return GT_FAIL;
}

/*****************************************************************************/
static GT_STATUS mvEomSetPhase(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesBaseAddr, GT_U32 targetPhase)
{
  static GT_32  currentPhase = 0;

  /* move to target phase value with step = 4 */
  if (((currentPhase - 4) > (GT_32)targetPhase ) || ((GT_U32)(currentPhase + 4) < targetPhase))
  {
    while((currentPhase - 4) > (GT_32)targetPhase)
    {
      currentPhase -= 4;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Control_Reg1, currentPhase, 0x7F));
	  CHECK_STATUS(mvEomSetValid(devNum, portGroup, serdesBaseAddr));
    }

    while((GT_U32)(currentPhase + 4) < targetPhase)
    {
      currentPhase += 4;
      CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Control_Reg1, currentPhase, 0x7F));
      CHECK_STATUS(mvEomSetValid(devNum, portGroup, serdesBaseAddr));
    }
  }

  CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Control_Reg1, targetPhase, 0x7F));
  CHECK_STATUS(mvEomSetValid(devNum, portGroup, serdesBaseAddr));

  currentPhase = targetPhase;

  return GT_OK;
}

/*****************************************************************************/
static GT_STATUS mvEomSetPopulationCount(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesBaseAddr, GT_U32 population)
{
	GT_STATUS rc;
	GT_U32 data;

	/* get unit base address and unit index for current device */

	/* set EOM Population counter [15:0] */
	data = (population) & 0xFFFF;
	rc = genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Population_Counter3, data, 0);

	/* set EOM Population counter [31:16] */
	data = ((population) >> 16) & 0xFFFF ;
	rc |= genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Population_Counter2, data, 0);
	/* clear other counter bits [32:63] */
	rc |= genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Population_Counter1, 0, 0);
	rc |= genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Population_Counter0, 0, 0);

	return rc;
}

/*****************************************************************************/
static GT_STATUS mvEomGetSampleCount
(
	GT_U8  devNum,
	GT_U32 portGroup,
	GT_U32 serdesBaseAddr,
	GT_U32 *sampleCountHigh,
	GT_U32 *sampleCountLow
)
{
	GT_U32 samp_31_16,samp_15_0;

	/* get unit base address and unit index for current device */
	CHECK_STATUS(genRegisterGet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Sample_Counter2, &samp_31_16, 0));
	CHECK_STATUS(genRegisterGet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Sample_Counter3, &samp_15_0, 0));

	*sampleCountLow =  (((samp_31_16 & 0xFFFF) << 16) + (samp_15_0 & 0xFFFF));
	*sampleCountHigh = 0;

	return GT_OK;
}

/*****************************************************************************/
static GT_STATUS mvEomGetErrorCount
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  serdesBaseAddr,
	GT_U32  *errorCountHigh,
	GT_U32  *errorCountLow
)
{
	GT_U32 err_15_0, err_31_16;

	/* get unit base address and unit index for current device */
	CHECK_STATUS(genRegisterGet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Error_Counter2, &err_31_16, 0));
	CHECK_STATUS(genRegisterGet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Error_Counter3, &err_15_0, 0));

	*errorCountLow =  (((err_31_16 & 0xFFFF) << 16) + (err_15_0 & 0xFFFF));
	*errorCountHigh = 0;

	return GT_OK;
}

/*****************************************************************************/
static GT_STATUS mvEomGetBer
(
	GT_U8  devNum,
	GT_U32 portGroup,
	GT_U32 serdesBaseAddr,
	GT_U32 sign1,
	GT_U32 sign2,
	GT_U32 voltage,
	GT_U32 phase,
	GT_U32 population,
	GT_U32 *ber
)
{
	GT_U32 errorHigh[2]={0};
	GT_U32 errorLow[2]={0};
	GT_U32 samplesHigh[2]={0};
	GT_U32 samplesLow[2]={0};
	GT_U32 sign[2];
	GT_U32 i;
	GT_U32 totalError=0;
	GT_U32 totalSamples=0;

	/* set population */
	CHECK_STATUS(mvEomSetPopulationCount(devNum, portGroup, serdesBaseAddr, population));

	/* set EOM voltage */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Control_Reg0, voltage, 0x3F));

	/* set phase */
	CHECK_STATUS(mvEomSetPhase(devNum, portGroup, serdesBaseAddr, phase));

	sign[0] = sign1;
	sign[1] = sign2;

	for (i = 0; i < 2; i++)
	{
		/* Set EOM_SET1_SIGN_BITS  = Sign1/2 Set EOM_MEASURE_STATE = Sign1/2 */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Set1_Control_Reg0, (sign[i] << 6), (3 << 6)));
		CHECK_STATUS(genRegisterSet(devNum, portGroup, serdesBaseAddr + COM_H_EOM_Circuit_Control_Reg, (sign[i] << 10), (3 << 10)));
		CHECK_STATUS(mvEomSetValid(devNum, portGroup, serdesBaseAddr));

		/* accumulate errors */
		CHECK_STATUS(mvEomGetErrorCount(devNum, portGroup, serdesBaseAddr, &errorHigh[i], &errorLow[i]));

		/* accumulate Samples */
		CHECK_STATUS(mvEomGetSampleCount(devNum, portGroup, serdesBaseAddr, &samplesHigh[i], &samplesLow[i]));

		totalError += errorLow[i];
		totalSamples += samplesLow[i];
	}

	/* calculate ber */
	*ber = (GT_U32)ABS(((GT_32)totalError)*((GT_32)totalSamples));

	if (eomDebug)
	{
		EOM_DBG(("\t totalError     0x%x\n", totalError));
		EOM_DBG(("\t totalSamples   0x%x\n", totalSamples));
		EOM_DBG(("\t BER            0x%x\n", *ber));
	}

	return GT_OK;
}

GT_STATUS mvHwsEomFindEyeCenter
(
	GT_U8  devNum,
	GT_U32 portGroup,
	GT_U32 serdesNum,
	GT_U32 voltage,
	GT_U32 population,
	GT_U32 targetBER,
	GT_U32 *leftEdge,
	GT_U32 *rightEdge
)
{
	GT_U32 baseAddr;
	GT_U32 unitAddr;
	GT_U32 unitIndex;
	GT_U32 phase = 0, phase_skip;
	GT_U32 eyeleftEdge = 0, eyerightEdge = 0;
	GT_U32 maxBER;
	GT_U32 ber_top=0, ber_bottom=0;
	MV_STATE_MODE state;

	phase = 0;
	phase_skip = 4;
	maxBER = 0xFFFFFFFF;
	state = SearchLeft;

	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;

	/* set EOM voltage */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_EOM_Set1_Control_Reg0, voltage, 0x3F));

	/*for(phase = (2*EOM_WINDOW);(state != Done) && (phase < (EOM_SERCH_RANGE-(2*EOM_WINDOW)));phase=phase+phase_skip)*/
	for(phase = EOM_WINDOW;(state != Done) && (phase < (EOM_SERCH_RANGE-(EOM_WINDOW)));phase=phase+phase_skip)
	{
		CHECK_STATUS(mvEomGetBer(devNum, portGroup, baseAddr, 1, 3, voltage, phase, population, &ber_top));
		CHECK_STATUS(mvEomGetBer(devNum, portGroup, baseAddr, 0, 2, voltage, phase, population, &ber_bottom));

		maxBER = (ber_top > ber_bottom) ? ber_top : ber_bottom;
		if (eomDebug)
		{
			EOM_DBG(("\t maxBER 0x%x\n", maxBER));
			EOM_DBG(("\t phase  0x%x\n", phase));
		}
		if (state == SearchLeft)
		{
			if (maxBER <= targetBER)
			{
				eyeleftEdge = phase - phase_skip;
				state = SearchRight;
			}
		}
		else
		{
			if ((maxBER > targetBER) && ((phase - eyeleftEdge) > 8))
			{
				eyerightEdge = phase - phase_skip;
				state = Done;
			}
			else
				if((maxBER > targetBER) && ((phase - eyeleftEdge) <= 8))
					state = SearchLeft;
		}
	}

	if (eomDebug)
	{
		EOM_DBG(("\t eyeleftEdge   %d\n", eyeleftEdge));
		EOM_DBG(("\t eyerightEdge  %d\n", eyerightEdge));
	}

	*leftEdge = eyeleftEdge;
	*rightEdge = eyerightEdge;

	return GT_OK;
}

/*****************************************************************************/
GT_STATUS mvHwsEomEyeMeasure
(
	GT_U8  devNum,
	GT_U32 portGroup,
	GT_U32 serdesNum,
	GT_U32 voltage,
	GT_U32 population,
	GT_U32 targetBER,
	GT_U32 LeftEdge,
	GT_U32 RightEdge,
	MV_EOM_EYE_DATA  *eomEyeData
)
{
	GT_U32 baseAddr;
	GT_U32 unitAddr;
	GT_U32 unitIndex;
	GT_U32 phase = 0 , phase_skip;
	GT_U32 maxBER;
	GT_U32 ber_top=0, ber_bottom=0;
	GT_U32 eom_window_margin;
	GT_U32 eomEyeWidth=0,eomLeftWidth=0,eomRightWidth=0;
	GT_U32 eyeCenter=0;

	phase = 0;
	phase_skip = 1;
	eom_window_margin = 8;
	maxBER = 0xFFFFFFFF;

	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
	baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;

	/* set EOM voltage */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_EOM_Set1_Control_Reg0, voltage, 0x3F));

	eyeCenter = (LeftEdge + RightEdge)/2;

	if (eomDebug)
	{
		EOM_DBG(("\t LeftEdge   %d\n", LeftEdge));
		EOM_DBG(("\t RightEdge  %d\n", RightEdge));
		EOM_DBG(("\t eyeCenter  %d\n", eyeCenter));
	}

	for(phase = (LeftEdge - eom_window_margin);phase < (LeftEdge + EOM_WINDOW + eom_window_margin);phase=phase+phase_skip)
	{
		CHECK_STATUS(mvEomGetBer(devNum, portGroup, baseAddr, 1, 3, voltage, phase, population, &ber_top));
		CHECK_STATUS(mvEomGetBer(devNum, portGroup, baseAddr, 0, 2, voltage, phase, population, &ber_bottom));
		maxBER = (ber_top >= ber_bottom) ? ber_top : ber_bottom;

		if (maxBER <= targetBER)
		{
						eomEyeWidth = eomEyeWidth + phase_skip;
		}
	}

	/* check left eye margin */
	ber_top=0;
	ber_bottom=0;
	for(phase = eyeCenter;phase > (eyeCenter - EOM_WINDOW);phase--)
	{
		CHECK_STATUS(mvEomGetBer(devNum, portGroup, baseAddr, 1, 3, voltage, phase, population, &ber_top));
		CHECK_STATUS(mvEomGetBer(devNum, portGroup, baseAddr, 0, 2, voltage, phase, population, &ber_bottom));
		maxBER = (ber_top >= ber_bottom) ? ber_top : ber_bottom;

		if (maxBER > targetBER)
		{
			eomLeftWidth = eyeCenter - phase;
			break;
		}
	}

	/* check right eye margin */
	ber_top=0;
	ber_bottom=0;
	for(phase = eyeCenter;phase < (eyeCenter + EOM_WINDOW);phase++)
	{
		CHECK_STATUS(mvEomGetBer(devNum, portGroup, baseAddr, 1, 3, voltage, phase, population, &ber_top));
		CHECK_STATUS(mvEomGetBer(devNum, portGroup, baseAddr, 0, 2, voltage, phase, population, &ber_bottom));
		maxBER = (ber_top >= ber_bottom) ? ber_top : ber_bottom;

		if (maxBER > targetBER)
		{
			eomRightWidth = phase - eyeCenter;
			break;
		}
	}

	if (eomDebug)
	{
		EOM_DBG(("\t eomEyeWidth    %d\n", eomEyeWidth));
		EOM_DBG(("\t eomLeftWidth   %d\n", eomLeftWidth));
		EOM_DBG(("\t eomRightWidth  %d\n", eomRightWidth));
	}

	eomEyeData->eyeWidth      = eomEyeWidth;
	eomEyeData->leftEyeWidth  = eomLeftWidth;
	eomEyeData->rightEyeWidth = eomRightWidth;

	return GT_OK;
}

/*******************************************************************************
* mvEomMapEye
*
* DESCRIPTION:
*       Calculate and return upper and lower Eye matrix
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       current DFE resolution.
*       current DFE V
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvEomMapEye
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_U32  population,
	GT_U32  targetBER,
    GT_U32  leftEdge,
    GT_U32  rightEdge,
    GT_U32  *upperEyeRes,
    GT_U32  *lowerEyeRes
)
{
  GT_U32 baseAddr;
  GT_U32 phase;
  GT_32  voltage;
  GT_U32 done;
  GT_U32 unitAddr;
  GT_U32 unitIndex;
	
  /* get unit base address and unit index for current device */
  mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
  baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;

  /* Begin Upper Half of Eye */
  done = 0;
  for (voltage = 0; (voltage < EOM_VOLT_RANGE) && (!done); voltage++)
  {
    done = 1;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_EOM_Set1_Control_Reg0, (GT_U32)voltage, 0x3F));
    for (phase = leftEdge; phase <= rightEdge; phase++)
    {
      /* set EOM_SET1_PHASE */
      CHECK_STATUS(mvEomSetPhase(devNum, portGroup, baseAddr, phase));
      CHECK_STATUS(mvEomGetBer(devNum, portGroup, baseAddr, 1, 3, voltage, phase, population, &(upperEyeRes[(voltage*EOM_PHASE_RANGE)+(phase - leftEdge)])));
      if (upperEyeRes[(voltage*EOM_PHASE_RANGE)+(phase - leftEdge)] < targetBER)
      {
        done = 0;
      }
    }
  }
  
  /* Begin Lower Half of Eye */
  done = 0;
  for (voltage = 0; (voltage < EOM_VOLT_RANGE) && (!done); voltage++)
  {
     done = 1;
    /* set EOM_SET1_VOLTAGE  */
    CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_EOM_Set1_Control_Reg0, voltage, 0x3F));
    for (phase = leftEdge; phase <= rightEdge; phase++)
    {
      /* set EOM_SET1_PHASE */
      CHECK_STATUS(mvEomSetPhase(devNum, portGroup, baseAddr, phase));
      CHECK_STATUS(mvEomGetBer(devNum, portGroup, baseAddr, 0, 2, voltage, phase, population, &(lowerEyeRes[(voltage*EOM_PHASE_RANGE)+(phase - leftEdge)])));
      if (lowerEyeRes[(voltage*EOM_PHASE_RANGE)+(phase - leftEdge)] < targetBER)
      {
        done = 0;
      }
    }
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsEomGetUi
*
* DESCRIPTION:
*       Returns the current system baud rate.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       current system baud rate.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsEomGetUi
(
    GT_U8      devNum,
    GT_U32     portGroup,
    GT_U32     serdesNum,
    GT_U32*    curUi
)
{
    GT_U32 baseAddr;
    GT_U32 gen;
    GT_U32 unitAddr;
    GT_U32 unitIndex;
	
    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex * serdesNum;

    if (curUi == NULL)
    {
        return GT_BAD_VALUE;
    }

    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+SERDES_EXTERNAL_CONFIGURATION_0, &gen, 0));

    switch ((gen >> 7) & 0xf)
    {
      case 0x3:
        *curUi = 200;
        break;
      case 0x6:
        *curUi = 800;
        break;
      case 0x8:
        *curUi = 320;
        break;
      case 0x9:
        *curUi = 266;
        break;
      case 0xA:
        *curUi = 235;
        break;
      case 0xB:
        *curUi = 160;
        break;
      case 0xD:
        *curUi = 133;
        break;
      case 0xE:
        *curUi = 97;
        break;
      case 0xF:
        *curUi = 80;
        break;
      default:
        return GT_BAD_VALUE;
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsEomGetDfeRes
*
* DESCRIPTION:
*       Returns the current DFE parameters.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       current DFE resolution.
*       current DFE V
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsEomGetDfeRes
(
    GT_U8      devNum,
    GT_U32     portGroup,
    GT_U32     serdesNum,
    GT_U32     *dfeRes
)
{
    GT_U32 baseAddr;
    GT_U32 data;
    GT_U32 unitAddr;
    GT_U32 unitIndex;
	
    /* get unit base address and unit index for current device */
    mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
    baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;

    if (dfeRes == NULL)
    {
        return GT_BAD_VALUE;
    }
	
    CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr+COM_H_DFE_Resolution_Control_Reg, &data, 0));
    data = (data >> 8) & 3; /*[9:8]*/
    switch(data)
    {
        case 0:
            *dfeRes = 3;
            break;
        case 1:
            *dfeRes = 4;
            break;
        case 2:
            *dfeRes = 5;
            break;
        case 3:
            *dfeRes = 6;
            break;
        default:
            return GT_BAD_VALUE;
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsEomInit
*
* DESCRIPTION:
*       Init EOM serdes mechanism.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsEomInit
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum
)
{
	CHECK_STATUS(mvEomSetVerticalResolution(devNum, portGroup, serdesNum));
	CHECK_STATUS(mvEomInit(devNum, portGroup, serdesNum));

    return GT_OK;
}

/*******************************************************************************
* mvHwsEomClose
*
* DESCRIPTION:
*       Disable EOM serdes mechanism.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsEomClose(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum)
{
	GT_U32 baseAddr;
	GT_U32 unitAddr;
	GT_U32 unitIndex;
	GT_U32 data ;

	/* get unit base address and unit index for current device */
	mvUnitInfoGet(devNum, SERDES_UNIT, &unitAddr, &unitIndex);
	baseAddr = unitAddr + unitIndex * serdesNum + SERDES_PHY_REGS;

	do
	{
		/* EOM disable */
		CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_EOM_Circuit_Control_Reg, 0, (1 << 15)));
		CHECK_STATUS(genRegisterGet(devNum, portGroup, baseAddr + COM_H_EOM_Circuit_Control_Reg, &data, 0));
		if (eomDebug)
		{
			EOM_DBG(("EOM closed - COM_H_EOM_Circuit_Control_Reg=0x%x\n", data));
		}
		data = data & 0x2000;
	} while(data != 0);

	if (eomDebug)
	{
		EOM_DBG(("EOM closed END!!! - COM_H_EOM_Circuit_Control_Reg=0x%x\n", data));
	}


	/* Use DFE auto-trained taps */
	CHECK_STATUS(genRegisterSet(devNum, portGroup, baseAddr + COM_H_EOM_DFE_Update_Control_Reg, 0, 0x3F));

	return GT_OK;
}

/*******************************************************************************
* mvHwsEomGetMatrix
*
* DESCRIPTION:
*       Returns the eye mapping matrix.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       timeout   - wait time in msec
*
* OUTPUTS:
*       horizontal/vertical Rx eye matrix
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsEomGetMatrix
(
    GT_U8     devNum,
    GT_U32    portGroup,
    GT_U32    serdesNum,
    GT_U32    timeout,
    GT_U32    *rowSize,
    GT_U32    *upMatrix,
    GT_U32    *loMatrix
)
{
    GT_U32 voltage,population,targetBER;
    GT_U32 ui;
    GT_U32 left, right;

    CHECK_STATUS(mvHwsEomGetUi(devNum,portGroup,serdesNum,&ui));

    population = ((timeout * 1000) / ui) * 1000; /* timeout in micro sec and ui in pico sec */
	targetBER =0;
	voltage = 0;
    /* find left and right edges */
    CHECK_STATUS(mvHwsEomFindEyeCenter(devNum, portGroup, serdesNum, voltage, population, targetBER, &left, &right));

	targetBER = 1000;
    CHECK_STATUS(mvEomMapEye(devNum, portGroup, serdesNum, population, targetBER, left, right, upMatrix, loMatrix));

    if (rowSize != NULL)
    {
        *rowSize = right - left;
    }
    return GT_OK;
}

#ifdef HWS_EOM_DEBUG
static void mvEomPrintMap
(
    GT_U32  leftEdge,
    GT_U32  rightEdge,
    GT_U32  *upperEyeRes,
    GT_U32  *lowerEyeRes
)
{
    GT_U32 i, j;
  /* print Upper Eye */
  osPrintf("\n\nEOM Eye:: eomLeftEdge = %d, eomRightEdge = %d\n", leftEdge, rightEdge);
  for (i = EOM_VOLT_RANGE-1; i > 0; i--)
  {
    osPrintf("\n%d\t#########", i);
    for (j = 0; j <= (rightEdge - leftEdge); j++)
    {
      if (upperEyeRes[(i*EOM_PHASE_RANGE)+j] == 0)
      {
        osPrintf(" ");
      }
      else if (upperEyeRes[(i*EOM_PHASE_RANGE)+j] < 10000)
      {
        osPrintf("+");
      }
      else if (upperEyeRes[(i*EOM_PHASE_RANGE)+j] < 1000000)
      {
        osPrintf("*");
      }
            else
            {
                osPrintf("#");
            }
    }
    osPrintf("##########");
  }
  
  for (i = 0; i < EOM_VOLT_RANGE; i++)
  {
    osPrintf("\n%d\t##########", i);
    for (j = 0; j <= (rightEdge - leftEdge) ; j++)
    {
      if (lowerEyeRes[(i*EOM_PHASE_RANGE)+j] == 0)
      {
        osPrintf(" ");
      }
      else if (lowerEyeRes[(i*EOM_PHASE_RANGE)+j] < 10000)
      {
        osPrintf("+");
      }
      else if (lowerEyeRes[(i*EOM_PHASE_RANGE)+j] < 1000000)
      {
        osPrintf("*");
      }
            else
            {
                osPrintf("#");
            }
    }
    osPrintf("##########");
  }
    osPrintf("\n");
}
#else
static void mvEomPrintMap
(
	GT_U32  leftEdge,
	GT_U32  rightEdge,
    GT_U32  *upperEyeRes,
    GT_U32  *lowerEyeRes
)
{
	/* avoid warnings */
	leftEdge = leftEdge;
	rightEdge = rightEdge;
	*upperEyeRes = *upperEyeRes;
	*lowerEyeRes = *lowerEyeRes;
}
#endif

/*******************************************************************************
* mvHwsComHSerdesEOMGet
*
* DESCRIPTION:
*       Returns the horizontal/vertical Rx eye margin.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       timeout   - wait time in msec
*
* OUTPUTS:
*       horizontal/vertical Rx eye margin and EOM status.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesEOMGet
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    GT_U32                    timeout,
    MV_HWS_SERDES_EOM_RES     *results
)
{
	GT_U32 eomLeftEdge=0;
    GT_U32 eomRightEdge=0;

    GT_U32 *upperEyeRes;
    GT_U32 *lowerEyeRes;

    GT_U32 matrixSize;

	GT_U32 population,targetBER;
	GT_U32 dfeRes, phaseRange, i, j;
	GT_U32 ui, voltage, maxVolt, minVolt, done;
    float  tmp;

	if(NULL == results)
      return GT_BAD_PTR;

	EOM_DBG(("mvEomSetVerticalResolution\n"));
	CHECK_STATUS(mvEomSetVerticalResolution(devNum, portGroup, serdesNum));
	
	EOM_DBG(("mvEomInit\n"));
	CHECK_STATUS(mvEomInit(devNum, portGroup, serdesNum));

	EOM_DBG(("mvHwsEomGetUi\n"));
    CHECK_STATUS(mvHwsEomGetUi(devNum,portGroup,serdesNum,&ui));
    phaseRange = GET_PHASE_RANGE(ui);
	
	EOM_DBG(("mvHwsEomFindEyeCenter\n"));
    population = ((timeout * 1000) / ui) * 1000; /* timeout in micro sec and ui in pico sec */
    targetBER = 0;
	voltage = 0;
    /* find left and right edges */
    CHECK_STATUS(mvHwsEomFindEyeCenter(devNum, portGroup, serdesNum, voltage, population, targetBER, &eomLeftEdge, &eomRightEdge));
	
    if (eomRightEdge == eomLeftEdge)
    {
        EOM_DBG(("\nCannot run EOM with eomLeftEdge = %d, eomRightEdge = %d\n", eomLeftEdge, eomRightEdge));
        return GT_FAIL;
    }

    matrixSize = EOM_PHASE_RANGE * EOM_VOLT_RANGE * sizeof(GT_U32);
    upperEyeRes = (GT_U32 *)hwsOsMallocFuncPtr(matrixSize);
    lowerEyeRes = (GT_U32 *)hwsOsMallocFuncPtr(matrixSize);

	hwsOsMemSetFuncPtr(upperEyeRes, 0xff, matrixSize);
	hwsOsMemSetFuncPtr(lowerEyeRes, 0xff, matrixSize);

	population = ((timeout * 1000) / ui) * 1000; /* timeout in micro sec and ui in pico sec */
	targetBER = 1000;

	EOM_DBG(("mvEomMapEye\n"));
	CHECK_STATUS(mvEomMapEye(devNum, portGroup, serdesNum, population, targetBER, eomLeftEdge, eomRightEdge, upperEyeRes, lowerEyeRes));
	
	EOM_DBG(("mvHwsEomGetDfeRes\n"));
	CHECK_STATUS(mvHwsEomGetDfeRes(devNum, portGroup, serdesNum, &dfeRes));

	EOM_DBG(("Calculate Vertical Eye\n"));
	/* calculate Vertical Eye = (MaxVoltage - MinVoltage)/DFE_RES */
	maxVolt = 0;
    done = 0;
	for (i = EOM_VOLT_RANGE-1; (i > 0) && (!done); i--)
	{
		for (j = 0; j <= (eomRightEdge - eomLeftEdge) ; j++)
		{
			if (upperEyeRes[(i*EOM_PHASE_RANGE)+j] == 0)
			{
				maxVolt = i;
				done = 1;
				break;
			}
		}
	}
	
	minVolt = 0;
    done = 0;
	for (i = EOM_VOLT_RANGE-1; (i > 0) && (!done); i--)
	{
		for (j = 0; j <= (eomRightEdge - eomLeftEdge); j++)
		{
			if (lowerEyeRes[(i*EOM_PHASE_RANGE)+j] == 0)
			{
				minVolt = i;
				done = 1;
				break;
			}
		}
	}
	
	/* calculate Horizontal Eye = (RigthEdge - eomLeftEdge)*UI/phaseRange, where voltage = 0 */
	if (results != NULL)
	{
		results->horizontalEye = ((eomRightEdge-eomLeftEdge)*ui)/phaseRange;
		results->verticalEye = (maxVolt + minVolt)*dfeRes;
	}
	tmp = (float)(eomRightEdge - eomLeftEdge) * ui;
	tmp = tmp/phaseRange;
	
	EOM_DBG(("mvHwsEomClose\n"));
	CHECK_STATUS(mvHwsEomClose(devNum, portGroup, serdesNum));
	
    EOM_DBG(("\nHorizontal Eye is %f ps.", tmp));
	EOM_DBG(("\nVertical Eye is %d mV.\n", (maxVolt + minVolt)*dfeRes));
	
	if (eomPrintEye)
	{
		mvEomPrintMap(eomLeftEdge, eomRightEdge, upperEyeRes, lowerEyeRes);
	}
	
    hwsOsFreeFuncPtr(upperEyeRes);
    hwsOsFreeFuncPtr(lowerEyeRes);
	
    return GT_OK;
}

/*******************************************************************************
* mvHwsComHSerdesEOMHorizantalScan
*
* DESCRIPTION:
*       Returns the horizontal/vertical Rx eye margin.
*       Can be run after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical serdes number
*       timeout   - wait time in pSec
*
* OUTPUTS:
*       horizontal/vertical Rx eye margin and EOM status.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComHSerdesEOMHorizantalScan
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    GT_U32                    timeout,
	GT_U32                    *rightEdge,
	GT_U32                    *leftEdge
)
{
    GT_U32 eomLeftEdge;
    GT_U32 eomRightEdge;

	GT_U32 voltage, population, targetBER;
	GT_U32 ui;

	/*CHECK_STATUS(mvEomSetVerticalResolution(devNum, portGroup, serdesNum));*/
	CHECK_STATUS(mvEomInit(devNum, portGroup, serdesNum));

    CHECK_STATUS(mvHwsEomGetUi(devNum,portGroup,serdesNum,&ui));

	/*population = ((timeout * 1000) / ui) * 1000;*/ /* timeout in micro sec and ui in pico sec */
	population = (timeout / ui) * 1000; /* timeout in pico sec and ui in pico sec */
    eomRightEdge = 0;
    eomLeftEdge = 0;
	voltage = 0;
	targetBER =0;
    /* find left and right edges */
    CHECK_STATUS(mvHwsEomFindEyeCenter(devNum, portGroup, serdesNum, voltage, population, targetBER, &eomLeftEdge, &eomRightEdge));

	*rightEdge = eomRightEdge;
	*leftEdge = eomLeftEdge;

	CHECK_STATUS(mvHwsEomClose(devNum, portGroup, serdesNum));

    return GT_OK;
}

/*****************************************************************************/
#ifdef HWS_EOM_DEBUG
GT_STATUS eomTestOld
(
	GT_U8                     devNum,
	GT_U32                    serdesNum,
	GT_U32                    timeout
)
{
	GT_U32	rightEdge;
	GT_U32  leftEdge;
	GT_STATUS rc;

	rc = mvHwsComHSerdesEOMHorizantalScan(devNum, devNum, serdesNum, timeout, &rightEdge, &leftEdge);
	if (rc != GT_OK)
	{
		EOM_DBG(("mvHwsComHSerdesEOMHorizantalScan failed (0x%x)\n", rc));
		return rc;
	}

	EOM_DBG(("Right Edge: %d\n", rightEdge));
	EOM_DBG(("Left Edge:  %d\n", leftEdge));

	return GT_OK;
}

GT_STATUS eomTest2Old
(
	GT_U8                     devNum,
	GT_U32                    serdesNum,
	GT_U32                    timeout
)
{
	MV_HWS_SERDES_EOM_RES     results;
	GT_STATUS rc;
	
	rc = mvHwsComHSerdesEOMGet(devNum, devNum, serdesNum, timeout, &results);
	if (rc != GT_OK)
	{
		EOM_DBG(("mvHwsComHSerdesEOMGet failed (0x%x)\n", rc));
		return rc;
	}
	
	EOM_DBG(("horizontalEye: %d\n", results.horizontalEye));
	EOM_DBG(("verticalEye:  %d\n", results.verticalEye));
	
	return GT_OK;
}
#endif

