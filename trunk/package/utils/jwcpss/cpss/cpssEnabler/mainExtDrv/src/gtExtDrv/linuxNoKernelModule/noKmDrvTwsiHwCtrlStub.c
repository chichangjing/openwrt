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
* noKmDrvTwsiHwCtrlStub.c
*
* DESCRIPTION:
*       API implementation for TWSI facilities.
*
* DEPENDENCIES:
*
*       $Revision: 1 $
*******************************************************************************/

/*Includes*/
#include <gtExtDrv/drivers/gtTwsiHwCtrl.h>
#include <gtExtDrv/drivers/pssBspApis.h>

/*******************************************************************************
* hwIfTwsiWriteReg
*
* DESCRIPTION:
*       Writes the unmasked bits of a register using TWSI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to write to.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiWriteReg
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    (void)devSlvId;
    (void)regAddr;
    (void)value;
    return GT_NOT_SUPPORTED;
}


/*******************************************************************************
* hwIfTwsiReadReg
*
* DESCRIPTION:
*       Reads the unmasked bits of a register using TWSI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to read from.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiReadReg
(               
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
)
{
    (void)devSlvId;
    (void)regAddr;
    (void)dataPtr;
    return GT_NOT_SUPPORTED;
}


/*******************************************************************************
* hwIfTwsiInitDriver
*
* DESCRIPTION:
*       Init the TWSI interface 
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK     - on success
*       GT_FAIL   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiInitDriver
(
    GT_VOID
)
{
    return GT_OK;
    return GT_NOT_SUPPORTED;
}


/*******************************************************************************
* hwIfTwsiWriteData
*
* DESCRIPTION:
*       Generic TWSI Write operation.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       dataPtr  - (pointer to) data to be send on the TWSI.
*                   dataPtr[0] - should include start offset 
*       dataLen  - number of bytes to send on the TWSI (from dataPtr).
*                  range: 1-9
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on hardware error
*       GT_BAD_PARAM    - on wrong parameters
*       GT_BAD_PTR      - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiWriteData
(
    IN GT_U32 devSlvId,
    IN GT_U8  *dataPtr,
    IN GT_U8  dataLen
)
{
    (void)devSlvId;
    (void)dataPtr;
    (void)dataLen;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* hwIfTwsiReadData
*
* DESCRIPTION:
*       Generic TWSI Read operation.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       dataLen  - number of bytes to recieve on the TWSI (into dataPtr).
*                  range: 1-8.
*       dataPtr  - dataPtr[0] should include start offset
*
* OUTPUTS:
*       dataPtr  - (pointer to) data to be send on the TWSI.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on hardware error
*       GT_BAD_PARAM    - on wrong parameters
*       GT_BAD_PTR      - on null pointer
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiReadData
(               
    IN GT_U32 devSlvId,
    IN GT_U8  dataLen,
    INOUT GT_U8 *dataPtr
)
{
    (void)devSlvId;
    (void)dataLen;
    (void)dataPtr;
    return GT_NOT_SUPPORTED;
}

/*******************************************************************************
* hwIfTwsiWriteByte
*
* DESCRIPTION:
*       Writes the 8 bits of a register using TWSI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to write to.
*       value   - Data to be written to register.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK    - on success
*       GT_FAIL - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiWriteByte
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    (void)devSlvId;
    (void)regAddr;
    (void)value;
    return GT_NOT_SUPPORTED;
}


/*******************************************************************************
* hwIfTwsiReadByte
*
* DESCRIPTION:
*       Reads the 8 bits of a register using TWSI.
*
* INPUTS:
*       devSlvId - Slave Device ID
*       regAddr - Register address to read from.
*
* OUTPUTS:
*       dataPtr    - Data read from register.
*
* RETURNS:
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiReadByte
(               
    IN  GT_U32  devSlvId,
    IN  GT_U32  regAddr,
    OUT GT_U32 *dataPtr
)
{
    (void)devSlvId;
    (void)regAddr;
    (void)dataPtr;
    return GT_NOT_SUPPORTED;
}
