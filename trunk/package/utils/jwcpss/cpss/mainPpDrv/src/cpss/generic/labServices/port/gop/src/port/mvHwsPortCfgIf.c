/*******************************************************************************
*           Copyright 2001, Marvell International Ltd.
* This code contains confidential information of Marvell semiconductor, inc.
* no rights are granted herein under any patent, mask work right or copyright
* of Marvell or any third party.
* Marvell reserves the right at its sole discretion to request that this code
* be immediately returned to Marvell. This code is provided "as is".
* Marvell makes no warranties, express, implied or otherwise, regarding its
* accuracy, completeness or performance.
********************************************************************************
* mvHwsPortCfgIf.c
*
* DESCRIPTION:
*           This file contains API for port configuartion and tuning parameters
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 48 $
******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <mvHwsPortInitIf.h>
#include <private/mvHwsPortPrvIf.h>
#include <private/mvHwsPortMiscIf.h>
#include <private/mvPortModeElements.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <pcs/mvHwsPcsIf.h>
#include <mac/mvHwsMacIf.h>
#include <common/siliconIf/siliconAddress.h>
#ifndef MV_HWS_REDUCED_BUILD
#include <mvHwsIpcApis.h>
#endif
/**************************** Globals ****************************************************/
static GT_U32 align90Delay = 60;

static GT_U32 serdesOptAlgoMask = 0x0;
extern GT_U32 optPrintLevel;

/* in Lion2/Hooper, this variable allows PCS WA or PCS Rx unreset if set to FALSE */
GT_BOOL performPcsWA = GT_TRUE;

/**************************** Definition *************************************************/
/* #define GT_DEBUG_HWS */
#ifdef  GT_DEBUG_HWS
#include <common/os/gtOs.h>
#define DEBUG_HWS_FULL(level,s) if (optPrintLevel >= level) {osPrintf s;}
#else
#define DEBUG_HWS_FULL(level,s)
#endif

/**************************** Pre-Declaration ********************************************/
extern GT_STATUS mvHwsComHRev2SerdesTrainingOptimization
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  *serdesList,
   GT_U32  numOfActLanes,
   MV_HWS_PORT_MAN_TUNE_MODE       tuningMode,
   GT_U32  algoMask
);

GT_STATUS hwsPortFixAlign90Ext
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_U32  optAlgoMask
);

GT_STATUS mvHwsComHRev2SerdesConfigRegulator
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  *serdesList,
   GT_U32  numOfActLanes,
   GT_BOOL setDefault
);

GT_STATUS mvHwsPortPostTraining
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode
);

/*******************************************************************************
* mvHwsPCSMarkModeSet
*
* DESCRIPTION:
*       Mark/Un-mark PCS unit
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       enable      - GT_TRUE  for mark the PCS,
*                GT_FALSE for un-mark the PCS
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK      - on success
*       GT_BAD_PARAM    - on wrong parameter
*******************************************************************************/
GT_STATUS mvHwsPCSMarkModeSet
(
   GT_U8   devNum,
   GT_UOPT portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_BOOL enable
)
{
   MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;

   hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

   if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].pcsMarkModeFunc == NULL)
   {
           return GT_OK;
   }

   return hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].pcsMarkModeFunc(devNum, portGroup, phyPortNum, portMode, enable);
}

/*******************************************************************************
* mvHwsPortPartialPowerDown
*
* DESCRIPTION:
*          Enable/Disable the power down Tx and Rx of on Port.
*          The configuration performs Enable/Disable of Tx and Rx on specific Serdes.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       powerDownRx - Enable/Disable the Port Rx power down
*       powerDownTx - Enable/Disable the Port Tx power down
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsPortPartialPowerDown
(
    GT_U8          devNum,
    GT_U32         portGroup,
    GT_U32         phyPortNum,
    MV_HWS_PORT_STANDARD   portMode,
    GT_BOOL        powerDownRx,
    GT_BOOL        powerDownTx
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32  curLanesList[HWS_MAX_SERDES_NUM];
    GT_U32 i;

    if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) || ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
    {
        return GT_BAD_PARAM;
    }

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

    /* rebuild active lanes list according to current configuration (redundancy) */
    CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

    /* on each related serdes */
    for (i = 0; i < curPortParams->numOfActLanes; i++)
    {
        /* Enable/Disable power down of Tx and Rx on Serdes */
        CHECK_STATUS(mvHwsSerdesPartialPowerDown(devNum, portGroup, (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), powerDownRx, powerDownTx));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsPortPartialPowerStatusGet
*
* DESCRIPTION:
*          Get the status of power Tx and Rx on port
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       powerRxStatus - GT_TRUE - Serdes power Rx is down
*                       GT_FALSE - Serdes power Rx is up
*       powerTxStatus - GT_TRUE - Serdes power Tx is down
*                       GT_FALSE - Serdes power Tx is up
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsPortPartialPowerStatusGet
(
    GT_U8          devNum,
    GT_U32         portGroup,
    GT_U32         phyPortNum,
    MV_HWS_PORT_STANDARD   portMode,
    GT_BOOL        *powerRxStatus,
    GT_BOOL        *powerTxStatus
)
{
    GT_U32  curLanesList[HWS_MAX_SERDES_NUM];

    if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) || ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
    {
        return GT_BAD_PARAM;
    }

    /* rebuild active lanes list according to current configuration (redundancy) */
    CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

    /* Get the status of Serdes Tx and Rx power */
    CHECK_STATUS(mvHwsSerdesPartialPowerStatusGet(devNum, portGroup, (curLanesList[0] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum),
                                                  powerRxStatus, powerTxStatus));

    return GT_OK;
}

/*******************************************************************************
* hwsPortTxAutoTuneStartSet
*
* DESCRIPTION:
*       Sets the port Tx only parameters according to different working
*       modes/topologies.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portTuningMode - port TX related tuning mode
*
* OUTPUTS:
*       Tuning results for recommended settings.(TBD)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS hwsPortTxAutoTuneStartSet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_PORT_AUTO_TUNE_MODE      portTuningMode,
   GT_U32  optAlgoMask
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   MV_HWS_AUTO_TUNE_STATUS rxStatus[HWS_MAX_SERDES_NUM];
   MV_HWS_AUTO_TUNE_STATUS txStatus[HWS_MAX_SERDES_NUM];
   GT_U32  curLanesList[HWS_MAX_SERDES_NUM];
   GT_BOOL autoTunePass[HWS_MAX_SERDES_NUM];
   GT_BOOL laneFail = GT_FALSE;
   GT_U32 i;
   MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;

   /* avoid warnings */
   optAlgoMask = optAlgoMask;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
       ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

    if ((portTuningMode == RxTrainingAdative) ||
        (portTuningMode == RxTrainingStopAdative))
    {
        return GT_NOT_SUPPORTED;
    }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   hwsOsMemSetFuncPtr(autoTunePass, 0, sizeof(autoTunePass));
   hwsOsMemSetFuncPtr(rxStatus, 0, sizeof(rxStatus));
   hwsOsMemSetFuncPtr(txStatus, 0, sizeof(txStatus));

   hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   /* TRxTuneCfg flow */
   if (portTuningMode == TRxTuneCfg)
   {
           /* on each related serdes */
           for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < curPortParams->numOfActLanes); i++)
           {
                   CHECK_STATUS(mvHwsSerdesAutoTuneCfg(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                           (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum),
                                                           GT_FALSE, GT_TRUE));
           }

           return GT_OK;
   }

   /* TRxTuneStart flow */
   if (portTuningMode == TRxTuneStart)
   {
           /* on each related serdes */
           for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < curPortParams->numOfActLanes); i++)
           {
                   if (autoTunePass[i] == GT_FALSE)
                   {
                           /* enable TX tunning */
                           CHECK_STATUS(mvHwsSerdesTxAutoTuneStart(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                                   (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), GT_TRUE));
                   }
           }

           /* mark PCS to be under TRx training */
           CHECK_STATUS(mvHwsPCSMarkModeSet(devNum, portGroup, phyPortNum, portMode, GT_TRUE));

           /* Reset MAC */
           CHECK_STATUS(mvHwsMacReset(devNum, portGroup, curPortParams->portMacNumber,
                                           curPortParams->portMacType, RESET));

           /* Reset PCS */
           CHECK_STATUS(mvHwsPcsReset(devNum, portGroup, curPortParams->portPcsNumber,
                                           curPortParams->portPcsType, RESET));

           /* on each related Serdes apply Rf reset */
           for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < curPortParams->numOfActLanes); i++)
           {
                   CHECK_STATUS(mvHwsSerdesDigitalReset(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                           (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), RESET));
           }

           return GT_OK;
   }

   /* TRxTuneStatus flow */
   for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < curPortParams->numOfActLanes); i++)
   {
           if (autoTunePass[i] == GT_FALSE)
           {
                   mvHwsSerdesAutoTuneStatus(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                   (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), NULL, &txStatus[i]);
                   if (txStatus[i] == TUNE_PASS)
                   {
                           autoTunePass[i] = GT_TRUE;
                   }
                   else
                   {
                           laneFail = GT_TRUE;
                   }
           }
   }

   if (laneFail == GT_TRUE)
   {
           /* if training failed - unreset all units to allow farther configurations */
           CHECK_STATUS(mvHwsPortPostTraining(devNum, portGroup, phyPortNum, portMode));
           return GT_FAIL;
   }

   if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portAutoTuneSetWaFunc != NULL)
   {
           hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portAutoTuneSetWaFunc(devNum, portGroup, phyPortNum, portMode, &(curLanesList[0])) ;
   }

   /* UnReset RF on each related Serdes */
   for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < curPortParams->numOfActLanes); i++)
   {
           CHECK_STATUS(mvHwsSerdesDigitalReset(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)), (curLanesList[i] & 0xFFFF),
                                            HWS_DEV_SERDES_TYPE(devNum), UNRESET));
   }

   /* UnReset PCS because FixAlign90 need signal */
   CHECK_STATUS(mvHwsPcsReset(devNum, portGroup, curPortParams->portPcsNumber,
                                           curPortParams->portPcsType, UNRESET));

   return GT_OK;
}

