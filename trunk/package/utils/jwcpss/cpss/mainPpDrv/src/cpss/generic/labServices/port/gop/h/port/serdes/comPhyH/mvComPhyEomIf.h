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
* mvComPhyEomIf.h
*
* DESCRIPTION:
*       
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/

#ifndef __mvComPhyEOMIf_H
#define __mvComPhyEOMIf_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
	

#define  EOM_VOLT_RANGE (32)
#define  EOM_WINDOW (64)
#define  EOM_PHASE_RANGE (128)
#define  EOM_SERCH_RANGE (2560)

/************************** Register Definition *************************/

/* internal registers */
#define COM_H_DFE_Resolution_Control_Reg    0x1C
#define COM_H_EOM_DFE_Update_Control_Reg    0x20
#define COM_H_Adapted_DFE_Coefficient0_Reg  0x2C
#define COM_H_EOM_Circuit_Control_Reg		0x9C
#define COM_H_EOM_Set1_Control_Reg0			0xA0
#define COM_H_EOM_Set1_Control_Reg1			0xA4
#define COM_H_EOM_Set1_Population_Counter0  0xA8
#define COM_H_EOM_Set1_Population_Counter1  0xAC
#define COM_H_EOM_Set1_Population_Counter2  0xB0
#define COM_H_EOM_Set1_Population_Counter3  0xB4
#define COM_H_EOM_Set1_Threshold_Counter0   0xB8
#define COM_H_EOM_Set1_Threshold_Counter1   0xBC
#define COM_H_EOM_Set1_Threshold_Counter2   0xC0
#define COM_H_EOM_Set1_Threshold_Counter3   0xC4
#define COM_H_EOM_Set1_Sample_Counter0		0xC8
#define COM_H_EOM_Set1_Sample_Counter1		0xCC
#define COM_H_EOM_Set1_Sample_Counter2		0xD0
#define COM_H_EOM_Set1_Sample_Counter3		0xD4
#define COM_H_EOM_Set1_Error_Counter0		0xD8
#define COM_H_EOM_Set1_Error_Counter1		0xDC
#define COM_H_EOM_Set1_Error_Counter2		0xE0
#define COM_H_EOM_Set1_Error_Counter3		0xE4


/************************************************************************/

typedef enum {SearchLeft, SearchRight, Done} MV_STATE_MODE;

typedef struct  
{
  GT_U32 totalError;
}MV_EOM_RESULTS;

typedef struct
{
	GT_U32 eyeWidth;
	GT_U32 leftEyeWidth;
	GT_U32 rightEyeWidth;
} MV_EOM_EYE_DATA;


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
);

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
GT_STATUS mvHwsEomClose
(
	GT_U8                     devNum,
	GT_U32                    portGroup,
	GT_U32                    serdesNum
);


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
	GT_U32*		 curUi
);

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
	GT_U32*		 dfeRes
);

/*******************************************************************************
* mvHwsEomGetMatrix
*
* DESCRIPTION:
*       Calculate and returns the eye mapping matrix.
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
	GT_U32	  *upMatrix,
	GT_U32    *loMatrix
);

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
);

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
);

/*******************************************************************************
* mvHwsEomFindEyeCenter
*
* DESCRIPTION:
*       Returns the right/left Rx eye margin.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical Serdes number
*       timeout   - wait time in mSec
*       voltage   - voltage
*       population- error population
*       targetBer - target BER
*
* OUTPUTS:
*       right/left edge if the eye.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsEomFindEyeCenter
(
	GT_U8  devNum, 
	GT_U32 portGroup, 
	GT_U32 serdesNum, 
	GT_U32 voltage, 
	GT_U32 population, 
	GT_U32 targetBer, 
	GT_U32 *leftEdge, 
	GT_U32 *rightEdge
);

/*******************************************************************************
* mvHwsEomEyeMeasure
*
* DESCRIPTION:
*       Returns the Rx eye measurments.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       serdesNum - physical Serdes number
*       timeout   - wait time in mSec
*       voltage   - voltage
*       population- error population
*       targetBer - target BER
*       leftEdge  - eye left edge
*       rightEdge - eye right edge
*
* OUTPUTS:
*       eye measurments.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsEomEyeMeasure
(
	GT_U8  devNum, 
	GT_U32 portGroup, 
	GT_U32 serdesNum, 
	GT_U32 voltage, 
	GT_U32 population, 
	GT_U32 targetBer, 
	GT_U32 leftEdge, 
	GT_U32 rightEdge, 
	MV_EOM_EYE_DATA  *eomEyeData
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mvComPhyEOMIf_H */

