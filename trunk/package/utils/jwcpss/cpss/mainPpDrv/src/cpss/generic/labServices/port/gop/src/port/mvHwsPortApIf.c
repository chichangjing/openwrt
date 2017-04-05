/*******************************************************************************
*                Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsPortApIf.c
*
* DESCRIPTION: AP Port external interface
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 42 $
******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>

#include <mvHwsPortApInitIf.h>
#include <mvHwsPortCtrlApInitIf.h>
#include <private/mvHwsPortApInitIfPrv.h>
#include <private/mvHwsPortPrvIf.h>
#include <private/mvHwsPortApCoCpuIf.h>
#include <private/mvPortModeElements.h>
#include <serdes/mvHwsSerdesPrvIf.h>

static MV_HWS_GLOBALS *apGlobalsCfgPtr = NULL;
static MV_HWS_GLOBALS apGlobalsCfgSnap;
static GT_BOOL apEngineInitOnDev[HWS_MAX_DEVICE_NUM][32]; /* check with CPSS real num of devs */


#define LINK_DOWN_TIMEOUT 60

/*******************************************************************************
* mvHwsApEngineInit
*
* DESCRIPTION:
*       Initialize AP engine. Initialize internal data structure on HwSrv level
*       and Z80 level, load firmware to Z80 and start it.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApEngineInit
(
    GT_U8                    devNum,
    GT_U32                   portGroup
)
{
  static GT_BOOL globalInitDone = GT_FALSE;

  if (devNum >= HWS_MAX_DEVICE_NUM)
  {
    return GT_BAD_PARAM;
  }

  if (!globalInitDone)
  {
    hwsOsMemSetFuncPtr(apEngineInitOnDev, 0, sizeof(apEngineInitOnDev));
    globalInitDone = GT_TRUE;
  }

  if (!apEngineInitOnDev[devNum][portGroup])
  {
    /* copy firmware to Co CPU memory and start Co CPU */
    mvApFwRun(devNum, portGroup);
    apEngineInitOnDev[devNum][portGroup] = GT_TRUE;
  }

  apGlobalsCfgPtr = (MV_HWS_GLOBALS*)mvApGetSharedMemPtr(devNum, portGroup);

  return GT_OK;
}

/*******************************************************************************
* mvApGlobalInfoGet
*
* DESCRIPTION:
*       Read all global info structure from shared memory.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*
* OUTPUTS:
*       None
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApGlobalInfoGet
(
 GT_U8   devNum,
 GT_U32  portGroup
 )
{
  GT_U32 *tmpPtr;
  GT_U32 i, wordNum;

  if (devNum >= HWS_MAX_DEVICE_NUM)
  {
    return GT_BAD_PARAM;
  }

  if (apGlobalsCfgPtr == NULL || apEngineInitOnDev[devNum][portGroup] == GT_FALSE)
  {
    return GT_NOT_INITIALIZED;
  }
  tmpPtr = (GT_U32 *)&apGlobalsCfgSnap;
  wordNum = sizeof(apGlobalsCfgSnap)/sizeof(GT_U32);
  for (i = 0; i < wordNum; i++)
  {
    /* read data from shared memory */
    mvApGetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)apGlobalsCfgPtr + i*4, tmpPtr++);
  }

  return GT_OK;
}

