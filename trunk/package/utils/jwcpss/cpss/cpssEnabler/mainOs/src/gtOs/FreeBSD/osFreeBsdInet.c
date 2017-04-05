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
* osFreeBsdInet.c
*
* DESCRIPTION:
*       Operating System wrapper for FreeBsd. Ineternet functions (in.h)
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 2 $
*******************************************************************************/

#include <gtOs/gtGenTypes.h>

#if (OS_TARGET) == FreeBSD && (OS_TARGET_RELEASE) == 4
	#include <sys/types.h>
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

/*******************************************************************************
* osNtohl
*
* DESCRIPTION:
*       convert long integer from network order to host order
*
* INPUTS:
*       data - long integer in network order
*
* OUTPUTS:
*       None
*
* RETURNS:
*       long integer in host order
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 osNtohl
(
    IN GT_U32   data
)
{
    return ntohl(data);;
}

/*******************************************************************************
* osHtonl
*
* DESCRIPTION:
*       convert long  integer from host order to network order
*
* INPUTS:
*       long integer in host order
*
* OUTPUTS:
*       None
*
* RETURNS:
*       long integer in network order
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U32 osHtonl
(
    IN GT_U32   data
)
{
    return htonl(data);
}

/*******************************************************************************
* osNtohs
*
* DESCRIPTION:
*       convert short integer from network order to host order
*
* INPUTS:
*       data - short integer in network order
*
* OUTPUTS:
*       None
*
* RETURNS:
*       short integer in host order
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U16 osNtohs
(
    IN GT_U16   data
)
{
    return ntohs(data);
}

/*******************************************************************************
* osHtons
*
* DESCRIPTION:
*       convert short integer from host order to network order
*
* INPUTS:
*       short integer in host order
*
* OUTPUTS:
*       None
*
* RETURNS:
*       short integer in network order
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_U16 osHtons
(
    IN GT_U16   data
)
{
    return htons(data);
}

/*******************************************************************************
* osInetNtoa
*
* DESCRIPTION:
*       convert an network address to dot notation, store it in a buffer.
*
* INPUTS:
*       ipAddr -  inet address 
*       
* OUTPUTS:
*       buf    -  where to return ASCII string
*
* RETURNS:
*       
*
* COMMENTS:
*       None
*
*******************************************************************************/
void    osInetNtoa
(
    IN  GT_U32      ipAddr,
    OUT GT_U8       *buf
)
{
    struct in_addr sa;
    char *str;

    if(buf == NULL)
        return;
    
    sa.s_addr = htonl(ipAddr);
    str = inet_ntoa(sa);
    strcpy((char*)buf, str);
}

