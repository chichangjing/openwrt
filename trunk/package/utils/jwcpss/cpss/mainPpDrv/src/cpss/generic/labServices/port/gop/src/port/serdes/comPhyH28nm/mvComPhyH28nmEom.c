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
*       $Revision: 12 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <serdes/mvHwsSerdesIf.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <serdes/comPhyH/mvComPhyEomIf.h>
#include <serdes/comPhyH28nm/mvComPhyH28nmEomIf.h>

/*#define HWS_EOM_DEBUG*/

#ifdef  HWS_EOM_DEBUG
#define EOM_DBG(s) 	osPrintf s
#else
#define EOM_DBG(s)
#endif /* HWS_EOM_DEBUG */

#ifdef FreeRTOS
#include "printf.h"
#define osPrintf    printf
#endif

GT_BOOL printMatrix = GT_TRUE;


#define HWS_DEBUG

static GT_STATUS mvHwsComH28nmEomSetVerticalResolution(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum)
{
  GT_U32 data;

  /* check analog reset */
  CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_Adapted_DFE_Coefficient0_Reg, &data, 0));
  if(((data >> 10) & 0x3F) > 40)
  {
    /* Decision Feedback Equalization (DFE) Resolution */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_DFE_Resolution_Control_Reg, (1 << 8), (3 << 8)));
  }

  return GT_OK;
}

static GT_STATUS mvHwsComH28nmEomHwInit(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum)
{
  GT_U32 data;
  GT_32 wait4EOMReadyCount = 10;
	

  /* EOM enable */
  CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Circuit_Control_Reg, (1 << 15), (1 << 15)));

  /* check EOM ready */
  do
  {
    hwsOsTimerWkFuncPtr(5);
    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Circuit_Control_Reg, &data, 0));
  } while(!((data >> 13) & 1) && (--wait4EOMReadyCount > 0));

  if ((data >> 13) & 1)
  {
    /* Use external tap override register values */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_DFE_Update_Control_Reg, 0, 0x3F));
    return GT_OK;
  }

  return GT_FAIL;
}

static GT_STATUS mvHwsComH28nmEomSetValid(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum, GT_U32 population)
{
  GT_U32 data, ready, sample;

  population = population; /* avoid warnings */

  /* set EOM valid */
  CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Control_Reg0, (1 << 13), (1 << 13)));

  /* check EOM ready */
  do
  {
		/* check EOM ready */
		CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Control_Reg1, &data, 0));
		ready = (data >> 15) & 1;

        /* read sample and compare with population */
		CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Sample_Counter3, &data, 0));
		CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Sample_Counter2, &sample, 0));
        sample = (sample << 16) + (data & 0xFFFF);

  } while(!ready);

  if (ready)
  {
        /* set EOM valid */
		CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Control_Reg0, 0, (1 << 13)));
		return GT_OK;
  }

  return GT_FAIL;
}


static GT_STATUS mvHwsComH28nmEomSetPhase(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum, GT_U32 targetPhase, GT_U32 population)
{
  static GT_32  currentPhase = 0;

  /* read Eye-Opening Measurement (EOM) Phase */
  /*CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Control_Reg1, &data, 0));
  currentPhase = data & 0x7F;*/
  /* move to target phase value with step = 4 */
  if (((currentPhase - 4) > (GT_32)targetPhase ) || ((GT_U32)(currentPhase + 4) < targetPhase))
  {
    while((currentPhase - 4) > (GT_32)targetPhase)
    {
      currentPhase -= 4;
      CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Control_Reg1, currentPhase, 0x7F));
      CHECK_STATUS(mvHwsComH28nmEomSetValid(devNum, (GT_U8)portGroup, serdesNum, population));
    }
    while((GT_U32)(currentPhase + 4) < targetPhase)
    {
      currentPhase += 4;
      CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Control_Reg1, currentPhase, 0x7F));
      CHECK_STATUS(mvHwsComH28nmEomSetValid(devNum, (GT_U8)portGroup, serdesNum, population));
    }
  }
  CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Control_Reg1, targetPhase, 0x7F));
  CHECK_STATUS(mvHwsComH28nmEomSetValid(devNum, (GT_U8)portGroup, serdesNum, population));
  currentPhase = targetPhase;

  return GT_OK;
}

