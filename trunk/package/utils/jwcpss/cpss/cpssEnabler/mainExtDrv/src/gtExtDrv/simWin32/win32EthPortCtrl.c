/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtEthPortCtrl.c
*
* DESCRIPTION:
*       This file includes all needed functions for receiving packet buffers
*       from upper layer, and sending them trough the PP's NIC.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*
*******************************************************************************/

#include <os/simTypes.h>
#include <gtExtDrv/drivers/gtEthPortCtrl.h>

#ifndef APPLICATION_SIDE_ONLY
    /* this file exists only when it is not application side only , because those
       files are on the devices side */
    #include <asicSimulation/SKernel/skernel.h>
    #include <common/SBUF/sbuf.h>
#else
    #define SIM_NIC_NOT_IMPLEMENTED
#endif /*!APPLICATION_SIDE_ONLY*/

#ifndef SIM_NIC_NOT_IMPLEMENTED

extern void * osMemCpy
(
    IN void *       destination,
    IN const void * source,
    IN GT_U32       size
);
extern int osPrintf(const char* format, ...);

extern GT_STATUS osTimerWkAfter
(
    IN GT_U32 mils
);

static GT_Tx_COMPLETE_FUNCPTR userTxCallback = NULL;
static GT_Rx_FUNCPTR          userRxCallback = NULL;

/* number of buffers in the pool */
#define SBSP_BUFFERS_NUMBER_CNS                   512

/* pss need up to 300 bytes for management */
#define SBSP_BUFFER_PSS_MANAGEMENT_OFFSET_CNS     300

static SBUF_POOL_ID             sbspBuffPool    = NULL;




/*******************************************************************************
* userRxCb
*
* DESCRIPTION:
*       This function wraps the Rx callback to disjoin the smain interface and
*       the external driver callback routine.
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segement length.
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
static GT_STATUS userRxCbWrapper
(
    IN GT_U8_PTR   segmentList[],
    IN GT_U32      segmentLen[],
    IN GT_U32      numOfSegments
)
{
    SBUF_BUF_ID  bufferId;
    GT_U8*      data_PTR;
    GT_U32      data_size;

    if (userRxCallback != NULL)
    {
        /* get buffer */
        bufferId = sbufAlloc(sbspBuffPool,SBUF_DATA_SIZE_CNS);

        if ( bufferId == NULL )
        {
            /* printf */ osPrintf(" userRxCbWrapper: no buffers for receive\n");
            return GT_FAIL;
        }

        /* get actual data pointer */
        sbufDataGet(bufferId, &data_PTR, &data_size);

        data_PTR += SBSP_BUFFER_PSS_MANAGEMENT_OFFSET_CNS;
        /* assume that simulation get only one segment */
        /* memcpy */ osMemCpy(data_PTR,segmentList[0],segmentLen[0]);


        return userRxCallback(&data_PTR, segmentLen , 1, 0);
    }
    else
        return GT_FAIL;
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
 *      rxBufPoolSize   - buffer pool size
 *      rxBufPool_PTR   - the address of the pool
 *      rxQNum          - the number of RX queues
 *      rxQbufPercentage- the buffer percentage dispersal for all queues
 *      rxBufSize       - the buffer requested size
 *      numOfRxBufs_PTR - number of requested buffers, and actual buffers created
 *      headerOffset    - packet header offset size
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
    if(sbspBuffPool == NULL)
    {
        /* init the buffers pool */
        sbspBuffPool = sbufPoolCreate(SBSP_BUFFERS_NUMBER_CNS);
    }

    return GT_OK;
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
    if(sbspBuffPool == NULL)
    {
        /* init the buffers pool */
        sbspBuffPool = sbufPoolCreate(SBSP_BUFFERS_NUMBER_CNS);
    }

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
GT_BOOL extDrvEthRawSocketModeGet(GT_VOID)
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
    /* STUB for link */
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
    /* STUB for link */
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
    /* STUB for link */
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
)
{
    return GT_NOT_IMPLEMENTED;
}


/*******************************************************************************
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
    return GT_NOT_IMPLEMENTED;
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
    GT_STATUS status;

    status = skernelNicOutput(segmentList, segmentLen, numOfSegments);

    /* give some time for simulation for packets processing */
    osTimerWkAfter(1);

    if (userTxCallback && (GT_OK == status))
    {
        userTxCallback(segmentList, numOfSegments);
    }

    return GT_OK == status ? GT_OK : GT_FAIL;
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
    IN GT_Rx_FUNCPTR userRxFunc
)
{
    userRxCallback = userRxFunc;

    skernelNicRxBind(userRxCbWrapper);

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
    userTxCallback = userTxFunc;

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
    SBUF_BUF_ID     bufId;

    /* find the Buffer */
    bufId = sbufGetBufIdByData(sbspBuffPool,segmentList[0]);

    if(bufId == NULL)
    {
        /*printf*/ osPrintf(" extDrvEthRxPacketFree : FAILED to free packet\n");
        return GT_FAIL;
    }


    /* free the buffer */
    sbufFree(sbspBuffPool, bufId);

    return GT_OK;
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
    return GT_OK;
}

#else /*SIM_NIC_NOT_IMPLEMENTED*/

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
    return GT_NOT_IMPLEMENTED;
}

GT_STATUS extDrvEthPortTxInit
(
    IN GT_U32           numOfTxBufs
)
{
    return GT_NOT_IMPLEMENTED;
}

GT_STATUS extDrvEthPortEnable
(
    GT_VOID
)
{
    return GT_NOT_IMPLEMENTED;
}

GT_STATUS extDrvEthPortDisable
(
    GT_VOID
)
{
    return GT_NOT_IMPLEMENTED;
}

GT_STATUS extDrvEthPortRxCb
(
    IN GT_U8_PTR   segmentList[],
    IN GT_U32      segmentLen[],
    IN GT_U32      numOfSegments,
    IN GT_U32      queueNum
)
{
    return GT_NOT_IMPLEMENTED;
}

GT_STATUS extDrvEthPortTxEndCb
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           numOfSegments
)
{
    return GT_NOT_IMPLEMENTED;
}

GT_STATUS extDrvEthPortTx
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           txQueue
)
{
    return GT_NOT_IMPLEMENTED;
}

GT_STATUS extDrvEthInputHookAdd
(
    IN GT_Rx_FUNCPTR userRxFunc
)
{
    return GT_NOT_IMPLEMENTED;
}


GT_STATUS extDrvEthTxCompleteHookAdd
(
    IN GT_Tx_COMPLETE_FUNCPTR userTxFunc
)
{
    return GT_NOT_IMPLEMENTED;
}

GT_STATUS extDrvEthRxPacketFree
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           numOfSegments,
    IN GT_U32           queueNum
)
{
    return GT_NOT_IMPLEMENTED;
}


GT_STATUS extDrvEthPortTxModeSet
(
    extDrvEthTxMode_ENT    txMode
)
{
    return GT_NOT_IMPLEMENTED;
}
#endif  /*SIM_NIC_NOT_IMPLEMENTED*/

