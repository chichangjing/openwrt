/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssOsInet.h
*
* DESCRIPTION:
*       Operating System wrapper. Internet functions (in.h)
*
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*******************************************************************************/

#ifndef __cpssOsIneth
#define __cpssOsIneth

#ifdef __cplusplus
extern "C" {
#endif

#include <cpss/extServices/os/gtOs/gtGenTypes.h>

/*******************************************************************************
* CPSS_OS_INET_NTOHL_FUNC
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
typedef GT_U32 (*CPSS_OS_INET_NTOHL_FUNC)
(
    IN GT_U32   data
);

/*******************************************************************************
* CPSS_OS_INET_HTONL_FUNC
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
typedef GT_U32 (*CPSS_OS_INET_HTONL_FUNC)
(
    IN GT_U32   data
);

/*******************************************************************************
* CPSS_OS_INET_NTOHS_FUNC
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
typedef GT_U16 (*CPSS_OS_INET_NTOHS_FUNC)
(
    IN GT_U16   data
);

/*******************************************************************************
* CPSS_OS_INET_HTONS_FUNC
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
typedef GT_U16 (*CPSS_OS_INET_HTONS_FUNC)
(
    IN GT_U16   data
);

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
typedef void    (*CPSS_OS_INET_NTOA_FUNC)
(
    IN  GT_U32      ipAddr,
    OUT GT_U8       *buf
);


/* CPSS_OS_INET_BIND_STC -
    structure that hold the "os Inet" functions needed be bound to cpss.

*/
typedef struct{
    CPSS_OS_INET_NTOHL_FUNC     osInetNtohlFunc;
    CPSS_OS_INET_HTONL_FUNC     osInetHtonlFunc;
    CPSS_OS_INET_NTOHS_FUNC     osInetNtohsFunc;
    CPSS_OS_INET_HTONS_FUNC     osInetHtonsFunc;
    CPSS_OS_INET_NTOA_FUNC      osInetNtoaFunc;

}CPSS_OS_INET_BIND_STC;

#ifdef __cplusplus
}
#endif

#endif /* __cpssOsIneth */