static GT_STATUS mvHwsComH28nmEomGetBer(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum, GT_U32 sign1, GT_U32 sign2, GT_U32 voltage, GT_U32 phase, GT_U32 population, GT_U32 *totalError, GT_U32 *errorRate)
{
  GT_U32 data;
  GT_U32 error[2];
  GT_U32 sign[2];
  GT_U32 i;

  /* set EOM counter */
  data = (population/2) & 0xFFFF;
  CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Population_Counter3, data, 0));
  
  data = (population/2) >> 16;
  CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Population_Counter2, data, 0));
  
  /* clear other counter bits [32:63] */
  CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Population_Counter1, 0, 0));

  CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Population_Counter0, 0, 0));

  /* set EOM voltage */
  CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Control_Reg0, voltage, 0x3F));

  /* set phase */
  CHECK_STATUS(mvHwsComH28nmEomSetPhase(devNum, (GT_U8)portGroup, serdesNum, phase, population));

  sign[0] = sign1;
  sign[1] = sign2;
  error[0] = 0;
  error[1] = 0;
  for (i = 0; i < 2; i++)
  {
    /* Set EOM_SET1_SIGN_BITS  = Sign1/2
       Set EOM_MEASURE_STATE = Sign1/2 */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Control_Reg0, (sign[i] << 6), (3 << 6)));
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Circuit_Control_Reg, (sign[i] << 10), (3 << 10)));

    CHECK_STATUS(mvHwsComH28nmEomSetValid(devNum, (GT_U8)portGroup, serdesNum, population));

    /* accumulate errors */
    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Error_Counter3, &error[i], 0));
    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Error_Counter2, &data, 0));
    error[i] += (data << 16);
  }

  /* temporary work with 32 bit counters */
  *totalError = error[0] + error[1];
  *errorRate = *totalError/population;

  return GT_OK;
}

static GT_STATUS mvHwsComH28nmEomFindEyeCenter(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum, GT_U32 population, GT_U32 *leftEdge, GT_U32 *rightEdge)
{
  GT_U32 voltage, phase;
  GT_U32 error, rate;
  MV_STATE_MODE state;

  voltage = 0;
  phase = 0;
  error = 0xFFFFFFFF;
  state = SearchLeft;

  *leftEdge = 0;
  *rightEdge = 0;
  for (phase = EOM_WINDOW; (state != Done) && (phase < (EOM_SERCH_RANGE-EOM_WINDOW)); phase++)
  {
	  /*osPrintf("Phase %d\n", phase);*/
    CHECK_STATUS(mvHwsComH28nmEomGetBer(devNum, portGroup, serdesNum, 1, 3, voltage, phase, population, &error, &rate));
    if (state == SearchLeft)
    {
      if (error == 0)
      {
        *leftEdge = phase;
        state = SearchRight;

      }
    }
    else
    {
      if (error > 0)
      {
        *rightEdge = phase;
        state = Done;
      }
    }
  }

  /* check eomLeftEdge */
  CHECK_STATUS(mvHwsComH28nmEomSetPhase(devNum, portGroup, serdesNum, *leftEdge, population));
  state = SearchLeft;

  for (phase = *leftEdge; (state != Done) && (phase > 0); phase--)
  {
    CHECK_STATUS(mvHwsComH28nmEomGetBer(devNum, portGroup, serdesNum, 1, 3, voltage, phase, population, &error, &rate));
    if (error == 0)
    {
        *leftEdge = phase;
    }
    else
    {
            state = Done;
    }
  }

  return GT_OK;
}