/*******************************************************************************
* hwsPortTxAutoTuneActivateSet
*
* DESCRIPTION:
*       Unreset MAC and PCS after TRx training
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portTuningMode - port TX related tuning mode
*
* OUTPUTS:
*       Tuning results for recommended settings.(TBD)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS hwsPortTxAutoTuneActivateSet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_PORT_AUTO_TUNE_MODE      portTuningMode
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

   /* avoid warnings */
   portTuningMode = portTuningMode;

   /* UnReset MAC: at this point FixAlign90 already finished and we are after TRX-Training */
   CHECK_STATUS(mvHwsMacReset(devNum, portGroup, curPortParams->portMacNumber,
                                           curPortParams->portMacType, UNRESET));

   /* unmark PCS to be under TRx training */
   CHECK_STATUS(mvHwsPCSMarkModeSet(devNum, portGroup, phyPortNum, portMode, GT_FALSE));

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortAutoTuneSet
*
* DESCRIPTION:
*       Sets the port Tx and Rx parameters according to different working
*       modes/topologies.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portTuningMode - port tuning mode
*
* OUTPUTS:
*       Tuning results for recommended settings.(TBD)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortAutoTuneSet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_PORT_AUTO_TUNE_MODE      portTuningMode,
   void *  results
)
{
   return mvHwsPortAutoTuneSetExt(devNum, portGroup, phyPortNum, portMode,
                                           portTuningMode, serdesOptAlgoMask, results);
}

/*******************************************************************************
* mvHwsPortAutoTuneSetExt
*
* DESCRIPTION:
*       Sets the port Tx and Rx parameters according to different working
*       modes/topologies.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       optAlgoMask - bit mask for optimization algorithms
*       portTuningMode - port tuning mode
*
* OUTPUTS:
*       Tuning results for recommended settings.(TBD)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortAutoTuneSetExt
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD        portMode,
   MV_HWS_PORT_AUTO_TUNE_MODE      portTuningMode,
   GT_U32  optAlgoMask,
   void *  results
)
{
   MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;

   hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

   if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portAutoTuneSetExtFunc == NULL)
   {
           return GT_OK;
   }

#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortAutoTuneSetExtIpc(devNum,portGroup, phyPortNum, portMode, portTuningMode, optAlgoMask, results));
#endif

   return hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portAutoTuneSetExtFunc(devNum, portGroup, phyPortNum,
                                                                           portMode, (MV_HWS_PORT_AUTO_TUNE_MODE)portTuningMode,
                                                                           optAlgoMask,  results);
}

/*******************************************************************************
* mvHwsPortManTuneSet
*
* DESCRIPTION:
*       Sets the port Tx and Rx parameters according to different working
*       modes/topologies.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portTuningMode - port tuning mode
*       tunParams      - pointer to tune parameters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortManTuneSet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_PORT_MAN_TUNE_MODE       portTuningMode,
   MV_HWS_AUTO_TUNE_RESULTS        *tunParams
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 i;

   portTuningMode = portTuningMode;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   /* on each related serdes */
   for (i = 0; i < curPortParams->numOfActLanes; i++)
   {
           CHECK_STATUS(mvHwsSerdesManualRxConfig(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                   (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum),
                                                   portTuningMode, tunParams->sqleuch, tunParams->ffeR, tunParams->ffeC,
                                                   GT_TRUE, tunParams->align90));
   }

   for (i = 0; i < curPortParams->numOfActLanes; i++)
   {
           CHECK_STATUS(mvHwsSerdesManualTxConfig(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                   (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum),
                                                   tunParams->txAmp, GT_TRUE, tunParams->txEmph0, tunParams->txEmph1,
                                                   tunParams->txAmpShft));
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortManualRxConfig
*
* DESCRIPTION:
*          configures SERDES Rx parameters for all SERDES lanes.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       configParams- pointer to array of the config params structures
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsPortManualRxConfig
(
    GT_U8                              devNum,
    GT_U32                             portGroup,
    GT_U32                             phyPortNum,
    MV_HWS_PORT_STANDARD           portMode,
    MV_HWS_PORT_MAN_TUNE_MODE       portTuningMode,
    MV_HWS_MAN_TUNE_RX_CONFIG_DATA  *configParams
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32  curLanesList[HWS_MAX_SERDES_NUM];
    GT_U32 i;

    if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) || ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum))
        || (HWS_DEV_SILICON_TYPE(devNum) == BobK))
    {
        return GT_BAD_PARAM;
    }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

    /* rebuild active lanes list according to current configuration (redundancy) */
    CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

    /* on each related serdes */
    for (i = 0; i < curPortParams->numOfActLanes; i++)
    {
        CHECK_STATUS(mvHwsSerdesManualRxConfig(devNum, portGroup,(curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum),
                                               portTuningMode, configParams[i].sqlch, configParams[i].ffeRes,
                                               configParams[i].ffeCap,configParams[i].dfeEn, configParams[i].alig));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsPortManualTxConfig
*
* DESCRIPTION:
*          configures SERDES tx parameters for all SERDES lanes.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       configParams- pointer to array of the config params structures
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsPortManualTxConfig
(
    GT_U8                              devNum,
    GT_U32                             portGroup,
    GT_U32                             phyPortNum,
    MV_HWS_PORT_STANDARD           portMode,
    MV_HWS_PORT_MAN_TUNE_MODE       portTuningMode,
    MV_HWS_MAN_TUNE_TX_CONFIG_DATA  *configParams
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32  curLanesList[HWS_MAX_SERDES_NUM];
    GT_U32 i;

    portTuningMode = portTuningMode; /* avoid warnings */

    if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) || ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
    {
        return GT_BAD_PARAM;
    }

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

    /* rebuild active lanes list according to current configuration (redundancy) */
    CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

    /* on each related serdes */
   for (i = 0; i < curPortParams->numOfActLanes; i++)
   {
           CHECK_STATUS(mvHwsSerdesManualTxConfig(devNum, portGroup,(curLanesList[i] & 0xFFFF),HWS_DEV_SERDES_TYPE(devNum),
                                               configParams[i].txAmp, configParams[i].txAmpAdj,
                                               configParams[i].txEmph0, configParams[i].txEmph1,
                                                                      configParams[i].txAmpShft));
   }

    return GT_OK;
}

