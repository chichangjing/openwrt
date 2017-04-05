/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChDiagPortSlicesDirectCtrl.c
*
* DESCRIPTION:
*       Wrapper functions for CPSS DXCH Port AP API
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

#if (defined _linux)

typedef enum
{
    CMD_APP_DEMO_ZARLINK_FREQ_10M_E,
    CMD_APP_DEMO_ZARLINK_FREQ_25M_E,
    CMD_APP_DEMO_ZARLINK_FREQ_31_25M_E,
    CMD_APP_DEMO_ZARLINK_FREQ_39_0625M_E,
    CMD_APP_DEMO_ZARLINK_FREQ_50M_E,
    CMD_APP_DEMO_ZARLINK_FREQ_62_5M_E,
    CMD_APP_DEMO_ZARLINK_FREQ_64_453125M_E,
    CMD_APP_DEMO_ZARLINK_FREQ_78_125M_E,
    CMD_APP_DEMO_ZARLINK_FREQ_100M_E,
    CMD_APP_DEMO_ZARLINK_FREQ_125M_E
}CMD_APP_DEMO_ZARLINK_FREQ_E;


/* Feature specific includes. */
extern GT_STATUS appDemoZarlinkRefFreqSet
(
    IN GT_U32 devSlvId,
    IN GT_U8  refClkSrc,
    IN CMD_APP_DEMO_ZARLINK_FREQ_E freq
);

extern GT_STATUS appDemoZarlinkDpllStatusGet
(
    IN  GT_U32 devSlvId,
    IN  GT_U8  dpllSrc,
    OUT GT_BOOL  *dpllHoldoverPtr,
    OUT GT_BOOL  *dpllLockPtr
);

extern GT_STATUS appDemoZarlinkRefClkSelect
(
    IN GT_U32 devSlvId,
    IN GT_U8  dpllSrc,
    IN GT_U8  refClkSrc
);

extern GT_STATUS appDemoZarlinkReadByte
(
    IN GT_U32 devSlvId,
    IN GT_U32 offset,
    OUT GT_U32 *dataPtr
);

extern GT_STATUS appDemoZarlinkWriteByte
(
    IN GT_U32 devSlvId,
    IN GT_U32 offset,
    IN GT_U32 data
);

extern GT_STATUS appDemoZarlinkRead
(
    IN GT_U32 devSlvId,
    IN GT_U32 dataLen,
    IN GT_U32 offset,
    OUT GT_U32 *dataPtr
);

