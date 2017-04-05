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
* mvHwsMacIf.h
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 13 $
*
*******************************************************************************/

#ifndef __mvHwsMacIf_H
#define __mvHwsMacIf_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mvHwsPortCfgIf.h>
#include <private/mvHwsPortTypes.h>
#include <private/mvHwsPortMiscIf.h>


typedef GT_STATUS (*MV_HWS_MAC_RESET_FUNC_PTR)
(
    GT_U8             devNum,
    GT_U32            portGroup,
    GT_U32            macNum,
    MV_HWS_RESET      action
);

typedef GT_STATUS (*MV_HWS_MAC_MODE_CFG_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_U32                  numOfLanes
);

typedef GT_STATUS (*MV_HWS_MAC_LB_CFG_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_LB_TYPE     lbType
);

typedef GT_STATUS (*MV_HWS_MAC_LB_GET_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_LB_TYPE     *lbType
);

typedef GT_STATUS (*MV_HWS_MAC_LINK_GET_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_BOOL                 *linkStatus
);

typedef GT_STATUS (*MV_HWS_MAC_ACTIVE_STATUS_FUNC_PTR)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_U32                  *numOfLanes
);

typedef GT_STATUS (*MV_HWS_MAC_CLEAR_CHANNEL_CFG)
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_U32                  txIpg,
    GT_U32                  txPreamble,
    GT_U32                  rxPreamble,
    MV_HWS_PORT_CRC_MODE    txCrc,
    MV_HWS_PORT_CRC_MODE    rxCrc
);

typedef char* (*MV_HWS_MAC_TYPE_GET_FUNC_PTR)(void);

typedef GT_STATUS (*MV_HWS_MAC_FC_STATE_CFG_FUNC_PTR)
(
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          macNum,
    MV_HWS_PORT_FLOW_CONTROL_ENT    fcState
);

typedef struct
{
  MV_HWS_MAC_RESET_FUNC_PTR     macRestFunc;
  MV_HWS_MAC_MODE_CFG_FUNC_PTR  macModeCfgFunc;
  MV_HWS_MAC_LB_CFG_FUNC_PTR    macLbCfgFunc;
  MV_HWS_MAC_LB_GET_FUNC_PTR    macLbStatusGetFunc;
  MV_HWS_MAC_LINK_GET_FUNC_PTR  macLinkGetFunc;
  MV_HWS_MAC_TYPE_GET_FUNC_PTR  macTypeGetFunc;
  MV_HWS_MAC_ACTIVE_STATUS_FUNC_PTR macActiveStatusFunc;
  MV_HWS_MAC_CLEAR_CHANNEL_CFG      macClearChannelCfgFunc;
  MV_HWS_MAC_FC_STATE_CFG_FUNC_PTR  macFcStateCfgFunc;

}MV_HWS_MAC_FUNC_PTRS;

/*******************************************************************************
* hwsMacIfInit
*
* DESCRIPTION:
*       Init all supported MAC types.
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
void hwsMacIfInit(void);

/*******************************************************************************
* mvHwsMacReset
*
* DESCRIPTION:
*       Set the MAC to reset or exit from reset.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*       macType   - MAC type according to port configuration
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
GT_STATUS mvHwsMacReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_MAC_TYPE    macType,
    MV_HWS_RESET            action
);

/*******************************************************************************
* mvHwsMacModeCfg
*
* DESCRIPTION:
*       Set the internal mux's to the required MAC in the GOP.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*       macType   - MAC type according to port configuration
*       numOfLanes - number of lanes aggregated by current mac/port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsMacModeCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_MAC_TYPE    macType,
    GT_U32                  numOfLanes
);

/*******************************************************************************
* mvHwsMacLoopbackCfg
*
* DESCRIPTION:
*       Configure MAC loopback.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*       macType   - MAC type according to port configuration
*       lbType    - loopback type (no loopback, rx2tx or tx2rx)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsMacLoopbackCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_MAC_TYPE    macType,
    MV_HWS_PORT_LB_TYPE     lbType
);

/*******************************************************************************
* mvHwsMacLinkStatusGet
*
* DESCRIPTION:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*       macType   - MAC type according to port configuration
*
* OUTPUTS:
*       linkStatus - link up, if true
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsMacLinkStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_MAC_TYPE    macType,
    GT_BOOL                 *linkStatus
);

/*******************************************************************************
* mvHwsMacLoopbackStatusGet
*
* DESCRIPTION:
*       Retrive MAC loopback status.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*       macType   - MAC type according to port configuration
*
* OUTPUTS:
*       lbType    - supported loopback type
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsMacLoopbackStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_MAC_TYPE    macType,
    MV_HWS_PORT_LB_TYPE     *lbType
);

/*******************************************************************************
* mvHwsMacModeSelectDefaultCfg
*
* DESCRIPTION:
*       MAC mode select default value configuration (needed for AP mechanism).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*       macType   - MAC type according to port configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsMacModeSelectDefaultCfg
(
    GT_U8                 devNum,
    GT_U32                portGroup,
    GT_U32                macNum,
    MV_HWS_PORT_MAC_TYPE  macType
);

/*******************************************************************************
* mvHwsMacActiveStatusGet
*
* DESCRIPTION:
*       Return number of MAC active lanes or 0, if current MAC isn't active.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*       macType   - MAC type according to port configuration
*
* OUTPUTS:
*       numOfLanes - number of lanes agregated in PCS
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsMacActiveStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_MAC_TYPE    macType,
    GT_U32                  *numOfLanes
);

/*******************************************************************************
* mvHwsMacClearChannelCfg
*
* DESCRIPTION:
*       Configures MAC advanced feature  accordingly.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       macNum    - MAC number
*       macType   - MAC type according to port configuration
*       txIpg      - TX_IPG
*       txPreamble - TX Preamble
*       rxPreamble - RX Preamble
*       txCrc      - TX CRC
*       rxCrc      - RX CRC
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsMacClearChannelCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    MV_HWS_PORT_MAC_TYPE    macType,
    GT_U32                  txIpg,
    GT_U32                  txPreamble,
    GT_U32                  rxPreamble,
    MV_HWS_PORT_CRC_MODE    txCrc,
    MV_HWS_PORT_CRC_MODE    rxCrc
);

/*******************************************************************************
* hwsMacIfClose
*
* DESCRIPTION:
*       Free all resources allocated by supported MAC types.
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
void hwsMacIfClose(void);

/*******************************************************************************
* mvHwsMacFcStateCfg
*
* DESCRIPTION:
*       Configure Flow Control state.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       macNum    - MAC number
*       macType   - MAC type according to port configuration
*       fcState    - flow control state
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsMacFcStateCfg
(
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          macNum,
    MV_HWS_PORT_MAC_TYPE            macType,
    MV_HWS_PORT_FLOW_CONTROL_ENT    fcState
);

GT_STATUS hwsMacGetFuncPtr
(
	GT_U8                   devNum,
	MV_HWS_MAC_FUNC_PTRS    **hwsFuncsPtr
);

#ifdef __cplusplus
}
#endif


#endif /* __mvHwsMacIf_H */

