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
#include "mtdApiRegs.h"
#include "mtdHwCntl.h"
#include "mtdAPI.h"
#include "mtdHwMsecCntl.h"


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

/* possible commands for accessing MacSec */
#define MTD_MACSEC_RD_CMD    1
#define MTD_MACSEC_WR_CMD    2
#define MTD_MACSEC_DBLWR_CMD 3

/* addresses for interfacing to MacSec indirectly */
#define MTD_MACSEC_CNTL        MTD_T_UNIT_PMA_PMD,0xC04A
#define MTD_MACSEC_ADDR_STATUS MTD_T_UNIT_PMA_PMD,0xC04B
#define MTD_MACSEC_HIGH_WORD   MTD_T_UNIT_PMA_PMD,0xC04C
#define MTD_MACSEC_LOW_WORD    MTD_T_UNIT_PMA_PMD,0xC04D

/* status of commands */
#define MTD_MACSEC_BUSY  0xF000
#define MTD_MACSEC_OK    0xF001
#define MTD_MACSEC_ERROR 0xFFFF /* can be caused by timeout, trying to read/write at odd address, MacSec in reset/sleep mode */
                                /* invalid command, address out of range, or trying to do a double write to an address not on a quad boundary */


MTD_STATUS mtdHwMsecWrite
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 dev,
    IN MTD_U16 reg,
    IN MTD_U32 value,
    IN MTD_BOOL useIndirectAddressing
)
{
    MTD_U16 readResult, cntr;

    if ((reg & 1) != 0)
    {
        return MTD_FAIL; /* reg must be even, since we are going to access reg and reg + 1 */
    }

    if (useIndirectAddressing == MTD_TRUE)
    {
        /* check if already busy with another command */
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_MACSEC_ADDR_STATUS,&readResult));

        if (readResult == MTD_MACSEC_BUSY)
        {
            return MTD_FAIL;
        }

        /* write 'write' command */
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_MACSEC_CNTL,MTD_MACSEC_WR_CMD));

        /* write data */
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_MACSEC_LOW_WORD,(MTD_U16)(value)));
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_MACSEC_HIGH_WORD,(MTD_U16)(value >> 16)));

        /* write address to start 'write' process */
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_MACSEC_ADDR_STATUS,reg));

        /* wait for done, error, or timeout */
        cntr = 0;
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_MACSEC_ADDR_STATUS,&readResult));
        while((readResult == MTD_MACSEC_BUSY) && (cntr++ < 10))
        {
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_MACSEC_ADDR_STATUS,&readResult));
        }

        if ((cntr == 10 && readResult == MTD_MACSEC_BUSY) || readResult == MTD_MACSEC_ERROR)
        {
            /* timed out or error */
            return MTD_FAIL;
        }

        /* write is done */
    }
    else
    {
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,dev,reg,(MTD_U16)(value & 0xFFFF)));
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,dev,reg+1,(MTD_U16)(value>>16)));
    }

    return MTD_OK;
}

MTD_STATUS mtdHwMsecRead
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 dev,
    IN MTD_U16 reg,
    OUT MTD_U32 *data,
    IN MTD_BOOL useIndirectAddressing
)
{
    MTD_U16 dummy, readResult, cntr;

    if ((reg & 1) != 0)
    {
        return MTD_FAIL; /* reg must be even, since we are going to access reg and reg + 1 */
    }

    if (useIndirectAddressing == MTD_TRUE)
    {
        /* check if already busy with another command */
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_MACSEC_ADDR_STATUS,&readResult));

        if (readResult == MTD_MACSEC_BUSY)
        {
            return MTD_FAIL;
        }

        /* write 'read' command */
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_MACSEC_CNTL,MTD_MACSEC_RD_CMD));

        /* write address to start 'read' process */
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_MACSEC_ADDR_STATUS,reg));

        /* wait for done, error, or timeout */
        cntr = 0;
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_MACSEC_ADDR_STATUS,&readResult));
        while((readResult == MTD_MACSEC_BUSY) && (cntr++ < 10))
        {
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_MACSEC_ADDR_STATUS,&readResult));
        }

        if ((cntr == 10 && readResult == MTD_MACSEC_BUSY) || readResult == MTD_MACSEC_ERROR)
        {
            /* timed out or error */
            return MTD_FAIL;
        }

        /* result ready, read it out */
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_MACSEC_LOW_WORD,&readResult));
        *data = (MTD_U32)readResult; /* assign low word */
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_MACSEC_HIGH_WORD,&readResult));
        *data |= (((MTD_U32)readResult) << 16);

    }
    else
    {
        /* first send the read request */
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,dev,reg,&dummy));

        /* now make sure the address read is correct */
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,dev,0x97FD,&dummy));

        if (dummy != reg)
        {
            return MTD_FAIL;
        }

        /* now read the low word */
        *data = 0;
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,dev,0x97FE,&dummy));
        *data = dummy;
        /* now read the high word */
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,dev,0x97FF,&dummy));
        *data |= (((MTD_U32)dummy)<<16);
    }

    return MTD_OK;
}

