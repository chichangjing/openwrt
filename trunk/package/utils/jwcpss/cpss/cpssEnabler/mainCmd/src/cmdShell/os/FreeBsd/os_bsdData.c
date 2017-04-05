/*******************************************************************************
*              (c), Copyright 2014, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* os_bsdData.c
*
* DESCRIPTION:
*       This file contains OS info functions
*
* FILE REVISION NUMBER:
*       $Revision: 1$
*
*******************************************************************************/
#include <cmdShell/os/cmdOs.h>

/*******************************************************************************
* cmdOsGetServerAndLocalIP
*
* DESCRIPTION:
*       Returns TFTP server IP address and local IP 
*
* INPUTS:
*       None
*
* OUTPUTS:
*       local_ip
*       server_ip
*
* RETURNS:
*       Status
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS cmdOsGetServerAndLocalIP(
  OUT GT_CHAR *local_ip,
  OUT GT_CHAR *server_ip
)
{
    return GT_FAIL;
}