/*******************************************************************************
* mvHwsPortManualTxConfigGet
*
* DESCRIPTION:
*       Get SERDES tx parameters for all SERDES lanes.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*
* OUTPUTS:
*       configParams- pointer to array of the config params structures:
*           txAmp     Tx amplitude
*           txAmpAdj  TX adjust
*           txemph0   TX emphasis 0
*           txemph1   TX emphasis 1
*           txAmpShft TX amplitude shift
*
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsPortManualTxConfigGet
(
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          phyPortNum,
    MV_HWS_PORT_STANDARD            portMode,
    MV_HWS_PORT_MAN_TUNE_MODE       portTuningMode,
    MV_HWS_MAN_TUNE_TX_CONFIG_DATA  *configParams
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32  curLanesList[HWS_MAX_SERDES_NUM];
    GT_U32 i;

    portTuningMode = portTuningMode; /* avoid warnings */

    if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) || ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
    {
        return GT_BAD_PARAM;
    }

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

    /* rebuild active lanes list according to current configuration (redundancy) */
    CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

    /* on each related serdes */
    for (i = 0; i < curPortParams->numOfActLanes; i++)
    {
        CHECK_STATUS(mvHwsSerdesManualTxConfigGet(devNum, portGroup, (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &configParams[i]));
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsPortTestGenerator
*
* DESCRIPTION:
*       Activate the port related PCS Tx generator and Rx checker control.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portPattern - port tuning mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortTestGenerator
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_PORT_TEST_GEN_PATTERN    portPattern,
   MV_HWS_PORT_TEST_GEN_ACTION     actionMode
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;

   actionMode = actionMode;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   CHECK_STATUS(mvHwsPcsTestGenCfg(devNum, portGroup, curPortParams->portPcsNumber,
                                                   curPortParams->portPcsType, portPattern));

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortTestGeneratorStatus
*
* DESCRIPTION:
*       Get test errors - every get clears the errors.
*       Can be run any time after delete port or after power up
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portTuningMode - port tuning mode
*
* OUTPUTS:
*       Test system results: total errors, good frames, checker Lock Status
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortTestGeneratorStatus
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_PORT_TEST_GEN_PATTERN    portPattern,
   MV_HWS_TEST_GEN_STATUS  *status
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) || ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   CHECK_STATUS(mvHwsPcsTestGenStatus(devNum, portGroup, curPortParams->portPcsNumber,
                                                   curPortParams->portPcsType, portPattern, status));

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortPPMSet
*
* DESCRIPTION:
*       Increase/decrease  Tx clock on port (added/sub ppm).
*       Can be run only after create port not under traffic.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portPPM    - limited to +/- 3 taps
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortPPMSet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_PPM_VALUE        portPPM
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 i;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortPPMSetIpc(devNum,portGroup, phyPortNum,portMode,portPPM));
#endif
   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   CHECK_STATUS(mvHwsPcsReset(devNum, portGroup, curPortParams->portPcsNumber,
                                           curPortParams->portPcsType, RESET));

   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   /* on each related serdes */
   for (i = 0; i < curPortParams->numOfActLanes; i++)
   {
           CHECK_STATUS(mvHwsSerdesPpmConfig(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                           (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), portPPM));
   }

   /* pcs unreset */
   CHECK_STATUS(mvHwsPcsReset(devNum, portGroup, curPortParams->portPcsNumber,
                                           curPortParams->portPcsType, UNRESET));

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortPPMGet
*
* DESCRIPTION:
*       Check the entire line configuration, return ppm value in case of match in all
*       or error in case of different configuration.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       portPPM    - current PPM
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortPPMGet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_PPM_VALUE        *portPPM
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   MV_HWS_PPM_VALUE lanePpm, curPpm;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 i;

   if ((portPPM == NULL) ||
       (phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
       ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortPPMGetIpc(devNum,portGroup, phyPortNum,portMode,portPPM));
#endif
   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

   if (curPortParams->numOfActLanes == 0)
   {
           *portPPM = (MV_HWS_PPM_VALUE)0;
           return GT_FAIL;
   }

   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   /* read first lane PPM */
   CHECK_STATUS(mvHwsSerdesPpmConfigGet(devNum, (portGroup + ((curLanesList[0] >> 16) & 0xFF)),
                                                   (curLanesList[0] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum),&lanePpm));
   *portPPM = lanePpm;

   /* on each related serdes */
   for (i = 1; i < curPortParams->numOfActLanes; i++)
   {
           CHECK_STATUS(mvHwsSerdesPpmConfigGet(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                           (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &curPpm));
           if (lanePpm != curPpm)
           {
                   return GT_FAIL;
           }
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortLoopbackSet
*
* DESCRIPTION:
*       Activates the port loopback modes.
*       Can be run only after create port not under traffic.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       lpPlace    - unit for loopback configuration
*       lpType     - loopback type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortLoopbackSet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_UNIT     lpPlace,
   MV_HWS_PORT_LB_TYPE     lbType
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   MV_HWS_SERDES_LB serdesLbType;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 i;
   MV_HWS_PORT_MAC_TYPE macType;
   MV_HWS_PORT_PCS_TYPE pcsType;
   MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;


   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortLoopbackSetIpc(devNum,portGroup, phyPortNum,portMode,lpPlace,lbType));
#endif

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   macType = curPortParams->portMacType;
   pcsType = curPortParams->portPcsType;

   /* In case of XG or XLG MAC, MAC and PCS need to be reset */
   if ((lpPlace == HWS_MAC) && ((macType == XGMAC) || (macType == XLGMAC)))
   {
           CHECK_STATUS(mvHwsMacReset(devNum, portGroup,
                                                   curPortParams->portMacNumber, macType, RESET));
           CHECK_STATUS(mvHwsPcsReset(devNum, portGroup,
                                                   curPortParams->portPcsNumber, pcsType, RESET));
   }

   hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

   if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portLbSetWaFunc != NULL)
   {
           hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portLbSetWaFunc(devNum, portGroup, phyPortNum, portMode,
                                                                                   (MV_HWS_PORT_LB_TYPE)lbType, &(curLanesList[0]));
   }

   switch (lpPlace)
   {
   case HWS_MAC:
           CHECK_STATUS(mvHwsMacLoopbackCfg(devNum, portGroup, curPortParams->portMacNumber,
                                            curPortParams->portMacType, lbType));
           break;
   case HWS_PCS:
           CHECK_STATUS(mvHwsPcsLoopbackCfg(devNum, portGroup, curPortParams->portPcsNumber,
                                            curPortParams->portPcsType, lbType));
           break;
   case HWS_PMA:
           /* convert port LB type to Serdes LB type */
           switch (lbType)
           {
           case DISABLE_LB:
                   serdesLbType = SERDES_LP_DISABLE;
                   /* signal detect mask set 0 per pcs lane*/
                   mvHwsPcsSignalDetectMaskSet(devNum, portGroup, curPortParams->portPcsNumber,
                                                   curPortParams->portPcsType, GT_FALSE);
                   break;
           case RX_2_TX_LB:
           serdesLbType = SERDES_LP_DIG_RX_TX;
                   break;
           case TX_2_RX_LB:
                   serdesLbType = SERDES_LP_AN_TX_RX;
           /*      signal detect mask set 1 per pcs lane*/
                   mvHwsPcsSignalDetectMaskSet(devNum, portGroup, curPortParams->portPcsNumber,
                                                   curPortParams->portPcsType, GT_TRUE);
                   break;
           case TX_2_RX_DIGITAL_LB:
                   serdesLbType = SERDES_LP_DIG_TX_RX;
                   /* signal detect mask set 1 per pcs lane*/
                   mvHwsPcsSignalDetectMaskSet(devNum, portGroup, curPortParams->portPcsNumber,
                                                   curPortParams->portPcsType, GT_TRUE);
                   break;
           default:
                   return GT_BAD_PARAM;
           }

           /* on each related serdes */
           for (i = 0; i < curPortParams->numOfActLanes; i++)
           {
                   CHECK_STATUS(mvHwsSerdesLoopback(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                           (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), serdesLbType));
           }
           break;
   default:
           return GT_NOT_SUPPORTED;
   }

   /* Un-reset in case of XG or XLG MAC */
   if ((lpPlace == HWS_MAC) && ((macType == XGMAC) || (macType == XLGMAC)))
   {
           CHECK_STATUS(mvHwsPcsReset(devNum, portGroup,
                                                   curPortParams->portPcsNumber, pcsType, UNRESET));
           CHECK_STATUS(mvHwsMacReset(devNum, portGroup,
                                                   curPortParams->portMacNumber, macType, UNRESET));
   }

   return GT_OK;
}