MTD_STATUS mtdHwMsecDoubleWrite
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16 port,
    IN MTD_U16 dev,
    IN MTD_U16 reg,
    IN MTD_U64 value,
    IN MTD_BOOL useIndirectAddressing
)
{
    MTD_U16 readResult, cntr;

    if ((reg & 0x3) != 0)
    {
        return MTD_FAIL; /* reg must be on 4 word boundary, since we are going to access reg to reg + 3 */
    }

    if (useIndirectAddressing == MTD_TRUE)
    {
        /* check if already busy with another command */
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_MACSEC_ADDR_STATUS,&readResult));

        if (readResult == MTD_MACSEC_BUSY)
        {
            return MTD_FAIL;
        }

        /* write 'write' command */
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_MACSEC_CNTL,MTD_MACSEC_DBLWR_CMD));

        /* write data */
        /* data should be in format 0xAAAABBBBCCCCDDDD where DDDD goes to addres reg, CCCC goes to address reg+1, */
        /* BBBB goes to address reg+2 and AAAA goes to address reg+3 */
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_MACSEC_LOW_WORD,(MTD_U16)(value >> 32))); /* write low word of reg + 2 */
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_MACSEC_HIGH_WORD,(MTD_U16)(value >> 48)));
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_MACSEC_LOW_WORD,(MTD_U16)(value)));       /* write low word of reg + 0 */
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_MACSEC_HIGH_WORD,(MTD_U16)(value >> 16)));

        /* write address to start 'write' process */
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,MTD_MACSEC_ADDR_STATUS,reg));

        /* wait for done, error, or timeout */
        cntr = 0;
        ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_MACSEC_ADDR_STATUS,&readResult));
        while((readResult == MTD_MACSEC_BUSY) && (cntr++ < 10))
        {
            ATTEMPT(mtdHwXmdioRead(contextPtr,port,MTD_MACSEC_ADDR_STATUS,&readResult));
        }

        if ((cntr == 10 && readResult == MTD_MACSEC_BUSY) || readResult == MTD_MACSEC_ERROR)
        {
            /* timed out or error */
            return MTD_FAIL;
        }

        /* write is done */

    }
    else
    {
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,dev,reg,(MTD_U16)(value & 0xFFFF)));
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,dev,reg+1,(MTD_U16)((value & 0xFFFF0000ULL)>>16)));
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,dev,reg+2,(MTD_U16)((value & 0xFFFF00000000ULL)>>32)));
        ATTEMPT(mtdHwXmdioWrite(contextPtr,port,dev,reg+3,(MTD_U16)((value & 0xFFFF000000000000ULL)>>48)));
    }

    return MTD_OK;
}


MTD_STATUS mtdHwGetMsecPhyRegField
(
    IN  CTX_PTR_TYPE contextPtr,
    IN  MTD_U16      port,
    IN  MTD_U16      dev,
    IN  MTD_U16      regAddr,
    IN  MTD_U8       fieldOffset,
    IN  MTD_U8       fieldLength,
    OUT MTD_U32      *data
)
{
    MTD_U32 dummy;

    if ((regAddr & 1) != 0)
    {
        return MTD_FAIL; /* reg must be even, since we are going to access reg and reg + 1 */
    }

    ATTEMPT(mtdHwMsecRead(contextPtr,port,dev,regAddr,&dummy,MTD_TRUE));

    ATTEMPT(mtdHwGetRegFieldFromLongWord(dummy,fieldOffset,fieldLength,data));

    return MTD_OK;
}

MTD_STATUS mtdHwSetMsecPhyRegField
(
    IN CTX_PTR_TYPE contextPtr,
    IN MTD_U16      port,
    IN MTD_U16      dev,
    IN MTD_U16      regAddr,
    IN MTD_U8       fieldOffset,
    IN MTD_U8       fieldLength,
    IN MTD_U32      data
)
{
    MTD_U32 dummy,dummy2;

    if ((regAddr & 1) != 0)
    {
        return MTD_FAIL; /* reg must be even, since we are going to access reg and reg + 1 */
    }

    ATTEMPT(mtdHwMsecRead(contextPtr,port,dev,regAddr,&dummy,MTD_TRUE));

    ATTEMPT(mtdHwMsecSetRegFieldToLongWord(dummy,data,fieldOffset,fieldLength,&dummy2));

    ATTEMPT(mtdHwMsecWrite(contextPtr,port,dev,regAddr,dummy2,MTD_TRUE));

    return MTD_OK;
}

MTD_STATUS mtdHwGetRegFieldFromLongWord
(
    IN  MTD_U32      regData,
    IN  MTD_U8       fieldOffset,
    IN  MTD_U8       fieldLength,
    OUT MTD_U32      *data
)
{
    MTD_U32 mask;

    /* do some range checking on the fieldOffset and fieldLength */
    if ((fieldOffset + fieldLength) > 32)
    {
        return MTD_FAIL;
    }

    mask = (((1<<(fieldOffset+fieldLength)))-(1<<fieldOffset));
    regData &= mask;
    *data = (regData>>fieldOffset);

    return MTD_OK;
}

MTD_STATUS mtdHwMsecSetRegFieldToLongWord
(
    IN  MTD_U32      regData,
    IN  MTD_U32      bitFieldData,
    IN  MTD_U8       fieldOffset,
    IN  MTD_U8       fieldLength,
    OUT MTD_U32      *data
)
{
    MTD_U32 mask;

    /* do some range checking on the fieldOffset and fieldLength */
    if ((fieldOffset + fieldLength) > 32)
    {
        return MTD_FAIL;
    }

    mask = (((1<<(fieldOffset+fieldLength)))-(1<<fieldOffset));

    /* Set the desired bits to 0.                       */
    regData &= ~mask;
    /* Set the given data into the above reset bits.    */
    regData |= ((bitFieldData << fieldOffset) & mask);

    *data = regData;

    return MTD_OK;
}
