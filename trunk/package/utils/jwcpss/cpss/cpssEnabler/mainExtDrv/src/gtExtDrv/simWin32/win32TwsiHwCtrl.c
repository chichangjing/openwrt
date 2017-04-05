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
* gtTwsiHwCtrl.c
*
* DESCRIPTION:
*       API implementation for TWSI facilities.
*
* DEPENDENCIES:
*
*       $Revision: 1.1.3 $
*******************************************************************************/

/*Includes*/

#include <gtExtDrv/drivers/gtTwsiDrvCtrl.h>
#include <gtExtDrv/drivers/gtTwsiHwCtrl.h>
#include <asicSimulation/SCIB/scib.h>


#define MAX_SLAVE_ID (256)

static GT_U32 i2cSimDevDb[MAX_SLAVE_ID];

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
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS hwIfTwsiInitDriver
(
    GT_VOID
)
{
    
    GT_U32     i2cSlave;
    GT_U32     simuDevId;

    for (i2cSlave = 0; i2cSlave < MAX_SLAVE_ID; i2cSlave++)
    {
        simuDevId = scibGetDeviceId(i2cSlave );
        i2cSimDevDb[ i2cSlave ] = simuDevId;      
    }
    
  
    return GT_OK;
}


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
*       GT_OK               - on success
*       GT_ERROR   - on hardware error
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
    scibWriteMemory(i2cSimDevDb[devSlvId], regAddr, 1, &value);
   
    return GT_OK;
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
*       GT_OK       - on success
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
    scibReadMemory(i2cSimDevDb[devSlvId], regAddr, 1, dataPtr);

    return GT_OK;
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
*       dataLen  - number of bytes to send on the TWSI (from dataPtr).
*                  range: 1-8.
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
    return GT_OK;
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
    OUT GT_U8 *dataPtr
)
{
    return GT_OK;
}

