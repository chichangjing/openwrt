/*******************************************************************************
*                Copyright 2015, MARVELL SEMICONDUCTOR, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), MARVELL ISRAEL LTD. (MSIL).                                          *
 ********************************************************************************
 * noKmDrvEthPortCtrlStub.c
 *
 * DESCRIPTION:
 *       This file includes all needed functions for receiving packet buffers
 *       from upper layer, and sending them trough the PP's NIC.
 *
 * DEPENDENCIES:
 *       None.
 *
 * FILE REVISION NUMBER:
 *       $Revision: 1 $
 *
 *******************************************************************************/
#include <gtOs/gtOsGen.h>
#include <gtExtDrv/drivers/gtEthPortCtrl.h>
#include <gtExtDrv/drivers/gtIntDrv.h>

/*******************************************************************************
 * extDrvNetIfIntConnect
 *
 * DESCRIPTION:
 *       Connect network interface rx/tx events.
 *
 * INPUTS:
 *       None
 *
 * OUTPUTS:
 *       None
 *
 * RETURNS:
 *       GT_OK   - on success
 *       GT_FAIL - on error
 *
 * COMMENTS:
 *       None
 *
 *******************************************************************************/
GT_U32 extDrvNetIfIntConnect(GT_VOID)
{
    return GT_FAIL;
}

/*******************************************************************************
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
    (void)rxBufPoolSize;
    (void)rxBufPool_PTR;
    (void)rxQNum;
    (void)rxQbufPercentage;
    (void)rxBufSize;
    (void)numOfRxBufs_PTR;
    (void)headerOffset;

    return GT_FAIL;
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
    (void)flag;
    return GT_FAIL;
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
  return GT_FALSE;
}

/*******************************************************************************
* extDrvLinuxModeGet
*
* DESCRIPTION:
*       Get port <portNum> Linux Mode indication (Linux Only)
*
* INPUTS:
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
    (void)portNum;
    return GT_FALSE;
}

/*******************************************************************************
 * extDrvEthPortRxInitWithoutInterruptConnection
 *
 * DESCRIPTION: this function initialises  ethernet port Rx interface without interrupt
 *              connection
 *              The function used in multiprocess appDemo for eth port init in appDemo
 *              process. In RxTxProcess used extDrvEthPortRxInit that run
 *              RxTxInterrupt thread
 *
 *
 * INPUTS:
 *  rxBufPoolSize - buffer pool size
 *  rxBufPool_PTR - the address of the pool
 *  rxQNum      - the number of RX queues
 *  rxQbufPercentage  - the buffer percentage dispersal for all queues
 *  rxBufSize - the buffer requested size
 *  numOfRxBufs_PTR - number of requested buffers, and actual buffers created
 *  headerOffset  - packet header offset size
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
GT_STATUS extDrvEthPortRxInitWithoutInterruptConnection
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
    (void)rxBufPoolSize;
    (void)rxBufPool_PTR;
    (void)rxQNum;
    (void)rxQbufPercentage;
    (void)rxBufSize;
    (void)numOfRxBufs_PTR;
    (void)headerOffset;

    return GT_FAIL;
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
    (void)numOfTxBufs;
    return GT_FAIL;
}

/*******************************************************************************
 * extDrvEthPortEnable
 *
 * DESCRIPTION: Enable the ethernet port interface
 *
 * INPUTS:
 *       None
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
    return GT_FAIL;
}


/*******************************************************************************
 * extDrvEthPortDisable
 *
 * DESCRIPTION: Disable the ethernet port interface
 *
 * INPUTS:
 *       None
 *
 * OUTPUTS:fprintf(stderr, "extDrvEthPortEnable:start, errno(%s)\n",strerror(errno));
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

    return GT_FAIL;
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
    (void)segmentList;
    (void)segmentLen;
    (void)numOfSegments;
    (void)queueNum;
    return GT_FAIL;
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
    (void)segmentList;
    (void)numOfSegments;
    return GT_FAIL;
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
    (void)segmentList;
    (void)segmentLen;
    (void)numOfSegments;
    (void)txQueue;
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
    (void)txMode;

    return GT_NOT_SUPPORTED;
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
    (void)userRxFunc;
    return GT_FAIL;
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
    (void)userRxFunc;
    return GT_FAIL;
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
    (void)userTxFunc;
    return GT_FAIL;
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
    (void)segmentList;
    (void)numOfSegments;
    (void)queueNum;

    return GT_FAIL;
}

/*******************************************************************************
* extDrvEthCpuCodeToQueue
*
* DESCRIPTION:
*       Binds DSA CPU code to RX queue.
*
* INPUTS:
*       dsaCpuCode - DSA CPU code
*       rxQueue    -  rx queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on fail
*       GT_NOT_SUPPORTED    - the API is not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthCpuCodeToQueue
(
    IN GT_U32  dsaCpuCode,
    IN GT_U8   rxQueue
)
{
    (void)dsaCpuCode;
    (void)rxQueue;

    return GT_FAIL;
}

/*******************************************************************************
* extDrvEthPrePendTwoBytesHeaderSet
*
* DESCRIPTION:
*       Enables/Disable pre-pending a two-byte header to all packets arriving
*       to the CPU.
*
* INPUTS:
*        enable    - GT_TRUE  - Two-byte header is pre-pended to packets
*                               arriving to the CPU.
*                    GT_FALSE - Two-byte header is not pre-pended to packets
*                               arriving to the CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success
*       GT_FAIL             - on fail
*       GT_NOT_SUPPORTED    - the API is not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPrePendTwoBytesHeaderSet
(
    IN GT_BOOL enable
)
{
    /*CPSS_TBD_BOOKMARK*/
    /* add support */
    return GT_OK;
}
