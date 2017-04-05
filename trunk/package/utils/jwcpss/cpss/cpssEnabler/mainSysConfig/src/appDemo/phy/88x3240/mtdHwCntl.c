#include "mtdCopyright.h" 
/* Copyright 2013 Marvell International Ltd, See included file for licensing information */

/********************************************************************
This file contains functions and global data for
interfacing with the host's hardware-specific MDIO and general-purpose
IO in order to control and query the Marvell 88X3240 ethernet PHY.

These functions as written were tested with a USB-interface to
access Marvell's EVK. These functions must be ported to 
the host's specific platform.
********************************************************************/
#include "mtdApiTypes.h"
#include "mtdHwCntl.h"
#include "mtdAPI.h"

#define _GNU_SOURCE
#include <sched.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>

#if DB_TEST_ENVIRONMENT
#if 0
#include <stdio.h>
#include <windows.h> 
#include <malloc.h>
#include <comutil.h>
#endif
#include <Windows.h>
#include <WinError.h>
#include "tstSMIInterface.h"
#endif


#include <cpss/generic/smi/cpssGenSmi.h>

/****************************************************************************/

/*******************************************************************
  PORT THESE FUNCTIONS
 *******************************************************************/
/****************************************************************************/
MTD_STATUS mtdHwXmdioWrite
( 
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port, 
    IN MTD_U16 dev, 
    IN MTD_U16 reg, 
    IN MTD_U16 value
)
{
    MTD_STATUS result = MTD_OK;

#if DB_TEST_ENVIRONMENT 
    if (MdioWrite((unsigned long)port,
                  (unsigned long)dev, 
                  (unsigned long)reg, 
                  (unsigned short)value) < S_OK)
    {
        result = MTD_FAIL;
        MTD_DBG_INFO("write 0x%04X failed to port=%d, dev=%d, reg=0x%04X\n", (unsigned)(value),(unsigned)port,(unsigned)dev,(unsigned)reg);
    }

#else
	PRESTERA_INFO *switchInfo = contextPtr;
	
    if( 0 != cpssXsmiPortGroupRegisterWrite(switchInfo->devNum, ( 1 << switchInfo->portGroup), 0 /* interface 0 */,port, reg, dev, value) )
	{
    result = MTD_FAIL;
		printf("write 0x%04X failed to port=%d, dev=%d, reg=0x%04X\n", (unsigned)(value),(unsigned)port,(unsigned)dev,(unsigned)reg);
	}
#endif 


    return result;
}

MTD_STATUS mtdHwXmdioRead
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port, 
    IN MTD_U16 dev, 
    IN MTD_U16 reg,
    OUT MTD_U16 *data
)
{    
    MTD_STATUS result = MTD_OK;

#if DB_TEST_ENVIRONMENT     
    HRESULT retCode;

    *data = (MTD_U16)MdioRead((unsigned long)port,
                              (unsigned long)dev,
                              (unsigned long)reg,
                              &retCode);
    
    if (retCode < S_OK)
    {
        result = MTD_FAIL;
        MTD_DBG_INFO("read failed from port=%d, dev=%d, reg=0x%04X\n", (unsigned)port,(unsigned)dev,(unsigned)reg);
    }
#else
	PRESTERA_INFO *switchInfo = contextPtr;
	
    if( 0 != cpssXsmiPortGroupRegisterRead(switchInfo->devNum, ( 1 << switchInfo->portGroup), 0 /* interface 0 */, port, reg, dev, data) )
	{
    result = MTD_FAIL;
		printf("read failed from port=%d, dev=%d, reg=0x%04X\n", (unsigned)port,(unsigned)dev,(unsigned)reg);
	}
/* 	else
		printf("read from port=%d, dev=%d, reg=0x%04X  =  0x%04X\n", (unsigned)port,(unsigned)dev,(unsigned)reg, *data); */
#endif 

    return result;
}

/* 
    This macro calculates the mask for partial read/write of register's data.
*/
#define MTD_CALC_MASK(fieldOffset,fieldLen,mask)        \
            if((fieldLen + fieldOffset) >= 16)      \
                mask = (0 - (1 << fieldOffset));    \
            else                                    \
                mask = (((1 << (fieldLen + fieldOffset))) - (1 << fieldOffset))


