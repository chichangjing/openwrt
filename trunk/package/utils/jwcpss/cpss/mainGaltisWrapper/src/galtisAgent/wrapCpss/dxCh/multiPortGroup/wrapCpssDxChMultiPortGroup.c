/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChMultiPortGroup.c
*
* DESCRIPTION:
*       wrappers for cpssDxChMultiPortGroup.c
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>

#include <cpss/dxCh/dxChxGen/multiPortGroup/cpssDxChMultiPortGroup.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>

/**** database initialization **************************************/

/*******************************************************************************
* cpssDxChMultiPortGroupFdbModeGet
*
* DESCRIPTION:
*       Function return the FDB mode for the multi-port group.
*       (no HW operations , return value from 'DB')
*
* APPLICABLE DEVICES:  Lion and above
*                      --ONLY for multi-Port Group device (and more then single port group)
*
* INPUTS:
*       devNum      - device number
* OUTPUTS:
*       fdbModePtr  - (pointer to) FDB mode.
*
* RETURNS:
*    GT_OK        - on success
*    GT_BAD_PARAM - on wrong devNum or fdbMode
*    GT_BAD_PTR   - on NULL pointer
*    GT_BAD_STATE - the DB with unknown value
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupFdbModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;
    CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT fdbMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChMultiPortGroupFdbModeGet(devNum, &fdbMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d" , fdbMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMultiPortGroupFdbModeSet
*
* DESCRIPTION:
*       Function sets multi port groups FDB modes
*       and affects the way FDB low-level APIs are handling the various operations.
*       (no HW operations , set value to 'DB')
*
* APPLICABLE DEVICES:  Lion and above
*                      --ONLY for multi-Port Group device (and more then single port group)
*
* INPUTS:
*       devNum      - device number
*       fdbMode     - FDB mode.
* OUTPUTS:
*       none
*
* RETURNS:
*    GT_OK        - on success
*    GT_BAD_PARAM - on wrong devNum or fdbMode
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupFdbModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;
    CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT fdbMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    fdbMode = (CPSS_DXCH_MULTI_PORT_GROUP_FDB_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChMultiPortGroupFdbModeSet(devNum, fdbMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMultiPortGroupLookupNotFoundTableGet
*
* DESCRIPTION:
*      function get the multi port groups FDB lookup not found info per source port groupId
*
* APPLICABLE DEVICES:  Lion and above
*                      --ONLY for multi-Port Group device (and more then single port group)
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                              - read only from first active port group of the bitmap.
*       sourcePortGroupId - the source Port Group Id (0..3), that started the lookups in
*                   the ring for the handled packet
*
* OUTPUTS:
*       infoPtr      - (pointer to) the setting for the sourcePortGroupId for lookup not found.
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or sourcePortGroupId
*    GT_BAD_PTR   - one of the parameters is NULL pointer
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupLookupNotFoundTableGet
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
    GT_U32  sourcePortGroupId;
    CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC info;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    sourcePortGroupId = (GT_U32)inArgs[2];

    /* convert device,port group bmp (if needed) */
    CONVERT_DEV_PORT_GROUPS_BMP_MAC(devNum, portGroupsBmp);

    /* call cpss api function */
    result = cpssDxChMultiPortGroupLookupNotFoundTableGet(devNum,
                portGroupsBmp,sourcePortGroupId ,
                &info);

    if(info.nextRingInterface.type == CPSS_INTERFACE_PORT_E)
    {
        info.nextRingInterface.trunkId = 0;

        /* remove casting and fix prot and function CONVERT_BACK_DEV_PORT_DATA_MAC*/
        CPSS_TBD_BOOKMARK_EARCH

        /* convert device,port (if needed) */
        CONVERT_BACK_DEV_PORT_DATA_MAC(info.nextRingInterface.devPort.hwDevNum,
                                  info.nextRingInterface.devPort.portNum)
    }
    else /* supposed to be 'trunk' */
    {
        info.nextRingInterface.devPort.hwDevNum = 0;
        info.nextRingInterface.devPort.portNum = 0;

        /* convert trunkId (if needed) */
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(info.nextRingInterface.trunkId);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d"
                        ,info.unknownDaCommand
                        ,info.nextRingInterface.type
                        ,info.nextRingInterface.devPort.hwDevNum
                        ,info.nextRingInterface.devPort.portNum
                        ,info.nextRingInterface.trunkId
                        );

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMultiPortGroupLookupNotFoundTableSet
*
* DESCRIPTION:
*      function set the multi port groups lookup not found info per source port groupId
*      The Table has 4 entries (representing each of the possible
*      4 ingress source portGroups) in each portGroup. Each entry is configured with either
*      REDIECT or NO_REDIRECT command. If entry is set with REDIRECT command,
*      application has to supply ring port interface information which can be
*      either port or Trunk-ID at the next portGroup
*
*      This configuration should be done similarly in cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet
*      enable = GT_FALSE in API of cpssDxChMultiBridgeUnknownDaMaskEnableSet
*      is like unknownDaCommand = CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_NO_REDIRECT_E in current API
*      and
*      enable=GT_TRUE is like unknownDaCommand = CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_REDIRECT_TO_RING_E
*
* APPLICABLE DEVICES:  Lion and above
*                      --ONLY for multi-Port Group device (and more then single port group)
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       sourcePortGroupId - the source Port Group Id (0..3), that started the lookups in
*                   the ring for the handled packet
*       infoPtr      - (pointer to) the setting for the sourcePortGroupId for lookup not found.
*
* OUTPUTS:
*       None
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or sourcePortGroupId or parameter in infoPtr
*    GT_OUT_OF_RANGE - on 'next interface' port/trunk values out of range
*    GT_BAD_PTR   - one of the parameters is NULL pointer
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupLookupNotFoundTableSet
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
    GT_U32  sourcePortGroupId;
    CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_STC info;
    GT_U32  ii = 0;
    GT_PORT_NUM tmpPort;
    GT_U8    tmpDev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[ii++];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[ii++];
    sourcePortGroupId = (GT_U32)inArgs[ii++];
    info.unknownDaCommand = (CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_ENT)inArgs[ii++];
    info.nextRingInterface.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[ii++];
    info.nextRingInterface.devPort.hwDevNum  = (GT_HW_DEV_NUM)inArgs[ii++];
    info.nextRingInterface.devPort.portNum = (GT_PORT_NUM)inArgs[ii++];
    info.nextRingInterface.trunkId = (GT_TRUNK_ID)inArgs[ii++];

    if(info.nextRingInterface.type == CPSS_INTERFACE_PORT_E)
    {
        /* remove casting and fix prot and function CONVERT_DEV_PORT_MAC*/
        CPSS_TBD_BOOKMARK_EARCH

        tmpPort = (GT_PORT_NUM)info.nextRingInterface.devPort.portNum;
        tmpDev = (GT_U8)info.nextRingInterface.devPort.hwDevNum;
        /* convert device,port (if needed) */
        CONVERT_DEV_PORT_U32_MAC(tmpDev,tmpPort)
        info.nextRingInterface.devPort.portNum = tmpPort;
        info.nextRingInterface.devPort.hwDevNum = tmpDev;

    }
    else /* supposed to be 'trunk' */
    {
        /* convert trunkId (if needed) */
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(info.nextRingInterface.trunkId);
    }

    /* convert device,port group bmp (if needed) */
    CONVERT_DEV_PORT_GROUPS_BMP_MAC(devNum, portGroupsBmp);

    /* call cpss api function */
    result = cpssDxChMultiPortGroupLookupNotFoundTableSet(devNum,
                portGroupsBmp,sourcePortGroupId ,
                &info);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChMultiPortGroupPortLookupEnableGet
*
* DESCRIPTION:
*      function gets whether an incoming packet from the port is
*       subject to multi-Port Group FDB lookup (enable/disable)
*
* APPLICABLE DEVICES:  Lion and above
*                      --ONLY for multi-Port Group device (and more then single port group)
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number (also CPU port)
* OUTPUTS:
*       enablePtr   - (pointer to) enable :
*                     GT_TRUE -  an incoming packet from the port is
*                               subject to multi-Port Group FDB lookup
*                     GT_FALSE - an incoming packet from the port is NOT
*                               subject to multi-Port Group FDB lookup
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or portNum
*    GT_BAD_PTR   - one of the parameters is NULL pointer
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupPortLookupEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;
    GT_PHYSICAL_PORT_NUM   portNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* convert device,port (if needed) */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum,portNum);

    /* call cpss api function */
    result = cpssDxChMultiPortGroupPortLookupEnableGet(devNum,
                portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d" ,enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMultiPortGroupPortLookupEnableSet
*
* DESCRIPTION:
*      function enable/disable whether an incoming packet from the port is
*       subject to multi-Port Group lookup
*
* APPLICABLE DEVICES:  Lion and above
*                      --ONLY for multi-Port Group device (and more then single port group)
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number (also CPU port)
*       enable      - GT_TRUE -  an incoming packet from the port is
*                               subject to multi-Port Group lookup
*                     GT_FALSE - an incoming packet from the port is NOT
*                               subject to multi-Port Group lookup
* OUTPUTS:
*       None
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or portNum
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupPortLookupEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;
    GT_PHYSICAL_PORT_NUM   portNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable =  (GT_BOOL)inArgs[2];

    /* convert device,port (if needed) */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum,portNum);

    /* call cpss api function */
    result = cpssDxChMultiPortGroupPortLookupEnableSet(devNum,
                portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "" );

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChMultiPortGroupPortRingEnableGet
*
* DESCRIPTION:
*      function gets whether a port is 'Ring port' (enable/disable)
*
* APPLICABLE DEVICES:  Lion and above
*                      --ONLY for multi-Port Group device (and more then single port group)
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number (also CPU port)
* OUTPUTS:
*       enablePtr   - (pointer to) enable :
*                     GT_TRUE -  a ring port
*                     GT_FALSE - not a ring port
*
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or portNum
*    GT_BAD_STATE - the settings in HW are not synchronized between the 2 sections
*                   (HA,TTI)
*    GT_BAD_PTR   - one of the parameters is NULL pointer
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupPortRingEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;
    GT_PHYSICAL_PORT_NUM   portNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];

    /* convert device,port (if needed) */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum,portNum);

    /* call cpss api function */
    result = cpssDxChMultiPortGroupPortRingEnableGet(devNum,
                portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d" ,enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMultiPortGroupPortRingEnableSet
*
* DESCRIPTION:
*      function enable/disable a port to be 'Ring port'.
*
* APPLICABLE DEVICES:  Lion and above
*                      --ONLY for multi-Port Group device (and more then single port group)
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number (also CPU port)
*       enable      - GT_TRUE -  a ring port
*                     GT_FALSE - not a ring port
* OUTPUTS:
*       None
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or portNum
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupPortRingEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;
    GT_PHYSICAL_PORT_NUM   portNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PHYSICAL_PORT_NUM)inArgs[1];
    enable =  (GT_BOOL)inArgs[2];

    /* convert device,port (if needed) */
    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum,portNum);

    /* call cpss api function */
    result = cpssDxChMultiPortGroupPortRingEnableSet(devNum,
                portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "" );

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet
*
* DESCRIPTION:
*      Function get the enable/disable state of the multi port groups Unknown DA mask
*
* APPLICABLE DEVICES:  Lion and above
*                      --ONLY for multi-Port Group device (and more then single port group)
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*                              - read only from first active port group of the bitmap.
*       sourcePortGroupId - the source Port Group Id (0..3), that started the lookups in
*                   the ring for the handled packet
*
* OUTPUTS:
*       enablePtr - (pointer to) enable/disable the mask
*               GT_FALSE -  Disable --> the 'src port group' is the last port group in
*               the inter- port group ring, If the DA is not found, and the packet
*               source port group is 'src port group', the relevant VLAN command
*               is applied to the packet:
*               UnregisteredIPv4BC , UnregisteredIPv4BC , UnknownUnicastCmd ,
*               UnregisteredIPv6MulticastCmd , UnregisteredIPv4MulticastCmd ,
*               UnregisteredNonIPMulticastCmd
*               GT_TRUE - Enable --> the 'src port group' is NOT the last port group
*               in the inter- port group ring,If the DA is not found, and the packet
*               source port group is 'src port group', the relevant Unknown/Unregistered
*               VLAN command is ignored and the packet is assigned the FORWARD
*               command
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or sourcePortGroupId
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet
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
    GT_U32  sourcePortGroupId;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    sourcePortGroupId = (GT_U32)inArgs[2];

    /* convert device,port group bmp (if needed) */
    CONVERT_DEV_PORT_GROUPS_BMP_MAC(devNum, portGroupsBmp);

    /* call cpss api function */
    result = cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet(devNum,
                portGroupsBmp,sourcePortGroupId ,
                &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d" , enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet
*
* DESCRIPTION:
*      Function enable/disable the multi port groups Unknown DA mask
*
*      This configuration should be done similarly in cpssDxChMultiPortGroupLookupNotFoundTableSet
*      unknownDaCommand = CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_NO_REDIRECT_E in API of
*      cpssDxChMultiPortGroupLookupNotFoundTableSet
*      is like enable = GT_FALSE in current API
*      and
*      unknownDaCommand = CPSS_DXCH_MULTI_PORT_GROUP_LOOKUP_NOT_FOUND_REDIRECT_TO_RING_E
*      is like enable=GT_TRUE
*
* APPLICABLE DEVICES:  Lion and above
*                      --ONLY for multi-Port Group device (and more then single port group)
*
* INPUTS:
*       devNum      - device number
*       portGroupsBmp - bitmap of Port Groups.
*                      NOTEs:
*                          bitmap must be set with at least one bit representing
*                          valid port group(s). If a bit of non valid port group
*                          is set then function returns GT_BAD_PARAM.
*                          Value CPSS_PORT_GROUP_UNAWARE_MODE_CNS is supported.
*       sourcePortGroupId - the source Port Group Id (0..3), that started the lookups in
*                   the ring for the handled packet
*       enable - enable/disable the mask
*               GT_FALSE -  Disable --> the 'src port group' is the last port group in
*               the inter- port group ring, If the DA is not found, and the packet
*               source port group is 'src port group', the relevant VLAN command
*               is applied to the packet:
*               UnregisteredIPv4BC , UnregisteredIPv4BC , UnknownUnicastCmd ,
*               UnregisteredIPv6MulticastCmd , UnregisteredIPv4MulticastCmd ,
*               UnregisteredNonIPMulticastCmd
*               GT_TRUE - Enable --> the 'src port group' is NOT the last port group
*               in the inter- port group ring,If the DA is not found, and the packet
*               source port group is 'src port group', the relevant Unknown/Unregistered
*               VLAN command is ignored and the packet is assigned the FORWARD
*               command
*
* OUTPUTS:
*       none
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or sourcePortGroupId
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet
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
    GT_U32  sourcePortGroupId;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portGroupsBmp = (GT_PORT_GROUPS_BMP)inArgs[1];
    sourcePortGroupId = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* convert device,port group bmp (if needed) */
    CONVERT_DEV_PORT_GROUPS_BMP_MAC(devNum, portGroupsBmp);

    /* call cpss api function */
    result = cpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet(devNum,
                portGroupsBmp,sourcePortGroupId ,
                enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/********************/
/* start of:        */
/* static variables for commands and tables that relate to API cpssDxChMultiPortGroupConfigSet(...) */
/********************/
/* max number of uplinks */
#define MAX_UPLINKS_CNS             8

static GT_PORT_NUM   uplinkPortsArr[MAX_UPLINKS_CNS];
static GT_U32  numOfUplinkPorts = 0;

static CPSS_DXCH_MULTI_PORT_GROUP_RING_INFO_STC   ringInfoArr[CPSS_MAX_PORT_GROUPS_CNS];
static GT_U32  ringInfoNum = 0;
static GT_U32  currentRingIndex;
static GT_U32 lastSrcPortGroupId;

/* max number of actual interconnection ports used */
#define MAX_NUM_OF_INTERCONNECTION_PORTS_CNS    CPSS_MAX_PORT_GROUPS_CNS

/*array for the interconnection ports (ring ports , 'redirected' ports )*/
static GT_PHYSICAL_PORT_NUM            interconnectionPortsArr[MAX_NUM_OF_INTERCONNECTION_PORTS_CNS];
/* current index in interconnectionPortsArr[]*/
static GT_U32           interconnectionIndex = 0;

/* max number of internal trunks */
#define MAX_INTERNAL_TRUNKS_CNS         12

/* array of internal trunks */
static GT_TRUNK_ID      internalTrunksArr[MAX_INTERNAL_TRUNKS_CNS];
/* current index in internalTrunksArr[]*/
static GT_U32           internalTrunksIndex = 0;

/********************/
/* end of:          */
/* static variables for commands and tables that relate to API cpssDxChMultiPortGroupConfigSet(...) */
/********************/

/*******************************************************************************
* cpssDxChMultiPortGroupConfigSet
*
* DESCRIPTION:
*
*       The purpose of this API is configure a Multi portGroup topology.
*       This API configures several tables/registers in the HW to support
*       requested topology. the aggregated functionality described in details in
*       the user guide for this feature.
*       Application can implement the buildup of the topology differently for
*       enhanced mode(but not for basic mode because the CPSS uses DB for
*       'runtime' purposes such as setting 'cpu code table',vlan entry , pvid ...)
*
* APPLICABLE DEVICES:  Lion and above
*                      --ONLY for multi-Port Group device (and more then single port group)
*
* INPUTS:
*       devNum - the device number
*       mode - selects between "Basic" and "Enhanced" modes (refer to FS for full
*              details on these 2 modes).
*       numOfUplinkPorts - number of uplink ports in uplinkPortsArr[]
*       uplinkPortsArr[] - array of uplink ports.
*                   configures the Uplinks of the defined topology.
*                      - In Basic mode all uplinks must reside on root portGroup.
*                      - In Basic+ mode all uplinks which are remote to the root portGroup
*                        should be redirected to dedicated redirected uplink on root portGroup.
*                      - In Enhanced mode uplinks can reside on any portGroup, no special restriction.
*
*       ringInfoNum - number of elements in array of ringInfoArr[]
*       ringInfoArr[] - array of information about ring connections.
*               The ringInfoNum & ringInfoArr holds the information of rings ports in topology.
*               Array is built in such way that for each portGroup that has uplinks should
*               configure all rings ports that would be used to channel its traffic to
*               all other portGroups (In 64K its only to its member in couple)
*               Multiple rings ports can be used to pass traffic between two hops of
*               same channel - in that case these groups of ports should be assigned with
*               trunk Id (In Basic/ Basic+ there is limitation of the trunkId possible
*               Ids...
*               its has to be :
*               trunkId 76 if its own portGroup 0,
*                        92 on portGroup 1,
*                        108 on portGroup 2 and
*                        124 on portGroup 3.
*               Same Rings ports can be reused to pass different channels from different
*               source portGroups, thus creating over-subscription of traffic.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or one of the ports in uplinkPortsArr[] ,
*                   or in ringInfoArr[]
*    GT_BAD_PTR   - on NULL pointer
*    GT_OUT_OF_CPU_MEM - dynamic allocation for internal DB failed
*    GT_FAIL - internal management implementation error.
*    GT_ALREADY_EXIST   - already initialized
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupConfigSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8   devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* NOTE: this command uses the configuration already set in tables :
        cpssDxChMultiPortGroupConfigSet_uplinks ,
        cpssDxChMultiPortGroupConfigSet_ringPorts ,
        cpssDxChMultiPortGroupConfigSet_TrunksForRingPorts

        that set next info:
        numOfUplinkPorts,
        uplinkPortsArr,
        ringInfoNum,
        ringInfoArr
    */

    /* call cpss api function */
    result = cpssDxChMultiPortGroupConfigSet(devNum,
                    numOfUplinkPorts,
                    uplinkPortsArr,
                    ringInfoNum,
                    ringInfoArr);

    /* reset the number of uplinks , ring info ...*/
    numOfUplinkPorts = 0;
    ringInfoNum = 0;
    interconnectionIndex = 0;
    internalTrunksIndex = 0;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChMultiPortGroupConfigSet_uplinksSet
*
* DESCRIPTION:
*
*       set uplinks for cpssDxChMultiPortGroupConfigSet
*
* COMMENTS
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupConfigSet_uplinksSet
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

    /* map input arguments to locals */

    if(numOfUplinkPorts >= MAX_UPLINKS_CNS)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_FULL, "");

        return CMD_OK;
    }

    uplinkPortsArr[numOfUplinkPorts++] = (GT_PORT_NUM)inFields[0];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChMultiPortGroupConfigSet_ringPortsSet
