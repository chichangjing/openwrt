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
* win32DragoniteDrv.c
*
* DESCRIPTION:
*       Includes DRAGONITE functions wrappers, for DRAGONITE manage
*
* DEPENDENCIES:
*       -   BSP.
*
* FILE REVISION NUMBER:
*       $Revision: 4 $
*
*******************************************************************************/
#include <private/simWin32/simDragonite/Globals.h>

#include <os/simTypesBind.h>
#define EXPLICIT_INCLUDE_TO_SIM_OS_H_FILES
#include <os/simOsTask.h>
#include <os/simOsIniFile.h>

#ifndef DRAGONITE_TYPE_A1

GT_STATUS bspDragoniteSWDownload
(
    IN  const GT_VOID *sourcePtr,
    IN  GT_U32         size
)
{
    /* to avoid warnings */
    sourcePtr;
    size;

    return GT_NOT_SUPPORTED;
}

GT_STATUS bspDragoniteEnableSet
(
    IN  GT_BOOL enable
)
{
    /* to avoid warnings */
    enable;

    return GT_NOT_SUPPORTED;
}

GT_STATUS bspDragoniteInit
(
    GT_VOID
)
{
    return GT_NOT_SUPPORTED;
}

GT_STATUS bspDragoniteSharedMemWrite
(
    IN  GT_U32         offset,
    IN  const GT_VOID *buffer,
    IN  GT_U32         length
)
{
    /* to avoid warnings */
    offset;
    buffer;
    length;

    return GT_NOT_SUPPORTED;
}

GT_STATUS bspDragoniteSharedMemRead
(
    IN  GT_U32   offset,
    OUT GT_VOID *buffer,
    IN  GT_U32   length
)
{
    /* to avoid warnings */
    offset;
    buffer;
    length;

    return GT_NOT_SUPPORTED;
}

GT_STATUS bspDragoniteSharedMemoryBaseAddrGet
(
    OUT GT_U32 *dtcmPtr
)
{
    /* to avoid warnings */
    dtcmPtr;

    return GT_NOT_SUPPORTED;
}

GT_STATUS bspDragoniteGetIntVec
(
    OUT GT_U32 *intVec
)
{
    /* to avoid warnings */
    intVec;

    return GT_NOT_SUPPORTED;
}

#else /* if DRAGONITE supported */

extern U32 Checksum(U8 *Ptr);

extern U32 intLine;

static GT_BOOL dragoniteSupported = GT_FALSE;
/*******************************************************************************
* bspDragoniteSWDownload
*
* DESCRIPTION:
*       Download new version of Dragonite firmware to Dragonite MCU
*
* INPUTS:
*       sourcePtr      - Pointer to memory where new version of Dragonite firmware resides.
*       size           - size of firmware to download to ITCM.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspDragoniteSWDownload
(
    IN  const GT_VOID *sourcePtr,
    IN  GT_U32         size
)
{
    if(!dragoniteSupported)
        return GT_NOT_SUPPORTED;
    else
        return GT_OK;
}

/*******************************************************************************
* bspDragoniteEnableSet
*
* DESCRIPTION: Enable/Disable DRAGONITE module
*
* INPUTS:
*       enable – GT_TRUE  – DRAGONITE MCU starts work with parameters set by application
*                GT_FALSE – DRAGONITE MCU stops function
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_INIT_ERROR - Failed to create packetRxThread task
*
* COMMENTS:
*       call after SW download
*
*******************************************************************************/
GT_STATUS bspDragoniteEnableSet
(
    IN  GT_BOOL enable
)
{
    if(!dragoniteSupported)
        return GT_NOT_SUPPORTED;

    /* if irq flag is 1, then interrupt set - otherwise no interrupt */
    INT_POLARITY = 1;

    /* run communication protocol simulation task */
    if (simOsTaskCreate(6/*GT_TASK_PRIORITY_NORMAL*/,
                        Task_HostComm,
                        NULL) <= 0)
    {
        printf("Failed to create packetRxThread task!\r\n");
        return GT_INIT_ERROR;
    }

    return GT_OK;
}