MTD_STATUS mtdHwGetPhyRegField
(
    IN  CTX_PTR_TYPE contextPtr, 
    IN  MTD_U16      port, 
    IN  MTD_U16      dev, 
    IN  MTD_U16      regAddr,
    IN  MTD_U8       fieldOffset,
    IN  MTD_U8       fieldLength,
    OUT MTD_U16      *data
)
{
    MTD_U16 tmpData;
    MTD_STATUS   retVal;

    retVal = mtdHwXmdioRead(contextPtr, port, dev, regAddr, &tmpData);

    if(retVal != MTD_OK)
    {
        MTD_DBG_ERROR("Failed to read register \n");
        return MTD_FAIL;
    }
    
    mtdHwGetRegFieldFromWord(tmpData,fieldOffset,fieldLength, data);

    MTD_DBG_INFO("fOff %d, fLen %d, data 0x%04X.\n",(int)fieldOffset,(int)fieldLength,(int)*data); 

    return MTD_OK;
}

MTD_STATUS mtdHwSetPhyRegField
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16      port, 
    IN MTD_U16      dev, 
    IN MTD_U16      regAddr,
    IN MTD_U8       fieldOffset,
    IN MTD_U8       fieldLength,
    IN MTD_U16      data
)
{
    MTD_U16 tmpData,newData;
    MTD_STATUS   retVal;

    retVal = mtdHwXmdioRead(contextPtr, port, dev, regAddr, &tmpData);
    if(retVal != MTD_OK)
    {
        MTD_DBG_ERROR("Failed to read register \n");
        return MTD_FAIL;
    }

    mtdHwSetRegFieldToWord(tmpData,data,fieldOffset,fieldLength,&newData);

    retVal = mtdHwXmdioWrite(contextPtr, port, dev, regAddr, newData);

    if(retVal != MTD_OK)
    {
        MTD_DBG_ERROR("Failed to write register \n");
        return MTD_FAIL;
    }

    MTD_DBG_INFO("fieldOff %d, fieldLen %d, data 0x%x.\n",fieldOffset,\
                  fieldLength,data); 

    return MTD_OK;
}

MTD_STATUS mtdHwGetRegFieldFromWord
(
    IN  MTD_U16      regData,
    IN  MTD_U8       fieldOffset,
    IN  MTD_U8       fieldLength,
    OUT MTD_U16      *data
)
{
    MTD_U16 mask;            /* Bits mask to be read */

    MTD_CALC_MASK(fieldOffset,fieldLength,mask);

    *data = (regData & mask) >> fieldOffset;

    return MTD_OK;
}

MTD_STATUS mtdHwSetRegFieldToWord
(
    IN  MTD_U16      regData,
    IN  MTD_U16      bitFieldData,
    IN  MTD_U8       fieldOffset,
    IN  MTD_U8       fieldLength,
    OUT MTD_U16      *data
)
{
    MTD_U16 mask;            /* Bits mask to be read */

    MTD_CALC_MASK(fieldOffset,fieldLength,mask);

    /* Set the desired bits to 0.                       */
    regData &= ~mask;
    /* Set the given data into the above reset bits.    */
    regData |= ((bitFieldData << fieldOffset) & mask);

    *data = regData;

    return MTD_OK;
}


MTD_STATUS mtdWait
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_UINT x
)
{
#if DB_TEST_ENVIRONMENT            
    Sleep(x);
    return MTD_OK;
#else
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = x*1000000;
    nanosleep(&ts, NULL);
    return MTD_OK;
#endif
}

#if 0
#if DB_TEST_ENVIRONMENT
/***************************************************************************************** 
    The header file stdio.h needs to be included before compiling and using 
    this function

    See the "Example" section of the function declarations of mtdUpdateFlashImage() and 
    mtdMdioRamDownload() to understand how mtdOpenReadFile() can be used.
******************************************************************************************/
MTD_INT mtdOpenReadFile
(
    IN CTX_PTR_TYPE contextPtr, 
    IN MTD_U16 port,
    INOUT FILE **pfp, 
    IN MTD_8 *fname, 
    OUT MTD_U8 *data,
    IN MTD_UINT max_data_size
)
{
    int size;
 
    if ((*pfp = fopen(fname, "rb")) == (FILE *)0)
    {
        return 0; // error
    }
            
    size = (int)fread(data, sizeof(char), max_data_size, *pfp);

    if (ferror(*pfp) || size == 0)
    {
        return 0; // error
    }
    else
    {
        MTD_DBG_INFO("Read from %s, %d bytes\n", fname, size);
    }
    return size;
}
#endif
#endif