/*******************************************************************************
* mvHwsComH28nmEomMapEye
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
static GT_STATUS mvHwsComH28nmEomMapEye
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  serdesNum,
    GT_U32  population,
    GT_U32  leftEdge,
    GT_U32  rightEdge,
    GT_U32  *upperEyeRes,
    GT_U32  *lowerEyeRes
)
{
  GT_U32 phase;
  GT_32  voltage;
  GT_U32 errorRate;
  GT_U32 done;

  /* Begin Upper Half of Eye */
  done = 0;
  for (voltage = 0; (voltage < EOM_VOLT_RANGE) && !done; voltage++)
  {
    done = 1;
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Control_Reg0, (GT_U32)voltage, 0x3F));
    for (phase = leftEdge; phase < rightEdge; phase++)
    {
      /* set EOM_SET1_PHASE */
      CHECK_STATUS(mvHwsComH28nmEomSetPhase(devNum, portGroup, serdesNum, phase, population));
      CHECK_STATUS(mvHwsComH28nmEomGetBer(devNum, portGroup, serdesNum, 2, 0, voltage, phase, population, &(upperEyeRes[voltage*EOM_PHASE_RANGE+(phase - leftEdge)]), &errorRate));
      if (upperEyeRes[voltage*EOM_PHASE_RANGE+(phase - leftEdge)] < 1000)
      {
        done = 0;
      }
    }
  }
  /* Begin Lower Half of Eye */
  done = 0;
  for (voltage = 0; (voltage < EOM_VOLT_RANGE) && !done; voltage++)
  {
     done = 1;
    /* set EOM_SET1_VOLTAGE  */
    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Set1_Control_Reg0, voltage, 0x3F));
    for (phase = leftEdge; phase < rightEdge; phase++)
    {
      /* set EOM_SET1_PHASE */
      CHECK_STATUS(mvHwsComH28nmEomSetPhase(devNum, portGroup, serdesNum, phase, population));
      CHECK_STATUS(mvHwsComH28nmEomGetBer(devNum, portGroup, serdesNum, 3, 1, voltage, phase, population, &(lowerEyeRes[voltage*EOM_PHASE_RANGE+(phase - leftEdge)]), &errorRate));
      if (lowerEyeRes[voltage*EOM_PHASE_RANGE+(phase - leftEdge)] < 1000)
      {
        done = 0;
      }
    }
  }

  return GT_OK;
}





/*******************************************************************************
* mvHwsComH28nmEomGetUi
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
GT_STATUS mvHwsComH28nmEomGetUi
(
    GT_U8      devNum,
    GT_U32     portGroup,
    GT_U32     serdesNum,
    GT_U32*      curUi
)
{
    GT_U32 gen;
	

    if (curUi == NULL)
    {
        return GT_BAD_VALUE;
    }

    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, (GT_U8)portGroup, EXTERNAL_REG, (GT_U8)serdesNum, SERDES_EXTERNAL_CONFIGURATION_0, &gen, 0));
	
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
* mvHwsComH28nmEomGetDfeRes
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
GT_STATUS mvHwsComH28nmEomGetDfeRes
(
    GT_U8      devNum,
    GT_U32     portGroup,
    GT_U32     serdesNum,
    GT_U32*      dfeRes
)
{
    GT_U32 data;

    if (dfeRes == NULL)
    {
        return GT_BAD_VALUE;
    }
    CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_DFE_Resolution_Control_Reg, &data, 0));
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
* mvHwsComH28nmEomInit
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
GT_STATUS mvHwsComH28nmEomInit
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum
)
{
  CHECK_STATUS(mvHwsComH28nmEomSetVerticalResolution(devNum, (GT_U8)portGroup, (GT_U8)serdesNum));
  CHECK_STATUS(mvHwsComH28nmEomHwInit(devNum, (GT_U8)portGroup, (GT_U8)serdesNum));

    return GT_OK;
}

/*******************************************************************************
* mvHwsComH28nmEomClose
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
GT_STATUS mvHwsComH28nmEomClose(GT_U8 devNum, GT_U32  portGroup, GT_U32  serdesNum)
{

  /* EOM disable */
  CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_Circuit_Control_Reg, 0, (1 << 15)));

  /* Use DFE auto-trained taps */
  CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, INTERNAL_REG, (GT_U8)serdesNum, COM_H_EOM_DFE_Update_Control_Reg, 0, 0x3F));

  return GT_OK;
}


