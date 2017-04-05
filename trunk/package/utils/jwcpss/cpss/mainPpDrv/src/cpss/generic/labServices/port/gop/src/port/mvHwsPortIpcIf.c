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
* mvHwsHost2HwsIfWraper.c
*
* DESCRIPTION: Port external interface
*
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 55 $
******************************************************************************/

#include <common/siliconIf/mvSiliconIf.h>
#include <private/mvHwsPortPrvIf.h>
#include <private/mvHwsPortMiscIf.h>
#include <mvHwsPortApInitIf.h>
#include <mvHwsPortCtrlApInitIf.h>
#include <mvHwsIpcDefs.h>
#include <private/mvHwsPortApInitIfPrv.h>
#include <mvHwsIpcDefs.h>
#include <mvHwsPortCtrlApDefs.h>

#ifdef _VISUALC
#pragma warning( disable : 4204)
#endif
/**************************** Globals ****************************************************/



/**************************** Pre-Declaration ********************************************/

/*******************************************************************************
* mvHwsIpcSendPortParams
*
* DESCRIPTION:
*       Sends to HWS request to set port parameters.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsIpcSendPortParams
(
	GT_U8                    devNum,
	GT_U32                   portGroup,
	GT_U32                   phyPortNum,
	MV_HWS_PORT_STANDARD     portMode
)
{
	GT_U8 i;
	MV_HWS_PORT_INIT_PARAMS *curPortParams;
	MV_HWS_IPC_CTRL_MSG_STRUCT requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_PARMAS_DATA_STRUCT portParams;

	/* get port params */
	curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
	if (curPortParams == NULL)
	{
		osPrintf("mvHwsIpcSendPortParams: portMode %d for port %d is not supported\n", portMode, phyPortNum);
		return GT_NOT_SUPPORTED;
	}

	/* fill IPC message data */
	portParams.portGroup = portGroup;
	portParams.phyPortNum = phyPortNum;
	portParams.portStandard = portMode;
	portParams.portMacType = curPortParams->portMacType;
	portParams.portMacNumber = curPortParams->portMacNumber;
	portParams.portPcsType = curPortParams->portPcsType;
	portParams.portPcsNumber = curPortParams->portPcsNumber;
	portParams.portFecMode = curPortParams->portFecMode;
	portParams.serdesSpeed = curPortParams->serdesSpeed;
	portParams.firstLaneNum = (GT_U8)curPortParams->firstLaneNum;
	portParams.numOfActLanes = (GT_U8)curPortParams->numOfActLanes;
	/* set active lane list*/
	for(i = 0; i< curPortParams->numOfActLanes; i++)
	{
		portParams.activeLanesList[i] = (GT_U8)curPortParams->activeLanesList[i];
	}
	portParams.serdesMediaType = curPortParams->serdesMediaType;
	portParams.serdes10BitStatus = curPortParams->serdes10BitStatus;

    /* Construct the msg */
    requestMsg.msgData.portParams = portParams;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_PARAMS_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortInitIpc
*
* DESCRIPTION:
*       Sends to HWS request to init physical port.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       lbPort      - if true, init port without serdes activity
*       refClock    - Reference clock frequency
*       refClockSrc - Reference clock source line
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortInitIpc
(
    GT_U8                    devNum,
    GT_U32                   portGroup,
    GT_U32                   phyPortNum,
    MV_HWS_PORT_STANDARD     portMode,
    GT_BOOL                  lbPort,
    MV_HWS_REF_CLOCK_SUP_VAL refClock,
    MV_HWS_REF_CLOCK_SOURCE  refClockSource)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_INIT_DATA_STRUCT portInit =
        {portGroup, phyPortNum, portMode, lbPort, refClock, refClockSource};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portInit = portInit;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INIT_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_INIT_MSG));

    return (replyData.returnCode);
}
/*******************************************************************************
* mvHwsPortResetIpc
*
* DESCRIPTION:
*       Sends to HWS request to power down or reset physical port.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       lbPort     - if true, init port without serdes activity
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
GT_STATUS mvHwsPortResetIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
    MV_HWS_PORT_ACTION   action
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_RESET_DATA_STRUCT portReset = {portGroup, phyPortNum, portMode, action};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portReset = portReset;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_RESET_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_RESET_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortAutoTuneStateCheckIpc
*
* DESCRIPTION:
*
* INPUTS:
*       devNum         - system device number
*       portGroup      - port group (core) number
*       phyPortNum     - physical port number
*       portMode       - port standard metric
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
GT_STATUS mvHwsPortAutoTuneStateCheckIpc
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_AUTO_TUNE_STATUS *rxTune,
    MV_HWS_AUTO_TUNE_STATUS *txTune
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_INFO_STRUCT portAutoTuneStateChk = {portGroup, phyPortNum, portMode};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    requestMsg.msgData.portAutoTuneStateChk = portAutoTuneStateChk;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_AUTO_TUNE_STATE_CHK_MSG));

    /* Update reply parameters */
    *rxTune = replyData.readData.portAutoTuneStateChk.rxTune;
    *txTune = replyData.readData.portAutoTuneStateChk.txTune;

    return (replyData.returnCode);
}
/*******************************************************************************
* mvHwsPortLinkStatusGet
*
* DESCRIPTION:
*       Returns port link status.
*       Can run at any time.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
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
GT_STATUS mvHwsPortLinkStatusGetIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
    GT_BOOL              *linkStatus
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_INFO_STRUCT portLinkStatus = {portGroup, phyPortNum, portMode};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portLinkStatus = portLinkStatus;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_LINK_STATUS_GET_MSG));

    /* Update reply parameters */
    *linkStatus = replyData.readData.portStatusGet.status;

   return (replyData.returnCode);
}
/*******************************************************************************
* mvHwsPortAutoTuneSetExtIpc
*
* DESCRIPTION:
*
* INPUTS:
*       devNum         - system device number
*       portGroup      - port group (core) number
*       phyPortNum     - physical port number
*       portMode       - port standard metric
*       portTuningMode - port TX related tuning mode
*       optAlgoMask    - bit mask for optimization algorithms
*       portTuningMode - port tuning mode
*
* OUTPUTS:
*       Tuning results for recommended settings.
*
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortAutoTuneSetExtIpc
(
    GT_U8                      devNum,
    GT_U32                     portGroup,
    GT_U32                     phyPortNum,
    MV_HWS_PORT_STANDARD       portMode,
    MV_HWS_PORT_AUTO_TUNE_MODE portTuningMode,
    GT_U32                     optAlgoMask,
    void                       *results
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_AUTO_TUNE_SET_EXT_DATA_STRUCT portAutoTuneSetExt =
        {portGroup, phyPortNum, portMode, portTuningMode, optAlgoMask};

    results = results; /* to avoid warning */

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portAutoTuneSetExt = portAutoTuneSetExt;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_AUTO_TUNE_SET_EXT_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_AUTO_TUNE_SET_EXT_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortAutoTuneStopIpc
*
* DESCRIPTION:
*       Send IPC message to stop Tx and Rx training
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       stopRx     - stop RX
*       stopTx     - stop Tx
*
* OUTPUTS:
*       None
*
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortAutoTuneStopIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
	GT_BOOL              stopRx,
	GT_BOOL              stopTx
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_AUTO_TUNE_STOP_DATA_STRUCT portAutoTuneStop =
        {portGroup, phyPortNum, portMode, stopRx, stopTx};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portAutoTuneStop = portAutoTuneStop;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_AUTO_TUNE_STOP_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_AUTO_TUNE_STOP_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsSerdesManualRxConfigIpc
