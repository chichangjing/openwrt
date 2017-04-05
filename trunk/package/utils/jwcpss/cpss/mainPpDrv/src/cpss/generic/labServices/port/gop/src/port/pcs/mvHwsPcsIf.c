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
* mvHwsPcsIf.c
*
* DESCRIPTION:
*       PCS level APIs
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
*
*******************************************************************************/

#include <common/siliconIf/mvSiliconIf.h>

#include <pcs/mvHwsPcsIf.h>

#ifdef __GNUC__
/* Test for GCC > 4.4 */
# if  (__GNUC__*100+__GNUC_MINOR__) >=404
#  define __NOOPT__ __attribute__((optimize("O0")))
# endif
#endif

#ifdef HWS_DEBUG
extern GT_FILEP fdHws;
#endif

static MV_HWS_PCS_FUNC_PTRS hwsPcsFuncsPtr[HWS_MAX_DEVICE_NUM][LAST_PCS];

/*******************************************************************************
* mvHwsPcsReset
*
* DESCRIPTION:
*       Set the selected PCS type and number to reset or exit from reset.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - physical PCS number
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
GT_STATUS mvHwsPcsReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_RESET            action
)
{
    if ((pcsType == PCS_NA) || (pcsType >= LAST_PCS))
    {
      return GT_BAD_PARAM;
    }

    if (hwsPcsFuncsPtr[devNum][pcsType].pcsResetFunc == NULL)
    {
        return GT_OK;
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
    osFprintf(fdHws, "\n;Run PCS Reset: pcsType %s, action %d.(0 - for reset)\n",
      hwsPcsFuncsPtr[devNum][pcsType].pcsTypeGetFunc(), action);
    osFclose(fdHws);
#endif

    CHECK_STATUS(hwsPcsFuncsPtr[devNum][pcsType].pcsResetFunc(devNum, portGroup, pcsNum, action));
    return GT_OK;
}

/*******************************************************************************
* mvHwsPcsModeCfg
*
* DESCRIPTION:
*       Set the internal mux's to the required PCS in the PI.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
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
GT_STATUS mvHwsPcsModeCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_PORT_FEC_MODE    fecCtrl,
    GT_U32                  numOfLanes
)
{
    if ((pcsType == PCS_NA) || (pcsType >= LAST_PCS) ||
        (hwsPcsFuncsPtr[devNum][pcsType].pcsModeCfgFunc == NULL))
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
    osFprintf(fdHws, "\n;Run PCS Mode Cfg: pcsType %s, number of lanes %d.\n",
      hwsPcsFuncsPtr[devNum][pcsType].pcsTypeGetFunc(), numOfLanes);
    osFclose(fdHws);
#endif
    CHECK_STATUS(hwsPcsFuncsPtr[devNum][pcsType].pcsModeCfgFunc(devNum, portGroup, pcsNum, fecCtrl,numOfLanes));

    return GT_OK;
}


/*******************************************************************************
* mvHwsPcsLoopbackCfg
*
* DESCRIPTION:
*       Set PCS loop back.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
*       lbType    - loop back type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPcsLoopbackCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_PORT_LB_TYPE     lbType
)
{
	MV_HWS_RESET pcsState = UNRESET;
	GT_U32       numOfLanes;
	GT_STATUS res;

    if ((pcsType == PCS_NA) || (pcsType >= LAST_PCS) ||
        (hwsPcsFuncsPtr[devNum][pcsType].pcsLbCfgFunc == NULL))
    {
      return GT_BAD_PARAM;
    }

	/* check if PCS us under reset or not */
	res = mvHwsPcsActiveStatusGet(devNum, portGroup, pcsNum, pcsType, &numOfLanes);
	if(res == GT_OK) /* check if this function is supported for this PCS */
	{
		pcsState = (numOfLanes == 0) ? RESET : UNRESET;
	}
	else if(res != GT_NOT_SUPPORTED)
	{
		return res;
	}

	/* rest PCS if it's not in reset*/
	if(pcsState == UNRESET)
	{
		CHECK_STATUS(mvHwsPcsReset (devNum,portGroup,pcsNum,pcsType,RESET));
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
    osFprintf(fdHws, "\n;Run PCS Loopback Cfg: pcsType %s, type %d.\n",
      hwsPcsFuncsPtr[devNum][pcsType].pcsTypeGetFunc(), lbType);
    osFclose(fdHws);
#endif

    CHECK_STATUS(hwsPcsFuncsPtr[devNum][pcsType].pcsLbCfgFunc(devNum, portGroup, pcsNum, lbType));

	if(pcsState == UNRESET)
	{
		CHECK_STATUS(mvHwsPcsReset (devNum,portGroup,pcsNum,pcsType,UNRESET));
	}

    return GT_OK;
}