/*******************************************************************************
* mvHwsApEngineStop
*
* DESCRIPTION:
*       Disable whole AP engine.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApEngineStop
(
    GT_U8                    devNum,
    GT_U32                   portGroup
)
{
  if (devNum >= HWS_MAX_DEVICE_NUM)
  {
    return GT_BAD_PARAM;
  }

    mvApFwStop(devNum, portGroup);

    apEngineInitOnDev[devNum][portGroup] = GT_FALSE;

    return GT_OK;
}

/*******************************************************************************
* mvHwsApEngineInitGet
*
* DESCRIPTION:
*       Checks if AP engine is enabled whole AP engine.
*
* INPUTS:
*       devNum        - system device number
*       portGroup     - port group (core) number
*       engineEnabled - pointer to boolean that indicated whether the engine
*                       is enabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApEngineInitGet
(
    GT_U8                    devNum,
    GT_U32                   portGroup,
    GT_BOOL                  *engineEnabled
)
{
    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    *engineEnabled = apEngineInitOnDev[devNum][portGroup];

    return GT_OK;
}

/*******************************************************************************
* mvHwsApPortStart
*
* DESCRIPTION:
*       Init AP port capability.
*       Runs AP protocol(802.3ap Serdes AutoNeg) and configures the best port
*       mode and all it's elements accordingly.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       apCfg       - AP configuration parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortStart
(
    GT_U8                    devNum,
    GT_U32                   portGroup,
    GT_U32                   phyPortNum,
    MV_HWS_AP_CFG            *apCfg
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    MV_HWS_PORT_STANDARD    portMode;
    GT_U32                  apPortCntrl;
    GT_U32                  apLaneNum;
    GT_BOOL                 apPortEn;
    MV_HWS_AP_CFG           apCfgTemp;
	MV_HWS_AP_PORT_STATUS   apResult;
	GT_BOOL                 linkStatus;
	GT_BOOL                 linkTimeout;
	GT_STATUS				res;

    if (apGlobalsCfgPtr == NULL)
    {
        return GT_NOT_INITIALIZED;
    }

  if (devNum >= HWS_MAX_DEVICE_NUM)
  {
    return GT_BAD_PARAM;
  }

    if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) || (apCfg == NULL))
    {
        return GT_BAD_PARAM;
    }

	/* if port's AP is already enabled then first stop it */
	if (apEngineInitOnDev[devNum][portGroup] == GT_FALSE)
    {
		return GT_NOT_INITIALIZED;
	}

	if (apCfg->apLaneNum != 0 && ((apCfg->modesVector >> 1) & 1) == 0 && ((apCfg->modesVector >> 3) & 1) == 0)
	{
		return GT_BAD_PARAM;
	}
	
	/* read port control to retrieve if it's enabled */
    res = mvHwsApPortConfigGet(devNum, portGroup, phyPortNum, &apPortEn, &apCfgTemp);
	if (res != GT_OK)
	{
		return res;
	}

	/* read port status to retrieve port mode */
	CHECK_STATUS(mvHwsApPortStatusGet(devNum, portGroup, phyPortNum, &apResult));
	
	/* if port's AP is already enabled then first stop it and 
	   force down the link (if it has a link) */
    if (apPortEn == GT_TRUE && apResult.hcdResult.hcdLinkStatus == GT_TRUE)
    {
        mvHwsApPortStop(devNum, portGroup, phyPortNum, PORT_POWER_DOWN);
        
		linkStatus = GT_TRUE;
		linkTimeout = 0;
		
		while (linkStatus == GT_TRUE && linkTimeout < LINK_DOWN_TIMEOUT)
		{
			hwsOsTimerWkFuncPtr(50);

			CHECK_STATUS(mvHwsPortLinkStatusGet(devNum, portGroup, phyPortNum, apResult.postApPortMode, &linkStatus));
			linkTimeout++;
		}

		if (linkTimeout >= LINK_DOWN_TIMEOUT)
		{
			return GT_TIMEOUT;
		}
    }

	/* set new configuration */
    apPortCntrl = 0;

	/* if 40G advertisement is enabled, we use 40G lane num.
	   this is because, in Lion2/Hooper, port 9 & 11 are extended ports and
	   40G mode uses different Serdes lane than 10G mode.
	   Note: if 10G is used as extended mode, hwsPortExtendedModeCfg should be called for both
	   10G SR_LR and 10G KR*/
	portMode = ((apCfg->modesVector >> 3) & 1) ? _40GBase_SR_LR : _10GBase_SR_LR;

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

    /* turn relative lane num to absolute lane num */
    apLaneNum = apCfg->apLaneNum + curPortParams->firstLaneNum;

    AP_CONTROL_LANE_NUM_SET(apPortCntrl, apLaneNum);
    AP_CONTROL_PCS_NUM_SET(apPortCntrl, (curPortParams->portPcsNumber + apCfg->apLaneNum)); /* used for AP machine */
	AP_CONTROL_PORT_PCS_NUM_SET(apPortCntrl, curPortParams->portPcsNumber); /* in case of a lane swap, this PCS is used for port
																						init/delete and link check */
    AP_CONTROL_MAC_NUM_SET(apPortCntrl, curPortParams->portMacNumber);

    AP_CONTROL_LB_EN_SET(apPortCntrl, apCfg->nonceDis);
    AP_CONTROL_FEC_ABIL_SET(apPortCntrl, apCfg->fecSup);
    AP_CONTROL_FEC_REQ_SET(apPortCntrl, apCfg->fecReq);
    AP_CONTROL_FC_ENABLE_SET(apPortCntrl, apCfg->fcPause);
    AP_CONTROL_FC_ASM_SET(apPortCntrl, apCfg->fcAsmDir);

    AP_CONTROL_ALL_ADV_MODE_SET(apPortCntrl, 0);

    /* set an appropriate advertise bits */
    AP_CONTROL_ADV_1000Base_KX_SET(apPortCntrl, (apCfg->modesVector & 1));
    AP_CONTROL_ADV_10GBase_KX4_SET(apPortCntrl, ((apCfg->modesVector >> 1) & 1));
    AP_CONTROL_ADV_10GBase_KR_SET(apPortCntrl,  ((apCfg->modesVector >> 2) & 1));
    AP_CONTROL_ADV_40GBase_KR4_SET(apPortCntrl, ((apCfg->modesVector >> 3) & 1));

	AP_CONTROL_AP_ENABLE_SET(apPortCntrl, 1);

    /* write data to shared memory */
    mvApSetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->apPortControl[phyPortNum]), apPortCntrl);

    return GT_OK;
}