*
* DESCRIPTION:
*       Send IPC message to configure SERDES Rx parameters for all SERDES lanes.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*                    config params: sqlch,ffeRes,ffeCap,dfeEn,alig
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsSerdesManualRxConfigIpc
(
    GT_U8                     devNum,
    GT_U32                    portGroup,
    GT_U32                    serdesNum,
	MV_HWS_SERDES_TYPE        serdesType,
    MV_HWS_PORT_MAN_TUNE_MODE portTuningMode,
    GT_U32                    sqlch,
    GT_U32                    ffeRes,
    GT_U32                    ffeCap,
    GT_BOOL                   dfeEn,
    GT_U32                    alig
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;

    MV_HWS_IPC_PORT_SERDES_MANUAL_RX_CONFIG_DATA_STRUCT serdesManualRxConfig =
        {portGroup, serdesNum, serdesType, portTuningMode, sqlch, ffeRes, ffeCap, dfeEn, alig};

    /* Construct the msg */
    requestMsg.msgData.serdesManualRxConfig = serdesManualRxConfig;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_SERDES_MANUAL_RX_CONFIG_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_SERDES_MANUAL_RX_CONFIG_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsSerdesManualTxConfigIpc
*
* DESCRIPTION:
*   	Send IPC message to configure SERDES Tx parameters for all SERDES lanes.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*                    config params: txAmp,txAmpAdj,emph0,emph1,txAmpShft
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*******************************************************************************/
GT_STATUS mvHwsSerdesManualTxConfigIpc
(
    GT_U8				devNum,
    GT_U32				portGroup,
    GT_U32				serdesNum,
	MV_HWS_SERDES_TYPE  serdesType,
    GT_U32				txAmp,
    GT_BOOL				txAmpAdj,
    GT_U32				emph0,
    GT_U32				emph1,
    GT_BOOL				txAmpShft
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;

    MV_HWS_IPC_PORT_SERDES_MANUAL_TX_CONFIG_DATA_STRUCT serdesManualTxConfig =
        {portGroup, serdesNum, serdesType, txAmp, txAmpAdj, emph0, emph1, txAmpShft};

    /* Construct the msg */
    requestMsg.msgData.serdesManualTxConfig = serdesManualTxConfig;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                               sizeof(MV_HWS_IPC_PORT_SERDES_MANUAL_TX_CONFIG_DATA_STRUCT),
                                               MV_HWS_IPC_PORT_SERDES_MANUAL_TX_CONFIG_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortPolaritySetIpc
*
* DESCRIPTION:
*       Send message to set the port polarity of the Serdes lanes (Tx/Rx).
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
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
GT_STATUS mvHwsPortPolaritySetIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
    GT_32                txInvMask,
    GT_32                rxInvMask
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_POLARITY_SET_DATA_STRUCT portPolaritySet =
        {portGroup,phyPortNum,portMode,txInvMask,rxInvMask};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portPolaritySet = portPolaritySet;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_POLARITY_SET_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_POLARITY_SET_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortFecConfigIpc
*
* DESCRIPTION:
*       Send message to configure FEC disable/enable on port.
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
GT_STATUS mvHwsPortFecConfigIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
    GT_BOOL              portFecEn
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_FEC_CONFIG_DATA_STRUCT portFecConfig = {portGroup, phyPortNum, portMode, portFecEn};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portFecConfig = portFecConfig;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_FEC_CONFIG_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_FEC_CONFIG_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortFecConfigGetIpc
*
* DESCRIPTION:
*       Send message to get  FEC status.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
       FEC status (true/false).
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortFecConfigGetIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
    GT_BOOL              *portFecEn
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_INFO_STRUCT portFecConfigGet = {portGroup, phyPortNum, portMode};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portFecConfigGet = portFecConfigGet;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_FEC_CONFIG_GET_MSG));

    /* Update reply parameters */
    *portFecEn = replyData.readData.portStatusGet.status;

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortTxEnable
*
* DESCRIPTION:
*       Turn of the port Tx according to selection.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
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
GT_STATUS mvHwsPortTxEnableIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
    GT_BOOL              enable
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT            requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT           replyData;
    MV_HWS_IPC_PORT_TX_ENABLE_DATA_STRUCT portTxEnableData = {portGroup,phyPortNum,portMode,enable};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portTxEnableData = portTxEnableData;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_TX_ENABLE_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_TX_ENABLE_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortTxEnableGetIpc
