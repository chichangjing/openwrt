/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* shrMemSharedData.c
*
* DESCRIPTION:
*       This file contains common data for OS/SharedMemory modules
*       which should be shared between different processes, thus should
*       be linked into libcpss.so (only that object
*       has shared BSS/DATA sections).
*
* FILE REVISION NUMBER:
*       $Revision: 8  $
*
*******************************************************************************/
#include <gtOs/gtEnvDep.h>
#include <gtOs/gtOsSharedMalloc.h>
#include <gtOs/gtOsSharedPp.h>
#include <gtOs/gtOsSharedUtil.h>


/* Registry for mapped into /dev/mvPP device sections.
 * Used by non-first clients to restore mapping. */
PP_MMAP_INFO_STC pp_mmap_sections_registry[PP_REGISTRY_SIZE_CNS];
int pp_mmap_registry_index = 0;




/*********************************************************************************************
*  The RxTxProcessParams struct used for interprocess communication in multi process appDemo
*  the RxTx parameters transmitted from appDemo process to RxTxProcess used to fill the 
*  the packet capture table
*  
*  function cpssMultiProcComSetRxTxParams used to set the RxTx parameters
*  function cpssMultiProcComGetRxTxParams used to get the RxTx parameters
*  function cpssMultiProcComSetTransmitParams used to set the Packet Transmit  parameters
*  function cpssMultiProcComGetTransmitParams used to get the Packet Transmit  parameters
*
**********************************************************************************************/
#ifdef SHARED_MEMORY

/* keep RxTx parameters struct definition */
static CPSS_RX_TX_MP_MODE_PARAMS_STC RxTxProcessParams;

/*******************************************************************************
* cpssMultiProcComSetRxTxParams
*
* DESCRIPTION:
*       
*	Used to set  packet RxTx parameters for transmission from appDemo process 
*  to RxTx process			
*
* INPUTS:
*  		paramPtr - pointer to struct CPSS_DXCH_TRANSMIT_MP_MODE_PARAMS_STC     
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcComSetRxTxParams
(
	CPSS_RX_TX_MP_MODE_PARAMS_STC *paramPtr
)
{
	RxTxProcessParams.valid = paramPtr->valid;
	RxTxProcessParams.copyRxInfoFunc = paramPtr->copyRxInfoFunc ;
	RxTxProcessParams.doCapture = paramPtr->doCapture;
	RxTxProcessParams.freeRxInfoFunc = paramPtr->freeRxInfoFunc;
	RxTxProcessParams.indexToInsertPacket = paramPtr->indexToInsertPacket;
	RxTxProcessParams.maxNumOfRxEntries = paramPtr->maxNumOfRxEntries;
	RxTxProcessParams.maxRxBufferSize = paramPtr->maxRxBufferSize;
	RxTxProcessParams.rxEntriesArr = paramPtr->rxEntriesArr;
	RxTxProcessParams.rxTableMode = paramPtr->rxTableMode;
	RxTxProcessParams.specificDeviceFormatPtr = paramPtr->specificDeviceFormatPtr;
	RxTxProcessParams.wasInitDone = paramPtr->wasInitDone;

	return GT_OK;
}

/*******************************************************************************
* cpssMultiProcComGetRxTxParams
*
* DESCRIPTION:
*       
*	Used to get  packet RxTx parameters for transmission from appDemo process and send it
* to RxTx process			
*
* INPUTS:
*  		paramPtr - pointer to struct CPSS_DXCH_TRANSMIT_MP_MODE_PARAMS_STC     
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcComGetRxTxParams
(
	CPSS_RX_TX_MP_MODE_PARAMS_STC *paramPtr
)
{
	paramPtr->valid = RxTxProcessParams.valid;
	paramPtr->copyRxInfoFunc = RxTxProcessParams.copyRxInfoFunc;
	paramPtr->doCapture = RxTxProcessParams.doCapture;
	paramPtr->freeRxInfoFunc = RxTxProcessParams.freeRxInfoFunc;
	paramPtr->indexToInsertPacket = RxTxProcessParams.indexToInsertPacket;
	paramPtr->maxNumOfRxEntries = RxTxProcessParams.maxNumOfRxEntries;
	paramPtr->maxRxBufferSize = RxTxProcessParams.maxRxBufferSize;
	paramPtr->rxEntriesArr = RxTxProcessParams.rxEntriesArr;
	paramPtr->rxTableMode = RxTxProcessParams.rxTableMode;
	paramPtr->specificDeviceFormatPtr = RxTxProcessParams.specificDeviceFormatPtr;
	paramPtr->wasInitDone = RxTxProcessParams.wasInitDone;

	return GT_OK;
}

#ifdef CHX_FAMILY


/*********** TX  ************/

static CPSS_DXCH_TRANSMIT_MP_MODE_PARAMS_STC transmitParams;

