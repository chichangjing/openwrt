/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* osWin32Ether.c
*
* DESCRIPTION:
*       Operating System wrapper for Win32
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/
#include <gtOs/gtGenTypes.h>
#ifndef APPLICATION_SIDE_ONLY
    #include <asicsimulation/SKernel/skernel.h>
#else
    #define SIM_NIC_NOT_IMPLEMENTED
#endif /*!APPLICATION_SIDE_ONLY*/

#ifndef SIM_NIC_NOT_IMPLEMENTED
static SKERNEL_NIC_RX_CB_FUN  userRxCallback = NULL;
#endif/* !SIM_NIC_NOT_IMPLEMENTED*/
/*******************************************************************************
* osEtherInit
*
* DESCRIPTION:
*       This function inits the internal data structure of the Ethernet port
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
*       segmentList[]   - array of transmit data segments.
*       segmentLen[]    - array of transmit data segments length.
*       numOfSegments   - number of transmit data segments.
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
static GT_STATUS osInputCallback
(
    IN GT_U8_PTR   segmentList[],
    IN GT_U32      segmentLen[],
    IN GT_U32      numOfSegments
)
{

    /* Call the user callback routine */
#ifndef SIM_NIC_NOT_IMPLEMENTED
    if (NULL == userRxCallback)
    {
        return GT_BAD_PTR;
    }

    userRxCallback(segmentList, segmentLen, numOfSegments);
#endif/* !SIM_NIC_NOT_IMPLEMENTED*/

    return GT_OK;
}

/*******************************************************************************
* osEtherInputHookAdd
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
GT_STATUS osEtherInputHookAdd
(
#ifndef SIM_NIC_NOT_IMPLEMENTED
    IN SKERNEL_NIC_RX_CB_FUN userRxFunc
#else
    IN void* userRxFunc
#endif
)
{

#ifndef SIM_NIC_NOT_IMPLEMENTED
    skernelNicRxBind(osInputCallback);

    userRxCallback = userRxFunc;
#endif/* !SIM_NIC_NOT_IMPLEMENTED*/

    return GT_OK;
}

/*******************************************************************************
* osEtherOutput
*
* DESCRIPTION:
*       This function transmits an Ethernet packet to the Packet processor
*
* INPUTS:
*       segmentList[]   - array of transmit data segments.
*       segmentLen[]    - array of transmit data segments length.
*       numOfSegments   - number of transmit data segments.
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
    IN GT_U8_PTR   segmentList[],
    IN GT_U32      segmentLen[],
    IN GT_U32      numOfSegments
)
{

    GT_STATUS status=GT_FAIL;

#ifndef SIM_NIC_NOT_IMPLEMENTED
    status = skernelNicOutput(segmentList, segmentLen, numOfSegments);
#endif/* !SIM_NIC_NOT_IMPLEMENTED*/


    return (GT_OK == status) ? GT_OK : GT_FAIL;
}

/*******************************************************************************
* osEtherInputHookDelete
*
* DESCRIPTION:
*       This unbinds the user Rx callback
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
GT_STATUS osEtherInputHookDelete
(
    void
)
{

#ifndef SIM_NIC_NOT_IMPLEMENTED
    skernelNicRxUnBind(osInputCallback);
#endif/* !SIM_NIC_NOT_IMPLEMENTED*/

    return GT_OK;
}

