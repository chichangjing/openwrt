/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChVnt.c
*
* DESCRIPTION:
*       TODO: Add proper description of this file here
*
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 6 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/vnt/cpssDxChVnt.h>

/*******************************************************************************
* cpssDxChVntOamPortLoopBackModeEnableSet
*
* DESCRIPTION:
*      Enable/Disable 802.3ah Loopback mode on the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      Loopback Mode is enabled for the port,
*      - All traffic destined for  this port but was not ingressed on this port
*        is discarded, except for FROM_CPU traffic - which is to allow the CPU
*        to send OAM control packets.
*      - Egress VLAN and spanning tree filtering is bypassed.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
*       enable   - GT_TRUE:  Enable OAM loopback mode.
*                  GT_FALSE: Disable OAM loopback mode.
*
*
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or port
*
*
* COMMENTS:
*      In the Policy TCAM, after the existing rule which traps OAMPDUs,
*      add a rule to redirect all traffic received on
*      the port back to the same port for getting loopback.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVntOamPortLoopBackModeEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PORT_NUM port;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChVntOamPortLoopBackModeEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVntOamPortLoopBackModeEnableGet
*
* DESCRIPTION:
*      Get 802.3ah Loopback mode on the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      Loopback Mode is enabled for the port,
*      - All traffic destined for  this port but was not ingressed on this port
*        is discarded, except for FROM_CPU traffic - which is to allow the CPU
*        to send OAM control packets.
*      - Egress VLAN and spanning tree filtering is bypassed.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum      - device number
*       port        - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to Loopback mode status:
*                     GT_TRUE:  Enable OAM loopback mode.
*                     GT_FALSE: Disable OAM loopback mode.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum or port
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
*
* COMMENTS:
*      In the Policy TCAM, after the existing rule which traps OAMPDUs,
*      add a rule to redirect all traffic received on
*      the port back to the same port for getting loopback.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVntOamPortLoopBackModeEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PORT_NUM port;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChVntOamPortLoopBackModeEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVntOamPortUnidirectionalEnableSet
*
* DESCRIPTION:
*      Enable/Disable the port for unidirectional transmit.
*      If unidirectional transmit enabled, sending
*      OAM (Operation, Administration, and Maintenance) control packets
*      and data traffic over failed links (ports with link down) is allowed.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
*       enable   - GT_TRUE:   Enable the port for unidirectional transmit.
*                  GT_FALSE:  Disable the port for unidirectional transmit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or port
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVntOamPortUnidirectionalEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM port;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChVntOamPortUnidirectionalEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVntOamPortUnidirectionalEnableGet
*
* DESCRIPTION:
*      Gets the current status of unidirectional transmit for the port.
*      If unidirectional transmit enabled, sending
*      OAM (Operation, Administration, and Maintenance) control packets
*      and data traffic over failed links (ports with link down) is allowed.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum      - device number
*       port        - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to current status of unidirectional transmit:
*                     GT_TRUE:   Enable the port for unidirectional transmit.
*                     GT_FALSE:  Disable the port for unidirectional transmit.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum or port
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVntOamPortUnidirectionalEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PHYSICAL_PORT_NUM port;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChVntOamPortUnidirectionalEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVntCfmEtherTypeSet
*
* DESCRIPTION:
*       Sets the EtherType to identify CFM (Connectivity Fault Management) PDUs.
*       If the packet EtherType matches the CFM EtherType, the ingress Policy
*       key implicitly uses the three User-Defined-Byte (UDB0, UDB1, UDB2) to
*       contain the three CFM data fields, MD Level, Opcode, and Flags,
*       respectively.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum      - device number
*       etherType   - CFM (Connectivity Fault Management) EtherType,
*                     values 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_HW_ERROR       - on hardware error
*       GT_BAD_PARAM      - wrong devNum.
*       GT_OUT_OF_RANGE   - on wrong etherType values.
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVntCfmEtherTypeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 etherType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    etherType = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChVntCfmEtherTypeSet(devNum, etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVntCfmEtherTypeGet
*
* DESCRIPTION:
*       Gets the current EtherType to identify CFM
*       (Connectivity Fault Management) PDUs.
*       If the packet EtherType matches the CFM EtherType, the ingress Policy
*       key implicitly uses the three User-Defined-Byte (UDB0, UDB1, UDB2) to
*       contain the three CFM data fields, MD Level, Opcode, and Flags,
*       respectively.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       etherTypePtr - pointer to CFM (Connectivity Fault Management) EtherType.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVntCfmEtherTypeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 etherType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChVntCfmEtherTypeGet(devNum, &etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", etherType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVntCfmLbrOpcodeSet
*
* DESCRIPTION:
*      Sets the CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum      - device number
*       opcode      - CFM LBR Opcode, values 0..0xFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_HW_ERROR       - on hardware error
*       GT_BAD_PARAM      - wrong devNum, opcode.
*       GT_OUT_OF_RANGE   - on wrong opcode values.
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVntCfmLbrOpcodeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 opcode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    opcode = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChVntCfmLbrOpcodeSet(devNum, opcode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVntCfmLbrOpcodeGet
*
* DESCRIPTION:
*      Gets the current CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       opcodePtr   - pointer to CFM LBR Opcode
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum.
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVntCfmLbrOpcodeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 opcode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChVntCfmLbrOpcodeGet(devNum, &opcode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", opcode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVntLastReadTimeStampGet
*
* DESCRIPTION:
*      Gets the last-read-time-stamp counter value that represent the exact
*      timestamp of the last read operation to the PP registers.
*
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       timeStampValuePtr - pointer to timestamp of the last read operation.
*                           200  MHz:   granularity - 5 nSec
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVntLastReadTimeStampGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 timeStampValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChVntLastReadTimeStampGet(devNum, &timeStampValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", timeStampValue);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVntOamPortPduTrapEnableSet
*
* DESCRIPTION:
*      Enable/Disable trap to CPU of 802.3ah Link Layer Control protocol on
*      the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      PDU trap is enabled for the port:
*      - Packets identified as OAM-PDUs destined for this port are trapped to the CPU.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
*       enable   - GT_TRUE:  Enable trap to CPU of 802.3ah Link Layer Control protocol.
*                  GT_FALSE: Disable trap to CPU of 802.3ah Link Layer Control protocol.
*
*
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVntOamPortPduTrapEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PORT_NUM port;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChVntOamPortPduTrapEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVntOamPortPduTrapEnableGet
*
* DESCRIPTION:
*      Get trap to CPU status(Enable/Disable) of 802.3ah Link Layer Control protocol on
*      the specified port.
*      If OAM (Operation, Administration, and Maintenance)
*      PDU trap is enabled for the port:
*      - Packets identified as OAM-PDUs are trapped to the CPU.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum   - device number
*       port     - physical port number including CPU port.
*
*
*
* OUTPUTS:
*       enablePtr   - pointer to Loopback mode status:
*                     GT_TRUE:  Enable trap to CPU of 802.3ah Link Layer Control protocol.
*                     GT_FALSE: Disable trap to CPU of 802.3ah Link Layer Control protocol..
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on bad pointer of enablePtr
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVntOamPortPduTrapEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;
    GT_PORT_NUM port;
    
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    
    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChVntOamPortPduTrapEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVntCfmReplyModeSet
*
* DESCRIPTION:
*      Sets the CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode change mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       mode    - CFM Opcode change mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, change mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVntCfmReplyModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChVntCfmReplyModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChVntCfmReplyModeGet
*
* DESCRIPTION:
*      Gets the CFM (Connectivity Fault Management)
*      LBR (Loopback Response) opcode change mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - (pointer to) CFM Opcode change mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, change mode.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - output parameter is NULL pointer.
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChVntCfmReplyModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_VNT_CFM_REPLY_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChVntCfmReplyModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssDxChVntOamPortLoopBackModeEnableSet",
         &wrCpssDxChVntOamPortLoopBackModeEnableSet,
         3, 0},
        {"cpssDxChVntOamPortLoopBackModeEnableGet",
         &wrCpssDxChVntOamPortLoopBackModeEnableGet,
         2, 0},
        {"cpssDxChVntOamPortUnidirectionalEnableSet",
         &wrCpssDxChVntOamPortUnidirectionalEnableSet,
         3, 0},
        {"cpssDxChVntOamPortUnidirectionalEnableGet",
         &wrCpssDxChVntOamPortUnidirectionalEnableGet,
         2, 0},
        {"cpssDxChVntCfmEtherTypeSet",
         &wrCpssDxChVntCfmEtherTypeSet,
         2, 0},
        {"cpssDxChVntCfmEtherTypeGet",
         &wrCpssDxChVntCfmEtherTypeGet,
         1, 0},
        {"cpssDxChVntCfmLbrOpcodeSet",
         &wrCpssDxChVntCfmLbrOpcodeSet,
         2, 0},
        {"cpssDxChVntCfmLbrOpcodeGet",
         &wrCpssDxChVntCfmLbrOpcodeGet,
         1, 0},
        {"cpssDxChVntLastReadTimeStampGet",
         &wrCpssDxChVntLastReadTimeStampGet,
         1, 0},
        {"cpssDxChVntOamPortPduTrapEnableSet",
         &wrCpssDxChVntOamPortPduTrapEnableSet,
         3, 0},
        {"cpssDxChVntOamPortPduTrapEnableGet",
         &wrCpssDxChVntOamPortPduTrapEnableGet,
         2, 0},

        {"cpssDxChVntCfmReplyModeSet",
         &wrCpssDxChVntCfmReplyModeSet,
         2, 0},
        {"cpssDxChVntCfmReplyModeGet",
         &wrCpssDxChVntCfmReplyModeGet,
         1, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChVnt
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
GT_STATUS cmdLibInitCpssDxChVnt
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

