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
* miiInf.h
*
* DESCRIPTION:
*       Interface for ethernet communication to MII-interface based ASICS.
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.2.2 $
*******************************************************************************/

#ifndef __INCmiiInfh
#define __INCmiiInfh

#define MAC_SRC_ADDR_SIZE       6
#define MARVELL_HEADER_SIZE     2

#define RESERVED_BIT_31        (1<<31)
#define RESERVED_BIT_15        (1<<15)
#define HDM_PASS               (1<<14)
#define UNIMAC_EN              (1<<7)
#define PROMISCUOUS_MODE       (1<<3)
#define PROMISCUOUS_UNICAST     1
#define UINIMAC_PCR_VALUE (RESERVED_BIT_31|RESERVED_BIT_15|HDM_PASS|UNIMAC_EN|PROMISCUOUS_MODE|PROMISCUOUS_UNICAST)/*0x8000C089*/

#define IN
#define GT_FAIL -1
#define GT_OK 0
#ifndef NULL
#define NULL 0
#endif

typedef unsigned long GT_BOOL;
typedef unsigned long GT_STATUS;
typedef unsigned char GT_U8;
typedef unsigned short GT_U16;
typedef unsigned long GT_U32;

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
    IN GT_U8     *packet_PTR,
    IN GT_U32     packetLen
);


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
);

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
);

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
);

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
);

#endif /* __INCmiiInfh */
  

