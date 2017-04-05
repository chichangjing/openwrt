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
* osFreeBsdSimEther.c
*
* DESCRIPTION:
*       Operating System wrapper for FreeBsd
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/
#include <gtOs/gtGenTypes.h>
#include <asicSimulation/SKernel/skernel.h>

/*******************************************************************************
* GT_Rx_FUNCP
*
* DESCRIPTION:
*       The prototype of the routine to be called after a packet was received
*
* INPUTS:
*       packet_PTR    - The recieved packet.
*       packetLen     - The recived packet len
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
typedef GT_BOOL (*GT_Rx_FUNCP)
(
    IN GT_U8_PTR  packet_PTR,
    IN GT_U32     packetLen
);

static GT_Rx_FUNCP userRxCallback = NULL;
/*******************************************************************************
* osEtherInit
*
* DESCRIPTION:
*       This function inits the internal data structure of the ethernet port
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
GT_STATUS osEtherInit
(
    void
)
{
    return GT_OK;
}
/*******************************************************************************
* osInputCallback
*
* DESCRIPTION:
*       This function is the internal Rx callback
*
* INPUTS:
*       pIf          - interface packet was received on
*       buffer_PTR   - received packet
*       bufferLen    - length of received packet
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
static GT_VOID osInputCallback
(
    IN GT_VOID		*pIf,
    IN GT_U8 *      buffer_PTR,
    IN GT_32        bufferLen
)
{
    
    /* Call the user callback routine */
    if (NULL == userRxCallback)
    {
        return;
    }
    userRxCallback(buffer_PTR, bufferLen);
}

/*******************************************************************************
* osEtherInputHookAdd
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
GT_STATUS osEtherInputHookAdd
(
    IN GT_Rx_FUNCP userRxFunc
)
{

	skernelNicRxBind((SKERNEL_NIC_RX_CB_FUN)osInputCallback);
	
	userRxCallback = userRxFunc;

    return GT_OK;
}

/*******************************************************************************
* osEtherOutput
*
* DESCRIPTION:
*       This function transmits an Ethenet packet to the Packet processor
*
* INPUTS:
*       header_PTR  - pointer the the 14 bytes packet header.
*       paylaod_PTR - pointer to the reset of the packet.
*       payloadLen  - length ot the payload.
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
GT_STATUS osEtherOutput
(
    IN GT_U8_PTR header_PTR[],
    IN GT_U32    paylaod_PTR[],
    IN GT_U32    payloadLen
)
{
    GT_STATUS status;

    status = skernelNicOutput(header_PTR, paylaod_PTR, payloadLen);

    return GT_OK == status ? GT_OK : GT_FAIL;
}

/*******************************************************************************
* osEtherInputHookDelete
*
* DESCRIPTION:
*       This unbinds the user Rx callback
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
GT_STATUS osEtherInputHookDelete
(
    void
)
{
	skernelNicRxUnBind((SKERNEL_NIC_RX_CB_FUN)osInputCallback);

    return GT_OK;
}

