/*******************************************************************************
*              Copyright 2003, RADLAN Computer Communications, LTD.
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
* GALILEO TECHNOLOGY, INC. (GTI) AND RADLAN Computer Communications, LTD.
********************************************************************************
* miiInf.c
*
* DESCRIPTION:
*       Interface for ethernet communication to MII-interface based ASICS.
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.1 $
*******************************************************************************/

#include "miiInf.h"
#include "miiInf.h"


#define MII_PORT         1

/*******************************************************************************
* miiInfInit
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
GT_STATUS miiInfInit
(
    void
)
{
    return  GT_OK;
}

/*******************************************************************************
* miiInfOutput
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
GT_STATUS miiInfOutput
(
    IN GT_U8    *header_PTR,
    IN GT_U8    *paylaod_PTR,
    IN GT_U32    payloadLen
)
{
    return GT_OK;
}

/*******************************************************************************
* miiInfInputHookAdd
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
GT_STATUS miiInfInputHookAdd
(
    IN GT_Rx_FUNCP userRxFunc
)
{
    return GT_OK;
}

/*******************************************************************************
* miiInfInputHookDelete
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
GT_STATUS miiInfInputHookDelete
(
    void
)
{
    return GT_OK;
}
