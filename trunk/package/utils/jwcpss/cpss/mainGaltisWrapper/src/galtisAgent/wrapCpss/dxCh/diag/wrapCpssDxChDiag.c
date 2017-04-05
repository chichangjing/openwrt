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
* wrapDiagCpssDxCh.c
*
* DESCRIPTION:
*       Wrapper functions for Diag cpss.dxCh functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 27 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiag.h>
#include <cpss/dxCh/dxChxGen/diag/cpssDxChDiagPacketGenerator.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* support for multi port groups */

/*******************************************************************************
* diagMultiPortGroupsBmpGet
*
* DESCRIPTION:
*       Get the portGroupsBmp for multi port groups device.
*       when 'enabled' --> wrappers will use the APIs
*       with portGroupsBmp parameter
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - (pointer to)enable / disable the use of APIs with portGroupsBmp parameter.
*       portGroupsBmpPtr - (pointer to)port groups bmp , relevant only when enable = GT_TRUE
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*******************************************************************************/
static void diagMultiPortGroupsBmpGet
(
    IN   GT_U8               devNum,
    OUT  GT_BOOL             *enablePtr,
    OUT  GT_PORT_GROUPS_BMP  *portGroupsBmpPtr
)
{
    /* default */
    *enablePtr  = GT_FALSE;
    *portGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if (0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return;
    }

    utilMultiPortGroupsBmpGet(devNum, enablePtr, portGroupsBmpPtr);
}

/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChDiagMemWrite
(
    IN GT_U8                          devNum,
    IN CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN GT_U32                         offset,
    IN GT_U32                         data
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    diagMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChDiagMemWrite(devNum, memType, offset, data);
    }
    else
    {
        return cpssDxChDiagPortGroupMemWrite(devNum, pgBmp, memType, offset,
                                             data);
    }
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChDiagMemRead
(
    IN  GT_U8                          devNum,
    IN  CPSS_DIAG_PP_MEM_TYPE_ENT      memType,
    IN  GT_U32                         offset,
    OUT GT_U32                         *dataPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    diagMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChDiagMemRead(devNum, memType, offset, dataPtr);
    }
    else
    {
        return cpssDxChDiagPortGroupMemRead(devNum, pgBmp, memType, offset,
                                            dataPtr);
    }
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChDiagRegsNumGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    *regsNumPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    diagMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChDiagRegsNumGet(devNum, regsNumPtr);
    }
    else
    {
        return cpssDxChDiagPortGroupRegsNumGet(devNum, pgBmp, regsNumPtr);
    }
}


