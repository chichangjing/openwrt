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
 * noKmDrvRawSocketStub.c
 *
 * DESCRIPTION:
 *       This file includes stubs for raw socket support on
 *
 * DEPENDENCIES:
 *       None.
 *
 * FILE REVISION NUMBER:
 *       $Revision: 1 $
 *
 *******************************************************************************/
#include <cpss/generic/cpssTypes.h>
#include <gtExtDrv/drivers/gtEthPortCtrl.h>
#include <gtOs/gtOs.h>


/*******************************************************************************
* prvExtDrvSendRawSocket
*
* DESCRIPTION:
*       This function transmits a packet through the linux raw socket
*
* INPUTS:
*       segmentList     - A list of pointers to the packets segments.
*       segmentLen      - A list of segement length.
*       numOfSegments   - The number of segment in segment list.
*       txQueue         - The TX queue.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS prvExtDrvSendRawSocket
(
    IN GT_U8_PTR        segmentList[],
    IN GT_U32           segmentLen[],
    IN GT_U32           numOfSegments,
    IN GT_U32           txQueue
)
{
    return GT_NOT_SUPPORTED;
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
    return GT_NOT_SUPPORTED;
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
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
 * prvExtDrvRawSocketEnable
 *
 * DESCRIPTION: Enable raw socket interface
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
GT_STATUS prvExtDrvRawSocketEnable(GT_VOID)
{
    return GT_NOT_SUPPORTED;
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
    return GT_NOT_SUPPORTED;
}


/*******************************************************************************
* prvExtDrvRawSocketInit
*
* DESCRIPTION:
*       Initialize raw socket operations
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - if success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS prvExtDrvRawSocketInit(GT_VOID)
{
    return GT_NOT_SUPPORTED;
}