#ifndef CO_CPU_RUN
/*******************************************************************************
* mvHwsPortLoopbackStatusGet
*
* DESCRIPTION:
*       Retrive MAC loopback status.
*       Can be run only after create port not under traffic.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       lpPlace    - unit for loopback configuration
*
* OUTPUTS:
*       lbType    - supported loopback type
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortLoopbackStatusGet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_UNIT     lpPlace,
   MV_HWS_PORT_LB_TYPE     *lbType
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   MV_HWS_SERDES_LB         serdesLbType;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortLoopbackStatusGetIpc(devNum,portGroup, phyPortNum,portMode,lpPlace,lbType));
#endif

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

   switch (lpPlace)
   {
   case HWS_MAC:
           CHECK_STATUS(mvHwsMacLoopbackStatusGet(devNum, portGroup, curPortParams->portMacNumber,
                                                  curPortParams->portMacType, lbType));
           break;
   case HWS_PCS:
           CHECK_STATUS(mvHwsPcsLoopbackCfgGet(devNum, portGroup, curPortParams->portPcsNumber,
                                                   curPortParams->portPcsType, lbType));
           break;
   case HWS_PMA:
           /* rebuild active lanes list according to current configuration (redundancy) */
           CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));
           /* read loopback status on first lane */
           CHECK_STATUS(mvHwsSerdesLoopbackGet(devNum, (portGroup + ((curLanesList[0] >> 16) & 0xFF)),
                                                   (curLanesList[0] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &serdesLbType));
           /* convert serdes LB type to port LB type */
           switch (serdesLbType)
           {
           case SERDES_LP_DISABLE:
                   *lbType = DISABLE_LB;
                   break;
           case SERDES_LP_DIG_RX_TX:
                   *lbType = RX_2_TX_LB;
                   break;
           case SERDES_LP_AN_TX_RX:
                   *lbType = TX_2_RX_LB;
                   break;
           case SERDES_LP_DIG_TX_RX:
                   *lbType = TX_2_RX_DIGITAL_LB;
                   break;
           default:
                   return GT_FAIL;
           }
           break;
   default:
           return GT_NOT_SUPPORTED;
   }

   return GT_OK;
}
#endif