*
* DESCRIPTION:
*       Retrieve the status of all port serdeses.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
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
GT_STATUS mvHwsPortTxEnableGetIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
    GT_BOOL              serdesTxStatus[]
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT  requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_INFO_STRUCT portTxEnableGet = {portGroup, phyPortNum, portMode};
    MV_HWS_PORT_INIT_PARAMS     *curPortParams;
    GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
    GT_U32 i;

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portTxEnableGet = portTxEnableGet;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_TX_ENABLE_GET_MSG));

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
    /* rebuild active lanes list according to current configuration (redundancy) */
    CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

    /* on each related serdes */
    for (i = 0; i < curPortParams->numOfActLanes; i++)
    {
        serdesTxStatus[i] = replyData.readData.portSerdesTxEnableGet.status[i];
    }

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortSignalDetectGetIpc
*
* DESCRIPTION:
*       Retrieve the status of all port serdeses.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       SignalDetect per serdes.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortSignalDetectGetIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
    GT_BOOL              signalDetect[]
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT  requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_INFO_STRUCT portSignalDetectGet = {portGroup, phyPortNum, portMode};
    MV_HWS_PORT_INIT_PARAMS     *curPortParams;
    GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
    GT_U32 i;

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portSignalDetectGet = portSignalDetectGet;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_SIGNAL_DETECT_GET_MSG));

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);

    /* rebuild active lanes list according to current configuration (redundancy) */
    CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

    /* on each related serdes */
    for (i = 0; i < curPortParams->numOfActLanes; i++)
    {
        signalDetect[i] = replyData.readData.portSerdesSignalDetectGet.status[i];
    }

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortCdrLockStatusGetIpc
*
* DESCRIPTION:
*       Send message to get the CDR lock status of all port serdeses.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*
* OUTPUTS:
*       SignalDetect per serdes.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortCdrLockStatusGetIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
    GT_BOOL              cdrLockStatus[]
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT  requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_INFO_STRUCT portCdrLockStatus = {portGroup, phyPortNum, portMode};
    MV_HWS_PORT_INIT_PARAMS     *curPortParams;
    GT_U32 curLanesList[HWS_MAX_SERDES_NUM];
    GT_U32 i;

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portCdrLockStatus = portCdrLockStatus;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_CDR_LOCK_STATUS_GET_MSG));

    curPortParams = hwsPortModeParamsGet(devNum, portGroup, phyPortNum, portMode);
    /* rebuild active lanes list according to current configuration (redundancy) */
    CHECK_STATUS(mvHwsRebuildActiveLaneList(devNum, portGroup, phyPortNum, portMode, curLanesList));

    /* on each related serdes */
    for (i = 0; i < curPortParams->numOfActLanes; i++)
    {
        cdrLockStatus[i] = replyData.readData.portSerdesCdrLockStatusGet.status[i];
    }

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortLoopbackSetIpc
*
* DESCRIPTION:
*       Send message to activates the port loopback modes.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
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

*******************************************************************************/
GT_STATUS mvHwsPortLoopbackSetIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
    MV_HWS_UNIT          lpPlace,
    MV_HWS_PORT_LB_TYPE  lbType
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT               requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT              replyData;
    MV_HWS_IPC_PORT_LOOPBACK_SET_DATA_STRUCT portLoopbackSet =
        {portGroup, phyPortNum, portMode, lpPlace, lbType};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portLoopbackSet = portLoopbackSet;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_LOOPBACK_SET_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_LOOPBACK_SET_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortFlowControlStateSetIpc
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
GT_STATUS mvHwsPortFlowControlStateSetIpc
(
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          phyPortNum,
    MV_HWS_PORT_STANDARD            portMode,
    MV_HWS_PORT_FLOW_CONTROL_ENT    fcState
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT               requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT              replyData;
    MV_HWS_IPC_PORT_FLOW_CONTROL_SET_DATA_STRUCT portFcStateSet =
        {portGroup, phyPortNum, portMode, fcState};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portFcStateSet = portFcStateSet;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_FLOW_CONTROL_SET_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_FC_STATE_SET_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortLoopbackStatusGetIpc
*
* DESCRIPTION:
*       Send IPC message to retrive MAC loopback status.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
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
GT_STATUS mvHwsPortLoopbackStatusGetIpc
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_UNIT             lpPlace,
    MV_HWS_PORT_LB_TYPE     *lbType
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT               requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT              replyData;
    MV_HWS_IPC_PORT_LOOPBACK_GET_DATA_STRUCT portLoopbackGet =
        {portGroup, phyPortNum, portMode, lpPlace};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portLoopbackGet = portLoopbackGet;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_LOOPBACK_GET_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_LOOPBACK_STATUS_GET_MSG));

    *lbType = replyData.readData.portLoopbackStatusGet.lbType;

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortPPMSetIpc
*
* DESCRIPTION:
*       Send IPC message to increase/decrease  Tx clock on port (added/sub ppm).
*       Can be run only after create port not under traffic.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
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
GT_STATUS mvHwsPortPPMSetIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
    MV_HWS_PPM_VALUE     portPPM
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT          requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT         replyData;
    MV_HWS_IPC_PORT_PPM_SET_DATA_STRUCT portPPMSet = {portGroup, phyPortNum, portMode, portPPM};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portPPMSet = portPPMSet;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_PPM_SET_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_PPM_SET_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortPPMGetIpc
*
* DESCRIPTION:
*       Send message to check the entire line configuration
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
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
GT_STATUS mvHwsPortPPMGetIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD portMode,
    MV_HWS_PPM_VALUE     *portPPM
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT  requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_INFO_STRUCT portPPMGet = {portGroup, phyPortNum, portMode};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portPPMGet = portPPMGet;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_PPM_GET_MSG));

    *portPPM = replyData.readData.portPpmGet.portPpm;

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortInterfaceGetIpc
*
* DESCRIPTION:
*       Send message to gets Interface mode and speed of a specified port.
*
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - core number
*       phyPortNum - physical port number (or CPU port)
*
* OUTPUTS:
*       portModePtr - interface mode
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortInterfaceGetIpc
(
    GT_U8                devNum,
    GT_U32               portGroup,
    GT_U32               phyPortNum,
    MV_HWS_PORT_STANDARD *portModePtr
)
{
#if 0
    MV_HWS_IPC_CTRL_MSG_STRUCT         requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT        replyData;
    MV_HWS_IPC_PORT_IF_GET_DATA_STRUCT portInterfaceGet = {portGroup, phyPortNum};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /* Construct the msg */
    requestMsg.msgData.portInterfaceGet = portInterfaceGet;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_IF_GET_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_IF_GET_MSG));

    *portModePtr = replyData.readData.portIfGet.portIf;

    return (replyData.returnCode);
#else
    /* avoid warnings */
    devNum      = devNum;
    portGroup   = portGroup;
    phyPortNum  = phyPortNum;
    portModePtr = portModePtr;

    /* this functionality cannot be ran in internal cpu since it requires port
        elements info for all port modes which doesn't exist in internal CPU */
    return GT_NOT_SUPPORTED;
#endif
}

/*******************************************************************************
* mvHwsPortPcsActiveStatusGetIpc
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
GT_STATUS mvHwsPortPcsActiveStatusGetIpc
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_U32                  *numOfLanes
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT                          requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT                         replyData;
    MV_HWS_IPC_PORT_PCS_ACTIVE_STATUS_GET_DATA_STRUCT   portPcsActiveStatus = {portGroup, phyPortNum, portMode};

    /* send port params (to internal CPU) */
    CHECK_STATUS(mvHwsIpcSendPortParams(devNum, portGroup, phyPortNum, portMode));

    /*construct the msg*/
    requestMsg.msgData.portPcsActiveStatus = portPcsActiveStatus;

    /* send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply (devNum,&requestMsg,&replyData,
                                                 sizeof(MV_HWS_IPC_PORT_PCS_ACTIVE_STATUS_GET_DATA_STRUCT),
                                                 MV_HWS_IPC_PORT_PCS_ACTIVE_STATUS_GET_CONFIG_MSG));

    *numOfLanes = replyData.readData.portReplyGet.results;

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortApStartIpc
*
* DESCRIPTION:
*       Send message to configure AP port parameters and start execution
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - core number
*       phyPortNum - physical port number
*       apCfg      - AP configuration parameters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortApStartIpc
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum,
    GT_U32 *apCfg
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT     requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT    replyData;
    MV_HWS_AP_CFG                  *localApCfg = (MV_HWS_AP_CFG*)apCfg;

    MV_HWS_IPC_PORT_AP_DATA_STRUCT apCfgIpc =
    {
        portGroup,                          /* portGroup  */
        phyPortNum,                         /* phyPortNum */
        (GT_U16)localApCfg->apLaneNum,      /* laneNum */
        (GT_U16)phyPortNum,                 /* pcsNum  */
        (GT_U16)phyPortNum,                 /* macNum  */
        (GT_U16)localApCfg->modesVector,    /* advMode */
        (GT_U16)localApCfg->fcPause       |
        (GT_U16)localApCfg->fcAsmDir << 1 |
        (GT_U16)localApCfg->fecSup   << 4 |
        (GT_U16)localApCfg->fecReq   << 5 |
        (GT_U16)localApCfg->nonceDis << 9   /* options  */
    };

    /* Construct the msg */
    requestMsg.msgData.apConfig = apCfgIpc;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_AP_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_AP_ENABLE_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortApStopIpc
*
* DESCRIPTION:
*       Send message to stop AP port execution
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - core number
*       phyPortNum - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortApStopIpc
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT     requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT    replyData;
    MV_HWS_IPC_PORT_AP_DATA_STRUCT apPortCfg = {portGroup, phyPortNum, 0, 0, 0, 0, 0};

    /* Construct the msg */
    requestMsg.msgData.apConfig = apPortCfg;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_AP_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_AP_DISABLE_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortApSysAckIpc
*
* DESCRIPTION:
*       Send message to notify AP state machine that port resource allocation
*       was executed by the Host and it can continue execution
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - core number
*       phyPortNum - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortApSysAckIpc
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT  requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_INFO_STRUCT apPortSysCfg = {portGroup, phyPortNum, NON_SUP_MODE};

    /* Construct the msg */
    requestMsg.msgData.apSysConfig = apPortSysCfg;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_AP_SYS_CFG_VALID_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortApCfgGetIpc
*
* DESCRIPTION:
*       Send message to retrive AP port configuration parameters
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - core number
*       phyPortNum - physical port number
*       apCfg      - AP configuration parameters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortApCfgGetIpc
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum,
    GT_U32 *apCfg
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT       requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT      replyData;
    MV_HWS_IPC_PORT_REPLY_AP_CFG_GET *apCfgIpc;
    MV_HWS_AP_CFG                    *apCfgReply = (MV_HWS_AP_CFG*)apCfg;
    MV_HWS_IPC_PORT_AP_DATA_STRUCT   apPortCfg = {portGroup, phyPortNum, 0, 0, 0, 0, 0};

    /* Construct the msg */
    requestMsg.msgData.apConfig = apPortCfg;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_AP_DATA_STRUCT),
                                                MV_HWS_IPC_PORT_AP_CFG_GET_MSG));

    apCfgIpc = &(replyData.readData.portApCfgGet);

    /* Update reply parameters */
    apCfgReply->apLaneNum   = AP_CTRL_LANE_GET(apCfgIpc->ifNum);
    apCfgReply->modesVector = (AP_CTRL_1000Base_KX_GET(apCfgIpc->capability) << _1000Base_KX_Bit0) |
                              (AP_CTRL_10GBase_KX4_GET(apCfgIpc->capability) << _10GBase_KX4_Bit1) |
                              (AP_CTRL_10GBase_KR_GET(apCfgIpc->capability)  << _10GBase_KR_Bit2)  |
                              (AP_CTRL_40GBase_KR4_GET(apCfgIpc->capability) << _40GBase_KR4_Bit3);
    apCfgReply->fcPause     = AP_CTRL_FC_PAUSE_GET(apCfgIpc->options);
    apCfgReply->fcAsmDir    = AP_CTRL_FC_ASM_GET(apCfgIpc->options);
    apCfgReply->fecSup      = AP_CTRL_FEC_ABIL_GET(apCfgIpc->options);
    apCfgReply->fecReq      = AP_CTRL_FEC_REQ_GET(apCfgIpc->options);
    apCfgReply->nonceDis    = AP_CTRL_LB_EN_GET(apCfgIpc->options);
    apCfgReply->refClockCfg.refClockFreq   = MHz_156;
    apCfgReply->refClockCfg.refClockSource = PRIMARY_LINE_SRC;

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortApStatusGetIpc
*
* DESCRIPTION:
*       Send message to retrive AP port status parameters
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - core number
*       phyPortNum - physical port number
*       apStatus   - AP Status parameters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortApStatusGetIpc
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum,
    GT_U32 *apStatus
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT          requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT         replyData;
    MV_HWS_IPC_PORT_REPLY_AP_STATUS_GET *apStatusIpc;
    MV_HWS_AP_PORT_STATUS               *apStatusReply = (MV_HWS_AP_PORT_STATUS*)apStatus;
    MV_HWS_IPC_PORT_INFO_STRUCT         apPortStatus = {portGroup, phyPortNum, NON_SUP_MODE};

    /* Construct the msg */
    requestMsg.msgData.apStatusGet = apPortStatus;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_AP_STATUS_MSG));

    apStatusIpc = &(replyData.readData.portApStatusGet);

    /* Update reply parameters */
    apStatusReply->apLaneNum                = apStatusIpc->laneNum;
    apStatusReply->smState                  = apStatusIpc->state;
    apStatusReply->smStatus                 = apStatusIpc->status;
    apStatusReply->arbStatus                = apStatusIpc->ARMSmStatus;
    apStatusReply->hcdResult.hcdFound       = AP_ST_HCD_FOUND_GET(apStatusIpc->hcdStatus);
    apStatusReply->hcdResult.hcdLinkStatus  = AP_ST_HCD_LINK_GET(apStatusIpc->hcdStatus);
    apStatusReply->hcdResult.hcdResult      = AP_ST_HCD_TYPE_GET(apStatusIpc->hcdStatus);
    apStatusReply->hcdResult.hcdFecEn       = AP_ST_HCD_FEC_RES_GET(apStatusIpc->hcdStatus);
    apStatusReply->hcdResult.hcdFcRxPauseEn = AP_ST_HCD_FC_RX_RES_GET(apStatusIpc->hcdStatus);
    apStatusReply->hcdResult.hcdFcTxPauseEn = AP_ST_HCD_FC_TX_RES_GET(apStatusIpc->hcdStatus);

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortApStatsGetIpc
*
* DESCRIPTION:
*       Send message to retrive AP port stats parameters
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - core number
*       phyPortNum - physical port number
*       apStats    - AP Stats parameters
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortApStatsGetIpc
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum,
    GT_U32 *apStats
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT         requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT        replyData;
    MV_HWS_IPC_PORT_REPLY_AP_STATS_GET *apStatsIpc;
    MV_HWS_AP_PORT_STATS               *apStatsReply = (MV_HWS_AP_PORT_STATS*)apStats;
    MV_HWS_IPC_PORT_INFO_STRUCT        apPortStats = {portGroup, phyPortNum, NON_SUP_MODE};

    /* Construct the msg */
    requestMsg.msgData.apStatsGet = apPortStats;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_AP_STATS_MSG));

    apStatsIpc = &(replyData.readData.portApStatsGet);

    /* Update reply parameters */
    apStatsReply->txDisCnt          = apStatsIpc->txDisCnt;
    apStatsReply->abilityCnt        = apStatsIpc->abilityCnt;
    apStatsReply->abilitySuccessCnt = apStatsIpc->abilitySuccessCnt;
    apStatsReply->linkFailCnt       = apStatsIpc->linkFailCnt;
    apStatsReply->linkSuccessCnt    = apStatsIpc->linkSuccessCnt;
    apStatsReply->hcdResoultionTime = apStatsIpc->hcdResoultionTime;
    apStatsReply->linkUpTime        = apStatsIpc->linkUpTime;

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortApStatsResetIpc
*
* DESCRIPTION:
*       Reset AP port statistics information
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortApStatsResetIpc
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT  requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_IPC_PORT_INFO_STRUCT apPortStats = {portGroup, phyPortNum, NON_SUP_MODE};

    /* Construct the msg */
    requestMsg.msgData.apStatsReset = apPortStats;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_AP_STATS_RESET_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortApIntropSetIpc
*
* DESCRIPTION:
*       Set AP port introp information
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       apintrop    - AP introp parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortApIntropSetIpc
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum,
    GT_U32 *apIntrop
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT  requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;
    MV_HWS_AP_PORT_INTROP       *localApIntropCfg = (MV_HWS_AP_PORT_INTROP*)apIntrop;

    MV_HWS_IPC_PORT_AP_INTROP_STRUCT apIntropIpc =
    {
        localApIntropCfg->attrBitMask,
        localApIntropCfg->txDisDuration,
        localApIntropCfg->abilityDuration,
        localApIntropCfg->abilityMaxInterval,
        localApIntropCfg->abilityFailMaxInterval,
        localApIntropCfg->apLinkDuration,
        localApIntropCfg->apLinkMaxInterval,
        localApIntropCfg->pdLinkDuration,
        localApIntropCfg->pdLinkMaxInterval
    };

    /* avoid warnings */
    portGroup  = portGroup;
    phyPortNum = phyPortNum;

    /* Construct the msg */
    requestMsg.msgData.apIntropSet = apIntropIpc;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_AP_INTROP_STRUCT),
                                                MV_HWS_IPC_PORT_AP_INTROP_SET_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortApIntropGetIpc
*
* DESCRIPTION:
*       Return AP port introp information
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       apintrop    - AP introp parameters
*
* OUTPUTS:
*       apintrop - AP introp parameters
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortApIntropGetIpc
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum,
    GT_U32 *apIntrop
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT          requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT         replyData;
    MV_HWS_IPC_PORT_REPLY_AP_INTROP_GET *apIntropIpc;
    MV_HWS_AP_PORT_INTROP               *apIntropReply = (MV_HWS_AP_PORT_INTROP*)apIntrop;
    MV_HWS_IPC_PORT_INFO_STRUCT         apPortIntrop = {portGroup, phyPortNum, NON_SUP_MODE};

    /* Construct the msg */
    requestMsg.msgData.apIntropGet = apPortIntrop;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_AP_INTROP_GET_MSG));

    apIntropIpc = &(replyData.readData.portApIntropGet);

    /* Update reply parameters */
    apIntropReply->txDisDuration          = apIntropIpc->txDisDuration;
    apIntropReply->abilityDuration        = apIntropIpc->abilityDuration;
    apIntropReply->abilityMaxInterval     = apIntropIpc->abilityMaxInterval;
    apIntropReply->abilityFailMaxInterval = apIntropIpc->abilityFailMaxInterval;
    apIntropReply->apLinkDuration         = apIntropIpc->apLinkDuration;
    apIntropReply->apLinkMaxInterval      = apIntropIpc->apLinkMaxInterval;
    apIntropReply->pdLinkDuration         = apIntropIpc->pdLinkDuration;
    apIntropReply->pdLinkMaxInterval      = apIntropIpc->pdLinkMaxInterval;

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortApDebugGetIpc
*
* DESCRIPTION:
*       Return AP debug information
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       apDebug    - AP debug parameters
*
* OUTPUTS:
*       apDebug - AP debug parameters
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortApDebugGetIpc
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum,
    GT_U32 *apDebug
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT          requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT         replyData;
    MV_HWS_IPC_PORT_REPLY_AP_DEBUG_GET *apDebugIpc;
    MV_HWS_AP_PORT_DEBUG               *apDebugReply = (MV_HWS_AP_PORT_DEBUG*)apDebug;
    MV_HWS_IPC_PORT_INFO_STRUCT         apPortDebug = {portGroup, phyPortNum, NON_SUP_MODE};

    /* Construct the msg */
    requestMsg.msgData.apDebugGet = apPortDebug;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_AP_DEBUG_GET_MSG));

    apDebugIpc = &(replyData.readData.portApDebugGet);

    /* Update reply parameters */
    apDebugReply->apFwBaseAddr       = apDebugIpc->fwBaseAddr;
    apDebugReply->apFwLogBaseAddr    = apDebugIpc->logBaseAddr;
    apDebugReply->apFwLogCountAddr   = apDebugIpc->logCountAddr;
    apDebugReply->apFwLogPointerAddr = apDebugIpc->logPointerAddr;
    apDebugReply->apFwLogResetAddr   = apDebugIpc->logResetAddr;

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortApDebugGetIpc
*
* DESCRIPTION:
*       Set AP state machine state when Avago GUI is enabled
*       Avago GUI access Avago Firmware as SBUS command level
*       Therefore it is required to stop the periodic behiviour of AP state
*       machine when Avago GUI is enabled
*
* INPUTS:
*       devNum - system device number
*       state  - Avago GUI state
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortApAvagoGuiSetIpc
(
    GT_U8 devNum,
    GT_U8 state
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT  requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT replyData;

    /* Construct the msg */
    requestMsg.msgData.avagoGuiSet.state = (GT_U32)state;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_AVAGO_GUI_STRUCT),
                                                MV_HWS_IPC_PORT_AVAGO_GUI_SET_MSG));

    return (replyData.returnCode);
}

