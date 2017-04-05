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
* wrapPhyCpssDxCh.c
*
* DESCRIPTION:
*       Wrapper functions for Phy cpss.dxCh functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 6.*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/phy/cpssDxChPhySmi.h>


/*******************************************************************************
* cpssDxChPhyPortSmiInit
*
* DESCRIPTION:
*       Initialiaze the SMI control register port, Check all GE ports and
*       activate the errata initialization fix.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyPortSmiInit

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPhyPortSmiInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPhyPortAddrGet
*
* DESCRIPTION:
*       Gets port's phy address from hardware and from database.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number.
*
* OUTPUTS:
*       phyAddFromHwPtr - (pointer to) phy address in HW.
*       phyAddFromDbPtr - (pointer to) phy address in DB.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For flex ports no hardware value exists and this field should be
*       ignored.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyPortAddrGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U8    phyAddFromHw;
    GT_U8    phyAddFromDb;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhyPortAddrGet(devNum, portNum, &phyAddFromHw, &phyAddFromDb);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", phyAddFromHw, phyAddFromDb);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPhyPortAddrSet
*
* DESCRIPTION:
*       Configure the port's default phy address, this function should be
*       used to change the default port's phy address.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number.
*       phyAddr     - The new phy address, (value 0...31).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For ports other then flex ports, this function should be called
*       after cpssDxChHwPpPhase1Init(), and before cpssDxChPhyPortSmiInit().
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyPortAddrSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U8    phyAddr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    phyAddr = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhyPortAddrSet(devNum, portNum, phyAddr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPhyPortSmiRegisterRead
*
* DESCRIPTION:
*       Read specified SMI Register on a specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       phyReg    - SMI register (value 0..31)
*
* OUTPUTS:
*       dataPtr   - (pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - phyPoryAddr or smiCtrlAddr not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyPortSmiRegisterRead

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U8    phyReg;
    GT_U16   dataPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    phyReg = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhyPortSmiRegisterRead(devNum, portNum, phyReg, &dataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPhyPortSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to specified SMI Register on a specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number.
*       phyReg      - The new phy address, (value 0...31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_SUPPORTED         - for XG ports
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - phyPoryAddr or smiCtrlAddr not initialized
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyPortSmiRegisterWrite

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U8    phyReg;
    GT_U16   data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    phyReg = (GT_U8)inArgs[2];
    data = (GT_U16)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhyPortSmiRegisterWrite(devNum, portNum, phyReg, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPhyPort10GSmiRegisterRead
*
* DESCRIPTION:
*       Read specified 10G SMI Register and PHY device of specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       phyId   - ID of external 10G PHY (value of 0..31).
*       useExternalPhy - boolean variable, defines if to use external 10G PHY
*       phyReg  - 10G SMI register, the register of PHY to read from
*       phyDev  - the PHY device to read from (value of 0..31).
*
* OUTPUTS:
*       dataPtr - (Pointer to) the read data.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_NOT_SUPPORTED         - for non XG ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If useExternalPhy flag is GT_FALSE, the phyId is being ignored
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyPort10GSmiRegisterRead

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U8    phyId;
    GT_BOOL  useExternalPhy;
    GT_U16   phyReg;
    GT_U8    phyDev;
    GT_U16   dataPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    phyId = (GT_U8)inArgs[2];
    useExternalPhy = (GT_BOOL)inArgs[3];
    phyReg = (GT_U16)inArgs[4];
    phyDev = (GT_U8)inArgs[5];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhyPort10GSmiRegisterRead(devNum, portNum, phyId,
                                               useExternalPhy, phyReg,
                                               phyDev, &dataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dataPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPhyPort10GSmiRegisterWrite
*
* DESCRIPTION:
*       Write value to a specified 10G SMI Register and PHY device of
*       specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - physical port number.
*       phyId       - ID of external 10G PHY (value of 0...31).
*       useExternalPhy - Boolean variable, defines if to use external 10G PHY
*       phyReg      - 10G SMI register, the register of PHY to read from
*       phyDev      - the PHY device to read from (value of 0..31).
*       data        - Data to write.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_SUPPORTED         - for non XG ports
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If useExternalPhy flag is GT_FALSE, the phyId is being ignored
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyPort10GSmiRegisterWrite

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U8    phyId;
    GT_BOOL  useExternalPhy;
    GT_U16   phyReg;
    GT_U8    phyDev;
    GT_U16   data;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    phyId = (GT_U8)inArgs[2];
    useExternalPhy = (GT_BOOL)inArgs[3];
    phyReg = (GT_U16)inArgs[4];
    phyDev = (GT_U8)inArgs[5];
    data = (GT_U16)inArgs[6];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhyPort10GSmiRegisterWrite(devNum, portNum, phyId,
                                                useExternalPhy, phyReg,
                                                phyDev, data);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPhyAutonegSmiGet
*
* DESCRIPTION:
*       This function gets the auto negotiation status between the PP and PHY.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - physical device number.
*       portNum      - port number: applicable ports from 0 till 23
*
* OUTPUTS:
*       enabledPtr   - (Pointer to) the auto negotiation process state between
*                      PP and Phy:
*                      0 = Auto-Negotiation process is preformed.
*                      1 = Auto-Negotiation process is not preforme
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Although the device ignores the information read from the PHY
*       registers, it keeps polling those registers.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyAutonegSmiGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_BOOL  enabledPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhyAutonegSmiGet(devNum, portNum, &enabledPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enabledPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPhyAutonegSmiSet
*
* DESCRIPTION:
*       This function sets the auto negotiation process, between
*       the PP and PHY, to enable/disable.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - physical device number.
*       portNum     - port number: applicable ports from 0 till 23
*       enable      - 0 = Auto-Negotiation process is preformed.
*                     1 = Auto-Negotiation process is not preforme
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Although the device ignores the information read from the PHY
*       registers, it keeps polling those registers.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyAutonegSmiSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_BOOL  enabled;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enabled = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhyAutonegSmiSet(devNum, portNum, enabled);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPhySmiAutoMediaSelectSet
*
* DESCRIPTION:
*       This function gets a bit per port bitmap (of 6 ports)
*       indicating whether this port is connected to a dual-media PHY,
*       such as Marvell 88E1112, that is able to perform Auto-media select
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number: applicable ports from 0 till 23
*       autoMediaSelect - Sets 6 bits.
*                         0 = Port is not connected to dual-media PHY
*                         1 = Port is connected to dual-media PHY
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_OUT_OF_RANGE          - phyAddr bigger then 31
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhySmiAutoMediaSelectSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U32   autoMediaSelect;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    autoMediaSelect = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhySmiAutoMediaSelectSet(devNum, portNum,
                                               autoMediaSelect);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPhySmiAutoMediaSelectGet
*
* DESCRIPTION:
*       This function sets a bit per port bitmap (of 6 ports)
*       indicating whether this port is connected to a dual-media PHY,
*       such as Marvell 88E1112, that is able to perform Auto-media select
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number:applicable ports from 0 till 23
*
* OUTPUTS:
*       autoMediaSelectPtr - (Pointer to) port state bitMap of 6 bits:
*                            0 = Port is not connected to dual-media PHY
*                            1 = Port is connected to dual-media PHY
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhySmiAutoMediaSelectGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U32   autoMediaSelectPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhySmiAutoMediaSelectGet(devNum, portNum,
                                           &autoMediaSelectPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", autoMediaSelectPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPhyPortSmiInterfaceSet
*
* DESCRIPTION:
*       Configure flex port SMI interface.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*       smiInterface    - port SMI interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum or SMI interface.
*       GT_NOT_SUPPORTED         - for non flex ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyPortSmiInterfaceSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    CPSS_PHY_SMI_INTERFACE_ENT  smiInterface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    smiInterface = (CPSS_PHY_SMI_INTERFACE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhyPortSmiInterfaceSet(devNum, portNum, smiInterface);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPhyPortSmiInterfaceGet
*
* DESCRIPTION:
*       Get flex port SMI interface.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*
* OUTPUTS:
*       smiInterfacePtr - (pointer to) port SMI interface.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_SUPPORTED         - for non flex ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - port SMI interface was not initialized
*                                  correctly.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyPortSmiInterfaceGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    CPSS_PHY_SMI_INTERFACE_ENT  smiInterface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhyPortSmiInterfaceGet(devNum, portNum, &smiInterface);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", smiInterface);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPhyAutoPollNumOfPortsSet
*
* DESCRIPTION:
*       Configure number of auto poll ports for SMI0 and SMI1.
*       This function should be called after cpssDxChHwPpPhase1Init()
*       and before cpssDxChPhyPortSmiInit().
*
* APPLICABLE DEVICES:  DxChXCat and above
*
* INPUTS:
*       devNum                  - physical device number.
*       autoPollNumOfPortsSmi0  - number of ports for SMI0.
*       autoPollNumOfPortsSmi1  - number of ports for SMI1.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, autoPollNumOfPortsSmi0 or
*                                                   autoPollNumOfPortsSmi1.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The acceptable combinations for Auto Poll number of ports are:
*       |-----------------------|
*       |   SMI 0   |   SMI 1   |
*       |-----------|-----------|
*       |     8     |    16     |
*       |     8     |    12     |
*       |    12     |    12     |
*       |    16     |    8      |
*       |-----------------------|
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyAutoPollNumOfPortsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           devNum;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi0;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi1;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    autoPollNumOfPortsSmi0 = (CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT)inArgs[1];
    autoPollNumOfPortsSmi1 = (CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPhyAutoPollNumOfPortsSet(devNum,
                                              autoPollNumOfPortsSmi0,
                                              autoPollNumOfPortsSmi1,
                                              CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E,
                                              CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_8_E);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;

}

/*******************************************************************************
* cpssDxChPhyAutoPollNumOfPortsSet_1
*
* DESCRIPTION:
*       Configure number of auto poll ports for SMI0 and SMI1.
*       This function should be called after cpssDxChHwPpPhase1Init()
*       and before cpssDxChPhyPortSmiInit().
*
* APPLICABLE DEVICES:  Bobcat2, Caelum, Bobcat3.
*
* INPUTS:
*       devNum                  - physical device number.
*       autoPollNumOfPortsSmi0  - number of ports for SMI0.
*       autoPollNumOfPortsSmi1  - number of ports for SMI1.
*       autoPollNumOfPortsSmi2  - number of ports for SMI2.
*       autoPollNumOfPortsSmi3  - number of ports for SMI3.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, autoPollNumOfPortsSmi0 or
*                                                   autoPollNumOfPortsSmi1.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The acceptable combinations for Auto Poll number of ports are:
*       |----------------------------------------------|
*       |   SMI 0   |   SMI 1   |   SMI 2   |   SMI 3  |
*       |-----------|-----------|-----------|----------|
*       |     8     |    16     |     8     |    16    |
*       |     8     |    12     |     8     |    12    |
*       |    12     |    12     |    12     |    12    |
*       |    16     |    8      |    16     |    8     |
*       |----------------------------------------------|
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyAutoPollNumOfPortsSet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           devNum;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi0;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi1;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi2;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi3;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    autoPollNumOfPortsSmi0 = (CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT)inArgs[1];
    autoPollNumOfPortsSmi1 = (CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT)inArgs[2];
    autoPollNumOfPortsSmi2 = (CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT)inArgs[3];
    autoPollNumOfPortsSmi3 = (CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPhyAutoPollNumOfPortsSet(devNum,
                                              autoPollNumOfPortsSmi0,
                                              autoPollNumOfPortsSmi1,
                                              autoPollNumOfPortsSmi2,
                                              autoPollNumOfPortsSmi3);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;

}


/*******************************************************************************
* cpssDxChPhyAutoPollNumOfPortsGet
*
* DESCRIPTION:
*       Get number of auto poll ports for SMI0 and SMI1.
*
* APPLICABLE DEVICES:  DxChXCat and above
*
* INPUTS:
*       devNum                  - physical device number.
*
* OUTPUTS:
*       autoPollNumOfPortsSmi0Ptr  - number of ports for SMI0.
*       autoPollNumOfPortsSmi1Ptr  - number of ports for SMI1.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, autoPollNumOfPortsSmi0 or
*                                                   autoPollNumOfPortsSmi1.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyAutoPollNumOfPortsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           devNum;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi0;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi1;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi2;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi3;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPhyAutoPollNumOfPortsGet(devNum,
                                              &autoPollNumOfPortsSmi0,
                                              &autoPollNumOfPortsSmi1,
                                              &autoPollNumOfPortsSmi2,
                                              &autoPollNumOfPortsSmi3);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", autoPollNumOfPortsSmi0,
                 autoPollNumOfPortsSmi1);
    return CMD_OK;

}

/*******************************************************************************
* cpssDxChPhyAutoPollNumOfPortsGet_1
*
* DESCRIPTION:
*       Get number of auto poll ports for SMI0 and SMI1.
*
* APPLICABLE DEVICES:  Bobcat2, Caelum, Bobcat3.
*
* INPUTS:
*       devNum                  - physical device number.
*
* OUTPUTS:
*       autoPollNumOfPortsSmi0Ptr  - number of ports for SMI0.
*       autoPollNumOfPortsSmi1Ptr  - number of ports for SMI1.
*       autoPollNumOfPortsSmi2Ptr  - number of ports for SMI2.
*       autoPollNumOfPortsSmi3Ptr  - number of ports for SMI3.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, autoPollNumOfPortsSmi0,
*                                                   autoPollNumOfPortsSmi1,
*                                                   autoPollNumOfPortsSmi2,
*                                                   autoPollNumOfPortsSmi3
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyAutoPollNumOfPortsGet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    GT_U8                                           devNum;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi0;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi1;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi2;
    CPSS_DXCH_PHY_SMI_AUTO_POLL_NUM_OF_PORTS_ENT    autoPollNumOfPortsSmi3;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPhyAutoPollNumOfPortsGet(devNum,
                                              &autoPollNumOfPortsSmi0,
                                              &autoPollNumOfPortsSmi1,
                                              &autoPollNumOfPortsSmi2,
                                              &autoPollNumOfPortsSmi3);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d",
                 autoPollNumOfPortsSmi0,autoPollNumOfPortsSmi1,
                 autoPollNumOfPortsSmi2, autoPollNumOfPortsSmi3);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPhyPortXSmiInterfaceSet
*
* DESCRIPTION:
*       Configure flex port XSMI interface.
*
* APPLICABLE DEVICES:  Bobcat2, Caelum, Bobcat3 Devices
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*       xsmiInterface   - port XSMI interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum or SMI interface.
*       GT_NOT_SUPPORTED         - for non flex ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyPortXSmiInterfaceSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    CPSS_PHY_XSMI_INTERFACE_ENT  xsmiInterface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    xsmiInterface = (CPSS_PHY_XSMI_INTERFACE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhyPortXSmiInterfaceSet(devNum, portNum, xsmiInterface);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPhyPortXSmiInterfaceGet
*
* DESCRIPTION:
*       Get port XSMI interface.
*
* APPLICABLE DEVICES:  Bobcat2, Caelum, Bobcat3 Devices
*
* INPUTS:
*       devNum          - physical device number.
*       portNum         - port number.
*
* OUTPUTS:
*       xsmiInterfacePtr - (pointer to) port XSMI interface.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, portNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_SUPPORTED         - for non flex ports
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_INITIALIZED       - port SMI interface was not initialized
*                                  correctly.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPhyPortXSmiInterfaceGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    CPSS_PHY_XSMI_INTERFACE_ENT  xsmiInterface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPhyPortXSmiInterfaceGet(devNum, portNum, &xsmiInterface);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", xsmiInterface);
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChPhyPortSmiInit",
        &wrCpssDxChPhyPortSmiInit,
        1, 0},

    {"cpssDxChPhyPortAddrGet",
        &wrCpssDxChPhyPortAddrGet,
        2, 0},

    {"cpssDxChPhyPortAddrSet",
        &wrCpssDxChPhyPortAddrSet,
        3, 0},

    {"cpssDxChPhyPortSmiRegisterRead",
        &wrCpssDxChPhyPortSmiRegisterRead,
        3, 0},

    {"cpssDxChPhyPortSmiRegisterWrite",
        &wrCpssDxChPhyPortSmiRegisterWrite,
        4, 0},

    {"cpssDxChPhyPort10GSmiRegisterRead",
        &wrCpssDxChPhyPort10GSmiRegisterRead,
        6, 0},

    {"cpssDxChPhyPort10GSmiRegisterWrite",
        &wrCpssDxChPhyPort10GSmiRegisterWrite,
        7, 0},

    {"cpssDxChPhyAutonegSmiGet",
        &wrCpssDxChPhyAutonegSmiGet,
        2, 0},

    {"cpssDxChPhyAutonegSmiSet",
        &wrCpssDxChPhyAutonegSmiSet,
        3, 0},

    {"cpssDxChPhySmiAutoMediaSelectSet",
        &wrCpssDxChPhySmiAutoMediaSelectSet,
        3, 0},

    {"cpssDxChPhySmiAutoMediaSelectGet",
        &wrCpssDxChPhySmiAutoMediaSelectGet,
        2, 0},

    {"cpssDxChPhyPortSmiInterfaceGet",
        &wrCpssDxChPhyPortSmiInterfaceGet,
        2, 0},

    {"cpssDxChPhyPortSmiInterfaceSet",
        &wrCpssDxChPhyPortSmiInterfaceSet,
        3, 0},

    {"cpssDxChPhyAutoPollNumOfPortsSet",
        &wrCpssDxChPhyAutoPollNumOfPortsSet,
        3, 0},

    {"cpssDxChPhyAutoPollNumOfPortsGet",
        &wrCpssDxChPhyAutoPollNumOfPortsGet,
        1, 0},

    {"cpssDxChPhyAutoPollNumOfPortsSet_1",
        &wrCpssDxChPhyAutoPollNumOfPortsSet_1,
        5, 0},

    {"cpssDxChPhyAutoPollNumOfPortsGet_1",
        &wrCpssDxChPhyAutoPollNumOfPortsGet_1,
        1, 0},

    {"cpssDxChPhyPortXSmiInterfaceGet",
        &wrCpssDxChPhyPortXSmiInterfaceGet,
        2, 0},

    {"cpssDxChPhyPortXSmiInterfaceSet",
        &wrCpssDxChPhyPortXSmiInterfaceSet,
        3, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChPhy
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
GT_STATUS cmdLibInitCpssDxChPhy
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


