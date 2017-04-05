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
* mvHwsPortModeElements.c
*
* DESCRIPTION:
*
* DEPENDENCIES:
*
*******************************************************************************/

#include <private/mvPortModeElements.h>
#include <port/mvHwsPortCfgIf.h>

/* array of all ports init parameters for different silicon types */
static MV_HWS_PORT_INIT_PARAMS **hwsDevPortsParams[LAST_SIL_TYPE] = { 0 };

/*******************************************************************************
* hwsInitPortsModesParam
*
* DESCRIPTION:
*       Registers silicon ports modes parameters in the system.
*       Calls ones per active silicon type.
*
* INPUTS:
*       devType   - silicon type.
*       portModes - pointer to array of ports modes parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_U32 hwsInitPortsModesParam
(
    MV_HWS_DEV_TYPE         devType,
    MV_HWS_PORT_INIT_PARAMS **portModes
)
{
    if (devType >= LAST_SIL_TYPE)
        return GT_BAD_PARAM;

    hwsDevPortsParams[devType] = portModes;
    return GT_OK;
}

/*******************************************************************************
* mvHwsBuildActiveLaneList
*
* DESCRIPTION:
*       Get SD vector.
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
GT_STATUS mvHwsBuildActiveLaneList
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  portNum,
    MV_HWS_PORT_STANDARD    portMode
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 i;
    GT_U32 laneCtnt = 0;
    GT_U32 *activeLanes;

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, portNum, portMode);
    if (curPortParams == NULL || curPortParams->numOfActLanes == 0)
    {
      return GT_NOT_SUPPORTED;
    }
    activeLanes = (GT_U32 *) hwsOsMallocFuncPtr(curPortParams->numOfActLanes * sizeof(GT_U32));
    if (activeLanes == NULL)
    {
      return GT_NO_RESOURCE;
    }

    for (i = curPortParams->firstLaneNum;
         i < (curPortParams->firstLaneNum + curPortParams->numOfActLanes); i++)
    {
        activeLanes[laneCtnt++] = i;
    }
    curPortParams->activeLanesList = activeLanes;

    return GT_OK;
}

/*******************************************************************************
* mvHwsRebuildActiveLaneList
*
* DESCRIPTION:
*       Get SD vector and rebuild active lane array. Can be called during CreatePort
*       only.
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
GT_STATUS mvHwsRebuildActiveLaneList
(
    GT_U8                 devNum,
    GT_U32                portGroup,
    GT_U32                portNum,
    MV_HWS_PORT_STANDARD  portMode,
    GT_U32                *laneList
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 sdVec, tmp, startSerdes;
    GT_U32 laneCtnt;

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, portNum, portMode);

    if ((NULL == curPortParams) || ((curPortParams->numOfActLanes == 0)
                                    && (curPortParams->portMacNumber == NA_NUM)))
    {
        return GT_NOT_SUPPORTED;
    }

    if (curPortParams->numOfActLanes != 0)
    {
        CHECK_STATUS(mvHwsRedundancyVectorGet(devNum, portGroup, &sdVec));

        startSerdes = curPortParams->activeLanesList[0];
        tmp = (sdVec >> startSerdes) & 0xF;
        for (laneCtnt = 0; (laneCtnt < curPortParams->numOfActLanes); laneCtnt++)
        {
            laneList[laneCtnt] = curPortParams->activeLanesList[laneCtnt];
            if (tmp & 1)
            {
                laneList[laneCtnt]++;
            }
            tmp = tmp >> 1;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsDestroyActiveLaneList
*
* DESCRIPTION:
*       Free lane list allocated memory.
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
GT_STATUS mvHwsDestroyActiveLaneList
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  portNum,
    MV_HWS_PORT_STANDARD    portMode
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, portNum, portMode);
    if ((curPortParams != NULL) && (curPortParams->activeLanesList != NULL))
    {
      hwsOsFreeFuncPtr(curPortParams->activeLanesList);
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsBuildDeviceLaneList
*
* DESCRIPTION:
*       Build the device lane list per port and per mort mode.
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
GT_STATUS mvHwsBuildDeviceLaneList
(
   GT_U8    devNum,
   GT_U32   portGroup
)
{
    GT_U32    portNum;
#ifndef MV_HWS_REDUCED_BUILD
    GT_U32    portMode;
    GT_STATUS res;

    /* init active lines in ports DB */
    for (portNum = 0; portNum < HWS_CORE_PORTS_NUM(devNum); portNum++)
    {
        for (portMode = 0; portMode < HWS_DEV_PORT_MODES(devNum); portMode++)
        {
            res = mvHwsBuildActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)portMode);
            if((res != GT_OK) && (res != GT_NOT_SUPPORTED))
            {
                return res;
            }
        }
    }