/*******************************************************************************
* mvHwsComH28nmEomGetMatrix
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
GT_STATUS mvHwsComH28nmEomGetMatrix
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
    GT_U32 population;
    GT_U32 ui;
    GT_U32 left, right;

    CHECK_STATUS(mvHwsComH28nmEomGetUi(devNum,(GT_U8)portGroup,(GT_U8)serdesNum,&ui));

    population = ((timeout * 1000) / ui) * 1000; /* timeout in micro sec and ui in pico sec */

    /* find left and right edges */
    CHECK_STATUS(mvHwsComH28nmEomFindEyeCenter(devNum, (GT_U8)portGroup, (GT_U8)serdesNum, population, &left, &right));

    CHECK_STATUS(mvHwsComH28nmEomMapEye(devNum, (GT_U8)portGroup, (GT_U8)serdesNum, population, left, right, upMatrix, loMatrix));

    if (rowSize != NULL)
    {
        *rowSize = right - left;
    }
    return GT_OK;
}

#ifdef HWS_DEBUG
/*******************************************************************************
* mvHwsComH28nmEomPrintMap
*
* DESCRIPTION:
*       Print matrix results.
*
* INPUTS:
*       leftEdge     - left edge
*       rightEdge    - right edge
*       upperEyeRes  - upper matrix
*       lowerEyeRes  - lower matrix
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void mvHwsComH28nmEomPrintMap
(
	GT_U32  leftEdge,
    GT_U32  rightEdge,
    GT_U32  *upperEyeRes,
    GT_U32  *lowerEyeRes
)
{
    GT_U32 i, j;
  
	/* print Upper Eye */
	EOM_DBG(("\n\nEOM Eye:: eomLeftEdge = %d, rigthEdge = %d\n", leftEdge, rightEdge));

	for (i = EOM_VOLT_RANGE-1; i > 0; i--)
	{
		EOM_DBG(("\n%d\t##########", i));
		for (j = 0; j <= (rightEdge - leftEdge); j++)
		{
			if (upperEyeRes[i*(EOM_PHASE_RANGE)+j] == 0)
			{
				EOM_DBG((" "));
			}
			else if (upperEyeRes[i*(EOM_PHASE_RANGE)+j] < 100)
			{
				EOM_DBG(("*"));
			}
			else if (upperEyeRes[i*(EOM_PHASE_RANGE)+j] < 10000)
			{
				EOM_DBG(("+"));
			}
            else
            {
				EOM_DBG(("#"));
            }
		}
		EOM_DBG(("##########"));
	}
	
	for (i = 0; i < EOM_VOLT_RANGE; i++)
	{
		EOM_DBG(("\n%d\t##########", i));
		for (j = 0; j <= (rightEdge - leftEdge) ; j++)
		{
			if (lowerEyeRes[i*(EOM_PHASE_RANGE)+j] == 0)
			{
				EOM_DBG((" "));
			}
			else if (lowerEyeRes[i*(EOM_PHASE_RANGE)+j] < 100)
			{
				EOM_DBG(("*"));
			}
			else if (lowerEyeRes[i*(EOM_PHASE_RANGE)+j] < 10000)
			{
				EOM_DBG(("+"));
			}
            else
            {
                EOM_DBG(("#"));
            }
		}
		EOM_DBG(("##########"));
	}
    EOM_DBG(("\n"));
}
#endif

