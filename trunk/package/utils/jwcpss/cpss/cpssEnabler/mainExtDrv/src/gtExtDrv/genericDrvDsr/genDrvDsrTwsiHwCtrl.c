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
*       $Revision: 3 $
*******************************************************************************/

/*Includes*/

#ifdef GT_I2C

#include <gtExtDrv/drivers/gtTwsiHwCtrl.h>
#include <gtExtDrv/drivers/pssBspApis.h>

/* Macroes definitions */
#define TWSI_SLAVE_ADDR(data)     (data)

#define SALSA_DEVICE
#ifdef  SALSA_DEVICE
#define MV_ON_READ_SET_BITS31(data)    (data[0] |= 0x80)
#define MV_ON_WRITE_RESET_BIT31(data)  (data[0] &= 0x7F)
#define MV_RESET_BIT_30(data)          (data[0] &= 0xBF)
#else
#define MV_ON_READ_SET_BITS31(data)    (data[0] |= 0x00)
#define MV_ON_WRITE_RESET_BIT31(data)  (data[0] &= ~0x00)
#define MV_RESET_BIT_30(data)          (data[0] &= ~0x00)
#endif


static GT_STATUS mvLongToChar (IN GT_U32 src, 
                               OUT GT_U8 dst[4]);

static GT_STATUS mvCharToLong (IN GT_U8 src[4], 
                               OUT GT_U32 *dst);

static GT_STATUS mvConcatCharArray (IN GT_U8 src0[4], 
                                    IN GT_U8 src1[4], 
                                    OUT GT_U8 dst[8]);

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
    GT_STATUS status;  
    status = bspTwsiInitDriver();
    if (status != GT_OK) return GT_FAIL;

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
    GT_U8           regCharAddr[4];
    GT_U8           regCharData[4];
    GT_U8           regCharAddrData[8];
    
    GT_STATUS  status;  

    /*Phase 1: Master Drives Address and Data over TWSI*/
    mvLongToChar (regAddr, regCharAddr);
    MV_ON_WRITE_RESET_BIT31 (regCharAddr);
    MV_RESET_BIT_30 (regCharAddr);

    mvLongToChar (value, regCharData);
    mvConcatCharArray(regCharAddr,regCharData,regCharAddrData);

    status = bspTwsiWaitNotBusy();
    if (status != GT_OK) return GT_FAIL;

    status = bspTwsiMasterWriteTrans(TWSI_SLAVE_ADDR(devSlvId),
                                              regCharAddrData,8,GT_TRUE);
    if (status != GT_OK) return GT_FAIL;

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
    GT_U8           twsiRdDataBuff[4];
    GT_U8           regCharAddr[4];       

    GT_STATUS  status;  

    /*PHASE 1: Write Structure, Master drives regAddr over TWSI*/
    mvLongToChar(regAddr, regCharAddr);
    MV_ON_READ_SET_BITS31 (regCharAddr);
    MV_RESET_BIT_30 (regCharAddr);
    
    status = bspTwsiWaitNotBusy();
    if (status != GT_OK) return GT_FAIL;

    status = bspTwsiMasterWriteTrans(TWSI_SLAVE_ADDR(devSlvId),
                                              regCharAddr,4,GT_FALSE);
    if (status != GT_OK) return GT_FAIL;
    
    status = bspTwsiMasterReadTrans(TWSI_SLAVE_ADDR(devSlvId),
                                              twsiRdDataBuff,4,GT_TRUE);
    if (status != GT_OK) return GT_FAIL;
    
    mvCharToLong(twsiRdDataBuff, dataPtr);
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
    GT_STATUS  status;

    if( dataPtr == NULL )
        return GT_BAD_PTR;

    if( dataLen < 1 || dataLen > 8 )
        return GT_BAD_PARAM;

    status = bspTwsiWaitNotBusy();
    if (status != GT_OK) 
        return GT_FAIL;

    status = bspTwsiMasterWriteTrans(TWSI_SLAVE_ADDR(devSlvId), dataPtr, dataLen, GT_TRUE);

    return status;
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
    GT_STATUS  status;

    if( dataPtr == NULL )
        return GT_BAD_PTR;

    if( dataLen < 1 || dataLen > 8 )
        return GT_BAD_PARAM;

    status = bspTwsiWaitNotBusy();
    if (status != GT_OK) 
        return GT_FAIL;

    status = bspTwsiMasterReadTrans(TWSI_SLAVE_ADDR(devSlvId), dataPtr, dataLen, GT_TRUE);

    return status;
}