#else
    /* Internal CPU receives the port parameters from the Host so no need to hold table per port mode */
    for (portNum = 0; portNum < HWS_CORE_PORTS_NUM(devNum); portNum++)
    {
        CHECK_STATUS(mvHwsBuildActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)0 /*portMode*/));
    }
#endif

    return GT_OK;
}

/*******************************************************************************
* mvHwsDestroyDeviceLaneList
*
* DESCRIPTION:
*       Free the device lane list allocated memory.
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
GT_STATUS mvHwsDestroyDeviceLaneList
(
   GT_U8    devNum,
   GT_U32   portGroup
)
{
    GT_U32  portNum;
    GT_U32  portMode;

    /* free active lines allocated memory in ports DB */
    for (portNum = 0; portNum < HWS_CORE_PORTS_NUM(devNum); portNum++)
    {
        for (portMode = 0; portMode < HWS_DEV_PORT_MODES(devNum); portMode++)
        {
            CHECK_STATUS(mvHwsDestroyActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)portMode));
        }
    }

    return GT_OK;
}

MV_HWS_PORT_INIT_PARAMS *hwsPortsParamsArrayGet
(
   GT_U8    devNum,
   GT_U32   portGroup,
   GT_U32   portNum
)
{
    GT_BOOL extMode;
    MV_HWS_PORT_INIT_PARAMS **tmp = NULL;
    MV_HWS_DEV_TYPE  devType = HWS_DEV_SILICON_TYPE(devNum);

    if (devType >= LAST_SIL_TYPE)
        return NULL;

    /* In Alleycat3: check if Ports #25 and #27 are in Extended mode by checking the
       EXTERNAL_CONTROL register value. If the port is in Extended mode, the related
       PortsParamsArray for Extended port will be #28/#29 */
    if (devType == Alleycat3A0)
    {
        if (mvHwsPortExtendedModeCfgGet(devNum, portGroup, portNum, _10GBase_KR, &extMode) != GT_OK)
        {
            return NULL;
        }

        if ((extMode == GT_TRUE) && (portNum == 25))
        {
            portNum = 28;
        }
        else if ((extMode == GT_TRUE) && (portNum == 27))
        {
            portNum = 29;
        }
    }
    /* In Lion2A0, Lion2B0,HooperA0: check if Ports #9 and #11 are in Extended mode by checking the
       EXTERNAL_CONTROL register value. If the port is in Extended mode, the related
       PortsParamsArray for Extended port will be #12/#13 */
    else if (HWS_DEV_GOP_REV(devNum) == GOP_40NM_REV1)
    {
        if (mvHwsPortExtendedModeCfgGet(devNum, portGroup, portNum, _10GBase_KR /* dummy */, &extMode) != GT_OK)
        {
            return NULL;
        }

        if ((extMode == GT_TRUE) && (portNum == 9))
        {
            portNum = 12; /* port 12 entry contains Port 9 externded mode parameters */
        }
        else if ((extMode == GT_TRUE) && (portNum == 11))
        {
            portNum = 13; /* port 13 entry contains Port 11 externded mode parameters */
        }
    }

    tmp = (MV_HWS_PORT_INIT_PARAMS**)hwsDevPortsParams[devType];

    if (tmp == NULL)
    {
        return NULL;
    }

    return tmp[portNum];
}

