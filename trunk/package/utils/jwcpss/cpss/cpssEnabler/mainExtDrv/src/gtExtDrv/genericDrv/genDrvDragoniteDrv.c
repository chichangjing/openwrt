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
* genDrvDragoniteDrv.c
*
* DESCRIPTION:
*       Includes DRAGONITE functions wrappers, for DRAGONITE manage
*
* DEPENDENCIES:
*       -   BSP.
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <gtExtDrv/drivers/gtDragoniteDrv.h>
#include <gtExtDrv/drivers/pssBspApis.h>

/*******************************************************************************
* extDrvDragoniteSharedMemWrite
*
* DESCRIPTION:
*       Write a given buffer to the given address in shared memory of DRAGONITE 
*       microcontroller.
*
* INPUTS:
*       offset     - Offset from beginning of shared memory
*       buffer      - The buffer to be written.
*       length      - Length of buffer in words.
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS extDrvDragoniteSharedMemWrite
(
    IN  GT_U32  offset,
    IN  GT_VOID *buffer,
    IN  GT_U32  length
)
{
    return bspDragoniteSharedMemWrite(offset, buffer, length);
}

/*******************************************************************************
* extDrvDragoniteSharedMemRead
*
* DESCRIPTION:
*       Read a data from the given address in shared memory of DRAGONITE microcontroller
*
* INPUTS:
*       offset     - Offset from beginning of shared memory
*       length     - Length of the memory block to read (in 
*                          words).
* OUTPUTS:
*       buffer  - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS extDrvDragoniteSharedMemRead
(
    IN  GT_U32  offset,
    IN  GT_U32  length,
    OUT GT_VOID *buffer
)
{
    return bspDragoniteSharedMemRead(offset, buffer, length);
}

/*******************************************************************************
* extDrvDragoniteShMemBaseAddrGet
*
* DESCRIPTION:
*       Get start address of communication structure in DTCM
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       dtcmPtr      - Pointer to beginning of DTCM, where
*                      communication structures must be placed
* RETURNS:
*       GT_OK if successful, or
*       GT_FAIL otherwise.
*
* COMMENTS:
*       None
*
*******************************************************************************/
GT_STATUS extDrvDragoniteShMemBaseAddrGet
(
    OUT  GT_U32  *dtcmPtr
)
{
    return bspDragoniteSharedMemoryBaseAddrGet(dtcmPtr);
}