/*******************************************************************************
* mvHwsPortLinkStatusGet
*
* DESCRIPTION:
*       Returns the port link status.
*       Can be run any time.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       Port link UP status (true/false).
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortLinkStatusGet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_BOOL *linkStatus
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;

#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortLinkStatusGetIpc(devNum,portGroup, phyPortNum,portMode,linkStatus));
#endif
   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
    if (curPortParams == NULL)
    {
        return GT_NOT_SUPPORTED;
    }

   CHECK_STATUS(mvHwsMacLinkStatusGet(devNum,portGroup,curPortParams->portMacNumber,
                                      curPortParams->portMacType,linkStatus));

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortPolaritySet
*
* DESCRIPTION:
*       Defines the port polarity of the Serdes lanes (Tx/Rx).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       txInvMask  - bitmap of 32 bit, each bit represent Serdes
*       rxInvMask  - bitmap of 32 bit, each bit represent Serdes
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortPolaritySet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_32   txInvMask,
   GT_32   rxInvMask
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_BOOL txInv, rxInv;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 i;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortPolaritySetIpc(devNum,portGroup, phyPortNum,portMode,txInvMask,rxInvMask));
#endif
   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   CHECK_STATUS(mvHwsPcsReset(devNum, portGroup, curPortParams->portPcsNumber,
                                           curPortParams->portPcsType, RESET));

   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   /* on each related serdes */
   for (i = 0; i < curPortParams->numOfActLanes; i++)
   {
           txInv = ((txInvMask >> i) & 1);
           rxInv = ((rxInvMask >> i) & 1);
           CHECK_STATUS(mvHwsSerdesPolarityConfig(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                   (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), txInv, rxInv));
   }

   /* pcs unreset */
   CHECK_STATUS(mvHwsPcsReset(devNum, portGroup, curPortParams->portPcsNumber,
                                           curPortParams->portPcsType, UNRESET));

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortTxEnable
*
* DESCRIPTION:
*       Turn of the port Tx according to selection.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       enable     - enable/disable port Tx
*
* OUTPUTS:
*       Tuning results for recommended settings.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortTxEnable
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_BOOL enable
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 i;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }
#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortTxEnableIpc(devNum,portGroup, phyPortNum,portMode,enable));
#endif

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   /* on each related serdes */
   for (i = 0; i < curPortParams->numOfActLanes; i++)
   {
           CHECK_STATUS(mvHwsSerdesTxEnable(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                            (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), enable));
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortTxEnableGet
*
* DESCRIPTION:
*       Retrieve the TX status of all port serdeses.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       TxStatus per serdes.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortTxEnableGet
(
   GT_U8                   devNum,
   GT_U32                  portGroup,
   GT_U32                  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_BOOL                 serdesTxStatus[]
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 i;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }
#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortTxEnableGetIpc(devNum,portGroup, phyPortNum,portMode,serdesTxStatus));
#endif

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   /* on each related serdes */
   for (i = 0; i < curPortParams->numOfActLanes; i++)
   {
           CHECK_STATUS(mvHwsSerdesTxEnableGet(devNum, portGroup,(curLanesList[i] & 0xFFFF),
                                            HWS_DEV_SERDES_TYPE(devNum), &serdesTxStatus[i]));
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortSignalDetectGet
*
* DESCRIPTION:
*       Retrieve the signal detected status of all port serdeses.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       signalDetect per serdes.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortSignalDetectGet
(
   GT_U8                   devNum,
   GT_U32                  portGroup,
   GT_U32                  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_BOOL                 signalDet[]
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 i;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }
#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortSignalDetectGetIpc(devNum,portGroup, phyPortNum,portMode,signalDet));
#endif

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   /* on each related serdes */
   for (i = 0; i < curPortParams->numOfActLanes; i++)
   {
           CHECK_STATUS(mvHwsSerdesSignalDetectGet(devNum, portGroup,(curLanesList[i] & 0xFFFF),
                                                HWS_DEV_SERDES_TYPE(devNum), &signalDet[i]));
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortCdrLockStatusGet
*
* DESCRIPTION:
*       Retrieve the CDR lock status of all port serdeses.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       cdrLockStatus per serdes.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortCdrLockStatusGet
(
   GT_U8                   devNum,
   GT_U32                  portGroup,
   GT_U32                  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_BOOL                 cdrLockStatus[]
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 i;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }
#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortCdrLockStatusGetIpc(devNum,portGroup, phyPortNum,portMode,cdrLockStatus));
#endif

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   /* on each related serdes */
   for (i = 0; i < curPortParams->numOfActLanes; i++)
   {
           CHECK_STATUS(mvHwsSerdesCdrLockStatusGet(devNum, portGroup,(curLanesList[i] & 0xFFFF),
                                                HWS_DEV_SERDES_TYPE(devNum), &cdrLockStatus[i]));
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortLaneNumberGet
*
* DESCRIPTION:
*       Returns the active port lanes (after applying redundancy).
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       List of lanes and lanes number.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortLaneNumberGet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  portNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_U32  *laneNumber,
   GT_U32  **listOfLanes
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;

   portGroup = portGroup;
   curPortParams = hwsPortModeParamsGet(devNum, portGroup, portNum, portMode);

   if (*laneNumber > curPortParams->numOfActLanes)
   {
           return GT_BAD_PARAM;
   }

   *listOfLanes = curPortParams[portNum].activeLanesList;

   return GT_OK;
}

/*******************************************************************************
* hwsPortExtendedModeCfg
*
* DESCRIPTION:
*       Enable / disable extended mode on port specified.
*       Extended ports supported only in Lion2 and Alleycat3 devices.
*       For Lion2:      1G, 10GBase-R, 20GBase-R2, RXAUI - can be normal or extended
*                                  XAUI, DXAUI, 40GBase-R - only extended
*          For Alleycat3:  ports 25 and 27 can be 10GBase_KR, 10GBase_SR_LR - normal or extended modes
*                                          port 27 can be 20GBase_KR, 20GBase_SR_LR - only in extended mode
*
* INPUTS:
*       devNum       - system device number
*       portGroup    - port group (core) number
*       phyPortNum   - physical port number
*       portMode     - port standard metric
*       extendedMode - enable / disable
*
* OUTPUTS:
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS hwsPortExtendedModeCfg
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_BOOL extendedMode
)
{
   MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;

   hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

   if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portExtModeCfgFunc == NULL)
   {
           return GT_OK;
   }

   return hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portExtModeCfgFunc(devNum, portGroup,
                                                                           phyPortNum, portMode, extendedMode);
}

/*******************************************************************************
* mvHwsPortExtendedModeCfgGet
*
* DESCRIPTION:
*       Returns the extended mode status on port specified.
*
* INPUTS:
*       devNum       - system device number
*       portGroup    - port group (core) number
*       phyPortNum   - physical port number
*       portMode     - port standard metric
*
* OUTPUTS:
*       extendedMode - enable / disable
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortExtendedModeCfgGet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_BOOL *extendedMode
)
{
   MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;

   hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

   if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portExtModeCfgGetFunc == NULL)
   {
           return GT_OK;
   }

   return hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portExtModeCfgGetFunc(devNum, portGroup,
                                                                           phyPortNum, portMode, extendedMode);
}

/*******************************************************************************
* mvHwsPortFixAlign90Ext
*
* DESCRIPTION:
*       Fix Align90 parameters
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFixAlign90Ext
(
    GT_U8   devNum,
    GT_U32  portGroup,
    GT_U32  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode
)
{
    MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;

    hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

    if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portFixAlign90Func == NULL)
    {
        return GT_OK;
    }

    return hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portFixAlign90Func(devNum, portGroup, phyPortNum, portMode);
}

/*******************************************************************************
* mvHwsPortAutoTuneStateCheck
*
* DESCRIPTION:
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portTuningMode - port TX related tuning mode
*
* OUTPUTS:
*       Tuning results for recommended settings.(TBD)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortAutoTuneStateCheck
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_AUTO_TUNE_STATUS *rxTune,
   MV_HWS_AUTO_TUNE_STATUS *txTune
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   MV_HWS_AUTO_TUNE_STATUS rxStatus[HWS_MAX_SERDES_NUM];
   MV_HWS_AUTO_TUNE_STATUS txStatus[HWS_MAX_SERDES_NUM];
   GT_U32  curLanesList[HWS_MAX_SERDES_NUM];
   GT_BOOL autoTunePass[HWS_MAX_SERDES_NUM];
   GT_U32 i;
#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortAutoTuneStateCheckIpc(devNum,portGroup, phyPortNum,portMode,rxTune,txTune));

#endif

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   hwsOsMemSetFuncPtr(autoTunePass, 0, sizeof(autoTunePass));
   hwsOsMemSetFuncPtr(rxStatus, 0, sizeof(rxStatus));
   hwsOsMemSetFuncPtr(txStatus, 0, sizeof(txStatus));

   *txTune = TUNE_PASS;
   *rxTune = TUNE_PASS;

   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < curPortParams->numOfActLanes); i++)
   {
           CHECK_STATUS(mvHwsSerdesAutoTuneStatusShort(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                   (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &rxStatus[i], &txStatus[i]));
           if (txStatus[i] != TUNE_PASS)
           {
                   *txTune = txStatus[i];
           }

           if (rxStatus[i] != TUNE_PASS)
           {
                   *rxTune = rxStatus[i];
           }
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortPostTraining
*
* DESCRIPTION:
*       Perform UnReset for Serdes(RF), PCS and MAC units after performing TRX-Training
*          Note: this function is used mainly used by AP process where
*                    training is executed in non-blocking mode (unlike "regular"
*                    port mode training where status functionality is not completed
*                    till all lanes finished their training).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortPostTraining
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
   MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;
   GT_32  j;

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

   if (performPcsWA == GT_FALSE)
   {
           /* UnReset PCS Rx*/
           CHECK_STATUS(mvHwsPcsRxReset(devNum, portGroup, curPortParams->portPcsNumber,
                                           curPortParams->portPcsType, UNRESET));

   }
   else
   {
           hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

           /* rebuild active lanes list according to current configuration (redundancy) */
           CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

           /* Serdes digital UnReset */
           for (j = curPortParams->numOfActLanes-1; j >= 0; j--)
           {
                   CHECK_STATUS(mvHwsSerdesDigitalReset(devNum, (portGroup + ((curLanesList[j] >> 16) & 0xFF)),
                                                           (curLanesList[j] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), UNRESET));
           }

           /* UnReset PCS */
           CHECK_STATUS(mvHwsPcsReset(devNum, portGroup, curPortParams->portPcsNumber,
                                           curPortParams->portPcsType, UNRESET));

        /* Run fix Align90 */
        CHECK_STATUS(mvHwsPortFixAlign90Ext(devNum, portGroup, phyPortNum, portMode));

           /* UnReset MAC */
           CHECK_STATUS(mvHwsMacReset(devNum, portGroup, curPortParams->portMacNumber,
                                           curPortParams->portMacType, UNRESET));
   }

   /* un-mark PCS to be under TRx training */
   CHECK_STATUS(mvHwsPCSMarkModeSet(devNum, portGroup, phyPortNum, portMode, GT_FALSE));

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortAutoTuneStop
*
* DESCRIPTION:
*       Stop Tx and Rx training.
*          Note: this function is used mainly used by AP process where
*                    training is executed in non-blocking mode (unlike "regular"
*                    port mode training where status functionality is not completed
*                    till all lanes finished their training).
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portTuningMode - port tuning mode
*
* OUTPUTS:
*       Tuning results for recommended settings.(TBD)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortAutoTuneStop
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_BOOL stopRx,
   GT_BOOL stopTx
)
{
   MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32  curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32  i;

    if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortAutoTuneStopIpc(devNum,portGroup, phyPortNum, portMode, stopRx,stopTx));
#endif

    hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

    if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portAutoTuneStopFunc != NULL)
    {
        return(hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portAutoTuneStopFunc(devNum, portGroup, phyPortNum, portMode, stopRx,stopTx));
    }

   /* perform UnReset for Serdes(RF), PCS and MAC units BEFORE TRX-Training is stopped */
   CHECK_STATUS(mvHwsPortPostTraining(devNum, portGroup, phyPortNum, portMode));

   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode,curLanesList));
   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

   /* on each related serdes */
   for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < curPortParams->numOfActLanes); i++)
   {
           /* stop an appropriate engine */
           if (stopTx == GT_TRUE)
           {
                   CHECK_STATUS(mvHwsSerdesTxAutoTuneStop(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                           (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum)));
           }

           if (stopRx == GT_TRUE)
           {
                   CHECK_STATUS(mvHwsSerdesRxAutoTuneStart(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                           (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), GT_FALSE));
           }
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortFecCofig
*
* DESCRIPTION:
*       Configure FEC disable/enable on port.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portFecEn  - GT_TRUE for FEC enable, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFecCofig
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_BOOL portFecEn
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
       return GT_BAD_PARAM;
   }

#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortFecConfigIpc(devNum,portGroup, phyPortNum,portMode, portFecEn));
#endif

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   CHECK_STATUS(mvHwsPcsReset(devNum, portGroup, curPortParams->portPcsNumber,
                                   curPortParams->portPcsType, RESET));

   CHECK_STATUS(mvHwsPcsFecConfig(devNum,portGroup, curPortParams->portPcsNumber,
                                   curPortParams->portPcsType, portFecEn));

   CHECK_STATUS(mvHwsPcsReset(devNum, portGroup, curPortParams->portPcsNumber,
                                   curPortParams->portPcsType, UNRESET));

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortFecCofigGet
*
* DESCRIPTION:
*       Return FEC status disable/enable on port.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       portFecEn  - GT_TRUE for FEC enable, GT_FALSE otherwise
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFecCofigGet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_BOOL *portFecEn
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortFecConfigGetIpc(devNum,portGroup, phyPortNum,portMode, portFecEn));
#endif

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

   return mvHwsPcsFecConfigGet(devNum,portGroup, curPortParams->portPcsNumber,
                                   curPortParams->portPcsType, portFecEn);
}

