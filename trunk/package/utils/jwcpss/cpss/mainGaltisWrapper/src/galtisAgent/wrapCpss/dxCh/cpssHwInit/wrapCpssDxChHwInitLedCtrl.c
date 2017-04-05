/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChHwInitLedCtrl.c
*
* DESCRIPTION:
*
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/cpssHwInit/cpssDxChHwInitLedCtrl.h>

static CMD_STATUS cmdLedStreamPortGroupConfigSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_32 version
);

static CMD_STATUS cmdLedStreamPortGroupConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_32 version
);

static CMD_STATUS cmdLedStreamPortGroupClassManipulationSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_32 version
);

static CMD_STATUS cmdLedStreamPortGroupClassManipulationGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_U32 version
);

/*******************************************************************************
* cpssDxChLedStreamClassAndGroupConfig
*
* DESCRIPTION:
*       This routine configures the control, the class manipulation and the
*       group configuration registers for both led interfaces in Dx.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum - physical device number
*       ledControl      - The user interface control register value.
*       ledClassConfPtr- (pointer to) a list of class manipulation registers
*       ledClassNum     - the class list length
*       ledGroupConfPtr- (pointer to) a list of group configuration registers
*       ledGroupNum     - the group list length
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success,
*       GT_FAIL                  - otherwise.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE - ledClassNum value or ledGroupNum value is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.  See the Led Serial Interface in the device's data-sheet for full
*           detail of the leds configuration, classes and groups.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamClassAndGroupConfig
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ledControl;
    CPSS_DXCH_LED_CLASS_CONF_STC ledClassConf[1];
    CPSS_DXCH_LED_GROUP_CONF_STC ledGroupConf[1];
    
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                 = (GT_U8)inArgs[0];
    ledControl             = (GT_U32)inArgs[1];
    ledClassConf[0].classNum  = (GT_U32)inArgs[2];
    ledClassConf[0].classConf = (GT_U32)inArgs[3];
    ledGroupConf[0].groupNum  = (GT_U32)inArgs[5];
    ledGroupConf[0].groupConf = (GT_U32)inArgs[6];
    
    /* call cpss api function */
    result = cpssDxChLedStreamClassAndGroupConfig(devNum, ledControl, 
                                                  ledClassConf, 1, ledGroupConf, 1);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamHyperGStackTxQStatusEnableSet
*
* DESCRIPTION:
*       Enables the display to HyperGStack Ports Transmit Queue Status via LED
*       Stream.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum   - physical device number
*       enable  - GT_TRUE - enable
*                           The status of the HyperGStack Ports Transmit Queues is
*                           conveyed via the LED stream. Port<i>TxqNotFull indication for each of
*                           those ports is set to 0 when the number of buffers currently allocated in all
*                           of this ports transmit queues exceeds the limit configured for this port.
*                 GT_FALSE - disable
*                           The status of the HyperGStack Ports Transmit Queues is
*                           not conveyed via the LED stream.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamHyperGStackTxQStatusEnableSet
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

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChLedStreamHyperGStackTxQStatusEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamHyperGStackTxQStatusEnableGet
*
* DESCRIPTION:
*       Gets HyperGStack Ports Transmit Queue Status via LED Stream.
*
* APPLICABLE DEVICES:  DxCh3 and above.
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE - enable
*                           The status of the HyperGStack Ports Transmit Queues is
*                           conveyed via the LED stream. Port<i>TxqNotFull indication for each of
*                           those ports is set to 0 when the number of buffers currently allocated in all
*                           of this ports transmit queues exceeds the limit configured for this port.
*                   GT_FALSE - disable
*                           The status of the HyperGStack Ports Transmit Queues is
*                           not conveyed via the LED stream.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamHyperGStackTxQStatusEnableGet
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

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChLedStreamHyperGStackTxQStatusEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupConfigSet
*
* DESCRIPTION:
*       This routine configures the LED stream.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANG           - ledStart or ledEnd out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortGroupConfigSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return cmdLedStreamPortGroupConfigSet(inArgs,inFields,numFields,outArgs,0);
}

