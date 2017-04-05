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
* mvHwsDdr3Bap.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/




#ifndef _MV_HWS_DDR3_BAP_H_
#define _MV_HWS_DDR3_BAP_H_

#include <common/siliconIf/mvSiliconIf.h>
#include <mvDdr3TrainingIp.h>

#ifdef __cplusplus
   extern "C"
   {
#endif

/********************************* definitions ********************************/

#define MV_BAP_MAX_CHANNELS 2

/******************************************************************************/
/* Definitions of PHY registers                                               */
/******************************************************************************/



/********************************* macros *************************************/

/********************************* structures *********************************/

typedef struct 
{
	MV_HWS_ALGO_TYPE    algoType;
	GT_U32              algoFlowMask;
} MV_HWS_BAP_ALGO_PARAMS;

/******************************************************************************/

/*******************************************************************************
* mvHwsDdr3BapRegRead
*
* DESCRIPTION:
*       Read BAP register.
*
* INPUTS:
*
* OUTPUTS:
*       data      - array for read data
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsDdr3BapRegRead
( 
    GT_U8                 devNum,
    GT_U32                bapId, 
    GT_U32                regAddr, 
	GT_U32                *dataLow,
	GT_U32                *dataHigh,
	GT_U32                maskLow,
    GT_U32                maskHigh
);

/*******************************************************************************
* mvHwsDdr3BapRegWrite
*
* DESCRIPTION:
*       Write BAP register.
*
* INPUTS:
*
* OUTPUTS:
*       data      - array for read data
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsDdr3BapRegWrite
( 
    GT_U8                 devNum,
    GT_U32                bapId, 
    GT_U32                regAddr, 
	GT_U32                dataLow,
    GT_U32                dataHigh,
	GT_U32                maskLow, 
    GT_U32                maskHigh
);

/*******************************************************************************
* mvHwsDdr3BapMemRead
*
* DESCRIPTION:
*       Read 128 bits (16 bytes) from the external memory.
*
* INPUTS:
*       devNum       - system device number
*       bapId	     - BAP ID
*       baseAddress  - offset physical port number
*
* OUTPUTS:
*       data      - array for read data
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsDdr3BapMemRead
(  
	GT_U8	    devNum,
	GT_U32      bapId,
	GT_U32      baseAddress,
	GT_U32      data[4]
);

/*******************************************************************************
* mvHwsDdr3BapMemWrite
*
* DESCRIPTION:
*       Write 128 bits (16 bytes) from the external memory.
*
* INPUTS:
*       devNum       - system device number
*       bapId	     - BAP ID
*       baseAddress  - offset physical port number
*       data         - array of data to be written
*
* OUTPUTS:
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsDdr3BapMemWrite
(  
	GT_U8       devNum,
	GT_U32      bapId,
	GT_U32      baseAddress,
	GT_U32      data[4]
);

/*******************************************************************************
* ddr3BapRunAlg
*
* DESCRIPTION:
*       Run DDR algorithm to configure the TM.
*
* INPUTS:
*       devNum       - system device number
*       eAlgoType    - algorithm type (static/dynamic)
*
* OUTPUTS:
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS    mvHwsDdr3BapRunAlg
( 
	GT_U8					  devNum, 
	MV_HWS_BAP_ALGO_PARAMS    *bapAlgoParams
);


#ifdef __cplusplus
   }
#endif


/*****************************************************************************/

#endif /* _MV_HWS_DDR3_BAP_H_ */