/*******************************************************************************
* mvHwsApPortStop
*
* DESCRIPTION:
*       Disable the AP engine on port and release all its resources.
*       Clears the port mode and release all its resources according to selected.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       action     - Power down or reset
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortStop
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_ACTION      action

)
{
	GT_U32 apPortCntrl;
    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }

    if (apGlobalsCfgPtr == NULL || apEngineInitOnDev[devNum][portGroup] == GT_FALSE)
    {
        return GT_NOT_INITIALIZED;
    }
    if (phyPortNum >= HWS_CORE_PORTS_NUM(devNum))
    {
        return GT_BAD_PARAM;
    }

    action = action;

	/* read port control from AP */
    mvApGetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->apPortControl[phyPortNum]), &apPortCntrl);

	AP_CONTROL_AP_ENABLE_SET(apPortCntrl, 0); 

    /* write data to shared memory */
    mvApSetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->apPortControl[phyPortNum]), apPortCntrl);
    return GT_OK;
}

/*******************************************************************************
* mvHwsApPortConfigGet
*
* DESCRIPTION:
*       Returns the AP port configuration.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*
* OUTPUTS:
*       apPortEn    - whether AP is enabled on this port
*       apCfg       - if not NULL, AP configuration parameters
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortConfigGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    GT_BOOL                 *apPortEnabled,
    MV_HWS_AP_CFG           *apCfg

)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 apPortCntrl;
    GT_U32 apLaneNum;

    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
        return GT_BAD_PARAM;
    }
    if (apGlobalsCfgPtr == NULL || apEngineInitOnDev[devNum][portGroup] == GT_FALSE)
    {
        return GT_NOT_INITIALIZED;
    }

    if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) || (apPortEnabled == NULL))
    {
        return GT_BAD_PARAM;
    }

    /* read port control from AP */
    mvApGetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->apPortControl[phyPortNum]), &apPortCntrl);

    *apPortEnabled = AP_CONTROL_AP_ENABLE_GET(apPortCntrl);

	if (apCfg == NULL)
    {
        return GT_OK;
    }

	hwsOsMemSetFuncPtr(apCfg, 0, sizeof(MV_HWS_AP_CFG));

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, _10GBase_SR_LR);

    apLaneNum = AP_CONTROL_LANE_NUM_GET(apPortCntrl);

    /* turn absolute lane num to relative lane num */
    apCfg->apLaneNum = apLaneNum - curPortParams->firstLaneNum;
    apCfg->nonceDis = AP_CONTROL_LB_EN_GET(apPortCntrl);
    apCfg->fecSup = AP_CONTROL_FEC_ABIL_GET(apPortCntrl);
    apCfg->fecReq = AP_CONTROL_FEC_REQ_GET(apPortCntrl);
    apCfg->fcPause = AP_CONTROL_FC_ENABLE_GET(apPortCntrl);
    apCfg->fcAsmDir = AP_CONTROL_FC_ASM_GET(apPortCntrl);

    /* read the appropriate advertise bits */
    apCfg->modesVector |= (AP_CONTROL_ADV_1000Base_KX_GET(apPortCntrl) * _1000Base_KX_Bit0);
    apCfg->modesVector |= (AP_CONTROL_ADV_10GBase_KX4_GET(apPortCntrl) * _10GBase_KX4_Bit1);
    apCfg->modesVector |= (AP_CONTROL_ADV_10GBase_KR_GET(apPortCntrl) * _10GBase_KR_Bit2);
    apCfg->modesVector |= (AP_CONTROL_ADV_40GBase_KR4_GET(apPortCntrl) * _40GBase_KR4_Bit3);

    apCfg->refClockCfg.refClockFreq = MHz_156;
    apCfg->refClockCfg.refClockSource = PRIMARY_LINE_SRC;

    return GT_OK;
}

