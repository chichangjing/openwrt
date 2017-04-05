/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssGenSmi.c
*
* DESCRIPTION:
*       Wrapper functions for API for read/write register of device, which 
*           connected to SMI master controller of packet processor
*
* FILE REVISION NUMBER:
*       $Revision: 7 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/phy/cpssGenPhySmi.h>
#include <cpss/generic/smi/cpssGenSmi.h>

/*******************************************************************************
* cpssSmiRegisterRead
*
* DESCRIPTION:
*      The function reads register of a device, which connected to SMI master 
*           controller of packet processor
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - register address
*
* OUTPUTS:
*       data - pointer to place data from read operation
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssSmiRegisterRead
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    CPSS_PHY_SMI_INTERFACE_ENT   smiInterface;
    GT_U32  smiAddr, regAddr, data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    smiInterface = (CPSS_PHY_SMI_INTERFACE_ENT)inArgs[2];
    smiAddr = (GT_U32)inArgs[3];
    regAddr = (GT_U32)inArgs[4];

    result = cpssSmiRegisterRead(devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, &data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "0x%x", data);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssSmiRegisterWrite
*
* DESCRIPTION:
*      The function reads register of a device, which connected to SMI master 
*           controller of packet processor
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - register address
*       data - pointer to place data from read operation
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssSmiRegisterWrite
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    CPSS_PHY_SMI_INTERFACE_ENT   smiInterface;
    GT_U32  smiAddr, regAddr, data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    smiInterface = (CPSS_PHY_SMI_INTERFACE_ENT)inArgs[2];
    smiAddr = (GT_U32)inArgs[3];
    regAddr = (GT_U32)inArgs[4];
    data = (GT_U32)inArgs[5];

    result = cpssSmiRegisterWrite(devNum, portGroupsBmp, smiInterface, smiAddr, regAddr, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssSmiRegisterReadShort
*
* DESCRIPTION:
*      The function reads 16-bit register of a device, which connected to SMI 
*      master controller of packet processor
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - register address
*
* OUTPUTS:
*       data - pointer to place data from read operation
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssSmiRegisterReadShort
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    CPSS_PHY_SMI_INTERFACE_ENT   smiInterface;
    GT_U32  smiAddr, regAddr;
    GT_U16  data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    smiInterface = (CPSS_PHY_SMI_INTERFACE_ENT)inArgs[2];
    smiAddr = (GT_U32)inArgs[3];
    regAddr = (GT_U32)inArgs[4];

    result = cpssSmiRegisterReadShort(devNum, portGroupsBmp, smiInterface, 
                                      smiAddr, regAddr, &data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "0x%x", (GT_U32)data);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssSmiRegisterWriteShort
*
* DESCRIPTION:
*      The function reads 16-bit register of a device, which connected to SMI 
*      master controller of packet processor
*
* APPLICABLE DEVICES:  All devices
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       smiInterface - SMI master interface Id
*       smiAddr     - address of configurated device on SMI (range 0..31)
*       regAddr     - register address
*       data - pointer to place data from read operation
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - bad devNum, smiInterface
*       GT_BAD_PTR      - pointer to place data is NULL
*       GT_NOT_READY    - smi is busy
*       GT_HW_ERROR     - hw error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssSmiRegisterWriteShort
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    CPSS_PHY_SMI_INTERFACE_ENT   smiInterface;
    GT_U32  smiAddr, regAddr;
    GT_U16  data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    smiInterface = (CPSS_PHY_SMI_INTERFACE_ENT)inArgs[2];
    smiAddr = (GT_U32)inArgs[3];
    regAddr = (GT_U32)inArgs[4];
    data = (GT_U16)inArgs[5];

    result = cpssSmiRegisterWriteShort(devNum, portGroupsBmp, smiInterface, 
                                       smiAddr, regAddr, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssXsmiPortGroupRegisterRead
*
* DESCRIPTION:
*       Read value of a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       xsmiAddr    - address of configurated device on XSMI (range 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to read from (value of 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiAddr, phyDev.
*       GT_BAD_PTR               - pointer to place data is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssXsmiPortGroupRegisterRead
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8   devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    GT_U32  xsmiAddr;
    GT_U32  regAddr;
    GT_U32  phyDev;
    GT_STATUS   result;
    GT_U16      data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum          = (GT_U8)inArgs[0];
    portGroupsBmp   = (GT_PORT_GROUPS_BMP)inArgs[1];
    xsmiAddr        = (GT_U32)inArgs[2];
    regAddr         = (GT_U32)inArgs[3];
    phyDev          = (GT_U32)inArgs[4];

    result = cpssXsmiPortGroupRegisterRead(devNum, portGroupsBmp, CPSS_PHY_XSMI_INTERFACE_0_E, 
                                           xsmiAddr, regAddr, phyDev, &data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "0x%x", data);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssXsmiPortGroupRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       xsmiAddr    - address of configurated device on XSMI (range 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to write to (value of 0..31).
*       data        - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiAddr, phyDev.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssXsmiPortGroupRegisterWrite
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8   devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    GT_U32  xsmiAddr;
    GT_U32  regAddr;
    GT_U32  phyDev;
    GT_U16  data;
    GT_STATUS result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum          = (GT_U8)inArgs[0];
    portGroupsBmp   = (GT_PORT_GROUPS_BMP)inArgs[1];
    xsmiAddr        = (GT_U32)inArgs[2];
    regAddr         = (GT_U32)inArgs[3];
    phyDev          = (GT_U32)inArgs[4];
    data            = (GT_U16)inArgs[5];

    result = cpssXsmiPortGroupRegisterWrite(devNum, portGroupsBmp, CPSS_PHY_XSMI_INTERFACE_0_E, 
                                            xsmiAddr, regAddr, phyDev, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssXsmiPortGroupRegisterReadIf
*
* DESCRIPTION:
*       Read value of a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (range 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to read from (value of 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiAddr, phyDev.
*       GT_BAD_PTR               - pointer to place data is NULL
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssXsmiPortGroupRegisterReadIf
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8   devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    GT_U32  xsmiAddr;
    GT_U32  regAddr;
    GT_U32  phyDev;
    GT_STATUS   result;
    GT_U16      data;
    CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum          = (GT_U8)inArgs[0];
    portGroupsBmp   = (GT_PORT_GROUPS_BMP)inArgs[1];
    xsmiInterface   = (CPSS_PHY_XSMI_INTERFACE_ENT)inArgs[2];
    xsmiAddr        = (GT_U32)inArgs[3];
    regAddr         = (GT_U32)inArgs[4];
    phyDev          = (GT_U32)inArgs[5];

    result = cpssXsmiPortGroupRegisterRead(devNum, portGroupsBmp, xsmiInterface, 
                                           xsmiAddr, regAddr, phyDev, &data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "0x%x", data);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssXsmiPortGroupRegisterWriteIf
*
* DESCRIPTION:
*       Write value to a specified XSMI Register.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - physical device number.
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                       1. for non multi-port groups device this parameter is IGNORED.
*                       2. for multi-port groups device :
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       xsmiInterface - XSMI instance
*       xsmiAddr    - address of configurated device on XSMI (range 0..31)
*       regAddr     - address of register of configurated device
*       phyDev      - the PHY device to write to (value of 0..31).
*       data        - data to write
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, xsmiAddr, phyDev.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssXsmiPortGroupRegisterWriteIf
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8   devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    GT_U32  xsmiAddr;
    GT_U32  regAddr;
    GT_U32  phyDev;
    GT_U16  data;
    GT_STATUS result;
    CPSS_PHY_XSMI_INTERFACE_ENT xsmiInterface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum          = (GT_U8)inArgs[0];
    portGroupsBmp   = (GT_PORT_GROUPS_BMP)inArgs[1];
    xsmiInterface   = (CPSS_PHY_XSMI_INTERFACE_ENT)inArgs[2];
    xsmiAddr        = (GT_U32)inArgs[3];
    regAddr         = (GT_U32)inArgs[4];
    phyDev          = (GT_U32)inArgs[5];
    data            = (GT_U16)inArgs[6];

    result = cpssXsmiPortGroupRegisterWrite(devNum, portGroupsBmp, xsmiInterface, 
                                            xsmiAddr, regAddr, phyDev, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssSmiRegisterRead",
        &wrCpssSmiRegisterRead,
        5, 0},

    {"cpssSmiRegisterWrite",
        &wrCpssSmiRegisterWrite,
        6, 0},

    {"cpssSmiRegisterReadShort",
        &wrCpssSmiRegisterReadShort,
        5, 0},

    {"cpssSmiRegisterWriteShort",
        &wrCpssSmiRegisterWriteShort,
        6, 0},

    {"cpssXsmiPortGroupRegisterRead",
        &wrCpssXsmiPortGroupRegisterRead,
        5, 0},

    {"cpssXsmiPortGroupRegisterWrite",
        &wrCpssXsmiPortGroupRegisterWrite,
        6, 0},

    {"cpssXsmiPortGroupRegisterReadIf",
        &wrCpssXsmiPortGroupRegisterReadIf,
        6, 0},

    {"cpssXsmiPortGroupRegisterWriteIf",
        &wrCpssXsmiPortGroupRegisterWriteIf,
        7, 0}
};


#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))



/*******************************************************************************
* cmdLibInitCpssGenSmi
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
GT_STATUS cmdLibInitCpssGenSmi
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