/*******************************************************************************
* bspDragoniteInit
*
* DESCRIPTION: Initialize DRAGONITE module
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK if successful, or
*       GT_BAD_PARAM - DRAGONITE interrupt line not defined in simulation ini-file
*
* COMMENTS:
*       Application will call this before firmware download
*
*******************************************************************************/
GT_STATUS bspDragoniteInit
(
    GT_VOID
)
{
    char param_str[SIM_OS_CNF_FILE_MAX_LINE_LENGTH_CNS];

    /* appDemo uses appDemoDb to see if dragonite supported */
    dragoniteSupported = GT_TRUE;

    /* read DRAGONITE interrupt line number */
    if(simOsGetCnfValue("dragonite",  "dragonite_int_line",
                        SIM_OS_CNF_FILE_MAX_LINE_LENGTH_CNS, param_str))
    {
        sscanf(param_str, "%u", &intLine);
    }
    else
    {
        return GT_BAD_PARAM;
    }

    /* set comm. protocol shared memory (not whole 8K) to 0xFF */
    memset(HOST_MSG_PTR, 0xFF, sizeof(t_HostMsg)); 

    return GT_OK;
}

/*******************************************************************************
* bspDragoniteSharedMemWrite
*
* DESCRIPTION:
*       Write a given buffer to the given offset in shared memory of DRAGONITE 
*        microcontroller.
*
* INPUTS:
*       offset  - Offset from beginning of shared memory
*       buffer  - The buffer to be written.
*       length  - Length of buffer in bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - otherwise.
*
* COMMENTS:
*       Only DTCM is reachable
*
*******************************************************************************/
GT_STATUS bspDragoniteSharedMemWrite
(
    IN  GT_U32         offset,
    IN  const GT_VOID *buffer,
    IN  GT_U32         length
)
{
    if(!dragoniteSupported)
        return GT_NOT_SUPPORTED;

    /* pay attention MAX_DATA_SIZE is not 8K it's 2304 */
    if (length > MAX_DATA_SIZE - offset) 
        return GT_BAD_PARAM;

    memcpy(HOST_MSG_PTR + offset, buffer, length);

    return GT_OK;
}

/*******************************************************************************
* bspDragoniteSharedMemRead
*
* DESCRIPTION:
*       Read a memory block from a given offset in shared memory of DRAGONITE
*        microcontroller.
*
* INPUTS:
*       offset  - Offset from beginning of shared memory
*       length  - Length of the memory block to read (in bytes).
*
* OUTPUTS:
*       buffer  - The read data.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_BAD_PARAM - otherwise.
*
* COMMENTS:
*       Only DTCM is reachanble
*
*******************************************************************************/
GT_STATUS bspDragoniteSharedMemRead
(
    IN  GT_U32   offset,
    OUT GT_VOID *buffer,
    IN  GT_U32   length
)
{
    if(!dragoniteSupported)
        return GT_NOT_SUPPORTED;

    /* pay attention MAX_DATA_SIZE is not 8K it's 2304 */
    if (length > MAX_DATA_SIZE - offset) 
        return GT_BAD_PARAM;

    memcpy(buffer, HOST_MSG_PTR + offset, length);

    return GT_OK;
}

/*******************************************************************************
* bspDragoniteSharedMemoryBaseAddrGet
*
* DESCRIPTION:
*       Get start address of DTCM
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       dtcmPtr - Pointer to beginning of DTCM where communication structures 
*                 must be placed
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspDragoniteSharedMemoryBaseAddrGet
(
    OUT GT_U32 *dtcmPtr
)
{
    if(!dragoniteSupported)
        return GT_NOT_SUPPORTED;

    *dtcmPtr = (GT_U32)&IrqData;

    return GT_OK;
}

/*******************************************************************************
* bspDragoniteGetIntVec
*
* DESCRIPTION:
*       This routine return the DRAGONITE interrupt vector.
*
* INPUTS:
*       None
*
* OUTPUTS:
*       intVec - DRAGONITE interrupt vector.
*
* RETURNS:
*       GT_OK      - on success.
*       GT_FAIL    - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS bspDragoniteGetIntVec
(
    OUT GT_U32 *intVec
)
{
    if(!dragoniteSupported)
        return GT_NOT_SUPPORTED;

    *intVec = intLine;

    return GT_OK;
}

#endif 