/*******************************************************************************
* mvHwsApPortSetActiveLanes
*
* DESCRIPTION:
*       Sets a bitmask of the active lanes of a port.
*
* INPUTS:
*       devNum       - system device number
*       portGroup    - port group (core) number
*       phyPortNum   - physical port number
*       lanesBitMask - bit mask of active lanes
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortSetActiveLanes
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
    GT_U32 activeLanesMask = 0;
    GT_U32 i;

  if (devNum >= HWS_MAX_DEVICE_NUM)
  {
    return GT_BAD_PARAM;
  }
    if (apGlobalsCfgPtr == NULL || apEngineInitOnDev[devNum][portGroup] == GT_FALSE)
    {
        return GT_NOT_INITIALIZED;
    }
    if (phyPortNum >= HWS_CORE_PORTS_NUM(devNum))
    {
        return GT_BAD_PARAM;
    }

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

    /* rebuild active lanes list according to current configuration (redundancy) */
    CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

    for (i = 0; i < curPortParams->numOfActLanes; i++)
    {
        U32_SET_FIELD(activeLanesMask, curLanesList[i], 1, 1);
    }

    /* write data to shared memory */
    mvApSetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->apPortActiveLanes[phyPortNum]), activeLanesMask);
    return GT_OK;
}

/*******************************************************************************
* mvHwsApPortResolutionMaskGet
*
* DESCRIPTION:
*       Returns the port's resolution bit mask
*
* INPUTS:
*       devNum       - system device number
*       portGroup    - port group (core) number
*
* OUTPUTS:
*       portBitmask  - port's resolution bit mask
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortResolutionMaskGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  *portBitmask
)
{
    GT_U32 portResolutionMask;

    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
       return GT_BAD_PARAM;
    }
    if (apGlobalsCfgPtr == NULL || apEngineInitOnDev[devNum][portGroup] == GT_FALSE)
    {
        return GT_NOT_INITIALIZED;
    }

    /* read the ports resolution mask*/
    mvApGetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->portResolutionMask), &portResolutionMask);
    if (portResolutionMask == 0)
    {
        *portBitmask = 0;
        return GT_OK;
    }

    /* signal AP to clear these ports (as they are being handles) */
    mvApSetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->portInitMask), portResolutionMask);

    /* save current resolution results */
    *portBitmask = portResolutionMask;

    /* what for AP to reset it */
    while ((portResolutionMask & *portBitmask) != 0)
    {
        mvApGetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->portResolutionMask), &portResolutionMask);
    }

    /* clear init ports */
    mvApSetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->portInitMask), 0);

    return GT_OK;
}

/*******************************************************************************
* mvApLockGet
*
* DESCRIPTION:
*       Acquires lock so host and AP machine won't access the same
*		resource at the same time.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*
* OUTPUTS:
*       apResult - AP/HCD results
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApLockGet
(
	GT_U8   devNum,
	GT_U32	portGroup,
	GT_U32	phyPortNum
)
{
	GT_U32 apPortControl;
	GT_U32 apPortStatus;

	if (apGlobalsCfgPtr == NULL || apEngineInitOnDev[devNum][portGroup] == GT_FALSE)
	{
		return GT_NOT_INITIALIZED;
	}

	/* read data from shared memory */
	mvApGetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->apPortStatus[phyPortNum]), &apPortStatus);
	mvApGetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->apPortControl[phyPortNum]), &apPortControl);

	/* check if AP machine acquired the Lock */
	if (AP_STATUS_AP_LOCK_GET(apPortStatus) == GT_TRUE)
	{
		return GT_NO_RESOURCE;
	}

	/* if AP machine  didn't acquire the lock, get the ownership */
	AP_CONTROL_HOST_LOCK_SET(apPortControl, GT_TRUE);
	mvApSetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->apPortControl[phyPortNum]), apPortControl);

	/* check again if AP machine acquired the Lock (at the time the Host got his)
	   in case it did, AP machine has higher priority so Host will release the Lock */
	mvApGetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->apPortStatus[phyPortNum]), &apPortStatus);
	if (AP_STATUS_AP_LOCK_GET(apPortStatus) == GT_TRUE)
	{
		CHECK_STATUS(mvApLockRelease(devNum, portGroup, phyPortNum));
		return GT_NO_RESOURCE;
	}

	return GT_OK;
}