*
* DESCRIPTION:
*
*       set rings ports info for cpssDxChMultiPortGroupConfigSet
*
* COMMENTS
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupConfigSet_ringPortsSet
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

    /* map input arguments to locals */

    if(interconnectionIndex >= MAX_NUM_OF_INTERCONNECTION_PORTS_CNS)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_FULL, "");

        return CMD_OK;
    }

    interconnectionPortsArr[interconnectionIndex++] = (GT_PHYSICAL_PORT_NUM)inFields[0];

    ringInfoArr[currentRingIndex].numOfRingPorts ++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChMultiPortGroupConfigSet_ringPortsSetFirst
*
* DESCRIPTION:
*
*       set rings ports info for cpssDxChMultiPortGroupConfigSet
*
* COMMENTS
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupConfigSet_ringPortsSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32  srcPortGroupId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    srcPortGroupId = (GT_U32)inArgs[0];

    /* loop over the already exists ringInfoArr[] and find index for current srcPortGroupId */
    for(currentRingIndex = 0 ; currentRingIndex < ringInfoNum; currentRingIndex++)
    {
        if(srcPortGroupId == ringInfoArr[currentRingIndex].srcPortGroupId)
        {
            /* already exists */
            break;
        }
    }

    if(currentRingIndex == ringInfoNum)
    {
        /* not found .. need new index */

        if(ringInfoNum >= CPSS_MAX_PORT_GROUPS_CNS)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_FULL, "");

            return CMD_OK;
        }

        ringInfoArr[ringInfoNum].srcPortGroupId = srcPortGroupId;
        ringInfoArr[ringInfoNum].numOfRingPorts = 0;
        ringInfoArr[ringInfoNum].ringPortsPtr = &interconnectionPortsArr[interconnectionIndex];

        currentRingIndex = ringInfoNum;
        ringInfoNum++;

        lastSrcPortGroupId = srcPortGroupId;
    }
    else
    {
        if(lastSrcPortGroupId != srcPortGroupId)
        {
            /* can't add new ports to array because already bound by ports from
               other source port group */

            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_BAD_STATE, "%s" ,
               "can't add new ports to array because already bound by ports from "
               "other source port group");

            return CMD_OK;
        }

        /* no change needed ... just go on */
    }

    return wrCpssDxChMultiPortGroupConfigSet_ringPortsSet(inArgs,inFields,numFields,outArgs);
}


