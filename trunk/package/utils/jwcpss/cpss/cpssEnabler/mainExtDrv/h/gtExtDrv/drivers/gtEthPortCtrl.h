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
* gtEthPortCtrl.h
*
* DESCRIPTION:
*       Includes ethernet port routines for sending/receiving packets.
*
* DEPENDENCIES:
*       -   BSP
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/
#ifndef __gtEthPortCtrlh
#define __gtEthPortCtrlh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/*
 * Typedef: enum extDrvEthTxMode_ENT
 *
 * Description:
 *             MII transmission mode
 *
 * Fields: 
 *          extDrvEthTxMode_asynch_E - execute asynchroniouse packet send
 *          extDrvEthTxMode_synch_E  - execute synchroniouse packet send
 *
 * Note:
 *
 */    
typedef enum 
{
    extDrvEthTxMode_asynch_E = 0,
    extDrvEthTxMode_synch_E
} extDrvEthTxMode_ENT;


/*
 * Typedef: enum extDrvEthNetPortType_ENT
 *
 * Description:
 *      This type defines types of switch ports for BSP ETH driver.
 *
 * Fields:
 *      extDrvEthNetPortType_cpss_E   - packets forwarded to CPSS
 *      extDrvEthNetPortType_raw_E    - packets forwarded to OS (without dsa removal)
 *      extDrvEthNetPortType_linux_E  - packets forwarded to OS (with dsa removal)
 *
 * Note:
 *      The enum has to be compatible with MV_NET_OWN and ap_packet.c
 *
 */
typedef enum
{

    /* cpss = the packet is sent directly to cpss */
    extDrvEthNetPortType_cpss_E    = 0,

    /* raw = the packet is sent to the network stack WITHOUT removing the dsa */
    extDrvEthNetPortType_raw_E    = 1,

    /* linux = the packet is sent to the network stack AFTER removing the dsa */
    extDrvEthNetPortType_linux_E    = 2,

    extDrvEthNetPortType_numOfTypes
} extDrvEthNetPortType_ENT;



/*******************************************************************************
* GT_ExtRx_FUNCP
*
* DESCRIPTION:
*       The prototype of the routine to be called after a packet was received
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segment length.
*       numOfSegments   - The number of segment in segment list.
*       queueNum        - the received queue number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if it has handled the input packet and no further action should 
*               be taken with it, or
*       GT_FALSE if it has not handled the input packet and normal processing.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*GT_Rx_FUNCPTR)
(
    IN GT_U8_PTR   segmentList[],
    IN GT_U32      segmentLen[],   
    IN GT_U32      numOfSegments,
    IN GT_U32      queueNum
);


/*******************************************************************************
* GT_RawSocketRx_FUNCPTR
*
* DESCRIPTION:
*       The prototype of the routine to be called after a raw packet was received
*
* INPUTS:
*       packet          - points to the entire packet.
*       packetLen       - The length of teh packet.
*       queueNum        - the received queue number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if it has handled the input packet and no further action should 
*               be taken with it, or
*       GT_FALSE if it has not handled the input packet and normal processing.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

typedef GT_STATUS (*GT_RawSocketRx_FUNCPTR)
(
    IN GT_U8_PTR        packet,
    IN GT_U32           packetLen,
    IN GT_U32           rxQueue
);


/*******************************************************************************
* GT_Tx_COMPLETE_FUNCPTR
*
* DESCRIPTION:
*       The prototype of the routine to be called after a packet was received
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       numOfSegments   - The number of segment in segment list.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_TRUE if it has handled the input packet and no further action should 
*               be taken with it, or
*       GT_FALSE if it has not handled the input packet and normal processing.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*GT_Tx_COMPLETE_FUNCPTR)
(
    IN GT_U8_PTR   segmentList[],
    IN GT_U32      numOfSegments
);


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
);

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
    IN GT_BOOL flag
);

/*******************************************************************************
 * extDrvEthRawSocketModeGet
 *
 * DESCRIPTION: returns the raw packet mode
 *
 * INPUTS:
 *       flag      - GT_TRUE  - rawSocketMode is set to GT_TRUE
 *                 - GT_FALSE - rawSocketMode is set to GT_FALSE
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
);

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
);

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
);

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
);

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
 );

/*******************************************************************************
* extDrvEthPortRxInitWithoutInterruptConnection
*
* DESCRIPTION: this function initialises  ethernet port Rx interface without interrupt 
*                           connection
*                           The function used in multiprocess appDemo for eth port init in appDemo 
*                           process. In RxTxProcess used extDrvEthPortRxInit that run 
*                           RxTxInterrupt thread 
*                           
*
* INPUTS:
*   rxBufPoolSize   -   buffer pool size
*   rxBufPool_PTR   -   the address of the pool
*   rxQNum          -   the number of RX queues
*   rxQbufPercentage    -   the buffer percentage dispersal for all queues
*   rxBufSize   -   the buffer requested size
*   numOfRxBufs_PTR -   number of requested buffers, and actual buffers created
*   headerOffset    -   packet header offset size
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
);

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
    IN GT_U32        numOfTxBufs
);


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
GT_STATUS extDrvEthPortEnable(GT_VOID);


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
GT_STATUS extDrvEthPortDisable(GT_VOID);


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
);


/*******************************************************************************
* extDrvEthPortRxCb
*
* DESCRIPTION:
*       This function receives a packet from the ethernet port.
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segment length.
*       numOfSegments   - The number of segment in segment list.
*       queueNum        - The received queue number
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
GT_STATUS extDrvEthPortRxCb
(
    IN GT_U8_PTR   segmentList[],
    IN GT_U32      segmentLen[],   
    IN GT_U32      numOfSegments,
    IN GT_U32      queueNum
);


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
    IN GT_U8_PTR   segmentList[],
    IN GT_U32      numOfSegments
);


/*******************************************************************************
* extDrvEthInputHookAdd
*
* DESCRIPTION:
*       This bind the user Rx callback
*
* INPUTS:
*       userRxFunc - the user Rx callback function
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
    IN GT_Rx_FUNCPTR userRxFunc
);

/*******************************************************************************
* extDrvEthRawSocketRxHookAdd
*
* DESCRIPTION:
*       This bind the user Rx callback
*
* INPUTS:
*       userRxFunc - the user Rx callback function
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
    IN GT_RawSocketRx_FUNCPTR userRxFunc
);

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
);


/*******************************************************************************
* extDrvEthRxPacketFree
*
* DESCRIPTION:
*       This routine frees the received Rx buffer. 
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       numOfSegments   - The number of segment in segment list.
*       queueNum        - Receive queue number
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
);

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
*       GT_OK               - on success
*       GT_FAIL             - on fail
*       GT_NOT_SUPPORTED    - the API is not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS extDrvEthPortTxModeSet
(
    extDrvEthTxMode_ENT    txMode
);

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
);

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
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __gtEthPortCtrlh */