/*******************************************************************************
* mvApSyncRelease
*
* DESCRIPTION:
*       Releases the synchronization lock (between Host and AP machine.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*
* OUTPUTS:
*       apResult - AP/HCD results
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApLockRelease
(
	GT_U8   devNum,
	GT_U32	portGroup,
	GT_U32	phyPortNum
)
{
	GT_U32 apPortControl;

	if (apGlobalsCfgPtr == NULL || apEngineInitOnDev[devNum][portGroup] == GT_FALSE)
	{
		return GT_NOT_INITIALIZED;
	}

	/* read data from shared memory */
	mvApGetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->apPortControl[phyPortNum]), &apPortControl);

	/* clear lock indication */
	AP_CONTROL_HOST_LOCK_SET(apPortControl, GT_FALSE);

	/* write data to shared memory */
    mvApSetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->apPortControl[phyPortNum]), apPortControl);

	return GT_OK;
}

/*******************************************************************************
* mvHwsApPortStatusGet
*
* DESCRIPTION:
*       Returns the AP port resolution info.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*
* OUTPUTS:
*       apResult - AP/HCD results
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApPortStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  apPortNum,
    MV_HWS_AP_PORT_STATUS   *apResult

)
{
  GT_U32 apPortStatus;
  GT_U32 apPortControl;
  if (devNum >= HWS_MAX_DEVICE_NUM)
  {
    return GT_BAD_PARAM;
  }

  if (apGlobalsCfgPtr == NULL || apEngineInitOnDev[devNum][portGroup] == GT_FALSE)
  {
    return GT_NOT_INITIALIZED;
  }

  if ((apPortNum >= HWS_CORE_PORTS_NUM(devNum)) || (apResult == NULL))
  {
    return GT_BAD_PARAM;
  }

    /* read shared memory */
  CHECK_STATUS(mvApGlobalInfoGet(devNum, portGroup));
  apPortStatus = apGlobalsCfgSnap.apPortStatus[apPortNum];
  apPortControl = apGlobalsCfgSnap.apPortControl[apPortNum];

  apResult->preApPortNum = apPortNum;
  apResult->postApPortNum = apPortNum;
  apResult->apLaneNum = AP_CONTROL_LANE_NUM_GET(apPortControl);
  /*apResult->portInit = AP_CONTROL_PORT_INIT_GET(apPortControl);*/

  apResult->postApPortMode = NON_SUP_MODE;
  apResult->hcdResult.hcdFound = AP_STATUS_HCD_FOUND_GET(apPortStatus);
  apResult->hcdResult.hcdFecEn = AP_STATUS_HCD_FEC_RES_GET(apPortStatus);
  apResult->hcdResult.hcdFcRxPauseEn = AP_STATUS_HCD_FC_RX_RES_GET(apPortStatus);
  apResult->hcdResult.hcdFcTxPauseEn = AP_STATUS_HCD_FC_TX_RES_GET(apPortStatus);

  if (apResult->hcdResult.hcdFound)
  {
	  switch (AP_STATUS_HCD_PORT_TYPE_GET(apPortStatus))
	  {
	  case Port_1000Base_KX:
		  apResult->postApPortMode = _1000Base_X;
		  break;
	  case Port_10GBase_KX4:
		  apResult->postApPortMode = _10GBase_KX4;
		  break;
	  case Port_10GBase_R:
		  apResult->postApPortMode = _10GBase_KR;
		  break;
	  case Port_40GBase_R:
		  apResult->postApPortMode = _40GBase_KR;


		  break;
	  default:
		  apResult->postApPortMode = NON_SUP_MODE;
		  break;
    }
  }

  apResult->hcdResult.hcdLinkStatus = AP_STATUS_HCD_LINK_GET(apPortStatus);

  return GT_OK;
}


