/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssEthPortCtrl.h
*
* DESCRIPTION:
*       Includes Ethernet port routines for sending/receiving packets.
*
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/
#ifndef __cpssEthPortCtrlh
#define __cpssEthPortCtrlh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************ Includes ********************************************************/

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/*
 * typedef: enum CPSS_EXTDRV_ETH_TX_MODE_ENT
 *
 * Description: Enumeration For MII transmission mode
 *
 * Enumerations:
 *      CPSS_EXTDRV_ETH_TX_MODE_ASYNC_E
 *      CPSS_EXTDRV_ETH_TX_MODE_SYNC_E
 */
typedef enum
{
    CPSS_EXTDRV_ETH_TX_MODE_ASYNC_E = 0,
    CPSS_EXTDRV_ETH_TX_MODE_SYNC_E
}CPSS_EXTDRV_ETH_TX_MODE_ENT;


/*******************************************************************************
* CPSS_EXTDRV_ETH_PORT_RX_FUNC
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
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
typedef GT_STATUS (*CPSS_EXTDRV_ETH_PORT_RX_FUNC)
(
    IN GT_U8_PTR   segmentList[],
    IN GT_U32      segmentLen[],
    IN GT_U32      numOfSegments,
    IN GT_U32      queueNum
);


/*******************************************************************************
* CPSS_EXTDRV_ETH_RAW_SOCKET_RX_FUNC
*
* DESCRIPTION:
*       This function is invoked when a packet has been received from the
*       raw packts handler task
*
* INPUT:
*       packet          - A pointer to the entire packet
*       packetLen       - The length of the packet.
*
* OUTPUT:
*       none
*
* RETURN:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/

typedef GT_STATUS (*CPSS_EXTDRV_ETH_RAW_SOCKET_RX_FUNC)
(
    IN GT_U8_PTR        packet,
    IN GT_U32           packetLen,
    IN GT_U32           rxQueue
);

/*******************************************************************************
* CPSS_EXTDRV_ETH_PORT_TX_COMPLETE_FUNC
*
* DESCRIPTION:
*       This function is called after a TxEnd event has been received, it passes
*       the needed information to the Tx packet sender.
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
typedef GT_STATUS (*CPSS_EXTDRV_ETH_PORT_TX_COMPLETE_FUNC)
(
    IN GT_U8_PTR   segmentList[],
    IN GT_U32      numOfSegments
);


/*******************************************************************************
* CPSS_EXTDRV_ETH_PORT_RX_INIT_FUNC
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
typedef GT_STATUS   (*CPSS_EXTDRV_ETH_PORT_RX_INIT_FUNC)
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

typedef GT_STATUS (*CPSS_EXTDRV_ETH_RAW_SOCKET_MODE_SET_FUNC)
(
    IN GT_BOOL flag
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

typedef GT_STATUS (*CPSS_EXTDRV_LINUX_MODE_SET_FUNC)
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

typedef GT_BOOL (*CPSS_EXTDRV_LINUX_MODE_GET_FUNC)
(
 IN GT_U32  portNum
);

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

typedef GT_BOOL (*CPSS_EXTDRV_ETH_RAW_SOCKET_MODE_GET_FUNC)
(
    IN GT_VOID
);

/*******************************************************************************
* CPSS_EXTDRV_ETH_PORT_TX_INIT_FUNC
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
typedef GT_STATUS   (*CPSS_EXTDRV_ETH_PORT_TX_INIT_FUNC)
(
    IN GT_U32        numOfTxBufs
);


/*******************************************************************************
* CPSS_EXTDRV_ETH_PORT_ENABLE_FUNC
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
typedef GT_STATUS   (*CPSS_EXTDRV_ETH_PORT_ENABLE_FUNC)(GT_VOID);


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
typedef GT_STATUS   (*CPSS_EXTDRV_ETH_PORT_DISABLE_FUNC)(GT_VOID);


/*******************************************************************************
* CPSS_EXTDRV_ETH_PORT_TX_FUNC
*
* DESCRIPTION:
*       This function send Tx packet , the packet may be as several segments.
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segment length.
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
typedef GT_STATUS   (*CPSS_EXTDRV_ETH_PORT_TX_FUNC)
(
    IN GT_U8_PTR   segmentList[],
    IN GT_U32      segmentLen[],
    IN GT_U32      numOfSegments,
    IN GT_U32      txQueue
);


/*******************************************************************************
* CPSS_EXTDRV_ETH_PORT_INPUT_HOOK_ADD_FUNC
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
typedef GT_STATUS   (*CPSS_EXTDRV_ETH_PORT_INPUT_HOOK_ADD_FUNC)
(
    IN  CPSS_EXTDRV_ETH_PORT_RX_FUNC  userRxFunc
);


/*******************************************************************************
* CPSS_EXTDRV_ETH_RAW_SOCKET_RX_HOOK_ADD_FUNC
*
* DESCRIPTION:
*       This bind the user raw packet Rx callback
*
* INPUTS:
*       userRxFunc - the user raw packet Rx callback function
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
typedef GT_STATUS (*CPSS_EXTDRV_ETH_RAW_SOCKET_RX_HOOK_ADD_FUNC)
(
    IN  CPSS_EXTDRV_ETH_RAW_SOCKET_RX_FUNC  userRxFunc
);


/*******************************************************************************
* CPSS_EXTDRV_ETH_PORT_TX_COMPLETE_HOOK_ADD_FUNC
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
typedef GT_STATUS   (*CPSS_EXTDRV_ETH_PORT_TX_COMPLETE_HOOK_ADD_FUNC)
(
    IN CPSS_EXTDRV_ETH_PORT_TX_COMPLETE_FUNC userTxFunc
);


/*******************************************************************************
* CPSS_EXTDRV_ETH_PORT_RX_PACKET_FREE_FUNC
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
typedef GT_STATUS   (*CPSS_EXTDRV_ETH_PORT_RX_PACKET_FREE_FUNC)
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           numOfSegments,
    IN GT_U32           queueNum
);

/*******************************************************************************
* CPSS_EXTDRV_ETH_PORT_TX_MODE_SET_FUNC
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
typedef GT_STATUS   (*CPSS_EXTDRV_ETH_PORT_TX_MODE_SET_FUNC)
(
    CPSS_EXTDRV_ETH_TX_MODE_ENT    txMode
);

/*******************************************************************************
* CPSS_EXTDRV_ETH_CPU_CODE_TO_QUEUE
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
typedef GT_STATUS (*CPSS_EXTDRV_ETH_CPU_CODE_TO_QUEUE)
(
    IN GT_U32  dsaCpuCode,
    IN GT_U8   rxQueue
);
/*******************************************************************************
* CPSS_EXTDRV_ETH_CPU_PREPEND_TWO_BYTES_FUNC
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
typedef GT_STATUS (*CPSS_EXTDRV_ETH_CPU_PREPEND_TWO_BYTES_FUNC)
(
    IN GT_BOOL enable
);

/* CPSS_EXT_DRV_ETH_PORT_STC -
    structure that hold the "external driver Ethernet port" functions needed be
    bound to cpss.
*/
typedef struct{
    CPSS_EXTDRV_ETH_PORT_RX_INIT_FUNC       extDrvEthPortRxInitFunc;
    CPSS_EXTDRV_ETH_RAW_SOCKET_MODE_SET_FUNC       extDrvEthRawSocketModeSetFunc;
    CPSS_EXTDRV_ETH_RAW_SOCKET_MODE_GET_FUNC       extDrvEthRawSocketModeGetFunc;
    CPSS_EXTDRV_LINUX_MODE_SET_FUNC       extDrvLinuxModeSetFunc;
    CPSS_EXTDRV_LINUX_MODE_GET_FUNC       extDrvLinuxModeGetFunc;
    CPSS_EXTDRV_ETH_PORT_TX_INIT_FUNC       extDrvEthPortTxInitFunc;
    CPSS_EXTDRV_ETH_PORT_ENABLE_FUNC        extDrvEthPortEnableFunc;
    CPSS_EXTDRV_ETH_PORT_DISABLE_FUNC       extDrvEthPortDisableFunc;
    CPSS_EXTDRV_ETH_PORT_TX_FUNC            extDrvEthPortTxFunc;
    CPSS_EXTDRV_ETH_PORT_INPUT_HOOK_ADD_FUNC  extDrvEthPortInputHookAddFunc;
    CPSS_EXTDRV_ETH_RAW_SOCKET_RX_HOOK_ADD_FUNC  extDrvEthRawSocketRxHookAddFunc;
    CPSS_EXTDRV_ETH_PORT_TX_COMPLETE_HOOK_ADD_FUNC    extDrvEthPortTxCompleteHookAddFunc;
    CPSS_EXTDRV_ETH_PORT_RX_PACKET_FREE_FUNC  extDrvEthPortRxPacketFreeFunc;
    CPSS_EXTDRV_ETH_PORT_TX_MODE_SET_FUNC   extDrvEthPortTxModeSetFunc;
    CPSS_EXTDRV_ETH_CPU_CODE_TO_QUEUE       extDrvEthCpuCodeToQueueFunc;
    CPSS_EXTDRV_ETH_CPU_PREPEND_TWO_BYTES_FUNC extDrvEthPrePendTwoBytesHeaderSetFunc;
}CPSS_EXT_DRV_ETH_PORT_STC;


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __cpssEthPortCtrlh */