/*******************************************************************************
* cpssMultiProcComSetTransmitParams
*
* DESCRIPTION:
*       
*	Used to set  packet transmit parameters for transmission to  RxTx process from appDemo process			
*
* INPUTS:
*  		paramPtr - pointer to struct CPSS_DXCH_TRANSMIT_MP_MODE_PARAMS_STC     
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcComSetTransmitParams
(
	CPSS_DXCH_TRANSMIT_MP_MODE_PARAMS_STC *paramPtr
)
{
	transmitParams.valid = paramPtr->valid;
	transmitParams.cpssDxChTxPacketDescTbl = paramPtr->cpssDxChTxPacketDescTbl ;
	transmitParams.flagStopTransmit = paramPtr->flagStopTransmit;
	transmitParams.modeSettings.cyclesNum = paramPtr->modeSettings.cyclesNum;
	transmitParams.modeSettings.gap = paramPtr->modeSettings.gap;
	transmitParams.txDxChTblCapacity = paramPtr->txDxChTblCapacity;
	transmitParams.txPoolPtr = paramPtr->txPoolPtr;

	return GT_OK;
}


/*******************************************************************************
* cpssMultiProcComGetTransmitParams
*
* DESCRIPTION:
*       
*	Used to get  packet transmit parameters to RxTx process from appDemo process			
*
* INPUTS:
*  		paramPtr - pointer to struct CPSS_DXCH_TRANSMIT_MP_MODE_PARAMS_STC     
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcComGetTransmitParams
(
	CPSS_DXCH_TRANSMIT_MP_MODE_PARAMS_STC *paramPtr
)
{
	paramPtr->valid = transmitParams.valid;
	paramPtr->cpssDxChTxPacketDescTbl = transmitParams.cpssDxChTxPacketDescTbl;
	paramPtr->flagStopTransmit = transmitParams.flagStopTransmit;
	paramPtr->modeSettings.cyclesNum = transmitParams.modeSettings.cyclesNum;
	paramPtr->modeSettings.gap = transmitParams.modeSettings.gap;
	paramPtr->txDxChTblCapacity = transmitParams.txDxChTblCapacity;
	paramPtr->txPoolPtr = transmitParams.txPoolPtr;
	
	return GT_OK;
}

/************ Command transmit ***********/

static CPSS_RX_TX_PROC_COM_CMD_STC rxTxGaltisCommands = { 0,0 }; 

/*******************************************************************************
* cpssMultiProcComGetRxTxCommand
*
* DESCRIPTION:
*       
*	Used to get if the Galtis command  cmdCpssRxPktClearTbl was run by appDemo process 
*  		
*
* INPUTS:
*  		paramPtr - pointer to struct CPSS_RX_TX_PROC_COM_CMD_STC     
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcComGetRxTxCommand
(
		CPSS_RX_TX_PROC_COM_CMD_STC *cmdParamPtr
)
{

		cmdParamPtr->valid = rxTxGaltisCommands.valid;
		cmdParamPtr->mpCmdCpssRxPktClearTbl = rxTxGaltisCommands.mpCmdCpssRxPktClearTbl;

		return GT_OK;
}

/*******************************************************************************
* cpssMultiProcComSetRxTxCommand
*
* DESCRIPTION:
*       
*	Used to set if the Galtis command  cmdCpssRxPktClearTbl was run by appDemo process 
*  		
*
* INPUTS:
*  		paramPtr - pointer to struct CPSS_RX_TX_PROC_COM_CMD_STC     
*
* OUTPUTS:
*       None.
*
* RETURNS:
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssMultiProcComSetRxTxCommand
(
	CPSS_RX_TX_PROC_COM_CMD_STC *cmdParamPtr
)
{
	rxTxGaltisCommands.valid = cmdParamPtr->valid;
	rxTxGaltisCommands.mpCmdCpssRxPktClearTbl = cmdParamPtr->mpCmdCpssRxPktClearTbl;

	return GT_OK;
}


/**************** SMI ETH port transmit *************/

static CPSS_DXCH_MP_SMI_TRANSMIT_PARAMS_STC txKeepSmiTransmitParams;


GT_STATUS cpssMultiProcComGetSmiTransmitParams
(
	CPSS_DXCH_MP_SMI_TRANSMIT_PARAMS_STC *paramPtr
)
{
	paramPtr->appDemoSysConfig = txKeepSmiTransmitParams.appDemoSysConfig;
	paramPtr->valid = txKeepSmiTransmitParams.valid;

	return 0;
}

GT_STATUS cpssMultiProcComSetSmiTransmitParams
(
	CPSS_DXCH_MP_SMI_TRANSMIT_PARAMS_STC *paramPtr
)
{
	txKeepSmiTransmitParams.appDemoSysConfig = paramPtr->appDemoSysConfig;
	txKeepSmiTransmitParams.valid = paramPtr->valid;

	return 0;
}



#endif
#endif

