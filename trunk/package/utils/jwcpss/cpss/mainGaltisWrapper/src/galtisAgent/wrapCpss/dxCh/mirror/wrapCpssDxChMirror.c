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
* wrapCpssDxChMirror.c
*
* DESCRIPTION:
*       Wrapper functions for Mirror cpss.dx Chita2 functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 20 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h>


/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagEnable
*
* DESCRIPTION:
*       Enable/Disable Analyzer port adding additional VLAN Tag to mirrored
*       packets. The content of this VLAN tag is configured
*       by gmirrSetAnalyzerVlanTagConfig.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       dev         - packet processor number.
*       portNum     - ports number to be set
*       enable      - GT_TRUE - additional VLAN tag inserted to mirrored packets
*                     GT_FALSE - no additional VLAN tag inserted to mirrored packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_NOT_SUPPORTED  - not supported device type.
*       GT_BAD_PARAM      - wrong device or port number.
*       GT_HW_ERROR       - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorAnalyzerVlanTagEnable
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_PORT_NUM       portNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    enable =  (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChMirrorAnalyzerVlanTagEnable(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagEnableGet
*
* DESCRIPTION:
*       Get Analyzer port VLAN Tag to mirrored packets mode.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       dev         - packet processor number.
*       portNum     - ports number to be set
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE - additional VLAN tag inserted to mirrored packets
*                     GT_FALSE - no additional VLAN tag inserted to mirrored packets

*
* RETURNS:
*       GT_OK             - on success.
*       GT_NOT_SUPPORTED  - not supported device type.
*       GT_BAD_PARAM      - wrong device or port number.
*       GT_HW_ERROR       - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorAnalyzerVlanTagEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_PORT_NUM       portNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChMirrorAnalyzerVlanTagEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerVlanTagConfig
*
* DESCRIPTION:
*      Set global Tx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*      dev         - packet processor number.
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_NOT_SUPPORTED  - not supported device type.
*       GT_BAD_PARAM      - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*
*
* COMMENTS:
*      Supported for 98DX2x5 devices only
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxAnalyzerVlanTagConfig
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                       devNum;
    CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC  analyzerVlanTagConfig;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    analyzerVlanTagConfig.etherType = (GT_U16)inArgs[1];
    analyzerVlanTagConfig.vpt = (GT_U8)inArgs[2];
    analyzerVlanTagConfig.cfi = (GT_U8)inArgs[3];
    analyzerVlanTagConfig.vid = (GT_U16)inArgs[4];

    /* call cpss api function */
    result = cpssDxChMirrorTxAnalyzerVlanTagConfig(devNum,
                                                   &analyzerVlanTagConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerVlanTagConfigGet
*
* DESCRIPTION:
*      Get global Tx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*      dev         - packet processor number.
*
* OUTPUTS:
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* RETURNS:
*       GT_OK             - on success.
*       GT_NOT_SUPPORTED  - not supported device type.
*       GT_BAD_PARAM      - wrong device number.
*       GT_HW_ERROR       - on writing to HW error.
*       GT_BAD_PTR        - on NULL pointer
*
*
* COMMENTS:
*      Supported for 98DX2x5 devices only
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxAnalyzerVlanTagConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                       devNum;
    CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC  analyzerVlanTagConfig;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrorTxAnalyzerVlanTagConfigGet(devNum,
                                                   &analyzerVlanTagConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", analyzerVlanTagConfig.etherType,
                                      analyzerVlanTagConfig.vpt,
                                      analyzerVlanTagConfig.cfi,
                                      analyzerVlanTagConfig.vid);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerVlanTagConfig
*
* DESCRIPTION:
*      Set global Rx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*      dev         - packet processor number.
*      mirrorType - defines in which one Analyzer port to set or both
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_NOT_SUPPORTED  - not supported device type.
*       GT_BAD_PARAM      - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*
*
* COMMENTS:
*      Supported for 98DX2x5 devices only
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorRxAnalyzerVlanTagConfig
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                       devNum;
    CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC  analyzerVlanTagConfig;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    analyzerVlanTagConfig.etherType = (GT_U16)inArgs[1];
    analyzerVlanTagConfig.vpt = (GT_U8)inArgs[2];
    analyzerVlanTagConfig.cfi = (GT_U8)inArgs[3];
    analyzerVlanTagConfig.vid = (GT_U16)inArgs[4];

    /* call cpss api function */
    result = cpssDxChMirrorRxAnalyzerVlanTagConfig(devNum,
                                                   &analyzerVlanTagConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerVlanTagConfigGet
*
* DESCRIPTION:
*      Get global Rx mirroring VLAN tag details: EtherType, VID, VPT, CFI
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*      dev         - packet processor number.
*
* OUTPUTS:
*      analyzerVlanTagConfigPtr - Pointer to analyzer Vlan tag configuration:
*                              EtherType, VPT, CFI and VID
*
* RETURNS:
*       GT_OK             - on success.
*       GT_NOT_SUPPORTED  - not supported device type.
*       GT_BAD_PARAM      - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR        - on NULL pointer
*
*
* COMMENTS:
*      Supported for 98DX2x5 devices only
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorRxAnalyzerVlanTagConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                       devNum;
    CPSS_DXCH_MIRROR_ANALYZER_VLAN_TAG_CFG_STC  analyzerVlanTagConfig;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrorRxAnalyzerVlanTagConfigGet(devNum,
                                                   &analyzerVlanTagConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", analyzerVlanTagConfig.etherType,
                                      analyzerVlanTagConfig.vpt,
                                      analyzerVlanTagConfig.cfi,
                                      analyzerVlanTagConfig.vid);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerPortSet
*
* DESCRIPTION:
*       Sets a port to be an analyzer port of tx mirrored
*       ports on all system pp's.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev  - the device number
*       analyzerPort - port number of analyzer port
*       analyzerdev - pp Device number of analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxAnalyzerPortSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_PORT_NUM     analyzerPort;
    GT_HW_DEV_NUM   analyzerHwDev;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC  interface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    analyzerPort = (GT_PORT_NUM)inArgs[1];
    analyzerHwDev =  (GT_HW_DEV_NUM)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_DATA_MAC(analyzerHwDev, analyzerPort);

    if(PRV_CPSS_PP_MAC(dev)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* call cpss api function */
        result = cpssDxChMirrorTxAnalyzerPortSet(dev, analyzerPort, analyzerHwDev);
    }
    else
    {
        interface.interface.type = CPSS_INTERFACE_PORT_E;
        interface.interface.devPort.hwDevNum = analyzerHwDev;
        interface.interface.devPort.portNum = analyzerPort;

        /* call cpss api function */
        result = cpssDxChMirrorAnalyzerInterfaceSet(dev, 1, &interface);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxAnalyzerPortGet
*
* DESCRIPTION:
*       Gets the analyzer port of tx mirrored ports on all system pp's.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev  - the device number
*
* OUTPUTS:
*       analyzerPortPtr - (pointer to)port number of analyzer port
*       analyzerDevPtr - (pointer to)pp Device number of analyzer port
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - wrong device number.
*       GT_HW_ERROR       - on writing to HW error.
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxAnalyzerPortGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;

    GT_PORT_NUM     analyzerPort;
    GT_HW_DEV_NUM   analyzerHwDev;

    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC  interface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];

    if(PRV_CPSS_PP_MAC(dev)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* call cpss api function */
        result = cpssDxChMirrorTxAnalyzerPortGet(dev, &analyzerPort, &analyzerHwDev);
    }
    else
    {
        /* call cpss api function */
        result = cpssDxChMirrorAnalyzerInterfaceGet(dev, 1, &interface);
        analyzerHwDev = interface.interface.devPort.hwDevNum;
        analyzerPort = interface.interface.devPort.portNum;
    }

    /* Override Back Device and Port */
    CONVERT_BACK_DEV_PORT_DATA_MAC(analyzerHwDev, analyzerPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", analyzerPort, analyzerHwDev);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerPortSet
*
* DESCRIPTION:
*       Sets a specific port to be an analyzer port of Rx mirrored
*       ports.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*
*       dev  - the device number
*       analyzerPort - port number of analyzer port
*       analyzerdev - pp Device number of analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorRxAnalyzerPortSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_PORT_NUM     analyzerPort;
    GT_HW_DEV_NUM   analyzerHwDev;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   interface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    analyzerPort =  (GT_PORT_NUM)inArgs[1];
    analyzerHwDev =  (GT_HW_DEV_NUM)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_DATA_MAC(analyzerHwDev, analyzerPort);

   if(PRV_CPSS_PP_MAC(dev)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
   {
       /* call cpss api function */
       result = cpssDxChMirrorRxAnalyzerPortSet(dev, analyzerPort, analyzerHwDev);
   }
   else
   {
       interface.interface.type = CPSS_INTERFACE_PORT_E;
       interface.interface.devPort.hwDevNum = analyzerHwDev;
       interface.interface.devPort.portNum = analyzerPort;
       /* call cpss api function */
       result = cpssDxChMirrorAnalyzerInterfaceSet(dev, 0, &interface);
   }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorRxAnalyzerPortGet
*
* DESCRIPTION:
*       Gets the analyzer port of Rx mirrored ports.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*
*       dev  - the device number
*
* OUTPUTS:
*       analyzerPortPtr - (pointer to)port number of analyzer port
*       analyzerDevPtr - (pointer to)pp Device number of analyzer port
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorRxAnalyzerPortGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_PORT_NUM     analyzerPort;
    GT_HW_DEV_NUM   analyzerHwDev;

    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC  interface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];

    if(PRV_CPSS_PP_MAC(dev)->devFamily < CPSS_PP_FAMILY_DXCH_XCAT_E)
    {
        /* call cpss api function */
        result = cpssDxChMirrorRxAnalyzerPortGet(dev, &analyzerPort, &analyzerHwDev);
    }
    else
    {
        /* call cpss api function */
        result = cpssDxChMirrorAnalyzerInterfaceGet(dev, 0, &interface);
        analyzerHwDev = interface.interface.devPort.hwDevNum;
        analyzerPort = interface.interface.devPort.portNum;
    }

    /* Override Back Device and Port */
    CONVERT_BACK_DEV_PORT_DATA_MAC(analyzerHwDev, analyzerPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", analyzerPort, analyzerHwDev);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxCascadeMonitorEnable
*
* DESCRIPTION:
*       One global bit that is set to 1 when performing egress mirroring or
*       egress STC of any of the cascading ports.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev       - the device number
*       enable    - enable/disable Egress Monitoring on cascading ports.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxCascadeMonitorEnable
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    enable =  (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChMirrorTxCascadeMonitorEnable(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxCascadeMonitorEnableGet
*
* DESCRIPTION:
*       Get One global bit that indicate performing egress mirroring or
*       egress STC of any of the cascading ports.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev       - the device number
*
* OUTPUTS:
*       enablePtr    - enable/disable Egress Monitoring on cascading ports.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxCascadeMonitorEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrorTxCascadeMonitorEnableGet(dev, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorRxPortSet
*
* DESCRIPTION:
*       Sets a specific port or group of ports in pp's devices to be Rx
*       mirrored ports
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev       - the device number
*       mirrPort  - array of logical ports.
*       enable    - enable/disable Rx mirror on this port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - wrong device, mirrPort.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       CPU port can not be Rx mirrored port.
*
*       the Salsa device support CPU port as Rx mirror port .
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorRxPortSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_PORT_NUM       mirrPort;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    mirrPort =  (GT_PORT_NUM)inArgs[1];
    enable =  (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, mirrPort);

    /* call cpss api function */
    result = cpssDxChMirrorRxPortSet(dev, mirrPort, GT_TRUE, enable, 0);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorRxPortGet
*
* DESCRIPTION:
*       Gets a specific port or group of ports in pp's devices to be Rx
*       mirrored ports
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev       - the device number
*       mirrPort  - array of logical ports.
*
* OUTPUTS:
*       enablePtr    - (pointer to) Rx mirror mode
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - wrong device, mirrPort.
*       GT_HW_ERROR       - on writing to HW error.
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*
* COMMENTS:
*       CPU port can not be Rx mirrored port.
*
*       the Salsa device support CPU port as Rx mirror port .
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorRxPortGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_PORT_NUM       mirrPort;
    GT_BOOL     enable;
    GT_U32      index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    mirrPort =  (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, mirrPort);

    /* call cpss api function */
    result = cpssDxChMirrorRxPortGet(dev, mirrPort, GT_TRUE, &enable, &index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorRxPortSet
*
* DESCRIPTION:
*       Sets a specific port to be Rx mirrored port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - the device number
*       mirrPort  - port number, CPU port supported.
*       enable    - enable/disable Rx mirror on this port
*                   GT_TRUE - Rx mirroring enabled, packets
*                             received on a mirrPort are
*                             mirrored to Rx analyzer.
*                   GT_FALSE - Rx mirroring disabled.
*       index     - Analyzer destination interface index. (0 - 6)
*                   Supported for DxChXcat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorRxPortExtSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_PORT_NUM mirrPort;
    GT_BOOL     enable;
    GT_U32      index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    mirrPort =  (GT_PORT_NUM)inArgs[1];
    enable =  (GT_BOOL)inArgs[2];
    index = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, mirrPort);

    /* call cpss api function */
    result = cpssDxChMirrorRxPortSet(dev, mirrPort, GT_TRUE, enable, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorRxPortGet
*
* DESCRIPTION:
*       Gets status of Rx mirroring (enabled or disabled) of specific port
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - the device number
*       mirrPort  - port number, CPU port supported.
*
* OUTPUTS:
*       enablePtr    - (pointer to) Rx mirror mode
*                   GT_TRUE - Rx mirroring enabled, packets
*                             received on a mirrPort are
*                             mirrored to Rx analyzer.
*                   GT_FALSE - Rx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index. (0 - 6)
*                   Supported for DxChXcat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorRxPortExtGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_PORT_NUM mirrPort;
    GT_BOOL     enable;
    GT_U32      index = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    mirrPort =  (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, mirrPort);

    /* call cpss api function */
    result = cpssDxChMirrorRxPortGet(dev, mirrPort, GT_TRUE, &enable, &index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, index);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorRxPortSet
*
* DESCRIPTION:
*       Sets a specific port to be Rx mirrored port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - the device number
*       mirrPort  - port number, CPU port supported.
*       enable    - enable/disable Rx mirror on this port
*                   GT_TRUE - Rx mirroring enabled, packets
*                             received on a mirrPort are
*                             mirrored to Rx analyzer.
*                   GT_FALSE - Rx mirroring disabled.
*       index     - Analyzer destination interface index. (0 - 6)
*                   Supported for DxChXcat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorRxPortSet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_PORT_NUM mirrPort;
    GT_BOOL     enable;
    GT_U32      index;
    GT_BOOL     isPhysicalPort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    mirrPort =  (GT_PORT_NUM)inArgs[1];
    isPhysicalPort = (GT_BOOL)inArgs[2];
    enable =  (GT_BOOL)inArgs[3];
    index = (GT_U32)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, mirrPort);

    /* call cpss api function */
    result = cpssDxChMirrorRxPortSet(dev, mirrPort, isPhysicalPort, enable, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorRxPortGet
*
* DESCRIPTION:
*       Gets status of Rx mirroring (enabled or disabled) of specific port
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - the device number
*       mirrPort  - port number, CPU port supported.
*
* OUTPUTS:
*       enablePtr    - (pointer to) Rx mirror mode
*                   GT_TRUE - Rx mirroring enabled, packets
*                             received on a mirrPort are
*                             mirrored to Rx analyzer.
*                   GT_FALSE - Rx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index. (0 - 6)
*                   Supported for DxChXcat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_TIMEOUT  - after max number of retries checking if PP ready
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorRxPortGet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_PORT_NUM mirrPort;
    GT_BOOL     enable;
    GT_U32      index = 0;
    GT_BOOL     isPhysicalPort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    mirrPort =  (GT_PORT_NUM)inArgs[1];
    isPhysicalPort = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, mirrPort);

    /* call cpss api function */
    result = cpssDxChMirrorRxPortGet(dev, mirrPort, isPhysicalPort, &enable, &index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, index);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxPortSet
*
* DESCRIPTION:
*       Adds ports to be Tx mirrored ports .
*       Up to 8 ports in all system can be tx mirrored ports.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev       - the device number
*       mirrPort  - array of ports.
*       enable    - enable/disable Rx mirror on this port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - wrong device or mirrPort.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       There can be up to 8 Tx mirrored ports per Prestera chipset.
*       CPU port can be Tx mirrored port.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxPortSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_PORT_NUM       mirrPort;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    mirrPort =  (GT_PORT_NUM)inArgs[1];
    enable =  (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, mirrPort);

    /* call cpss api function */
    result = cpssDxChMirrorTxPortSet(dev, mirrPort, GT_TRUE, enable, 0);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxPortSet
*
* DESCRIPTION:
*       Enable or disable Tx mirroring per port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - the device number
*       mirrPort  - port number.
*       enable    - enable/disable Tx mirror on this port
*                   GT_TRUE - Tx mirroring enabled, packets
*                             transmitted from a mirrPort are
*                             mirrored to Tx analyzer.
*                   GT_FALSE - Tx mirroring disabled.
*       index     - Analyzer destination interface index. (0 - 6)
*                   Supported for DxChXcat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxPortExtSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_PORT_NUM mirrPort;
    GT_BOOL     enable;
    GT_U32      index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    mirrPort =  (GT_PORT_NUM)inArgs[1];
    enable =  (GT_BOOL)inArgs[2];
    index =  (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, mirrPort);

    /* call cpss api function */
    result = cpssDxChMirrorTxPortSet(dev, mirrPort, GT_TRUE, enable, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxPortGet
*
* DESCRIPTION:
*       Adds ports to be Tx mirrored ports .
*       Up to 8 ports in all system can be tx mirrored ports.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev       - the device number
*       mirrPort  - array of ports.
*
* OUTPUTS:
*       enablePtr    - (pointer to) Tx mirror mode on this port
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - wrong device or mirrPort.
*       GT_HW_ERROR       - on writing to HW error.
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*
* COMMENTS:
*       There can be up to 8 Tx mirrored ports per Prestera chipset.
*       CPU port can be Tx mirrored port.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxPortGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_PORT_NUM       mirrPort;
    GT_BOOL     enable;
    GT_U32      index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    mirrPort =  (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, mirrPort);

    /* call cpss api function */
    result = cpssDxChMirrorTxPortGet(dev, mirrPort, GT_TRUE, &enable, &index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxPortGet
*
* DESCRIPTION:
*       Get status (enabled/disabled) of Tx mirroring per port .
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - the device number
*       mirrPort  - port number.
*
* OUTPUTS:
*       enablePtr - (pointer to) Tx mirror mode on this port
*                                GT_TRUE - Tx mirroring enabled, packets
*                                          transmitted from a mirrPort are
*                                          mirrored to Tx analyzer.
*                                GT_FALSE - Tx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index. (0 - 6)
*                   Supported for DxChXcat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxPortExtGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_PORT_NUM mirrPort;
    GT_BOOL     enable;
    GT_U32      index = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    mirrPort =  (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, mirrPort);

    /* call cpss api function */
    result = cpssDxChMirrorTxPortGet(dev, mirrPort, GT_TRUE, &enable, &index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, index);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxPortSet
*
* DESCRIPTION:
*       Enable or disable Tx mirroring per port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - the device number
*       mirrPort  - port number.
*       enable    - enable/disable Tx mirror on this port
*                   GT_TRUE - Tx mirroring enabled, packets
*                             transmitted from a mirrPort are
*                             mirrored to Tx analyzer.
*                   GT_FALSE - Tx mirroring disabled.
*       index     - Analyzer destination interface index. (0 - 6)
*                   Supported for DxChXcat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxPortSet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_PORT_NUM mirrPort;
    GT_BOOL     enable;
    GT_U32      index;
    GT_BOOL     isPhysicalPort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    mirrPort =  (GT_PORT_NUM)inArgs[1];
    isPhysicalPort = (GT_BOOL)inArgs[2];
    enable =  (GT_BOOL)inArgs[3];
    index =  (GT_U32)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, mirrPort);

    /* call cpss api function */
    result = cpssDxChMirrorTxPortSet(dev, mirrPort, isPhysicalPort, enable, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxPortGet
*
* DESCRIPTION:
*       Get status (enabled/disabled) of Tx mirroring per port .
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - the device number
*       mirrPort  - port number.
*
* OUTPUTS:
*       enablePtr - (pointer to) Tx mirror mode on this port
*                                GT_TRUE - Tx mirroring enabled, packets
*                                          transmitted from a mirrPort are
*                                          mirrored to Tx analyzer.
*                                GT_FALSE - Tx mirroring disabled.
*       indexPtr  - (pointer to) Analyzer destination interface index. (0 - 6)
*                   Supported for DxChXcat and above device.
*                   Used only if forwarding mode to analyzer is Source-based.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mirrPort.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxPortGet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8       dev;
    GT_PORT_NUM mirrPort;
    GT_BOOL     enable;
    GT_U32      index = 0;
    GT_BOOL     isPhysicalPort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    mirrPort =  (GT_PORT_NUM)inArgs[1];
    isPhysicalPort = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, mirrPort);

    /* call cpss api function */
    result = cpssDxChMirrorTxPortGet(dev, mirrPort, isPhysicalPort, &enable, &index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, index);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrRxStatMirroringToAnalyzerRatioSet
*
* DESCRIPTION:
*       Set the statistical mirroring rate in the Rx Analyzer port
*       Set Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*      dev   - packet processor number.
*      ratio - Indicates the ratio of egress mirrored to analyzer port packets
*              forwarded to the analyzer port. 1 of every 'ratio' packets are
*              forwarded to the analyzer port.
*
*      Examples:
*         ratio 0 -> no Rx mirroring
*         ratio 1 -> all Rx mirrored packets are forwarded out the Rx
*             Analyzer port
*         ratio 10 -> 1 in 10 Rx mirrored packets are forwarded out the Rx
*            Analyzer port
*         The maximum ratio value is 2047.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - wrong device or ratio.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrRxStatMirroringToAnalyzerRatioSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_U32      ratio;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    ratio =  (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChMirrRxStatMirroringToAnalyzerRatioSet(dev, ratio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrRxStatMirroringToAnalyzerRatioGet
*
* DESCRIPTION:
*       Get the statistical mirroring rate in the Rx Analyzer port
*       Get Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*      dev   - packet processor number.
*
* OUTPUTS:
*      ratioPtr - (pointer to) Indicates the ratio of egress mirrored to analyzer port packets
*                 forwarded to the analyzer port. 1 of every 'ratio' packets are
*                 forwarded to the analyzer port.
*
*      Examples:
*         ratio 0 -> no Rx mirroring
*         ratio 1 -> all Rx mirrored packets are forwarded out the Rx
*             Analyzer port
*         ratio 10 -> 1 in 10 Rx mirrored packets are forwarded out the Rx
*            Analyzer port
*         The maximum ratio value is 2047.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - wrong device
*       GT_HW_ERROR       - on writing to HW error.
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrRxStatMirroringToAnalyzerRatioGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_U32      ratio;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrRxStatMirroringToAnalyzerRatioGet(dev, &ratio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ratio);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrRxStatMirrorToAnalyzerEnable
*
* DESCRIPTION:
*       Enable Ingress Statistical Mirroring to the Egress Analyzer Port
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*      dev    - packet processor number.
*      enable - enable\disable Ingress Statistical Mirroring.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrRxStatMirrorToAnalyzerEnable
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    enable =  (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChMirrRxStatMirrorToAnalyzerEnable(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrRxStatMirrorToAnalyzerEnableGet
*
* DESCRIPTION:
*       Get Ingress Statistical Mirroring to the Ingress Analyzer Port Mode
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*      dev    - packet processor number.
*
* OUTPUTS:
*      enablePtr - (pointer to) Ingress Statistical Mirroring mode.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrRxStatMirrorToAnalyzerEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrRxStatMirrorToAnalyzerEnableGet(dev, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrRxAnalyzerDpTcSet
*
* DESCRIPTION:
*       The TC/DP assigned to the packet forwarded to the ingress analyzer port due
*       to ingress mirroring to the analyzer port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*      dev          - packet processor number.
*      analyzerDp   - the Drop Precedence to be set
*      analyzerTc   - traffic class on analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_BAD_PARAM      - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrRxAnalyzerDpTcSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8             dev;
    CPSS_DP_LEVEL_ENT analyzerDp;
    GT_U8             analyzerTc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    analyzerDp =  (CPSS_DP_LEVEL_ENT)inArgs[1];
    analyzerTc =  (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChMirrRxAnalyzerDpTcSet(dev, analyzerDp, analyzerTc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrRxAnalyzerDpTcGet
*
* DESCRIPTION:
*       Get TC/DP assigned to the packet forwarded to the ingress analyzer port due
*       to ingress mirroring to the analyzer port cofiguration.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*      dev             - packet processor number.
*
* OUTPUTS:
*      analyzerDpPtr   - pointer to the Drop Precedence.
*      analyzerTcPtr   - pointer to traffic class on analyzer port
*
* RETURNS:
*       GT_OK          - on success.
*       GT_BAD_PARAM   - wrong device number.
*       GT_BAD_PTR     - wrong pointer.
*       GT_HW_ERROR    - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrRxAnalyzerDpTcGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8             dev;
    CPSS_DP_LEVEL_ENT analyzerDp;
    GT_U8             analyzerTc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrRxAnalyzerDpTcGet(dev, &analyzerDp, &analyzerTc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", analyzerDp, analyzerTc);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrTxAnalyzerDpTcSet
*
* DESCRIPTION:
*       The TC/DP assigned to the packet forwarded to the egress analyzer port due
*       to ingress mirroring to the analyzer port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*      dev          - packet processor number.
*      analyzerDp   - the Drop Precedence to be set
*      analyzerTc   - traffic class on analyzer port
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrTxAnalyzerDpTcSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8             dev;
    CPSS_DP_LEVEL_ENT analyzerDp;
    GT_U8             analyzerTc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    analyzerDp =  (CPSS_DP_LEVEL_ENT)inArgs[1];
    analyzerTc =  (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChMirrTxAnalyzerDpTcSet(dev, analyzerDp, analyzerTc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrTxAnalyzerDpTcGet
*
* DESCRIPTION:
*       Get TC/DP assigned to the packet forwarded to the egress analyzer port due
*       to ingress mirroring to the analyzer port cofiguration.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*      dev             - packet processor number.
*
* OUTPUTS:
*      analyzerDpPtr   - pointer to the Drop Precedence.
*      analyzerTcPtr   - pointer to traffic class on analyzer port
*
* RETURNS:
*       GT_OK          - on success.
*       GT_BAD_PARAM   - wrong device number.
*       GT_BAD_PTR     - wrong pointer.
*       GT_HW_ERROR    - on writing to HW error.
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrTxAnalyzerDpTcGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8             dev;
    CPSS_DP_LEVEL_ENT analyzerDp;
    GT_U8             analyzerTc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrTxAnalyzerDpTcGet(dev, &analyzerDp, &analyzerTc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", analyzerDp, analyzerTc);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrTxStatMirroringToAnalyzerPortRatioSet
*
* DESCRIPTION:
*       Set the statistical mirroring rate in the Tx Analyzer port
*       Set Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*      dev   - packet processor number.
*      ratio - Indicates the ratio of egress mirrored to analyzer port packets
*              forwarded to the analyzer port. 1 of every 'ratio' packets are
*              forwarded to the analyzer port.
*
*      Examples:
*         ratio 0 -> no Tx mirroring
*         ratio 1 -> all Tx mirrored packets are forwarded out the Tx
*             Analyzer port
*         ratio 10 -> 1 in 10 Tx mirrored packets are forwarded out the Tx
*            Analyzer port
*         The maximum ratio value is 2047.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device or ratio.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrTxStatMirroringToAnalyzerRatioSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_U32      ratio;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    ratio =  (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChMirrTxStatMirroringToAnalyzerRatioSet(dev, ratio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrTxStatMirroringToAnalyzerRatioGet
*
* DESCRIPTION:
*       Get the statistical mirroring rate in the Tx Analyzer port
*       Get Statistic mirroring to analyzer port statistical ratio configuration.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*      dev   - packet processor number.
*
* OUTPUTS:
*      ratioPtr - (pointer to)Indicates the ratio of egress mirrored to analyzer
*              port packets forwarded to the analyzer port. 1 of every 'ratio'
*              packets are forwarded to the analyzer port.
*
*      Examples:
*         ratio 0 -> no Tx mirroring
*         ratio 1 -> all Tx mirrored packets are forwarded out the Tx
*             Analyzer port
*         ratio 10 -> 1 in 10 Tx mirrored packets are forwarded out the Tx
*            Analyzer port
*         The maximum ratio value is 2047.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR      - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrTxStatMirroringToAnalyzerRatioGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_U32      ratio;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrTxStatMirroringToAnalyzerRatioGet(dev, &ratio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ratio);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrTxStatMirrorToAnalyzerEnable
*
* DESCRIPTION:
*       Enable Egress Statistical Mirroring to the Egress Analyzer Port
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*      dev    - packet processor number.
*      enable - enable\disable Egress Statistical Mirroring.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrTxStatMirrorToAnalyzerEnable
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];
    enable =  (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChMirrTxStatMirrorToAnalyzerEnable(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrTxStatMirrorToAnalyzerEnableGet
*
* DESCRIPTION:
*       Get mode of Egress Statistical Mirroring to the Egress Analyzer Port
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*      dev    - packet processor
*
* OUTPUTS:
*      enablePtr - (pointer to)  Egress Statistical Mirroring.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR      - on NULL pointer

*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrTxStatMirrorToAnalyzerEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrTxStatMirrorToAnalyzerEnableGet(dev, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChMirrorToAnalyzerForwardingModeSet
*
* DESCRIPTION:
*       Set Forwarding mode to Analyzer for egress/ingress mirroring.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*      devNum    - device number.
*      mode   - mode of forwarding To Analyzer packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, mode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To change  Forwarding mode, applicaton should
*       disable Rx/Tx mirrorred ports.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorToAnalyzerForwardingModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    mode =  (CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChMirrorToAnalyzerForwardingModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorToAnalyzerForwardingModeGet
*
* DESCRIPTION:
*       Get Forwarding mode to Analyzer for egress/ingress mirroring.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*      devNum    - device number.
*
* OUTPUTS:
*      modePtr   - pointer to mode of forwarding To Analyzer packets.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorToAnalyzerForwardingModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     devNum;
    CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrorToAnalyzerForwardingModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);
    return CMD_OK;

}

/*******************************************************************************
* cpssDxChMirrorAnalyzerInterfaceSet
*
* DESCRIPTION:
*       This function sets analyzer interface.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*      devNum         - device number.
*      index          - index of analyzer interface. (0 - 6)
*      interfacePtr   - Pointer to analyzer interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index, interface type.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_RANGE          - on wrong port or device number in interfacePtr.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorAnalyzerInterfaceSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     devNum;
    GT_U32    index;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   interface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    interface.interface.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[2];
    interface.interface.devPort.hwDevNum = (GT_HW_DEV_NUM)inArgs[3];
    interface.interface.devPort.portNum = (GT_PORT_NUM)inArgs[4];

    /*dev,port conversion*/
    CONVERT_DEV_PORT_DATA_MAC(interface.interface.devPort.hwDevNum ,
                              interface.interface.devPort.portNum);

    /* call cpss api function */
    result = cpssDxChMirrorAnalyzerInterfaceSet(devNum, index, &interface);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerInterfaceGet
*
* DESCRIPTION:
*       This function gets analyzer interface.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*      devNum         - device number.
*      index          - index of analyzer interface. (0 - 6)
*
* OUTPUTS:
*      interfacePtr   - Pointer to analyzer interface.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorAnalyzerInterfaceGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    index;
    CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   interface;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    index =  (GT_U8)inArgs[1];

    cpssOsMemSet(&interface, 0, sizeof(CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC));

    /* call cpss api function */
    result = cpssDxChMirrorAnalyzerInterfaceGet(devNum, index, &interface);

    /* Override Back Device and Port */
    CONVERT_BACK_DEV_PORT_DATA_MAC(interface.interface.devPort.hwDevNum,
                                   interface.interface.devPort.portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d", interface.interface.type,
                 interface.interface.devPort.hwDevNum,
                 interface.interface.devPort.portNum,
                 interface.interface.trunkId, interface.interface.vidx,
                 interface.interface.vlanId, interface.interface.hwDevNum,
                 interface.interface.fabricVidx, interface.interface.index);

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet
*
* DESCRIPTION:
*       This function sets analyzer interface index, used for ingress
*       mirroring from all engines except
*       port-mirroring source-based-forwarding mode.
*       (Port-Based hop-by-hop mode, Policy-Based, VLAN-Based,
*        FDB-Based, Router-Based).
*       If a packet is mirrored by both the port-based ingress mirroring,
*       and one of the other ingress mirroring, the selected analyzer is
*       the one with the higher index.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable mirroring for
*                  Port-Based hop-by-hop mode, Policy-Based,
*                  VLAN-Based, FDB-Based, Router-Based.
*                  - GT_TRUE - enable mirroring.
*                  - GT_FALSE - No mirroring.
*      index     - Analyzer destination interface index. (0 - 6)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     devNum;
    GT_U32    index;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];
    index = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet(devNum, enable, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet
*
* DESCRIPTION:
*       This function gets analyzer interface index, used for ingress mirroring
*       from all engines except port-mirroring source-based-forwarding mode.
*       (Port-Based hop-by-hop mode, Policy-Based, VLAN-Based,
*        FDB-Based, Router-Based).
*       If a packet is mirrored by both the port-based ingress mirroring,
*       and one of the other ingress mirroring, the selected analyzer is
*       the one with the higher index.
*
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      enablePtr    - Pointer to global enable/disable mirroring for
*                     Port-Based hop-by-hop mode, Policy-Based,
*                     VLAN-Based, FDB-Based, Router-Based.
*                     - GT_TRUE - enable mirroring.
*                     - GT_FALSE - No mirroring.
*      indexPtr     - pointer to analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    index = 0;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet(devNum, &enable,
                                                             &index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, index);

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet
*
* DESCRIPTION:
*       This function sets analyzer interface index, used for egress
*       mirroring for Port-Based hop-by-hop mode.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*      devNum    - device number.
*      enable    - global enable/disable mirroring for
*                  Port-Based hop-by-hop mode.
*                  - GT_TRUE - enable mirroring.
*                  - GT_FALSE - No mirroring.
*      index     - Analyzer destination interface index. (0 - 6)
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_OUT_OF_RANGE          - index is out of range.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     devNum;
    GT_U32    index;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];
    index = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet(devNum, enable, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet
*
* DESCRIPTION:
*       This function gets analyzer interface index, used for egress
*       mirroring for Port-Based hop-by-hop mode.
*
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*      devNum         - device number.
*
* OUTPUTS:
*      enablePtr    - Pointer to global enable/disable mirroring for
*                     Port-Based hop-by-hop mode.
*                     - GT_TRUE - enable mirroring.
*                     - GT_FALSE - No mirroring.
*      indexPtr     - pointer to analyzer destination interface index.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    index = 0;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet(devNum, &enable,
                                                             &index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, index);
    return CMD_OK;

}

/*******************************************************************************
* cpssDxChMirrorAnalyzerMirrorOnDropEnableSet
*
* DESCRIPTION:
*       Enable / Disable mirroring of dropped packets.
*
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*      devNum - device number.
*      index  - Analyzer destination interface index. (0 - 6)
*      enable - GT_TRUE  - dropped packets are mirrored to analyzer interface.
*               GT_FALSE - dropped packets are not mirrored to analyzer interface.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorAnalyzerMirrorOnDropEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     devNum;
    GT_U32    index;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChMirrorAnalyzerMirrorOnDropEnableSet(devNum, index, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerMirrorOnDropEnableGet
*
* DESCRIPTION:
*       Get mirroring status of dropped packets.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*      devNum - device number.
*      index  - Analyzer destination interface index. (0 - 6)
*
* OUTPUTS:
*      enablePtr - Pointer to mirroring status of dropped packets.
*                  - GT_TRUE  - dropped packets are mirrored to
*                               analyzer interface.
*                  - GT_FALSE - dropped packets are not mirrored to
*                               analyzer interface.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device, index.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorAnalyzerMirrorOnDropEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    index;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChMirrorAnalyzerMirrorOnDropEnableGet(devNum, index,
                                                         &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;

}

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet
*
* DESCRIPTION:
*       Enable/Disable VLAN tag removal of mirrored traffic.
*       When VLAN tag removal is enabled for a specific analyzer port, all
*       packets that are mirrored to this port are sent without any VLAN tags.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum      - device number
*       portNum     - port number
*       enable      - GT_TRUE  - VLAN tag is removed from mirrored traffic.
*                     GT_FALSE - VLAN tag isn't removed from mirrored traffic.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorAnalyzerVlanTagRemoveEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     devNum;
    GT_PORT_NUM     portNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /*dev,port conversion*/
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet
*
* DESCRIPTION:
*       Get status of enabling/disabling VLAN tag removal of mirrored traffic.
*       When VLAN tag removal is enabled for a specific analyzer port, all
*       packets that are mirrored to this port are sent without any VLAN tags.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       enablePtr  - pointer to status of VLAN tag removal of mirrored traffic.
*                  - GT_TRUE  - VLAN tag is removed from mirrored traffic.
*                    GT_FALSE - VLAN tag isn't removed from mirrored traffic.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or port number.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorAnalyzerVlanTagRemoveEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_PORT_NUM     portNum;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /*dev,port conversion*/
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet(devNum, portNum,
                                                          &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;

}

/*******************************************************************************
* cpssDxChMirrorEnhancedMirroringPriorityModeSet
*
* DESCRIPTION:
*       Setting Enhanced Mirroring Priority mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum  - device number
*       mode    - Enhanced Mirroring Priority selected working mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device or mode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorEnhancedMirroringPriorityModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT   mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    mode =  (CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChMirrorEnhancedMirroringPriorityModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorEnhancedMirroringPriorityModeGet
*
* DESCRIPTION:
*       Getting Enhanced Mirroring Priority mode.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       modePtr    - (pointer to) Enhanced Mirroring Priority selected working mode.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorEnhancedMirroringPriorityModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     devNum;
    CPSS_DXCH_MIRROR_ENHANCED_PRIORITY_MODE_ENT   mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrorEnhancedMirroringPriorityModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);
    return CMD_OK;

}
/*******************************************************************************
* cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet
*
* DESCRIPTION:
*       Set the drop code to be used for Hop-By-Hop mirroring mode, when a packet
*       is duplicated to a target analyzer, and the global Ingress Analyzer Index
*       or Egress Analyzer Index is DISABLED.
*       In this case the packet is hard dropped with this drop code.
*
*       NOTE: not relevant to the device that generates the duplication but only
*       to device that will get 'TO_ANALYZER' DSA tag (in HOP-BY_HOP mode)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       dropCode  - the drop code. (values like 'cpu code')
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device , bad dropCode.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    CPSS_NET_RX_CPU_CODE_ENT   dropCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    dropCode =  (CPSS_NET_RX_CPU_CODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet(devNum, dropCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet
*
* DESCRIPTION:
*       Get the drop code to be used for Hop-By-Hop mirroring mode, when a packet
*       is duplicated to a target analyzer, and the global Ingress Analyzer Index
*       or Egress Analyzer Index is DISABLED.
*       In this case the packet is hard dropped with this drop code.
*
*       NOTE: not relevant to the device that generates the duplication but only
*       to device that will get 'TO_ANALYZER' DSA tag (in HOP-BY_HOP mode)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       dropCodePtr  - (pointer to) the drop code. (values like 'cpu code')
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    CPSS_NET_RX_CPU_CODE_ENT   dropCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet(devNum, &dropCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dropCode);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxPortVlanEnableSet
*
* DESCRIPTION:
*       Enable or disable Tx vlan mirroring from specific physical port.
*       (define if the physical port allow/deny egress vlan mirroring)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       portNum   - physical port number.
*       enable    - enable/disable Tx vlan mirroring from this physical port
*                   GT_TRUE  - Tx vlan mirroring enabled  for this physical port.
*                   GT_FALSE - Tx vlan mirroring disabled for this physical port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device , bad portNum.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxPortVlanEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_BOOL                 enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    portNum =  (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable =  (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChMirrorTxPortVlanEnableSet(devNum, portNum , enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMirrorTxPortVlanEnableGet
*
* DESCRIPTION:
*       Get is the Tx vlan mirroring from specific physical port enabled/disabled.
*       (define if the physical port allow/deny egress vlan mirroring)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - the device number
*       portNum   - physical port number.
*
* OUTPUTS:
*       enablePtr - (pointer to)enable/disable Tx vlan mirroring from this physical port
*                   GT_TRUE  - Tx vlan mirroring enabled  for this physical port.
*                   GT_FALSE - Tx vlan mirroring disabled for this physical port.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong device.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_HW_ERROR              - on writing to HW error.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMirrorTxPortVlanEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_BOOL                 enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum =  (GT_U8)inArgs[0];
    portNum =  (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChMirrorTxPortVlanEnableGet(devNum, portNum ,&enable );

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChMirrorAnalyzerVlanTagEnable",
        &wrCpssDxChMirrorAnalyzerVlanTagEnable,
        3, 0},

    {"cpssDxChMirrorAnalyzerVlanTagEnableGet",
        &wrCpssDxChMirrorAnalyzerVlanTagEnableGet,
        2, 0},

    {"cpssDxChMirrorTxAnalyzerVlanTagConfig",
        &wrCpssDxChMirrorTxAnalyzerVlanTagConfig,
        5, 0},

    {"cpssDxChMirrorTxAnalyzerVlanTagConfigGet",
        &wrCpssDxChMirrorTxAnalyzerVlanTagConfigGet,
        1, 0},

    {"cpssDxChMirrorRxAnalyzerVlanTagConfig",
        &wrCpssDxChMirrorRxAnalyzerVlanTagConfig,
        5, 0},

    {"cpssDxChMirrorRxAnalyzerVlanTagConfigGet",
        &wrCpssDxChMirrorRxAnalyzerVlanTagConfigGet,
        1, 0},

    {"cpssDxChMirrorTxAnalyzerPortSet",
        &wrCpssDxChMirrorTxAnalyzerPortSet,
        3, 0},

    {"cpssDxChMirrorTxAnalyzerPortGet",
        &wrCpssDxChMirrorTxAnalyzerPortGet,
        1, 0},

    {"cpssDxChMirrorRxAnalyzerPortSet",
        &wrCpssDxChMirrorRxAnalyzerPortSet,
        3, 0},

    {"cpssDxChMirrorRxAnalyzerPortGet",
        &wrCpssDxChMirrorRxAnalyzerPortGet,
        1, 0},

    {"cpssDxChMirrorTxCascadeMonitorEnable",
        &wrCpssDxChMirrorTxCascadeMonitorEnable,
        2, 0},

    {"cpssDxChMirrorTxCascadeMonitorEnableGet",
        &wrCpssDxChMirrorTxCascadeMonitorEnableGet,
        1, 0},

    {"cpssDxChMirrorRxPortSet",
        &wrCpssDxChMirrorRxPortSet,
        3, 0},

    {"cpssDxChMirrorRxPortGet",
        &wrCpssDxChMirrorRxPortGet,
        2, 0},

    {"cpssDxChMirrorRxPortExtSet",
        &wrCpssDxChMirrorRxPortExtSet,
        4, 0},

    {"cpssDxChMirrorRxPortExtGet",
        &wrCpssDxChMirrorRxPortExtGet,
        2, 0},

    {"cpssDxChMirrorRxPortSet_1",
        &wrCpssDxChMirrorRxPortSet_1,
        5, 0},

    {"cpssDxChMirrorRxPortGet_1",
        &wrCpssDxChMirrorRxPortGet_1,
        3, 0},

    {"cpssDxChMirrorTxPortSet",
        &wrCpssDxChMirrorTxPortSet,
        3, 0},

    {"cpssDxChMirrorTxPortExtSet",
        &wrCpssDxChMirrorTxPortExtSet,
        4, 0},

    {"cpssDxChMirrorTxPortGet",
        &wrCpssDxChMirrorTxPortGet,
        2, 0},

    {"cpssDxChMirrorTxPortExtGet",
        &wrCpssDxChMirrorTxPortExtGet,
        2, 0},

    {"cpssDxChMirrorTxPortSet_1",
        &wrCpssDxChMirrorTxPortSet_1,
        5, 0},

    {"cpssDxChMirrorTxPortGet_1",
        &wrCpssDxChMirrorTxPortGet_1,
        3, 0},

    {"cpssDxChMirrRxStatMirroringToAnalyzerRatioSet",
        &wrCpssDxChMirrRxStatMirroringToAnalyzerRatioSet,
        2, 0},

    {"cpssDxChMirrRxStatMirroringToAnalyzerRatioGet",
        &wrCpssDxChMirrRxStatMirroringToAnalyzerRatioGet,
        1, 0},

    {"cpssDxChMirrRxStatMirrorToAnalyzerEnable",
        &wrCpssDxChMirrRxStatMirrorToAnalyzerEnable,
        2, 0},

    {"cpssDxChMirrRxStatMirrorToAnalyzerEnableGet",
        &wrCpssDxChMirrRxStatMirrorToAnalyzerEnableGet,
        1, 0},

    {"cpssDxChMirrRxAnalyzerDpTcSet",
        &wrCpssDxChMirrRxAnalyzerDpTcSet,
        3, 0},

    {"cpssDxChMirrRxAnalyzerDpTcGet",
        &wrCpssDxChMirrRxAnalyzerDpTcGet,
        1, 0},

    {"cpssDxChMirrTxAnalyzerDpTcSet",
        &wrCpssDxChMirrTxAnalyzerDpTcSet,
        3, 0},

    {"cpssDxChMirrTxAnalyzerDpTcGet",
        &wrCpssDxChMirrTxAnalyzerDpTcGet,
        1, 0},

    {"cpssDxChMirrTxStatMirroringToAnalyzerRatioSet",
        &wrCpssDxChMirrTxStatMirroringToAnalyzerRatioSet,
        2, 0},

    {"cpssDxChMirrTxStatMirroringToAnalyzerRatioGet",
        &wrCpssDxChMirrTxStatMirroringToAnalyzerRatioGet,
        1, 0},

    {"cpssDxChMirrTxStatMirrorToAnalyzerEnable",
        &wrCpssDxChMirrTxStatMirrorToAnalyzerEnable,
        2, 0},

    {"cpssDxChMirrTxStatMirrorToAnalyzerEnableGet",
        &wrCpssDxChMirrTxStatMirrorToAnalyzerEnableGet,
        1, 0},

    {"cpssDxChMirrorToAnalyzerForwardingModeSet",
        &wrCpssDxChMirrorToAnalyzerForwardingModeSet,
        2, 0},

    {"cpssDxChMirrorToAnalyzerForwardingModeGet",
        &wrCpssDxChMirrorToAnalyzerForwardingModeGet,
        1, 0},

    {"cpssDxChMirrorAnalyzerInterfaceSet",
        &wrCpssDxChMirrorAnalyzerInterfaceSet,
        5, 0},

    {"cpssDxChMirrorAnalyzerInterfaceGet",
        &wrCpssDxChMirrorAnalyzerInterfaceGet,
        2, 0},

    {"cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet",
        &wrCpssDxChMirrorRxGlobalAnalyzerInterfaceIndexSet,
        3, 0},

    {"cpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet",
        &wrCpssDxChMirrorRxGlobalAnalyzerInterfaceIndexGet,
        1, 0},

    {"cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet",
        &wrCpssDxChMirrorTxGlobalAnalyzerInterfaceIndexSet,
        3, 0},

    {"cpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet",
        &wrCpssDxChMirrorTxGlobalAnalyzerInterfaceIndexGet,
        1, 0},

    {"cpssDxChMirrorAnalyzerMirrorOnDropEnableSet",
        &wrCpssDxChMirrorAnalyzerMirrorOnDropEnableSet,
        3, 0},

    {"cpssDxChMirrorAnalyzerMirrorOnDropEnableGet",
        &wrCpssDxChMirrorAnalyzerMirrorOnDropEnableGet,
        2, 0},

    {"cpssDxChMirrorAnalyzerVlanTagRemoveEnableSet",
        &wrCpssDxChMirrorAnalyzerVlanTagRemoveEnableSet,
        3, 0},

    {"cpssDxChMirrorAnalyzerVlanTagRemoveEnableGet",
        &wrCpssDxChMirrorAnalyzerVlanTagRemoveEnableGet,
        2, 0},

    {"cpssDxChMirrorEnhancedMirroringPriorityModeSet",
        &wrCpssDxChMirrorEnhancedMirroringPriorityModeSet,
        2, 0},

    {"cpssDxChMirrorEnhancedMirroringPriorityModeGet",
        &wrCpssDxChMirrorEnhancedMirroringPriorityModeGet,
        1, 0},

    {"cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet",
        &wrCpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeSet,
        2, 0},

    {"cpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet",
        &wrCpssDxChMirrorHopByHopBadAnalyzerIndexDropCodeGet,
        1, 0},

    {"cpssDxChMirrorTxPortVlanEnableSet",
        &wrCpssDxChMirrorTxPortVlanEnableSet,
        3, 0},

    {"cpssDxChMirrorTxPortVlanEnableGet",
        &wrCpssDxChMirrorTxPortVlanEnableGet,
        2, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChBridge
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
GT_STATUS cmdLibInitCpssDxChMirror
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