/*******************************************************************************
* mvHwsPortAvagoCfgAddrGetIpc
*
* DESCRIPTION:
*       Return Avago Serdes Configuration structure address
*
* INPUTS:
*       devNum       - system device number
*       portGroup    - port group (core) number
*       phyPortNum - physical port number
*       avagoCfgAddr - Avago Serdes Configuration structure address
*
* OUTPUTS:
*       avagoCfgAddr - Avago Serdes Configuration structure address
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsPortAvagoCfgAddrGetIpc
(
    GT_U8  devNum,
    GT_U32 portGroup,
    GT_U32 phyPortNum,
    GT_U32 *avagoCfgAddr
)
{
    MV_HWS_IPC_CTRL_MSG_STRUCT   requestMsg;
    MV_HWS_IPC_REPLY_MSG_STRUCT  replyData;
    MV_HWS_IPC_PORT_REPLY_RESULT *avagoAddrIpc;
    MV_HWS_IPC_PORT_INFO_STRUCT  avagoAddr = {portGroup, phyPortNum, NON_SUP_MODE};

    /* Construct the msg */
    requestMsg.msgData.avagoAddrGet = avagoAddr;

    /* Send request to HWS and wait for the reply */
    CHECK_STATUS(mvHwsIpcSendRequestAndGetReply(devNum,&requestMsg,&replyData,
                                                sizeof(MV_HWS_IPC_PORT_INFO_STRUCT),
                                                MV_HWS_IPC_PORT_AVAGO_SERDES_INIT_MSG));

    avagoAddrIpc = &(replyData.readData.portReplyGet);

    /* Update reply parameters */
    *avagoCfgAddr = avagoAddrIpc->results;

    return (replyData.returnCode);
}