/*******************************************************************************
* wrCpssDxChMultiPortGroupConfigSet_TrunksForRingPortsSet
*
* DESCRIPTION:
*
*       set rings trunks info for cpssDxChMultiPortGroupConfigSet
*
* COMMENTS
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupConfigSet_TrunksForRingPortsSet
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

    /* map input arguments to locals */

    if(internalTrunksIndex >= MAX_INTERNAL_TRUNKS_CNS)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_FULL, "");

        return CMD_OK;
    }

    internalTrunksArr[internalTrunksIndex++] = (GT_TRUNK_ID)inFields[0];

    ringInfoArr[currentRingIndex].trunkIdNum ++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChMultiPortGroupConfigSet_TrunksForRingPortsSetFirst
*
* DESCRIPTION:
*
*       set ring trunks info for cpssDxChMultiPortGroupConfigSet
*
* COMMENTS
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupConfigSet_TrunksForRingPortsSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32  srcPortGroupId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    srcPortGroupId = (GT_U32)inArgs[0];

    /* loop over the already exists ringInfoArr[] and find index for current srcPortGroupId */
    for(currentRingIndex = 0 ; currentRingIndex < ringInfoNum; currentRingIndex++)
    {
        if(srcPortGroupId == ringInfoArr[currentRingIndex].srcPortGroupId)
        {
            break;
        }
    }

    if(currentRingIndex == ringInfoNum)
    {
        /* not found -->
           you must set first the table cpssDxChMultiPortGroupConfigSet_ringPorts */
        galtisOutput(outArgs, GT_NOT_FOUND, "%s",
            "you must set first table cpssDxChMultiPortGroupConfigSet_ringPorts");

        return CMD_OK;
    }

    ringInfoArr[currentRingIndex].trunkIdNum = 0;
    ringInfoArr[currentRingIndex].trunkIdPtr = &internalTrunksArr[internalTrunksIndex];

    return wrCpssDxChMultiPortGroupConfigSet_ringPortsSet(inArgs,inFields,numFields,outArgs);
}

