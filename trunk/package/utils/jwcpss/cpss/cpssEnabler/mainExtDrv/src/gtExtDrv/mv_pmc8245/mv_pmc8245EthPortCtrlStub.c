/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gt64260EthPortCtrlStub.c 
*
* DESCRIPTION:
*       This file includes stub functions for receiving packet buffers
*       from upper layer, and sending them trough the PP's NIC.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/

#include <gtOs/gtOsGen.h>
#include <gtExtDrv/drivers/gtEthPortCtrl.h>
#include <prestera/driver/interrupts/gtDriverInterrupts.h>

/********************************************************************************
* extDrvEthPortRxInit
*
* DESCRIPTION: Init the ethernet port Rx interface
*
* INPUTS:
*       rxBufPoolSize   - buffer pool size
*       rxBufPool_PTR   - the address of the pool
*       rxQNum          - the number of RX queues
*       rxQbufPercentage- the buffer percentage dispersal for all queues
*       rxBufSize       - the buffer requested size
*       numOfRxBufs_PTR - number of requested buffers, and actual buffers created
*       headerOffset    - packet header offset size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPortRxInit
(
    IN GT_U32           rxBufPoolSize,
    IN GT_U8_PTR        rxBufPool_PTR,
    IN GT_U32           rxQNum,
    IN GT_U32           rxQbufPercentage[],
    IN GT_U32           rxBufSize,
    INOUT GT_U32        *numOfRxBufs_PTR,
    IN GT_U32           headerOffset
)
{
    return GT_OK;
}

/*******************************************************************************
 * extDrvEthRawSocketModeSet
 *
 * DESCRIPTION: Sets the raw packet mode
 *
 * INPUTS:
 *       flag      - GT_TRUE  - rawSocketMode is set to GT_TRUE
 *                 - GT_FALSE - rawSocketMode is set to GT_FALSE
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK always
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthRawSocketModeSet
(
 IN GT_BOOL          flag
 )
{
  /* stub */
  return GT_OK;
}

/*******************************************************************************
 * extDrvEthRawSocketModeGet
 *
 * DESCRIPTION: returns the raw packet mode
 *
 * INPUTS:
 *       None:
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_BOOL indicating raw mode is true or false.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_BOOL extDrvEthRawSocketModeGet
(
 IN GT_VOID
 )
{
  /* stub */
  return GT_FALSE;
}

/*******************************************************************************
* extDrvLinuxModeSet
*
* DESCRIPTION:
*       Set port <portNum> to Linux Mode (Linux Only)
*
* INPUTS:
*       portNum         - The port number to be defined for Linux mode
*       ip1, ip2, ip3, ip4 - The ip address to assign to the port, 4 numbers
* OUTPUTS:
*
* RETURNS:
*       GT_OK   - always,
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_STATUS extDrvLinuxModeSet
(
 IN GT_U32 portNum,
 IN GT_U32 ip1,
 IN GT_U32 ip2,
 IN GT_U32 ip3,
 IN GT_U32 ip4
)
{
  /* stub */
  return GT_OK;
}              

/*******************************************************************************
* extDrvLinuxModeGet
*
* DESCRIPTION:
*       Get port <portNum> Linux Mode indication (Linux Only)
*
* INPUTS:
*       portNum         - The port number to be defined for Linux mode
* OUTPUTS:
*
* RETURNS:
*       GT_TRUE if Linux mode, or
*       GT_FALSE otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

GT_BOOL extDrvLinuxModeGet
(
 IN GT_U32  portNum
)
{
  /* stub */
  return GT_FALSE;
}              

/*******************************************************************************
 * extDrvEthMuxSet
 *
 * DESCRIPTION: Sets the mux mode to one of cpss, raw, linux
 *
 * INPUTS:
 *       portNum   - The port number for the action
 *       portType  - one of the modes: cpss, raw, Linux
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK always
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthMuxSet
(
 IN unsigned long portNum,
 IN extDrvEthNetPortType_ENT portType
 )
{
  /* stub */
  return GT_OK;
}

/*******************************************************************************
 * extDrvEthMuxGet
 *
 * DESCRIPTION: Get the mux mosde of the port
 *
 * INPUTS:
 *       portNum   - The port number for the action
 *
 * OUTPUTS:
 *       portType  - port type information
 *
 * RETURNS:
 *       GT_OK always
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthMuxGet
(
 IN unsigned long portNum,
 OUT extDrvEthNetPortType_ENT *portTypeP
 )
{
  /* stub */
  *portTypeP = extDrvEthNetPortType_numOfTypes; /* make it invalid */
  return GT_OK;
}