/*******************************************************************************
* mvHwsPcsLoopbackCfgGet
*
* DESCRIPTION:
*       Return the PCS loop back mode state.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
*       lbType    - loop back type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPcsLoopbackCfgGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_PORT_LB_TYPE     *lbType
)
{
	if (hwsPcsFuncsPtr[devNum][pcsType].pcsLbCfgGetFunc == NULL)
	{
		return GT_NOT_SUPPORTED;
	}

	CHECK_STATUS(hwsPcsFuncsPtr[devNum][pcsType].pcsLbCfgGetFunc(devNum, portGroup, pcsNum, lbType));

	return GT_OK;
}

/*******************************************************************************
* mvHwsPcsTestGenCfg
*
* DESCRIPTION:
*       Set PCS internal test generator mechanisms.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
*       pattern   - pattern to generate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPcsTestGenCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_PORT_TEST_GEN_PATTERN  portPattern
)
{
    if ((pcsType == PCS_NA) || (pcsType >= LAST_PCS) ||
        (hwsPcsFuncsPtr[devNum][pcsType].pcsTestGenFunc == NULL))
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
    osFprintf(fdHws, "\n;Run PCS Test Generator Cfg: pcsType %s, pattern is %d.\n",
      hwsPcsFuncsPtr[devNum][pcsType].pcsTypeGetFunc(),portPattern);
    osFclose(fdHws);
#endif
    CHECK_STATUS(hwsPcsFuncsPtr[devNum][pcsType].pcsTestGenFunc(devNum, portGroup, pcsNum, portPattern));

    return GT_OK;
}

/*******************************************************************************
* mvHwsPcsTestGenStatus
*
* DESCRIPTION:
*       Get PCS internal test generator mechanisms error counters and status.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
*       pattern   - pattern to generate
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPcsTestGenStatus
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_PORT_TEST_GEN_PATTERN     pattern,
    MV_HWS_TEST_GEN_STATUS        *status
)
{
    if ((pcsType == PCS_NA) || (pcsType >= LAST_PCS) ||
        (hwsPcsFuncsPtr[devNum][pcsType].pcsTestGenFunc == NULL))
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
    osFprintf(fdHws, "\n;Run PCS Test Generator Status: pcsType %s, pattern is %d.\n",
      hwsPcsFuncsPtr[devNum][pcsType].pcsTypeGetFunc(),pattern);
    osFclose(fdHws);
#endif
    CHECK_STATUS(hwsPcsFuncsPtr[devNum][pcsType].pcsTestGenStatusFunc(devNum, portGroup, pcsNum, pattern, status));

    return GT_OK;
}

/*******************************************************************************
* mvHwsPcsRxReset
*
* DESCRIPTION:
*       Set the selected RX PCS type and number to reset or exit from reset.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
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
GT_STATUS mvHwsPcsRxReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    MV_HWS_RESET            action
)
{
    if ((pcsType == PCS_NA) || (pcsType >= LAST_PCS) ||
        (hwsPcsFuncsPtr[devNum][pcsType].pcsRxResetFunc == NULL))
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
    osFprintf(fdHws, "\n;Run RX PCS Reset: pcsType %s, action %d.(0 - for reset)\n",
      hwsPcsFuncsPtr[devNum][pcsType].pcsTypeGetFunc(), action);
    osFclose(fdHws);
#endif

    CHECK_STATUS(hwsPcsFuncsPtr[devNum][pcsType].pcsRxResetFunc(devNum, portGroup, pcsNum, action));
    return GT_OK;
}

/*******************************************************************************
* mvHwsPcsExtPllCfg
*
* DESCRIPTION:
*       Set the selected PCS type and number to external PLL mode.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPcsExtPllCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType
)
{
    if ((pcsType != MMPCS) || (hwsPcsFuncsPtr[devNum][pcsType].pcsExtPllCfgFunc == NULL))
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
    osFprintf(fdHws, "\nRun External PLL configuration: pcsType %s, pscNum %d\n",
      hwsPcsFuncsPtr[devNum][pcsType].pcsTypeGetFunc(), pcsNum);
    osFclose(fdHws);
#endif

    CHECK_STATUS(hwsPcsFuncsPtr[devNum][pcsType].pcsExtPllCfgFunc(devNum, portGroup, pcsNum));
    return GT_OK;
}

/*******************************************************************************
* mvHwsPcsSignalDetectMaskSet
*
* DESCRIPTION:
*       Set all related PCS with Signal Detect Mask value (1/0).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
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
GT_STATUS mvHwsPcsSignalDetectMaskSet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    GT_BOOL                 maskEn
)
{
    if (hwsPcsFuncsPtr[devNum][pcsType].pcsSignalDetectMaskEn == NULL)
    {
      return GT_OK;
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
    osFprintf(fdHws, "\nRun Signal Detect Mask configuration: pcsType %s, pscNum %d\n",
      hwsPcsFuncsPtr[devNum][pcsType].pcsTypeGetFunc(), pcsNum);
    osFclose(fdHws);
#endif

    CHECK_STATUS(hwsPcsFuncsPtr[devNum][pcsType].pcsSignalDetectMaskEn(devNum, portGroup, pcsNum, maskEn));
    return GT_OK;
}

/*******************************************************************************
* mvHwsPcsFecConfig
*
* DESCRIPTION:
*       Configure FEC disable/enable on PCS.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
*       fecEn     - if true, enable FEC
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPcsFecConfig
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    GT_BOOL                 fecEn
)
{
    if (hwsPcsFuncsPtr[devNum][pcsType].pcsFecCfgFunc == NULL)
    {
      return GT_NOT_SUPPORTED;
    }

    CHECK_STATUS(hwsPcsFuncsPtr[devNum][pcsType].pcsFecCfgFunc(devNum, portGroup, pcsNum, fecEn));
    return GT_OK;
}

/*******************************************************************************
* mvHwsPcsFecConfigGet
*
* DESCRIPTION:
*       Return the FEC state on PCS.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
*       fecEn     - if true, enable FEC
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPcsFecConfigGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    GT_BOOL                 *fecEn
)
{
    if (hwsPcsFuncsPtr[devNum][pcsType].pcsFecCfgGetFunc == NULL)
    {
      return GT_NOT_SUPPORTED;
    }

    CHECK_STATUS(hwsPcsFuncsPtr[devNum][pcsType].pcsFecCfgGetFunc(devNum, portGroup, pcsNum, fecEn));
    return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsPcsActiveStatusGet
*
* DESCRIPTION:
*       Return number of PCS active lanes or 0, if current PCS isn't active.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       pcsNum    - physical PCS number
*       pcsType   - PCS type
*
* OUTPUTS:
*       numOfLanes - number of lanes agregated in PCS
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPcsActiveStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    GT_U32                  *numOfLanes
)
{
    if (hwsPcsFuncsPtr[devNum][pcsType].pcsActiveStatusGetFunc == NULL)
    {
      return GT_NOT_SUPPORTED;
    }

    CHECK_STATUS(hwsPcsFuncsPtr[devNum][pcsType].pcsActiveStatusGetFunc(devNum, portGroup, pcsNum, numOfLanes));
    return GT_OK;
}
#endif

/*******************************************************************************
* mvHwsPcsCheckGearBox
*
* DESCRIPTION:
*       Check Gear Box Status on related lanes (currently used only on MMPCS).
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
GT_STATUS mvHwsPcsCheckGearBox
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  pcsNum,
    MV_HWS_PORT_PCS_TYPE    pcsType,
    GT_U32                  numOfLanes,
    GT_BOOL                 *laneLock
)
{
    if (hwsPcsFuncsPtr[devNum][pcsType].pcsCheckGearBoxFunc == NULL)
    {
      return GT_NOT_SUPPORTED;
    }

    CHECK_STATUS(hwsPcsFuncsPtr[devNum][pcsType].pcsCheckGearBoxFunc(devNum, portGroup, pcsNum, numOfLanes, laneLock));
    return GT_OK;
}

/*******************************************************************************
* hwsPcsGetFuncPtr
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
GT_STATUS hwsPcsGetFuncPtr
(
	GT_U8                 devNum,
	MV_HWS_PCS_FUNC_PTRS  **hwsFuncsPtr
)
{
	if (devNum >= HWS_MAX_DEVICE_NUM)
	{
		return GT_NOT_SUPPORTED;
	}

    *hwsFuncsPtr = &hwsPcsFuncsPtr[devNum][0];

	return GT_OK;
}
