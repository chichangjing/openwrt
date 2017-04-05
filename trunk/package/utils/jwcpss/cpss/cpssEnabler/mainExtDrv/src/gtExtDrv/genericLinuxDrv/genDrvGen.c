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
* noKmDrvGen.c
*
* DESCRIPTION:
*       Generic extDrv functions
*
* DEPENDENCIES:
*       None
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <gtExtDrv/drivers/gtGenDrv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "kerneldrv/include/presteraGlob.h"


/*************** Globals ******************************************************/

/* file descriptor returnd by openning the PP *nix device driver */
extern GT_32 gtPpFd;


/*******************************************************************************
* extDrvBoardIdGet
*
* DESCRIPTION:
*       Get BoardId value
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       boardIdPtr  - boardId
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - othersise.
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS extDrvBoardIdGet
(
    OUT GT_U32  *boardIdPtr
)
{
    mv_board_id_t id;

    if (ioctl (gtPpFd, PRESTERA_IOC_GETBOARDID, &id))
    {
        fprintf(stderr, "ioctl(PRESTERA_IOC_GETBOARDID) connect failed: errno(%s)\n",
                        strerror(errno));
        return GT_FAIL;
    }
    *boardIdPtr = (GT_U32)id;
    return GT_OK;
}