/*******************************************************************************
* mvHwsPortFixAlign90
*
* DESCRIPTION:
*       Run fix Align90 process on current port.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFixAlign90
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_U32 dummyForCompilation
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   MV_HWS_ALIGN90_PARAMS serdesParams[HWS_MAX_SERDES_NUM];
   GT_U32  curLanesList[HWS_MAX_SERDES_NUM];
   MV_HWS_AUTO_TUNE_STATUS status[HWS_MAX_SERDES_NUM];
   GT_BOOL allLanesPass;
   GT_U32 i, loopNum;

   /* avoid warnings */
   dummyForCompilation = dummyForCompilation;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   hwsOsMemSetFuncPtr(serdesParams, 0, sizeof(serdesParams));
   hwsOsMemSetFuncPtr(status, TUNE_NOT_COMPLITED, sizeof(status));

   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   /* start Align90 on each related serdes */
   for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < curPortParams->numOfActLanes); i++)
   {
           mvHwsSerdesFixAlign90Start(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                      (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &(serdesParams[i]));
   }

   loopNum = 0;
   do
   {
           allLanesPass = GT_TRUE;
           hwsOsExactDelayPtr(devNum, portGroup, align90Delay);
           /* check status on each related serdes */
           for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < curPortParams->numOfActLanes); i++)
           {
                   mvHwsSerdesFixAlign90Status(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                               (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &status[i]);
                   if (status[i] != TUNE_PASS)
                   {
                           DEBUG_HWS_FULL(0, ("\nFix Align90 failed on serdes %d (device %d).",(curLanesList[i] & 0xFFFF), devNum));
                           allLanesPass = GT_FALSE;
                   }
           }
           loopNum++;
   } while ((!allLanesPass) && (loopNum < 10));

   /* stop Align90 on each related serdes */
   for (i = 0; (i < HWS_MAX_SERDES_NUM) && (i < curPortParams->numOfActLanes); i++)
   {
           mvHwsSerdesFixAlign90Stop(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                           (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &(serdesParams[i]),
                                           ((status[i] == TUNE_PASS) ? GT_TRUE : GT_FALSE));
           mvHwsSerdesRev2DfeCheck(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                           (curLanesList[i] & 0xFFFF), (MV_HWS_SERDES_TYPE)HWS_DEV_SERDES_TYPE(devNum));
   }

   if (allLanesPass)
   {
           return GT_OK;
   }

   return GT_FAIL;
}