/*******************************************************************************
* mvHwsApEngineStatusGet
*
* DESCRIPTION:
*       Returns the AP status for all actives AP ports.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       apPortActNum - size of result array
*       apResult     - pointer to array of AP ports results
*
* OUTPUTS:
*       apResult - AP results
*       apPortActNum - size of result array
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsApEngineStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  apPortActNum,
    MV_HWS_AP_PORT_STATUS   *apResult

)
{
  GT_U32 portNum;

  if (devNum >= HWS_MAX_DEVICE_NUM)
  {
    return GT_BAD_PARAM;
  }
  for (portNum = 0; portNum < apPortActNum; portNum++)
  {
    CHECK_STATUS(mvHwsApPortStatusGet(devNum,portGroup,portNum,&apResult[portNum]));
  }

  return GT_OK;
}


/*******************************************************************************
* mvApCheckCounterGet
*
* DESCRIPTION:
*       Read checkCounter value display AP engine activity.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*
* OUTPUTS:
*       'checkCounter' value
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApCheckCounterGet
(
  GT_U8   devNum,
  GT_U32  portGroup,
  GT_U32 *counter
)
{
  if (devNum >= HWS_MAX_DEVICE_NUM)
  {
    return GT_BAD_PARAM;
  }

  if (apGlobalsCfgPtr == NULL || apEngineInitOnDev[devNum][portGroup] == GT_FALSE)
  {
      return GT_NOT_INITIALIZED;
  }
  /* read data from shared memory */
  mvApGetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->checkCounter), counter);

  return GT_OK;
}


/*******************************************************************************
* mvApDebugTimeoutSet
*
* DESCRIPTION:
*       DEBUG: set 10/40 G-KR timeout (in ms).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       timeout   - timeout in ms
*
* OUTPUTS:
*       'checkCounter' value
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApDebugTimeoutSet
(
 GT_U8   devNum,
 GT_U32  portGroup,
 GT_U32  timeout
)
{
    GT_U32 currTimer;
    if (devNum >= HWS_MAX_DEVICE_NUM)
    {
      return GT_BAD_PARAM;
    }

    if (apGlobalsCfgPtr == NULL || apEngineInitOnDev[devNum][portGroup] == GT_FALSE)
    {
        return GT_NOT_INITIALIZED;
    }

    /* read data from shared memory */
    mvApGetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->reserved), &currTimer);
    /*osPrintf("Current timer is %d\n", currTimer);*/


    mvApSetSharedMem(devNum, portGroup, (GT_U32)(GT_UINTPTR)&(apGlobalsCfgPtr->reserved), timeout);

    return GT_OK;
}