/*******************************************************************************
 * extDrvEthRawSocketRxHookAdd
 *
 * DESCRIPTION:
 *       bind the raw packet Rx callback
 *
 * INPUTS:
 *       userRawRxFunc - the user ra packet Rx callbak function
 *
 * OUTPUTS:
 *       None.
 *
 * RETURNS:
 *       GT_OK if successful, or
 *       GT_FAIL otherwise.
 *
 * COMMENTS:
 *       None.
 *
 *******************************************************************************/
GT_STATUS extDrvEthRawSocketRxHookAdd
(
 IN GT_RawSocketRx_FUNCPTR    userRxFunc
 )
{
  /* stub */
  return GT_OK;
}

/*******************************************************************************
* extDrvEthPortTxInit
*
* DESCRIPTION: Init the ethernet port Tx interface
*
* INPUTS:
*       numOfTxBufs - number of requested buffers
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPortTxInit
(
    IN GT_U32           numOfTxBufs
)
{
    return GT_OK;
}


/*******************************************************************************
* extDrvEthPortEnable
*
* DESCRIPTION: Enable the ethernet port interface
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPortEnable
(
    GT_VOID
)
{
    return GT_OK;
}


/*******************************************************************************
* extDrvEthPortDisable
*
* DESCRIPTION: Disable the ethernet port interface
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPortDisable
(
    GT_VOID
)
{
    return GT_OK;
}


/*******************************************************************************
* extDrvEthPortRxCb
*
* DESCRIPTION:
*       This function receives a packet from the ethernet port.
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segement length.
*       numOfSegments   - The number of segment in segment list.
*       queueNum        - the received queue number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if successful, or
*       GT_FALSE otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPortRxCb
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],   
    IN GT_U32           numOfSegments,
    IN GT_U32           queueNum
)
{
    return GT_OK;
}


/******************************************************************************
* extDrvEthPortTxEndCb
*
* DESCRIPTION:
*       This function receives a packet from the ethernet port.
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       numOfSegments   - The number of segment in segment list.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPortTxEndCb
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           numOfSegments
)
{
    return GT_OK;
}


/*******************************************************************************
* extDrvEthPortTx
*
* DESCRIPTION:
*       This function transmits a packet through the ethernet port in salsa.
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segement length.
*       numOfSegments   - The number of segment in segment list.
*       txQueue         - The TX queue.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPortTx
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           txQueue
)
{
    /* send the packet */
    return GT_FAIL;
}


/*******************************************************************************
* extDrvEthInputHookAdd
*
* DESCRIPTION:
*       This bind the user Rx callback
*
* INPUTS:
*       userRxFunc - the user Rx callbak function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthInputHookAdd
(
    IN GT_Rx_FUNCPTR    userRxFunc
)
{
    return GT_OK;
}


/*******************************************************************************
* extDrvEthTxCompleteHookAdd
*
* DESCRIPTION:
*       This bind the user Tx complete callback
*
* INPUTS:
*       userTxFunc - the user Tx callback function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthTxCompleteHookAdd
(
    IN GT_Tx_COMPLETE_FUNCPTR userTxFunc
)
{
    return GT_OK;
}


/*******************************************************************************
* extDrvEthRxPacketFree
*
* DESCRIPTION:
*       This routine frees the recievd Rx buffer. 
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       numOfSegments   - The number of segment in segment list.
*       queueNum        - receive queue number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthRxPacketFree
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           numOfSegments,
    IN GT_U32           queueNum
)
{
    return GT_FAIL;
}

/*******************************************************************************
* extDrvEthPortTxModeSet
*
* DESCRIPTION: Set the ethernet port tx mode
*
* INPUTS:
*       txMode - extDrvEthTxMode_asynch_E - don't wait for TX done - free packet
*                when interrupt received 
*                extDrvEthTxMode_asynch_E - wait to TX done and free packet
*                immediately
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise
*       GT_NOT_SUPPORTED
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPortTxModeSet
(
    extDrvEthTxMode_ENT    txMode
)
{
    return GT_NOT_SUPPORTED;
}