/*******************************************************************************
* appDemoZarlinkRefFreqSet
*
* DESCRIPTION:
*       Configures the input frequency parameters.
*
* INPUTS:
*       devSlvId    - ZARLINK I2C address.
*       refClkSrc   - selects which ref clock is configured.
*                   (APPLICABLE RANGES: 0..10)
*       freq        - input reference frequency in Hz should be selected
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrAppDemoZarlinkRefFreqSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U32                      devSlvId;
    GT_U8                       refClkSrc;
    CMD_APP_DEMO_ZARLINK_FREQ_E freq;

   /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devSlvId = (GT_U32)inArgs[0];
    refClkSrc = (GT_U8)inArgs[1];
    freq = (CMD_APP_DEMO_ZARLINK_FREQ_E)inArgs[2];

    result = appDemoZarlinkRefFreqSet(devSlvId, refClkSrc, freq);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* appDemoZarlinkRefClkSelect
*
* DESCRIPTION:
*       Selects manualy which ref clk is used by the DPLL.
*
* INPUTS:
*       devSlvId    - ZARLINK I2C address.
*       dpllSrc     - selects which DPLL is configured.
*                   (APPLICABLE RANGES: 0..1)
*       refClkSrc   - selects which ref clock is configured.
*                   (APPLICABLE RANGES: 0..10)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrAppDemoZarlinkRefClkSelect
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U32      devSlvId;
    GT_U8       dpllSrc;
    GT_U8       refClkSrc;

   /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devSlvId = (GT_U32)inArgs[0];
    dpllSrc = (GT_U8)inArgs[1];
    refClkSrc = (GT_U8)inArgs[2];

    result = appDemoZarlinkRefClkSelect(devSlvId, dpllSrc, refClkSrc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* appDemoZarlinkDpllStatusGet
*
* DESCRIPTION:
*       Reads the Lock and holdover indication of the DPLLs
*
* INPUTS:
*       devSlvId    - ZARLINK I2C address.
*       dpllSrc     - selects which DPLL is configured.
*                   (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       dpllHoldoverPtr - (pointer to) holdover status of DPLL
*       dpllLockPtr     - (pointer to) lock status of DPLL
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrAppDemoZarlinkDpllStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U32      devSlvId;
    GT_U8       dpllSrc;
    GT_BOOL     dpllHoldover = GT_FALSE;
    GT_BOOL     dpllLock = GT_FALSE;

   /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devSlvId = (GT_U32)inArgs[0];
    dpllSrc = (GT_U8)inArgs[1];

    result = appDemoZarlinkDpllStatusGet(devSlvId, dpllSrc, &dpllHoldover, &dpllLock);

   /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", dpllHoldover, dpllLock);

    return CMD_OK;
}

/*******************************************************************************
* appDemoZarlinkRead
*
* DESCRIPTION:
*       Reads from ZARLINK
*
* INPUTS:
*       devSlvId - ZARLINK I2C address.
*       dataLen - number of bytes to read (APPLICABLE RANGES: 0..255)
*       offset - regester offset
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrAppDemoZarlinkReadByte
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U32      devSlvId;
    GT_U32      offset;
    GT_U32      data = 0;

   /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devSlvId = (GT_U32)inArgs[0];
    offset = (GT_U32)inArgs[1];

    result = appDemoZarlinkReadByte(devSlvId, offset, &data);

   /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", data);

    return CMD_OK;

}

/*******************************************************************************
* appDemoZarlinkWrite
*
* DESCRIPTION:
*       Reads from ZARLINK
*
* INPUTS:
*       devSlvId - ZARLINK I2C address.
*       dataLen - number of bytes to read
*       offset - regester offset
*       dataPtr - (pointer to) data to be written
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrAppDemoZarlinkWriteByte
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U32      devSlvId;
    GT_U32      offset;
    GT_U32      data;

   /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devSlvId = (GT_U32)inArgs[0];
    offset = (GT_U32)inArgs[1];
    data = (GT_U32)inArgs[2];

    result = appDemoZarlinkWriteByte(devSlvId, offset, data);

   /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/* dump table iterator */
GT_U32 i2cDumpDataLen = 0;
GT_U32 i2cDumpCurrentOffset = 0;

static CMD_STATUS wrAppDemoZarlinkDumpGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U32      devSlvId;
    GT_U32      offset;
    GT_U32      data[4] = {0};

   /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devSlvId = (GT_U32)inArgs[0];
    offset = (GT_U32)inArgs[1];

    if(i2cDumpCurrentOffset >= i2cDumpDataLen)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK; 
    }

    result = appDemoZarlinkRead(devSlvId, 4, (offset + i2cDumpCurrentOffset), data);
    /* check for valid arguments */
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    } 

    fieldOutput("%d%d%d%d%d",(offset + i2cDumpCurrentOffset),data[0],data[1],data[2],data[3]);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f"); 

    i2cDumpCurrentOffset += 4;

    return CMD_OK;

}

static CMD_STATUS wrAppDemoZarlinkDumpGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

   /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    i2cDumpDataLen = (GT_U32)inArgs[2];
    i2cDumpCurrentOffset = 0;

    /* update dataLen to multiplication of 4 */
    if(i2cDumpDataLen % 4 != 0)
    {
        i2cDumpDataLen = ((i2cDumpDataLen >> 2) + 1 ) << 2;
    }

    return wrAppDemoZarlinkDumpGetNext(inArgs, inFields, numFields, outArgs);
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"appDemoZarlinkDpllStatusGet",
      wrAppDemoZarlinkDpllStatusGet,
      2, 0},

    {"appDemoZarlinkRefClkSelect",
      wrAppDemoZarlinkRefClkSelect,
      3, 0},

    {"appDemoZarlinkRefFreqSet",
      wrAppDemoZarlinkRefFreqSet,
      3, 0},

    {"appDemoZarlinkReadByte",
      wrAppDemoZarlinkReadByte,
      2, 0},

    {"appDemoZarlinkWriteByte",
      wrAppDemoZarlinkWriteByte,
      3, 0},

    {"appDemoZarlinkDumpGetFirst",
      wrAppDemoZarlinkDumpGetFirst,
      3, 0},

    {"appDemoZarlinkDumpGetNext",
      wrAppDemoZarlinkDumpGetNext,
      3, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitAppDemoZarlink
*
* DESCRIPTION:
*     Library database initialization function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     GT_OK   - on success.
*     GT_FAIL - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
GT_STATUS cmdLibInitAppDemoZarlink
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

#endif
