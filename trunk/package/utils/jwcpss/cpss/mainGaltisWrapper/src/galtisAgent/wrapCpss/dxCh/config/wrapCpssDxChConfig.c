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
* wrapConfigCpssDxSal.c
*
* DESCRIPTION:
*       Wrapper functions for Config cpss.dxSal functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 23 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/config/cpssDxChCfgInit.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>



/*******************************************************************************
* cpssDxChCfgDevRemove
*
* DESCRIPTION:
*
*       Remove the device from the CPSS.
*       This function we release the DB of CPSS that associated with the PP ,
*       and will "know" that the device is not longer valid.
*       This function will not do any HW actions , and therefore can be called
*       before or after the HW was actually removed.
*
*       After this function is called the devNum is no longer valid for any API
*       until this devNum is used in the initialization sequence for a "new" PP.
*
*       NOTE: 1. Application may want to disable the Traffic to the CPU , and
*             messages , prior to this operation (if the device still exists).
*             2. Application may want to a reset the HW PP , and there for need
*             to call the "hw reset" function (before calling this function)
*             and after calling this function to do the initialization
*             sequence all over again for the device.
*
*
* APPLICABLE DEVICES:  All 98Dx250/260/270/246/243/166/163
*                          98Dx253/263/273/247/167/106/107 devices
* INPUTS:
*       devNum - device number to remove.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_BAD_PARAM - wrong dev
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgDevRemove

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
    result = cpssDxChCfgDevRemove(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCfgDevEnable
*
* DESCRIPTION:
*       This routine sets the device state.
*
* APPLICABLE DEVICES:  All 98Dx250/260/270/246/243/166/163
*                          98Dx253/263/273/247/167/106/107 devices
* INPUTS:
*       devNum - device number to set.
*       enable - GT_TRUE device enable, GT_FALSE disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL - on error
*
* COMMENTS:
*
* GalTis:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgDevEnable

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCfgDevEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgDevEnableGet
*
* DESCRIPTION:
*       This routine gets the device state.
*
* APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*       None.
*
* INPUTS:
*       devNum - device number to set.
*
* OUTPUTS:
*       enablePtr - GT_TRUE device enable, GT_FALSE disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgDevEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable = GT_FALSE;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCfgDevEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet
*
* DESCRIPTION:
*       Sets device ID modification for Routed packets.
*       Enables/Disables FORWARD DSA tag modification of the <source device>
*       and <source port> fields of packets routed by the local device.
*       The <source device> is set to the local device ID and the <source port>
*       is set to 61 (the virtual router port).
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       devNum          - Physical device number.
*       portNum         - Physical port number.
*       modifyEnable    - Boolean value of the FORWARD DSA tag modification:
*                             GT_TRUE  -  Device ID Modification is Enabled.
*                             GT_FALSE -  Device ID Modification is Disabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum or portNum.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_PORT_NUM    portNum;
    GT_BOOL  modifyEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    modifyEnable = (GT_BOOL)inArgs[2];

    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet(devNum, portNum, modifyEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet
*
* DESCRIPTION:
*       Gets device ID modification for Routed packets.
*       See Set Api comments.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum           - Device number.
*       portNum          - Port number (or CPU port)
*
* OUTPUTS:
*       modifyEnablePtr  - Boolean value of the FORWARD DSA tag modification:
*                             GT_TRUE  -  Device ID Modification is Enabled.
*                             GT_FALSE -  Device ID Modification is Disabled.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL ptr
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8    devNum;
    GT_PORT_NUM    portNum;
    GT_BOOL  modifyEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet(devNum, portNum, &modifyEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "&d", modifyEnable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortModifySet
*
* DESCRIPTION:
*       Enables/Disables Device ID Modification upon packet sending to another
*       stack unit.
*
*       When Connecting DxCh Devices to SOHO in a Value Blade model to enable
*       DxCh grade Capabilities for FE Ports, in a staking system, we must be
*       able to overcome the 32 devices limitation.
*       To do that, SOHO Device Numbers are not unique and packets received
*       from the SOHO by the DxCh and are relayed to other stack units
*       have their Device ID changed to the DxCh Device ID.
*       On Upstream (packet from SOHO to DxCh):
*           The SOHO Sends a packet to the DxCh and the packet is sent back
*           to the SOHO. In this case we don't want to change the Device ID in
*           the DSA Tag so that the SOHO is able to filter the Src Port and is
*           able to send back the packet to its source when doing cross chip
*           flow control.
*       On Downsteam (packet from SOHO to SOHO):
*           The SOHO receives a packet from another SOHO in this case we
*           want to change the Device ID so that the packet is not filtered.
*
*       On top of the above, for forwarding packets between the DxCh devices
*       and for Auto Learning, the Port Number must also be changed.
*       In addition Changing the Device ID is needed only for FORWARD DSA Tag.
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       devNum              - Physical device number.
*       modifedDsaSrcDev    - Boolean value of Enables/Disables Device ID
*                             Modification:
*                             GT_TRUE  -  DSA<SrcDev> is modified to the
*                                         DxCh Device ID and DSA<SrcPort>
*                                         is modified to the DxCh Ingress
*                                         port if all of the following are
*                                         conditions are met:
*                                           - packet is received with a
*                                           non-extended DSA Tag FORWARD    and
*                                           - DSA<SrcIsTrunk> = 0           and
*                                           - packet is transmitted with an
*                                           extended DSA Tag FORWARD.
*
*                             GT_FALSE -  DSA<SrcDev> is not modified when the
*                                         packet is sent to another stac unit
*                                         via an uplink.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on hardware error.
*       GT_BAD_PARAM        - on wrong devNum or portNum.
*
* COMMENTS:
*       This field is relevant for GigE Ports Only.
*       This change is in Addition to the SrcDev Change for Routed packets.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgDsaTagSrcDevPortModifySet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  modifedDsaSrcDev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    modifedDsaSrcDev = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCfgDsaTagSrcDevPortModifySet(devNum, modifedDsaSrcDev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgDsaTagSrcDevPortModifyGet
*
* DESCRIPTION:
*       Get status of Device ID Modification upon packet sending to another
*       stack unit. See Set Api comments.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Lion3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum               - Device number.
*
* OUTPUTS:
*       modifedDsaSrcDevPtr  - Boolean value of Enables/Disables Device ID
*                             Modification:
*                             GT_TRUE  -  DSA<SrcDev> is modified to the
*                                         DxCh Device ID and DSA<SrcPort>
*                                         is modified to the DxCh Ingress
*                                         port if all of the following are
*                                         conditions are met:
*                                           - packet is received with a
*                                           non-extended DSA Tag FORWARD    and
*                                           - DSA<SrcIsTrunk> = 0           and
*                                           - packet is transmitted with an
*                                           extended DSA Tag FORWARD.
*
*                             GT_FALSE -  DSA<SrcDev> is not modified when the
*                                         packet is sent to another stac unit
*                                         via an uplink.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or portNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This field is relevant for GigE Ports Only.
*       This change is in Addition to the SrcDev Change for Routed packets.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgDsaTagSrcDevPortModifyGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  modifedDsaSrcDev = GT_FALSE;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCfgDsaTagSrcDevPortModifyGet(devNum, &modifedDsaSrcDev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", modifedDsaSrcDev);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgTableNumEntriesGet
*
*
* DESCRIPTION:
*       the function return the number of entries of each individual table in
*       the HW
*
*       when several type of entries like ARP and tunnelStart resize in the same
*       table (memory) , the function returns the number of entries for the least
*       denominator type of entry --> in this case number of ARP entries.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum       - physical device number
*       table        - type of the specific table
*
* OUTPUTS:
*       numEntriesPtr - (pointer to) number of entries
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgTableNumEntriesGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_CFG_TABLES_ENT table;
    GT_U32 numEntries;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    table = (CPSS_DXCH_CFG_TABLES_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCfgTableNumEntriesGet(devNum, table, &numEntries);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", numEntries);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgReNumberDevNum
*
* DESCRIPTION:
*       function allow the caller to modify the DB of the cpss ,
*       so all info that was 'attached' to 'oldDevNum' will be moved to 'newDevNum'.
*
*       NOTE:
*       1. it is the responsibility of application to update the 'devNum' of
*       HW entries from 'oldDevNum' to 'newDevNum' using appropriate API's
*       such as 'Trunk members','Fdb entries','NH entries','PCE entries'...
*       2. it's application responsibility to update the HW device number !
*          see API cpssDxChCfgHwDevNumSet
*       3. no HW operations involved in this API
*
*       NOTE:
*       this function MUST be called under 'Interrupts are locked' and under
*       'Tasks lock'
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       oldDevNum  - old device number
*       newDevNum  - new device number (0..31)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_BAD_PARAM - the device oldDevNum not exist
*       GT_OUT_OF_RANGE - the device newDevNum value > 0x1f (0..31)
*       GT_ALREADY_EXIST - the new device number is already used
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgReNumberDevNum
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    oldDevNum;
    GT_U8    newDevNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    oldDevNum = (GT_U8)inArgs[0];
    newDevNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCfgReNumberDevNum(oldDevNum, newDevNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCfgIngressDropCntrSet
*
* DESCRIPTION:
*       Set the Ingress Drop Counter value.
*
* INPUTS:
*       devNum      - Physical device number.
*       counter     - Ingress Drop Counter value
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgIngressDropCntrSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      counter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    counter = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCfgIngressDropCntrSet(devNum, counter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgIngressDropCntrGet
*
* DESCRIPTION:
*       Get the Ingress Drop Counter value.
*
* INPUTS:
*       devNum      - Physical device number.
*
* OUTPUTS:
*       counterPtr  - (pointer to) Ingress Drop Counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgIngressDropCntrGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      counter;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCfgIngressDropCntrGet(devNum, &counter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", counter);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgIngressDropCntrModeSet
*
* DESCRIPTION:
*       Set the Ingress Drop Counter Mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - Device number.
*       mode      - Ingress Drop Counter Mode.
*       port      - port number monitored by Ingress Drop Counter.
*                   This parameter is applied upon
*                   CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E
*                   counter mode.
*       vlan      - VLAN ID monitored by Ingress Drop Counter.
*                   This parameter is applied upon
*                   CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E
*                   counter mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum or mode.
*       GT_OUT_OF_RANGE          - on wrong portNum or vlan.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgIngressDropCntrModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT mode;
    GT_PORT_NUM                                      portNum;
    GT_U16                                      vlan;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (GT_U32)inArgs[1];
    portNum = (GT_PORT_NUM)inArgs[2];
    vlan = (GT_U16)inArgs[3];

    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCfgIngressDropCntrModeSet(devNum, mode, (GT_PORT_NUM)portNum, vlan);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgIngressDropCntrModeGet
*
* DESCRIPTION:
*       Get the Ingress Drop Counter Mode.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum    - device number.
*
* OUTPUTS:
*       mode      - pointer to Ingress Drop Counter Mode.
*       port      - pointer to port number monitored by Ingress Drop Counter.
*                   This parameter is applied upon
*                   CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_PORT_E
*                   counter mode.
*       vlan      - pointer to VLAN ID monitored by Ingress Drop Counter.
*                   This parameter is applied upon
*                   CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_VLAN_E
*                   counter mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on hardware error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - illegal state of configuration
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgIngressDropCntrModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_HW_DEV_NUM       hwDevNum;
    CPSS_DXCH_CFG_INGRESS_DROP_COUNTER_MODE_ENT mode;
    GT_PORT_NUM                                 portNum;
    GT_U16                                      vlan;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    hwDevNum = (GT_HW_DEV_NUM)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCfgIngressDropCntrModeGet((GT_U8)hwDevNum, &mode, &portNum, &vlan);

    /* Override Back Device and Port */
    CONVERT_BACK_DEV_PORT_DATA_MAC(hwDevNum, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", mode, portNum, vlan);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgGlobalEportSet
*
* DESCRIPTION:
*       Set Global ePorts ranges configuration.
*       Global ePorts are ePorts that are global in the entire system, and are
*       not unique per device as regular ePorts are.
*
*       Global ePorts are used for Representing:
*       1. a Trunk.(map target ePort to trunkId by the E2PHY)
*       2. a ePort ECMP Group.(map 'primary' target ePort to 'secondary' target ePort)
*       3. an ePort Replication Group (eVIDX).(map target ePort to eVIDX group)
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
*       globalPtr   - (pointer to) Global EPorts for the Bridge,L2Mll to use.
*                           In the Bridge Used for:
*                           a) Whether the bridge engine looks at the device number
*                              when performing MAC SA lookup or local switching check.
*                           b) That the local device ID (own device) is learned in the
*                              FDB entry with the global ePort
*
*                           In the L2MLL Used for:
*                           Enabling the L2MLL replication engine to ignore the
*                           device ID when comparing source and target interfaces
*                           for source filtering.
*                           Typically configured to include global ePorts
*                           representing a trunk or an ePort ECMP group
*
*                       NOTE: For defining the EPorts that used for 'Multi-Target ePort to eVIDX Mapping'
*                           use API cpssDxChL2MllMultiTargetPortSet(...)
*                           (see cpssDxChL2Mll.h file for more related functions)
*
*       l2EcmpPtr   - (pointer to) L2 ECMP Primary ePorts.
*                     Used for: The 'Primary ePorts' that need to be converted to
*                      one of their 'Secondary ePorts'.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgGlobalEportSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    CPSS_DXCH_CFG_GLOBAL_EPORT_STC  ecmp;
    CPSS_DXCH_CFG_GLOBAL_EPORT_STC  global;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    global.enable = (GT_BOOL)inArgs[1];
    global.pattern = (GT_U32)inArgs[2];
    global.mask = (GT_U32)inArgs[3];

    ecmp.enable = (GT_BOOL)inArgs[4];
    ecmp.pattern = (GT_U32)inArgs[5];
    ecmp.mask = (GT_U32)inArgs[6];

    /* call cpss api function */
    result = cpssDxChCfgGlobalEportSet(devNum, &global , &ecmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgGlobalEportGet
*
* DESCRIPTION:
*       Get Global ePorts ranges configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       globalPtr   - (pointer to) Global EPorts for the Bridge,L2MLL to use.
*       l2EcmpPtr   - (pointer to) L2 ECMP Primary ePorts.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgGlobalEportGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    CPSS_DXCH_CFG_GLOBAL_EPORT_STC  ecmp = {0,0,0};
    CPSS_DXCH_CFG_GLOBAL_EPORT_STC  global;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCfgGlobalEportGet(devNum, &global,&ecmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d",
                 global.enable,global.pattern,global.mask,
                 ecmp.enable,ecmp.pattern,ecmp.mask);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgDevInfoGet
* 
* Return device family and subfamily info
* 
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum       - Device number
*
* OUTPUTS:
*       devInfoPtr   - (pointer to) device information
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgDevInfoGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result;
    GT_U8                        devNum;
    CPSS_DXCH_CFG_DEV_INFO_STC   devInfo;

    GT_UTIL_PP_FAMILY_TYPE_ENT      wrFamily;
    GT_UTIL_PP_SUB_FAMILY_TYPE_ENT  wrSubFamily;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCfgDevInfoGet(devNum, &devInfo);

    if( GT_OK != utilCpssToWrFamilyConvert(devInfo.genDevInfo.devFamily,&wrFamily) )
    {
        cmdOsPrintf("utilCpssToWrFamilyConvert failed\n");
        return GT_FAIL;
    }

    if( GT_OK != utilCpssToWrSubFamilyConvert(devInfo.genDevInfo.devSubFamily, &wrSubFamily) )
    {
        cmdOsPrintf("utilCpssToWrSubFamilyConvert failed\n");
        return GT_FAIL;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", devInfo.genDevInfo.devType,
                                                                    devInfo.genDevInfo.revision,
                                                                    wrFamily,
                                                                    wrSubFamily,
                                                                    devInfo.genDevInfo.maxPortNum,
                                                                    devInfo.genDevInfo.numOfVirtPorts,
                                                                    devInfo.genDevInfo.existingPorts.ports[0],
                                                                    devInfo.genDevInfo.existingPorts.ports[1],
                                                                    devInfo.genDevInfo.existingPorts.ports[2],
                                                                    devInfo.genDevInfo.existingPorts.ports[3],
                                                                    devInfo.genDevInfo.existingPorts.ports[4],
                                                                    devInfo.genDevInfo.existingPorts.ports[5],
                                                                    devInfo.genDevInfo.existingPorts.ports[6],
                                                                    devInfo.genDevInfo.existingPorts.ports[7],
                                                                    devInfo.genDevInfo.hwDevNumMode,
                                                                    devInfo.genDevInfo.cpuPortMode);

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChCfgHitlessWriteMethodEnableSet
*
* DESCRIPTION:
*       Enable/disable hitless write methood.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE:  hitless write methood is enable. In this case write operation
*                                 would be performed only if writing data is not equal with
*                                 affected memory contents.
*                       GT_FALSE: hitless write methood is disable. In this case write operation
*                                 is done at any rate.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgHitlessWriteMethodEnableSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCfgHitlessWriteMethodEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCfgPortDefaultSourceEportNumberSet
*
* DESCRIPTION:
*       Set Port default Source ePort number.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number
*       ePort         - ePort number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Default value is physical port number. In Multi-core
*          architecture it is the global physical port number (i.e. Local Core
*          Port number bitwise ORed with LocalCoreID<<4)
*       2. Can be overriden by TTI Action/RBridge
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgPortDefaultSourceEportNumberSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PHYSICAL_PORT_NUM               portNum;
    GT_PORT_NUM                        ePort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    ePort = (GT_PORT_NUM)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC (devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCfgPortDefaultSourceEportNumberSet(devNum, portNum, ePort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCfgPortDefaultSourceEportNumberGet
*
* DESCRIPTION:
*       Get Port default Source ePort number.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*
* OUTPUTS:
*       ePortPtr      - pointer to ePort number
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Default value is physical port number. In Multi-core
*          architecture it is the global physical port number (i.e. Local Core
*          Port number bitwise ORed with LocalCoreID<<4)
*       2. Can be overriden by TTI Action/RBridge
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCfgPortDefaultSourceEportNumberGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;
    GT_U8                              devNum;
    GT_PHYSICAL_PORT_NUM               portNum;
    GT_PORT_NUM                        ePort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCfgPortDefaultSourceEportNumberGet(devNum, portNum, &ePort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",ePort);
    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChCfgDevRemove",
        &wrCpssDxChCfgDevRemove,
        1, 0},

    {"cpssDxChCfgDevEnable",
        &wrCpssDxChCfgDevEnable,
        2, 0},

    {"cpssDxChCfgDevEnableGet",
        &wrCpssDxChCfgDevEnableGet,
        1, 0},

    {"cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet",
        &wrCpssDxChCfgDsaTagSrcDevPortRoutedPcktModifySet,
        3, 0},

    {"cpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet",
        &wrCpssDxChCfgDsaTagSrcDevPortRoutedPcktModifyGet,
        2, 0},

    {"cpssDxChCfgDsaTagSrcDevPortModifySet",
        &wrCpssDxChCfgDsaTagSrcDevPortModifySet,
        2, 0},

    {"cpssDxChCfgDsaTagSrcDevPortModifyGet",
        &wrCpssDxChCfgDsaTagSrcDevPortModifyGet,
        1, 0},

    {"cpssDxChCfgTableNumEntriesGet",
        &wrCpssDxChCfgTableNumEntriesGet,
        2, 0},

    {"cpssDxChCfgReNumberDevNum",
        &wrCpssDxChCfgReNumberDevNum,
        2, 0},

    {"cpssDxChCfgIngressDropCntrSet",
        &wrCpssDxChCfgIngressDropCntrSet,
        2, 0},

    {"cpssDxChCfgIngressDropCntrGet",
        &wrCpssDxChCfgIngressDropCntrGet,
        1, 0},

    {"cpssDxChCfgIngressDropCntrModeSet",
        &wrCpssDxChCfgIngressDropCntrModeSet,
        4, 0},

    {"cpssDxChCfgIngressDropCntrModeGet",
        &wrCpssDxChCfgIngressDropCntrModeGet,
        1, 0},

    {"cpssDxChCfgGlobalEportSet",
        &wrCpssDxChCfgGlobalEportSet,
        7, 0},

    {"cpssDxChCfgGlobalEportGet",
        &wrCpssDxChCfgGlobalEportGet,
        1, 0},

    {"cpssDxChCfgDevInfoGet",
        &wrCpssDxChCfgDevInfoGet,
        1, 0},

    {"cpssDxChCfgHitlessWriteMethodEnableSet",
        &wrCpssDxChCfgHitlessWriteMethodEnableSet,
        2, 0},

    {"cpssDxChCfgPortDefaultSourceEportNumberSet",
        &wrCpssDxChCfgPortDefaultSourceEportNumberSet,
        3, 0},

    {"cpssDxChCfgPortDefaultSourceEportNumberGet",
        &wrCpssDxChCfgPortDefaultSourceEportNumberGet,
        2, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChConfig
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
GT_STATUS cmdLibInitCpssDxChConfig
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