/*******************************************************************************
* mvHwsPortFixAlign90Start
*
* DESCRIPTION:
*       Start fix Align90 process on current port.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFixAlign90Start
(
   GT_U8               devNum,
   GT_U32              portGroup,
   GT_U32              phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_ALIGN90_PARAMS   *serdesParams
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 numOfActLanes;
   GT_U32 i;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   numOfActLanes = curPortParams->numOfActLanes;
   if (numOfActLanes > HWS_MAX_SERDES_NUM)
   {
           return GT_BAD_PARAM;
   }

   /* start Align90 on each related serdes */
   for (i = 0; i < numOfActLanes; i++)
   {
           DEBUG_HWS_FULL(2, ("Start Align90 for Serdes %d\n", curLanesList[i] & 0xFFFF));

           CHECK_STATUS(mvHwsSerdesFixAlign90Start(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                           (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &(serdesParams[i])));
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortFixAlign90Stop
*
* DESCRIPTION:
*       Stop fix Align90 process on current port.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFixAlign90Stop
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_ALIGN90_PARAMS   *serdesParams
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 numOfActLanes;
   GT_U32 i;
   MV_HWS_AUTO_TUNE_STATUS status[HWS_MAX_SERDES_NUM];

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   numOfActLanes = curPortParams->numOfActLanes;

   /* stop Align90 on each related serdes */
   for (i = 0; i < numOfActLanes; i++)
   {
           DEBUG_HWS_FULL(2, ("Fix Align90 stop for serdes %d (device %d)\n", (curLanesList[i] & 0xFFFF), devNum));


           CHECK_STATUS(mvHwsSerdesFixAlign90Status(devNum,(portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                           (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &status[i]));

           CHECK_STATUS(mvHwsSerdesFixAlign90Stop(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                   (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &(serdesParams[i]),
                                                   ((status[i] == TUNE_PASS) ? GT_TRUE : GT_FALSE)));
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortFixAlign90Status
*
* DESCRIPTION:
*       Run fix Align90 process on current port.
*       Can be run any time after create port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       statusPtr - do all lanes of port passed align90 successfully
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFixAlign90Status
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_AUTO_TUNE_STATUS *statusPtr
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 numOfActLanes;
   GT_U32 i;
   MV_HWS_AUTO_TUNE_STATUS status[HWS_MAX_SERDES_NUM];

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));
   numOfActLanes = curPortParams->numOfActLanes;

   *statusPtr = TUNE_PASS;
   /* check status on each related serdes */
   for (i = 0; i < numOfActLanes; i++)
   {
           CHECK_STATUS(mvHwsSerdesFixAlign90Status(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                           (curLanesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &status[i]));

           if (status[i] != TUNE_PASS)
           {
                   if (status[i] == TUNE_FAIL)
                   {
                           *statusPtr = TUNE_FAIL;
                           DEBUG_HWS_FULL(2, ("Fix Align90 failed for serdes %d (device %d)\n", (curLanesList[i] & 0xFFFF), devNum));

                           /* if one lane failed no reason to continue */
                           return GT_OK;
                   }
                   else if (status[i] == TUNE_NOT_COMPLITED)
                   {
                           *statusPtr = TUNE_NOT_COMPLITED;

                           DEBUG_HWS_FULL(2, ("Fix Align90 not completed for serdes %d (device %d)\n", (curLanesList[i] & 0xFFFF), devNum));
                   }
           }
   }

   return GT_OK;
}

/*******************************************************************************
* hwsPortFixAlign90Flow
*
* DESCRIPTION:
*       Run Align90 flow.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       optAlgoMask- bit mask of optimization algorithms
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS hwsPortFixAlign90Flow
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  *serdesList,
   GT_U32  numOfActLanes,
   GT_BOOL *allLanesPass
)
{
   GT_U32 i;
   GT_U32 loopNum;
   MV_HWS_ALIGN90_PARAMS serdesParams[HWS_MAX_SERDES_NUM];
   MV_HWS_AUTO_TUNE_STATUS status[HWS_MAX_SERDES_NUM];

   if (numOfActLanes > HWS_MAX_SERDES_NUM)
   {
           return GT_BAD_PARAM;
   }

   hwsOsMemSetFuncPtr(serdesParams, 0, sizeof(serdesParams));
   hwsOsMemSetFuncPtr(status, TUNE_NOT_COMPLITED, sizeof(status));

   /* start Align90 on each related serdes */
   for (i = 0; i < numOfActLanes; i++)
   {
           if (serdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
           {
                   continue;
           }

           DEBUG_HWS_FULL(2, ("Start Align90 for Serdes %d\n", serdesList[i] & 0xFFFF));

           mvHwsSerdesFixAlign90Start(devNum, (portGroup + ((serdesList[i] >> 16) & 0xFF)),
                                      (serdesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &(serdesParams[i]));
   }

   loopNum = 0;

   do
   {
           *allLanesPass = GT_TRUE;
           hwsOsExactDelayPtr(devNum, portGroup, align90Delay);
           /* check status on each related serdes */
           for (i = 0; i < numOfActLanes; i++)
           {
                   if ((serdesList[i] == MV_HWS_SERDES_NOT_ACTIVE) || (status[i] == TUNE_PASS))
                   {
                           continue;
                   }

                   mvHwsSerdesFixAlign90Status(devNum, (portGroup + ((serdesList[i] >> 16) & 0xFF)),
                                                   (serdesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &status[i]);

                   if (status[i] != TUNE_PASS)
                   {
                           *allLanesPass = GT_FALSE;

                           if (status[i] == TUNE_FAIL)
                           {
                                   DEBUG_HWS_FULL(2, ("Fix Align90 failed for serdes %d (device %d)\n",
                                                           (serdesList[i] & 0xFFFF), devNum));
                           }
                           else if (status[i] == TUNE_NOT_COMPLITED)
                           {
                                   DEBUG_HWS_FULL(2, ("Fix Align90 no completed for serdes %d (device %d)\n",
                                                           (serdesList[i] & 0xFFFF), devNum));
                           }
                   }
           }

           loopNum++;

   } while ((!(*allLanesPass)) && (loopNum < 10));

   /* stop Align90 on each related serdes */
   for (i = 0; i < numOfActLanes; i++)
   {
           if (serdesList[i] == MV_HWS_SERDES_NOT_ACTIVE)
           {
                   continue;
           }

           mvHwsSerdesFixAlign90Stop(devNum, (portGroup + ((serdesList[i] >> 16) & 0xFF)),
                                           (serdesList[i] & 0xFFFF), HWS_DEV_SERDES_TYPE(devNum), &(serdesParams[i]),
                                           ((status[i] == TUNE_PASS) ? GT_TRUE : GT_FALSE));

           /* after we stop the Serdes, we can mark it as not active */
           if (status[i] == TUNE_PASS)
           {
                   DEBUG_HWS_FULL(2, ("Align90 completed for Serdes %d\n", serdesList[i] & 0xFFFF));
                   serdesList[i] = MV_HWS_SERDES_NOT_ACTIVE;
           }
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortAutoTuneOptimization
*
* DESCRIPTION:
*       Run optimization for the training results.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       optAlgoMask- bit mask of optimization algorithms
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortAutoTuneOptimization
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_U32  optAlgoMask
)
{
   MV_HWS_PORT_MAN_TUNE_MODE       tuningMode;
   GT_BOOL isTrainingMode;
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32  curLanesList[HWS_MAX_SERDES_NUM];

   CHECK_STATUS(hwsPortGetTuneMode(portMode, &tuningMode, &isTrainingMode));
   if (isTrainingMode == GT_FALSE)
   {
           return GT_BAD_PARAM;
   }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   return mvHwsComHRev2SerdesTrainingOptimization(devNum, portGroup, curLanesList,
                                                   curPortParams->numOfActLanes, tuningMode, optAlgoMask);
}

/*******************************************************************************
* hwsPortGetTuneMode
*
* DESCRIPTION:
*       returns the tune mode according to port mode..
*
* INPUTS:
*       portMode   - port standard metric
*
* OUTPUTS:
*       Tune mode (Long/Short reach)
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS hwsPortGetTuneMode
(
   MV_HWS_PORT_STANDARD    portMode,
   MV_HWS_PORT_MAN_TUNE_MODE       *tuneMode,
   GT_BOOL *isTrainingMode
)
{
   *isTrainingMode = GT_TRUE;

   if (IS_KR_MODE(portMode))
   {
           /*osPrintf("Long Reach\n");*/
           *tuneMode = StaticLongReach;
   }
   else if (IS_SR_MODE(portMode))
   {
           /*osPrintf("Short Reach\n");*/
           *tuneMode = StaticShortReach;
   }
   else
   {
           /*osPrintf("Not a training mode\n");*/
           *isTrainingMode = GT_FALSE;
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortInterfaceGet
*
* DESCRIPTION:
*       Gets Interface mode and speed of a specified port.
*
*
* INPUTS:
*       devNum   - physical device number
*       portGroup - core number
*       phyPortNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       portModePtr - interface mode
*
* RETURNS:
*       GT_OK               - on success
*       GT_BAD_PARAM        - on wrong port number or device
*       GT_BAD_PTR          - one of the parameters is NULL pointer
*       GT_HW_ERROR         - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS mvHwsPortInterfaceGet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    *portModePtr
)
{
   MV_HWS_SERDES_SPEED rate;
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32  curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 pcsLaneNums, macLaneNums;
   GT_U32 i;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) || (portModePtr == NULL))
   {
           return GT_BAD_PARAM;
   }

#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
           return (mvHwsPortInterfaceGetIpc(devNum,portGroup, phyPortNum,portModePtr));
#endif
   for (i = 0; i < hwsDeviceSpecInfo[devNum].lastSupPortMode; i++)
   {
           curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, i);

           if ((curPortParams->activeLanesList != NULL) && (curPortParams->serdesSpeed != SPEED_NA))
           {
                   /* rebuild active lanes list according to current configuration (redundancy) */
                   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum,
                                                   curPortParams->portStandard, curLanesList));

            /* get serdes rate */
            CHECK_STATUS(mvHwsSerdesSpeedGet(devNum,(portGroup + ((curLanesList[0] >> 16) & 0xFF)),(curLanesList[0] & 0xFFFF),
                                             HWS_DEV_SERDES_TYPE(devNum), &rate));

                   if ((rate == SPEED_NA) || (rate != curPortParams->serdesSpeed))
                   {
                           continue;
                   }

                   /* get number of active PCSs */
                   mvHwsPcsActiveStatusGet(devNum,portGroup,curPortParams->portPcsNumber, curPortParams->portPcsType, &pcsLaneNums);
                   if ((pcsLaneNums == 0) || (pcsLaneNums != curPortParams->numOfActLanes))
                   {
                           if ((curPortParams->portPcsType == HGLPCS) && ((rate == _3_125G) || (rate == _6_25G)))
                           {
                                   pcsLaneNums = 6;
                           }
                           else
                           {
                                   continue;
                           }
                   }

                   /* get number of active MACs */
                   mvHwsMacActiveStatusGet(devNum,portGroup,curPortParams->portMacNumber, curPortParams->portMacType, &macLaneNums);
                   if (macLaneNums == 0)
                   {
                           continue;
                   }

                   *portModePtr = (MV_HWS_PORT_STANDARD)i;
                   return GT_OK;
           }
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortClearChannelCfg
*
* DESCRIPTION:
*       Configures MAC advanced feature  accordingly.
*       Can be run before create port or after delete port.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
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
GT_STATUS mvHwsPortClearChannelCfg
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_U32  txIpg,
   GT_U32  txPreamble,
   GT_U32  rxPreamble,
   MV_HWS_PORT_CRC_MODE    txCrc,
   MV_HWS_PORT_CRC_MODE    rxCrc
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           (portMode >= (MV_HWS_PORT_STANDARD)HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   CHECK_STATUS(mvHwsMacClearChannelCfg(devNum, portGroup,curPortParams->portMacNumber,
                                           curPortParams->portMacType,
                                           txIpg,txPreamble,rxPreamble,txCrc,rxCrc));

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortAcTerminationCfg
*
* DESCRIPTION:
*       Configures AC termination in all port serdes lanes according to mode.
*       Can be run after create port only.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portAcTermEn - enable or disable AC termination
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortAcTerminationCfg
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_BOOL portAcTermEn
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;
   GT_U32  curLanesList[HWS_MAX_SERDES_NUM];
   GT_U32 i;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   if (curPortParams->numOfActLanes == 0)
   {
           return GT_NOT_SUPPORTED;
   }

   /* rebuild active lanes list according to current configuration (redundancy) */
   CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

   for (i = 0; i < curPortParams->numOfActLanes; i++)
   {
           CHECK_STATUS(mvHwsSerdesAcTerminationCfg(devNum, (portGroup + ((curLanesList[i] >> 16) & 0xFF)),
                                                           (curLanesList[i] & 0xFFFF), (MV_HWS_SERDES_TYPE)(HWS_DEV_SERDES_TYPE(devNum)),
                                                           portAcTermEn));
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortCheckGearBox
*
* DESCRIPTION:
*       Check Gear Box Status on related lanes.
*       Can be run after create port only.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       laneLock  - true or false.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortCheckGearBox
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode,
   GT_BOOL *laneLock
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
           return GT_BAD_PARAM;
   }

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
   if (curPortParams->numOfActLanes == 0)
   {
           return GT_NOT_SUPPORTED;
   }

   CHECK_STATUS(mvHwsPcsCheckGearBox(devNum, portGroup,
                                           curPortParams->portPcsNumber,
                                           curPortParams->portPcsType,
                                           curPortParams->numOfActLanes,laneLock));

   return GT_OK;
}

/*******************************************************************************
* mvHwsExtendedPortSerdesTxIfSelectSet
*
* DESCRIPTION:
*       Configures the Serdes Tx interface selector for Extended Ports
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsExtendedPortSerdesTxIfSelectSet
(
   GT_U8   devNum,
   GT_U32  portGroup,
   GT_U32  phyPortNum,
   MV_HWS_PORT_STANDARD    portMode
)
{
   /* relevant for ports 25 and 27 */
   switch (phyPortNum)
   {
   case 25:
           switch (portMode)
           {
           case SGMII:
           case _1000Base_X:
           case _5GBase_DQX:
           case _10GBase_KR:
           case _10GBase_SR_LR:
           case _12_5GBase_KR:
           case _5GBaseR:
           case _12GBaseR:
                   CHECK_STATUS(mvHwsSerdesTxIfSelect(devNum, portGroup, 10,
                                                           (MV_HWS_SERDES_TYPE)(HWS_DEV_SERDES_TYPE(devNum)), 0));
                   break;
           default:
                   return GT_NOT_SUPPORTED;
           }
           break;
   case 27:
           switch (portMode)
           {
           case SGMII:
           case _1000Base_X:
           case _5GBase_DQX:
           case _10GBase_KR:
           case _10GBase_SR_LR:
           case _12_5GBase_KR:
           case _5GBaseR:
           case _12GBaseR:
           case _2_5GBase_QX:
                   CHECK_STATUS(mvHwsSerdesTxIfSelect(devNum, portGroup, 11,
                                                           (MV_HWS_SERDES_TYPE)(HWS_DEV_SERDES_TYPE(devNum)), 0));
                   break;
           case _5GBase_HX:
           case _20GBase_KR:
           case _20GBase_SR_LR:
                   CHECK_STATUS(mvHwsSerdesTxIfSelect(devNum, portGroup, 10,
                                                           (MV_HWS_SERDES_TYPE)(HWS_DEV_SERDES_TYPE(devNum)), 1));
                   CHECK_STATUS(mvHwsSerdesTxIfSelect(devNum, portGroup, 11,
                                                           (MV_HWS_SERDES_TYPE)(HWS_DEV_SERDES_TYPE(devNum)), 0));
                   break;
           default:
                   return GT_NOT_SUPPORTED;
           }
           break;
   default:
           return GT_NOT_SUPPORTED;
   }

   return GT_OK;
}

/*******************************************************************************
* mvHwsPortUnitReset
*
* DESCRIPTION:
*       Reset/Unreset the MAC/PCS unit number of port.
*       For PMA (Serdes) unit, performs Power-Down for all port lanes
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       unitType    - Type of unit: MAC, PCS or PMA(Serdes)
*       action      - RESET:   Reset the MAC/PCS.  Power-down for PMA(Serdes)
*                     UNRESET: Unreset the MAC/PCS.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortUnitReset
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_UNIT             unitType,
    MV_HWS_RESET            action
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
    GT_U32 i;

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

    switch (unitType)
    {
        case HWS_MAC:
            CHECK_STATUS(mvHwsMacReset(devNum, portGroup, curPortParams->portMacNumber, curPortParams->portMacType, action));
            break;
        case HWS_PCS:
            CHECK_STATUS(mvHwsPcsReset(devNum, portGroup, curPortParams->portPcsNumber, curPortParams->portPcsType, action));
            break;
        case HWS_PMA: /* Serdes */
            if (action == UNRESET) /* for Serdes only Power-down is enabled */
            {
                return GT_NOT_SUPPORTED;
            }
            /* rebuild active lanes list according to current configuration (redundancy) */
            CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

            for (i = 0; i < curPortParams->numOfActLanes; i++)
            {
                /* for Serdes Power-Down operation, refClock and refClockSource values are not relevant */
                CHECK_STATUS(mvHwsSerdesPowerCtrl(devNum, portGroup, MV_HWS_SERDES_NUM(curLanesList[i]),
                                                  (MV_HWS_SERDES_TYPE)(HWS_DEV_SERDES_TYPE(devNum)), GT_FALSE, curPortParams->serdesSpeed,
                                                  _156dot25Mhz, PRIMARY, curPortParams->serdesMediaType, curPortParams->serdes10BitStatus));
            }
            break;
        default:
            return GT_NOT_SUPPORTED;
    }

    return GT_OK;
}