GT_U32 hwsPortModeParamsIndexGet
(
   GT_U8                    devNum,
   GT_U32                   portGroup,
   GT_U32                   portNum,
   MV_HWS_PORT_STANDARD     portMode
)
{
#ifndef MV_HWS_REDUCED_BUILD
    MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;

    /* get func DB */
    hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

    /* From BobK and on, new mapping was introduced (to reduce memory usage) so
       port modes are mapped whether thy are supported or not */
    if (hwsDevFunc[hwsDeviceSpecInfo[devNum].devType].portParamsIndexGetFunc != 0)
    {
        return hwsDevFunc[hwsDeviceSpecInfo[devNum].devType].portParamsIndexGetFunc(devNum, portGroup, portNum, portMode);
    }
    else
    {
        /* In devices before BobK, all ports are mapped */
        return portMode;
    }
#else
    /* for reduced driver, there is only one row
       of port parameters*/
    return 0;
#endif
}

MV_HWS_PORT_INIT_PARAMS *hwsPortModeParamsGet
(
   GT_U8                    devNum,
   GT_U32                   portGroup,
   GT_U32                   portNum,
   MV_HWS_PORT_STANDARD     portMode
)
{
    GT_U32 portModeIndex;

    MV_HWS_PORT_INIT_PARAMS *portParams = hwsPortsParamsArrayGet(devNum, portGroup, portNum);
    if (portParams == NULL)
    {
        return NULL;
    }

    portModeIndex = hwsPortModeParamsIndexGet(devNum, portGroup, portNum, portMode);
    if (portModeIndex == NA_NUM)
    {
        return NULL;
    }

    return &portParams[portModeIndex];
}

GT_STATUS hwsPortModeParamsSet
(
   GT_U8                    devNum,
   GT_U32                   portGroup,
   GT_U32                   portNum,
   MV_HWS_PORT_INIT_PARAMS  *portParams
)
{
    MV_HWS_PORT_INIT_PARAMS *portModesCurr;
    GT_U32                  *activeLanesList;

    if(portParams->portStandard >= (MV_HWS_PORT_STANDARD)HWS_DEV_PORT_MODES(devNum))
    {
        return GT_BAD_PARAM;
    }

    portModesCurr = hwsPortModeParamsGet(devNum, portGroup, portNum, portParams->portStandard);
    if(portModesCurr == NULL)
    {
        return GT_NOT_INITIALIZED;
    }

    /* save current active lane list ptr */
    activeLanesList = portModesCurr->activeLanesList;

    /* copy new port parameters */
    hwsOsMemCopyFuncPtr(portModesCurr, portParams, sizeof(MV_HWS_PORT_INIT_PARAMS));

    /* set back pointer to active lane list */
    portModesCurr->activeLanesList = activeLanesList;

    if(activeLanesList == NULL)
    {
        /* skip copy of lane list */
        return GT_OK;
    }

    /* copy list of active lanes */
    hwsOsMemCopyFuncPtr(activeLanesList, portParams->activeLanesList, sizeof(GT_U32) * portParams->numOfActLanes);

    return GT_OK;
}

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
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 i;
    GT_U32 *activeLanes;

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, portNum, portMode);
    curPortParams->portMacNumber = macNum;
    curPortParams->portPcsNumber = pcsNum;

    if (curPortParams->numOfActLanes != sdVecSize)
    {
      if (curPortParams->activeLanesList != 0)
      {
        hwsOsFreeFuncPtr(curPortParams->activeLanesList);
      }
      activeLanes = (GT_U32 *) hwsOsMallocFuncPtr(sdVecSize * sizeof(GT_U32));
      if (activeLanes == NULL)
      {
        return GT_NO_RESOURCE;
      }
    }
    else
    {
      activeLanes = curPortParams->activeLanesList;
    }

    for (i = 0; i < sdVecSize; i++)
    {
        activeLanes[i] = sdVector[i];
    }
    curPortParams->numOfActLanes = sdVecSize;
    curPortParams->activeLanesList = activeLanes;

    return GT_OK;
}

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
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, portNum, portMode);

    if (NULL == curPortParams)
    {
        return GT_BAD_PARAM;
    }

    *macNum = curPortParams->portMacNumber;
    *pcsNum = curPortParams->portPcsNumber;
    *sdVecSize = (GT_UOPT)curPortParams->numOfActLanes;

    if (curPortParams->numOfActLanes != 0)
    {
        *sdVector = (GT_UOPT*)curPortParams->activeLanesList;
    }
    else
    {
        *sdVector = NULL;
    }

    return GT_OK;
}