#ifdef AP_SHOW
/*******************************************************************************
* mvApPortStatusShow
*
* DESCRIPTION:
*       Print AP port status information stored in system.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       apPortNum - AP port number
*
* OUTPUTS:
*       None
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApPortStatusShow
(
  GT_U8   devNum,
  GT_U32  portGroup,
  GT_U32  apPortNum
)
{
  GT_U32 apPortStatus;

  if (apPortNum >= HWS_CORE_PORTS_NUM(devNum))
  {
    return GT_BAD_PARAM;
  }
  if (apGlobalsCfgPtr)
  {
    /* read shared memory */
    CHECK_STATUS(mvApGlobalInfoGet(devNum, portGroup));
    apPortStatus = apGlobalsCfgSnap.apPortStatus[apPortNum];
    osPrintf("\n\t======== AP Port %d info ========", apPortNum);
    osPrintf("\n\tAP Port Status        - ");
    (AP_STATUS_AP_ACTIVE_GET(apPortStatus)) ? osPrintf("Active.") : osPrintf("Not Active.");
    osPrintf("\n\tAP Port State         - ");
    switch (AP_STATUS_REQ_STATE_GET(apPortStatus))
    {
    case Idle:
      osPrintf("Idle.");
      break;
    case Prepare_AP:
      osPrintf("Prepare_AP.");
      break;
    case Arb_Fsm_Wait:
      osPrintf("Arb_Fsm_Wait.");
      break;
    case Arb_Fsm_Wait_Done:
      osPrintf("Arb_Fsm_Wait_Done.");
      break;
    case Tx_Disable_Wait:
      osPrintf("Tx_Disable_Wait.");
      break;
    case Tx_Disable_Wait_Done:
      osPrintf("Tx_Disable_Wait_Done.");
      break;
    case Ability_Detect_Wait:
      osPrintf("Ability_Detect_Wait.");
      break;
    case Ability_Detect_Wait_Done:
      osPrintf("Ability_Detect_Wait_Done.");
      break;
    case An_Wait:
      osPrintf("An_Wait.");
      break;
    case An_Wait_Done:
      osPrintf("An_Wait_Done.");
      break;
    case Power_Up_Link:
      osPrintf("Power_Up_Link.");
      break;
    case Link_Fail_Inh_Wait:
      osPrintf("Link_Fail_Inh_Wait.");
      break;
    case Link_Fail_Inh_Done:
      osPrintf("Link_Fail_Inh_Done.");
      break;
    case Link_Up:
      osPrintf("Link_Up.");
      break;
    default:
      osPrintf("Unknown.");
      break;
    }
    osPrintf("\n\tAP Port ARB Error     - ");
    (AP_STATUS_AP_ERROR_GET(apPortStatus)) ? osPrintf("No Resolution.") : osPrintf("TBD.");
    /*(AP_STATUS_AP_ERROR_GET(apPortStatus)) ? osPrintf("No Resolution.") : osPrintf("Resolution Done.");*/
    osPrintf("\n\tAP Port HCD Found     - ");
    (AP_STATUS_HCD_FOUND_GET(apPortStatus)) ? osPrintf("HCD Found.") : osPrintf("HCD not Found.");
    osPrintf("\n\tAP Port ARB State     - ");
    switch (AP_STATUS_ARB_FSM_STATE_GET(apPortStatus))
    {
    case ST_AN_ENABLE:
      osPrintf("ST_AN_ENABLE.");
      break;
    case ST_TX_DISABLE:
      osPrintf("ST_TX_DISABLE.");
      break;
    case ST_LINK_STAT_CK:
      osPrintf("ST_LINK_STAT_CK.");
      break;
    case ST_PARALLEL_FLT:
      osPrintf("ST_PARALLEL_FLT.");
      break;
    case ST_ABILITY_DET:
      osPrintf("ST_ABILITY_DET.");
      break;
    case ST_ACK_DETECT:
      osPrintf("ST_ACK_DETECT.");
      break;
    case ST_COMPLETE_ACK:
      osPrintf("ST_COMPLETE_ACK.");
      break;
    case ST_NP_WAIT:
      osPrintf("ST_NP_WAIT.");
      break;
    case ST_AN_GOOD_CK:
      osPrintf("ST_AN_GOOD_CK.");
      break;
    case ST_AN_GOOD:
      osPrintf("ST_AN_GOOD.");
      break;
    case ST_SERDES_WAIT:
      osPrintf("ST_SERDES_WAIT.");
      break;
    default:
      osPrintf("Unknown state.");
      break;
    }
    osPrintf("\n\tAP Port HCD Port Type - ");
    switch (AP_STATUS_HCD_PORT_TYPE_GET(apPortStatus))
    {
    case Port_1000Base_KX:
      osPrintf("Port_1000Base_KX.");
      break;
    case Port_10GBase_KX4:
      osPrintf("Port_10GBase_KX4.");
      break;
    case Port_10GBase_R:
      osPrintf("Port_10GBase_R.");
      break;
    case Port_40GBase_R:
      osPrintf("Port_40GBase_R.");
      break;
    default:
      osPrintf("Unknown type.");
      break;
    }
    osPrintf("\n\tAP Port HCD FEC Res   - ");
    (AP_STATUS_HCD_FEC_RES_GET(apPortStatus)) ? osPrintf("FEC enabled.") : osPrintf("FEC disabled.");

    osPrintf("\n\tAP Port HCD FC Rx Pause  - ");
    (AP_STATUS_HCD_FC_RX_RES_GET(apPortStatus)) ? osPrintf("FC Rx enabled.") : osPrintf("FC Rx disabled.");
    osPrintf("\n\tAP Port HCD FC Tx Pause   - ");
    (AP_STATUS_HCD_FC_TX_RES_GET(apPortStatus)) ? osPrintf("FC Tx enabled.") : osPrintf("FC Tx disabled.");

    osPrintf("\n\tAP Port HCD Link      - ");
    (AP_STATUS_HCD_LINK_GET(apPortStatus)) ? osPrintf("Link Up.") : osPrintf("Link Down.");
    osPrintf("\n\tAP Port PCS Lock      - TBD");
    /*osPrintf("\n\tAP Port PCS Lock      - %d", AP_STATUS_PCS_LOCK_GET(apPortStatus));*/

    osPrintf("\n");
    return GT_OK;
  }

  return GT_NOT_INITIALIZED;
}