/********************/
/* start of:        */
/* static variables cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet(...) */
/********************/

/*array for the pairs of ports (trunk port + source port)*/
static CPSS_DXCH_MULTI_PORT_GROUP_SRC_PORT_HASH_PAIR_STC   pairsArr[MAX_NUM_OF_INTERCONNECTION_PORTS_CNS];
/* current index in pairsArr[] */
static GT_U32   pairsIndex = 0;

/********************/
/* end of:          */
/* static variables cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet(...) */
/********************/

/*******************************************************************************
* cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet
*
* DESCRIPTION:
*
*       1. By default traffic flowing via Multi-portGroups Ring Trunks is
*          distributed over Trunk’s member ports based on Packet-info hashing.
*       2. API should be called after topology buildup , in case application
*          wishes to use srcPort Trunk hashing over certain multi-portGroup
*          ring Trunk.
*       3. Application should use this API to map portGroup local source ports
*          to next’s PortGroup Trunk member ports.
*       4. Source ports used for mapping are always local portGroup ports.
*       5. Each Source Port is mapped to Trunk member port
*       6. Multiple Source Ports can be mapped to same Trunk member port allowing
*          oversubscribing
*       7. Application can't map two Source ports that falls into same Source
*          hash index into different trunk member ports
*       8. In Basic mode, API configures Trunk designated table
*       9. In Enhance mode, API is consuming 2 device numbers (device 29,30) -
*          and these 2 devices (from device map table) are used for mapping to
*          Trunks Designated member table.
*       10. An application is responsible to check ports in trunkPort are really
*           the members of trunkId. (the function does not check)
*       11. In Basic mode application can use only %8 (modulo) mode.
*       12. In Enhanced mode both modulo %8 and %64 (modulo) modes are supported.
*
*   in 'basic' mode:
*       the function sets the designated device table with the portsArr[].trunkPort
*       in indexes that match hash (%8(modulo)) on the ports in portsArr[].srcPort
*       this to allow 'Src port' trunk hash for traffic sent to the specified
*       trunk.
*   in 'enhanced' mode:
*       the function 'unbind' the trunkId from 'next ring interface' setting in
*       the relevant port group and 'bind' other 'virtual interface' {port,device}.
*       this 'virtual interface' actually point the PP to the 'device map table'
*       which in turn will point to the specified trunk , and will set 'src port'
*       hash.
*       the function also sets the designated device table like described for
*       'basic' mode, the only difference is that also hash mode of %64 (modulo)
*       supported.
*
* APPLICABLE DEVICES:  Lion and above
*                      --ONLY for multi-Port Group device (and more then single port group)
*
* INPUTS:
*       devNum      - device number
*       trunkId     - the trunkId that the ports in portsArr[].trunkPort belongs to.
*       numOfPorts  - number of pairs in array portsArr[].
*       portsArr[] - (array of) pairs of 'source ports' ,'trunk ports'
*                     for the source port hash.
*       mode - hash mode (%8 or %64 (modulo))
*
* OUTPUTS:
*       None
*
* RETURNS:
*    GT_OK        - on success
*    GT_HW_ERROR  - on hardware error
*    GT_BAD_PARAM - on wrong devNum or trunkId or port in portsArr[].srcPort or
*                   port in portsArr[].trunkPort or mode
*                   or conflict according to description bullet#7
*    GT_NOT_APPLICABLE_DEVICE - on not applicable device (device is not multi-port groups device)
*
* COMMENTS:
*
*******************************************************************************/
/*******************************************************************************
* wrCpssDxChMultiPortGroupTrunkSrcPortHashMappingSetSet
*
* DESCRIPTION:
*
*       set info for cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet
*
* COMMENTS
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupTrunkSrcPortHashMappingSetSet
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

    /* map input arguments to locals */

    if(pairsIndex >= MAX_NUM_OF_INTERCONNECTION_PORTS_CNS)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_FULL, "");

        return CMD_OK;
    }

    pairsArr[pairsIndex].srcPort   = (GT_PHYSICAL_PORT_NUM)inFields[0];
    pairsArr[pairsIndex].trunkPort = (GT_PHYSICAL_PORT_NUM)inFields[1];
    pairsIndex++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChMultiPortGroupTrunkSrcPortHashMappingSetSetEnd