/*******************************************************************************
* mvHwsComH28nmSerdesMatrixMap
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
GT_STATUS mvHwsComH28nmSerdesMatrixMap
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  serdesNum,
    GT_U32                  timeout,
    GT_U32					*rowSize,
    GT_U32					*upperMatrix,
    GT_U32					*lowerMatrix
)
{
	GT_U32 eomLeftEdge;
	GT_U32 eomRightEdge;

	GT_U32 matrixSize;

	GT_U32 population;
	GT_U32 ui;

	if(NULL == upperMatrix || NULL == lowerMatrix)
	{
		return GT_BAD_PTR;
	}

	CHECK_STATUS(mvHwsComH28nmEomSetVerticalResolution(devNum, (GT_U8)portGroup, serdesNum));

	CHECK_STATUS(mvHwsComH28nmEomHwInit(devNum, (GT_U8)portGroup, serdesNum));

	CHECK_STATUS(mvHwsComH28nmEomGetUi(devNum,(GT_U8)portGroup,serdesNum,&ui));

	population = ((timeout * 1000) / ui) * 1000; /* timeout in micro sec and ui in pico sec */
	eomRightEdge = 0;
	eomLeftEdge = 0;

	/* find left and right edges */
	CHECK_STATUS(mvHwsComH28nmEomFindEyeCenter(devNum, (GT_U8)portGroup, serdesNum, population, &eomLeftEdge, &eomRightEdge));

    if (eomRightEdge == eomLeftEdge)
    {
        EOM_DBG(("\nCannot run EOM with eomLeftEdge = %d, rigthEdge = %d\n", eomLeftEdge, eomRightEdge));
        return GT_FAIL;
    }

	matrixSize = EOM_PHASE_RANGE * EOM_VOLT_RANGE * sizeof(GT_U32);

	hwsOsMemSetFuncPtr(upperMatrix, 0xff, matrixSize);
	hwsOsMemSetFuncPtr(lowerMatrix, 0xff, matrixSize);

	CHECK_STATUS(mvHwsComH28nmEomMapEye(devNum, (GT_U8)portGroup, serdesNum, population, eomLeftEdge, eomRightEdge, upperMatrix, lowerMatrix));

	EOM_DBG(("eomLeftEdge = %d, rigthEdge = %d\n", eomLeftEdge, eomRightEdge));
	*rowSize = eomRightEdge - eomLeftEdge;
	EOM_DBG(("rowSize = %d\n", *rowSize));

	CHECK_STATUS(mvHwsComH28nmEomClose(devNum, (GT_U8)portGroup, serdesNum));

#ifdef HWS_DEBUG
    mvHwsComH28nmEomPrintMap(0, *rowSize, upperMatrix, lowerMatrix);
#endif

    return GT_OK;
}


/*******************************************************************************
* mvHwsComH28nmSerdesEOMGetTry
*
* DESCRIPTION:
*       Returns the horizontal/vertical Rx eye margin.
*       Can be run after create port.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       serdesNum   - physical serdes number
*       timeout     - wait time in msec
*       eomResults  - for returning: 
*                       1. horizontal Rx eye margin
*                       2. vertical Rx eye margin
*                       3. status
*                       4. row size of the EOM matrix
*                       5. a pointer to the upper matrix
*                       6. a pointer to the lower matrix
* OUTPUTS:
*       eomResults is updated.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComH28nmSerdesEOMGetTry
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    GT_U32                    timeout,
    MV_HWS_SERDES_EOM_RES     *eomResults,
    GT_U32				      *eomLeftEdgeRes,
    GT_U32					  *eomRightEdgeRes
)
{
    GT_U32 eomLeftEdge;
    GT_U32 eomRightEdge;
    GT_U32 population;
	GT_U32 ui;


    if(NULL == eomResults) 
        return GT_BAD_PTR;

    CHECK_STATUS(mvHwsComH28nmEomSetVerticalResolution(devNum, (GT_U8)portGroup, serdesNum));

    CHECK_STATUS(mvHwsComH28nmEomHwInit(devNum, (GT_U8)portGroup, serdesNum));

    CHECK_STATUS(mvHwsComH28nmEomGetUi(devNum,(GT_U8)portGroup,serdesNum,&ui));
   
    population = ((timeout * 1000) / ui) * 1000; /* timeout in micro sec and ui in pico sec */
    eomRightEdge = 0;
    eomLeftEdge = 0;
    /* find left and right edges */
    CHECK_STATUS(mvHwsComH28nmEomFindEyeCenter(devNum, (GT_U8)portGroup, serdesNum, population, &eomLeftEdge, &eomRightEdge));

    if (eomRightEdge == eomLeftEdge)
    {
#ifdef HWS_DEBUG
	EOM_DBG(("\nCannot run EOM with eomLeftEdge = %d, rigthEdge = %d\n", eomLeftEdge, eomRightEdge));
#endif
        return GT_FAIL;
    }
	*eomLeftEdgeRes = eomLeftEdge;
	*eomRightEdgeRes = eomRightEdge;

	EOM_DBG(("\neomLeftEdge = %d, rigthEdge = %d\n", *eomLeftEdgeRes, *eomRightEdgeRes));

    return GT_OK;
}



