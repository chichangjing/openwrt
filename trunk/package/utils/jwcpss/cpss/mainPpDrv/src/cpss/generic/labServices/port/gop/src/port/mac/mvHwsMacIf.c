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
* mvHwsMacIf.c
*
* DESCRIPTION:
*       MAC specific HW Services API
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <mac/mvHwsMacIf.h>

#ifdef HWS_DEBUG
extern GT_FILEP fdHws;
#endif

static MV_HWS_MAC_FUNC_PTRS hwsMacFuncsPtr[HWS_MAX_DEVICE_NUM][LAST_MAC];

extern GT_STATUS mvHwsXlgMacModeSelectDefaultCfg
(
    GT_U8                 devNum,
    GT_U32                portGroup,
    GT_U32                macNum
);

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
)
{
    if ((macType == MAC_NA) || (macType >= LAST_MAC) ||
        (hwsMacFuncsPtr[devNum][macType].macRestFunc == NULL))
    {
      return GT_BAD_PARAM;
    }
#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Mac Reset: macType %s, action %d.(0 - for reset)\n",
      hwsMacFuncsPtr[devNum][macType].macTypeGetFunc(), action);
    osFclose(fdHws);
#endif

    return hwsMacFuncsPtr[devNum][macType].macRestFunc(devNum, portGroup, macNum, action);
}

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
)
{
    if ((macType == MAC_NA) || (macType >= LAST_MAC) ||
        (hwsMacFuncsPtr[devNum][macType].macModeCfgFunc == NULL))
    {
      return GT_BAD_PARAM;
    }
#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Mac Mode Cfg: macType %s, number of lanes %d.\n",
      hwsMacFuncsPtr[devNum][macType].macTypeGetFunc(), numOfLanes);
    osFclose(fdHws);
#endif

    return hwsMacFuncsPtr[devNum][macType].macModeCfgFunc(devNum, portGroup, macNum, numOfLanes);
}


/*******************************************************************************
* mvHwsMacLinkStatusGet
*
* DESCRIPTION:
*       Get MAC link status.
*
* INPUTS:
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
)
{
    if ((macType == MAC_NA) || (macType >= LAST_MAC) ||
        (hwsMacFuncsPtr[devNum][macType].macLinkGetFunc == NULL))
    {
      return GT_BAD_PARAM;
    }

    return hwsMacFuncsPtr[devNum][macType].macLinkGetFunc(devNum, portGroup, macNum, linkStatus);
}

#ifndef CO_CPU_RUN
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
)
{
    if ((macType == MAC_NA) || (macType >= LAST_MAC) ||
        (hwsMacFuncsPtr[devNum][macType].macLbCfgFunc == NULL))
    {
      return GT_BAD_PARAM;
    }

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run Mac Loopback Cfg: macType %s, type %d.\n",
      hwsMacFuncsPtr[devNum][macType].macTypeGetFunc(), lbType);
    osFclose(fdHws);
#endif

    hwsMacFuncsPtr[devNum][macType].macLbCfgFunc(devNum, portGroup, macNum, lbType);

    return GT_OK;
}

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
)
{
    if ((macType == MAC_NA) || (macType >= LAST_MAC) ||
        (hwsMacFuncsPtr[devNum][macType].macLbStatusGetFunc == NULL))
    {
      return GT_BAD_PARAM;
    }

    return hwsMacFuncsPtr[devNum][macType].macLbStatusGetFunc(devNum, portGroup, macNum, lbType);
}
#endif

#ifndef CO_CPU_RUN
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
)
{
    macType = macType;
    return mvHwsXlgMacModeSelectDefaultCfg(devNum,portGroup,macNum);
}

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
)
{
    if ((macType == MAC_NA) || (macType >= LAST_MAC) ||
        (hwsMacFuncsPtr[devNum][macType].macActiveStatusFunc == NULL))
    {
      return GT_BAD_PARAM;
    }

    return hwsMacFuncsPtr[devNum][macType].macActiveStatusFunc(devNum, portGroup, macNum, numOfLanes);
}

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
)
{
    GT_STATUS rc;

    if ((macType == MAC_NA) || (macType >= LAST_MAC) ||
        (hwsMacFuncsPtr[devNum][macType].macClearChannelCfgFunc == NULL))
    {
      return GT_BAD_PARAM;
    }

    rc = hwsMacFuncsPtr[devNum][macType].macClearChannelCfgFunc(devNum, portGroup, macNum, txIpg, txPreamble, rxPreamble, txCrc, rxCrc);
    return rc;
}
#endif

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
*       fcState   - flow control state
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
)
{
    if ((macType == MAC_NA) || (macType >= LAST_MAC) ||
        (hwsMacFuncsPtr[devNum][macType].macFcStateCfgFunc == NULL))
    {
      return GT_BAD_PARAM;
    }

    return hwsMacFuncsPtr[devNum][macType].macFcStateCfgFunc(devNum, portGroup,
                                                                macNum, fcState);
}

/*******************************************************************************
* hwsMacGetFuncPtr
*
* DESCRIPTION:
*       Get function structure pointer.
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
GT_STATUS hwsMacGetFuncPtr
(
   GT_U8                     devNum,
   MV_HWS_MAC_FUNC_PTRS    **hwsFuncsPtr
)
{
   if (devNum >= HWS_MAX_DEVICE_NUM)
   {
       return GT_NOT_SUPPORTED;
   }

   *hwsFuncsPtr = &hwsMacFuncsPtr[devNum][0];

   return GT_OK;
}