*
* DESCRIPTION:
*
*       set info for cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet
*
* COMMENTS
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMultiPortGroupTrunkSrcPortHashMappingSetSetEnd
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8             devNum;
    GT_TRUNK_ID       trunkId;
    CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT  mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    mode = (CPSS_DXCH_MULTI_PORT_GROUP_SRC_HASH_MODE_ENT)inArgs[2];

    if(PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        if (0 == PRV_CPSS_DXCH_LION_FAMILY_CHECK_MAC(devNum))
        {
            /* WA to force the CPSS to think that the current operation is under 'basic mode' */
            PRV_CPSS_DXCH_PP_MAC(devNum)->portGroupsExtraInfo.debugInfo.debugInfoValid = GT_TRUE;
        }
    }

    /* call the API */
    result = cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet(
                devNum,trunkId,pairsIndex,pairsArr,mode);

    /* reset for next setting */
    pairsIndex = 0;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrDummyOk
*
* DESCRIPTION:
*
*       dummy function to return GT_OK
*
* COMMENTS
*
*******************************************************************************/
static CMD_STATUS wrDummyOk
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(outArgs)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");
    }

    return CMD_OK;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChMultiPortGroupFdbModeGet",
    &wrCpssDxChMultiPortGroupFdbModeGet,
    1, 0},

    {"cpssDxChMultiPortGroupFdbModeSet",
    &wrCpssDxChMultiPortGroupFdbModeSet,
    2, 0},

    {"cpssDxChMultiPortGroupLookupNotFoundTableGet",
    &wrCpssDxChMultiPortGroupLookupNotFoundTableGet,
    3, 0},

    {"cpssDxChMultiPortGroupLookupNotFoundTableSet",
    &wrCpssDxChMultiPortGroupLookupNotFoundTableSet,
    8, 0},

    {"cpssDxChMultiPortGroupPortLookupEnableGet",
    &wrCpssDxChMultiPortGroupPortLookupEnableGet,
    2, 0},

    {"cpssDxChMultiPortGroupPortLookupEnableSet",
    &wrCpssDxChMultiPortGroupPortLookupEnableSet,
    3, 0},

    {"cpssDxChMultiPortGroupPortRingEnableGet",
    &wrCpssDxChMultiPortGroupPortRingEnableGet,
    2, 0},

    {"cpssDxChMultiPortGroupPortRingEnableSet",
    &wrCpssDxChMultiPortGroupPortRingEnableSet,
    3, 0},

    {"MultiPortGroupBridgeUnknownDaMaskEnableGet",
    &wrCpssDxChMultiPortGroupBridgeUnknownDaMaskEnableGet,
    3, 0},

    {"MultiPortGroupBridgeUnknownDaMaskEnableSet",
    &wrCpssDxChMultiPortGroupBridgeUnknownDaMaskEnableSet,
    4, 0},

    /********************/
    /* start of commands , tables that relate to API cpssDxChMultiPortGroupConfigSet */
    /********************/
    {"cpssDxChMultiPortGroupConfigSet",
    &wrCpssDxChMultiPortGroupConfigSet,
    2, 0},

    /* start table cpssDxChMultiPortGroupConfigSet_uplinks
    multi Set Table */

    /* set first */
    {"cpssDxChMultiPortGroupConfigSet_uplinksSetFirst",
    &wrCpssDxChMultiPortGroupConfigSet_uplinksSet,
    0, 1},

    /* set next */
    {"cpssDxChMultiPortGroupConfigSet_uplinksSetNext",
    &wrCpssDxChMultiPortGroupConfigSet_uplinksSet,
    0, 1},

    /* set end */
    {"cpssDxChMultiPortGroupConfigSet_uplinksEndSet",
    &wrDummyOk,
    0, 1},

    /* cancel set */
    {"cpssDxChMultiPortGroupConfigSet_uplinksCancelSet",
    &wrDummyOk,
    0, 1},
    /* end table cpssDxChMultiPortGroupConfigSet_uplinks */



    /* start table cpssDxChMultiPortGroupConfigSet_ringPorts
    multi Set Table */

    /* set first */
    {"cpssDxChMultiPortGroupConfigSet_ringPortsSetFirst",
    &wrCpssDxChMultiPortGroupConfigSet_ringPortsSetFirst,
    1, 1},

    /* set next */
    {"cpssDxChMultiPortGroupConfigSet_ringPortsSetNext",
    &wrCpssDxChMultiPortGroupConfigSet_ringPortsSet,
    1, 1},

    /* set end */
    {"cpssDxChMultiPortGroupConfigSet_ringPortsEndSet",
    &wrDummyOk,
    1, 1},

    /* cancel set */
    {"cpssDxChMultiPortGroupConfigSet_ringPortsCancelSet",
    &wrDummyOk,
    1, 1},
    /* end table cpssDxChMultiPortGroupConfigSet_ringPorts */



    /* start table cpssDxChMultiPortGroupConfigSet_TrunksForRingPorts
    multi Set Table */

    /* set first */
    {"cpssDxChMultiPortGroupConfigSet_TrunksForRingPortsSetFirst",
    &wrCpssDxChMultiPortGroupConfigSet_TrunksForRingPortsSetFirst,
    1, 1},

    /* set next */
    {"cpssDxChMultiPortGroupConfigSet_TrunksForRingPortsSetNext",
    &wrCpssDxChMultiPortGroupConfigSet_TrunksForRingPortsSet,
    1, 1},

    /* set end */
    {"cpssDxChMultiPortGroupConfigSet_TrunksForRingPortsEndSet",
    &wrDummyOk,
    1, 1},

    /* cancel set */
    {"cpssDxChMultiPortGroupConfigSet_TrunksForRingPortsCancelSet",
    &wrDummyOk,
    1, 1},
    /* end table cpssDxChMultiPortGroupConfigSet_TrunksForRingPorts */


    /********************/
    /* end of commands , tables that relate to API cpssDxChMultiPortGroupConfigSet */
    /********************/


    /* start table cpssDxChMultiPortGroupTrunkSrcPortHashMappingSet
    multi Set Table */

    /* set first */
    {"GroupTrunkSrcPortHashMappingSetSetFirst",
    &wrCpssDxChMultiPortGroupTrunkSrcPortHashMappingSetSet,
    3, 2},

    /* set next */
    {"GroupTrunkSrcPortHashMappingSetSetNext",
    &wrCpssDxChMultiPortGroupTrunkSrcPortHashMappingSetSet,
    3, 2},

    /* set end */
    {"GroupTrunkSrcPortHashMappingSetEndSet",
    &wrCpssDxChMultiPortGroupTrunkSrcPortHashMappingSetSetEnd,
    3, 0},

    /* cancel set */
    {"GroupTrunkSrcPortHashMappingSetCancelSet",
    &wrDummyOk,
    3, 2},
    /* end table cpssDxChMultiPortGroupConfigSet_TrunksForRingPorts */



};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))


/*******************************************************************************
* cmdLibInitCpssDxChMultiPortGroup
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
GT_STATUS cmdLibInitCpssDxChMultiPortGroup
(
    GT_VOID
)
{
    GT_STATUS rc;

    /* support the updated commands without the '_G003' in the string name */
    rc = cmdInitLibrary(dbCommands, numCommands);
    return rc;
}