/*******************************************************************************
* mvHwsComH28nmSerdesEOMGet
*
* DESCRIPTION:
*       Returns the horizontal/vertical Rx eye margin.
*       Can be run after create port.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       serdesNum   - physical serdes number
*       timeout     - wait time in msec
*       eomResults  - for returning: 
*                       1. horizontal Rx eye margin
*                       2. vertical Rx eye margin
*                       3. status
*                       4. row size of the EOM matrix
*                       5. a pointer to the upper matrix
*                       6. a pointer to the lower matrix
* OUTPUTS:
*       eomResults is updated.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsComH28nmSerdesEOMGet
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    GT_U32                    timeout,
    MV_HWS_SERDES_EOM_RES     *eomResults
)
{
	GT_STATUS retVal;
	GT_U32 tryCount = 0;
	GT_U32 eomLeftEdge = 0, eomRightEdge = 0, matrixSize, population;
	GT_U32 dfeRes, phaseRang, i, j;
    GT_U32 ui, maxVolt, minVolt, done;
    GT_U32 *upperEyeRes;
    GT_U32 *lowerEyeRes;
	GT_U32 rowSize;

	if (eomResults == NULL)
	{
		return GT_BAD_PARAM;
	}

	CHECK_STATUS(mvHwsComH28nmEomGetUi(devNum,(GT_U8)portGroup,serdesNum,&ui));
    phaseRang = 128;
    if (ui < 160)
    {
        phaseRang = 64;
    }
	population = ((timeout * 1000) / ui) * 1000; /* timeout in micro sec and ui in pico sec */

	for( tryCount = 0; tryCount < 3; tryCount++)
	{
		retVal = mvHwsComH28nmSerdesEOMGetTry(devNum, portGroup, serdesNum, timeout, eomResults, &eomLeftEdge, &eomRightEdge);
		rowSize = eomRightEdge - eomLeftEdge;
		if ((retVal == GT_OK) && (rowSize > 10))
			break;
	}
	EOM_DBG(("\nNumber of retries = %d \n ", tryCount));
    matrixSize = EOM_PHASE_RANGE * EOM_VOLT_RANGE * sizeof(GT_U32);
    upperEyeRes = eomResults->upperMatrix;
    lowerEyeRes = eomResults->lowerMatrix;

    hwsOsMemSetFuncPtr(upperEyeRes, 0xff, matrixSize);
    hwsOsMemSetFuncPtr(lowerEyeRes, 0xff, matrixSize);
	
    CHECK_STATUS(mvHwsComH28nmEomMapEye(devNum, (GT_U8)portGroup, serdesNum, population, eomLeftEdge, eomRightEdge, upperEyeRes, lowerEyeRes));

	EOM_DBG(("eomLeftEdge = %d, rigthEdge = %d\n", eomLeftEdge, eomRightEdge));
	eomResults->rowSize = eomRightEdge - eomLeftEdge;
	EOM_DBG(("rowSize = %d\n", eomResults->rowSize));
	
    CHECK_STATUS(mvHwsComH28nmEomGetDfeRes(devNum, (GT_U8)portGroup, serdesNum, &dfeRes));

    /* calculate Vertical Eye = (MaxVoltage - MinVoltage)/DFE_RES */
    maxVolt = 0;
    done = 0;
    for (i = EOM_VOLT_RANGE-1; (i > 0) && (!done); i--)
    {
        for (j = 0; j <= (eomRightEdge - eomLeftEdge) ; j++)
        {
            if (upperEyeRes[i*(EOM_PHASE_RANGE)+j] == 0)
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
            if (lowerEyeRes[i*(EOM_PHASE_RANGE)+j] == 0)
            {
                minVolt = i;
                done = 1;
                break;
            }
        }
    }
    
	/* calculate Horizontal Eye = (RigthEdge - eomLeftEdge)*UI/phaseRang, where voltage = 0 */
    eomResults->horizontalEye = ((eomRightEdge-eomLeftEdge)*ui)/phaseRang;
    eomResults->verticalEye = (maxVolt + minVolt)*dfeRes;
    
	EOM_DBG(("\nmvHwsComH28nmEomClose\n"));
    CHECK_STATUS(mvHwsComH28nmEomClose(devNum, (GT_U8)portGroup, serdesNum));