/*******************************************************************************
* mvLongToChar
*
* DESCRIPTION:
*       Transforms unsigned long int type to 4 separate chars.
*
* INPUTS:
*       src - source unsigned long integer.
*
* OUTPUTS:
*       dst - Array of 4 chars
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL   - on failure
*
* COMMENTS:
*           MSB is copied to dst[0]!!
*
*******************************************************************************/
static GT_STATUS mvLongToChar (IN GT_U32 src, 
                               OUT GT_U8 dst[4])
{
    GT_U32 i;

    for (i = 4 ; i > 0 ; i--)
    {
        dst[i-1] = (GT_U8) src & 0xFF;
        src>>=8;
    }

  return GT_OK;
}

/*******************************************************************************
* mvCharToLong
*
* DESCRIPTION:
*       Transforms an array of 4 separate chars to unsigned long integer type
*
* INPUTS:
*       src - Source Array of 4 chars
*
* OUTPUTS:
*       dst - Unsigned long integer number
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL   - on failure
*
* COMMENTS:
*           MSB resides in src[0]!!
*
*******************************************************************************/
static GT_STATUS mvCharToLong (IN GT_U8 src[4], 
                               OUT GT_U32 *dst)
{
    GT_U32  i;
    GT_U32  tempU32 = 0x00000000;    

    for (i = 4 ; i > 0 ; i--)
    {
        tempU32 += ((GT_U32)src[i-1]) << (8*(4-i));
    }

  *dst = tempU32;
  return GT_OK;
}


/*******************************************************************************
* mvConcatCharArrays
*
* DESCRIPTION:
*       Concatinate 2 Arrays of Chars to one Array of chars
*
* INPUTS:
*       src0 - Source Array of 4 chars long
*       src1 - Source Array of 4 chars long
*
* OUTPUTS:
*       dst - Conacatinated Array of 8 chars long {src1,src0}
*
* RETURNS:
*       GT_OK      - on success
*       GT_FAIL   - on failure
*
* COMMENTS:
*           None
*
*******************************************************************************/
static GT_STATUS mvConcatCharArray (IN GT_U8 src0[4], 
                                    IN GT_U8 src1[4], 
                                    OUT GT_U8 dst[8])
{
    GT_U32 i=0,j; /*Source and Dest Counters*/

    for (j=0; j < 8; j++)
    {
        if (j < 4)
            dst[j] = src0[i++];
        else if (j == 4)
        {
            dst[j] = src1[0];
            i=1;
        } else
            dst[j] = src1[i++];
    }
    return GT_OK;
}



#undef DEBUG_ONLY
#ifdef  DEBUG_ONLY

#include <stdio.h>

GT_STATUS _twsiwr
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr,
    IN GT_U32 value
)
{
    GT_STATUS rc;
    rc = hwIfTwsiWriteReg(devSlvId,regAddr,value);
    printf("write TWSI (%08lX) address -> %08lX data -> %08lX \n",
           devSlvId,regAddr,value);

    return rc;
}


GT_STATUS _twsird
(
    IN GT_U32 devSlvId,
    IN GT_U32 regAddr
)
{
    GT_U32 value = 0;
    GT_STATUS rc;
    rc = hwIfTwsiReadReg(devSlvId,regAddr,&value);
    printf("read TWSI (%08lX) address -> %08lX data -> %08lX \n",
           devSlvId,regAddr,value);

    return rc;
}

#endif /* DEBUG_ONLY */

#else /* GT_I2C */

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
    return GT_NOT_IMPLEMENTED;
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
	return GT_NOT_IMPLEMENTED;
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
    return GT_NOT_IMPLEMENTED;
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
    return GT_NOT_IMPLEMENTED;
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
    return GT_NOT_IMPLEMENTED;
}

#endif /* GT_I2C */

