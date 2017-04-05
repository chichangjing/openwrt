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
* mvComPhyH28nmEomIf.h
*
* DESCRIPTION:
*       
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

#ifndef __mvComPhy28nmEOMIf_H
#define __mvComPhy28nmEOMIf_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define  EOM_VOLT_RANGE (32)
#define  EOM_PHASE_RANGE (128)

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
);

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
GT_STATUS mvHwsComH28nmEomClose
(
	GT_U8                     devNum,
	GT_U32                    portGroup,
	GT_U32                    serdesNum
);


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
	GT_U32*	   curUi
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
GT_STATUS mvHwsComH28nmEomGetDfeRes
(
	GT_U8      devNum,
	GT_U32     portGroup,
	GT_U32     serdesNum,
	GT_U32*	   dfeRes
);

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
	GT_U32	  *upMatrix,
	GT_U32    *loMatrix
);

/*******************************************************************************
* mvHwsComH28nmSerdesEOMGet
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
GT_STATUS mvHwsComH28nmSerdesEOMGet
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
    GT_U32                    timeout,
    MV_HWS_SERDES_EOM_RES     *results
);

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
);


void mvHwsComH28nmEomPrintMap
(
	GT_U32  leftEdge,
	GT_U32  rightEdge,
	GT_U32  *upperEyeRes,
	GT_U32  *lowerEyeRes
);

/* test function */
GT_STATUS eomTest2
(
	GT_U8                     devNum,
	GT_U32                    serdesNum,
	GT_U32                    timeout
);

GT_STATUS eomTest3
(
	GT_U8                     devNum,
	GT_U32                    serdesNum,
	GT_U32                    timeout
);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __mvComPhy28nmEOMIf_H */