#ifdef HWS_DEBUG
    if (printMatrix == GT_TRUE)
    {  
		EOM_DBG(("\nHorizontal Eye is %d ps.", (((eomRightEdge - eomLeftEdge) * ui)/phaseRang)));
		EOM_DBG(("\nVertical Eye is %d mV.\n", (maxVolt + minVolt)*dfeRes));
		mvHwsComH28nmEomPrintMap(0, eomResults->rowSize, upperEyeRes, lowerEyeRes);
    }
#endif

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
GT_STATUS mvHwsComH28nmSerdesEOMHorizantalScan
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

	GT_U32 population;
	GT_U32 ui;

	/*CHECK_STATUS(mvHwsComH28nmEomSetVerticalResolution(devNum, (GT_U8)portGroup, serdesNum));*/
	CHECK_STATUS(mvHwsComH28nmEomHwInit(devNum, (GT_U8)portGroup, serdesNum));

    CHECK_STATUS(mvHwsComH28nmEomGetUi(devNum,(GT_U8)portGroup,serdesNum,&ui));

	/*population = ((timeout * 1000) / ui) * 1000;*/ /* timeout in micro sec and ui in pico sec */
	population = (timeout / ui) * 1000; /* timeout in pico sec and ui in pico sec */
    eomRightEdge = 0;
    eomLeftEdge = 0;
    /* find left and right edges */
    CHECK_STATUS(mvHwsComH28nmEomFindEyeCenter(devNum, (GT_U8)portGroup, serdesNum, population, &eomLeftEdge, &eomRightEdge));

	*rightEdge = eomRightEdge;
	*leftEdge = eomLeftEdge;

	CHECK_STATUS(mvHwsComH28nmEomClose(devNum, (GT_U8)portGroup, serdesNum));

    return GT_OK;
}

GT_STATUS eomTest
(
	GT_U8	devNum,
	GT_U32	serdesNum,
	GT_U32	timeout
)
{
	GT_U32	rightEdge;
	GT_U32  leftEdge;
	GT_STATUS rc;

	rc = mvHwsComH28nmSerdesEOMHorizantalScan(devNum, devNum, serdesNum, timeout, &rightEdge, &leftEdge);
	if (rc != GT_OK)
	{
		EOM_DBG(("mvHwsComHSerdesEOMHorizantalScan failed (0x%x)\n", rc));
		return rc;
	}

	EOM_DBG(("Right Edge: %d\n", rightEdge));
	EOM_DBG(("Left Edge:  %d\n", leftEdge));

	return GT_OK;
}

GT_STATUS eomTest2
(
	GT_U8	devNum,
	GT_U32	serdesNum,
	GT_U32	timeout
)
{
	MV_HWS_SERDES_EOM_RES	results;
	GT_STATUS rc;
	
	rc = mvHwsComH28nmSerdesEOMGet(devNum, devNum, serdesNum, timeout, &results);
	if (rc != GT_OK)
	{
		EOM_DBG(("mvHwsComH28nmSerdesEOMGet failed (0x%x)\n", rc));
		return rc;
	}
	
	EOM_DBG(("horizontalEye: %d\n", results.horizontalEye));
	EOM_DBG(("verticalEye:  %d\n", results.verticalEye));
	
	return GT_OK;
}

#ifndef FreeRTOS
GT_U32 upperMatrix[EOM_VOLT_RANGE * EOM_PHASE_RANGE];
GT_U32 lowerMatrix[EOM_VOLT_RANGE * EOM_PHASE_RANGE];

GT_STATUS eomTest3
(
	GT_U8	devNum,
	GT_U32	serdesNum,
	GT_U32	timeout
)
{
	GT_U32 rowSize;

	GT_STATUS rc;
	
	rc = mvHwsComH28nmSerdesMatrixMap(devNum, devNum, serdesNum, timeout, &rowSize, upperMatrix, lowerMatrix);
	if (rc != GT_OK)
	{
		EOM_DBG(("mvHwsComH28nmSerdesMatrixMap failed (0x%x)\n", rc));
		return rc;
	}
	
	EOM_DBG(("rowSize: %d\n", rowSize));
	
	return GT_OK;
}
#endif


