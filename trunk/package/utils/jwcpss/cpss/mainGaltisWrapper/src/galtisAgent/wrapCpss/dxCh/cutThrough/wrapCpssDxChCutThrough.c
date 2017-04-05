/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChCutThrough.c
*
* DESCRIPTION:
*       Wrapper functions for Cut Through cpss.dxCh functions
*
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 9 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/cutThrough/cpssDxChCutThrough.h>

/*******************************************************************************
* cpssDxChCutThroughPortEnableSet
*
* DESCRIPTION:
*      Enable/Disable Cut Through forwarding for packets received on the port.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number including CPU port.
*       enable   - GT_TRUE:  Enable the port for Cut Through.
*                  GT_FALSE: Disable the port for Cut Through.
*       untaggedEnable -  GT_TRUE:  Enable Cut Through forwarding for
*                                   untagged packets received on the port.      
*                         GT_FALSE: Disable Cut Through forwarding for 
*                                    untagged packets received on the port.
*                         Used only if enable == GT_TRUE.
*                         Untagged packets for Cut Through purposes - packets
*                         that don't have VLAN tag or its ethertype isn't equal 
*                         to one of two configurable VLAN ethertypes.
*                         See cpssDxChCutThroughVlanEthertypeSet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       Cut Through forwarding for tagged packets is enabled
*       per source port and UP. See cpssDxChCutThroughUpEnableSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughPortEnableSet
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
    GT_BOOL   untaggedEnable; 

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];
    untaggedEnable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCutThroughPortEnableSet(devNum, portNum, enable, untaggedEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughPortEnableGet
*
* DESCRIPTION:
*      Get Cut Through forwarding mode on the specified port.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number including CPU port.
*
* OUTPUTS:
*       enablePtr   - pointer to Cut Through forwarding mode status:
*                    - GT_TRUE:  Enable the port for Cut Through.
*                      GT_FALSE: Disable the port for Cut Through.
*       untaggedEnablePtr - pointer to Cut Through forwarding mode status
*                            for untagged packets.
*                            Used only if *enablePtr == GT_TRUE.
*                            GT_TRUE:  Enable Cut Through forwarding for
*                                      untagged packets received on the port.      
*                            GT_FALSE: Disable Cut Through forwarding for 
*                                      untagged packets received on the port. 
*                         Untagged packets for Cut Through purposes - packets
*                         that don't have VLAN tag or its ethertype isn't equal 
*                         to one of two configurable VLAN ethertypes.
*                         See cpssDxChCutThroughVlanEthertypeSet.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughPortEnableGet
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
    GT_BOOL untaggedEnable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCutThroughPortEnableGet(devNum, portNum,
                                             &enable, &untaggedEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, untaggedEnable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughUpEnableSet
*
* DESCRIPTION:
*       Enable / Disable tagged packets, with the specified UP 
*       to be Cut Through.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum          - device number.
*       up              - user priority of a VLAN or DSA tagged
*                         packet [0..7].
*       enable   - GT_TRUE:  tagged packets, with the specified UP
*                            may be subject to Cut Through forwarding.
*                  GT_FALSE: tagged packets, with the specified UP
*                            aren't subject to Cut Through forwarding. 
*                   
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or up.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The ingress port should be enabled for Cut Through forwarding.
*       Use cpssDxChCutThroughPortEnableSet for it.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughUpEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8     up;    
    GT_BOOL   enable; 

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    up = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCutThroughUpEnableSet(devNum, up, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughUpEnableGet
*
* DESCRIPTION:
*      Get Cut Through forwarding mode for tagged packets, 
*      with the specified UP.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum      - device number
*       up              - user priority of a VLAN or DSA tagged
*                         packet [0..7].
*
* OUTPUTS:
*       enablePtr   - pointer to Cut Through forwarding mode status 
*                     for tagged packets, with the specified UP.:
*                     - GT_TRUE:  tagged packets, with the specified UP
*                                 may be subject to Cut Through forwarding.
*                       GT_FALSE: tagged packets, with the specified UP
*                                 aren't subject to Cut Through forwarding. 
*                  
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or up
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughUpEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8     up;    
    GT_BOOL   enable; 


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    up = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCutThroughUpEnableGet(devNum, up, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughVlanEthertypeSet
*
* DESCRIPTION:
*       Set VLAN Ethertype in order to identify tagged packets.
*       A packed is identified as VLAN tagged for cut-through purposes.
*       Packet considered as tagged if packet's Ethertype equals to one of two 
*       configurable VLAN Ethertypes.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum           - device number.
*       etherType0       - VLAN EtherType0, values 0..0xFFFF
*       etherType1       - VLAN EtherType1, values 0..0xFFFF
*                   
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong etherType0 or etherType1  
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughVlanEthertypeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    etherType0;
    GT_U32    etherType1;  

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    etherType0 = (GT_U32)inArgs[1];
    etherType1 = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCutThroughVlanEthertypeSet(devNum, etherType0, etherType1);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughVlanEthertypeGet
*
* DESCRIPTION:
*       Get VLAN Ethertype in order to identify tagged packets.
*       A packed is identified as VLAN tagged for cut-through purposes.
*       Packet considered as tagged if packet's Ethertype equals to one of two 
*       configurable VLAN Ethertypes.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum           - device number.
*                   
* OUTPUTS:
*       etherType0Ptr    - Pointer to VLAN EtherType0
*       etherType1Ptr    - Pointer to VLAN EtherType1. 
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughVlanEthertypeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    etherType0;
    GT_U32    etherType1;  


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCutThroughVlanEthertypeGet(devNum, &etherType0, &etherType1);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", etherType0, etherType1);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughMinimalPacketSizeSet
*
* DESCRIPTION:
*       Set minimal packet size that is enabled for Cut Through.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum      - device number.
*       size        - minimal packet size in bytes for Cut Through [129..2047].
*                     Granularity - 8 bytes.
*                   
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong size
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When using cut-through to 1G port, 
*       the minimal packet size should be 512 bytes.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughMinimalPacketSizeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    size;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    size = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCutThroughMinimalPacketSizeSet(devNum, size);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughMinimalPacketSizeGet
*
* DESCRIPTION:
*       Get minimal packet size that is enabled for Cut Through.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum      - device number.
*                   
* OUTPUTS:
*       sizePtr     - pointer to minimal packet size in bytes for Cut Through.
*                     Granularity - 8 bytes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum 
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughMinimalPacketSizeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    size;
    
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    
    /* call cpss api function */
    result = cpssDxChCutThroughMinimalPacketSizeGet(devNum, &size);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", size);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughMemoryRateLimitSet
*
* DESCRIPTION:
*       Set rate limiting of read operations from the memory
*       per target port in Cut Through mode according to the port speed.
*       To prevent congestion in egress pipe, buffer memory read operations 
*       are rate limited according to the target port speed. 
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number including CPU port.
*       enable       - GT_TRUE - rate limiting is enabled.
*                    - GT_FALSE - rate limiting is disabled.
*       portSpeed    - port speed.
*                   
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum, portNum or portSpeed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Rate limit is recommended be enabled 
*       on all egress ports for cut-through traffic.
*       Rate limit to the CPU port should be configured as a 1G port. 
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughMemoryRateLimitSet
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
    CPSS_PORT_SPEED_ENT  portSpeed; 

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];
    portSpeed = (CPSS_PORT_SPEED_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCutThroughMemoryRateLimitSet(devNum, portNum, enable, portSpeed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughMemoryRateLimitGet
*
* DESCRIPTION:
*       Get rate limiting of read operations from the memory
*       per target port in Cut Through mode.
*       To prevent congestion in egress pipe, buffer memory read operations 
*       are rate limited according to the target port speed. 
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum        - device number.
*       portNum       - physical port number including CPU port.
*                     
* OUTPUTS:            
*       enablePtr     - pointer to rate limiting mode status.    
*                        - GT_TRUE - rate limiting is enabled.
*                        - GT_FALSE - rate limiting is disabled.
*       portSpeedPtr  - pointer to port speed.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum or portNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughMemoryRateLimitGet
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
    CPSS_PORT_SPEED_ENT  portSpeed; 


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCutThroughMemoryRateLimitGet(devNum, portNum,
                                             &enable, &portSpeed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, portSpeed);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughBypassRouterAndPolicerEnableSet
*
* DESCRIPTION:
*       Enable / Disable bypassing the Router and Ingress Policer engines.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum       - device number.
*       enable       - GT_TRUE -  bypassing of Router and Ingress Policer
*                                 engines is enabled.
*                    - GT_FALSE - bypassing of Router and Ingress Policer
*                                 engines is disabled.                                     
*                   
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughBypassRouterAndPolicerEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;        
    GT_BOOL   enable;        

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCutThroughBypassRouterAndPolicerEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughBypassRouterAndPolicerEnableGet
*
* DESCRIPTION:
*       Get mode of bypassing the Router and Ingress Policer engines.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum        - device number.
*                     
* OUTPUTS:            
*       enablePtr     - pointer to bypassing the Router and Ingress
*                       Policer engines status.    
*                      - GT_TRUE  -  bypassing of Router and Ingress Policer
*                                    engines is enabled.
*                      - GT_FALSE -  bypassing of Router and Ingress Policer
*                                  engines is disabled.                                     
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum 
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughBypassRouterAndPolicerEnableGet
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
    result = cpssDxChCutThroughBypassRouterAndPolicerEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughBypassModeSet 
*
* DESCRIPTION:
*       Set Bypass mode.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2, Lion.
*
* INPUTS:
*       devNum   - device number.
*       bypassModePtr - pointer to Bypass mode.
*                   
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong Bypass Ingress Pcl mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughBypassModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;        
    CPSS_DXCH_CUT_THROUGH_BYPASS_STC  bypassMode;        

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    bypassMode.bypassRouter = (GT_BOOL)inArgs[1];                                          
    bypassMode.bypassIngressPolicerStage0 = (GT_BOOL)inArgs[2];                          
    bypassMode.bypassIngressPolicerStage1 = (GT_BOOL)inArgs[3];                          
    bypassMode.bypassEgressPolicer = (GT_BOOL)inArgs[4];                                 
    bypassMode.bypassEgressPcl = (GT_BOOL)inArgs[5];                                     
    bypassMode.bypassIngressPcl = (CPSS_DXCH_CUT_THROUGH_BYPASS_INGRESS_PCL_ENT)inArgs[6];

    /* call cpss api function */
    result = cpssDxChCutThroughBypassModeSet(devNum, &bypassMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughBypassModeGet 
*
* DESCRIPTION:
*       Get Bypass mode.
*
* APPLICABLE DEVICES:
*        Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; xCat2; Lion.
*
* INPUTS:
*       devNum   - device number.
*                   
* OUTPUTS:
*       bypassModePtr - pointer to Bypass mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughBypassModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;        
    CPSS_DXCH_CUT_THROUGH_BYPASS_STC  bypassMode;        


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCutThroughBypassModeGet(devNum, &bypassMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d", bypassMode.bypassRouter,
                                        bypassMode.bypassIngressPolicerStage0, 
                                        bypassMode.bypassIngressPolicerStage1, 
                                        bypassMode.bypassEgressPolicer,
                                        bypassMode.bypassEgressPcl,
                                        bypassMode.bypassIngressPcl);


    return CMD_OK;

}

/*******************************************************************************
* cpssDxChCutThroughPortGroupMaxBuffersLimitSet
*
* DESCRIPTION:
*        Set max buffers limit for Cut-Through packets.
*        Packets are handled as non-Cut-Through when number of allocated buffers more then limit.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion3.
*
* INPUTS:
*       devNum                    - device number.
*       portGroupsBmp             - bitmap of Port Groups.
*                                   NOTEs:
*                                    1. for non multi-port groups device this parameter is IGNORED.
*                                    2. for multi-port groups device :
*                                       bitmap must be set with at least one bit representing
*                                       valid port group(s). If a bit of non valid port group
*                                       is set then function returns GT_BAD_PARAM.
*                                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       buffersLimitEnable        - Buffers limit enable.
*                                     - GT_TRUE  - enable.
*                                     - GT_FALSE - disable.
*       buffersLimit              - Max buffers limit for Cut-Trough traffic.
*                                     The resolution is one buffer.
*                                     (APPLICABLE RANGES - 0..0x3FFF)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_OUT_OF_RANGE          - on wrong maxSharedBuffersLimit
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B1
*       buffersLimit = (buffers number per port group) – (guard band).
*       where guard band is (MTU) * (number of cut-through ports in port group)
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughPortGroupMaxBuffersLimitSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS          result;
    GT_U8              devNum;
    GT_PORT_GROUPS_BMP portGroupsBmp;
    GT_BOOL            buffersLimitEnable;
    GT_U32             buffersLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum             = (GT_U8)inArgs[0];
    portGroupsBmp      = (GT_PORT_GROUPS_BMP)inArgs[1];
    buffersLimitEnable = (GT_BOOL)inArgs[2];
    buffersLimit       = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChCutThroughPortGroupMaxBuffersLimitSet(
        devNum, portGroupsBmp, buffersLimitEnable, buffersLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCutThroughPortGroupMaxBuffersLimitGet
*
* DESCRIPTION:
*        Get max buffers limit for Cut-Through packets.
*        Packets are handled as non-Cut-Through when number of allocated buffers more then limit.
*
* APPLICABLE DEVICES:
*        Lion2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion3.
*
* INPUTS:
*       devNum                    - device number.
*       portGroupsBmp             - bitmap of Port Groups.
*                                   NOTEs:
*                                    1. for non multi-port groups device this parameter is IGNORED.
*                                    2. for multi-port groups device :
*                                       bitmap must be set with at least one bit representing
*                                       valid port group(s). If a bit of non valid port group
*                                       is set then function returns GT_BAD_PARAM.
*                                       value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*
* OUTPUTS:
*       buffersLimitEnablePtr        - (pointer to)Buffers limit enable.
*                                        - GT_TRUE  - enable.
*                                        - GT_FALSE - disable.
*       buffersLimitPtr              - (pointer to)Max buffers limit for Cut-Trough traffic.
*                                        The resolution is one buffer.
*                                        (APPLICABLE RANGES - 0..0x3FFF)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on Null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Lion2 device: applicable starting from revision B1
*       buffersLimit = (buffers number per port group) – (guard band).
*       where guard band is (MTU) * (number of cut-through ports in port group)
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCutThroughPortGroupMaxBuffersLimitGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS          result;
    GT_U8              devNum;
    GT_PORT_GROUPS_BMP portGroupsBmp;
    GT_BOOL            buffersLimitEnable;
    GT_U32             buffersLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum             = (GT_U8)inArgs[0];
    portGroupsBmp      = (GT_PORT_GROUPS_BMP)inArgs[1];
    buffersLimitEnable = 0;
    buffersLimit       = 0;

    /* call cpss api function */
    result = cpssDxChCutThroughPortGroupMaxBuffersLimitGet(
        devNum, portGroupsBmp, &buffersLimitEnable, &buffersLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", buffersLimitEnable, buffersLimit);

    return CMD_OK;
}

/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssDxChCutThroughPortEnableSet",
         &wrCpssDxChCutThroughPortEnableSet,
         4, 0},
        {"cpssDxChCutThroughPortEnableGet",
         &wrCpssDxChCutThroughPortEnableGet,
         2, 0},
        {"cpssDxChCutThroughUpEnableSet",
         &wrCpssDxChCutThroughUpEnableSet,
         3, 0},
        {"cpssDxChCutThroughUpEnableGet",
         &wrCpssDxChCutThroughUpEnableGet,
         2, 0},
        {"cpssDxChCutThroughVlanEthertypeSet",
         &wrCpssDxChCutThroughVlanEthertypeSet,
         3, 0},
        {"cpssDxChCutThroughVlanEthertypeGet",
         &wrCpssDxChCutThroughVlanEthertypeGet,
         1, 0},
        {"cpssDxChCutThroughMinimalPacketSizeSet",
         &wrCpssDxChCutThroughMinimalPacketSizeSet,
         2, 0},
        {"cpssDxChCutThroughMinimalPacketSizeGet",
         &wrCpssDxChCutThroughMinimalPacketSizeGet,
         1, 0},
        {"cpssDxChCutThroughMemoryRateLimitSet",
         &wrCpssDxChCutThroughMemoryRateLimitSet,
         4, 0},
        {"cpssDxChCutThroughMemoryRateLimitGet",
         &wrCpssDxChCutThroughMemoryRateLimitGet,
         2, 0},
        {"cpssDxChCutThroughBypassRouterAndPolicerEnableSet",
         &wrCpssDxChCutThroughBypassRouterAndPolicerEnableSet,
         2, 0},
        {"cpssDxChCutThroughBypassRouterAndPolicerEnableGet",
         &wrCpssDxChCutThroughBypassRouterAndPolicerEnableGet,
         1, 0},
        {"cpssDxChCutThroughBypassModeSet",
         &wrCpssDxChCutThroughBypassModeSet,
         7, 0},
        {"cpssDxChCutThroughBypassModeGet",
         &wrCpssDxChCutThroughBypassModeGet,
         1, 0},
        {"cpssDxChCutThroughPortGroupMaxBuffersLimitSet",
         &wrCpssDxChCutThroughPortGroupMaxBuffersLimitSet,
         4, 0},
        {"cpssDxChCutThroughPortGroupMaxBuffersLimitGet",
         &wrCpssDxChCutThroughPortGroupMaxBuffersLimitGet,
         2, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChCutThrough
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
GT_STATUS cmdLibInitCpssDxChCutThrough
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

