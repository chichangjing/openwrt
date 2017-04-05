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
* mvPortModeElements.h
*
* DESCRIPTION:
*       Ln2CpuPortunit registers and tables classes declaration.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/

#ifndef __mvPortModeElements_H
#define __mvPortModeElements_H

#ifdef __cplusplus
extern "C" {
#endif

/* General H Files */
#include <mvHwsPortInitIf.h>
#include <private/mvHwsPortTypes.h>
#include <common/siliconIf/mvSiliconIf.h>

#define NA_NUM (0xffffffff)

/*
 * Typedef: struct MV_HWS_PORT_INIT_PARAMS
 *
 * Description: Defines the serdes 10Bit status.
 *
 */
typedef struct
{
    MV_HWS_PORT_STANDARD    portStandard;
    MV_HWS_PORT_MAC_TYPE    portMacType;
    GT_U32                  portMacNumber;
    MV_HWS_PORT_PCS_TYPE    portPcsType;
    GT_U32                  portPcsNumber;
    MV_HWS_PORT_FEC_MODE    portFecMode;

    MV_HWS_SERDES_SPEED     serdesSpeed;
    GT_U32                  firstLaneNum;
    GT_U32                  numOfActLanes;
    GT_U32                  *activeLanesList;
    MV_HWS_SERDES_MEDIA     serdesMediaType;
    MV_HWS_SERDES_10B_MODE  serdes10BitStatus;

}MV_HWS_PORT_INIT_PARAMS;

MV_HWS_PORT_INIT_PARAMS *hwsPortsParamsArrayGet
(
	GT_U8			 devNum,
	GT_U32           portGroup,
	GT_U32           portNum
);

MV_HWS_PORT_INIT_PARAMS *hwsPortModeParamsGet
(
   GT_U8                    devNum,
   GT_U32                   portGroup,
   GT_U32                   portNum,
   MV_HWS_PORT_STANDARD     portMode
);

GT_STATUS hwsPortModeParamsSet
(
   GT_U8                    devNum,
   GT_U32                   portGroup,
   GT_U32                   portNum,
   MV_HWS_PORT_INIT_PARAMS  *portParams
);

GT_STATUS hwsPortsParamsCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  portNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_U32                  macNum,
    GT_U32                  pcsNum,
    GT_U32                  sdVecSize,
    GT_U32                  *sdVector
);

GT_STATUS hwsPortsParamsCfgGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_UOPT                 portNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_UOPT                 *macNum,
    GT_UOPT                 *pcsNum,
    GT_UOPT                 *sdVecSize,
    GT_UOPT                 **sdVector
);

GT_U32 hwsInitPortsModesParam
(
    MV_HWS_DEV_TYPE devType, 
    MV_HWS_PORT_INIT_PARAMS **portModes
);

#ifdef __cplusplus
}
#endif

#endif /* __mvPortModeElements_H */