/*******************************************************************************
* wrCpssDxChLedStreamPortGroupConfigSet_1
*
* DESCRIPTION:
*       This routine configures the LED stream.
*
* APPLICABLE DEVICES:  Bobcat2, Caelum, Bobcat3
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANG           - ledStart or ledEnd out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortGroupConfigSet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return cmdLedStreamPortGroupConfigSet(inArgs,inFields,numFields,outArgs,1);
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupConfigGet
*
* DESCRIPTION:
*       This routine gets the LED stream configuration.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*
* OUTPUTS:
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on bad hardware value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortGroupConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return cmdLedStreamPortGroupConfigGet(inArgs,inFields,numFields,outArgs, 0);
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupConfigGet_1
*
* DESCRIPTION:
*       This routine gets the LED stream configuration.
*
* APPLICABLE DEVICES:  Bobcat2, Caelum, Bobcat3
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*
* OUTPUTS:
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on bad hardware value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortGroupConfigGet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return cmdLedStreamPortGroupConfigGet(inArgs,inFields,numFields,outArgs, 1);
}

/*******************************************************************************
* cpssDxChLedStreamCpuOrPort27ModeSet
*
* DESCRIPTION:
*       This routine configures the whether LED stream interface 0 contains CPU 
*       or port 27 indications.
*
* APPLICABLE DEVICES:  DxCh3 and DxChXcat
*
* INPUTS:
*       devNum          - physical device number
*       indicatedPort   - the selected port indication: CPU or port 27.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamCpuOrPort27ModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_LED_CPU_OR_PORT27_ENT   indicatedPort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    indicatedPort = (CPSS_DXCH_LED_CPU_OR_PORT27_ENT)inArgs[1];
    
    /* call cpss api function */
    result = cpssDxChLedStreamCpuOrPort27ModeSet(devNum, indicatedPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamCpuOrPort27ModeGet
*
* DESCRIPTION:
*       This routine gets the whether LED stream interface 0 contains CPU 
*       or port 27 indications.
*
* APPLICABLE DEVICES:  DxCh3 and DxChXcat
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       indicatedPortPtr    - (pointer to) the selected port indication: 
*                             CPU or port 27.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamCpuOrPort27ModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_LED_CPU_OR_PORT27_ENT   indicatedPort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
        
    /* call cpss api function */
    result = cpssDxChLedStreamCpuOrPort27ModeGet(devNum, &indicatedPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", indicatedPort);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamClassManipulationSet
*
* DESCRIPTION:
*       This routine configures a LED stream class manipulation.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number. 
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamClassManipulationSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32                            ledInterfaceNum;
    CPSS_DXCH_LED_PORT_TYPE_ENT       portType;       
    GT_U32                            classNum;       
    CPSS_LED_CLASS_MANIPULATION_STC   classParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ledInterfaceNum = (GT_U32)inArgs[1];
    portType = (CPSS_DXCH_LED_PORT_TYPE_ENT)inArgs[2];
    classNum = (GT_U32)inArgs[3];
    classParams.invertEnable = (GT_BOOL)inArgs[4];
    classParams.blinkEnable = (GT_BOOL)inArgs[5];       
    classParams.blinkSelect = (CPSS_LED_BLINK_SELECT_ENT)inArgs[6];
    classParams.forceEnable = (GT_BOOL)inArgs[7];
    classParams.forceData = (GT_U32)inArgs[8];  
    
    /* call cpss api function */
    result = cpssDxChLedStreamClassManipulationSet(devNum, ledInterfaceNum, 
                                            portType, classNum, &classParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamClassManipulationGet
*
* DESCRIPTION:
*       This routine gets a LED stream class manipulation configuration.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number. 
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*
* OUTPUTS:
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamClassManipulationGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32                            ledInterfaceNum;
    CPSS_DXCH_LED_PORT_TYPE_ENT       portType;       
    GT_U32                            classNum;       
    CPSS_LED_CLASS_MANIPULATION_STC   classParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ledInterfaceNum = (GT_U32)inArgs[1];
    portType = (CPSS_DXCH_LED_PORT_TYPE_ENT)inArgs[2];
    classNum = (GT_U32)inArgs[3];
        
    /* call cpss api function */
    result = cpssDxChLedStreamClassManipulationGet(devNum, ledInterfaceNum, 
                                            portType, classNum, &classParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d", classParams.invertEnable,
                                                classParams.blinkEnable, 
                                                classParams.blinkSelect, 
                                                classParams.forceEnable, 
                                                classParams.forceData); 
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChLedStreamGroupConfigSet
*
* DESCRIPTION:
*       This routine configures a LED stream group.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       groupNum        - group number 
*                         For tri-speed ports: range 0..3
*                         For XG ports: range 0..1
*       groupParamsPtr  - (pointer to) group configuration parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - on out of range class number 
*                                  (allowable class range o..15)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamGroupConfigSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32                      ledInterfaceNum;
    CPSS_DXCH_LED_PORT_TYPE_ENT portType;       
    GT_U32                      groupNum;       
    CPSS_LED_GROUP_CONF_STC     groupParams;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ledInterfaceNum = (GT_U32)inArgs[1];
    portType = (CPSS_DXCH_LED_PORT_TYPE_ENT)inArgs[2];
    groupNum = (GT_U32)inArgs[3];
    groupParams.classA = (GT_U32)inArgs[4];
    groupParams.classB = (GT_U32)inArgs[5];
    groupParams.classC = (GT_U32)inArgs[6];
    groupParams.classD = (GT_U32)inArgs[7];
    
    /* call cpss api function */
    result = cpssDxChLedStreamGroupConfigSet(devNum, ledInterfaceNum, 
                                            portType, groupNum, &groupParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamGroupConfigGet
*
* DESCRIPTION:
*       This routine gets a LED stream group configuration.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       groupNum        - group number 
*                         For tri-speed ports: range 0..3
*                         For XG ports: range 0..1
*
* OUTPUTS:
*       groupParamsPtr  - (pointer to) group configuration parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamGroupConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32                      ledInterfaceNum;
    CPSS_DXCH_LED_PORT_TYPE_ENT portType;       
    GT_U32                      groupNum;       
    CPSS_LED_GROUP_CONF_STC     groupParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ledInterfaceNum = (GT_U32)inArgs[1];
    portType = (CPSS_DXCH_LED_PORT_TYPE_ENT)inArgs[2];
    groupNum = (GT_U32)inArgs[3];
        
    /* call cpss api function */
    result = cpssDxChLedStreamGroupConfigSet(devNum, ledInterfaceNum, 
                                             portType, groupNum, &groupParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", groupParams.classA, 
                                              groupParams.classB, 
                                              groupParams.classC, 
                                              groupParams.classD); 
                                                
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamConfigSet
*
* DESCRIPTION:
*       This routine configures the LED stream.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANG           - ledStart or ledEnd out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamConfigSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32              ledInterfaceNum;
    CPSS_LED_CONF_STC   ledConf;     
    

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ledInterfaceNum = (GT_U32)inArgs[1];
    ledConf.ledOrganize = (CPSS_LED_ORDER_MODE_ENT)inArgs[2];;      
    ledConf.disableOnLinkDown = (GT_BOOL)inArgs[3];
    ledConf.blink0DutyCycle = (CPSS_LED_BLINK_DUTY_CYCLE_ENT)inArgs[4];  
    ledConf.blink0Duration = (CPSS_LED_BLINK_DURATION_ENT)inArgs[5];   
    ledConf.blink1DutyCycle = (CPSS_LED_BLINK_DUTY_CYCLE_ENT)inArgs[6];  
    ledConf.blink1Duration = (CPSS_LED_BLINK_DURATION_ENT)inArgs[7];   
    ledConf.pulseStretch = (CPSS_LED_PULSE_STRETCH_ENT)inArgs[8];     
    ledConf.ledStart = (GT_U32)inArgs[9];         
    ledConf.ledEnd = (GT_U32)inArgs[10];           
    ledConf.clkInvert = (GT_BOOL)inArgs[11];        
    ledConf.class5select = (CPSS_LED_CLASS_5_SELECT_ENT)inArgs[12];     
    ledConf.class13select = (CPSS_LED_CLASS_13_SELECT_ENT)inArgs[13];    

    /* call cpss api function */
    result = cpssDxChLedStreamConfigSet(devNum, ledInterfaceNum, &ledConf);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChLedStreamConfigGet
*
* DESCRIPTION:
*       This routine gets the LED stream configuration.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*
* OUTPUTS:
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on bad hardware value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32              ledInterfaceNum;
    CPSS_LED_CONF_STC   ledConf;     

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ledInterfaceNum = (GT_U32)inArgs[1];
        
    /* call cpss api function */
    result = cpssDxChLedStreamConfigGet(devNum, ledInterfaceNum, &ledConf);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d%d%d%d", ledConf.ledOrganize,
                                                       ledConf.disableOnLinkDown,
                                                       ledConf.blink0DutyCycle,
                                                       ledConf.blink0Duration,
                                                       ledConf.blink1DutyCycle,
                                                       ledConf.blink1Duration, 
                                                       ledConf.pulseStretch,
                                                       ledConf.ledStart,
                                                       ledConf.ledEnd,
                                                       ledConf.clkInvert,
                                                       ledConf.class5select,
                                                       ledConf.class13select);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupClassManipulationSet
*
* DESCRIPTION:
*       This routine configures a LED stream class manipulation.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number. 
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortGroupClassManipulationSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return cmdLedStreamPortGroupClassManipulationSet(inArgs,inFields,numFields,outArgs,0);
}

/*******************************************************************************
* wrCpssDxChLedStreamPortGroupClassManipulationSet_1
*
* DESCRIPTION:
*       This routine configures a LED stream class manipulation.
*
* APPLICABLE DEVICES:  Bobcat2, Caelum, Bobcat3
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number. 
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortGroupClassManipulationSet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return cmdLedStreamPortGroupClassManipulationSet(inArgs,inFields,numFields,outArgs,1);
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupClassManipulationGet
*
* DESCRIPTION:
*       This routine gets a LED stream class manipulation configuration.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number. 
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*
* OUTPUTS:
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortGroupClassManipulationGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return cmdLedStreamPortGroupClassManipulationGet(inArgs,inFields,numFields,outArgs,0);
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupClassManipulationGet_1
*
* DESCRIPTION:
*       This routine gets a LED stream class manipulation configuration.
*
* APPLICABLE DEVICES:  Bobcat2, Caelum, Bobcat3
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number. 
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*
* OUTPUTS:
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortGroupClassManipulationGet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return cmdLedStreamPortGroupClassManipulationGet(inArgs,inFields,numFields,outArgs,1);
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupGroupConfigSet
*
* DESCRIPTION:
*       This routine configures a LED stream group.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       groupNum        - group number 
*                         For tri-speed ports: range 0..3
*                         For XG ports: range 0..1
*       groupParamsPtr  - (pointer to) group configuration parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - on out of range class number 
*                                  (allowable class range o..15)
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortGroupGroupConfigSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    GT_U32              ledInterfaceNum;
    CPSS_DXCH_LED_PORT_TYPE_ENT portType;
    GT_U32                      groupNum;
    CPSS_LED_GROUP_CONF_STC     ledGroupParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    ledInterfaceNum = (GT_U32)inArgs[2];
    portType = (CPSS_DXCH_LED_PORT_TYPE_ENT)inArgs[3];
    groupNum = (GT_U32)inArgs[4];
    ledGroupParams.classA = (GT_U32)inArgs[5];
    ledGroupParams.classB = (GT_U32)inArgs[6];
    ledGroupParams.classC = (GT_U32)inArgs[7];
    ledGroupParams.classD = (GT_U32)inArgs[8];
    
    /* call cpss api function */
    result = cpssDxChLedStreamPortGroupGroupConfigSet(devNum, portGroupsBmp, ledInterfaceNum, 
                                                      portType, groupNum, &ledGroupParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupGroupConfigGet
*
* DESCRIPTION:
*       This routine gets a LED stream group configuration.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       groupNum        - group number 
*                         For tri-speed ports: range 0..3
*                         For XG ports: range 0..1
*
* OUTPUTS:
*       groupParamsPtr  - (pointer to) group configuration parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortGroupGroupConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    GT_U32              ledInterfaceNum;
    CPSS_DXCH_LED_PORT_TYPE_ENT portType;
    GT_U32                      groupNum;
    CPSS_LED_GROUP_CONF_STC     ledGroupParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    ledInterfaceNum = (GT_U32)inArgs[2];
    portType = (CPSS_DXCH_LED_PORT_TYPE_ENT)inArgs[3];
    groupNum = (GT_U32)inArgs[4];
        
    /* call cpss api function */
    result = cpssDxChLedStreamPortGroupGroupConfigGet(devNum, portGroupsBmp, ledInterfaceNum, 
                                                      portType, groupNum, &ledGroupParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", 
                 ledGroupParams.classA,
                 ledGroupParams.classB,
                 ledGroupParams.classC,
                 ledGroupParams.classD);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamClassIndicationSet
*
* DESCRIPTION:
*       This routine configures classes 9-11 indication (debug) for flex ports.
*
* APPLICABLE DEVICES:  DxChXcat only.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1.
*       classNum        - class number, range 9..11
*       indication      - port (debug) indication data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamClassIndicationSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ledInterfaceNum;
    GT_U32 classNum;
    CPSS_DXCH_LED_INDICATION_ENT   indication;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ledInterfaceNum = (GT_U32)inArgs[1];
    classNum = (GT_U32)inArgs[2];
    indication = (CPSS_DXCH_LED_INDICATION_ENT)inArgs[3];
    
    /* call cpss api function */
    result = cpssDxChLedStreamClassIndicationSet(devNum, ledInterfaceNum, classNum, indication);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamClassIndicationGet
*
* DESCRIPTION:
*       This routine gets classes 9-11 indication (debug) for flex ports.
*
* APPLICABLE DEVICES:  DxChXcat only.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1.
*       classNum        - class number, range 9..11
*
* OUTPUTS:
*       indicationPtr   - (pointer to) port (debug) indication data
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_STATE             - on bad hardware value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamClassIndicationGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ledInterfaceNum;
    GT_U32 classNum;
    CPSS_DXCH_LED_INDICATION_ENT   indication;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ledInterfaceNum = (GT_U32)inArgs[1];
    classNum = (GT_U32)inArgs[2];
        
    /* call cpss api function */
    result = cpssDxChLedStreamClassIndicationGet(devNum, ledInterfaceNum, classNum, &indication);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", indication);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamDirectModeEnableSet
*
* DESCRIPTION:
*       This routine enables\disables LED Direct working mode.
*
* APPLICABLE DEVICES:  DxChXcat only.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1.
*       enable          - GT_TRUE: direct mode
*                         GT_FALSE: serial mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS: In Direct mode LED pin indicates a different Stack port status.
*           The external pins are mapped as follows:
*           LEDDATA1 — indicates stack port 24
*           LEDSTB1  — indicates stack port 25
*           LEDDATA0 — indicates stack port 26
*           LEDSTB0  — indicates stack port 27
*           The indication for each port is driven from the LED group0 
*           configurations.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamDirectModeEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ledInterfaceNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ledInterfaceNum = (GT_U32)inArgs[1];
    enable = inArgs[2];
        
    /* call cpss api function */
    result = cpssDxChLedStreamDirectModeEnableSet(devNum, ledInterfaceNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamDirectModeEnableGet
*
* DESCRIPTION:
*       This routine gets LED pin indication direct mode enabling status.
*
* APPLICABLE DEVICES:  DxChXcat only.
*
* INPUTS:
*       devNum          - physical device number
*       ledInterfaceNum - LED stream interface number, range 0-1.
*
* OUTPUTS:
*       modePtr         - (pointer to) GT_TRUE: direct mode
*                                      GT_FALSE: serial mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_STATE             - on bad hardware value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS: In Direct mode LED pin indicates a different Stack port status.
*           The external pins are mapped as follows:
*           LEDDATA1 — indicates stack port 24
*           LEDSTB1  — indicates stack port 25
*           LEDDATA0 — indicates stack port 26
*           LEDSTB0  — indicates stack port 27
*           The indication for each port is driven from the LED group0 
*           configurations.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamDirectModeEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ledInterfaceNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ledInterfaceNum = (GT_U32)inArgs[1];
            
    /* call cpss api function */
    result = cpssDxChLedStreamDirectModeEnableGet(devNum, ledInterfaceNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamPortFlexTriSpeedIndicationEnableSet
*
* DESCRIPTION:
*       This routine enables having some LED indications of Giga Ethernet MAC 
*       for a selected stack\flex port.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - flex port number 
*       enable  - GT_TRUE: enable having some Giga Ethernet MAC indications.
*                 GT_FALSE: use the legacy indications.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The Giga Ethernet mac of the stack port is not sending out on the LED
*       stream interface few important indications: Activity, GMII & MII 
*       speeds.
*       The logic which decided which indications to send out was modified for
*       the STACK PORT case: the indications are manipulated to resemble the XG
*       stream - as a result few important indications for Giga Ethernet mode 
*       are not in the stream by default.
*
*       When enabling by this API, the following indication will be replaced:
*       Old indication    						            New Indication
*       ==============                                      ==============
*       p rej    (CPSS_DXCH_LED_INDICATION_P_REJ_E)         Activity 
*       jabber   (CPSS_DXCH_LED_INDICATION_JABBER_E)        GMII speed
*       fragment (CPSS_DXCH_LED_INDICATION_FRAGMENT_E)      MII speed
*
*       Refer to cpssDxChLedStreamClassIndicationSet\Get APIs and the 
*       CPSS_DXCH_LED_INDICATION_ENT structure definition.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortFlexTriSpeedIndicationEnableSet
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
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChLedStreamPortFlexTriSpeedIndicationEnableSet(devNum, 
                                                                  portNum,
                                                                  enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamPortFlexTriSpeedIndicationEnableGet
*
* DESCRIPTION:
*       This routine gets enabling status of having some LED indications of 
*       Giga Ethernet MAC for a selected stack\flex port.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - flex port number 
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE: enable having some Giga Ethernet MAC indications.
*                    GT_FALSE: use the legacy indications.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       See comment for cpssDxChLedStreamPortFlexTriSpeedIndicationEnableSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortFlexTriSpeedIndicationEnableGet
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
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);
            
    /* call cpss api function */
    result = cpssDxChLedStreamPortFlexTriSpeedIndicationEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChLedStreamPortPositionSet
*
* DESCRIPTION:
*       Sets port position in LED stream.
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - port number
*       position        - port position index in LED stream
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - position out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortPositionSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U32                  position;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    position = (GT_U32)inArgs[2];
        
    /* call cpss api function */
    result = cpssDxChLedStreamPortPositionSet(devNum, portNum, position);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;

}


/*******************************************************************************
* cpssDxChLedStreamPortPositionGet
*
* DESCRIPTION:
*       Gets port position in LED stream.
*
* APPLICABLE DEVICES:
*       xCat3; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - port number
*
* OUTPUTS:
*       position        - (pointer to) port position index in LED stream
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - position out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortPositionGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U32                  position;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
        
    /* call cpss api function */
    result = cpssDxChLedStreamPortPositionGet(devNum, portNum, &position);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", position);

    return CMD_OK;

}

/*******************************************************************************
* cpssDxChLedStreamPortClassPolarityInvertEnableSet
*
* DESCRIPTION:
*       This routine set the polarity of the selected indications
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; xCat3; Lion2.
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - physical port number. 
*       classNum        - class number.
*                         (APPLICABLE RANGES: 0..5)
*       invertEnable    - status of the selected polarity indication. 
*                        GT_TRUE  - polarity inverted 
*                        GT_FALSE - polarity not inverted
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*       GT_NOT_INITIALIZED       - port's LED position is not set yet by cpssDxChLedStreamPortPositionSet
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortClassPolarityInvertEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U32      classNum;
    GT_BOOL     invertEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    classNum = (GT_U32)inArgs[2];
    invertEnable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum); 
    /* call cpss api function */
    result = cpssDxChLedStreamPortClassPolarityInvertEnableSet(devNum, portNum, classNum, invertEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamPortClassPolarityInvertEnableGet
*
* DESCRIPTION:
*       This routine set the polarity of the selected indications
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; xCat3; Lion2.
*
* INPUTS:
*       devNum          - physical device number
*       portNum         - physical port number. 
*       classNum        - class number.
*                         (APPLICABLE RANGES: 0..5)
*
*
* OUTPUTS:
*       invertEnablePtr - (pointer to) status of the selected polarity indication. 
*                        GT_TRUE  - polarity inverted 
*                        GT_FALSE - polarity not inverted
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*       GT_NOT_INITIALIZED       - port's LED position is not set yet by cpssDxChLedStreamPortPositionSet
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLedStreamPortClassPolarityInvertEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_PHYSICAL_PORT_NUM    portNum;
    GT_U32      classNum;
    GT_BOOL     invertEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    classNum = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum); 
    /* call cpss api function */
    result = cpssDxChLedStreamPortClassPolarityInvertEnableGet(devNum, portNum, classNum, &invertEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", invertEnable);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChLedStreamClassAndGroupConfig",
         &wrCpssDxChLedStreamClassAndGroupConfig,
         8, 0},
    {"cpssDxChLedStreamHyperGStackTxQStatusEnableSet",
         &wrCpssDxChLedStreamHyperGStackTxQStatusEnableSet,
         2, 0},
    {"cpssDxChLedStreamHyperGStackTxQStatusEnableGet",
         &wrCpssDxChLedStreamHyperGStackTxQStatusEnableGet,
         1, 0},

    {"cpssDxChLedStreamPortGroupConfigSet",
         &wrCpssDxChLedStreamPortGroupConfigSet,
         15, 0},

    {"cpssDxChLedStreamPortGroupConfigGet",
         &wrCpssDxChLedStreamPortGroupConfigGet,
         3, 0},

    {"cpssDxChLedStreamPortGroupConfigSet_1",
         &wrCpssDxChLedStreamPortGroupConfigSet_1,
         17, 0},

    {"cpssDxChLedStreamPortGroupConfigGet_1",
         &wrCpssDxChLedStreamPortGroupConfigGet_1,
         3, 0},


    {"cpssDxChLedStreamCpuOrPort27ModeSet",
         &wrCpssDxChLedStreamCpuOrPort27ModeSet,
         2, 0},

    {"cpssDxChLedStreamCpuOrPort27ModeGet",
         &wrCpssDxChLedStreamCpuOrPort27ModeGet,
         1, 0},

    {"cpssDxChLedStreamClassManipulationSet",
         &wrCpssDxChLedStreamClassManipulationSet,
         9, 0},

    {"cpssDxChLedStreamClassManipulationGet",
         &wrCpssDxChLedStreamClassManipulationGet,
         4, 0},

    {"cpssDxChLedStreamGroupConfigSet",
         &wrCpssDxChLedStreamGroupConfigSet,
         8, 0},

    {"cpssDxChLedStreamGroupConfigGet",
         &wrCpssDxChLedStreamGroupConfigGet,
         4, 0},

    {"cpssDxChLedStreamConfigSet",
         &wrCpssDxChLedStreamConfigSet,
         14, 0},

    {"cpssDxChLedStreamConfigGet",
         &wrCpssDxChLedStreamConfigGet,
         2, 0},

    {"cpssDxChLedStreamPortGroupClassManipulationSet",
         &wrCpssDxChLedStreamPortGroupClassManipulationSet,
         10, 0},

    {"cpssDxChLedStreamPortGroupClassManipulationGet",
         &wrCpssDxChLedStreamPortGroupClassManipulationGet,
         5, 0},

    {"cpssDxChLedStreamPortGroupClassManipulationSet_1",
         &wrCpssDxChLedStreamPortGroupClassManipulationSet_1,
         12, 0},

    {"cpssDxChLedStreamPortGroupClassManipulationGet_1",
         &wrCpssDxChLedStreamPortGroupClassManipulationGet_1,
         5, 0},

    {"cpssDxChLedStreamPortGroupGroupConfigSet",
         &wrCpssDxChLedStreamPortGroupGroupConfigSet,
         9, 0},

    {"cpssDxChLedStreamPortGroupGroupConfigGet",
         &wrCpssDxChLedStreamPortGroupGroupConfigGet,
         5, 0},

    {"cpssDxChLedStreamClassIndicationSet",
         &wrCpssDxChLedStreamClassIndicationSet,
         4, 0},

    {"cpssDxChLedStreamClassIndicationGet",
         &wrCpssDxChLedStreamClassIndicationGet,
         3, 0},

    {"cpssDxChLedStreamDirectModeEnableSet",
         &wrCpssDxChLedStreamDirectModeEnableSet,
         3, 0},

    {"cpssDxChLedStreamDirectModeEnableGet",
         &wrCpssDxChLedStreamDirectModeEnableGet,
         2, 0},

    /* "cpssDxChLedStreamPortFlexTriSpeedIndicationEnableSet" to long for GUI */
    {"cpssDxChLedStreamPortFlexTriSpeedIndicationEnSet",
         &wrCpssDxChLedStreamPortFlexTriSpeedIndicationEnableSet,
         3, 0},

    /* "cpssDxChLedStreamPortFlexTriSpeedIndicationEnableGet" to long for GUI */
    {"cpssDxChLedStreamPortFlexTriSpeedIndicationEnGet",
         &wrCpssDxChLedStreamPortFlexTriSpeedIndicationEnableGet,
         2, 0},

    {"cpssDxChLedStreamPortPositionSet",
        &wrCpssDxChLedStreamPortPositionSet,
        3, 0},

    {"cpssDxChLedStreamPortPositionGet",
        &wrCpssDxChLedStreamPortPositionGet,
        2, 0},

    {"cpssDxChLedStreamPortClassPolarityInvertEnableSet",
        &wrCpssDxChLedStreamPortClassPolarityInvertEnableSet,
        4, 0},

    {"cpssDxChLedStreamPortClassPolarityInvertEnableGet",
        &wrCpssDxChLedStreamPortClassPolarityInvertEnableGet,
        3, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChHwInitLedCtrl
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
GT_STATUS cmdLibInitCpssDxChHwInitLedCtrl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

/*******************************************************************************
* cmdLedStreamPortGroupConfigSet
*
* DESCRIPTION:
*       This routine configures the LED stream.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANG           - ledStart or ledEnd out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS cmdLedStreamPortGroupConfigSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_32 version
)
{
    GT_STATUS              result;
    GT_U8               devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    GT_U32              ledInterfaceNum;
    CPSS_LED_CONF_STC   ledStreamConf;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    ledInterfaceNum = (GT_U32)inArgs[2];
    ledStreamConf.ledOrganize = (CPSS_LED_ORDER_MODE_ENT)inArgs[3];
    ledStreamConf.disableOnLinkDown = (GT_BOOL)inArgs[4];
    ledStreamConf.blink0DutyCycle = (CPSS_LED_BLINK_DUTY_CYCLE_ENT)inArgs[5];
    ledStreamConf.blink0Duration  = (CPSS_LED_BLINK_DURATION_ENT)inArgs[6];
    ledStreamConf.blink1DutyCycle = (CPSS_LED_BLINK_DUTY_CYCLE_ENT)inArgs[7];
    ledStreamConf.blink1Duration  = (CPSS_LED_BLINK_DURATION_ENT)inArgs[8];
    ledStreamConf.pulseStretch  = (CPSS_LED_PULSE_STRETCH_ENT)inArgs[9];
    ledStreamConf.ledStart  = (GT_U32)inArgs[10];
    ledStreamConf.ledEnd    = (GT_U32)inArgs[11];

    ledStreamConf.clkInvert = (GT_BOOL)inArgs[12];
    ledStreamConf.class5select  = (CPSS_LED_CLASS_5_SELECT_ENT)inArgs[13];
    ledStreamConf.class13select = (CPSS_LED_CLASS_13_SELECT_ENT)inArgs[14];

    if (version == 1)
    {
        /* Bobcat2, Caelum, Bobcat3 */
        ledStreamConf.invertEnable = (GT_BOOL)inArgs[15];
        ledStreamConf.ledClockFrequency = (CPSS_LED_CLOCK_OUT_FREQUENCY_ENT)inArgs[16];
    }

    /* call cpss api function */
    result = cpssDxChLedStreamPortGroupConfigSet(devNum, portGroupsBmp, ledInterfaceNum, &ledStreamConf);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChLedStreamPortGroupConfigGet
*
* DESCRIPTION:
*       This routine gets the LED stream configuration.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*
* OUTPUTS:
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on bad hardware value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS cmdLedStreamPortGroupConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_32 version
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    GT_U32              ledInterfaceNum;
    CPSS_LED_CONF_STC   ledStreamConf;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    ledInterfaceNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChLedStreamPortGroupConfigGet(devNum, portGroupsBmp, ledInterfaceNum, &ledStreamConf);

    if (version == 0)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d%d%d%d", 
                     ledStreamConf.ledOrganize,
                     ledStreamConf.disableOnLinkDown,
                     ledStreamConf.blink0DutyCycle,
                     ledStreamConf.blink0Duration,
                     ledStreamConf.blink1DutyCycle,
                     ledStreamConf.blink1Duration,
                     ledStreamConf.pulseStretch,
                     ledStreamConf.ledStart,
                     ledStreamConf.ledEnd,
                     ledStreamConf.clkInvert,
                     ledStreamConf.class5select,
                     ledStreamConf.class13select);
    }
    else
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d", 
                     ledStreamConf.ledOrganize,
                     ledStreamConf.disableOnLinkDown,
                     ledStreamConf.blink0DutyCycle,
                     ledStreamConf.blink0Duration,
                     ledStreamConf.blink1DutyCycle,
                     ledStreamConf.blink1Duration,
                     ledStreamConf.pulseStretch,
                     ledStreamConf.ledStart,
                     ledStreamConf.ledEnd,
                     ledStreamConf.clkInvert,
                     ledStreamConf.class5select,
                     ledStreamConf.class13select,
                     ledStreamConf.invertEnable,
                     ledStreamConf.ledClockFrequency);
    }

    return CMD_OK;
}