/*******************************************************************************
* mvHwsPortPcsActiveStatusGet
*
* DESCRIPTION:
*       Return number of PCS active lanes or 0 if PCS unit is under RESET.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       numOfLanes - number of lanes agregated in PCS
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortPcsActiveStatusGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_U32                  *numOfLanes
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 pcsLaneNums;

#ifndef MV_HWS_REDUCED_BUILD
    if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
        return (mvHwsPortPcsActiveStatusGetIpc(devNum,portGroup, phyPortNum, portMode, numOfLanes));
#endif

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

    CHECK_STATUS(mvHwsPcsActiveStatusGet(devNum, portGroup, curPortParams->portPcsNumber, curPortParams->portPcsType, &pcsLaneNums));

    *numOfLanes = pcsLaneNums;

    return GT_OK;
}

/*******************************************************************************
* mvHwsPortPsyncBypassCfg
*
* DESCRIPTION:
*       Configures the bypass of synchronization module.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortPsyncBypassCfg
(
    GT_U8                  devNum,
    GT_U32                 portGroup,
    GT_U32                 phyPortNum,
    MV_HWS_PORT_STANDARD   portMode
)
{
    MV_HWS_PORT_INIT_PARAMS   *curPortParams;
    GT_UOPT macNum;

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
    if (curPortParams == NULL)
    {
        return GT_NOT_SUPPORTED;
    }

    macNum = curPortParams->portMacNumber;

    if (portMode == SGMII2_5)
    {
        /* set P Sync Bypass */
        CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, macNum,
            GIG_PORT_MAC_CONTROL_REGISTER4, (1 << 6), (1 << 6)));
        return GT_OK;
    }

    /* unset P Sync Bypass */
    CHECK_STATUS(genUnitRegisterSet(devNum, portGroup, GEMAC_UNIT, curPortParams->portMacNumber,
        GIG_PORT_MAC_CONTROL_REGISTER4, 0, (1 << 6)));

    return GT_OK;
}

#if 0
/*******************************************************************************
* mvHwsPortBetterAdaptationSet
*
* DESCRIPTION:
*       Run the better Align90 adaptation algorithm on specific port number.
*       The function returns indication if the Better adaptation algorithm
*       succeeded to run on port or not.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       betterAlign90 - indicates if the Better adaptation algorithm run on
*                  specific port or not
*
* RETURNS:  GT_OK   - on success
*           GT_FAIL - on error
*******************************************************************************/
GT_STATUS mvHwsPortBetterAdaptationSet
(
    GT_U8                   devNum,
    GT_U32                     portGroup,
    GT_U32                     phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                    *betterAlign90
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
    MV_HWS_DEV_FUNC_PTRS *hwsDevFunc;
    GT_U32 i;

    hwsDeviceSpecGetFuncPtr(&hwsDevFunc);

    if (hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portBetterAdapSetFunc == NULL)
    {
        return GT_OK;
    }

    *betterAlign90 = GT_FALSE;

    if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
        ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
    {
        return GT_BAD_PARAM;
    }

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

    /* rebuild active lanes list according to current configuration (redundancy) */
    CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

    /* on each related serdes */
    for (i = 0; i < curPortParams->numOfActLanes; i++)
    {
        /* find a better Align90 adaptation value when temperature on device is changed from cold->hot or from hot->cold */
        CHECK_STATUS(hwsDevFunc[HWS_DEV_SILICON_TYPE(devNum)].portBetterAdapSetFunc(devNum, portGroup, (curLanesList[i] & 0xFFFF), betterAlign90));
    }

    return GT_OK;
}
#endif

/*******************************************************************************
* mvHwsPortFlowControlStateSet
*
* DESCRIPTION:
*       Activates the port loopback modes.
*       Can be run only after create port not under traffic.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       fcState     - flow control state
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFlowControlStateSet
(
   GT_U8                           devNum,
   GT_U32                          portGroup,
   GT_U32                          phyPortNum,
   MV_HWS_PORT_STANDARD            portMode,
   MV_HWS_PORT_FLOW_CONTROL_ENT    fcState
)
{
   MV_HWS_PORT_INIT_PARAMS *curPortParams;

   if ((phyPortNum >= HWS_CORE_PORTS_NUM(devNum)) ||
           ((GT_U32)portMode >= HWS_DEV_PORT_MODES(devNum)))
   {
       return GT_BAD_PARAM;
   }

#ifndef MV_HWS_REDUCED_BUILD
   if (hwsDeviceSpecInfo[devNum].ipcConnType == HOST2SERVICE_CPU_IPC_CONNECTION)
       return (mvHwsPortFlowControlStateSetIpc(devNum,portGroup, phyPortNum,portMode,fcState));
#endif

   curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

   CHECK_STATUS(mvHwsMacFcStateCfg(devNum, portGroup, curPortParams->portMacNumber,
                                               curPortParams->portMacType, fcState));

   return GT_OK;
}

