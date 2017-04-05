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
* mvHwsIpcApis.h
*
* DESCRIPTION:
*       Definitions for HWS IPS feature
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
******************************************************************************/

#ifndef __mvHwsIpcApis_H
#define __mvHwsIpcApis_H


#include <private/mvHwsPortTypes.h>
#include <mvHwsServiceCpuFwIf.h>

/*******************************************************************************
* mvHwsIpcDbInit
*
* DESCRIPTION:
*       HW Services Ipc database initialization
*
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_VOID mvHwsIpcDbInit(GT_U8 devNum);

/*******************************************************************************
* mvHwsServiceCpuMsgSend
*
* DESCRIPTION:
*       Send message to Service CPU via IPC channel
*
* INPUTS:
*       channel - transmit channel to send the message
*       msg - message buffer to transmit
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuMsgSend(unsigned char devNum, unsigned int channel, char* msg);

/*******************************************************************************
* mvHwsServiceCpuMsgRecv
*
* DESCRIPTION:
*       Receive message from Service CPU via IPC channel
*
* INPUTS:
*       channel - transmit channel to send the message
*       msg - message buffer to transmit
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuMsgRecv(unsigned char devNum, unsigned int channel, char* msg);

/*******************************************************************************
* mvHwsServiceCpuRead
*
* DESCRIPTION:
*       Read from Service CPU "firmware section"
*
* INPUTS:
*       offset - address, offset from "firmware" base address (0xFFF80000)
*       data   - value
*       size   - size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuRead(unsigned char devNum, unsigned int offset, unsigned int *data, unsigned int size);

/*******************************************************************************
* mvHwsServiceCpuWrite
*
* DESCRIPTION:
*       Write to Service CPU "firmware section"
*
* INPUTS:
*       offset - address, offset from "firmware" base address (0xFFF80000)
*       data   - value
*       size   - size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsServiceCpuWrite(unsigned char devNum, unsigned int offset, unsigned int *data, unsigned int size);

/*******************************************************************************
* mvHwsPortInitIpc
*
* DESCRIPTION:
*       Sends to HWS request to init physical port.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       lbPort     - if true, init port without serdes activity
*       refClock   - Reference clock frequency
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
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 lbPort,
    MV_HWS_REF_CLOCK_SUP_VAL refClock,
    MV_HWS_REF_CLOCK_SOURCE  refClockSource

);

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
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_PORT_ACTION	    action
);

/*******************************************************************************
* mvHwsPortAutoTuneStateCheckIpc
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
GT_STATUS mvHwsPortAutoTuneStateCheckIpc
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    MV_HWS_AUTO_TUNE_STATUS *rxTune,
    MV_HWS_AUTO_TUNE_STATUS *txTune
);

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
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL                 portFecEn

);

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
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL                 *portFecEn

);

/*******************************************************************************
* mvHwsPortLinkStatusGetIpc
*
* DESCRIPTION:
*       Sends message to get the port link status.
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
GT_STATUS mvHwsPortLinkStatusGetIpc
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode,
    GT_BOOL                 *linkStatus
);

/*******************************************************************************
* mvHwsPortAutoTuneSetExtIpc
*
* DESCRIPTION:
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
*       portTuningMode - port TX related tuning mode
*       optAlgoMask - bit mask for optimization algorithms
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
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          phyPortNum,
    MV_HWS_PORT_STANDARD            portMode,
    MV_HWS_PORT_AUTO_TUNE_MODE      portTuningMode,
    GT_U32                          optAlgoMask,
    void                            *results
);

/*******************************************************************************
* mvHwsPortAutoTuneStopIpc
*
* DESCRIPTION:
*       Send IPC message to stop Tx and Rx training
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*       stopRx      - stop RX
*       stopTx      - stop Tx
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
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          phyPortNum,
    MV_HWS_PORT_STANDARD            portMode,
	GT_BOOL                         stopRx,
	GT_BOOL                         stopTx
);

/*******************************************************************************
* mvHwsSerdesManualRxConfigIpc
*
* DESCRIPTION:
*   	Send IPC message to configure SERDES Rx parameters for all SERDES lanes.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*                     config params:
*                                  sqlch,ffeRes,ffeCap,dfeEn,alig
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
    GT_U8                       devNum,
    GT_U32                      portGroup,
    GT_U32                      serdesNum,
	MV_HWS_SERDES_TYPE			serdesType,
    MV_HWS_PORT_MAN_TUNE_MODE   portTuningMode,
    GT_U32                      sqlch,
    GT_U32                      ffeRes,
    GT_U32                      ffeCap,
    GT_BOOL                     dfeEn,
    GT_U32                      alig
);

/*******************************************************************************
* mvHwsSerdesManualTxConfigIpc
*
* DESCRIPTION:
*   	Send IPC message to configure SERDES Tx parameters for all SERDES lanes.
*
* INPUTS:
*       devNum      - system device number
*       portGroup   - port group (core) number
*       phyPortNum  - physical port number
*       portMode    - port standard metric
*                     config params:
*                                  txAmp,txAmpAdj,emph0,emph1,txAmpShft
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
    GT_U8               devNum,
    GT_U32              portGroup,
    GT_U32              serdesNum,
	MV_HWS_SERDES_TYPE  serdesType,
    GT_U32              txAmp,
    GT_BOOL             txAmpAdj,
    GT_U32              emph0,
    GT_U32              emph1,
    GT_BOOL             txAmpShft
);

/*******************************************************************************
* mvHwsPortPolaritySet
*
* DESCRIPTION:
*       Send message to set the port polarity of the Serdes lanes (Tx/Rx).
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
GT_STATUS mvHwsPortPolaritySetIpc
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_32                   txInvMask,
	GT_32                   rxInvMask
);

/*******************************************************************************
* mvHwsPortTxDisable
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
GT_STATUS mvHwsPortTxEnableIpc
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL                 enable
);

/*******************************************************************************
* mvHwsPortTxEnableGetIpc
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
GT_STATUS mvHwsPortTxEnableGetIpc
(
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL serdesTxStatus[]
);

/*******************************************************************************
* mvHwsPortLoopbackSetIpc
*
* DESCRIPTION:
*       Send message to activates the port loopback modes.

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
GT_STATUS mvHwsPortLoopbackSetIpc
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	MV_HWS_UNIT             lpPlace,
	MV_HWS_PORT_LB_TYPE     lbType
);

/*******************************************************************************
* mvHwsPortLoopbackStatusGetIpc
*
* DESCRIPTION:
*       Send IPC message to retrive MAC loopback status.
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
GT_STATUS mvHwsPortLoopbackStatusGetIpc
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	MV_HWS_UNIT             lpPlace,
	MV_HWS_PORT_LB_TYPE     *lbType
);

/*******************************************************************************
* mvHwsPortPPMSetIpc
*
* DESCRIPTION:
*       Send IPC message to increase/decrease  Tx clock on port (added/sub ppm).
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
GT_STATUS mvHwsPortPPMSetIpc
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	MV_HWS_PPM_VALUE	    portPPM
);

/*******************************************************************************
* mvHwsPortPPMGetIpc
*
* DESCRIPTION:
*       Send message to check the entire line configuration
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
GT_STATUS mvHwsPortPPMGetIpc
(
	GT_U8                   devNum,
	GT_U32                  portGroup,
	GT_U32                  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	MV_HWS_PPM_VALUE	    *portPPM
);

/*******************************************************************************
* mvHwsPortInterfaceGetIpc
*
* DESCRIPTION:
*       Send message to gets Interface mode and speed of a specified port.
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
*       GT_OK   		 - on success
*       GT_BAD_PARAM    	 - on wrong port number or device
*       GT_BAD_PTR      	 - one of the parameters is NULL pointer
*       GT_HW_ERROR     	 - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS mvHwsPortInterfaceGetIpc
(
	GT_U8	                devNum,
	GT_U32	                portGroup,
	GT_U32	                phyPortNum,
	MV_HWS_PORT_STANDARD	*portModePtr
);

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
);

/*******************************************************************************
* mvHwsPortSignalDetectGetIpc
*
* DESCRIPTION:
*       Retrieve the signalDetect status of all port serdeses.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
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
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL signalDetect[]
);

/*******************************************************************************
* mvHwsPortCdrLockStatusGetIpc
*
* DESCRIPTION:
*       Send message to get the CDR lock status of all port serdeses.
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
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
	GT_U8   devNum,
	GT_U32  portGroup,
	GT_U32  phyPortNum,
	MV_HWS_PORT_STANDARD    portMode,
	GT_BOOL cdrLockStatus[]
);


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
);

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
);

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
);

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
*       portApCfg  - AP configuration parameters
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
);

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
    GT_U32 *portApStatus
);

/*******************************************************************************
* mvHwsPortApStatsGetIpc
*
* DESCRIPTION:
*       Send message to retrive AP port stats parameters
*
* INPUTS:
*       devNum      - physical device number
*       portGroup   - core number
*       phyPortNum  - physical port number
*       portApStats - AP Stats parameters
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
    GT_U32 *portApStats
);

/*******************************************************************************
* mvHwsPortApStatsResetIpc
*
* DESCRIPTION:
*       Reset AP port statistics information
*
* INPUTS:
*       devNum    - system device number
*       portGroup - port group (core) number
*       apPortNum - AP port number
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
    GT_U32 apPortNum
);

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
    GT_U32 *portApIntrop
);

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
    GT_U32 *portApIntrop
);

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
    GT_U32 *portApDebug
);

/*******************************************************************************
* mvHwsPortApAvagoGuiSetIpc
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
);

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
);

/*******************************************************************************
* mvHwsPortFlowControlStateSetIpc
*
* DESCRIPTION:
*       Activates the port loopback modes.
*       Can be run only after create port not under traffic.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       phyPortNum - physical port number
*       portMode   - port standard metric
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
GT_STATUS mvHwsPortFlowControlStateSetIpc
(
    GT_U8                           devNum,
    GT_U32                          portGroup,
    GT_U32                          phyPortNum,
    MV_HWS_PORT_STANDARD            portMode,
    MV_HWS_PORT_FLOW_CONTROL_ENT    fcState
);

#ifdef __cplusplus
}
#endif

#endif /* __mvHwsIpcApis_H */