/*******************************************************************************
* mvApPortTimerStatus
*
* DESCRIPTION:
*       Print AP port timer information stored in system.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       apPortNum - AP port number
*
* OUTPUTS:
*       None
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApPortTimerStatus
(
  GT_U8   devNum,
  GT_U32  portGroup,
  GT_U32  apPortNum
)
{
  MV_HWS_AP_TIMER_INFO  timer;

  if (apPortNum >= HWS_CORE_PORTS_NUM(devNum))
  {
    return GT_BAD_PARAM;
  }
  if (apGlobalsCfgPtr)
  {
    /* read shared memory */
    CHECK_STATUS(mvApGlobalInfoGet(devNum, portGroup));
    timer = apGlobalsCfgSnap.apTimerList[apPortNum];
    osPrintf("\n\t======== AP Port %d Timer info ========", apPortNum);
    osPrintf("\n\tTimer name          - ");
    switch(timer.timerName)
    {
    case ArbFsmPolling:
      osPrintf("ArbFsmPolling.");
      break;
    case BreakLink:
      osPrintf("BreakLink.");
      break;
    case AutoNegWait:
      osPrintf("AutoNegWait.");
      break;
    case LinkFailInhibit:
      osPrintf("LinkFailInhibit.");
      break;
    default:
      osPrintf("Unknown name.");
      break;
    }
    osPrintf("\n\tTimer status        - ");
    (timer.isActive) ? osPrintf("Timer is active.") : osPrintf("Timer not active.");
    osPrintf("\n\tTimer current value - %d", timer.period);

    osPrintf("\n");
    return GT_OK;
  }

  return GT_NOT_INITIALIZED;
}

/*******************************************************************************
* mvApPortControlShow
*
* DESCRIPTION:
*       Print AP port configuration information stored in system.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       apPortNum - AP port number
*
* OUTPUTS:
*       None
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvApPortControlShow
(
  GT_U8   devNum,
  GT_U32  portGroup,
  GT_U32  apPortNum
)
{
    GT_BOOL         apPortEnabled;
    MV_HWS_AP_CFG   apCfg;

    CHECK_STATUS(mvHwsApPortConfigGet(devNum, portGroup, apPortNum, &apPortEnabled, &apCfg));
    /* read shared memory */

    osPrintf("\n\t======== AP Port %d configuration info ========", apPortNum);
    osPrintf("\n\tAP Port                - ");
    (apPortEnabled ? osPrintf("ENABLE") : osPrintf("DISABLE"));
    if (apPortEnabled)
    {
      osPrintf("\n\tAP Port lane number    - %d", apCfg.apLaneNum);
      osPrintf("\n\tAP Port FC pause       - ");
      (apCfg.fcPause ? osPrintf("Enable") : osPrintf("Disable"));
      osPrintf("\n\tAP Port FC direction   - ");
      (apCfg.fcAsmDir ? osPrintf("Asymmetric") : osPrintf("Symmetric"));
      osPrintf("\n\tAP Port FEC ability    - ");
      (apCfg.fecSup ? osPrintf("Enable") : osPrintf("Disable"));
      osPrintf("\n\tAP Port FEC request    - ");
      (apCfg.fecReq ? osPrintf("Enable") : osPrintf("Disable"));
      osPrintf("\n\tAP Port Loopback       - ");
      (apCfg.nonceDis ? osPrintf("Enable") : osPrintf("Disable"));
      osPrintf("\n\tAP Port Advertisements:  \n");
      (apCfg.modesVector | _40GBase_KR4_Bit3) ? osPrintf("\n\t                        40GBase_KR4") : osPrintf("");
      (apCfg.modesVector | _10GBase_KR_Bit2) ?  osPrintf("\n\t                        10GBase_KR")  : osPrintf("");
      (apCfg.modesVector | _10GBase_KX4_Bit1) ? osPrintf("\n\t                        10GBase_KX4") : osPrintf("");
      (apCfg.modesVector | _1000Base_KX_Bit0) ? osPrintf("\n\t                        1000Base_KX") : osPrintf("");
      osPrintf("\n\tAP Ref Clock Freq      - ");
      ((apCfg.refClockCfg.refClockFreq == MHz_156) ? osPrintf("MHz_156") : osPrintf("Un Known"));
      osPrintf("\n\tAP Ref Clock Source    - ");
      ((apCfg.refClockCfg.refClockSource == PRIMARY_LINE_SRC) ? osPrintf("Primary") : osPrintf("Secondary"));
    }

    osPrintf("\n");
    return GT_OK;
}
#endif