/*******************************************************************************
* cmdLedStreamPortGroupClassManipulationSet
*
* DESCRIPTION:
*       This routine configures a LED stream class manipulation.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number. 
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS cmdLedStreamPortGroupClassManipulationSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_32 version
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    GT_U32              ledInterfaceNum;
    CPSS_DXCH_LED_PORT_TYPE_ENT     portType;
    GT_U32                          classNum;
    CPSS_LED_CLASS_MANIPULATION_STC ledClassParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    ledInterfaceNum = (GT_U32)inArgs[2];
    portType = (CPSS_DXCH_LED_PORT_TYPE_ENT)inArgs[3];
    classNum = (GT_U32)inArgs[4];
    ledClassParams.invertEnable = (GT_BOOL)inArgs[5];
    ledClassParams.blinkEnable  = (GT_BOOL)inArgs[6];
    ledClassParams.blinkSelect  = inArgs[7];
    ledClassParams.forceEnable  = (GT_BOOL)inArgs[8];
    ledClassParams.forceData    = (GT_U32)inArgs[9];
    if (version == 1)
    {
        ledClassParams.pulseStretchEnable = (GT_BOOL)inArgs[10];
        ledClassParams.disableOnLinkDown = (GT_BOOL)inArgs[11];
    }
    /* call cpss api function */
    result = cpssDxChLedStreamPortGroupClassManipulationSet(devNum, portGroupsBmp, ledInterfaceNum, 
                                                            portType, classNum, &ledClassParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cmdLedStreamPortGroupClassManipulationGet
*
* DESCRIPTION:
*       This routine gets a LED stream class manipulation configuration.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum          - physical device number
*       portGroupsBmp   - bitmap of Port Groups.
*                         NOTEs:
*                         1. for non multi-port groups device this parameter is IGNORED.
*                         2. for multi-port groups device :
*                            bitmap must be set with at least one bit representing
*                            valid port group(s). If a bit of non valid port group
*                            is set then function returns GT_BAD_PARAM.
*                            value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                            - read only from first active port group of the bitmap.
*       ledInterfaceNum - LED stream interface number, range 0-1, for Lion: 0 only.
*       portType        - tri-speed or XG port type.
*       classNum        - class number. 
*                         For tri-speed ports: range 0..6
*                         For XG ports: range 0..11
*
* OUTPUTS:
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS cmdLedStreamPortGroupClassManipulationGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_U32 version
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PORT_GROUPS_BMP  portGroupsBmp;
    GT_U32              ledInterfaceNum;
    CPSS_DXCH_LED_PORT_TYPE_ENT     portType;
    GT_U32                          classNum;
    CPSS_LED_CLASS_MANIPULATION_STC ledClassParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    ledInterfaceNum = (GT_U32)inArgs[2];
    portType = (CPSS_DXCH_LED_PORT_TYPE_ENT)inArgs[3];
    classNum = (GT_U32)inArgs[4];
        
    /* call cpss api function */
    result = cpssDxChLedStreamPortGroupClassManipulationGet(devNum, portGroupsBmp, ledInterfaceNum, 
                                                            portType, classNum, &ledClassParams);

    if (version == 0)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%d%d%d%d", 
                     ledClassParams.invertEnable,
                     ledClassParams.blinkEnable,
                     ledClassParams.blinkSelect,
                     ledClassParams.forceEnable,
                     ledClassParams.forceData);
    }
    else
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d%d%d%d%d%d%d", 
                     ledClassParams.invertEnable,
                     ledClassParams.blinkEnable,
                     ledClassParams.blinkSelect,
                     ledClassParams.forceEnable,
                     ledClassParams.forceData,
                     ledClassParams.pulseStretchEnable,
                     ledClassParams.disableOnLinkDown);
    }

    return CMD_OK;
}