/* Port Group aqnd Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChDiagRegsDump
(
    IN    GT_U8     devNum,
    INOUT GT_U32    *regsNumPtr,
    IN    GT_U32    offset,
    OUT   GT_U32    *regAddrPtr,
    OUT   GT_U32    *regDataPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    diagMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChDiagRegsDump(devNum, regsNumPtr, offset, regAddrPtr,
                                    regDataPtr);
    }
    else
    {
        return cpssDxChDiagPortGroupRegsDump(devNum, pgBmp, regsNumPtr,
                                             offset, regAddrPtr, regDataPtr);
    }
}

/*******************************************************************************
* cpssDxChDiagMemTest
*
* DESCRIPTION:
*       Performs memory test on a specified memory location and size for a
*       specified memory type.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       devNum      - The device number to test
*       memType     - The packet processor memory type to verify.
*       startOffset - The offset address to start the test from.
*       size        - The memory size in byte to test (start from offset).
*       profile     - The test profile.
*
* OUTPUTS:
*       testStatusPtr  - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr        - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr     - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr    - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong memory type
*       GT_FAIL      - on error
*
* COMMENTS:
*       The test is done by writing and reading a test pattern.
*       The function may be called after Phase 2 initialization.
*       The test is destructive and leaves the memory corrupted.
*       Supported memories:
*       - Buffer DRAM
*       - MAC table memory
*       - VLAN table memory
*       For buffer DRAM:
*          startOffset must be aligned to 64 Bytes and size must be in 64 bytes
*          resolution.
*       For MAC table:
*          startOffset must be aligned to 16 Bytes and size must be in 16 bytes
*          resolution.
*       For VLAN table:
*          DX CH devices: startOffset must be aligned to 12 Bytes and size must
*                         be in 12 bytes resolution.
*          DX CH2 devices: startOffset must be aligned to 16 Bytes and size must
*                          be in 16 bytes resolution.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagMemTest

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    CPSS_DIAG_PP_MEM_TYPE_ENT      memType;
    GT_U32                         startOffset;
    GT_U32                         size;
    CPSS_DIAG_TEST_PROFILE_ENT     profile;
    GT_BOOL                        testStatusPtr;
    GT_U32                         addrPtr;
    GT_U32                         readValPtr;
    GT_U32                         writeValPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DIAG_PP_MEM_TYPE_ENT)inArgs[1];
    startOffset = (GT_U32)inArgs[2];
    size = (GT_U32)inArgs[3];
    profile = (CPSS_DIAG_TEST_PROFILE_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssDxChDiagMemTest(devNum, memType, startOffset, size, profile,
                         &testStatusPtr, &addrPtr, &readValPtr, &writeValPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", testStatusPtr, addrPtr,
                                             readValPtr, writeValPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagAllMemTest
*
* DESCRIPTION:
*       Performs memory test for all the internal and external memories.
*       Tested memories:
*       - Buffer DRAM
*       - MAC table memory
*       - VLAN table memory
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       devNum      - The device number to test
*
* OUTPUTS:
*       testStatusPtr  - GT_TRUE if the test succeeded or GT_FALSE for failure
*       addrPtr        - Address offset of memory error, if testStatusPtr is
*                        GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       readValPtr     - Contains the value read from the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*       writeValPtr    - Contains the value written to the register which caused
*                        the failure if testStatusPtr is GT_FALSE. Irrelevant if
*                        testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The test is done by invoking cpssDxChDiagMemTest in loop for all the
*       memory types and for AA-55, random and incremental patterns.
*       The function may be called after Phase 2 initialization.
*       The test is destructive and leaves the memory corrupted.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagAllMemTest

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    GT_BOOL                        testStatusPtr;
    GT_U32                         addrPtr;
    GT_U32                         readValPtr;
    GT_U32                         writeValPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* to avoid warning */
    testStatusPtr = GT_FALSE;

    /* call cpss api function */
    result = cpssDxChDiagAllMemTest(devNum, &testStatusPtr, &addrPtr,
                                           &readValPtr, &writeValPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", testStatusPtr, addrPtr,
                                             readValPtr, writeValPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagMemWrite
*
* DESCRIPTION:
*       performs a single 32 bit data write to one of the PP memory spaces.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       devNum  - The device number
*       memType - The packet processor memory type
*       offset  - The offset address to write to
*       data    - data to write
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       offset must be aligned to 4 Bytes.
*       The function may be called after Phase 2 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagMemWrite

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    CPSS_DIAG_PP_MEM_TYPE_ENT      memType;
    GT_U32                         offset;
    GT_U32                         data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DIAG_PP_MEM_TYPE_ENT)inArgs[1];
    offset = (GT_U32)inArgs[2];
    data = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = pg_wrap_cpssDxChDiagMemWrite(devNum, memType, offset, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagMemRead
*
* DESCRIPTION:
*       performs a single 32 bit data read from one of the PP memory spaces.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       devNum  - The device number
*       memType - The packet processor memory type
*       offset  - The offset address to read from
*
* OUTPUTS:
*       dataPtr - read data
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       offset must be aligned to 4 Bytes.
*       The function may be called after Phase 2 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagMemRead

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    CPSS_DIAG_PP_MEM_TYPE_ENT      memType;
    GT_U32                         offset;
    GT_U32                         dataPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memType = (CPSS_DIAG_PP_MEM_TYPE_ENT)inArgs[1];
    offset = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChDiagMemRead(devNum, memType, offset, &dataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the PP PCI configuration or
*       registers.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       IfChannel  - interface channel (PCI/SMI/TWSI)
*       regType    - The register type
*       offset     - the register offset
*       data       - data to write
*       doByteSwap - GT_TRUE:  byte swap will be done on the written data
*                    GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagRegWrite

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U32                             baseAddr;
    CPSS_PP_INTERFACE_CHANNEL_ENT      ifChannel;
    CPSS_DIAG_PP_REG_TYPE_ENT          regType;
    GT_U32                             offset;
    GT_U32                             data;
    GT_BOOL                            doByteSwap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    baseAddr = (GT_U32)inArgs[0];
    ifChannel = (CPSS_PP_INTERFACE_CHANNEL_ENT)inArgs[1];
    regType = (CPSS_DIAG_PP_REG_TYPE_ENT)inArgs[2];
    offset = (GT_U32)inArgs[3];
    data = (GT_U32)inArgs[4];
    doByteSwap = (GT_BOOL)inArgs[5];

    /* call cpss api function */
    result = cpssDxChDiagRegWrite(baseAddr, ifChannel, regType,
                                      offset, data, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the PP PCI configuration or
*       registers.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       baseAddr   - The base address to access the device
*       IfChannel  - interface channel (PCI/SMI/TWSI)
*       regType    - The register type
*       offset     - the register offset
*       doByteSwap - GT_TRUE:  byte swap will be done on the read data
*                    GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       dataPtr    - read data
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagRegRead

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U32                             baseAddr;
    CPSS_PP_INTERFACE_CHANNEL_ENT      ifChannel;
    CPSS_DIAG_PP_REG_TYPE_ENT          regType;
    GT_U32                             offset;
    GT_U32                             dataPtr;
    GT_BOOL                            doByteSwap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    baseAddr = (GT_U32)inArgs[0];
    ifChannel = (CPSS_PP_INTERFACE_CHANNEL_ENT)inArgs[1];
    regType = (CPSS_DIAG_PP_REG_TYPE_ENT)inArgs[2];
    offset = (GT_U32)inArgs[3];
    doByteSwap = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssDxChDiagRegRead(baseAddr, ifChannel, regType,
                                      offset, &dataPtr, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagPhyRegWrite
*
* DESCRIPTION:
*       Performs single 32 bit data write to one of the PHY registers.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       baseAddr     - The base address to access the device
*       IfChannel    - interface channel (PCI/SMI/TWSI)
*       smiRegOffset - The SMI register offset
*       phyAddr      - phy address to access
*       offset       - PHY register offset
*       data         - data to write
*       doByteSwap   - GT_TRUE:  byte swap will be done on the written data
*                      GT_FALSE: byte swap will not be done on the written data
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPhyRegWrite

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U32                             baseAddr;
    CPSS_PP_INTERFACE_CHANNEL_ENT      ifChannel;
    GT_U32                             smiRegOffset;
    GT_U32                             phyAddr;
    GT_U32                             offset;
    GT_U32                             data;
    GT_BOOL                            doByteSwap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    baseAddr = (GT_U32)inArgs[0];
    ifChannel = (CPSS_PP_INTERFACE_CHANNEL_ENT)inArgs[1];
    smiRegOffset = (GT_U32)inArgs[2];
    phyAddr = (GT_U32)inArgs[3];
    offset = (GT_U32)inArgs[4];
    data = (GT_U32)inArgs[5];
    doByteSwap = (GT_BOOL)inArgs[6];

    /* call cpss api function */
    result = cpssDxChDiagPhyRegWrite(baseAddr, ifChannel, smiRegOffset,
                                     phyAddr, offset, data, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagPhyRegRead
*
* DESCRIPTION:
*       Performs single 32 bit data read from one of the PHY registers.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       baseAddr     - The base address to access the device
*       IfChannel    - interface channel (PCI/SMI/TWSI)
*       smiRegOffset - The SMI register offset
*       phyAddr      - phy address to access
*       offset       - PHY register offset
*       doByteSwap   - GT_TRUE:  byte swap will be done on the read data
*                      GT_FALSE: byte swap will not be done on the read data
*
* OUTPUTS:
*       dataPtr    - read data
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called before Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPhyRegRead

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U32                             baseAddr;
    CPSS_PP_INTERFACE_CHANNEL_ENT      ifChannel;
    GT_U32                             smiRegOffset;
    GT_U32                             phyAddr;
    GT_U32                             offset;
    GT_U32                             dataPtr;
    GT_BOOL                            doByteSwap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    baseAddr = (GT_U32)inArgs[0];
    ifChannel = (CPSS_PP_INTERFACE_CHANNEL_ENT)inArgs[1];
    smiRegOffset = (GT_U32)inArgs[2];
    phyAddr = (GT_U32)inArgs[3];
    offset = (GT_U32)inArgs[4];
    doByteSwap = (GT_BOOL)inArgs[5];

    /* call cpss api function */
    result = cpssDxChDiagPhyRegRead(baseAddr, ifChannel, smiRegOffset,
                                phyAddr, offset, &dataPtr, doByteSwap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagRegsNumGet
*
* DESCRIPTION:
*       Gets the number of registers for the PP.
*       Used to allocate memory for cpssDxChDiagRegsDump.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       devNum        - The device number
*
* OUTPUTS:
*       regsNumPtr    - number of registers
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagRegsNumGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U32                             regsNumPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChDiagRegsNumGet(devNum, &regsNumPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", regsNumPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagRegsDump
*
* DESCRIPTION:
*       Dumps the device register addresses and values.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       devNum        - The device number
*       regsNumPtr    - Number of registers to dump. This number must not be
*                       bigger than the number of registers that can be dumped
*                       (starting at offset).
*       offset        - The first register address to dump
*
* OUTPUTS:
*       regsNumPtr    - Number of registers that were dumped
*       regAddrPtr    - The addresses of the dumped registers
*       regDataPtr    - The data in the dumped registers
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       To dump all the registers the user may call cpssDxChDiagRegsNumGet in
*       order to get the number of registers of the PP.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagRegsDump

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      offset;
    GT_U32      regsNum;
    GT_U32      *regAddrPtr;
    GT_U32      *regDataPtr;
    GT_U32      tmpRegsNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    regsNum = tmpRegsNum= (GT_U32)inArgs[1];
    offset = (GT_U32)inArgs[2];

    regAddrPtr = cmdOsMalloc(regsNum * sizeof(GT_U32));
    if (regAddrPtr == NULL)
    {
        return CMD_AGENT_ERROR;
    }

    regDataPtr = cmdOsMalloc(regsNum * sizeof(GT_U32));
    if (regDataPtr == NULL)
    {
        cmdOsFree(regAddrPtr);
        return CMD_AGENT_ERROR;
    }

    /* call cpss api function */
    result = pg_wrap_cpssDxChDiagRegsDump(devNum, &tmpRegsNum, offset,
                                          regAddrPtr, regDataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", regsNum, regAddrPtr[(regsNum - 1)],
                 regDataPtr[(regsNum - 1)]);

    cmdOsFree(regAddrPtr);
    cmdOsFree(regDataPtr);
    return CMD_OK;
}


GT_STATUS wrCpssDxChDiagRegsPrint(int devNum)
{
    GT_U32 *regAddrPtr, *regDataPtr; /* pointers to arrays of reg addresses and data*/
    GT_U32 regNum, i; /* number of regs */
    GT_STATUS       result;

    /* call cpss api function */
    result = cpssDxChDiagRegsNumGet((GT_U8)devNum, &regNum);
    if(result != GT_OK)
    {
        return (int)result;
    }
    cmdOsPrintf("registers number %d\n\n", regNum);
    regAddrPtr = cmdOsMalloc(regNum * sizeof(GT_U32));
    regDataPtr = cmdOsMalloc(regNum * sizeof(GT_U32));


    result = cpssDxChDiagRegsDump((GT_U8)devNum, &regNum, 0, regAddrPtr, regDataPtr);


    if(result != GT_OK)
    {
        return result;
    }

    cmdOsPrintf("registers number %d\n\n", regNum);
    for(i = 0; i < regNum; i++)
    {
        cmdOsPrintf("register addr 0x%08X value 0x%08X\n", regAddrPtr[i], regDataPtr[i]);
    }

    cmdOsFree(regAddrPtr);
    cmdOsFree(regDataPtr);

    return GT_OK;
}

GT_STATUS wrCpssDxChDiagResetAndInitControllerRegsPrint(int devNum)
{
    GT_U32 *regAddrPtr, *regDataPtr; /* pointers to arrays of reg addresses and data*/
    GT_U32 regNum, i; /* number of regs */
    GT_STATUS       result;

    /* call cpss api function */
    result = cpssDxChDiagResetAndInitControllerRegsNumGet((GT_U8)devNum, &regNum);
    if(result != GT_OK)
    {
        return result;
    }
    cmdOsPrintf("registers number %d\n\n", regNum);
    regAddrPtr = cmdOsMalloc(regNum * sizeof(GT_U32));
    regDataPtr = cmdOsMalloc(regNum * sizeof(GT_U32));


    result = cpssDxChDiagResetAndInitControllerRegsDump((GT_U8)devNum, &regNum, 0, regAddrPtr, regDataPtr);


    if(result != GT_OK)
    {
        return result;
    }

    cmdOsPrintf("registers number %d\n\n", regNum);
    for(i = 0; i < regNum; i++)
    {
        cmdOsPrintf("register addr 0x%08X value 0x%08X\n", regAddrPtr[i], regDataPtr[i]);
    }

    cmdOsFree(regAddrPtr);
    cmdOsFree(regDataPtr);

    return GT_OK;
}


/*******************************************************************************
* cpssDxChDiagRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of a specific register.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       devNum        - The device number
*       regAddr       - Register to test
*       regMask       - Register mask. The test verifies only the non-masked
*                       bits.
*       profile       - The test profile
*
* OUTPUTS:
*       testStatusPtr - GT_TRUE if the test succeeded or GT_FALSE for failure
*       readValPtr    - The value read from the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*       writeValPtr   - The value written to the register if testStatusPtr is
*                       GT_FALSE. Irrelevant if testStatusPtr is GT_TRUE.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The function may be called after Phase 1 initialization.
*       After the register is tested, the original value prior to the test is
*       restored.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagRegTest

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U32                             regAddr;
    GT_U32                             regMask;
    CPSS_DIAG_TEST_PROFILE_ENT         profile;
    GT_BOOL                            testStatusPtr;
    GT_U32                             readValPtr;
    GT_U32                             writeValPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    regAddr = (GT_U32)inArgs[1];
    regMask = (GT_U32)inArgs[2];
    profile = (CPSS_DIAG_TEST_PROFILE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagRegTest(devNum, regAddr, regMask, profile,
                         &testStatusPtr, &readValPtr, &writeValPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", testStatusPtr, readValPtr,
                                                          writeValPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagAllRegTest
*
* DESCRIPTION:
*       Tests the device read/write ability of all the registers.
*
* APPLICABLE DEVICES:  All DX CH devices
*
* INPUTS:
*       devNum        - The device number
*
* OUTPUTS:
*       testStatusPtr - GT_TRUE if the test succeeded or GT_FALSE for failure
*       badRegPtr     - Address of the register which caused the failure if
*                       testStatusPtr is GT_FALSE. Irrelevant if testStatusPtr
*                       is GT_TRUE.
*       readValPtr    - The value read from the register which caused the
*                       failure if testStatusPtr is GT_FALSE. Irrelevant if
*                       testStatusPtr is GT_TRUE
*       writeValPtr   - The value written to the register which caused the
*                       failure if testStatusPtr is GT_FALSE. Irrelevant if
*                       testStatusPtr is GT_TRUE
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       The test is done by invoking cpssDxChDiagRegTest in loop for all the
*       diagnostics registers and for all the patterns.
*       After each register is tested, the original value prior to the test is
*       restored.
*       The function may be called after Phase 1 initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagAllRegTest

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_BOOL                            testStatusPtr;
    GT_U32                             badRegPtr;
    GT_U32                             readValPtr;
    GT_U32                             writeValPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChDiagAllRegTest(devNum, &testStatusPtr, &badRegPtr,
                                             &readValPtr, &writeValPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", testStatusPtr, badRegPtr,
                                               readValPtr, writeValPtr);
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChDiagPrbsPortTransmitModeSet
*
* DESCRIPTION:
*       Set transmit mode for specified port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*       mode     - transmit mode
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device or mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       CPU port doesn't support the transmit mode.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortTransmitModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNum;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];
    mode = (CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortTransmitModeSet(devNum, portNum, laneNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortTransmitModeGet
*
* DESCRIPTION:
*       Get transmit mode for specified port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       modePtr  - transmit mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       CPU port doesn't support the transmit mode.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortTransmitModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNum;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortTransmitModeGet(devNum, portNum, laneNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortGenerateEnableSet
*
* DESCRIPTION:
*       Enable/Disable PRBS (Pseudo Random Bit Generator) pattern generation
*       per Port and per lane.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*       enable   - GT_TRUE - PRBS pattern generation is enabled
*                  GT_FALSE - PRBS pattern generation is disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       When operating, enable the PRBS checker before the generator.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortGenerateEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortGenerateEnableSet(devNum, portNum, laneNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortGenerateEnableGet
*
* DESCRIPTION:
*       Get the status of PRBS (Pseudo Random Bit Generator) pattern generation
*       per port and per lane.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - PRBS pattern generation is enabled
*                     GT_FALSE - PRBS pattern generation is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortGenerateEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortGenerateEnableGet(devNum, portNum, laneNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable PRBS (Pseudo Random Bit Generator) checker per port and
*       per lane.
*       When the checker is enabled, it seeks to lock onto the incoming bit
*       stream, and once this is achieved the PRBS checker starts counting the
*       number of bit errors. Tne number of errors can be retrieved by
*       cpssDxChDiagPrbsGigPortStatusGet API.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*       enable   - GT_TRUE - PRBS checker is enabled
*                  GT_FALSE - PRBS checker is disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*       When operating, enable the PRBS checker before the generator.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortCheckEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortCheckEnableSet(devNum, portNum, laneNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortCheckEnableGet
*
* DESCRIPTION:
*       Get the status (enabled or disabled) of (Pseudo Random Bit Generator)
*       checker per port and per lane.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       enablePtr   - GT_TRUE - PRBS checker is enabled
*                     GT_FALSE - PRBS checker is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortCheckEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortCheckEnableGet(devNum, portNum, laneNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortCheckReadyGet
*
* DESCRIPTION:
*       Get the PRBS checker ready status.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
* OUTPUTS:
*       isReadyPtr  - GT_TRUE - PRBS checker is ready.
*                     PRBS checker has completed the initialization phase.
*                     GT_FALSE - PRBS checker is not ready.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the Tri-speed ports.
*       The Check ready status indicates that the PRBS checker has completed
*       the initialization phase. The PRBS generator at the transmit side may
*       be enabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortCheckReadyGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_BOOL isReady;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);


    /* call cpss api function */
    result = cpssDxChDiagPrbsPortCheckReadyGet(devNum, portNum, &isReady);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isReady);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsPortStatusGet
*
* DESCRIPTION:
*       Get PRBS (Pseudo Random Bit Generator) Error Counter and Checker Locked
*       status per port and per lane.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*       laneNum  - lane number, relevant only for XAUI/HGS port
*
* OUTPUTS:
*       checkerLockedPtr        - GT_TRUE - checker is locked on the sequence
*                                           stream.
*                                 GT_FALSE - checker isn't locked on the sequence
*                                           stream.
*       errorCntrPtr            - PRBS Error counter, this counter represents
*                                 the number of bit mismatches detected since
*                                 the PRBS checker of the port has locked.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       PRBS Error counter is cleared on read.
*       Applicable only for Tri-speed ports of all DxCh devices
*       and XAUI/HGS ports of DxCh3 and above devices.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsPortStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNum;
    GT_BOOL checkerLocked;
    GT_U32 errorCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    laneNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPrbsPortStatusGet(devNum, portNum, laneNum, &checkerLocked, &errorCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", checkerLocked, errorCntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsCyclicDataSet
*
* DESCRIPTION:
*       Set cylic data for transmition. See cpssDxChDiagPrbsPortTransmitModeSet.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*       laneNum         - lane number
*       cyclicDataArr   - cyclic data array
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the XAUI/HGS ports.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsCyclicDataSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNumber;
    GT_U32 cyclicDataArr[4];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    laneNumber = (GT_U32)inArgs[2];
    cyclicDataArr[0]=(GT_U32)inArgs[3];
    cyclicDataArr[1]=(GT_U32)inArgs[4];
    cyclicDataArr[2]=(GT_U32)inArgs[5];
    cyclicDataArr[3]=(GT_U32)inArgs[6];

    /* call cpss api function */
    result = cpssDxChDiagPrbsCyclicDataSet(devNum, portNum, laneNumber, cyclicDataArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsCyclicDataGet
*
* DESCRIPTION:
*       Get cylic data for transmition. See cpssDxChDiagPrbsPortTransmitModeSet.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*       laneNum         - lane number
*
* OUTPUTS:
*       cyclicDataArr   - cyclic data array
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Applicable only for the XAUI/HGS ports.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsCyclicDataGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNumber;
    GT_U32 cyclicDataArr[4];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    laneNumber = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPrbsCyclicDataGet(devNum, portNum, laneNumber, cyclicDataArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d",cyclicDataArr[0],cyclicDataArr[1],cyclicDataArr[2],
                                             cyclicDataArr[3]);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChDiagPrbsSerdesTestEnableSet
*
* DESCRIPTION:
*       Enable/Disable SERDES PRBS (Pseudo Random Bit Generator) test mode.
*
* APPLICABLE DEVICES: DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*       enable    - GT_TRUE - test enabled
*                   GT_FALSE - test disabled
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Transmit mode should be set before enabling test mode.
*       See test cpssDxChDiagPrbsSerdesTransmitModeSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsSerdesTestEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    laneNum = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChDiagPrbsSerdesTestEnableSet(devNum, portNum, laneNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsSerdesTestEnableGet
*
* DESCRIPTION:
*       Get the status of PRBS (Pseudo Random Bit Generator) test mode.
*
* APPLICABLE DEVICES: DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*
* OUTPUTS:
*       serdesPtr   - GT_TRUE - test enabled
*                     GT_FALSE - test disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsSerdesTestEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    laneNum = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChDiagPrbsSerdesTestEnableGet(devNum, portNum, laneNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsSerdesTransmitModeSet
*
* DESCRIPTION:
*       Set transmit mode for SERDES PRBS on specified port/lane.
*
* APPLICABLE DEVICES: DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
*       mode      - transmit mode
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*                                  or unsupported transmit mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Only CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E supported.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsSerdesTransmitModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNum;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    laneNum = (GT_U32)inArgs[2];
    mode = (CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChDiagPrbsSerdesTransmitModeSet(devNum, portNum, laneNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsSerdesTransmitModeGet
*
* DESCRIPTION:
*       Get transmit mode for SERDES PRBS on specified port/lane.
*
* APPLICABLE DEVICES: DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       portNum   - physical port number
*       laneNum   - lane number, relevant only for flexLink ports (0..3)
* OUTPUTS:
*       modePtr   - transmit mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on unkonown transmit mode
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsSerdesTransmitModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNum;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    laneNum = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChDiagPrbsSerdesTransmitModeGet(devNum, portNum, laneNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPrbsSerdesStatusGet
*
* DESCRIPTION:
*       Get SERDES PRBS (Pseudo Random Bit Generator) pattern detector state,
*       error counter and pattern counter.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       serdesNum - SERDES number
*
* OUTPUTS:
*       lockedPtr      - (pointer to) Pattern detector state.
*                         GT_TRUE - Pattern detector had locked onto the pattern.
*                         GT_FALSE - Pattern detector is not locked onto
*                                   the pattern.

*       errorCntrPtr   - (pointer to) PRBS Error counter. This counter represents
*                                 the number of bit mismatches detected since
*                                 the PRBS checker of the port has locked.
*       patternCntrPtr - (pointer to) Pattern counter. Number of 40-bit patterns
*                                 received since acquiring pattern lock.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number, device or lane number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - request is not supported for this port type
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPrbsSerdesStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM portNum;
    GT_U32 laneNum;
    GT_BOOL locked;
    GT_U32 errorCntr;
    GT_U64 patternCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    laneNum = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChDiagPrbsSerdesStatusGet(devNum, portNum, laneNum, &locked, &errorCntr, &patternCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    galtisOutput(outArgs, result, "%d%d%d%d", locked, errorCntr,patternCntr.l[0],patternCntr.l[1]);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagMemoryBistBlockStatusGet
*
* DESCRIPTION:
*       Gets redundancy block status.
*
* APPLICABLE DEVICES: DxCh3 only
*
* INPUTS:
*       devNum        - device number
*       memBistType   - type of memory
*       blockIndex    - block index
*                       CPSS_DIAG_PP_MEM_BIST_PCL_TCAM_E:
*                         valid blockIndex: 0 to 13.
*                       CPSS_DIAG_PP_MEM_BIST_ROUTER_TCAM_E:
*                         valid blockIndex: 0 to 19.
*
* OUTPUTS:
*       blockFixedPtr   - (pointer to) block status
*                                 GT_TRUE - row in the block was fixed
*                                 GT_FALSE - row in the block wasn't fixed
*       replacedIndexPtr - (pointer to) replaced row index.
*                                Only valid if  *blockFixedPtr is GT_TRUE.
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*          Each redundancy block contains 256 rows. Block 0: rows 0 to 255,
*          block 1: raws 256 to 511 ...
*          One raw replacement is possible though the full redundancy block
*          of 256 rows.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagMemoryBistBlockStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DIAG_PP_MEM_BIST_TYPE_ENT memBistType;
    GT_U32 blockIndex;
    GT_BOOL blockFixed;
    GT_U32 replacedIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memBistType = (CPSS_DIAG_PP_MEM_BIST_TYPE_ENT)inArgs[1];
    blockIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagMemoryBistBlockStatusGet(devNum, memBistType,
                                blockIndex, &blockFixed, &replacedIndex);

    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", blockFixed, replacedIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagMemoryBistsRun
*
* DESCRIPTION:
*       Runs BIST (Built-in self-test) on specified memory.
*
* APPLICABLE DEVICES: DxCh3 only
*
* INPUTS:
*       devNum        - device number
*       memBistType   - type of memory
*       timeOut       - maximal time in milisecond to wait for BIST finish.
*       clearMemoryAfterTest - GT_TRUE  - Clear memory after test.
*                              GT_FALSE - Don't clear memory after test.
* OUTPUTS:
*      None
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT    - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_UNFIXABLE_BIST_ERROR  - on unfixable problem in the TCAM
*
* COMMENTS:
*       1. This function should not be called under traffic.
*       2. Bist destroys the content of the memory. If clearMemoryAfterTest set
*          function will clear the content of the memory after the test.
*          Application responsible to restore the content after the function
*          completion.
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagMemoryBistsRun
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DIAG_PP_MEM_BIST_TYPE_ENT memBistType;
    GT_U32 timeOut;
    GT_BOOL clearMemoryAfterTest;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memBistType = (CPSS_DIAG_PP_MEM_BIST_TYPE_ENT)inArgs[1];
    timeOut = (GT_U32)inArgs[2];
    clearMemoryAfterTest = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChDiagMemoryBistsRun(devNum, memBistType, CPSS_DIAG_PP_MEM_BIST_PURE_MEMORY_TEST_E,
                                        timeOut, clearMemoryAfterTest, NULL, NULL);

    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagMemoryBistsRun1
*
* DESCRIPTION:
*       Runs BIST (Built-in self-test) on specified memory.
*
* APPLICABLE DEVICES: Lion
*
* INPUTS:
*       devNum        - device number
*       memBistType   - type of memory
*       bistTestType  - BIST test type (pure memory test/compare memory test)
*       timeOut       - maximal time in milisecond to wait for BIST finish.
*       clearMemoryAfterTest - GT_TRUE  - Clear memory after test.
*                              GT_FALSE - Don't clear memory after test.
*       testsToRunBmpPtr - bitmap of required test related for
*                               CPSS_DIAG_PP_MEM_BIST_COMPARE_TEST_C001_E
*                       (from 0 to CPSS_DIAG_PP_MEM_BIST_COMPARE_TESTS_NUM_CNS,
*                        so minimum bitmap size 32 bits, maximum depends on future
*                        number of tests); 1 - run test, 0 - don't run test.
*                        Bit 0 set in bitmap means run test 0.0 from "Table 1: TCAM Compare BIST:
*                                                   Parameters and Expected Result Configurations"
*                        (see full tests list and definitions in Application Notes)
*
* OUTPUTS:
*      testsResultBmpPtr - pointer to bitmap with results of compare memory test,
*                          !!! pay attention: 0 - test pass, 1 - test failed;
*                          if NULL, just final result of test will be returned;
*                          regarding size see testsToRunBmpPtr above
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT    - after max number of retries checking if PP ready
*       GT_UNFIXABLE_BIST_ERROR  - on unfixable problem in the TCAM
*
* COMMENTS:
*       1. This function should not be called under traffic.
*       2. Bist destroys the content of the memory. If clearMemoryAfterTest set
*          function will clear the content of the memory after the test.
*          Application responsible to restore the content after the function
*          completion.
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagMemoryBistsRun1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DIAG_PP_MEM_BIST_TYPE_ENT memBistType;
    CPSS_DIAG_PP_MEM_BIST_TEST_TYPE_ENT bistTestType;
    GT_U32 timeOut;
    GT_BOOL clearMemoryAfterTest;
    GT_U32 testsToRunBmp[1];
    GT_U32 testsResultBmp[1];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    memBistType     = (CPSS_DIAG_PP_MEM_BIST_TYPE_ENT)inArgs[1];
    bistTestType    = (CPSS_DIAG_PP_MEM_BIST_TEST_TYPE_ENT)inArgs[2];
    timeOut         = (GT_U32)inArgs[3];
    clearMemoryAfterTest = (GT_BOOL)inArgs[4];
    testsToRunBmp[0]   = (GT_BOOL)inArgs[5];

    /* call cpss api function */
    result = cpssDxChDiagMemoryBistsRun(devNum, memBistType, bistTestType,
                                        timeOut, clearMemoryAfterTest,
                                        testsToRunBmp, testsResultBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "0x%x", testsResultBmp[0]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagDeviceTemperatureSensorsSelectSet
*
* DESCRIPTION:
*       Select Temperature Sensors.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*       sensorType - Sensor type
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, sensorType
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDeviceTemperatureSensorsSelectSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_ENT  sensorType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    sensorType = (CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChDiagDeviceTemperatureSensorsSelectSet(devNum, sensorType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagDeviceTemperatureSensorsSelectGet
*
* DESCRIPTION:
*       Get Temperature Sensors Select.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       sensorTypePtr - Pointer to Sensor type
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDeviceTemperatureSensorsSelectGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    CPSS_DXCH_DIAG_TEMPERATURE_SENSOR_ENT  sensorType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChDiagDeviceTemperatureSensorsSelectGet(
        devNum, &sensorType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", sensorType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagDeviceTemperatureThresholdSet
*
* DESCRIPTION:
*       Set Threshold for Interrupt. If Temperature Sensors is equal or above
*       the threshold, interrupt is activated.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum      - device number
*       thresholdValue - Threshold value in Celsius degrees.
*                        (APPLICABLE RANGES: -142..228)
*
* OUTPUTS:
*       None.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong thresholdValue
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDeviceTemperatureThresholdSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_32  thresholdValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    thresholdValue = (GT_32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChDiagDeviceTemperatureThresholdSet(devNum, thresholdValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}


/*******************************************************************************
* cpssDxChDiagDeviceTemperatureThresholdGet
*
* DESCRIPTION:
*       Get Threshold for Interrupt. If Temperature Sensors is equal or above
*       the threshold, interrupt is activated.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       thresholdValuePtr - pointer to Threshold value in Celsius degrees.
*
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDeviceTemperatureThresholdGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_32    thresholdValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChDiagDeviceTemperatureThresholdGet(
        devNum, &thresholdValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", thresholdValue);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChDiagDeviceTemperatureGet
*
* DESCRIPTION:
*       Gets the PP temperature.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum          - device number
* OUTPUTS:
*       temperaturePtr  - (pointer to) temperature in Celsius degrees
*                         (can be negative)
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDeviceTemperatureGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_32     temperature;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    temperature = 0;

    /* call cpss api function */
    result = cpssDxChDiagDeviceTemperatureGet(
        devNum, &temperature);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", temperature);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChDiagDeviceTemperatureKelvinGet
*
* DESCRIPTION:
*       Gets the PP temperature.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum          - device number
* OUTPUTS:
*       temperaturePtr  - (pointer to) temperature in Kelvin degrees
*                         (can be negative)
* RETURNS :
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter
*       GT_BAD_PTR               - null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagDeviceTemperatureKelvinGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_32     temperature;
    GT_U32    temperatureKelvin;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    temperature = 0;

    /* call cpss api function */
    result = cpssDxChDiagDeviceTemperatureGet(
        devNum, &temperature);

    temperatureKelvin = temperature + 273;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", temperatureKelvin);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagSerdesTuningRxTune Get
*
* DESCRIPTION:
*       Gets serdes RX tuning values
*
* APPLICABLE DEVICES:  Xcat and above
*
* INPUTS:
*       devNum  - device number
*       portNum - global port number
*       laneBmp - bitmap of serdes lanes of portNum, where to generate PRBS signal
*       mode    - type of PRBS signal (not relevant if enable == GT_FALSE) [5-8]
*                 Now supported:
*                               CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS7_E
*                               CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS15_E
*                               CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS23_E
*                               CPSS_DXCH_DIAG_TRANSMIT_MODE_PRBS31_E
*       prbsTime - wait time till PRBS test finish (set 0 for CPSS default)
*       optMode  - optimization algorithm mode
*       dbgPrintEn  -   GT_TRUE - enable report print during run,
*                       GT_FALSE - disable report print.
*
* OUTPUTS:
*       optResultArrPtr - array of tuning result values
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
/* for now maximum number of lanes in Lion XLG */
#define DXCH_MAX_SERDES_LANES_PER_PORT 8

static GT_U32      portLaneArrLength;  /* length of (port,laneNum) array */
static CPSS_DXCH_DIAG_SERDES_TUNE_PORT_LANE_STC portLaneArr
                            [DXCH_MAX_SERDES_LANES_PER_PORT];
        /* array of (port,laneNum) pairs, where to run the test */
static GT_U32      laneNumber;          /* iterator */
static CPSS_DXCH_PORT_SERDES_TUNE_STC optResultArr
                    [DXCH_MAX_SERDES_LANES_PER_PORT];
                                    /* array of tuning result values */

static CMD_STATUS wrCpssDxChDiagSerdesTuningRxTuneGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                            devNum;
    GT_PHYSICAL_PORT_NUM             portNum;
    GT_U32                           laneBmp;
    CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT mode;
    GT_U32                           prbsTime;
    GT_BOOL                          dbgPrintEn;
    CPSS_DXCH_DIAG_SERDES_OPTIMIZE_MODE_ENT     optMode;

    GT_STATUS   rc;         /* return code */
    GT_U32      i,j;          /* iterator */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    laneBmp     = (GT_U32)inArgs[2];
    mode        = (CPSS_DXCH_DIAG_TRANSMIT_MODE_ENT)inArgs[3];
    prbsTime    = (GT_U32)inArgs[4];
    dbgPrintEn  = (GT_BOOL)inArgs[5];
    optMode     = (CPSS_DXCH_DIAG_SERDES_OPTIMIZE_MODE_ENT)inArgs[6];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    rc = cpssDxChDiagSerdesTuningTracePrintEnable(devNum,dbgPrintEn);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChDiagSerdesTuningTracePrintEnable");
        return CMD_OK;
    }

    rc = cpssDxChDiagSerdesTuningSystemInit(devNum);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChDiagSerdesTuningSystemInit");
        return CMD_OK;
    }

    for(i = 0, portLaneArrLength=0; i < 32; i++)
    {
        if(laneBmp & (1<<i))
        {
            portLaneArrLength++;
        }
    }

    for(i = 0,j=0; i < 32; i++)
    {
        if(laneBmp & (1<<i))
        {
            portLaneArr[j].portNum = portNum;
            portLaneArr[j++].laneNum = i;
        }
    }

    rc = cpssDxChDiagSerdesTuningRxTune(devNum, portLaneArr, portLaneArrLength,
                                        mode, prbsTime, optMode, optResultArr);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChDiagSerdesTuningRxTune");
        return CMD_OK;
    }

    inFields[0] = portLaneArr[0].laneNum;
    inFields[1] = optResultArr[0].dfe;
    inFields[2] = optResultArr[0].ffeC;
    inFields[3] = optResultArr[0].ffeR;
    inFields[4] = optResultArr[0].sampler;
    inFields[5] = optResultArr[0].sqlch;
    laneNumber = 1;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                    inFields[3], inFields[4], inFields[5]);
    galtisOutput(outArgs, rc, "%f");

    rc = cpssDxChDiagSerdesTuningSystemClose(devNum);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChDiagSerdesTuningSystemClose");
        return CMD_OK;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChDiagSerdesTuningRxTuneGetNext
*
* DESCRIPTION:
*
*   This function is called for ending table printing.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagSerdesTuningRxTuneGetNext

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if(laneNumber >= portLaneArrLength)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }
    else
    {
        inFields[0] = portLaneArr[laneNumber].laneNum;
        inFields[1] = optResultArr[laneNumber].dfe;
        inFields[2] = optResultArr[laneNumber].ffeC;
        inFields[3] = optResultArr[laneNumber].ffeR;
        inFields[4] = optResultArr[laneNumber].sampler;
        inFields[5] = optResultArr[laneNumber].sqlch;
        laneNumber++;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                        inFields[3], inFields[4], inFields[5]);
        galtisOutput(outArgs, GT_OK, "%f");
    }

    return CMD_OK;
}

static GT_U32   wrDxChPortNum = 0;

/*******************************************************************************
* wrCpssDxChDiagPacketGeneratorConnectGetNext
*
* DESCRIPTION:
*       Get the connect status of specified port.
*       Get packet generator's configurations.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number to connect
*                         its MAC to packet generator.
*
* OUTPUTS:
*       connectPtr      - (pointer to)
*                         GT_TRUE: port connected to packet generator.
*                         GT_FALSE: port not connected to packet generator.
*       configPtr       - (pointer to) packet generator configurations.
*                         Relevant only if enable==GT_TRUE
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPacketGeneratorConnectGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                 devNum;
    GT_BOOL                               connect;
    CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC  config;
    GT_STATUS  result = GT_OK;
    GT_BOOL showOnlyConnected;
    GT_PHYSICAL_PORT_NUM                  tempPortNum;

    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&config, 0, sizeof(CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    showOnlyConnected = (GT_BOOL)inArgs[1];

    /* add port convertion*/
    while(1)
    {
        if(wrDxChPortNum == CPSS_MAX_PORTS_NUM_CNS)
        {
            /* no ports */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        if (!PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) || PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, wrDxChPortNum))
        {
            tempPortNum = (GT_PHYSICAL_PORT_NUM)wrDxChPortNum;

            /* Override Device and Port */
            CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, tempPortNum);

            /* get port's attributes */
            result = cpssDxChDiagPacketGeneratorConnectGet(devNum,tempPortNum,&connect,&config);
            if ((result == GT_NOT_INITIALIZED) || ((showOnlyConnected == GT_TRUE) && (connect == GT_FALSE)))
            {
                wrDxChPortNum++;
                continue;
            }
            break;
        }
        else
        {
            wrDxChPortNum++;
        }
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = wrDxChPortNum;
    inFields[1] = connect;
    inFields[3] = config.macDaIncrementEnable;
    inFields[5] = config.vlanTagEnable;
    inFields[6] = config.vpt;
    inFields[7] = config.cfi;
    inFields[8] = config.vid;
    inFields[9] = config.etherType;
    inFields[10] = config.payloadType;
    inFields[12] = config.packetLengthType;
    inFields[13] = config.packetLength;
    inFields[14] = config.undersizeEnable;
    inFields[15] = config.transmitMode;
    inFields[16] = config.packetCount;
    inFields[17] = config.packetCountMultiplier;
    inFields[18] = config.ipg;

    /* pack and output table fields */
    fieldOutput("%d%d%6b%d%6b%d%d%d%d%d%d%8b%d%d%d%d%d%d%d",
                          inFields[0],  inFields[1],  config.macDa.arEther,  inFields[3],
                          config.macSa.arEther, inFields[5], inFields[6],
                          inFields[7],  inFields[8],  inFields[9],
                          inFields[10], config.cyclicPatternArr, inFields[12],
                          inFields[13], inFields[14], inFields[15],
                          inFields[16], inFields[17], inFields[18]);
    galtisOutput(outArgs, GT_OK, "%f");

    /* move to the next portNum */
    wrDxChPortNum++;
    return CMD_OK;

}

static CMD_STATUS wrCpssDxChDiagPacketGeneratorConnectGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    wrDxChPortNum = 0;
    return wrCpssDxChDiagPacketGeneratorConnectGetNext(inArgs,inFields,numFields,outArgs);

}



static CMD_STATUS wrCpssDxChDiagPacketGeneratorConnect_1GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                                 devNum;
    GT_BOOL                               connect;
    CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC  config;
    GT_STATUS  result = GT_OK;
    GT_BOOL showOnlyConnected;
    GT_PHYSICAL_PORT_NUM                  tempPortNum;

    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&config, 0, sizeof(CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    showOnlyConnected = (GT_BOOL)inArgs[1];

    /* add port convertion*/
    while(1)
    {
        if(wrDxChPortNum == CPSS_MAX_PORTS_NUM_CNS)
        {
            /* no ports */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        if (!PRV_CPSS_IS_DEV_EXISTS_MAC(devNum) || PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, wrDxChPortNum))
        {
            tempPortNum = (GT_PHYSICAL_PORT_NUM)wrDxChPortNum;

            /* Override Device and Port */
            CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, tempPortNum);

            /* get port's attributes */
            result = cpssDxChDiagPacketGeneratorConnectGet(devNum,tempPortNum,&connect,&config);
            if ((result == GT_NOT_INITIALIZED) || ((showOnlyConnected == GT_TRUE) && (connect == GT_FALSE)))
            {
                wrDxChPortNum++;
                continue;
            }
            break;
        }
        else
        {
            wrDxChPortNum++;
        }
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[ 0] = wrDxChPortNum;
    inFields[ 1] = connect;
    inFields[ 3] = config.macDaIncrementEnable;
    inFields[ 4] = config.macDaIncrementLimit;
    inFields[ 6] = config.vlanTagEnable;
    inFields[ 7] = config.vpt;
    inFields[ 8] = config.cfi;
    inFields[ 9] = config.vid;
    inFields[10] = config.etherType;
    inFields[11] = config.packetLengthType;
    inFields[12] = config.packetLength;
    inFields[13] = config.undersizeEnable;
    inFields[14] = config.transmitMode;
    inFields[15] = config.packetCount;
    inFields[16] = config.packetCountMultiplier;
    inFields[17] = config.ipg;
    inFields[18] = config.interfaceSize;
    inFields[19] = config.payloadType;
    inFields[20] = 8;

    /* pack and output table fields */
               /* 1 2  3 4 5 6 7 8 91011121314151617181920 */
    fieldOutput("%d%d%6b%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%8b",
                          inFields[0],  inFields[1],  
                          config.macDa.arEther,  inFields[3],inFields[4],       /* mac DA, increment enable, increment  limit */
                          config.macSa.arEther, 
                          inFields[6], inFields[7],   inFields[8],  inFields[9], /* vlan tag */
                          inFields[10],                                          /* ethernet type */
                          inFields[11], inFields[12], inFields[13],               /* packet length type, packet length, undersize enable */
                          inFields[14], inFields[15], inFields[16],               /* transmit mode, packet count, packet count multiplier */
                          inFields[17],                                           /* ipg */
                          inFields[18],                                           /* interface size */
                          inFields[19], inFields[20],config.cyclicPatternArr);

    galtisOutput(outArgs, GT_OK, "%f");

    /* move to the next portNum */
    wrDxChPortNum++;
    return CMD_OK;

}

static CMD_STATUS wrCpssDxChDiagPacketGeneratorConnect_1GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    wrDxChPortNum = 0;
    return wrCpssDxChDiagPacketGeneratorConnect_1GetNext(inArgs,inFields,numFields,outArgs);

}


/*******************************************************************************
* wrCpssDxChDiagPacketGeneratorConnectSet
*
* DESCRIPTION:
*       Connect/Disconnect port to packet generator.
*       Set packet generator's configurations.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number to connect
*                         its MAC to packet generator.
*       connect         - GT_TRUE: connect the given port's MAC to packet generator.
*                         GT_FALSE: disconnect the given port's MAC from
*                         packet generator; return the port's MAC
*                         to normal egress pipe.
*       configPtr       - (pointer to) packet generator configurations.
*                         Relevant only if enable==GT_TRUE
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. In case packet generator connected to other port the function perfroms
*          the following:
*          - stops traffic.
*          - connects packet generator to new port
*          - overrides packet generator's configurations
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPacketGeneratorConnectSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;

    GT_U8                                 devNum;
    GT_PHYSICAL_PORT_NUM                  portNum;
    GT_BOOL                               connect;
    CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC  config;
    GT_BYTE_ARRY cyclicBArr;

    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inFields[0];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    connect = (GT_BOOL)inFields[1];
    galtisMacAddr(&config.macDa, (GT_U8*)inFields[2]);
    config.macDaIncrementEnable = (GT_BOOL)inFields[3];
    config.macDaIncrementLimit  = 0;
    galtisMacAddr(&config.macSa, (GT_U8*)inFields[4]);
    config.vlanTagEnable = (GT_BOOL)inFields[5];
    config.vpt = (GT_U8)inFields[6];
    config.cfi = (GT_U8)inFields[7];
    config.vid = (GT_U16)inFields[8];
    config.etherType = (GT_U16)inFields[9];
    config.payloadType = (CPSS_DIAG_PG_PACKET_PAYLOAD_TYPE_ENT)inFields[10];
    galtisBArray(&cyclicBArr,(GT_U8*)inFields[11]);
    cmdOsMemCpy(config.cyclicPatternArr, cyclicBArr.data, 8);
    config.packetLengthType = (CPSS_DIAG_PG_PACKET_LENGTH_TYPE_ENT)inFields[12];
    config.packetLength = (GT_U32)inFields[13];
    config.undersizeEnable = (GT_BOOL)inFields[14];
    config.transmitMode = (CPSS_DIAG_PG_TRANSMIT_MODE_ENT)inFields[15];
    config.packetCount = (GT_U32)inFields[16];
    config.packetCountMultiplier = (CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_ENT)inFields[17];
    config.ipg = (GT_U32)inFields[18];
    config.interfaceSize = CPSS_DIAG_PG_IF_SIZE_DEFAULT_E;

    /* call cpss api function */
    result = cpssDxChDiagPacketGeneratorConnectSet(devNum,portNum,connect,&config);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;

}


static CMD_STATUS wrCpssDxChDiagPacketGeneratorConnect_1Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;

    GT_U8                                 devNum;
    GT_PHYSICAL_PORT_NUM                  portNum;
    GT_BOOL                               connect;
    CPSS_DXCH_DIAG_PG_CONFIGURATIONS_STC  config;
    GT_BYTE_ARRY cyclicBArr;

    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inFields[0];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    connect                      = (GT_BOOL)inFields[1];
    galtisMacAddr(&config.macDa,   (GT_U8*) inFields[2]);
    config.macDaIncrementEnable  = (GT_BOOL)inFields[3];
    config.macDaIncrementLimit   = (GT_U32) inFields[4];
    galtisMacAddr(&config.macSa,   (GT_U8*) inFields[5]);
    config.vlanTagEnable         = (GT_BOOL)inFields[6];
    config.vpt                   = (GT_U8)  inFields[7];
    config.cfi                   = (GT_U8)  inFields[8];
    config.vid                   = (GT_U16) inFields[9];
    config.etherType             = (GT_U16) inFields[10];
    config.packetLengthType      = (CPSS_DIAG_PG_PACKET_LENGTH_TYPE_ENT)inFields[11];
    config.packetLength          = (GT_U32) inFields[12];
    config.undersizeEnable       = (GT_BOOL)inFields[13];
    config.transmitMode          = (CPSS_DIAG_PG_TRANSMIT_MODE_ENT)inFields[14];
    config.packetCount           = (GT_U32) inFields[15];
    config.packetCountMultiplier = (CPSS_DIAG_PG_PACKET_COUNT_MULTIPLIER_ENT)inFields[16];
    config.ipg                   = (GT_U32) inFields[17];
    config.interfaceSize         = (CPSS_DIAG_PG_IF_SIZE_ENT)inFields[18]; 
    config.payloadType           = (CPSS_DIAG_PG_PACKET_PAYLOAD_TYPE_ENT)inFields[19];
    galtisBArray(&cyclicBArr,      (GT_U8*)inFields[21]);
    cmdOsMemCpy(config.cyclicPatternArr, cyclicBArr.data, 8);

    /* call cpss api function */
    result = cpssDxChDiagPacketGeneratorConnectSet(devNum,portNum,connect,&config);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;

}


/*******************************************************************************
* wrCpssDxChDiagPacketGeneratorTransmitEnable
*
* DESCRIPTION:
*       Enable/Disable(Start/Stop) transmission on specified port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*       enable          - GT_TRUE: enable(start) transmission
*                         GT_FALSE: disable(stop) transmission
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_STATE             - on port not connected to packet generator
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPacketGeneratorTransmitEnable
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_PHYSICAL_PORT_NUM     portNum;
    GT_BOOL   enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChDiagPacketGeneratorTransmitEnable(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagPacketGeneratorBurstTransmitStatusGet
*
* DESCRIPTION:
*       Get burst transmission status on specified port.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number
*
* OUTPUTS:
*       burstTransmitDonePtr - (pointer to) burst transmit done status
*                               GT_TRUE: burst transmission done
*                               GT_FALSE: burst transmission not done
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on port not connected to packet generator
*       GT_NOT_INITIALIZED       - on port is not initialized (interface/speed)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Burst transmission status is clear on read.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagPacketGeneratorBurstTransmitStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_PHYSICAL_PORT_NUM     portNum;
    GT_BOOL   burstTransmitDone;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChDiagPacketGeneratorBurstTransmitStatusGet(devNum, portNum, &burstTransmitDone);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", burstTransmitDone);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagBistTriggerAllSet
*
* DESCRIPTION:
*       Trigger the starting of BIST on device physical RAMs.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Lion3.
*
* INPUTS:
*       devNum        - PP device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Triggering the BIST will cause memory content corruption!!!
*
******************************************************************************/
static CMD_STATUS wrCpssDxChDiagBistTriggerAllSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChDiagBistTriggerAllSet(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

#define CPSS_DXCH_DIAG_BIST_RAMS_NUM_CNS 1667
CPSS_DXCH_DIAG_BIST_RESULT_STC              bistResultsArr[CPSS_DXCH_DIAG_BIST_RAMS_NUM_CNS];
GT_U32                                      bistResultsNum = 0;
GT_U32                                      currentBistResult;

/*******************************************************************************
* cpssDxChDiagBistResultsGet
*
* DESCRIPTION:
*       Retrieve the BIST results.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion; Lion2; Lion3.
*
* INPUTS:
*       devNum              - PP device number
*       resultsNumPtr       - (pointer to) max num of results that can be
*                             reported due to size limit of resultsArr[].
*
*
* OUTPUTS:
*       resultsStatusPtr    - (pointer to) the status of the BIST.
*       resultsArr[]        - (pointer to) the BIST failures.
*                             Relevant only if BIST failed.
*       resultsNumPtr       - (pointer to) the number of failures detected.
*                             Relevant only if BIST failed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
******************************************************************************/
static CMD_STATUS wrCpssDxChDiagBistStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    GT_U8                                       devNum;
    CPSS_DXCH_DIAG_BIST_STATUS_ENT              bistStatus;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    bistResultsNum = CPSS_DXCH_DIAG_BIST_RAMS_NUM_CNS;

    /* call cpss api function */
    result = cpssDxChDiagBistResultsGet(devNum, &bistStatus, bistResultsArr, &bistResultsNum);

    if( CPSS_DXCH_DIAG_BIST_STATUS_FAIL_E != bistStatus )
    {
        bistResultsNum = 0;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", bistStatus, bistResultsNum);

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChDiagBistResultsGetFirst
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

    currentBistResult = 0;

    if( 0 < bistResultsNum )
    {
        inFields[0] = bistResultsArr[currentBistResult].memType;
        inFields[1] = bistResultsArr[currentBistResult].location.dfxPipeId;
        inFields[2] = bistResultsArr[currentBistResult].location.dfxClientId;
        inFields[3] = bistResultsArr[currentBistResult].location.dfxMemoryId;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChDiagBistResultsGetNext
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

    currentBistResult++;

    if( currentBistResult < bistResultsNum )
    {
        inFields[0] = bistResultsArr[currentBistResult].memType;
        inFields[1] = bistResultsArr[currentBistResult].location.dfxPipeId;
        inFields[2] = bistResultsArr[currentBistResult].location.dfxClientId;
        inFields[3] = bistResultsArr[currentBistResult].location.dfxMemoryId;

        /* pack and output table fields */
        fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChDiagExternalMemoriesBistRun
*
* DESCRIPTION:
*       This function runs BIST in given set of DDR units.
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum           - device number
*       extMemoBitmap    - bitmap of External DRAM units.
*                          Bobcat2 devices support 5 External DRAM units.
*       testWholeMemory  - GT_TRUE - test all memory,
*                          GT_FALSE - test area specified by
*                          testedAreaOffset and testedAreaLength only.
*       testedAreaOffset - Tested Area Offset in 64-bit units.
*                          Relevant only when testWholeMemory is GT_FALSE.
*       testedAreaLength - Tested Area Length in 64-bit units.
*                          Relevant only when testWholeMemory is GT_FALSE.
*       pattern          - pattern for writing to the memory.
*
* OUTPUTS:
*       testStatusPtr    - (pointer to)GT_TRUE – no errors, GT_FALSE – errors
*       errorInfoArr     - array of Error Info structures for external DRAMs.
*                          An array should contain 5 structures even not all
*                          5 memories tested. Counters for bypassed memories
*                          will contain zeros.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function should be called when External DRAM was already initialized.
*       The cpssDxChTmGlueDramInit is used for External DRAM initialization.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDiagExternalMemoriesBistRun
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                    result;
    GT_U8                                        devNum;
    GT_U32                                       extMemoBitmap;
    GT_BOOL                                      testWholeMemory;
    GT_U32                                       testedAreaOffset;
    GT_U32                                       testedAreaLength;
    CPSS_DXCH_DIAG_EXT_MEMORY_BIST_PATTERN_ENT   pattern;
    GT_BOOL                                      testStatus;
    CPSS_DXCH_DIAG_EXT_MEMORY_INF_BIST_ERROR_STC errorInfoArr[5];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum            = (GT_U8)inArgs[0];
    extMemoBitmap     = (GT_U32)inArgs[1];
    testWholeMemory   = (GT_BOOL)inArgs[2];
    testedAreaOffset  = (GT_U32)inArgs[3];
    testedAreaLength  = (GT_U32)inArgs[4];
    pattern           = (CPSS_DXCH_DIAG_EXT_MEMORY_BIST_PATTERN_ENT)inArgs[5];

    cpssOsMemSet(errorInfoArr, 0, sizeof(errorInfoArr));

    /* call cpss api function */
    result = cpssDxChDiagExternalMemoriesBistRun(
    	devNum, extMemoBitmap, testWholeMemory,
        testedAreaOffset, testedAreaLength,
        pattern, &testStatus, errorInfoArr);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d%d%d%d%d%d%d%d", testStatus,
        errorInfoArr[0].errCounter, errorInfoArr[0].lastFailedAddr,
        errorInfoArr[1].errCounter, errorInfoArr[1].lastFailedAddr,
        errorInfoArr[2].errCounter, errorInfoArr[2].lastFailedAddr,
        errorInfoArr[3].errCounter, errorInfoArr[3].lastFailedAddr,
        errorInfoArr[4].errCounter, errorInfoArr[4].lastFailedAddr);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChDiagMemTest",
        &wrCpssDxChDiagMemTest,
        5, 0},

    {"cpssDxChDiagAllMemTest",
        &wrCpssDxChDiagAllMemTest,
        1, 0},

    {"cpssDxChDiagMemWrite",
        &wrCpssDxChDiagMemWrite,
        4, 0},

    {"cpssDxChDiagMemRead",
        &wrCpssDxChDiagMemRead,
        3, 0},

    {"cpssDxChDiagRegWrite",
        &wrCpssDxChDiagRegWrite,
        6, 0},

    {"cpssDxChDiagRegRead",
        &wrCpssDxChDiagRegRead,
        5, 0},

    {"cpssDxChDiagPhyRegWrite",
        &wrCpssDxChDiagPhyRegWrite,
        7, 0},

    {"cpssDxChDiagPhyRegRead",
        &wrCpssDxChDiagPhyRegRead,
        6, 0},

    {"cpssDxChDiagRegsNumGet",
        &wrCpssDxChDiagRegsNumGet,
        1, 0},

    {"cpssDxChDiagRegsDump",
        &wrCpssDxChDiagRegsDump,
        3, 0},

    {"cpssDxChDiagRegTest",
        &wrCpssDxChDiagRegTest,
        4, 0},

    {"cpssDxChDiagAllRegTest",
        &wrCpssDxChDiagAllRegTest,
        1, 0},
    {"cpssDxChDiagPrbsPortTransmitModeSet",
        &wrCpssDxChDiagPrbsPortTransmitModeSet,
        4, 0},
    {"cpssDxChDiagPrbsPortTransmitModeGet",
        &wrCpssDxChDiagPrbsPortTransmitModeGet,
        3, 0},
    {"cpssDxChDiagPrbsPortGenerateEnableSet",
        &wrCpssDxChDiagPrbsPortGenerateEnableSet,
        4, 0},
    {"cpssDxChDiagPrbsPortGenerateEnableGet",
        &wrCpssDxChDiagPrbsPortGenerateEnableGet,
        3, 0},
    {"cpssDxChDiagPrbsPortCheckEnableSet",
        &wrCpssDxChDiagPrbsPortCheckEnableSet,
        4, 0},
    {"cpssDxChDiagPrbsPortCheckEnableGet",
        &wrCpssDxChDiagPrbsPortCheckEnableGet,
        3, 0},
    {"cpssDxChDiagPrbsPortCheckReadyGet",
        &wrCpssDxChDiagPrbsPortCheckReadyGet,
        2, 0},
    {"cpssDxChDiagPrbsPortStatusGet",
        &wrCpssDxChDiagPrbsPortStatusGet,
        3, 0},
    {"cpssDxChDiagPrbsCyclicDataSet",
        &wrCpssDxChDiagPrbsCyclicDataSet,
        7, 0},
    {"cpssDxChDiagPrbsCyclicDataGet",
        &wrCpssDxChDiagPrbsCyclicDataGet,
        3, 0},
    {"cpssDxChDiagPrbsSerdesTestEnableSet",
        &wrCpssDxChDiagPrbsSerdesTestEnableSet,
        4, 0},
    {"cpssDxChDiagPrbsSerdesTestEnableGet",
        &wrCpssDxChDiagPrbsSerdesTestEnableGet,
        3, 0},
    {"cpssDxChDiagPrbsSerdesTransmitModeSet",
        &wrCpssDxChDiagPrbsSerdesTransmitModeSet,
        4, 0},
    {"cpssDxChDiagPrbsSerdesTransmitModeSet1",
        &wrCpssDxChDiagPrbsSerdesTransmitModeSet,
        4, 0},
    {"cpssDxChDiagPrbsSerdesTransmitModeGet",
        &wrCpssDxChDiagPrbsSerdesTransmitModeGet,
        3, 0},
    {"cpssDxChDiagPrbsSerdesTransmitModeGet1",
        &wrCpssDxChDiagPrbsSerdesTransmitModeGet,
        3, 0},
    {"cpssDxChDiagPrbsSerdesStatusGet",
        &wrCpssDxChDiagPrbsSerdesStatusGet,
        3, 0},
    {"cpssDxChDiagMemoryBistBlockStatusGet",
        &wrCpssDxChDiagMemoryBistBlockStatusGet,
        3, 0},
    {"cpssDxChDiagMemoryBistsRun",
        &wrCpssDxChDiagMemoryBistsRun,
        4, 0},
    {"cpssDxChDiagMemoryBistsRun1",
         &wrCpssDxChDiagMemoryBistsRun1,
         6, 0},
    {"cpssDxChDiagDeviceTemperatureGet",
        &wrCpssDxChDiagDeviceTemperatureGet,
        1, 0},
    {"cpssDxChDiagDeviceTemperatureKelvinGet",
        &wrCpssDxChDiagDeviceTemperatureKelvinGet,
        1, 0},
    {"cpssDxChDiagDeviceTemperatureSensorsSelectSet",
        &wrCpssDxChDiagDeviceTemperatureSensorsSelectSet,
        2, 0},
    {"cpssDxChDiagDeviceTemperatureSensorsSelectGet",
        &wrCpssDxChDiagDeviceTemperatureSensorsSelectGet,
        1, 0},
    {"cpssDxChDiagDeviceTemperatureThresholdSet",
        &wrCpssDxChDiagDeviceTemperatureThresholdSet,
        2, 0},
    {"cpssDxChDiagDeviceTemperatureThresholdGet",
        &wrCpssDxChDiagDeviceTemperatureThresholdGet,
        1, 0},
    {"cpssDxChDiagSerdesTuningRxTuneGetFirst",
        &wrCpssDxChDiagSerdesTuningRxTuneGet,
        7, 0},
    {"cpssDxChDiagSerdesTuningRxTuneGetNext",
        &wrCpssDxChDiagSerdesTuningRxTuneGetNext,
        7, 0},
    {"cpssDxChDiagPacketGeneratorConnectGetFirst",
        &wrCpssDxChDiagPacketGeneratorConnectGetFirst,
        2, 0},
    {"cpssDxChDiagPacketGeneratorConnectGetNext",
        &wrCpssDxChDiagPacketGeneratorConnectGetNext,
        2, 0},

    {"cpssDxChDiagPacketGeneratorConnectSet",
        &wrCpssDxChDiagPacketGeneratorConnectSet,
        2, 18},

    {"cpssDxChDiagPacketGeneratorConnect_1GetFirst",
        &wrCpssDxChDiagPacketGeneratorConnect_1GetFirst,
        2, 0},
    {"cpssDxChDiagPacketGeneratorConnect_1GetNext",
        &wrCpssDxChDiagPacketGeneratorConnect_1GetNext,
        2, 0},

    {"cpssDxChDiagPacketGeneratorConnect_1Set",
        &wrCpssDxChDiagPacketGeneratorConnect_1Set,
        2, 20},

    {"cpssDxChDiagPacketGeneratorTransmitEnable",
        &wrCpssDxChDiagPacketGeneratorTransmitEnable,
        3, 0},
    {"cpssDxChDiagPacketGeneratorBurstTransmitStatusGet",
        &wrCpssDxChDiagPacketGeneratorBurstTransmitStatusGet,
        2, 0},

    {"cpssDxChDiagBistTriggerAllSet",
        &wrCpssDxChDiagBistTriggerAllSet,
        1, 0},

    {"cpssDxChDiagBistStatusGet",
        &wrCpssDxChDiagBistStatusGet,
        1, 0},

    {"cpssDxChDiagBistResultsGetFirst",
        &wrCpssDxChDiagBistResultsGetFirst,
        1,0},

    {"cpssDxChDiagBistResultsGetNext",
        &wrCpssDxChDiagBistResultsGetNext,
        1,0},

    {"cpssDxChDiagExternalMemoriesBistRun",
        &wrCpssDxChDiagExternalMemoriesBistRun,
        5,0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChDiag
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
GT_STATUS cmdLibInitCpssDxChDiag
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

#define PRV_DIAG_DEBUG_MAC 1

#if PRV_DIAG_DEBUG_MAC

/* gebug purposed functions */

GT_STATUS    prvDebugCpssDxChDiagExternalMemoriesBistRun
(
	IN  GT_U8	                                     devNum,
    IN  GT_U32                                       extMemoBitmap,
    IN  GT_BOOL                                      testWholeMemory,
    IN  GT_U32                                       testedAreaOffset,
    IN  GT_U32                                       testedAreaLength,
    IN  CPSS_DXCH_DIAG_EXT_MEMORY_BIST_PATTERN_ENT   pattern
)
{
    GT_STATUS                                    result;
    GT_BOOL                                      testStatus;
    CPSS_DXCH_DIAG_EXT_MEMORY_INF_BIST_ERROR_STC errorInfoArr[5];
    GT_U32                                       i;

    /* call cpss api function */
    result = cpssDxChDiagExternalMemoriesBistRun(
        devNum, extMemoBitmap, testWholeMemory,
        testedAreaOffset, testedAreaLength,
        pattern, &testStatus, errorInfoArr);
    if (result != GT_OK)
    {
        cpssOsPrintf(
            "cpssDxChDiagExternalMemoriesBistRun failed, rc = %d\n",
            result);
        return result;
    }

    /* pack output arguments to galtis string */
    cpssOsPrintf(
        "cpssDxChDiagExternalMemoriesBistRun testStatus = %d\n",
        testStatus);
    if (testStatus == GT_FALSE)
    {
        for (i = 0; (i < 5); i++)
        {
            if (((extMemoBitmap >> i) & 1) == 0)
            {
                continue;
            }
            cpssOsPrintf(
                "#%d errCounter: 0x%8.8X, lastFailedAddr: 0x%8.8X\n",
                i, errorInfoArr[i].errCounter, errorInfoArr[i].lastFailedAddr);
        }
    }

    return CMD_OK;
}

#include <mvDdr3TrainingIp.h>
#include <mvDdr3TrainingIpDb.h>
#include <mvDdr3TrainingIpDef.h>
#include <mvDdr3TrainingIpBist.h>
#include <mvDdr3TrainingIpFlow.h>
#include <mvDdr3TrainingIpPrvIf.h>
#include <mvHwsDdr3Bc2.h>

GT_STATUS ddr3BapSetAlgoFlow
(
	GT_U8       devNum,
	GT_U32		algoFlowMask
);

/*******************************************************************************
* prvCpssDxChDiagExternalMemoriesBistInit
*
* DESCRIPTION:
*       This function initializes DDR units for BIST.
*
* APPLICABLE DEVICES:
*        Bobcat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum           - device number
*       boardId          - boardId: 0 and 1 supported
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_CPU_MEM        - no CPU memory allocation fail
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This function should be called when External DRAM was not initialized.
*       by cpssDxChTmGlueDramInit.
*
*******************************************************************************/
extern MV_HWS_TOPOLOGY_MAP bc2TopologyMap[];
GT_STATUS prvCpssDxChDiagExternalMemoriesBistInit
(
	IN  GT_U8 	                                     devNum,
	IN  GT_U32	                                     boardId
)
{
    GT_STATUS rc;

    PRV_CPSS_DXCH_DEV_CHECK_MAC(devNum);
    PRV_CPSS_NOT_APPLICABLE_DEV_CHECK_MAC(
        devNum, CPSS_CH1_E | CPSS_CH1_DIAMOND_E
        | CPSS_CH2_E | CPSS_CH3_E | CPSS_XCAT_E | CPSS_XCAT3_E |
        CPSS_XCAT2_E | CPSS_LION_E | CPSS_LION2_E );

    if (boardId >= 2)
    {
        return GT_BAD_PARAM;
    }

    if (PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.mvHwsDevTopologyMapPtr == NULL)
    {
        PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.mvHwsDevTopologyMapPtr =
            (MV_HWS_TOPOLOGY_MAP*)cpssOsMalloc(sizeof(MV_HWS_TOPOLOGY_MAP));
        if (PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.mvHwsDevTopologyMapPtr == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }
    }

    cpssOsMemCpy(
        PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.mvHwsDevTopologyMapPtr,
        &(bc2TopologyMap[boardId]),
        sizeof(MV_HWS_TOPOLOGY_MAP));

    rc = mvHwsDdr3TipLoadTopologyMap(
        devNum, PRV_CPSS_DXCH_PP_MAC(devNum)->extMemory.mvHwsDevTopologyMapPtr);
    if( GT_OK != rc )
    {
        return rc;
    }

    rc = mvHwsDdr3TipSelectDdrController(devNum, GT_TRUE);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = ddr3BapSetAlgoFlow(devNum, 0x668F1);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = mvHwsDdr3TipRunAlg(devNum, ALGO_TYPE_DYNAMIC);
    if (rc != GT_OK)
    {
        return rc;
    }

    return GT_OK;
}

GT_STATUS    prvDebugDdrIfRegWrite
(
    GT_U32                devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId,
    GT_U32                regAddr,
    GT_U32                dataValue
)
{
    return mvHwsDdr3TipIFWrite(
        devNum, interfaceAccess, interfaceId,
        regAddr, dataValue, 0xFFFFFFFF /*mask*/);
}

GT_STATUS    prvDebugDdrIfRegRead
(
    GT_U32                devNum,
    GT_U32                interfaceId,
    GT_U32                regAddr
)
{
    GT_STATUS rc;
    GT_U32 readData[MAX_INTERFACE_NUM];

    rc = mvHwsDdr3TipIFRead(
        devNum, ACCESS_TYPE_UNICAST, interfaceId,
        regAddr, readData, 0xFFFFFFFF /*mask*/);
    if (rc != GT_OK)
    {
        return rc;
    }
    cpssOsPrintf("Read value 0x%8.8X\n", readData[interfaceId]);
    return GT_OK;
}

GT_STATUS prvDebugDdrBistSingle(GT_U8 devNum, MV_HWS_PATTERN  pattern)
{
    GT_STATUS rc;
    GT_U32 res[MAX_INTERFACE_NUM];
    GT_U32 i;

    for(i=0; i<MAX_INTERFACE_NUM; i++)
    {
        res[i] = 0;
    }

    rc = mvHwsDdr3RunBist(0, pattern, &(res[0]), 0);
    if (rc != GT_OK)
    {
        cpssOsPrintf("mvHwsDdr3RunBist rc: 0x%X\n", rc);
        return rc;
    }

    for(i=0; i<MAX_INTERFACE_NUM; i++)
    {
        cpssOsPrintf("result interface %d 0x%X\n", i, res[i]);
    }

    return GT_OK;
}

#endif /*PRV_DIAG_DEBUG_MAC*/


