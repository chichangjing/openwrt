/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChTti.c
*
* DESCRIPTION:
*       DxCh TTI wrappers
*
*
* FILE REVISION NUMBER:
*       $Revision: 77 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>

#include <cpss/dxCh/dxChxGen/tti/cpssDxChTti.h>
#include <cpss/dxCh/dxChxGen/virtualTcam/cpssDxChVirtualTcam.h>

extern GT_U32 appDemoDxChLion3TcamTtiConvertedIndexGet
(
    IN     GT_U8                            devNum,
    IN     GT_U32                           index
);

extern GT_VOID appDemoDxChLion3NewTtiTcamSupportSet
(
    IN  GT_BOOL             useAppdemoOffset
);

extern GT_U32 appDemoDxChLion3TcamTtiBaseIndexGet
(
    IN     GT_U8                            devNum,
    IN     GT_U32                           hitNum
);

extern GT_U32 appDemoDxChLion3TcamTtiNumOfIndexsGet
(
    IN     GT_U8                            devNum,
    IN     GT_U32                           hitNum
);

/* support for multi port groups */

/*******************************************************************************
* ttiMultiPortGroupsBmpGet
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
static void ttiMultiPortGroupsBmpGet
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

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiMacToMeSet
(
    IN  GT_U8                                           devNum,
    IN  GT_U32                                          entryIndex,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC                      *valuePtr,
    IN  CPSS_DXCH_TTI_MAC_VLAN_STC                      *maskPtr,
    IN  CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiMacToMeSet(
            devNum, entryIndex, valuePtr, maskPtr, interfaceInfoPtr);
    }
    else
    {
        return cpssDxChTtiPortGroupMacToMeSet(
            devNum, pgBmp, entryIndex, valuePtr, maskPtr, interfaceInfoPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiMacToMeGet
(
    IN  GT_U8                                           devNum,
    IN  GT_U32                                          entryIndex,
    IN  CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  *interfaceInfoPtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC                      *valuePtr,
    OUT CPSS_DXCH_TTI_MAC_VLAN_STC                      *maskPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiMacToMeGet(
            devNum, entryIndex, valuePtr, maskPtr, interfaceInfoPtr);
    }
    else
    {
        return cpssDxChTtiPortGroupMacToMeGet(
            devNum, pgBmp, entryIndex, valuePtr, maskPtr, interfaceInfoPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiMacModeSet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    IN  CPSS_DXCH_TTI_MAC_MODE_ENT        macMode
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiMacModeSet(
            devNum, keyType, macMode);
    }
    else
    {
        return cpssDxChTtiPortGroupMacModeSet(
            devNum, pgBmp, keyType, macMode);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiMacModeGet
(
    IN  GT_U8                             devNum,
    IN  CPSS_DXCH_TTI_KEY_TYPE_ENT        keyType,
    OUT CPSS_DXCH_TTI_MAC_MODE_ENT        *macModePtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiMacModeGet(
            devNum, keyType, macModePtr);
    }
    else
    {
        return cpssDxChTtiPortGroupMacModeGet(
            devNum, pgBmp, keyType, macModePtr);
    }
}

extern GT_U32          wrCpssDxChVirtualTcam_useVTcamApi;
extern GT_U32          wrCpssDxChVirtualTcam_vTcamMngId;
extern GT_U32          wrCpssDxChVirtualTcam_vTcamId;
extern GT_U32          wrCpssDxChVirtualTcam_vTcamRulePrio;

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiRuleSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         keyType,
    IN  CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    IN  CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr,
    IN  GT_BOOL                             absoluteTcamIndexing
)
{
    if (wrCpssDxChVirtualTcam_useVTcamApi)
    {
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ATTRIBUTES_STC  ruleAttributes;
        CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC        ruleType;
        CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC        ruleData;

        ruleAttributes.priority = wrCpssDxChVirtualTcam_vTcamRulePrio;
        ruleType.ruleType = CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_TTI_E;
        ruleType.rule.tti.ruleFormat = keyType;
        ruleData.valid = GT_TRUE;
        ruleData.rule.tti.actionPtr = actionPtr;
        ruleData.rule.tti.patternPtr = patternPtr;
        ruleData.rule.tti.maskPtr = maskPtr;

        return cpssDxChVirtualTcamRuleWrite(wrCpssDxChVirtualTcam_vTcamMngId, 
                                            wrCpssDxChVirtualTcam_vTcamId,
                                            routerTtiTcamRow,
                                            &ruleAttributes,
                                            &ruleType,
                                            &ruleData);
    }
    else
    {
        GT_BOOL             pgEnable; /* multi port group  enable */
        GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

        ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

        if (absoluteTcamIndexing == GT_FALSE)
        {
            appDemoDxChLion3NewTtiTcamSupportSet(GT_TRUE);
            routerTtiTcamRow = appDemoDxChLion3TcamTtiConvertedIndexGet(devNum,routerTtiTcamRow);
        }

        if (pgEnable == GT_FALSE)
        {
            return cpssDxChTtiRuleSet(
                devNum, routerTtiTcamRow, keyType,
                patternPtr, maskPtr, actionType, actionPtr);
        }
        else
        {
            return cpssDxChTtiPortGroupRuleSet(
                devNum, pgBmp, routerTtiTcamRow, keyType,
                patternPtr, maskPtr, actionType, actionPtr);
        }
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiRuleGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_RULE_TYPE_ENT         keyType,
    OUT CPSS_DXCH_TTI_RULE_UNT              *patternPtr,
    OUT CPSS_DXCH_TTI_RULE_UNT              *maskPtr,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    OUT CPSS_DXCH_TTI_ACTION_UNT            *actionPtr,
    IN  GT_BOOL                             absoluteTcamIndexing
)
{
    if (wrCpssDxChVirtualTcam_useVTcamApi)
    {
        CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_STC        ruleType;
        CPSS_DXCH_VIRTUAL_TCAM_RULE_DATA_STC        ruleData;

        ruleType.ruleType = CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_TTI_E;
        ruleType.rule.tti.ruleFormat = keyType;

        ruleData.rule.tti.actionPtr = actionPtr;
        ruleData.rule.tti.patternPtr = patternPtr;
        ruleData.rule.tti.maskPtr = maskPtr;

        return cpssDxChVirtualTcamRuleRead(wrCpssDxChVirtualTcam_vTcamMngId, 
                                            wrCpssDxChVirtualTcam_vTcamId,
                                            routerTtiTcamRow,
                                            &ruleType,
                                            &ruleData);
    }
    else
    {
        GT_BOOL             pgEnable; /* multi port group  enable */
        GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

        ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

        if (absoluteTcamIndexing == GT_FALSE)
        {
            appDemoDxChLion3NewTtiTcamSupportSet(GT_TRUE);
            routerTtiTcamRow = appDemoDxChLion3TcamTtiConvertedIndexGet(devNum,routerTtiTcamRow);
        }

        if (pgEnable == GT_FALSE)
        {
            return cpssDxChTtiRuleGet(
                devNum, routerTtiTcamRow, keyType,
                patternPtr, maskPtr, actionType, actionPtr);
        }
        else
        {
            return cpssDxChTtiPortGroupRuleGet(
                devNum, pgBmp, routerTtiTcamRow, keyType,
                patternPtr, maskPtr, actionType, actionPtr);
        }
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiRuleActionUpdate
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  CPSS_DXCH_TTI_ACTION_TYPE_ENT       actionType,
    IN  CPSS_DXCH_TTI_ACTION_UNT            *actionPtr,
    IN  GT_BOOL                             absoluteTcamIndexing,
    IN  GT_BOOL                             isNewIndex
)
{
    if (wrCpssDxChVirtualTcam_useVTcamApi)
    {
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_TYPE_STC actionType;
        CPSS_DXCH_VIRTUAL_TCAM_RULE_ACTION_DATA_STC actionData;
        GT_STATUS rc;

        actionType.ruleType = CPSS_DXCH_VIRTUAL_TCAM_RULE_TYPE_TTI_E;

        actionData.action.tti.actionPtr = actionPtr;

        if (isNewIndex)
        {
            /* whole rule must be created before update action */
            return GT_OK;
        }

        rc = cpssDxChVirtualTcamRuleActionUpdate(wrCpssDxChVirtualTcam_vTcamMngId,
                                            wrCpssDxChVirtualTcam_vTcamId,
                                            routerTtiTcamRow,
                                            &actionType,
                                            &actionData);
        if (rc == GT_NOT_FOUND)
        {
            /* ignore NOT_FOUND case. Rule was not created yet. */
            rc = GT_OK;
        }

        return rc;
    }
    else
    {
        GT_BOOL             pgEnable; /* multi port group  enable */
        GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

        ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

        if (absoluteTcamIndexing == GT_FALSE)
        {
            appDemoDxChLion3NewTtiTcamSupportSet(GT_TRUE);
            routerTtiTcamRow = appDemoDxChLion3TcamTtiConvertedIndexGet(devNum,routerTtiTcamRow);
        }

        if (pgEnable == GT_FALSE)
        {
            return cpssDxChTtiRuleActionUpdate(
                devNum, routerTtiTcamRow, actionType, actionPtr);
        }
        else
        {
            return cpssDxChTtiPortGroupRuleActionUpdate(
                devNum, pgBmp, routerTtiTcamRow, actionType, actionPtr);
        }
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiRuleValidStatusSet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    IN  GT_BOOL                             valid,
    IN  GT_BOOL                             absoluteTcamIndexing
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (absoluteTcamIndexing == GT_FALSE)
    {
        appDemoDxChLion3NewTtiTcamSupportSet(GT_TRUE);
        routerTtiTcamRow = appDemoDxChLion3TcamTtiConvertedIndexGet(devNum,routerTtiTcamRow);
    }

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiRuleValidStatusSet(
            devNum, routerTtiTcamRow, valid);
    }
    else
    {
        return cpssDxChTtiPortGroupRuleValidStatusSet(
            devNum, pgBmp, routerTtiTcamRow, valid);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChTtiRuleValidStatusGet
(
    IN  GT_U8                               devNum,
    IN  GT_U32                              routerTtiTcamRow,
    OUT GT_BOOL                             *validPtr,
    IN  GT_BOOL                             absoluteTcamIndexing
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ttiMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (absoluteTcamIndexing == GT_FALSE)
    {
        appDemoDxChLion3NewTtiTcamSupportSet(GT_TRUE);
        routerTtiTcamRow = appDemoDxChLion3TcamTtiConvertedIndexGet(devNum,routerTtiTcamRow);
    }

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChTtiRuleValidStatusGet(
            devNum, routerTtiTcamRow, validPtr);
    }
    else
    {
        return cpssDxChTtiPortGroupRuleValidStatusGet(
            devNum, pgBmp, routerTtiTcamRow, validPtr);
    }
}


/******************Table:cpssDxChTtiMacToMe*****************************************/

static GT_U8 macToMeTableIndex;

/*******************************************************************************
* cpssDxChTtiMacToMeSet
*
* DESCRIPTION:
*       This function sets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MAC2ME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MAC2ME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum            - device number
*       mac2MeIndex       - Index of mac and vlan in Mac2Me table (0..7)
*       macVlanPtr        - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong device id
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMacToMeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mac2MeIndex;
    CPSS_DXCH_TTI_MAC_VLAN_STC macVlan;
    CPSS_DXCH_TTI_MAC_VLAN_STC mask;
    CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  interfaceInfo;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mac2MeIndex=(GT_U8)inFields[0];
    galtisMacAddr(&macVlan.mac,(GT_U8*)inFields[1]);
    macVlan.vlanId=(GT_U16)inFields[2];
    galtisMacAddr(&mask.mac,(GT_U8*)inFields[3]);
    mask.vlanId=(GT_U16)inFields[4];

    /* use only mac and vlan fields */
    cmdOsMemSet(&interfaceInfo, 0, sizeof(CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC));
    interfaceInfo.includeSrcInterface = CPSS_DXCH_TTI_DONT_USE_SRC_INTERFACE_FIELDS_E;

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiMacToMeSet(devNum, mac2MeIndex, &macVlan, &mask,
                                           &interfaceInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMacToMeSet
*
* DESCRIPTION:
*       function sets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or wrong vlan/mac values
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMacToMe_1Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mac2MeIndex;
    CPSS_DXCH_TTI_MAC_VLAN_STC macVlan;
    CPSS_DXCH_TTI_MAC_VLAN_STC mask;
    CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC  interfaceInfo;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mac2MeIndex=(GT_U8)inFields[0];
    galtisMacAddr(&macVlan.mac,(GT_U8*)inFields[1]);
    macVlan.vlanId=(GT_U16)inFields[2];
    galtisMacAddr(&mask.mac,(GT_U8*)inFields[3]);
    mask.vlanId=(GT_U16)inFields[4];
    interfaceInfo.includeSrcInterface =
        (CPSS_DXCH_TTI_MAC_TO_ME_INCLUDE_SRC_INTERFACE_TYPE_ENT)inFields[5];
    interfaceInfo.srcHwDevice = (GT_HW_DEV_NUM)inFields[6];
    interfaceInfo.srcIsTrunk = (GT_BOOL)inFields[7];
    interfaceInfo.srcPortTrunk = (GT_U32)inFields[8];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiMacToMeSet(devNum, mac2MeIndex, &macVlan, &mask,
                                           &interfaceInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMacToMeGet
*
* DESCRIPTION:
*       This function sets the TTI MacToMe relevant Mac address and Vlan.
*       if a match is found, an internal flag is set. The MAC2ME flag
*       is optionally used as part of the TT lookup trigger for IPV4 or MPLS traffic.
*       Note: if the packet is TT and the pasenger is Ethernet, another MAC2ME
*       lookup is performed and the internal flag is set accordingly.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum            - device number
*       mac2MeIndex       - Index of mac and vlan in Mac2Me table (0..7).
*
* OUTPUTS:
*       macVlanPtr        - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - wrong device id
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMacToMeGetEntry
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;

    CPSS_DXCH_TTI_MAC_VLAN_STC macVlan;
    CPSS_DXCH_TTI_MAC_VLAN_STC mask;
    CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC interfaceInfo;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (macToMeTableIndex>7)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* initialize macVlan and mask */
    cmdOsMemSet(&macVlan , 0, sizeof(macVlan));
    cmdOsMemSet(&mask , 0, sizeof(mask));

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiMacToMeGet(devNum, macToMeTableIndex,
                                           &interfaceInfo, &macVlan, &mask);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0]= macToMeTableIndex;
    inFields[2]=macVlan.vlanId;
    inFields[4]=mask.vlanId;


    fieldOutput("%d%6b%d%6b%d", inFields[0],macVlan.mac.arEther,inFields[2],mask.mac.arEther,inFields[4]);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMacToMeGet
*
* DESCRIPTION:
*       This function gets a TTI MacToMe entry.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; xCat3; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum            - device number
*       entryIndex        - Index of mac and vlan in MacToMe table.
*                           (APPLICABLE RANGES:
*                            DxCh3; xCat; xCat3; Lion; xCat2; Lion2: 0..7
*                            Bobcat2; Caelum; Bobcat3: 0..127)
*
* OUTPUTS:
*       valuePtr          - points to Mac To Me and Vlan To Me
*       maskPtr           - points to mac and vlan's masks
*       interfaceInfoPtr  - points to source interface info (APPLICABLE DEVICES: Bobcat2; Caelum; Bobcat3)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong parameter's value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMacToMe_1GetEntry
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 lastMacToMeEntryIndex;
    GT_U8 firstEntry, numOfEntries;

    CPSS_DXCH_TTI_MAC_VLAN_STC macVlan;
    CPSS_DXCH_TTI_MAC_VLAN_STC mask;
    CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC interfaceInfo;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    firstEntry = (GT_U8)inArgs[1];
    numOfEntries = (GT_U8)inArgs[2];

    if (PRV_CPSS_DXCH_PP_HW_INFO_E_ARCH_ENABLED_MAC(devNum))
    {
        lastMacToMeEntryIndex = 127;
    }
    else
    {
        lastMacToMeEntryIndex = 7;
    }

    if (macToMeTableIndex == 0)
    {
        macToMeTableIndex = firstEntry;
    }

    if (firstEntry + numOfEntries - 1 < lastMacToMeEntryIndex)
    {
        lastMacToMeEntryIndex = firstEntry + numOfEntries - 1;
    }

    if (macToMeTableIndex > lastMacToMeEntryIndex)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* initialize macVlan and mask */
    cmdOsMemSet(&macVlan , 0, sizeof(macVlan));
    cmdOsMemSet(&mask , 0, sizeof(mask));
    cmdOsMemSet(&interfaceInfo, 0, sizeof(CPSS_DXCH_TTI_MAC_TO_ME_SRC_INTERFACE_INFO_STC));

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiMacToMeGet(devNum, macToMeTableIndex,
                                           &interfaceInfo, &macVlan, &mask);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = macToMeTableIndex;
    inFields[2] = macVlan.vlanId;
    inFields[4] = mask.vlanId;
    inFields[5] = interfaceInfo.includeSrcInterface;
    inFields[6] = interfaceInfo.srcHwDevice;
    inFields[7] = interfaceInfo.srcIsTrunk;
    inFields[8] = interfaceInfo.srcPortTrunk;

    fieldOutput("%d%6b%d%6b%d%d%d%d%d", inFields[0], macVlan.mac.arEther,
                inFields[2], mask.mac.arEther, inFields[4], inFields[5],
                inFields[6], inFields[7], inFields[8]);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/**********************************************************************************/

static CMD_STATUS wrCpssDxChTtiMacToMeGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    macToMeTableIndex=0;

    return wrCpssDxChTtiMacToMeGetEntry(inArgs,inFields,numFields,outArgs);
}

/**********************************************************************************/

static CMD_STATUS wrCpssDxChTtiMacToMeGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    macToMeTableIndex++;

    return wrCpssDxChTtiMacToMeGetEntry(inArgs,inFields,numFields,outArgs);

}

/**********************************************************************************/

static CMD_STATUS wrCpssDxChTtiMacToMe_1GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    macToMeTableIndex=0;

    return wrCpssDxChTtiMacToMe_1GetEntry(inArgs,inFields,numFields,outArgs);
}

/**********************************************************************************/

static CMD_STATUS wrCpssDxChTtiMacToMe_1GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    macToMeTableIndex++;

    return wrCpssDxChTtiMacToMe_1GetEntry(inArgs,inFields,numFields,outArgs);

}

/*******************************************************************************
* cpssDxChTtiPortLookupEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for the specified key
*       type at the port.
*
* APPLICABLE DEVICES:
*       All DXCH devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IP_V4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*       enable        - GT_TRUE: enable TTI lookup
*                       GT_FALSE: disable TTI lookup
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPortLookupEnableSet
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
    CPSS_DXCH_TTI_KEY_TYPE_ENT keyType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortLookupEnableSet(devNum, port, keyType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortLookupEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of  the
*       TTI lookup for the specified key type.
*
* APPLICABLE DEVICES:
*       All DXCH devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IP_V4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup
*                       GT_TRUE: TTI lookup is enabled
*                       GT_FALSE: TTI lookup is disabled
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id, port or key type
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPortLookupEnableGet
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
    CPSS_DXCH_TTI_KEY_TYPE_ENT keyType;
    GT_BOOL enable;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortLookupEnableGet(devNum, port, keyType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyTunneledEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only tunneled
*       packets received on port.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for tunneled packets
*                       GT_FALSE: disable IPv4 TTI lookup only for tunneled packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPortIpv4OnlyTunneledEnableSet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortIpv4OnlyTunneledEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyTunneledEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only tunneled packets received on port.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       tunneled packets
*                       GT_TRUE: IPv4 TTI lookup only for tunneled packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for tunneled packets is disabled
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or port
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPortIpv4OnlyTunneledEnableGet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortIpv4OnlyTunneledEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyMac2MeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the IPv4 TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: enable IPv4 TTI lookup only for mac to me packets
*                       GT_FALSE: disable IPv4 TTI lookup only for mac to me packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPortIpv4OnlyMac2MeEnableSet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortIpv4OnlyMacToMeEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortIpv4OnlyMac2MeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       IPv4 TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: IPv4 TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: IPv4 TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or port
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPortIpv4OnlyMac2MeEnableGet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortIpv4OnlyMacToMeEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiIpv4McEnableSet
*
* DESCRIPTION:
*       This function enables/disables the TTI lookup for IPv4 multicast
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE: enable TTI lookup for IPv4 MC
*                       GT_FALSE: disable TTI lookup for IPv4 MC
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiIpv4McEnableSet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiIpv4McEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiIpv4McEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of TTI lookup for
*       IPv4 multicast (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable TTI lookup for IPv4 MC
*                       GT_TRUE: TTI lookup for IPv4 MC enabled
*                       GT_FALSE: TTI lookup for IPv4 MC disabled
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiIpv4McEnableGet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTtiIpv4McEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortMplsOnlyMac2MeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MPLS TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE: enable MPLS TTI lookup only for mac to me packets
*                       GT_FALSE: disable MPLS TTI lookup only for mac to me packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPortMplsOnlyMac2MeEnableSet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortMplsOnlyMacToMeEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortMplsOnlyMac2MeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MPLS TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable IPv4 TTI lookup only for
*                       mac to me packets
*                       GT_TRUE: MPLS TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MPLS TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or port
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPortMplsOnlyMac2MeEnableGet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortMplsOnlyMacToMeEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortMimOnlyMacToMeEnableSet
*
* DESCRIPTION:
*       This function enables/disables the MIM TTI lookup for only mac to me
*       packets received on port.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*       enable        - GT_TRUE:  enable MIM TTI lookup only for mac to me packets
*                       GT_FALSE: disable MIM TTI lookup only for mac to me packets
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
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPortMimOnlyMacToMeEnableSet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortMimOnlyMacToMeEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortMimOnlyMacToMeEnableGet
*
* DESCRIPTION:
*       This function gets the port's current state (enable/disable) of the
*       MIM TTI lookup for only mac to me packets received on port.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum        - device number
*       port          - port number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable MIM TTI lookup only for
*                       mac to me packets
*                       GT_TRUE:  MIM TTI lookup only for mac to me packets is enabled
*                       GT_FALSE: MIM TTI lookup only for mac to me packets is disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id or port
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPortMimOnlyMacToMeEnableGet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortMimOnlyMacToMeEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*********************************/
/* cpssDxChTtiRuleAction Table */
/*********************************/

/* table cpssDxChTtiRuleAction global variables */
#define ACTION_TABLE_SIZE 1024

/* Database to hold TTI actions that has been set.
   Non valid entries maintain link list of free table entries */
typedef struct
{
    GT_BOOL                         valid;          /* weather this entry is valid           */
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType;     /* action type                           */
    CPSS_DXCH_TTI_ACTION_UNT        actionEntry;    /* action data                           */
    GT_U32                          ruleIndex;      /* rule index associated with the action */
    GT_U8                           devNum;         /* device associated with the action     */
    GT_U32                          nextFree;       /* in case not valid, next free entry    */
}ttiActionDB;

static ttiActionDB ttiActionDatabase[ACTION_TABLE_SIZE];

/* index of first free TTI action entry in the action table */
static GT_U32   firstFree = 0;
/* weather TTI action was used before or not (used to invalidate all entries
   on first time the table is used) */
static GT_BOOL  firstRun = GT_TRUE;
/* indicated invalid rule entry */
#define INVALID_RULE_INDEX 0x7FFFFFF0
/* index to go over TTI actions */
static  GT_U32   ttiActionGetIndex;

/* Database to hold TTI Multi action that has been set.*/

typedef struct
{
    GT_U8                           devNum;         /* device associated with the action     */
    GT_U32                          numOfRulesToAdd;/* number of Rules and Actions to add    */
    GT_U32                          ruleIndex;      /* rule index associated with the action */
    GT_U32                          ruleIndexDelta; /* delta between 2 indexes               */

    CPSS_DXCH_TTI_ACTION_2_STC      actionEntry;    /* action data - only type2 is supported for multi action set                           */
    GT_PORT_NUM                     portNum_0;      /* port interface to use when (ruleIndex % 4 == 0)                                      */
    GT_PORT_NUM                     portNum_1;      /* port interface to use when (ruleIndex % 4 == 1)                                      */
    GT_PORT_NUM                     portNum_2;      /* port interface to use when (ruleIndex % 4 == 2)                                      */
    GT_PORT_NUM                     portNum_3;      /* port interface to use when (ruleIndex % 4 == 3)                                      */
}ttiMultiActionDB;

static CPSS_DXCH_TTI_RULE_UNT       ethRuleMaskData;
static CPSS_DXCH_TTI_RULE_UNT       ethRulePatternData;
static ttiMultiActionDB             ttiMultiActionDatabaseParamsForEthRule;

/* holding keyTypes UDB10, UDB20, UDB30 */
static CPSS_DXCH_TTI_RULE_UNT       udbRuleMaskDataArray[3];
static CPSS_DXCH_TTI_RULE_UNT       udbRulePatternDataArray[3];
static ttiMultiActionDB             ttiMultiActionDatabaseParamsForUdbRuleArray[3];

/*******************************************************************************
* ttiActionDatabaseGet
*
* DESCRIPTION:
*  The function searches the action entries database for action entry that match
*  the rule index and device number and returns the actionEntry.
*
*******************************************************************************/
static void ttiActionDatabaseGet
(
    IN  GT_U8                   devNum,
    IN  GT_U32                  ruleIndex,
    OUT GT_U32                  *tableIndex
)
{
    GT_U32              index = 0;
    GT_BOOL             entryFound = GT_FALSE;

    while ((!entryFound) && (index < ACTION_TABLE_SIZE))
    {
        /* get next valid entry */
        while ((ttiActionDatabase[index].valid == GT_FALSE) && (index < ACTION_TABLE_SIZE))
            index++;
        /* check if the entry match the device number and rule index */
        if ((index < ACTION_TABLE_SIZE) &&
            (ttiActionDatabase[index].ruleIndex == ruleIndex) &&
            (ttiActionDatabase[index].devNum == devNum))
        {
            entryFound = GT_TRUE;
        }
        else
        {
            index++;
        }
    }

    if (entryFound)
    {
        *tableIndex = index;
    }
    else
    {
        *tableIndex = INVALID_RULE_INDEX;
    }
}

/*******************************************************************************
* ttiActionDatabaseInit
*
* DESCRIPTION:
*  The function initializes the TTI action database (set all entries as not
*  valid and create free list of all non valid entries).
*
*******************************************************************************/
static CMD_STATUS ttiActionDatabaseInit(void)
{
    GT_U32      index = 0;

    firstFree = 0;

    while (index < ACTION_TABLE_SIZE-1)
    {
        ttiActionDatabase[index].valid = GT_FALSE;
        ttiActionDatabase[index].nextFree = index + 1;
        index++;
    }
    ttiActionDatabase[index].nextFree = INVALID_RULE_INDEX;
    ttiActionDatabase[index].valid = GT_FALSE;

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiRuleActionUpdate
*
* DESCRIPTION:
*       This function updates rule action.
*
* APPLICABLE DEVICES:
*       All DXCH devices.
*
* INPUTS:
*       devNum        - device number
*       ruleIndex     - Index of the rule in the TCAM (0..3k-1).
*       actionPtr     - points to the TTI rule action that applied on packet
*                       if packet's search key matched with masked pattern.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleActionUpdate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result = GT_OK;
    GT_U8                               devNum;
    GT_U32                              ruleIndex;
    GT_U32                              index;
    CPSS_DXCH_TTI_ACTION_UNT            *actionPtr;
    GT_BOOL                             isNewIndex = GT_FALSE;
    ttiActionDB                         ttiTempAction = {0};

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)inFields[0];
    ruleIndex = (GT_U32)inFields[1];
    if (ruleIndex > 3071)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Rule index has to be in range [0..3071].\n");

        return CMD_AGENT_ERROR;
    }

    /* in case it is first usage of TTI action database, initialize it */
    if (firstRun)
    {
        ttiActionDatabaseInit();
        firstRun = GT_FALSE;
    }

    /* look for the action in the TTI action database */
    ttiActionDatabaseGet((GT_U8)inFields[0], (GT_U32)inFields[1], &index);

    /* the action wasn't in the TTI action database */
    if (index == INVALID_RULE_INDEX)
    {
        if (firstFree == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
            return CMD_AGENT_ERROR;
        }

        index = firstFree;
        firstFree = ttiActionDatabase[firstFree].nextFree;
        isNewIndex = GT_TRUE;
    }

    /* this is update of an existing action:
       save the old action parameters */
    if (isNewIndex == GT_FALSE)
    {
        cpssOsMemCpy(&ttiTempAction,&ttiActionDatabase[index],sizeof(ttiTempAction));
    }

    /* set action entry in TTI action database as valid */
    ttiActionDatabase[index].valid      = GT_TRUE;
    ttiActionDatabase[index].ruleIndex  = ruleIndex;
    ttiActionDatabase[index].devNum     = devNum;
    ttiActionDatabase[index].actionType = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

    actionPtr = &(ttiActionDatabase[index].actionEntry);

    /* fill action fields */
    actionPtr->type1.tunnelTerminate                  = (GT_BOOL)inFields[2];
    actionPtr->type1.passengerPacketType              = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
    actionPtr->type1.copyTtlFromTunnelHeader          = (GT_BOOL)inFields[4];
    actionPtr->type1.command                          = (CPSS_PACKET_CMD_ENT)inFields[5];
    actionPtr->type1.redirectCommand                  = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[6];
    actionPtr->type1.egressInterface.type             = (CPSS_INTERFACE_TYPE_ENT)inFields[7];
    actionPtr->type1.egressInterface.devPort.hwDevNum   = (GT_HW_DEV_NUM)inFields[8];
    actionPtr->type1.egressInterface.devPort.portNum  = (GT_PORT_NUM)inFields[9];
    /* Override Device and Port */
    CONVERT_DEV_PORT_DATA_MAC(actionPtr->type1.egressInterface.devPort.hwDevNum, actionPtr->type1.egressInterface.devPort.portNum);
    actionPtr->type1.egressInterface.trunkId          = (GT_TRUNK_ID)inFields[10];
    CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type1.egressInterface.trunkId);
    actionPtr->type1.egressInterface.vidx             = (GT_U16)inFields[11];
    actionPtr->type1.egressInterface.vlanId           = (GT_U16)inFields[12];

    actionPtr->type1.tunnelStart                      = (GT_BOOL)inFields[13];
    actionPtr->type1.tunnelStartPtr                   = (GT_U32)inFields[14];
    actionPtr->type1.routerLookupPtr                  = (GT_U32)inFields[15];
    actionPtr->type1.vrfId                            = (GT_U32)inFields[16];
    /* the fields useVidx,targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
       from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be ignored */

    actionPtr->type1.sourceIdSetEnable                = (GT_BOOL)inFields[21];
    actionPtr->type1.sourceId                         = (GT_U32)inFields[22];
    actionPtr->type1.vlanCmd                          = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[23];
    actionPtr->type1.vlanId                           = (GT_U16)inFields[24];
    actionPtr->type1.vlanPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[25];
    actionPtr->type1.nestedVlanEnable                 = (GT_BOOL)inFields[26];
    actionPtr->type1.bindToPolicer                    = (GT_BOOL)inFields[27];
    actionPtr->type1.policerIndex                     = (GT_U32)inFields[28];
    actionPtr->type1.qosPrecedence                    = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[29];
    actionPtr->type1.qosTrustMode                     = (CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT)inFields[30];
    actionPtr->type1.qosProfile                       = (GT_U32)inFields[31];
    actionPtr->type1.modifyUpEnable                   = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[32];
    actionPtr->type1.modifyDscpEnable                 = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[33];
    actionPtr->type1.up                               = (GT_U32)inFields[34];
    actionPtr->type1.remapDSCP                        = (GT_BOOL)inFields[35];
    actionPtr->type1.mirrorToIngressAnalyzerEnable    = (GT_BOOL)inFields[36];
    actionPtr->type1.userDefinedCpuCode               = (CPSS_NET_RX_CPU_CODE_ENT)inFields[37];
    actionPtr->type1.activateCounter                  = (GT_BOOL)inFields[38];
    actionPtr->type1.counterIndex                     = (GT_U32)inFields[39];
    actionPtr->type1.vntl2Echo                        = (GT_BOOL)inFields[40];
    actionPtr->type1.bridgeBypass                     = (GT_BOOL)inFields[41];
    actionPtr->type1.actionStop                       = (GT_BOOL)inFields[42];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiRuleActionUpdate(devNum,ruleIndex,ttiActionDatabase[index].actionType,actionPtr,GT_FALSE,isNewIndex);

    /* if the rule action didn't succeed:  */
    if (result != GT_OK)
    {
        /* if this is a new action remove it from the database */
        if (isNewIndex)
        {
            ttiActionDatabase[index].valid = GT_FALSE;
            ttiActionDatabase[index].nextFree = firstFree;
            firstFree = index;
        }
        /* if this is an update for existing action restore previous data */
        else
        {
            cpssOsMemCpy(&ttiActionDatabase[index],&ttiTempAction,sizeof(ttiTempAction));
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleActionGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_ACTION_UNT    *actionPtr;
    GT_HW_DEV_NUM               tempHwDev;    /* used for port,dev converting */
    GT_PORT_NUM                 tempPort;   /* used for port,dev converting */

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* get next valid TTI action entry that is from action type 1 */
    while (ttiActionGetIndex < ACTION_TABLE_SIZE)
    {
        if ((ttiActionDatabase[ttiActionGetIndex].valid == GT_TRUE) &&
        (ttiActionDatabase[ttiActionGetIndex].actionType == CPSS_DXCH_TTI_ACTION_TYPE1_ENT))
            break;
        else
            ttiActionGetIndex++;
    }

    if (ttiActionGetIndex == ACTION_TABLE_SIZE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    actionPtr = &(ttiActionDatabase[ttiActionGetIndex].actionEntry);

    inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
    inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
    inFields[2]  = actionPtr->type1.tunnelTerminate;
    inFields[3]  = actionPtr->type1.passengerPacketType;
    inFields[4]  = actionPtr->type1.copyTtlFromTunnelHeader;
    inFields[5]  = actionPtr->type1.command;
    inFields[6]  = actionPtr->type1.redirectCommand;
    inFields[7]  = actionPtr->type1.egressInterface.type;
    tempHwDev      = actionPtr->type1.egressInterface.devPort.hwDevNum;
    tempPort     = actionPtr->type1.egressInterface.devPort.portNum;
    CONVERT_BACK_DEV_PORT_DATA_MAC(tempHwDev,tempPort);
    inFields[8]  = tempHwDev;
    inFields[9]  = tempPort;
    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type1.egressInterface.trunkId);
    inFields[10] = actionPtr->type1.egressInterface.trunkId;
    inFields[11] = actionPtr->type1.egressInterface.vidx;
    inFields[12] = actionPtr->type1.egressInterface.vlanId;
    inFields[13] = actionPtr->type1.tunnelStart;
    inFields[14] = actionPtr->type1.tunnelStartPtr;
    inFields[15] = actionPtr->type1.routerLookupPtr;
    inFields[16] = actionPtr->type1.vrfId;
    /* the fields targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
       from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be hardcoded  */
    inFields[17] = GT_FALSE;
    inFields[18] = 0;
    inFields[19] = 0;
    inFields[20] = 0;
    inFields[21] = actionPtr->type1.sourceIdSetEnable;
    inFields[22] = actionPtr->type1.sourceId;
    inFields[23] = actionPtr->type1.vlanCmd;
    inFields[24] = actionPtr->type1.vlanId;
    inFields[25] = actionPtr->type1.vlanPrecedence;
    inFields[26] = actionPtr->type1.nestedVlanEnable;
    inFields[27] = actionPtr->type1.bindToPolicer;
    inFields[28] = actionPtr->type1.policerIndex;
    inFields[29] = actionPtr->type1.qosPrecedence;
    inFields[30] = actionPtr->type1.qosTrustMode;
    inFields[31] = actionPtr->type1.qosProfile;
    inFields[32] = actionPtr->type1.modifyUpEnable;
    inFields[33] = actionPtr->type1.modifyDscpEnable;
    inFields[34] = actionPtr->type1.up;
    inFields[35] = actionPtr->type1.remapDSCP;
    inFields[36] = actionPtr->type1.mirrorToIngressAnalyzerEnable;
    inFields[37] = actionPtr->type1.userDefinedCpuCode;
    inFields[38] = actionPtr->type1.activateCounter;
    inFields[39] = actionPtr->type1.counterIndex;
    inFields[40] = actionPtr->type1.vntl2Echo;
    inFields[41] = actionPtr->type1.bridgeBypass;
    inFields[42] = actionPtr->type1.actionStop;

    ttiActionGetIndex++;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],   inFields[2],   inFields[3],
                inFields[4],  inFields[5],   inFields[6],   inFields[7],
                inFields[8],  inFields[9],   inFields[10],  inFields[11],
                inFields[12], inFields[13],  inFields[14],  inFields[15],
                inFields[16], inFields[17],  inFields[18],  inFields[19],
                inFields[20], inFields[21],  inFields[22],  inFields[23],
                inFields[24], inFields[25],  inFields[26],  inFields[27],
                inFields[28], inFields[29],  inFields[30],  inFields[31],
                inFields[32], inFields[33],  inFields[34],  inFields[35],
                inFields[36], inFields[37],  inFields[38],  inFields[39],
                inFields[40], inFields[41],  inFields[42]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleActionGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ttiActionGetIndex = 0;

    return wrCpssDxChTtiRuleActionGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleActionDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32                              ruleIndex;
    GT_U8                               devNum;
    GT_U32                              index;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum =  (GT_U8)inFields[0];
    ruleIndex = (GT_U32)inFields[1];

    ttiActionDatabaseGet(devNum, ruleIndex, &index);

    /* the rule is not found */
    if (index == INVALID_RULE_INDEX)
    {
        galtisOutput(outArgs, GT_NO_SUCH, "%d", -1);
        return CMD_AGENT_ERROR;
    }

    ttiActionDatabase[index].valid = GT_FALSE;
    ttiActionDatabase[index].nextFree = firstFree;
    firstFree = index;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleActionClear
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ttiActionDatabaseInit();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/***********************************/
/* cpssDxChTtiRuleAction_1 Table */
/***********************************/

/*******************************************************************************
* wrCpssDxChTtiRuleAction_1Update
*
* DESCRIPTION:
*       This function updates rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_1Update
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result = GT_OK;
    GT_U8                               devNum;
    GT_U32                              ruleIndex;
    GT_U32                              index;
    CPSS_DXCH_TTI_ACTION_UNT            *actionPtr;
    GT_BOOL                             isNewIndex = GT_FALSE;
    ttiActionDB                         ttiTempAction = {0};

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)inFields[0];
    ruleIndex = (GT_U32)inFields[1];
    if (ruleIndex > 3071)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Rule index has to be in range [0..3071].\n");

        return CMD_AGENT_ERROR;
    }

    /* in case it is first usage of TTI action database, initialize it */
    if (firstRun)
    {
        ttiActionDatabaseInit();
        firstRun = GT_FALSE;
    }

    /* look for the action in the TTI action database */
    ttiActionDatabaseGet(devNum, ruleIndex, &index);

    /* the action wasn't in the TTI action database */
    if (index == INVALID_RULE_INDEX)
    {
        if (firstFree == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
            return CMD_AGENT_ERROR;
        }

        index = firstFree;
        firstFree = ttiActionDatabase[firstFree].nextFree;
        isNewIndex = GT_TRUE;
    }

    /* this is update of an existing action:
       save the old action parameters */
    if (isNewIndex == GT_FALSE)
    {
        cpssOsMemCpy(&ttiTempAction,&ttiActionDatabase[index],sizeof(ttiTempAction));
    }

    /* set action entry in TTI action database as valid */
    ttiActionDatabase[index].valid      = GT_TRUE;
    ttiActionDatabase[index].ruleIndex  = ruleIndex;
    ttiActionDatabase[index].devNum     = devNum;
    ttiActionDatabase[index].actionType = (CPSS_DXCH_TTI_ACTION_TYPE_ENT)inArgs[0];

    actionPtr = &(ttiActionDatabase[index].actionEntry);

    /* fill action fields */
    switch (ttiActionDatabase[index].actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:

        actionPtr->type1.tunnelTerminate                  = (GT_BOOL)inFields[2];
        actionPtr->type1.passengerPacketType              = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
        actionPtr->type1.copyTtlFromTunnelHeader          = (GT_BOOL)inFields[4];
        actionPtr->type1.command                          = (CPSS_PACKET_CMD_ENT)inFields[5];
        actionPtr->type1.redirectCommand                  = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[6];
        actionPtr->type1.egressInterface.type             = (CPSS_INTERFACE_TYPE_ENT)inFields[7];
        actionPtr->type1.egressInterface.devPort.hwDevNum   = (GT_HW_DEV_NUM)inFields[8];
        actionPtr->type1.egressInterface.devPort.portNum  = (GT_PORT_NUM)inFields[9];
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionPtr->type1.egressInterface.devPort.hwDevNum, actionPtr->type1.egressInterface.devPort.portNum);
        actionPtr->type1.egressInterface.trunkId          = (GT_TRUNK_ID)inFields[10];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type1.egressInterface.trunkId);
        actionPtr->type1.egressInterface.vidx             = (GT_U16)inFields[11];
        actionPtr->type1.egressInterface.vlanId           = (GT_U16)inFields[12];
        actionPtr->type1.tunnelStart                      = (GT_BOOL)inFields[13];
        actionPtr->type1.tunnelStartPtr                   = (GT_U32)inFields[14];
        actionPtr->type1.routerLookupPtr                  = (GT_U32)inFields[15];
        actionPtr->type1.vrfId                            = (GT_U32)inFields[16];
        /* the fields useVidx,targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
           from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be ignored */
        actionPtr->type1.sourceIdSetEnable                = (GT_BOOL)inFields[21];
        actionPtr->type1.sourceId                         = (GT_U32)inFields[22];
        actionPtr->type1.vlanCmd                          = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[23];
        actionPtr->type1.vlanId                           = (GT_U16)inFields[24];
        actionPtr->type1.vlanPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[25];
        actionPtr->type1.nestedVlanEnable                 = (GT_BOOL)inFields[26];
        actionPtr->type1.bindToPolicer                    = (GT_BOOL)inFields[27];
        actionPtr->type1.policerIndex                     = (GT_U32)inFields[28];
        actionPtr->type1.qosPrecedence                    = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[29];
        actionPtr->type1.qosTrustMode                     = (CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT)inFields[30];
        actionPtr->type1.qosProfile                       = (GT_U32)inFields[31];
        actionPtr->type1.modifyUpEnable                   = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[32];
        actionPtr->type1.modifyDscpEnable                 = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[33];
        actionPtr->type1.up                               = (GT_U32)inFields[34];
        actionPtr->type1.remapDSCP                        = (GT_BOOL)inFields[35];
        actionPtr->type1.mirrorToIngressAnalyzerEnable    = (GT_BOOL)inFields[36];
        actionPtr->type1.userDefinedCpuCode               = (CPSS_NET_RX_CPU_CODE_ENT)inFields[37];
        actionPtr->type1.vntl2Echo                        = (GT_BOOL)inFields[38];
        actionPtr->type1.bridgeBypass                     = (GT_BOOL)inFields[39];
        actionPtr->type1.actionStop                       = (GT_BOOL)inFields[40];
        actionPtr->type1.activateCounter                  = (GT_BOOL)inFields[41];
        actionPtr->type1.counterIndex                     = (GT_U32)inFields[42];
        break;

    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:

        actionPtr->type2.tunnelTerminate                 = (GT_BOOL)inFields[2];
        actionPtr->type2.ttPassengerPacketType           = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
        actionPtr->type2.tsPassengerPacketType           = (CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT)inFields[4];
        actionPtr->type2.copyTtlExpFromTunnelHeader      = (GT_BOOL)inFields[5];
        actionPtr->type2.mplsCommand                     = (CPSS_DXCH_TTI_MPLS_CMD_ENT)inFields[6];
        actionPtr->type2.mplsTtl                         = (GT_U32)inFields[7];
        actionPtr->type2.enableDecrementTtl              = (GT_BOOL)inFields[8];
        actionPtr->type2.command                         = (CPSS_PACKET_CMD_ENT)inFields[9];
        actionPtr->type2.redirectCommand                 = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[10];
        actionPtr->type2.egressInterface.type            = (CPSS_INTERFACE_TYPE_ENT)inFields[11];
        actionPtr->type2.egressInterface.devPort.hwDevNum  = (GT_HW_DEV_NUM)inFields[12];
        actionPtr->type2.egressInterface.devPort.portNum = (GT_PORT_NUM)inFields[13];
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionPtr->type2.egressInterface.devPort.hwDevNum, actionPtr->type2.egressInterface.devPort.portNum);
        actionPtr->type2.egressInterface.trunkId         = (GT_TRUNK_ID)inFields[14];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type2.egressInterface.trunkId);
        actionPtr->type2.egressInterface.vidx            = (GT_U16)inFields[15];
        actionPtr->type2.egressInterface.vlanId          = (GT_U16)inFields[16];
        actionPtr->type2.arpPtr                          = (GT_U32)inFields[17];
        actionPtr->type2.tunnelStart                     = (GT_BOOL)inFields[18];
        actionPtr->type2.tunnelStartPtr                  = (GT_U32)inFields[19];
        actionPtr->type2.routerLttPtr                    = (GT_U32)inFields[20];
        actionPtr->type2.vrfId                           = (GT_U32)inFields[21];
        actionPtr->type2.sourceIdSetEnable               = (GT_BOOL)inFields[22];
        actionPtr->type2.sourceId                        = (GT_U32)inFields[23];
        actionPtr->type2.tag0VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[24];
        actionPtr->type2.tag0VlanId                      = (GT_U16)inFields[25];
        actionPtr->type2.tag1VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[26];
        actionPtr->type2.tag1VlanId                      = (GT_U16)inFields[27];
        actionPtr->type2.tag0VlanPrecedence              = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[28];
        actionPtr->type2.nestedVlanEnable                = (GT_BOOL)inFields[29];
        actionPtr->type2.bindToPolicerMeter              = (GT_BOOL)inFields[30];
        actionPtr->type2.bindToPolicer                   = (GT_BOOL)inFields[31];
        actionPtr->type2.policerIndex                    = (GT_U32)inFields[32];
        actionPtr->type2.qosPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[33];
        actionPtr->type2.keepPreviousQoS                 = (GT_BOOL)inFields[34];
        actionPtr->type2.trustUp                         = (GT_BOOL)inFields[35];
        actionPtr->type2.trustDscp                       = (GT_BOOL)inFields[36];
        actionPtr->type2.trustExp                        = (GT_BOOL)inFields[37];
        actionPtr->type2.qosProfile                      = (GT_U32)inFields[38];
        actionPtr->type2.modifyTag0Up                    = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[39];
        actionPtr->type2.tag1UpCommand                   = (CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT)inFields[40];
        actionPtr->type2.modifyDscp                      = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[41];
        actionPtr->type2.tag0Up                          = (GT_U32)inFields[42];
        actionPtr->type2.tag1Up                          = (GT_U32)inFields[43];
        actionPtr->type2.remapDSCP                       = (GT_BOOL)inFields[44];
        actionPtr->type2.pcl0OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[45];
        actionPtr->type2.pcl0_1OverrideConfigIndex       = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[46];
        actionPtr->type2.pcl1OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[47];
        actionPtr->type2.iPclConfigIndex                 = (GT_U32)inFields[48];
        actionPtr->type2.mirrorToIngressAnalyzerEnable   = (GT_BOOL)inFields[49];
        actionPtr->type2.userDefinedCpuCode              = (CPSS_NET_RX_CPU_CODE_ENT)inFields[50];
        actionPtr->type2.bindToCentralCounter            = (GT_BOOL)inFields[51];
        actionPtr->type2.centralCounterIndex             = (GT_U32)inFields[52];
        actionPtr->type2.vntl2Echo                       = (GT_BOOL)inFields[53];
        actionPtr->type2.bridgeBypass                    = (GT_BOOL)inFields[54];
        actionPtr->type2.ingressPipeBypass               = (GT_BOOL)inFields[55];
        actionPtr->type2.actionStop                      = (GT_BOOL)inFields[56];

        if (actionPtr->type2.tunnelStart == GT_FALSE)
        {
            actionPtr->type2.modifyMacDa = GT_TRUE;
            actionPtr->type2.modifyMacSa = GT_TRUE;
        }
        else
        {
            actionPtr->type2.modifyMacDa = GT_FALSE;
            actionPtr->type2.modifyMacSa = GT_FALSE;
        }

        break;

    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_AGENT_ERROR;
    }

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiRuleActionUpdate(devNum,ruleIndex,ttiActionDatabase[index].actionType,actionPtr,GT_FALSE,isNewIndex);

    /* if the rule action didn't succeed:  */
    if (result != GT_OK)
    {
        /* if this is a new action remove it from the database */
        if (isNewIndex)
        {
            ttiActionDatabase[index].valid = GT_FALSE;
            ttiActionDatabase[index].nextFree = firstFree;
            firstFree = index;
        }
        /* if this is an update for existing action restore previous data */
        else
        {
            cpssOsMemCpy(&ttiActionDatabase[index],&ttiTempAction,sizeof(ttiTempAction));
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_1Get
*
* DESCRIPTION:
*       This function gets single rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_1Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_ACTION_UNT    *actionPtr;
    GT_HW_DEV_NUM               tempHwDev;    /* used for port,dev converting */
    GT_PORT_NUM                 tempPort;   /* used for port,dev converting */

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* get next valid TTI action entry */
    while ((ttiActionGetIndex < ACTION_TABLE_SIZE) &&
           (ttiActionDatabase[ttiActionGetIndex].valid == GT_FALSE))
        ttiActionGetIndex++;

    if (ttiActionGetIndex == ACTION_TABLE_SIZE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    actionPtr = &(ttiActionDatabase[ttiActionGetIndex].actionEntry);

    switch (ttiActionDatabase[ttiActionGetIndex].actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:

        inArgs[0] = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

        inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
        inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
        inFields[2]  = actionPtr->type1.tunnelTerminate;
        inFields[3]  = actionPtr->type1.passengerPacketType;
        inFields[4]  = actionPtr->type1.copyTtlFromTunnelHeader;
        inFields[5]  = actionPtr->type1.command;
        inFields[6]  = actionPtr->type1.redirectCommand;
        inFields[7]  = actionPtr->type1.egressInterface.type;
        tempHwDev      = actionPtr->type1.egressInterface.devPort.hwDevNum;
        tempPort     = actionPtr->type1.egressInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempHwDev,tempPort);
        inFields[8]  = tempHwDev;
        inFields[9]  = tempPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type1.egressInterface.trunkId);
        inFields[10] = actionPtr->type1.egressInterface.trunkId;
        inFields[11] = actionPtr->type1.egressInterface.vidx;
        inFields[12] = actionPtr->type1.egressInterface.vlanId;
        inFields[13] = actionPtr->type1.tunnelStart;
        inFields[14] = actionPtr->type1.tunnelStartPtr;
        inFields[15] = actionPtr->type1.routerLookupPtr;
        inFields[16] = actionPtr->type1.vrfId;
        /* the fields targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
           from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be hardcoded  */
        inFields[17] = GT_FALSE;
        inFields[18] = 0;
        inFields[19] = 0;
        inFields[20] = 0;
        inFields[21] = actionPtr->type1.sourceIdSetEnable;
        inFields[22] = actionPtr->type1.sourceId;
        inFields[23] = actionPtr->type1.vlanCmd;
        inFields[24] = actionPtr->type1.vlanId;
        inFields[25] = actionPtr->type1.vlanPrecedence;
        inFields[26] = actionPtr->type1.nestedVlanEnable;
        inFields[27] = actionPtr->type1.bindToPolicer;
        inFields[28] = actionPtr->type1.policerIndex;
        inFields[29] = actionPtr->type1.qosPrecedence;
        inFields[30] = actionPtr->type1.qosTrustMode;
        inFields[31] = actionPtr->type1.qosProfile;
        inFields[32] = actionPtr->type1.modifyUpEnable;
        inFields[33] = actionPtr->type1.modifyDscpEnable;
        inFields[34] = actionPtr->type1.up;
        inFields[35] = actionPtr->type1.remapDSCP;
        inFields[36] = actionPtr->type1.mirrorToIngressAnalyzerEnable;
        inFields[37] = actionPtr->type1.userDefinedCpuCode;
        inFields[38] = actionPtr->type1.vntl2Echo;
        inFields[39] = actionPtr->type1.bridgeBypass;
        inFields[40] = actionPtr->type1.actionStop;
        inFields[41] = actionPtr->type1.activateCounter;
        inFields[42] = actionPtr->type1.counterIndex;

        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],   inFields[2],   inFields[3],
                    inFields[4],  inFields[5],   inFields[6],   inFields[7],
                    inFields[8],  inFields[9],   inFields[10],  inFields[11],
                    inFields[12], inFields[13],  inFields[14],  inFields[15],
                    inFields[16], inFields[17],  inFields[18],  inFields[19],
                    inFields[20], inFields[21],  inFields[22],  inFields[23],
                    inFields[24], inFields[25],  inFields[26],  inFields[27],
                    inFields[28], inFields[29],  inFields[30],  inFields[31],
                    inFields[32], inFields[33],  inFields[34],  inFields[35],
                    inFields[36], inFields[37],  inFields[38],  inFields[39],
                    inFields[40], inFields[41],  inFields[42]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d%f", 0);
        break;

    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:

        inArgs[0] = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
        inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
        inFields[2]  = actionPtr->type2.tunnelTerminate;
        inFields[3]  = actionPtr->type2.ttPassengerPacketType;
        inFields[4]  = actionPtr->type2.tsPassengerPacketType;
        inFields[5]  = actionPtr->type2.copyTtlExpFromTunnelHeader;
        inFields[6]  = actionPtr->type2.mplsCommand;
        inFields[7]  = actionPtr->type2.mplsTtl;
        inFields[8]  = actionPtr->type2.enableDecrementTtl;
        inFields[9]  = actionPtr->type2.command;
        inFields[10] = actionPtr->type2.redirectCommand;
        inFields[11] = actionPtr->type2.egressInterface.type;
        tempHwDev      = actionPtr->type2.egressInterface.devPort.hwDevNum;
        tempPort     = actionPtr->type2.egressInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempHwDev,tempPort);
        inFields[12] = tempHwDev;
        inFields[13] = tempPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type2.egressInterface.trunkId);
        inFields[14] = actionPtr->type2.egressInterface.trunkId;
        inFields[15] = actionPtr->type2.egressInterface.vidx;
        inFields[16] = actionPtr->type2.egressInterface.vlanId;
        inFields[17] = actionPtr->type2.arpPtr;
        inFields[18] = actionPtr->type2.tunnelStart;
        inFields[19] = actionPtr->type2.tunnelStartPtr;
        inFields[20] = actionPtr->type2.routerLttPtr;
        inFields[21] = actionPtr->type2.vrfId;
        inFields[22] = actionPtr->type2.sourceIdSetEnable;
        inFields[23] = actionPtr->type2.sourceId;
        inFields[24] = actionPtr->type2.tag0VlanCmd;
        inFields[25] = actionPtr->type2.tag0VlanId;
        inFields[26] = actionPtr->type2.tag1VlanCmd;
        inFields[27] = actionPtr->type2.tag1VlanId;
        inFields[28] = actionPtr->type2.tag0VlanPrecedence;
        inFields[29] = actionPtr->type2.nestedVlanEnable;
        inFields[30] = actionPtr->type2.bindToPolicerMeter;
        inFields[31] = actionPtr->type2.bindToPolicer;
        inFields[32] = actionPtr->type2.policerIndex;
        inFields[33] = actionPtr->type2.qosPrecedence;
        inFields[34] = actionPtr->type2.keepPreviousQoS;
        inFields[35] = actionPtr->type2.trustUp;
        inFields[36] = actionPtr->type2.trustDscp;
        inFields[37] = actionPtr->type2.trustExp;
        inFields[38] = actionPtr->type2.qosProfile;
        inFields[39] = actionPtr->type2.modifyTag0Up;
        inFields[40] = actionPtr->type2.tag1UpCommand;
        inFields[41] = actionPtr->type2.modifyDscp;
        inFields[42] = actionPtr->type2.tag0Up;
        inFields[43] = actionPtr->type2.tag1Up;
        inFields[44] = actionPtr->type2.remapDSCP;
        inFields[45] = actionPtr->type2.pcl0OverrideConfigIndex;
        inFields[46] = actionPtr->type2.pcl0_1OverrideConfigIndex;
        inFields[47] = actionPtr->type2.pcl1OverrideConfigIndex;
        inFields[48] = actionPtr->type2.iPclConfigIndex;
        inFields[49] = actionPtr->type2.mirrorToIngressAnalyzerEnable;
        inFields[50] = actionPtr->type2.userDefinedCpuCode;
        inFields[51] = actionPtr->type2.bindToCentralCounter;
        inFields[52] = actionPtr->type2.centralCounterIndex;
        inFields[53] = actionPtr->type2.vntl2Echo;
        inFields[54] = actionPtr->type2.bridgeBypass;
        inFields[55] = actionPtr->type2.ingressPipeBypass;
        inFields[56] = actionPtr->type2.actionStop;

        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],   inFields[2],   inFields[3],
                    inFields[4],  inFields[5],   inFields[6],   inFields[7],
                    inFields[8],  inFields[9],   inFields[10],  inFields[11],
                    inFields[12], inFields[13],  inFields[14],  inFields[15],
                    inFields[16], inFields[17],  inFields[18],  inFields[19],
                    inFields[20], inFields[21],  inFields[22],  inFields[23],
                    inFields[24], inFields[25],  inFields[26],  inFields[27],
                    inFields[28], inFields[29],  inFields[30],  inFields[31],
                    inFields[32], inFields[33],  inFields[34],  inFields[35],
                    inFields[36], inFields[37],  inFields[38],  inFields[39],
                    inFields[40], inFields[41],  inFields[42],  inFields[43],
                    inFields[44], inFields[45],  inFields[46],  inFields[47],
                    inFields[48], inFields[49],  inFields[50],  inFields[51],
                    inFields[52], inFields[53],  inFields[54],  inFields[55],
                    inFields[56]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d%f", 1);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI ACTION FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    ttiActionGetIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_1GetFirst
*
* DESCRIPTION:
*       This function gets first rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_1GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ttiActionGetIndex = 0;

    return wrCpssDxChTtiRuleAction_1Get(inArgs,inFields,numFields,outArgs);
}

/*************************/
/* cpssDxChTtiRule Table */
/*************************/

/* tables cpssExMxPmTtiRule global variables */

static  CPSS_DXCH_TTI_RULE_UNT          maskData;
static  CPSS_DXCH_TTI_RULE_UNT          patternData;
static  CPSS_DXCH_TTI_RULE_TYPE_ENT     ruleFormat;
static  GT_BOOL                         mask_set = GT_FALSE;    /* is mask set   */
static  GT_BOOL                         pattern_set = GT_FALSE; /* is pattern set*/
static  GT_U32                          mask_ruleIndex = 0;
static  GT_U32                          pattern_ruleIndex = 0;

/* variables for TTI rule table entries get */

/* table to hold the rule type of each added rule */
static  CPSS_DXCH_TTI_RULE_TYPE_ENT     ttiRuleFormat[36864];
/* table to hold the action type for each rule entry set (used in rule entry get) */
static  CPSS_DXCH_TTI_ACTION_TYPE_ENT   ttiRuleActionType[36864];

static  GT_U32                          ruleIndex;
static  GT_U32                          ruleIndexMaxGet;
static  GT_BOOL                         isMask;

/* convert trunk fields of test for mask ,pattern of cpss */
static void ttiRuleTrunkInfoFromTestToCpss
(
    INOUT CPSS_DXCH_TTI_RULE_COMMON_STC *commonPtr,
    IN GT_U32                  srcPortTrunk,
    IN GT_U32                  dsaSrcPortTrunk
)
{
    CPSS_DXCH_TTI_RULE_COMMON_STC *patternCommonPtr =
        &patternData.ipv4.common;/* all tti rules starts with the 'common'*/
    CPSS_DXCH_TTI_RULE_COMMON_STC *maskCommonPtr =
        &maskData.ipv4.common;/* all tti rules starts with the 'common'*/
    GT_TRUNK_ID trunkId;

    /*srcPortTrunk*/
    {
        commonPtr->srcPortTrunk = srcPortTrunk;

        trunkId = (GT_TRUNK_ID)commonPtr->srcPortTrunk;
        if(commonPtr == maskCommonPtr)
        {
            /* we do the mask now */
            gtPclMaskTrunkConvertFromTestToCpss(
                maskCommonPtr->srcIsTrunk,
                patternCommonPtr->srcIsTrunk,
                &trunkId);
        }
        else
        {
            /* we do the pattern now */
            gtPclPatternTrunkConvertFromTestToCpss(
                maskCommonPtr->srcIsTrunk,
                patternCommonPtr->srcIsTrunk,
                &trunkId);
        }

        commonPtr->srcPortTrunk = trunkId;
    }

    /*dsaSrcPortTrunk*/
    {
        commonPtr->dsaSrcPortTrunk = dsaSrcPortTrunk;

        trunkId = (GT_TRUNK_ID)commonPtr->dsaSrcPortTrunk;
        if(commonPtr == maskCommonPtr)
        {
            /* we do the mask now */
            gtPclMaskTrunkConvertFromTestToCpss(
                maskCommonPtr->dsaSrcIsTrunk,
                patternCommonPtr->dsaSrcIsTrunk,
                &trunkId);
        }
        else
        {
            /* we do the pattern now */
            gtPclPatternTrunkConvertFromTestToCpss(
                maskCommonPtr->dsaSrcIsTrunk,
                patternCommonPtr->dsaSrcIsTrunk,
                &trunkId);
        }

        commonPtr->dsaSrcPortTrunk = trunkId;
    }


    return;
}

/* convert trunk fields of test for mask ,pattern of cpss */
static void ttiRuleTrunkInfoFromTestToCpss_1
(
    IN GT_U32       pattern
)
{
    if(pattern == 0)/*mask*/
    {
        if(pattern_set == GT_FALSE)
        {
            return;
        }
        /* we do mask now and pattern already done*/
        /* we can get full info */
    }
    else
    {
        if(pattern_set == GT_FALSE)
        {
            return;
        }

        /* we do pattern now and mask already done*/
        /* we can get full info */
    }

    ttiRuleTrunkInfoFromTestToCpss(
    &maskData.ipv4.common, /* all keys start with common */
    maskData.ipv4.common.srcPortTrunk,
    maskData.ipv4.common.dsaSrcPortTrunk);

    ttiRuleTrunkInfoFromTestToCpss(
    &patternData.ipv4.common, /* all keys start with common */
    patternData.ipv4.common.srcPortTrunk,
    patternData.ipv4.common.dsaSrcPortTrunk);
}


/* convert trunk fields of mask ,pattern back to test value */
static void ttiRuleTrunkInfoFromCpssToTest
(
    INOUT CPSS_DXCH_TTI_RULE_COMMON_STC *commonPtr
)
{
    CPSS_DXCH_TTI_RULE_COMMON_STC *patternCommonPtr =
        &patternData.ipv4.common;/* all tti rules starts with the 'common'*/
    CPSS_DXCH_TTI_RULE_COMMON_STC *maskCommonPtr =
        &maskData.ipv4.common;/* all tti rules starts with the 'common'*/
    GT_TRUNK_ID trunkId;

    {
        trunkId = (GT_TRUNK_ID)commonPtr->srcPortTrunk;
        if(commonPtr == maskCommonPtr)
        {
            /* we do the mask now */
            gtPclMaskTrunkConvertFromCpssToTest(
                maskCommonPtr->srcIsTrunk,
                patternCommonPtr->srcIsTrunk,
                &trunkId);
        }
        else
        {
            /* we do the pattern now */
            gtPclPatternTrunkConvertFromCpssToTest(
                maskCommonPtr->srcIsTrunk,
                patternCommonPtr->srcIsTrunk,
                &trunkId);
        }

        commonPtr->srcPortTrunk = trunkId;
    }

    {
        trunkId = (GT_TRUNK_ID)commonPtr->dsaSrcPortTrunk;
        if(commonPtr == maskCommonPtr)
        {
            /* we do the mask now */
            gtPclMaskTrunkConvertFromCpssToTest(
                maskCommonPtr->dsaSrcIsTrunk,
                patternCommonPtr->dsaSrcIsTrunk,
                &trunkId);
        }
        else
        {
            /* we do the pattern now */
            gtPclPatternTrunkConvertFromCpssToTest(
                maskCommonPtr->dsaSrcIsTrunk,
                patternCommonPtr->dsaSrcIsTrunk,
                &trunkId);
        }

        commonPtr->dsaSrcPortTrunk = trunkId;
    }


    return;
}


/* convert trunk fields of test for mask ,pattern of cpss */
static void ttiRuleTrunkInfoFromCpssToTest_1
(
    void
)
{
    ttiRuleTrunkInfoFromCpssToTest(
    &maskData.ipv4.common);

    ttiRuleTrunkInfoFromCpssToTest(
    &patternData.ipv4.common);
}


/*******************************************************************************
* wrCpssDxChTtiRule_KEY_IPV4_Set
*
* DESCRIPTION:
*       This function maps input arguments to IPv4 TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_KEY_IPV4_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_IPV4_E;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr        = &maskData;
        mask_ruleIndex = (GT_U32)inFields[0];
    }
    else /* pattern */
    {
        rulePtr           = &patternData;
        pattern_ruleIndex = (GT_U32)inFields[0];
    }

    rulePtr->ipv4.common.pclId           = (GT_U32)inFields[2];
    rulePtr->ipv4.common.srcIsTrunk      = (GT_U32)inFields[3];
    rulePtr->ipv4.common.srcPortTrunk    = (GT_U32)inFields[4];

    if (rulePtr->ipv4.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->ipv4.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        rulePtr->ipv4.common.srcPortTrunk = (GT_U32)port;
    }

    galtisMacAddr(&rulePtr->ipv4.common.mac,(GT_U8*)inFields[5]);
    rulePtr->ipv4.common.vid             = (GT_U16)inFields[6];
    rulePtr->ipv4.common.isTagged        = (GT_BOOL)inFields[7];
    rulePtr->ipv4.tunneltype             = (GT_U32)inFields[8];
    galtisIpAddr(&rulePtr->ipv4.srcIp,(GT_U8*)inFields[9]);
    galtisIpAddr(&rulePtr->ipv4.destIp,(GT_U8*)inFields[10]);
    rulePtr->ipv4.isArp                  = (GT_BOOL)inFields[11];
    rulePtr->ipv4.common.dsaSrcIsTrunk   = (GT_BOOL)inFields[12];
    rulePtr->ipv4.common.dsaSrcPortTrunk =(GT_U32)inFields[13];
    rulePtr->ipv4.common.dsaSrcDevice    = (GT_U32)inFields[14];

    /* convert the trunk fields */
    ttiRuleTrunkInfoFromTestToCpss_1((GT_U32)inFields[1]);

    if (inFields[1] == 0) /* mask */
    {
        mask_set       = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_set       = GT_TRUE;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_KEY_MPLS_Set
*
* DESCRIPTION:
*       This function maps input arguments to MPLS TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_KEY_MPLS_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MPLS_E;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr        = &maskData;
        mask_ruleIndex = (GT_U32)inFields[0];
    }
    else /* pattern */
    {
        rulePtr           = &patternData;
        pattern_ruleIndex = (GT_U32)inFields[0];
    }

    rulePtr->mpls.common.pclId           = (GT_U32)inFields[2];
    rulePtr->mpls.common.srcIsTrunk      = (GT_U32)inFields[3];
    rulePtr->mpls.common.srcPortTrunk    = (GT_U32)inFields[4];

    if (rulePtr->mpls.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->mpls.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        rulePtr->mpls.common.srcPortTrunk = (GT_U32)port;
    }

    galtisMacAddr(&rulePtr->mpls.common.mac,(GT_U8*)inFields[5]);
    rulePtr->mpls.common.vid             = (GT_U16)inFields[6];
    rulePtr->mpls.common.isTagged        = (GT_BOOL)inFields[7];
    rulePtr->mpls.label0                 = (GT_U32)inFields[8];
    rulePtr->mpls.exp0                   = (GT_U32)inFields[9];
    rulePtr->mpls.label1                 = (GT_U32)inFields[10];
    rulePtr->mpls.exp1                   = (GT_U32)inFields[11];
    rulePtr->mpls.label2                 = (GT_U32)inFields[12];
    rulePtr->mpls.exp2                   = (GT_U32)inFields[13];
    rulePtr->mpls.numOfLabels            = (GT_U32)inFields[14];
    rulePtr->mpls.protocolAboveMPLS      = (GT_U32)inFields[15];
    rulePtr->mpls.common.dsaSrcIsTrunk   = (GT_BOOL)inFields[16];
    rulePtr->mpls.common.dsaSrcPortTrunk = (GT_U32)inFields[17];
    rulePtr->mpls.common.dsaSrcDevice    = (GT_U32)inFields[18];

    /* convert the trunk fields */
    ttiRuleTrunkInfoFromTestToCpss_1((GT_U32)inFields[1]);

    if (inFields[1] == 0) /* mask */
    {
        mask_set       = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_set       = GT_TRUE;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_KEY_ETH_Set
*
* DESCRIPTION:
*       This function maps input arguments to ETH TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_KEY_ETH_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_ETH_E;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr        = &maskData;
        mask_ruleIndex = (GT_U32)inFields[0];
    }
    else /* pattern */
    {
        rulePtr           = &patternData;
        pattern_ruleIndex = (GT_U32)inFields[0];
    }

    rulePtr->eth.common.pclId           = (GT_U32)inFields[2];
    rulePtr->eth.common.srcIsTrunk      = (GT_U32)inFields[3];
    rulePtr->eth.common.srcPortTrunk    = (GT_U32)inFields[4];

    if (rulePtr->eth.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->eth.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        rulePtr->eth.common.srcPortTrunk = (GT_U32)port;
    }

    galtisMacAddr(&rulePtr->eth.common.mac,(GT_U8*)inFields[5]);
    rulePtr->eth.common.vid             = (GT_U16)inFields[6];
    rulePtr->eth.common.isTagged        = (GT_BOOL)inFields[7];
    rulePtr->eth.up0                    = (GT_U32)inFields[8];
    rulePtr->eth.cfi0                   = (GT_U32)inFields[9];
    rulePtr->eth.isVlan1Exists          = (GT_BOOL)inFields[10];
    rulePtr->eth.vid1                   = (GT_U16)inFields[11];
    rulePtr->eth.up1                    = (GT_U32)inFields[12];
    rulePtr->eth.cfi1                   = (GT_U32)inFields[13];
    rulePtr->eth.etherType              = (GT_U32)inFields[14];
    rulePtr->eth.macToMe                = (GT_BOOL)inFields[15];
    rulePtr->eth.common.dsaSrcIsTrunk   = inFields[16];
    rulePtr->eth.common.dsaSrcPortTrunk = (GT_U32)inFields[17];
    rulePtr->eth.common.dsaSrcDevice    = (GT_U32)inFields[18];

    /* convert the trunk fields */
    ttiRuleTrunkInfoFromTestToCpss_1((GT_U32)inFields[1]);

    if (inFields[1] == 0) /* mask */
    {
        mask_set       = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_set       = GT_TRUE;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleSetFirst
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleSetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;


    switch (inArgs[1])
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleSetNext
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleSetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result = GT_OK;
    GT_U8                       devNum;
    GT_U32                      ruleIndex;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType;
    GT_U32                      actionIndex;

    CPSS_DXCH_TTI_ACTION_UNT    *actionPtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Check Validity */
    if (inFields[1] > 1)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be MASK or PATTERN).\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 0) && mask_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 1) && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inFields[0];

    switch (inArgs[1])
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (mask_set && pattern_set && (mask_ruleIndex != pattern_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }


    /* Get the action */
    ttiActionDatabaseGet(devNum,ruleIndex,&actionIndex);

    if ((mask_set && pattern_set) && (actionIndex != INVALID_RULE_INDEX))
    {
        actionPtr = &(ttiActionDatabase[actionIndex].actionEntry);

        keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];
        ttiRuleFormat[ruleIndex] = keyType;

        /* call cpss api function */
        result = pg_wrap_cpssDxChTtiRuleSet(devNum,ruleIndex,keyType,&patternData,&maskData,ttiActionDatabase[actionIndex].actionType,actionPtr,GT_FALSE);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
        cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleEndSet
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleEndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (mask_set || pattern_set)
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_KEY_IPV4_Get
*
* DESCRIPTION:
*       This function maps IPv4 TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_KEY_IPV4_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_IPV4_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->ipv4.common.pclId;
    inFields[3]  = rulePtr->ipv4.common.srcIsTrunk;
    inFields[4]  = rulePtr->ipv4.common.srcPortTrunk;

    if (rulePtr->ipv4.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->ipv4.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        inFields[4] = (GT_U32)port;
    }

    inFields[6]  = rulePtr->ipv4.common.vid;
    inFields[7]  = rulePtr->ipv4.common.isTagged;
    inFields[8]  = rulePtr->ipv4.tunneltype;
    inFields[11] = rulePtr->ipv4.isArp;
    inFields[12] = rulePtr->ipv4.common.dsaSrcIsTrunk;
    inFields[13] = rulePtr->ipv4.common.dsaSrcPortTrunk;
    inFields[14] = rulePtr->ipv4.common.dsaSrcDevice;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%4b%4b%d%d%d%d",
                inFields[0], inFields[1],  inFields[2], inFields[3],
                inFields[4], rulePtr->ipv4.common.mac.arEther, inFields[6],
                inFields[7], inFields[8], rulePtr->ipv4.srcIp.arIP,
                rulePtr->ipv4.destIp.arIP, inFields[11], inFields[12],inFields[13],inFields[14]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_KEY_MPLS_Get
*
* DESCRIPTION:
*       This function maps MPLS TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_KEY_MPLS_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MPLS_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->mpls.common.pclId;
    inFields[3]  = rulePtr->mpls.common.srcIsTrunk;
    inFields[4]  = rulePtr->mpls.common.srcPortTrunk;

    if (rulePtr->mpls.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->mpls.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        inFields[4] = (GT_U32)port;
    }

    inFields[6]  = rulePtr->mpls.common.vid;
    inFields[7]  = rulePtr->mpls.common.isTagged;
    inFields[8]  = rulePtr->mpls.label0;
    inFields[9]  = rulePtr->mpls.exp0 ;
    inFields[10] = rulePtr->mpls.label1;
    inFields[11] = rulePtr->mpls.exp1;
    inFields[12] = rulePtr->mpls.label2;
    inFields[13] = rulePtr->mpls.exp2;
    inFields[14] = rulePtr->mpls.numOfLabels;
    inFields[15] = rulePtr->mpls.protocolAboveMPLS;
    inFields[16] = rulePtr->mpls.common.dsaSrcIsTrunk;
    inFields[17] = rulePtr->mpls.common.dsaSrcPortTrunk;
    inFields[18] = rulePtr->mpls.common.dsaSrcDevice;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->mpls.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11], inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17],inFields[18]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;

}

/*******************************************************************************
* wrCpssDxChTtiRule_KEY_ETH_Get
*
* DESCRIPTION:
*       This function maps ETH TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_KEY_ETH_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_ETH_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->eth.common.pclId;
    inFields[3]  = rulePtr->eth.common.srcIsTrunk;
    inFields[4]  = rulePtr->eth.common.srcPortTrunk;

    if (rulePtr->eth.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->eth.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        inFields[4] = (GT_U32)port;
    }

    inFields[6]  = rulePtr->eth.common.vid;
    inFields[7]  = rulePtr->eth.common.isTagged;
    inFields[8]  = rulePtr->eth.up0;
    inFields[9]  = rulePtr->eth.cfi0;
    inFields[10] = rulePtr->eth.isVlan1Exists;
    inFields[11] = rulePtr->eth.vid1;
    inFields[12] = rulePtr->eth.up1;
    inFields[13] = rulePtr->eth.cfi1;
    inFields[14] = rulePtr->eth.etherType;
    inFields[15] = rulePtr->eth.macToMe;
    inFields[16] = rulePtr->eth.common.dsaSrcIsTrunk;
    inFields[17] = rulePtr->eth.common.dsaSrcPortTrunk;
    inFields[18] = rulePtr->eth.common.dsaSrcDevice;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->eth.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11],inFields[12],inFields[13], inFields[14],
                    inFields[15],inFields[16],inFields[17],inFields[18]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChTtiRuleGet
*
* DESCRIPTION:
*       This function gets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    GT_U32                      actionIndex;

    /* if isMask == GT_TRUE it is mask's turn (the first one to get), so there need to refresh the data
    else the data is updated and it is pattern's turn (was updated earlier)*/
    if (isMask)
    {
        /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];
        keyType = inArgs[1];

        actionIndex = INVALID_RULE_INDEX;
        for (; actionIndex == INVALID_RULE_INDEX && ruleIndex < 3072; ruleIndex++)
            ttiActionDatabaseGet(devNum,ruleIndex,&actionIndex);

        if (ruleIndex == 3072)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_OK, "%d", -1);

            return CMD_OK;
        }

        if (actionIndex == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_OK, "%d", -1);

            return CMD_OK;
        }

        ruleIndex--;

        /* call cpss api function */
        result = pg_wrap_cpssDxChTtiRuleGet(devNum,ruleIndex,keyType,&patternData,&maskData,ttiActionDatabase[actionIndex].actionType,&action,GT_FALSE);
        if (result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d", -1);
            return CMD_OK;
        }
    }

    switch (ttiRuleFormat[ruleIndex])
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_KEY_IPV4_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_KEY_MPLS_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_KEY_ETH_Get(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (isMask)
        ruleIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleGetFirst
*
* DESCRIPTION:
*       This function gets first TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ruleIndex = 0;

    /* first to get is mask */
    isMask = GT_TRUE;

    return wrCpssDxChTtiRuleGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_IPV4_Set
*
* DESCRIPTION:
*       This function maps input arguments to IPv4 TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_IPV4_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* IPv4 key in TtiRule_1 table is the same as in TtiRule table */
    return wrCpssDxChTtiRule_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_MPLS_Set
*
* DESCRIPTION:
*       This function maps input arguments to MPLS TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_MPLS_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* MPLS key in TtiRule_1 table is the same as in TtiRule table */
    return wrCpssDxChTtiRule_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_ETH_Set
*
* DESCRIPTION:
*       This function maps input arguments to ETH TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_ETH_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_ETH_E;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr        = &maskData;
        mask_ruleIndex = (GT_U32)inFields[0];
    }
    else /* pattern */
    {
        rulePtr           = &patternData;
        pattern_ruleIndex = (GT_U32)inFields[0];
    }

    rulePtr->eth.common.pclId           = (GT_U32)inFields[2];
    rulePtr->eth.common.srcIsTrunk      = (GT_U32)inFields[3];
    rulePtr->eth.common.srcPortTrunk    = (GT_U32)inFields[4];

    if (rulePtr->eth.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->eth.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        rulePtr->eth.common.srcPortTrunk = (GT_U32)port;
    }

    galtisMacAddr(&rulePtr->eth.common.mac,(GT_U8*)inFields[5]);
    rulePtr->eth.common.vid             = (GT_U16)inFields[6];
    rulePtr->eth.common.isTagged        = (GT_BOOL)inFields[7];
    rulePtr->eth.up0                    = (GT_U32)inFields[8];
    rulePtr->eth.cfi0                   = (GT_U32)inFields[9];
    rulePtr->eth.isVlan1Exists          = (GT_BOOL)inFields[10];
    rulePtr->eth.vid1                   = (GT_U16)inFields[11];
    rulePtr->eth.up1                    = (GT_U32)inFields[12];
    rulePtr->eth.cfi1                   = (GT_U32)inFields[13];
    rulePtr->eth.etherType              = (GT_U32)inFields[14];
    rulePtr->eth.macToMe                = (GT_BOOL)inFields[15];
    rulePtr->eth.common.dsaSrcIsTrunk   = inFields[16];
    rulePtr->eth.common.dsaSrcPortTrunk = (GT_U32)inFields[17];
    rulePtr->eth.common.dsaSrcDevice    = (GT_U32)inFields[18];
    rulePtr->eth.srcId                  = (GT_U32)inFields[19];
    rulePtr->eth.dsaQosProfile          = (GT_U32)inFields[20];

    /* convert the trunk fields */
    ttiRuleTrunkInfoFromTestToCpss_1((GT_U32)inFields[1]);


    if (inFields[1] == 0) /* mask */
    {
        mask_set       = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_set       = GT_TRUE;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_MIM_Set
*
* DESCRIPTION:
*       This function maps input arguments to MIM TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_MIM_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MIM_E;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr        = &maskData;
        mask_ruleIndex = (GT_U32)inFields[0];
    }
    else /* pattern */
    {
        rulePtr           = &patternData;
        pattern_ruleIndex = (GT_U32)inFields[0];
    }

    rulePtr->mim.common.pclId           = (GT_U32)inFields[2];
    rulePtr->mim.common.srcIsTrunk      = (GT_BOOL)inFields[3];
    rulePtr->mim.common.srcPortTrunk    = (GT_U32)inFields[4];

    if (rulePtr->mim.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->mim.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        rulePtr->mim.common.srcPortTrunk = (GT_U32)port;
    }

    galtisMacAddr(&rulePtr->mim.common.mac,(GT_U8*)inFields[5]);
    rulePtr->mim.common.vid             = (GT_U16)inFields[6];
    rulePtr->mim.common.isTagged        = (GT_BOOL)inFields[7];
    rulePtr->mim.common.dsaSrcIsTrunk   = (GT_BOOL)inFields[8];
    rulePtr->mim.common.dsaSrcPortTrunk = (GT_U32)inFields[9];
    rulePtr->mim.common.dsaSrcDevice    = (GT_U32)inFields[10];
    rulePtr->mim.bUp                    = (GT_U32)inFields[11];
    rulePtr->mim.bDp                    = (GT_U32)inFields[12];
    rulePtr->mim.iSid                   = (GT_U32)inFields[13];
    rulePtr->mim.iUp                    = (GT_U32)inFields[14];
    rulePtr->mim.iDp                    = (GT_U32)inFields[15];
    rulePtr->mim.iRes1                  = (GT_U32)inFields[16];
    rulePtr->mim.iRes2                  = (GT_U32)inFields[17];

    /* convert the trunk fields */
    ttiRuleTrunkInfoFromTestToCpss_1((GT_U32)inFields[1]);


    if (inFields[1] == 0) /* mask */
    {
        mask_set       = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_set       = GT_TRUE;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1SetFirst
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1SetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;

    switch (inArgs[3]) /* key type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_1_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_1_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_1_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_1_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1SetNext
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1SetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;
    GT_U8                           devNum;
    GT_U32                          ruleIndex;
    CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType;
    GT_U32                          actionIndex;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType;
    CPSS_DXCH_TTI_ACTION_UNT        *actionPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Check Validity */
    if (inFields[1] > 1)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be MASK or PATTERN).\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 0) && mask_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 1) && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inFields[0];

    switch (inArgs[3]) /* key type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_1_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_1_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_1_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_1_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (mask_set && pattern_set && (mask_ruleIndex != pattern_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }


    /* Get the action */
    ttiActionDatabaseGet(devNum,ruleIndex,&actionIndex);

    if ((mask_set && pattern_set) && (actionIndex != INVALID_RULE_INDEX))
    {
        actionPtr                    = &(ttiActionDatabase[actionIndex].actionEntry);
        keyType                      = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[3];
        ttiRuleFormat[ruleIndex]     = keyType;
        actionType                   = ttiActionDatabase[actionIndex].actionType;
        ttiRuleActionType[ruleIndex] = actionType;

        /* call cpss api function */
        result = pg_wrap_cpssDxChTtiRuleSet(devNum,ruleIndex,keyType,&patternData,&maskData,actionType,actionPtr,GT_FALSE);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
        cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1EndSet
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1EndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (mask_set || pattern_set)
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_IPV4_Get
*
* DESCRIPTION:
*       This function maps IPv4 TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_IPV4_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* IPv4 key in TtiRule_1 table is the same as in TtiRule table */
    return wrCpssDxChTtiRule_KEY_IPV4_Get(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_MPLS_Get
*
* DESCRIPTION:
*       This function maps MPLS TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_MPLS_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* MPLS key in TtiRule_1 table is the same as in TtiRule table */
    return wrCpssDxChTtiRule_KEY_MPLS_Get(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_ETH_Get
*
* DESCRIPTION:
*       This function maps ETH TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_ETH_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_ETH_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->eth.common.pclId;
    inFields[3]  = rulePtr->eth.common.srcIsTrunk;
    inFields[4]  = rulePtr->eth.common.srcPortTrunk;

    if (rulePtr->eth.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->eth.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        inFields[4] = (GT_U32)port;
    }

    inFields[6]  = rulePtr->eth.common.vid;
    inFields[7]  = rulePtr->eth.common.isTagged;
    inFields[8]  = rulePtr->eth.up0;
    inFields[9]  = rulePtr->eth.cfi0;
    inFields[10] = rulePtr->eth.isVlan1Exists;
    inFields[11] = rulePtr->eth.vid1;
    inFields[12] = rulePtr->eth.up1;
    inFields[13] = rulePtr->eth.cfi1;
    inFields[14] = rulePtr->eth.etherType;
    inFields[15] = rulePtr->eth.macToMe;
    inFields[16] = rulePtr->eth.common.dsaSrcIsTrunk;
    inFields[17] = rulePtr->eth.common.dsaSrcPortTrunk;
    inFields[18] = rulePtr->eth.common.dsaSrcDevice;
    inFields[19] = rulePtr->eth.srcId;
    inFields[20] = rulePtr->eth.dsaQosProfile;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->eth.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11],inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17],inFields[18],
                inFields[19],inFields[20]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1_KEY_MIM_Get
*
* DESCRIPTION:
*       This function maps MIM TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1_KEY_MIM_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MIM_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2] = rulePtr->mim.common.pclId;
    inFields[3] = rulePtr->mim.common.srcIsTrunk;
    inFields[4] = rulePtr->mim.common.srcPortTrunk;

    if (rulePtr->mim.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->mim.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        inFields[4] = (GT_U32)port;
    }

    inFields[6] = rulePtr->mim.common.vid;
    inFields[7] = rulePtr->mim.common.isTagged;
    inFields[8] = rulePtr->mim.common.dsaSrcIsTrunk;
    inFields[9] = rulePtr->mim.common.dsaSrcPortTrunk;
    inFields[10] = rulePtr->mim.common.dsaSrcDevice;
    inFields[11] = rulePtr->mim.bUp;
    inFields[12] = rulePtr->mim.bDp;
    inFields[13] = rulePtr->mim.iSid;
    inFields[14] = rulePtr->mim.iUp;
    inFields[15] = rulePtr->mim.iDp;
    inFields[16] = rulePtr->mim.iRes1;
    inFields[17] = rulePtr->mim.iRes2;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->eth.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11],inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1Get
*
* DESCRIPTION:
*       This function gets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    GT_BOOL                     ruleValid;

    /* if isMask == GT_TRUE it is mask's turn (the first one to get), so there need to refresh the data
    else the data is updated and it is pattern's turn (was updated earlier)*/
    if (isMask)
    {
        /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];

        /* look for another valid rule within the range */
        for ((ruleValid = GT_FALSE) ; ((ruleValid == GT_FALSE) && (ruleIndex < 3072) && (ruleIndex <= ruleIndexMaxGet)) ; ruleIndex++)
        {
            /* check if the rule is valid */
            result = pg_wrap_cpssDxChTtiRuleValidStatusGet(devNum,ruleIndex,&ruleValid,GT_FALSE);
            if (result != GT_OK)
            {
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }
        }

        /* check if no valid rule was found in the requested range */
        if ((ruleValid == GT_FALSE))
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        /* valid rule was found in the requested range */
        ruleIndex--;

        /* call cpss api function, note that it is assumed that rule action
           type is not modified after setting the rule */
        result = pg_wrap_cpssDxChTtiRuleGet(devNum,ruleIndex,ttiRuleFormat[ruleIndex],&patternData,&maskData,ttiRuleActionType[ruleIndex],&action,GT_FALSE);
    }

    switch (ttiRuleFormat[ruleIndex])
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_1_KEY_IPV4_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_1_KEY_MPLS_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_1_KEY_ETH_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_1_KEY_MIM_Get(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (isMask)
        ruleIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_1GetFirst
*
* DESCRIPTION:
*       This function gets first TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_1GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ruleIndex = 0;

    /* first to get is mask */
    isMask = GT_TRUE;

    /* check if the application request 0 entries */
    if (inArgs[2] == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    ruleIndex       = (GT_U32)inArgs[1];
    ruleIndexMaxGet = ruleIndex + (GT_U32)inArgs[2] - 1;

    return wrCpssDxChTtiRule_1Get(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChTtiMacModeSet
*
* DESCRIPTION:
*       This function sets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IP_V4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*       macMode       - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMacModeSet
(
IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT keyType;
    CPSS_DXCH_TTI_MAC_MODE_ENT macMode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];
    macMode = (CPSS_DXCH_TTI_MAC_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiMacModeSet(devNum, keyType, macMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMacModeGet
*
* DESCRIPTION:
*       This function gets the lookup Mac mode for the specified key type.
*       This setting controls the Mac that would be used for key generation
*       (Source/Destination).
*
* APPLICABLE DEVICES:
*       All DXCH devices.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IP_V4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*
* OUTPUTS:
*       macModePtr    - MAC mode to use; valid values:
*                           CPSS_DXCH_TTI_MAC_MODE_DA_E
*                           CPSS_DXCH_TTI_MAC_MODE_SA_E
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or key type
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMacModeGet
(
IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT keyType;
    CPSS_DXCH_TTI_MAC_MODE_ENT macMode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiMacModeGet(devNum, keyType, &macMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", macMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPclIdSet
*
* DESCRIPTION:
*       This function sets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E
*       pclId         - PCL ID value (10 bits)
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
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPclIdSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType;
    GT_U32                      pclId;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum  = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];
    pclId   = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiPclIdSet(devNum,keyType,pclId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPclIdGet
*
* DESCRIPTION:
*       This function gets the PCL ID for the specified key type. The PCL ID
*       is used to distinguish between different TTI keys in the TCAM.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum        - device number
*       keyType       - TTI key type; valid values:
*                           CPSS_DXCH_TTI_KEY_IPV4_E
*                           CPSS_DXCH_TTI_KEY_MPLS_E
*                           CPSS_DXCH_TTI_KEY_ETH_E
*                           CPSS_DXCH_TTI_KEY_MIM_E
*
* OUTPUTS:
*       pclIdPtr      - (points to) PCL ID value (10 bits)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The TTI PCL ID values are only relevant for the TTI TCAM and are not
*       related to the PCL ID values in the PCL TCAM.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPclIdGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType;
    GT_U32                      pclId;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum  = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiPclIdGet(devNum,keyType,&pclId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", pclId);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiIpv4GreEthTypeSet
*
* DESCRIPTION:
*       This function sets the IPv4 GRE protocol Ethernet type. Two Ethernet
*       types are supported, used by TTI for Ethernet over GRE tunnels
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type; valid values:
*                           CPSS_DXCH_TTI_IPV4_GRE0_E
*                           CPSS_DXCH_TTI_IPV4_GRE1_E
*       ethType       - Ethernet type value (range 16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiIpv4GreEthTypeSet
(
IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_TUNNEL_ETHERTYPE_TYPE_ENT  greTunnelType;
    GT_U32 ethType;
    GT_U32 tempGreTunnelType;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tempGreTunnelType = (GT_U32)inArgs[1]; /* need to convert from old enum */
    ethType = (GT_U32)inArgs[2];

    (tempGreTunnelType == 0) ?
        (greTunnelType = CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE0_E) : (greTunnelType = CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE1_E);

    /* call cpss api function */
    result = cpssDxChTtiEthernetTypeSet(devNum,greTunnelType,ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiIpv4GreEthTypeGet
*
* DESCRIPTION:
*       This function gets the IPv4 GRE protocol Ethernet type. Two Ethernet
*       types are supported, used by TTI for Ethernet over GRE tunnels
*       (relevant only to IPv4 lookup keys).
*
* APPLICABLE DEVICES:
*       All DXCH3 devices.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type; valid values:
*                           CPSS_DXCH_TTI_IPV4_GRE0_E
*                           CPSS_DXCH_TTI_IPV4_GRE1_E
*
* OUTPUTS:
*       ethTypePtr    - points to Ethernet type value
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or GRE tunnel type
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiIpv4GreEthTypeGet
(
IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_TUNNEL_ETHERTYPE_TYPE_ENT greTunnelType;
    GT_U32 ethType;
    GT_U32 tempGreTunnelType;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tempGreTunnelType = (GT_U32)inArgs[1]; /* need to convert from old enum */

    (tempGreTunnelType == 0) ?
        (greTunnelType = CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE0_E) : (greTunnelType = CPSS_TUNNEL_ETHERTYPE_TYPE_IPV4_GRE1_E);

    /* call cpss api function */
    result = cpssDxChTtiEthernetTypeGet(devNum, greTunnelType, &ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ethType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiEthernetTypeSet
*
* DESCRIPTION:
*       This function sets the TTI Ethernet type value that is used
*       to identify packets for TTI triggering.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type
*       ethType       - Ethernet type value (range 16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong value in any of the parameters
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiEthernetTypeSet
(
IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_TUNNEL_ETHERTYPE_TYPE_ENT  greTunnelType;
    GT_U32 ethType;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    greTunnelType = (GT_U32)inArgs[1];
    ethType = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiEthernetTypeSet(devNum,greTunnelType,ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiEthernetTypeGet
*
* DESCRIPTION:
*       This function gets the TTI Ethernet type value that is used
*       to identify packets for TTI triggering.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       greTunnelType - GRE tunnel type
*
* OUTPUTS:
*       ethTypePtr    - points to Ethernet type value
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - wrong device id or GRE tunnel type
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiEthernetTypeGet
(
IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_TUNNEL_ETHERTYPE_TYPE_ENT greTunnelType;
    GT_U32 ethType;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    greTunnelType = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiEthernetTypeGet(devNum, greTunnelType, &ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ethType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMimEthTypeSet
*
* DESCRIPTION:
*       This function sets the MIM Ethernet type.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum        - device number
*       ethType       - Ethernet type value (range 16 bits)
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
*      specialEthertypes is used to identify tunnel for incoming packets in
*      the TTI.
*      routerHdrAltIEthertype is used for outgoing tunnel in the header alteration.
*      Those registers are configured to have the same value.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMimEthTypeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ethType;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ethType = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiEthernetTypeSet(devNum, CPSS_TUNNEL_ETHERTYPE_TYPE_MIM_E, ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMimEthTypeGet
*
* DESCRIPTION:
*       This function gets the MIM Ethernet type.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       ethTypePtr    - points to Ethernet type value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMimEthTypeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ethType;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTtiEthernetTypeGet(devNum, CPSS_TUNNEL_ETHERTYPE_TYPE_MIM_E, &ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ethType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMplsEthTypeSet
*
* DESCRIPTION:
*       This function sets the MPLS Ethernet type.
*
* APPLICABLE DEVICES: DxChXcat A1 and above.
*
* INPUTS:
*       devNum        - device number
*       ucMcSet       - weather to set it for unicast packets or multicast.
*       ethType       - Ethernet type value (range 16 bits)
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
*      mplsEthertypes is used to identify tunnel for incoming packets in
*      the TTI.
*      hdrAltMplsEthertype is used for outgoing tunnel in the header alteration.
*      Those registers are configured to have the same value.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMplsEthTypeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ethType;
    CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet;
    CPSS_TUNNEL_ETHERTYPE_TYPE_ENT  mplsTunnelType;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ucMcSet = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[1];
    ethType = (GT_U32)inArgs[2];

    (ucMcSet == CPSS_IP_UNICAST_E) ?
        (mplsTunnelType = CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_UNICAST_E) : (mplsTunnelType = CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_MULTICAST_E);

    /* call cpss api function */
    result = cpssDxChTtiEthernetTypeSet(devNum,mplsTunnelType,ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMplsEthTypeGet
*
* DESCRIPTION:
*       This function gets the MPLS Ethernet type.
*
* APPLICABLE DEVICES: DxChXcat A1 and above.
*
* INPUTS:
*       devNum        - device number
*       ucMcSet       - weather to get it for unicast packets or multicast.
*
* OUTPUTS:
*       ethTypePtr    - points to Ethernet type value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMplsEthTypeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ethType;
    CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet;
    CPSS_TUNNEL_ETHERTYPE_TYPE_ENT  mplsTunnelType;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ucMcSet = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[1];

    (ucMcSet == CPSS_IP_UNICAST_E) ?
        (mplsTunnelType = CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_UNICAST_E) : (mplsTunnelType = CPSS_TUNNEL_ETHERTYPE_TYPE_MPLS_MULTICAST_E);

    /* call cpss api function */
    result = cpssDxChTtiEthernetTypeGet(devNum,mplsTunnelType,&ethType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ethType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiExceptionCmdSet
*
* DESCRIPTION:
*       Set tunnel termination exception command.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum        - device number
*       exceptionType - tunnel termination exception type to set command for
*       command       - command to set
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*
* COMMENTS:
*       Commands for the different exceptions are:
*
*       CPSS_DXCH_TTI_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiExceptionCmdSet
(
IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_TTI_EXCEPTION_ENT exceptionType;
    CPSS_PACKET_CMD_ENT command;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_DXCH_TTI_EXCEPTION_ENT)inArgs[1];
    command = (CPSS_PACKET_CMD_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiExceptionCmdSet(devNum, exceptionType, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiExceptionCmdGet
*
* DESCRIPTION:
*       Get tunnel termination exception command.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set command for
*
* OUTPUTS:
*       commandPtr    - points to the command for the exception type
*
* RETURNS:
*       GT_OK       - on success.
*       GT_BAD_PTR  - on NULL pointer
*       GT_FAIL     - on error.
*
* COMMENTS:
*       Commands for the different exceptions are:
*
**       CPSS_DXCH_TTI_IPV4_HEADER_ERROR_E       -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_IPV4_OPTION_FRAG_ERROR_E  -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*       CPSS_DXCH_TTI_IPV4_UNSUP_GRE_ERROR_E    -
*                               CPSS_PACKET_CMD_TRAP_TO_CPU_E
*                               CPSS_PACKET_CMD_DROP_HARD_E
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiExceptionCmdGet
(
IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
IN  GT_32 numFields,
OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_TTI_EXCEPTION_ENT exceptionType;
    CPSS_PACKET_CMD_ENT command;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_DXCH_TTI_EXCEPTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiExceptionCmdGet(devNum, exceptionType, &command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", command);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChTtiRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM (0..3k-1).
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleValidStatusSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    IN GT_U8   devNum;
    IN GT_U32  routerTtiTcamRow;
    IN GT_BOOL valid;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum           = (GT_U8)inArgs[0];
    routerTtiTcamRow = (GT_U32)inArgs[1];
    valid            = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiRuleValidStatusSet(devNum, routerTtiTcamRow, valid, GT_FALSE);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiRuleValidStatusSet
*
* DESCRIPTION:
*       This function validates / invalidates the rule in TCAM.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM (0..3k-1).
*                           This is an absolute index.
*       valid             - GT_TRUE - valid, GT_FALSE - invalid
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleValidStatus_2Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    IN GT_U8   devNum;
    IN GT_U32  routerTtiTcamRow;
    IN GT_BOOL valid;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum           = (GT_U8)inArgs[0];
    routerTtiTcamRow = (GT_U32)inArgs[1];
    valid            = (GT_BOOL)inArgs[2];

    /* call cpss api function - routerTtiTcamRow is an absolute index */
    result = pg_wrap_cpssDxChTtiRuleValidStatusSet(devNum, routerTtiTcamRow, valid, GT_TRUE);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiRuleValidStatusGet
*
* DESCRIPTION:
*       This function returns the valid status of the rule in TCAM
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM (0..3k-1).
*
* OUTPUTS:
*       validPtr          - GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleValidStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    IN  GT_U8   devNum;
    IN  GT_U32  routerTtiTcamRow;
    OUT GT_BOOL valid;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum           = (GT_U8)inArgs[0];
    routerTtiTcamRow = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiRuleValidStatusGet(devNum, routerTtiTcamRow, &valid, GT_FALSE);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", valid);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiRuleValidStatusGet
*
* DESCRIPTION:
*       This function returns the valid status of the rule in TCAM
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum            - device number
*       routerTtiTcamRow  - Index of the tunnel termination entry in the
*                           the router / tunnel termination TCAM (0..3k-1).
*                           This is an absolute index.
*
* OUTPUTS:
*       validPtr          - GT_TRUE - valid, GT_FALSE - invalid
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleValidStatus_2Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    IN  GT_U8   devNum;
    IN  GT_U32  routerTtiTcamRow;
    OUT GT_BOOL valid;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum           = (GT_U8)inArgs[0];
    routerTtiTcamRow = (GT_U32)inArgs[1];

    /* call cpss api function - routerTtiTcamRow is an absolute index */
    result = pg_wrap_cpssDxChTtiRuleValidStatusGet(devNum, routerTtiTcamRow, &valid, GT_TRUE);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", valid);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_IPV4_Set
*
* DESCRIPTION:
*       This function maps input arguments to IPv4 TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_IPV4_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr = &maskData;
    }
    else /* pattern */
    {
        rulePtr = &patternData;
    }

    /* IPv4 key in TtiRule_2 table is the same as in TtiRule table except for field sourcePortGroupId */
    wrCpssDxChTtiRule_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
    rulePtr->ipv4.common.sourcePortGroupId = (GT_U32)inFields[15];

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_MPLS_Set
*
* DESCRIPTION:
*       This function maps input arguments to MPLS TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_MPLS_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr = &maskData;
    }
    else /* pattern */
    {
        rulePtr = &patternData;
    }

    /* MPLS key in TtiRule_2 table is the same as in TtiRule table except for field sourcePortGroupId */
    wrCpssDxChTtiRule_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
    rulePtr->mpls.common.sourcePortGroupId = (GT_U32)inFields[19];

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_ETH_Set
*
* DESCRIPTION:
*       This function maps input arguments to ETH TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_ETH_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr = &maskData;
    }
    else /* pattern */
    {
        rulePtr = &patternData;
    }

    /* ETH key in TtiRule_2 table is the same as in TtiRule_1 table except for field sourcePortGroupId */
    wrCpssDxChTtiRule_1_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
    rulePtr->eth.common.sourcePortGroupId = (GT_U32)inFields[21];

    rulePtr->eth.eTagGrp = (GT_U32)inFields[22];

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_MIM_Set
*
* DESCRIPTION:
*       This function maps input arguments to MIM TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_MIM_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr = &maskData;
    }
    else /* pattern */
    {
        rulePtr = &patternData;
    }

    /* MIM key in TtiRule_2 table is the same as in TtiRule_1 table except for field sourcePortGroupId */
    wrCpssDxChTtiRule_1_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
    rulePtr->mim.common.sourcePortGroupId = (GT_U32)inFields[18];

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2SetFirst
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2SetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;

    switch (inArgs[3]) /* key type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_2_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_2_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2SetNext
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2SetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;
    GT_U8                           devNum;
    GT_U32                          ruleIndex;
    CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType;
    GT_U32                          actionIndex;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType;
    CPSS_DXCH_TTI_ACTION_UNT        *actionPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Check Validity */
    if (inFields[1] > 1)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be MASK or PATTERN).\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 0) && mask_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 1) && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inFields[0];

    switch (inArgs[3]) /* key type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_2_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_2_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (mask_set && pattern_set && (mask_ruleIndex != pattern_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }


    /* Get the action */
    ttiActionDatabaseGet(devNum,ruleIndex,&actionIndex);

    if ((mask_set && pattern_set) && (actionIndex != INVALID_RULE_INDEX))
    {
        actionPtr                    = &(ttiActionDatabase[actionIndex].actionEntry);
        keyType                      = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[3];
        ttiRuleFormat[ruleIndex]     = keyType;
        actionType                   = ttiActionDatabase[actionIndex].actionType;
        ttiRuleActionType[ruleIndex] = actionType;

        /* call cpss api function */
        result = pg_wrap_cpssDxChTtiRuleSet(devNum,ruleIndex,keyType,&patternData,&maskData,actionType,actionPtr,GT_FALSE);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
        cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2EndSet
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2EndSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    if (mask_set || pattern_set)
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2CancelSet
*
* DESCRIPTION:
*       This function is called in case of error in TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2CancelSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : result is not GT_OK\n");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_IPV4_Get
*
* DESCRIPTION:
*       This function maps IPv4 TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_IPV4_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_IPV4_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->ipv4.common.pclId;
    inFields[3]  = rulePtr->ipv4.common.srcIsTrunk;
    inFields[4]  = rulePtr->ipv4.common.srcPortTrunk;

    if (rulePtr->ipv4.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->ipv4.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        inFields[4] = (GT_U32)port;
    }

    inFields[6]  = rulePtr->ipv4.common.vid;
    inFields[7]  = rulePtr->ipv4.common.isTagged;
    inFields[8]  = rulePtr->ipv4.tunneltype;
    inFields[11] = rulePtr->ipv4.isArp;
    inFields[12] = rulePtr->ipv4.common.dsaSrcIsTrunk;
    inFields[13] = rulePtr->ipv4.common.dsaSrcPortTrunk;
    inFields[14] = rulePtr->ipv4.common.dsaSrcDevice;
    inFields[15] = rulePtr->ipv4.common.sourcePortGroupId;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%4b%4b%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2], inFields[3],
                inFields[4], rulePtr->ipv4.common.mac.arEther, inFields[6],
                inFields[7], inFields[8], rulePtr->ipv4.srcIp.arIP,
                rulePtr->ipv4.destIp.arIP, inFields[11], inFields[12],
                inFields[13],inFields[14],inFields[15]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_MPLS_Get
*
* DESCRIPTION:
*       This function maps MPLS TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_MPLS_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MPLS_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->mpls.common.pclId;
    inFields[3]  = rulePtr->mpls.common.srcIsTrunk;
    inFields[4]  = rulePtr->mpls.common.srcPortTrunk;

    if (rulePtr->mpls.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->mpls.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        inFields[4] = (GT_U32)port;
    }

    inFields[6]  = rulePtr->mpls.common.vid;
    inFields[7]  = rulePtr->mpls.common.isTagged;
    inFields[8]  = rulePtr->mpls.label0;
    inFields[9]  = rulePtr->mpls.exp0 ;
    inFields[10] = rulePtr->mpls.label1;
    inFields[11] = rulePtr->mpls.exp1;
    inFields[12] = rulePtr->mpls.label2;
    inFields[13] = rulePtr->mpls.exp2;
    inFields[14] = rulePtr->mpls.numOfLabels;
    inFields[15] = rulePtr->mpls.protocolAboveMPLS;
    inFields[16] = rulePtr->mpls.common.dsaSrcIsTrunk;
    inFields[17] = rulePtr->mpls.common.dsaSrcPortTrunk;
    inFields[18] = rulePtr->mpls.common.dsaSrcDevice;
    inFields[19] = rulePtr->mpls.common.sourcePortGroupId;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->mpls.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11], inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17],inFields[18],inFields[19]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_ETH_Get
*
* DESCRIPTION:
*       This function maps ETH TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_ETH_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_ETH_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->eth.common.pclId;
    inFields[3]  = rulePtr->eth.common.srcIsTrunk;
    inFields[4]  = rulePtr->eth.common.srcPortTrunk;

    if (rulePtr->eth.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->eth.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        inFields[4] = (GT_U32)port;
    }

    inFields[6]  = rulePtr->eth.common.vid;
    inFields[7]  = rulePtr->eth.common.isTagged;
    inFields[8]  = rulePtr->eth.up0;
    inFields[9]  = rulePtr->eth.cfi0;
    inFields[10] = rulePtr->eth.isVlan1Exists;
    inFields[11] = rulePtr->eth.vid1;
    inFields[12] = rulePtr->eth.up1;
    inFields[13] = rulePtr->eth.cfi1;
    inFields[14] = rulePtr->eth.etherType;
    inFields[15] = rulePtr->eth.macToMe;
    inFields[16] = rulePtr->eth.common.dsaSrcIsTrunk;
    inFields[17] = rulePtr->eth.common.dsaSrcPortTrunk;
    inFields[18] = rulePtr->eth.common.dsaSrcDevice;
    inFields[19] = rulePtr->eth.srcId;
    inFields[20] = rulePtr->eth.dsaQosProfile;
    inFields[21] = rulePtr->eth.common.sourcePortGroupId;
    inFields[22] = rulePtr->eth.eTagGrp;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->eth.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11],inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17],inFields[18],
                inFields[19],inFields[20], inFields[21], inFields[22]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2_KEY_MIM_Get
*
* DESCRIPTION:
*       This function maps MIM TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2_KEY_MIM_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MIM_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2] = rulePtr->mim.common.pclId;
    inFields[3] = rulePtr->mim.common.srcIsTrunk;
    inFields[4] = rulePtr->mim.common.srcPortTrunk;

    if (rulePtr->mim.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->mim.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        inFields[4] = (GT_U32)port;
    }

    inFields[6] = rulePtr->mim.common.vid;
    inFields[7] = rulePtr->mim.common.isTagged;
    inFields[8] = rulePtr->mim.common.dsaSrcIsTrunk;
    inFields[9] = rulePtr->mim.common.dsaSrcPortTrunk;
    inFields[10] = rulePtr->mim.common.dsaSrcDevice;
    inFields[11] = rulePtr->mim.bUp;
    inFields[12] = rulePtr->mim.bDp;
    inFields[13] = rulePtr->mim.iSid;
    inFields[14] = rulePtr->mim.iUp;
    inFields[15] = rulePtr->mim.iDp;
    inFields[16] = rulePtr->mim.iRes1;
    inFields[17] = rulePtr->mim.iRes2;
    inFields[18] = rulePtr->mim.common.sourcePortGroupId;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->eth.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11],inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17],inFields[18]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_2Get
*
* DESCRIPTION:
*       This function gets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    GT_BOOL                     ruleValid;

    /* if isMask == GT_TRUE it is mask's turn (the first one to get), so there need to refresh the data
    else the data is updated and it is pattern's turn (was updated earlier)*/
    if (isMask)
    {
        /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];

        /* look for another valid rule within the range */
        for ((ruleValid = GT_FALSE) ; ((ruleValid == GT_FALSE) && (ruleIndex < 3072) && (ruleIndex <= ruleIndexMaxGet)) ; ruleIndex++)
        {
            /* check if the rule is valid */
            result = pg_wrap_cpssDxChTtiRuleValidStatusGet(devNum,ruleIndex,&ruleValid,GT_FALSE);
            if (result != GT_OK)
            {
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }
        }

        /* check if no valid rule was found in the requested range */
        if ((ruleValid == GT_FALSE))
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        /* valid rule was found in the requested range */
        ruleIndex--;

        /* call cpss api function, note that it is assumed that rule action
           type is not modified after setting the rule */
        result = pg_wrap_cpssDxChTtiRuleGet(devNum,ruleIndex,ttiRuleFormat[ruleIndex],&patternData,&maskData,ttiRuleActionType[ruleIndex],&action,GT_FALSE);
    }

    switch (ttiRuleFormat[ruleIndex])
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_2_KEY_MPLS_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_2_KEY_MIM_Get(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (isMask)
        ruleIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleActionHwGet
*
* DESCRIPTION:
*   The function gets the Rule Action
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum          - device number
*       ruleIndex       - index of the rule in the TCAM.
*
* OUTPUTS:
*       action          - Policy rule action that applied on packet if packet's
*                          search key matched with masked pattern.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - on wrong parameters
*       GT_HW_ERROR    - on hardware error
*
* COMMENTS:
*       NONE
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleActionHwGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    GT_HW_DEV_NUM               tempHwDev;    /* used for port,dev converting */
    GT_PORT_NUM                 tempPort;   /* used for port,dev converting */
    CPSS_DXCH_TTI_KEY_TYPE_ENT  keyType;

    /* check if requested refresh range is already refreshed */
    if (ruleIndex > ruleIndexMaxGet)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* to support the not seted/geted members of structures */
    cpssOsMemSet(&action, 0, sizeof(action));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    keyType= (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[3];


    /* call cpss api function, note that it is assumed that rule action
    type is not modified after setting the rule */

    result = pg_wrap_cpssDxChTtiRuleGet(devNum,ruleIndex,keyType,&patternData,&maskData,CPSS_DXCH_TTI_ACTION_TYPE2_ENT,&action,GT_FALSE);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* CPSS_DXCH_TTI_ACTION_TYPE2_ENT */

        inArgs[0] = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;
        inFields[0]  = devNum;
        inFields[1]  = ruleIndex;
        inFields[2]  = action.type2.tunnelTerminate;
        inFields[3]  = action.type2.ttPassengerPacketType;
        inFields[4]  = action.type2.tsPassengerPacketType;
        inFields[5]  = action.type2.copyTtlExpFromTunnelHeader;
        inFields[6]  = action.type2.mplsCommand;
        inFields[7]  = action.type2.mplsTtl;
        inFields[8]  = action.type2.enableDecrementTtl;
        inFields[9]  = action.type2.command;
        inFields[10] = action.type2.redirectCommand;
        inFields[11] = action.type2.egressInterface.type;
        tempHwDev      = action.type2.egressInterface.devPort.hwDevNum;
        tempPort     = action.type2.egressInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempHwDev,tempPort);
        inFields[12] = tempHwDev;
        inFields[13] = tempPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(action.type2.egressInterface.trunkId);
        inFields[14] = action.type2.egressInterface.trunkId;
        inFields[15] = action.type2.egressInterface.vidx;
        inFields[16] = action.type2.egressInterface.vlanId;
        inFields[17] = action.type2.arpPtr;
        inFields[18] = action.type2.tunnelStart;
        inFields[19] = action.type2.tunnelStartPtr;
        inFields[20] = action.type2.routerLttPtr;
        inFields[21] = action.type2.vrfId;
        inFields[22] = action.type2.sourceIdSetEnable;
        inFields[23] = action.type2.sourceId;
        inFields[24] = action.type2.tag0VlanCmd;
        inFields[25] = action.type2.tag0VlanId;
        inFields[26] = action.type2.tag1VlanCmd;
        inFields[27] = action.type2.tag1VlanId;
        inFields[28] = action.type2.tag0VlanPrecedence;
        inFields[29] = action.type2.nestedVlanEnable;
        inFields[30] = action.type2.bindToPolicerMeter;
        inFields[31] = action.type2.bindToPolicer;
        inFields[32] = action.type2.policerIndex;
        inFields[33] = action.type2.qosPrecedence;
        inFields[34] = action.type2.keepPreviousQoS;
        inFields[35] = action.type2.trustUp;
        inFields[36] = action.type2.trustDscp;
        inFields[37] = action.type2.trustExp;
        inFields[38] = action.type2.qosProfile;
        inFields[39] = action.type2.modifyTag0Up;
        inFields[40] = action.type2.tag1UpCommand;
        inFields[41] = action.type2.modifyDscp;
        inFields[42] = action.type2.tag0Up;
        inFields[43] = action.type2.tag1Up;
        inFields[44] = action.type2.remapDSCP;
        inFields[45] = action.type2.pcl0OverrideConfigIndex;
        inFields[46] = action.type2.pcl0_1OverrideConfigIndex;
        inFields[47] = action.type2.pcl1OverrideConfigIndex;
        inFields[48] = action.type2.iPclConfigIndex;
        inFields[49] = action.type2.mirrorToIngressAnalyzerEnable;
        inFields[50] = action.type2.userDefinedCpuCode;
        inFields[51] = action.type2.bindToCentralCounter;
        inFields[52] = action.type2.centralCounterIndex;
        inFields[53] = action.type2.vntl2Echo;
        inFields[54] = action.type2.bridgeBypass;
        inFields[55] = action.type2.ingressPipeBypass;
        inFields[56] = action.type2.actionStop;
        inFields[57] = action.type2.hashMaskIndex;
        inFields[58] = action.type2.modifyMacSa;
        inFields[59] = action.type2.modifyMacDa;
        inFields[60] = action.type2.ResetSrcPortGroupId;
        inFields[61] = action.type2.multiPortGroupTtiEnable;

        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],   inFields[2],   inFields[3],
                    inFields[4],  inFields[5],   inFields[6],   inFields[7],
                    inFields[8],  inFields[9],   inFields[10],  inFields[11],
                    inFields[12], inFields[13],  inFields[14],  inFields[15],
                    inFields[16], inFields[17],  inFields[18],  inFields[19],
                    inFields[20], inFields[21],  inFields[22],  inFields[23],
                    inFields[24], inFields[25],  inFields[26],  inFields[27],
                    inFields[28], inFields[29],  inFields[30],  inFields[31],
                    inFields[32], inFields[33],  inFields[34],  inFields[35],
                    inFields[36], inFields[37],  inFields[38],  inFields[39],
                    inFields[40], inFields[41],  inFields[42],  inFields[43],
                    inFields[44], inFields[45],  inFields[46],  inFields[47],
                    inFields[48], inFields[49],  inFields[50],  inFields[51],
                    inFields[52], inFields[53],  inFields[54],  inFields[55],
                    inFields[56], inFields[57],  inFields[58],  inFields[59],
                    inFields[60], inFields[61]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d%f", 1);
       /* break;*/

        ruleIndex++;
        return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleActionHwGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /*ttiActionGetIndex = 0;*/
    ruleIndex=0;

    /* check if the application request 0 entries */
    if (inArgs[2] == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    ruleIndex       = (GT_U32)inArgs[1];
    ruleIndexMaxGet = ruleIndex + (GT_U32)inArgs[2] - 1;

    return wrCpssDxChTtiRuleActionHwGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_2GetFirst
*
* DESCRIPTION:
*       This function gets first TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_2GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ruleIndex = 0;

    /* first to get is mask */
    isMask = GT_TRUE;

    /* check if the application request 0 entries */
    if (inArgs[2] == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    ruleIndex       = (GT_U32)inArgs[1];
    ruleIndexMaxGet = ruleIndex + (GT_U32)inArgs[2] - 1;

    return wrCpssDxChTtiRule_2Get(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_4_KEY_UDB_Set
*
* DESCRIPTION:
*       This function maps input arguments to UDB TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_4_KEY_UDB_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr        = &maskData;
        mask_ruleIndex = (GT_U32)inFields[0];
    }
    else /* pattern */
    {
        rulePtr           = &patternData;
        pattern_ruleIndex = (GT_U32)inFields[0];
    }

    switch (inFields[2]) /* rule size */
    {
    case CPSS_DXCH_TTI_KEY_SIZE_10_B_E:
        ruleFormat = CPSS_DXCH_TTI_RULE_UDB_10_E;

        rulePtr->udbArray[0] = (GT_U8)inFields[3];
        rulePtr->udbArray[1] = (GT_U8)inFields[4];
        rulePtr->udbArray[2] = (GT_U8)inFields[5];
        rulePtr->udbArray[3] = (GT_U8)inFields[6];
        rulePtr->udbArray[4] = (GT_U8)inFields[7];
        rulePtr->udbArray[5] = (GT_U8)inFields[8];
        rulePtr->udbArray[6] = (GT_U8)inFields[9];
        rulePtr->udbArray[7] = (GT_U8)inFields[10];
        rulePtr->udbArray[8] = (GT_U8)inFields[11];
        rulePtr->udbArray[9] = (GT_U8)inFields[12];
        break;

    case CPSS_DXCH_TTI_KEY_SIZE_20_B_E:
        ruleFormat = CPSS_DXCH_TTI_RULE_UDB_20_E;

        rulePtr->udbArray[0] = (GT_U8)inFields[3];
        rulePtr->udbArray[1] = (GT_U8)inFields[4];
        rulePtr->udbArray[2] = (GT_U8)inFields[5];
        rulePtr->udbArray[3] = (GT_U8)inFields[6];
        rulePtr->udbArray[4] = (GT_U8)inFields[7];
        rulePtr->udbArray[5] = (GT_U8)inFields[8];
        rulePtr->udbArray[6] = (GT_U8)inFields[9];
        rulePtr->udbArray[7] = (GT_U8)inFields[10];
        rulePtr->udbArray[8] = (GT_U8)inFields[11];
        rulePtr->udbArray[9] = (GT_U8)inFields[12];

        rulePtr->udbArray[10] = (GT_U8)inFields[13];
        rulePtr->udbArray[11] = (GT_U8)inFields[14];
        rulePtr->udbArray[12] = (GT_U8)inFields[15];
        rulePtr->udbArray[13] = (GT_U8)inFields[16];
        rulePtr->udbArray[14] = (GT_U8)inFields[17];
        rulePtr->udbArray[15] = (GT_U8)inFields[18];
        rulePtr->udbArray[16] = (GT_U8)inFields[19];
        rulePtr->udbArray[17] = (GT_U8)inFields[20];
        rulePtr->udbArray[18] = (GT_U8)inFields[21];
        rulePtr->udbArray[19] = (GT_U8)inFields[22];
        break;

    case CPSS_DXCH_TTI_KEY_SIZE_30_B_E:
        ruleFormat = CPSS_DXCH_TTI_RULE_UDB_30_E;

        rulePtr->udbArray[0] = (GT_U8)inFields[3];
        rulePtr->udbArray[1] = (GT_U8)inFields[4];
        rulePtr->udbArray[2] = (GT_U8)inFields[5];
        rulePtr->udbArray[3] = (GT_U8)inFields[6];
        rulePtr->udbArray[4] = (GT_U8)inFields[7];
        rulePtr->udbArray[5] = (GT_U8)inFields[8];
        rulePtr->udbArray[6] = (GT_U8)inFields[9];
        rulePtr->udbArray[7] = (GT_U8)inFields[10];
        rulePtr->udbArray[8] = (GT_U8)inFields[11];
        rulePtr->udbArray[9] = (GT_U8)inFields[12];

        rulePtr->udbArray[10] = (GT_U8)inFields[13];
        rulePtr->udbArray[11] = (GT_U8)inFields[14];
        rulePtr->udbArray[12] = (GT_U8)inFields[15];
        rulePtr->udbArray[13] = (GT_U8)inFields[16];
        rulePtr->udbArray[14] = (GT_U8)inFields[17];
        rulePtr->udbArray[15] = (GT_U8)inFields[18];
        rulePtr->udbArray[16] = (GT_U8)inFields[19];
        rulePtr->udbArray[17] = (GT_U8)inFields[20];
        rulePtr->udbArray[18] = (GT_U8)inFields[21];
        rulePtr->udbArray[19] = (GT_U8)inFields[22];

        rulePtr->udbArray[20] = (GT_U8)inFields[23];
        rulePtr->udbArray[21] = (GT_U8)inFields[24];
        rulePtr->udbArray[22] = (GT_U8)inFields[25];
        rulePtr->udbArray[23] = (GT_U8)inFields[26];
        rulePtr->udbArray[24] = (GT_U8)inFields[27];
        rulePtr->udbArray[25] = (GT_U8)inFields[28];
        rulePtr->udbArray[26] = (GT_U8)inFields[29];
        rulePtr->udbArray[27] = (GT_U8)inFields[30];
        rulePtr->udbArray[28] = (GT_U8)inFields[31];
        rulePtr->udbArray[29] = (GT_U8)inFields[32];
        break;

    default:
        /* do nothing */
        break;
    }

    if (inFields[1] == 0) /* mask */
    {
        mask_set       = GT_TRUE;
    }
    else /* pattern */
    {
        pattern_set       = GT_TRUE;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_4SetFirst
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_4SetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;

    switch (inArgs[3]) /* rule type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_2_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_2_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_RULE_UDB_10_E:
    case CPSS_DXCH_TTI_RULE_UDB_20_E:
    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        wrCpssDxChTtiRule_4_KEY_UDB_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_4SetNext
*
* DESCRIPTION:
*       This function sets TTI rule.
*       Note: the rule index is an absolute index in the TCAM.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_4SetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;
    GT_U8                           devNum;
    GT_U32                          ruleIndex;
    CPSS_DXCH_TTI_RULE_TYPE_ENT     keyType;
    GT_U32                          actionIndex;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType;
    CPSS_DXCH_TTI_ACTION_UNT        *actionPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Check Validity */
    if (inFields[1] > 1)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be MASK or PATTERN).\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 0) && mask_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 1) && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inFields[0];

    switch (inArgs[3]) /* rule type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_2_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_2_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_RULE_UDB_10_E:
    case CPSS_DXCH_TTI_RULE_UDB_20_E:
    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        wrCpssDxChTtiRule_4_KEY_UDB_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (mask_set && pattern_set && (mask_ruleIndex != pattern_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }


    /* Get the action */
    ttiActionDatabaseGet(devNum,ruleIndex,&actionIndex);

    if ((mask_set && pattern_set) && (actionIndex != INVALID_RULE_INDEX))
    {
        actionPtr                    = &(ttiActionDatabase[actionIndex].actionEntry);
        keyType                      = ruleFormat;
        ttiRuleFormat[ruleIndex]     = keyType;
        actionType                   = ttiActionDatabase[actionIndex].actionType;
        ttiRuleActionType[ruleIndex] = actionType;

        /* call cpss api function - ruleIndex is an absolute index */
        result = pg_wrap_cpssDxChTtiRuleSet(devNum,ruleIndex,keyType,&patternData,&maskData,actionType,actionPtr,GT_TRUE);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
        cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_4_KEY_UDB_Get
*
* DESCRIPTION:
*       This function maps UDB TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_4_KEY_UDB_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2] = ttiRuleFormat[ruleIndex];
    ruleFormat = ttiRuleFormat[ruleIndex];

    switch (ttiRuleFormat[ruleIndex])
    {
    case CPSS_DXCH_TTI_RULE_UDB_10_E:
        inFields[2] = CPSS_DXCH_TTI_KEY_SIZE_10_B_E;

        inFields[3] = rulePtr->udbArray[0];
        inFields[4] = rulePtr->udbArray[1];
        inFields[5] = rulePtr->udbArray[2];
        inFields[6] = rulePtr->udbArray[3];
        inFields[7] = rulePtr->udbArray[4];
        inFields[8] = rulePtr->udbArray[5];
        inFields[9] = rulePtr->udbArray[6];
        inFields[10] = rulePtr->udbArray[7];
        inFields[11] = rulePtr->udbArray[8];
        inFields[12] = rulePtr->udbArray[9];

        inFields[13] = 0;
        inFields[14] = 0;
        inFields[15] = 0;
        inFields[16] = 0;
        inFields[17] = 0;
        inFields[18] = 0;
        inFields[19] = 0;
        inFields[20] = 0;
        inFields[21] = 0;
        inFields[22] = 0;

        inFields[23] = 0;
        inFields[24] = 0;
        inFields[25] = 0;
        inFields[26] = 0;
        inFields[27] = 0;
        inFields[28] = 0;
        inFields[29] = 0;
        inFields[30] = 0;
        inFields[31] = 0;
        inFields[32] = 0;

        break;
    case CPSS_DXCH_TTI_RULE_UDB_20_E:
        inFields[2] = CPSS_DXCH_TTI_KEY_SIZE_20_B_E;

        inFields[3] = rulePtr->udbArray[0];
        inFields[4] = rulePtr->udbArray[1];
        inFields[5] = rulePtr->udbArray[2];
        inFields[6] = rulePtr->udbArray[3];
        inFields[7] = rulePtr->udbArray[4];
        inFields[8] = rulePtr->udbArray[5];
        inFields[9] = rulePtr->udbArray[6];
        inFields[10] = rulePtr->udbArray[7];
        inFields[11] = rulePtr->udbArray[8];
        inFields[12] = rulePtr->udbArray[9];

        inFields[13] = rulePtr->udbArray[10];
        inFields[14] = rulePtr->udbArray[11];
        inFields[15] = rulePtr->udbArray[12];
        inFields[16] = rulePtr->udbArray[13];
        inFields[17] = rulePtr->udbArray[14];
        inFields[18] = rulePtr->udbArray[15];
        inFields[19] = rulePtr->udbArray[16];
        inFields[20] = rulePtr->udbArray[17];
        inFields[21] = rulePtr->udbArray[18];
        inFields[22] = rulePtr->udbArray[19];

        inFields[23] = 0;
        inFields[24] = 0;
        inFields[25] = 0;
        inFields[26] = 0;
        inFields[27] = 0;
        inFields[28] = 0;
        inFields[29] = 0;
        inFields[30] = 0;
        inFields[31] = 0;
        inFields[32] = 0;

        break;



    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        inFields[2] = CPSS_DXCH_TTI_KEY_SIZE_30_B_E;

        inFields[3] = rulePtr->udbArray[0];
        inFields[4] = rulePtr->udbArray[1];
        inFields[5] = rulePtr->udbArray[2];
        inFields[6] = rulePtr->udbArray[3];
        inFields[7] = rulePtr->udbArray[4];
        inFields[8] = rulePtr->udbArray[5];
        inFields[9] = rulePtr->udbArray[6];
        inFields[10] = rulePtr->udbArray[7];
        inFields[11] = rulePtr->udbArray[8];
        inFields[12] = rulePtr->udbArray[9];

        inFields[13] = rulePtr->udbArray[10];
        inFields[14] = rulePtr->udbArray[11];
        inFields[15] = rulePtr->udbArray[12];
        inFields[16] = rulePtr->udbArray[13];
        inFields[17] = rulePtr->udbArray[14];
        inFields[18] = rulePtr->udbArray[15];
        inFields[19] = rulePtr->udbArray[16];
        inFields[20] = rulePtr->udbArray[17];
        inFields[21] = rulePtr->udbArray[18];
        inFields[22] = rulePtr->udbArray[19];

        inFields[23] = rulePtr->udbArray[20];
        inFields[24] = rulePtr->udbArray[21];
        inFields[25] = rulePtr->udbArray[22];
        inFields[26] = rulePtr->udbArray[23];
        inFields[27] = rulePtr->udbArray[24];
        inFields[28] = rulePtr->udbArray[25];
        inFields[29] = rulePtr->udbArray[26];
        inFields[30] = rulePtr->udbArray[27];
        inFields[31] = rulePtr->udbArray[28];
        inFields[32] = rulePtr->udbArray[29];

        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
            inFields[0], inFields[1], inFields[2], inFields[3],
            inFields[4], inFields[5], inFields[6], inFields[7],
            inFields[8], inFields[9], inFields[10],inFields[11],
            inFields[12],inFields[13],inFields[14],inFields[15],
            inFields[16],inFields[17],inFields[18],inFields[19],
            inFields[20],inFields[21],inFields[22],inFields[23],
            inFields[24],inFields[25],inFields[26],inFields[27],
            inFields[28],inFields[29],inFields[30],inFields[31],
            inFields[32]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 4);
    return CMD_OK;
}
/*******************************************************************************
* wrCpssDxChTtiRule_4Get
*
* DESCRIPTION:
*       This function gets TTI rule.
*       Note: the rule index is an absolute index in the TCAM.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_4Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    GT_BOOL                     ruleValid;
    GT_U32                      ruleIndexStep = 0;

    /* if isMask == GT_TRUE it is mask's turn (the first one to get), so there need to refresh the data
    else the data is updated and it is pattern's turn (was updated earlier)*/
    if (isMask)
    {
        /* check if requested refresh range is already refreshed */
        if ((ruleIndex >= 36864) || (ruleIndex > ruleIndexMaxGet))
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];

        /* look for another valid rule within the range */
        for ((ruleValid = GT_FALSE) ; ((ruleValid == GT_FALSE) && (ruleIndex < 36864) && (ruleIndex <= ruleIndexMaxGet)) ; ruleIndex += ruleIndexStep)
        {
            /* check if the rule is valid - ruleIndex is an absolute index */
            result = pg_wrap_cpssDxChTtiRuleValidStatusGet(devNum,ruleIndex,&ruleValid,GT_TRUE);
            if (result != GT_OK)
            {
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }

            if (ttiRuleFormat[ruleIndex] == CPSS_DXCH_TTI_RULE_UDB_10_E)
            {
                ruleIndexStep = 1;
            }
            else if (ttiRuleFormat[ruleIndex] == CPSS_DXCH_TTI_RULE_UDB_20_E)
            {
                ruleIndexStep = 2;
            }
            else
            {
                ruleIndexStep = 3;
            }
        }

        /* check if no valid rule was found in the requested range */
        if ((ruleValid == GT_FALSE))
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        /* valid rule was found in the requested range */
        ruleIndex -= ruleIndexStep;

        /* call cpss api function, note that it is assumed that rule action
           type is not modified after setting the rule.
           ruleIndex is an absolute index */
        result = pg_wrap_cpssDxChTtiRuleGet(devNum,ruleIndex,ttiRuleFormat[ruleIndex],&patternData,&maskData,ttiRuleActionType[ruleIndex],&action,GT_TRUE);
    }

    switch (ttiRuleFormat[ruleIndex])
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_2_KEY_MPLS_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_2_KEY_MIM_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_RULE_UDB_10_E:
    case CPSS_DXCH_TTI_RULE_UDB_20_E:
    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        wrCpssDxChTtiRule_4_KEY_UDB_Get(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (isMask)
    {
        if (ttiRuleFormat[ruleIndex] == CPSS_DXCH_TTI_RULE_UDB_10_E)
        {
            ruleIndexStep = 1;
        }
        else if (ttiRuleFormat[ruleIndex] == CPSS_DXCH_TTI_RULE_UDB_20_E)
        {
            ruleIndexStep = 2;
        }
        else
        {
            ruleIndexStep = 3;
        }

        ruleIndex += ruleIndexStep;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_4GetFirst
*
* DESCRIPTION:
*       This function gets first TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_4GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ruleIndex = 0;

    /* first to get is mask */
    isMask = GT_TRUE;

    /* check if the application request 0 entries */
    if (inArgs[2] == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    ruleIndex       = (GT_U32)inArgs[1];
    ruleIndexMaxGet = ruleIndex + (GT_U32)inArgs[2] - 1;

    return wrCpssDxChTtiRule_4Get(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_3_KEY_MIM_Set
*
* DESCRIPTION:
*       This function maps input arguments to MIM TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_3_KEY_MIM_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr = &maskData;
    }
    else /* pattern */
    {
        rulePtr = &patternData;
    }

    /* MIM key in TtiRule_3 table is the same as in TtiRule_2 table except for fields:
       macToMe, passengerPacketOuterTagExists, passengerPacketOuterTagVid,
       passengerPacketOuterTagUp, passengerPacketOuterTagDei */
    wrCpssDxChTtiRule_2_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
    rulePtr->mim.macToMe                        = (GT_BOOL)inFields[19];
    rulePtr->mim.passengerPacketOuterTagExists  = (GT_BOOL)inFields[20];
    rulePtr->mim.passengerPacketOuterTagVid     = (GT_U32)inFields[21];
    rulePtr->mim.passengerPacketOuterTagUp      = (GT_U32)inFields[22];
    rulePtr->mim.passengerPacketOuterTagDei     = (GT_U32)inFields[23];

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_3_KEY_MIM_Get
*
* DESCRIPTION:
*       This function maps MIM TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_3_KEY_MIM_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MIM_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2] = rulePtr->mim.common.pclId;
    inFields[3] = rulePtr->mim.common.srcIsTrunk;
    inFields[4] = rulePtr->mim.common.srcPortTrunk;

    if (rulePtr->mim.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->mim.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        inFields[4] = (GT_U32)port;
    }

    inFields[6] = rulePtr->mim.common.vid;
    inFields[7] = rulePtr->mim.common.isTagged;
    inFields[8] = rulePtr->mim.common.dsaSrcIsTrunk;
    inFields[9] = rulePtr->mim.common.dsaSrcPortTrunk;
    inFields[10] = rulePtr->mim.common.dsaSrcDevice;
    inFields[11] = rulePtr->mim.bUp;
    inFields[12] = rulePtr->mim.bDp;
    inFields[13] = rulePtr->mim.iSid;
    inFields[14] = rulePtr->mim.iUp;
    inFields[15] = rulePtr->mim.iDp;
    inFields[16] = rulePtr->mim.iRes1;
    inFields[17] = rulePtr->mim.iRes2;
    inFields[18] = rulePtr->mim.common.sourcePortGroupId;
    inFields[19] = rulePtr->mim.macToMe;
    inFields[20] = rulePtr->mim.passengerPacketOuterTagExists;
    inFields[21] = rulePtr->mim.passengerPacketOuterTagVid;
    inFields[22] = rulePtr->mim.passengerPacketOuterTagUp;
    inFields[23] = rulePtr->mim.passengerPacketOuterTagDei;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->eth.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11],inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17],inFields[18],
                inFields[19],inFields[20],inFields[21],inFields[22],
                inFields[23]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_5SetFirst
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_5SetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;

    switch (inArgs[3]) /* rule type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_2_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_3_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_RULE_UDB_10_E:
    case CPSS_DXCH_TTI_RULE_UDB_20_E:
    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        wrCpssDxChTtiRule_4_KEY_UDB_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_5SetNext
*
* DESCRIPTION:
*       This function sets TTI rule.
*       Note: the rule index is an absolute index in the TCAM.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_5SetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;
    GT_U8                           devNum;
    GT_U32                          ruleIndex;
    CPSS_DXCH_TTI_RULE_TYPE_ENT     keyType;
    GT_U32                          actionIndex;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType;
    CPSS_DXCH_TTI_ACTION_UNT        *actionPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Check Validity */
    if (inFields[1] > 1)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be MASK or PATTERN).\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 0) && mask_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 1) && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inFields[0];

    switch (inArgs[3]) /* rule type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_2_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_3_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_RULE_UDB_10_E:
    case CPSS_DXCH_TTI_RULE_UDB_20_E:
    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        wrCpssDxChTtiRule_4_KEY_UDB_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (mask_set && pattern_set && (mask_ruleIndex != pattern_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }


    /* Get the action */
    ttiActionDatabaseGet(devNum,ruleIndex,&actionIndex);

    if ((mask_set && pattern_set) && (actionIndex != INVALID_RULE_INDEX))
    {
        actionPtr                    = &(ttiActionDatabase[actionIndex].actionEntry);
        keyType                      = ruleFormat;
        ttiRuleFormat[ruleIndex]     = keyType;
        actionType                   = ttiActionDatabase[actionIndex].actionType;
        ttiRuleActionType[ruleIndex] = actionType;

        /* call cpss api function - ruleIndex is an absolute index */
        result = pg_wrap_cpssDxChTtiRuleSet(devNum,ruleIndex,keyType,&patternData,&maskData,actionType,actionPtr,GT_TRUE);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
        cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_5Get
*
* DESCRIPTION:
*       This function gets TTI rule.
*       Note: the rule index is an absolute index in the TCAM.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_5Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    GT_BOOL                     ruleValid;
    GT_U32                      ruleIndexStep = 0;

    /* if isMask == GT_TRUE it is mask's turn (the first one to get), so there need to refresh the data
    else the data is updated and it is pattern's turn (was updated earlier)*/
    if (isMask)
    {
        /* check if requested refresh range is already refreshed */
        if ((ruleIndex >= 36864) || (ruleIndex > ruleIndexMaxGet))
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];

        /* look for another valid rule within the range */
        for ((ruleValid = GT_FALSE) ; ((ruleValid == GT_FALSE) && (ruleIndex < 36864) && (ruleIndex <= ruleIndexMaxGet)) ; ruleIndex += ruleIndexStep)
        {
            /* check if the rule is valid - ruleIndex is an absolute index */
            result = pg_wrap_cpssDxChTtiRuleValidStatusGet(devNum,ruleIndex,&ruleValid,GT_TRUE);
            if (result != GT_OK)
            {
                ruleIndexStep = 1;
                ruleValid=GT_FALSE; /* so we will continue the loop */
            }
            else
            {
                if(ruleValid==GT_FALSE)
                {
                    ruleIndexStep = 1;
                }
                else
                {
                    if (ttiRuleFormat[ruleIndex] == CPSS_DXCH_TTI_RULE_UDB_10_E)
                    {
                        ruleIndexStep = 1;
                    }
                    else if (ttiRuleFormat[ruleIndex] == CPSS_DXCH_TTI_RULE_UDB_20_E)
                    {
                        ruleIndexStep = 2;
                    }
                    else
                    {
                        ruleIndexStep = 3;
                    }
                }
            }
        }

        /* check if no valid rule was found in the requested range */
        if ((ruleValid == GT_FALSE))
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        /* valid rule was found in the requested range */
        ruleIndex -= ruleIndexStep;

        /* call cpss api function, note that it is assumed that rule action
           type is not modified after setting the rule.
           ruleIndex is an absolute index */
        result = pg_wrap_cpssDxChTtiRuleGet(devNum,ruleIndex,ttiRuleFormat[ruleIndex],&patternData,&maskData,ttiRuleActionType[ruleIndex],&action,GT_TRUE);
    }

    switch (ttiRuleFormat[ruleIndex])
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_2_KEY_MPLS_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_3_KEY_MIM_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_RULE_UDB_10_E:
    case CPSS_DXCH_TTI_RULE_UDB_20_E:
    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        wrCpssDxChTtiRule_4_KEY_UDB_Get(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (isMask)
    {
        if (ttiRuleFormat[ruleIndex] == CPSS_DXCH_TTI_RULE_UDB_10_E)
        {
            ruleIndexStep = 1;
        }
        else if (ttiRuleFormat[ruleIndex] == CPSS_DXCH_TTI_RULE_UDB_20_E)
        {
            ruleIndexStep = 2;
        }
        else
        {
            ruleIndexStep = 3;
        }

        ruleIndex += ruleIndexStep;
    }

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_5GetFirst
*
* DESCRIPTION:
*       This function gets first TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_5GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ruleIndex = 0;

    /* first to get is mask */
    isMask = GT_TRUE;

    /* check if the application request 0 entries */
    if (inArgs[2] == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    ruleIndex       = (GT_U32)inArgs[1];
    ruleIndexMaxGet = ruleIndex + (GT_U32)inArgs[2] - 1;

    return wrCpssDxChTtiRule_5Get(inArgs,inFields,numFields,outArgs);
}

/***********************************/
/* cpssDxChTtiRuleAction_2 Table */
/***********************************/

/*******************************************************************************
* wrCpssDxChTtiRuleAction_2Update
*
* DESCRIPTION:
*       This function updates rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_2Update
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result = GT_OK;
    GT_U8                               devNum;
    GT_U32                              ruleIndex;
    GT_U32                              index;
    CPSS_DXCH_TTI_ACTION_UNT            *actionPtr;
    GT_BOOL                             isNewIndex = GT_FALSE;
    ttiActionDB                         ttiTempAction = {0};

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)inFields[0];
    ruleIndex = (GT_U32)inFields[1];

    /* in case it is first usage of TTI action database, initialize it */
    if (firstRun)
    {
        ttiActionDatabaseInit();
        firstRun = GT_FALSE;
    }

    /* look for the action in the TTI action database */
    ttiActionDatabaseGet(devNum, ruleIndex, &index);

    /* the action wasn't in the TTI action database */
    if (index == INVALID_RULE_INDEX)
    {
        if (firstFree == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
            return CMD_AGENT_ERROR;
        }

        index = firstFree;
        firstFree = ttiActionDatabase[firstFree].nextFree;
        isNewIndex = GT_TRUE;
    }

    /* this is update of an existing action:
       save the old action parameters */
    if (isNewIndex == GT_FALSE)
    {
        cpssOsMemCpy(&ttiTempAction,&ttiActionDatabase[index],sizeof(ttiTempAction));
    }

    /* set action entry in TTI action database as valid */
    ttiActionDatabase[index].valid      = GT_TRUE;
    ttiActionDatabase[index].ruleIndex  = ruleIndex;
    ttiActionDatabase[index].devNum     = devNum;
    ttiActionDatabase[index].actionType = (CPSS_DXCH_TTI_ACTION_TYPE_ENT)inArgs[0];

    actionPtr = &(ttiActionDatabase[index].actionEntry);

    /* fill action fields */
    switch (ttiActionDatabase[index].actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:

        actionPtr->type1.tunnelTerminate                  = (GT_BOOL)inFields[2];
        actionPtr->type1.passengerPacketType              = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
        actionPtr->type1.copyTtlFromTunnelHeader          = (GT_BOOL)inFields[4];
        actionPtr->type1.command                          = (CPSS_PACKET_CMD_ENT)inFields[5];
        actionPtr->type1.redirectCommand                  = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[6];
        actionPtr->type1.egressInterface.type             = (CPSS_INTERFACE_TYPE_ENT)inFields[7];
        actionPtr->type1.egressInterface.devPort.hwDevNum   = (GT_HW_DEV_NUM)inFields[8];
        actionPtr->type1.egressInterface.devPort.portNum  = (GT_PORT_NUM)inFields[9];
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionPtr->type1.egressInterface.devPort.hwDevNum, actionPtr->type1.egressInterface.devPort.portNum);
        actionPtr->type1.egressInterface.trunkId          = (GT_TRUNK_ID)inFields[10];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type1.egressInterface.trunkId);
        actionPtr->type1.egressInterface.vidx             = (GT_U16)inFields[11];
        actionPtr->type1.egressInterface.vlanId           = (GT_U16)inFields[12];
        actionPtr->type1.tunnelStart                      = (GT_BOOL)inFields[13];
        actionPtr->type1.tunnelStartPtr                   = (GT_U32)inFields[14];
        actionPtr->type1.routerLookupPtr                  = (GT_U32)inFields[15];
        actionPtr->type1.vrfId                            = (GT_U32)inFields[16];
        /* the fields useVidx,targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
           from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be ignored */
        actionPtr->type1.sourceIdSetEnable                = (GT_BOOL)inFields[21];
        actionPtr->type1.sourceId                         = (GT_U32)inFields[22];
        actionPtr->type1.vlanCmd                          = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[23];
        actionPtr->type1.vlanId                           = (GT_U16)inFields[24];
        actionPtr->type1.vlanPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[25];
        actionPtr->type1.nestedVlanEnable                 = (GT_BOOL)inFields[26];
        actionPtr->type1.bindToPolicer                    = (GT_BOOL)inFields[27];
        actionPtr->type1.policerIndex                     = (GT_U32)inFields[28];
        actionPtr->type1.qosPrecedence                    = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[29];
        actionPtr->type1.qosTrustMode                     = (CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT)inFields[30];
        actionPtr->type1.qosProfile                       = (GT_U32)inFields[31];
        actionPtr->type1.modifyUpEnable                   = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[32];
        actionPtr->type1.modifyDscpEnable                 = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[33];
        actionPtr->type1.up                               = (GT_U32)inFields[34];
        actionPtr->type1.remapDSCP                        = (GT_BOOL)inFields[35];
        actionPtr->type1.mirrorToIngressAnalyzerEnable    = (GT_BOOL)inFields[36];
        actionPtr->type1.userDefinedCpuCode               = (CPSS_NET_RX_CPU_CODE_ENT)inFields[37];
        actionPtr->type1.vntl2Echo                        = (GT_BOOL)inFields[38];
        actionPtr->type1.bridgeBypass                     = (GT_BOOL)inFields[39];
        actionPtr->type1.actionStop                       = (GT_BOOL)inFields[40];
        actionPtr->type1.activateCounter                  = (GT_BOOL)inFields[41];
        actionPtr->type1.counterIndex                     = (GT_U32)inFields[42];
        break;

    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:

        actionPtr->type2.tunnelTerminate                 = (GT_BOOL)inFields[2];
        actionPtr->type2.ttPassengerPacketType           = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
        actionPtr->type2.tsPassengerPacketType           = (CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT)inFields[4];
        actionPtr->type2.copyTtlExpFromTunnelHeader      = (GT_BOOL)inFields[5];
        actionPtr->type2.mplsCommand                     = (CPSS_DXCH_TTI_MPLS_CMD_ENT)inFields[6];
        actionPtr->type2.mplsTtl                         = (GT_U32)inFields[7];
        actionPtr->type2.enableDecrementTtl              = (GT_BOOL)inFields[8];
        actionPtr->type2.command                         = (CPSS_PACKET_CMD_ENT)inFields[9];
        actionPtr->type2.redirectCommand                 = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[10];
        actionPtr->type2.egressInterface.type            = (CPSS_INTERFACE_TYPE_ENT)inFields[11];
        actionPtr->type2.egressInterface.devPort.hwDevNum  = (GT_HW_DEV_NUM)inFields[12];
        actionPtr->type2.egressInterface.devPort.portNum = (GT_PORT_NUM)inFields[13];
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionPtr->type2.egressInterface.devPort.hwDevNum, actionPtr->type2.egressInterface.devPort.portNum);
        actionPtr->type2.egressInterface.trunkId         = (GT_TRUNK_ID)inFields[14];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type2.egressInterface.trunkId);
        actionPtr->type2.egressInterface.vidx            = (GT_U16)inFields[15];
        actionPtr->type2.egressInterface.vlanId          = (GT_U16)inFields[16];
        actionPtr->type2.arpPtr                          = (GT_U32)inFields[17];
        actionPtr->type2.tunnelStart                     = (GT_BOOL)inFields[18];
        actionPtr->type2.tunnelStartPtr                  = (GT_U32)inFields[19];
        actionPtr->type2.routerLttPtr                    = (GT_U32)inFields[20];
        actionPtr->type2.vrfId                           = (GT_U32)inFields[21];
        actionPtr->type2.sourceIdSetEnable               = (GT_BOOL)inFields[22];
        actionPtr->type2.sourceId                        = (GT_U32)inFields[23];
        actionPtr->type2.tag0VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[24];
        actionPtr->type2.tag0VlanId                      = (GT_U16)inFields[25];
        actionPtr->type2.tag1VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[26];
        actionPtr->type2.tag1VlanId                      = (GT_U16)inFields[27];
        actionPtr->type2.tag0VlanPrecedence              = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[28];
        actionPtr->type2.nestedVlanEnable                = (GT_BOOL)inFields[29];
        actionPtr->type2.bindToPolicerMeter              = (GT_BOOL)inFields[30];
        actionPtr->type2.bindToPolicer                   = (GT_BOOL)inFields[31];
        actionPtr->type2.policerIndex                    = (GT_U32)inFields[32];
        actionPtr->type2.qosPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[33];
        actionPtr->type2.keepPreviousQoS                 = (GT_BOOL)inFields[34];
        actionPtr->type2.trustUp                         = (GT_BOOL)inFields[35];
        actionPtr->type2.trustDscp                       = (GT_BOOL)inFields[36];
        actionPtr->type2.trustExp                        = (GT_BOOL)inFields[37];
        actionPtr->type2.qosProfile                      = (GT_U32)inFields[38];
        actionPtr->type2.modifyTag0Up                    = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[39];
        actionPtr->type2.tag1UpCommand                   = (CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT)inFields[40];
        actionPtr->type2.modifyDscp                      = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[41];
        actionPtr->type2.tag0Up                          = (GT_U32)inFields[42];
        actionPtr->type2.tag1Up                          = (GT_U32)inFields[43];
        actionPtr->type2.remapDSCP                       = (GT_BOOL)inFields[44];
        actionPtr->type2.pcl0OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[45];
        actionPtr->type2.pcl0_1OverrideConfigIndex       = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[46];
        actionPtr->type2.pcl1OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[47];
        actionPtr->type2.iPclConfigIndex                 = (GT_U32)inFields[48];
        actionPtr->type2.mirrorToIngressAnalyzerEnable   = (GT_BOOL)inFields[49];
        actionPtr->type2.userDefinedCpuCode              = (CPSS_NET_RX_CPU_CODE_ENT)inFields[50];
        actionPtr->type2.bindToCentralCounter            = (GT_BOOL)inFields[51];
        actionPtr->type2.centralCounterIndex             = (GT_U32)inFields[52];
        actionPtr->type2.vntl2Echo                       = (GT_BOOL)inFields[53];
        actionPtr->type2.bridgeBypass                    = (GT_BOOL)inFields[54];
        actionPtr->type2.ingressPipeBypass               = (GT_BOOL)inFields[55];
        actionPtr->type2.actionStop                      = (GT_BOOL)inFields[56];
        actionPtr->type2.hashMaskIndex                   = (GT_U32)inFields[57];
        actionPtr->type2.modifyMacSa                     = (GT_BOOL)inFields[58];
        actionPtr->type2.modifyMacDa                     = (GT_BOOL)inFields[59];
        actionPtr->type2.ResetSrcPortGroupId             = (GT_BOOL)inFields[60];
        actionPtr->type2.multiPortGroupTtiEnable         = (GT_BOOL)inFields[61];

        break;

    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_AGENT_ERROR;
    }

    /* call cpss api function */
    result = pg_wrap_cpssDxChTtiRuleActionUpdate(devNum,ruleIndex,ttiActionDatabase[index].actionType,actionPtr,GT_FALSE,isNewIndex);

    /* if the rule action didn't succeed:  */
    if (result != GT_OK)
    {
        /* if this is a new action remove it from the database */
        if (isNewIndex)
        {
            ttiActionDatabase[index].valid = GT_FALSE;
            ttiActionDatabase[index].nextFree = firstFree;
            firstFree = index;
        }
        /* if this is an update for existing action restore previous data */
        else
        {
            cpssOsMemCpy(&ttiActionDatabase[index],&ttiTempAction,sizeof(ttiTempAction));
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_2Get
*
* DESCRIPTION:
*       This function gets single rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_2Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_ACTION_UNT    *actionPtr;
    GT_HW_DEV_NUM               tempHwDev;    /* used for port,dev converting */
    GT_PORT_NUM                 tempPort;   /* used for port,dev converting */

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* get next valid TTI action entry */
    while ((ttiActionGetIndex < ACTION_TABLE_SIZE) &&
           (ttiActionDatabase[ttiActionGetIndex].valid == GT_FALSE))
        ttiActionGetIndex++;

    if (ttiActionGetIndex == ACTION_TABLE_SIZE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    actionPtr = &(ttiActionDatabase[ttiActionGetIndex].actionEntry);

    switch (ttiActionDatabase[ttiActionGetIndex].actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:

        inArgs[0] = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

        inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
        inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
        inFields[2]  = actionPtr->type1.tunnelTerminate;
        inFields[3]  = actionPtr->type1.passengerPacketType;
        inFields[4]  = actionPtr->type1.copyTtlFromTunnelHeader;
        inFields[5]  = actionPtr->type1.command;
        inFields[6]  = actionPtr->type1.redirectCommand;
        inFields[7]  = actionPtr->type1.egressInterface.type;
        tempHwDev      = actionPtr->type1.egressInterface.devPort.hwDevNum;
        tempPort     = actionPtr->type1.egressInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempHwDev,tempPort);
        inFields[8]  = tempHwDev;
        inFields[9]  = tempPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type1.egressInterface.trunkId);
        inFields[10] = actionPtr->type1.egressInterface.trunkId;
        inFields[11] = actionPtr->type1.egressInterface.vidx;
        inFields[12] = actionPtr->type1.egressInterface.vlanId;
        inFields[13] = actionPtr->type1.tunnelStart;
        inFields[14] = actionPtr->type1.tunnelStartPtr;
        inFields[15] = actionPtr->type1.routerLookupPtr;
        inFields[16] = actionPtr->type1.vrfId;
        /* the fields targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
           from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be hardcoded  */
        inFields[17] = GT_FALSE;
        inFields[18] = 0;
        inFields[19] = 0;
        inFields[20] = 0;
        inFields[21] = actionPtr->type1.sourceIdSetEnable;
        inFields[22] = actionPtr->type1.sourceId;
        inFields[23] = actionPtr->type1.vlanCmd;
        inFields[24] = actionPtr->type1.vlanId;
        inFields[25] = actionPtr->type1.vlanPrecedence;
        inFields[26] = actionPtr->type1.nestedVlanEnable;
        inFields[27] = actionPtr->type1.bindToPolicer;
        inFields[28] = actionPtr->type1.policerIndex;
        inFields[29] = actionPtr->type1.qosPrecedence;
        inFields[30] = actionPtr->type1.qosTrustMode;
        inFields[31] = actionPtr->type1.qosProfile;
        inFields[32] = actionPtr->type1.modifyUpEnable;
        inFields[33] = actionPtr->type1.modifyDscpEnable;
        inFields[34] = actionPtr->type1.up;
        inFields[35] = actionPtr->type1.remapDSCP;
        inFields[36] = actionPtr->type1.mirrorToIngressAnalyzerEnable;
        inFields[37] = actionPtr->type1.userDefinedCpuCode;
        inFields[38] = actionPtr->type1.vntl2Echo;
        inFields[39] = actionPtr->type1.bridgeBypass;
        inFields[40] = actionPtr->type1.actionStop;
        inFields[41] = actionPtr->type1.activateCounter;
        inFields[42] = actionPtr->type1.counterIndex;

        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],   inFields[2],   inFields[3],
                    inFields[4],  inFields[5],   inFields[6],   inFields[7],
                    inFields[8],  inFields[9],   inFields[10],  inFields[11],
                    inFields[12], inFields[13],  inFields[14],  inFields[15],
                    inFields[16], inFields[17],  inFields[18],  inFields[19],
                    inFields[20], inFields[21],  inFields[22],  inFields[23],
                    inFields[24], inFields[25],  inFields[26],  inFields[27],
                    inFields[28], inFields[29],  inFields[30],  inFields[31],
                    inFields[32], inFields[33],  inFields[34],  inFields[35],
                    inFields[36], inFields[37],  inFields[38],  inFields[39],
                    inFields[40], inFields[41],  inFields[42]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d%f", 0);
        break;

    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:

        inArgs[0] = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
        inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
        inFields[2]  = actionPtr->type2.tunnelTerminate;
        inFields[3]  = actionPtr->type2.ttPassengerPacketType;
        inFields[4]  = actionPtr->type2.tsPassengerPacketType;
        inFields[5]  = actionPtr->type2.copyTtlExpFromTunnelHeader;
        inFields[6]  = actionPtr->type2.mplsCommand;
        inFields[7]  = actionPtr->type2.mplsTtl;
        inFields[8]  = actionPtr->type2.enableDecrementTtl;
        inFields[9]  = actionPtr->type2.command;
        inFields[10] = actionPtr->type2.redirectCommand;
        inFields[11] = actionPtr->type2.egressInterface.type;
        tempHwDev      = actionPtr->type2.egressInterface.devPort.hwDevNum;
        tempPort     = actionPtr->type2.egressInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempHwDev,tempPort);
        inFields[12] = tempHwDev;
        inFields[13] = tempPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type2.egressInterface.trunkId);
        inFields[14] = actionPtr->type2.egressInterface.trunkId;
        inFields[15] = actionPtr->type2.egressInterface.vidx;
        inFields[16] = actionPtr->type2.egressInterface.vlanId;
        inFields[17] = actionPtr->type2.arpPtr;
        inFields[18] = actionPtr->type2.tunnelStart;
        inFields[19] = actionPtr->type2.tunnelStartPtr;
        inFields[20] = actionPtr->type2.routerLttPtr;
        inFields[21] = actionPtr->type2.vrfId;
        inFields[22] = actionPtr->type2.sourceIdSetEnable;
        inFields[23] = actionPtr->type2.sourceId;
        inFields[24] = actionPtr->type2.tag0VlanCmd;
        inFields[25] = actionPtr->type2.tag0VlanId;
        inFields[26] = actionPtr->type2.tag1VlanCmd;
        inFields[27] = actionPtr->type2.tag1VlanId;
        inFields[28] = actionPtr->type2.tag0VlanPrecedence;
        inFields[29] = actionPtr->type2.nestedVlanEnable;
        inFields[30] = actionPtr->type2.bindToPolicerMeter;
        inFields[31] = actionPtr->type2.bindToPolicer;
        inFields[32] = actionPtr->type2.policerIndex;
        inFields[33] = actionPtr->type2.qosPrecedence;
        inFields[34] = actionPtr->type2.keepPreviousQoS;
        inFields[35] = actionPtr->type2.trustUp;
        inFields[36] = actionPtr->type2.trustDscp;
        inFields[37] = actionPtr->type2.trustExp;
        inFields[38] = actionPtr->type2.qosProfile;
        inFields[39] = actionPtr->type2.modifyTag0Up;
        inFields[40] = actionPtr->type2.tag1UpCommand;
        inFields[41] = actionPtr->type2.modifyDscp;
        inFields[42] = actionPtr->type2.tag0Up;
        inFields[43] = actionPtr->type2.tag1Up;
        inFields[44] = actionPtr->type2.remapDSCP;
        inFields[45] = actionPtr->type2.pcl0OverrideConfigIndex;
        inFields[46] = actionPtr->type2.pcl0_1OverrideConfigIndex;
        inFields[47] = actionPtr->type2.pcl1OverrideConfigIndex;
        inFields[48] = actionPtr->type2.iPclConfigIndex;
        inFields[49] = actionPtr->type2.mirrorToIngressAnalyzerEnable;
        inFields[50] = actionPtr->type2.userDefinedCpuCode;
        inFields[51] = actionPtr->type2.bindToCentralCounter;
        inFields[52] = actionPtr->type2.centralCounterIndex;
        inFields[53] = actionPtr->type2.vntl2Echo;
        inFields[54] = actionPtr->type2.bridgeBypass;
        inFields[55] = actionPtr->type2.ingressPipeBypass;
        inFields[56] = actionPtr->type2.actionStop;
        inFields[57] = actionPtr->type2.hashMaskIndex;
        inFields[58] = actionPtr->type2.modifyMacSa;
        inFields[59] = actionPtr->type2.modifyMacDa;
        inFields[60] = actionPtr->type2.ResetSrcPortGroupId;
        inFields[61] = actionPtr->type2.multiPortGroupTtiEnable;

        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],   inFields[2],   inFields[3],
                    inFields[4],  inFields[5],   inFields[6],   inFields[7],
                    inFields[8],  inFields[9],   inFields[10],  inFields[11],
                    inFields[12], inFields[13],  inFields[14],  inFields[15],
                    inFields[16], inFields[17],  inFields[18],  inFields[19],
                    inFields[20], inFields[21],  inFields[22],  inFields[23],
                    inFields[24], inFields[25],  inFields[26],  inFields[27],
                    inFields[28], inFields[29],  inFields[30],  inFields[31],
                    inFields[32], inFields[33],  inFields[34],  inFields[35],
                    inFields[36], inFields[37],  inFields[38],  inFields[39],
                    inFields[40], inFields[41],  inFields[42],  inFields[43],
                    inFields[44], inFields[45],  inFields[46],  inFields[47],
                    inFields[48], inFields[49],  inFields[50],  inFields[51],
                    inFields[52], inFields[53],  inFields[54],  inFields[55],
                    inFields[56], inFields[57],  inFields[58],  inFields[59],
                    inFields[60], inFields[61]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d%f", 1);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI ACTION FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    ttiActionGetIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_2GetFirst
*
* DESCRIPTION:
*       This function gets first rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_2GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ttiActionGetIndex = 0;

    return wrCpssDxChTtiRuleAction_2Get(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_5Update
*
* DESCRIPTION:
*       This function updates rule action.
*       Note: the rule index is an absolute index in the TCAM.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_5Update
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result = GT_OK;
    GT_U8                               devNum;
    GT_U32                              ruleIndex;
    GT_U32                              index;
    CPSS_DXCH_TTI_ACTION_UNT            *actionPtr;
    GT_BOOL                             isNewIndex = GT_FALSE;
    ttiActionDB                         ttiTempAction = {0};

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)inFields[0];
    ruleIndex = (GT_U32)inFields[1];

    /* in case it is first usage of TTI action database, initialize it */
    if (firstRun)
    {
        ttiActionDatabaseInit();
        firstRun = GT_FALSE;
    }

    /* look for the action in the TTI action database */
    ttiActionDatabaseGet(devNum, ruleIndex, &index);

    /* the action wasn't in the TTI action database */
    if (index == INVALID_RULE_INDEX)
    {
        if (firstFree == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
            return CMD_AGENT_ERROR;
        }

        index = firstFree;
        firstFree = ttiActionDatabase[firstFree].nextFree;
        isNewIndex = GT_TRUE;
    }

    /* this is update of an existing action:
       save the old action parameters */
    if (isNewIndex == GT_FALSE)
    {
        cpssOsMemCpy(&ttiTempAction,&ttiActionDatabase[index],sizeof(ttiTempAction));
    }

    /* set action entry in TTI action database as valid */
    ttiActionDatabase[index].valid      = GT_TRUE;
    ttiActionDatabase[index].ruleIndex  = ruleIndex;
    ttiActionDatabase[index].devNum     = devNum;
    ttiActionDatabase[index].actionType = (CPSS_DXCH_TTI_ACTION_TYPE_ENT)inArgs[0];

    actionPtr = &(ttiActionDatabase[index].actionEntry);

    /* fill action fields */
    switch (ttiActionDatabase[index].actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:

        actionPtr->type1.tunnelTerminate                  = (GT_BOOL)inFields[2];
        actionPtr->type1.passengerPacketType              = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
        actionPtr->type1.copyTtlFromTunnelHeader          = (GT_BOOL)inFields[4];
        actionPtr->type1.command                          = (CPSS_PACKET_CMD_ENT)inFields[5];
        actionPtr->type1.redirectCommand                  = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[6];
        actionPtr->type1.egressInterface.type             = (CPSS_INTERFACE_TYPE_ENT)inFields[7];
        actionPtr->type1.egressInterface.devPort.hwDevNum   = (GT_HW_DEV_NUM)inFields[8];
        actionPtr->type1.egressInterface.devPort.portNum  = (GT_PORT_NUM)inFields[9];
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionPtr->type1.egressInterface.devPort.hwDevNum, actionPtr->type1.egressInterface.devPort.portNum);
        actionPtr->type1.egressInterface.trunkId          = (GT_TRUNK_ID)inFields[10];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type1.egressInterface.trunkId);
        actionPtr->type1.egressInterface.vidx             = (GT_U16)inFields[11];
        actionPtr->type1.egressInterface.vlanId           = (GT_U16)inFields[12];
        actionPtr->type1.tunnelStart                      = (GT_BOOL)inFields[13];
        actionPtr->type1.tunnelStartPtr                   = (GT_U32)inFields[14];
        actionPtr->type1.routerLookupPtr                  = (GT_U32)inFields[15];
        actionPtr->type1.vrfId                            = (GT_U32)inFields[16];
        /* the fields useVidx,targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
           from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be ignored */
        actionPtr->type1.sourceIdSetEnable                = (GT_BOOL)inFields[21];
        actionPtr->type1.sourceId                         = (GT_U32)inFields[22];
        actionPtr->type1.vlanCmd                          = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[23];
        actionPtr->type1.vlanId                           = (GT_U16)inFields[24];
        actionPtr->type1.vlanPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[25];
        actionPtr->type1.nestedVlanEnable                 = (GT_BOOL)inFields[26];
        actionPtr->type1.bindToPolicer                    = (GT_BOOL)inFields[27];
        actionPtr->type1.policerIndex                     = (GT_U32)inFields[28];
        actionPtr->type1.qosPrecedence                    = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[29];
        actionPtr->type1.qosTrustMode                     = (CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT)inFields[30];
        actionPtr->type1.qosProfile                       = (GT_U32)inFields[31];
        actionPtr->type1.modifyUpEnable                   = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[32];
        actionPtr->type1.modifyDscpEnable                 = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[33];
        actionPtr->type1.up                               = (GT_U32)inFields[34];
        actionPtr->type1.remapDSCP                        = (GT_BOOL)inFields[35];
        actionPtr->type1.mirrorToIngressAnalyzerEnable    = (GT_BOOL)inFields[36];
        actionPtr->type1.userDefinedCpuCode               = (CPSS_NET_RX_CPU_CODE_ENT)inFields[37];
        actionPtr->type1.vntl2Echo                        = (GT_BOOL)inFields[38];
        actionPtr->type1.bridgeBypass                     = (GT_BOOL)inFields[39];
        actionPtr->type1.actionStop                       = (GT_BOOL)inFields[40];
        actionPtr->type1.activateCounter                  = (GT_BOOL)inFields[41];
        actionPtr->type1.counterIndex                     = (GT_U32)inFields[42];
        break;

    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:

        actionPtr->type2.tunnelTerminate                 = (GT_BOOL)inFields[2];
        actionPtr->type2.ttPassengerPacketType           = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
        actionPtr->type2.tsPassengerPacketType           = (CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT)inFields[4];

        actionPtr->type2.ttHeaderLength                  = (GT_U32)inFields[5];
        actionPtr->type2.continueToNextTtiLookup         = (GT_BOOL)inFields[6];

        actionPtr->type2.copyTtlExpFromTunnelHeader      = (GT_BOOL)inFields[7];
        actionPtr->type2.mplsCommand                     = (CPSS_DXCH_TTI_MPLS_CMD_ENT)inFields[8];
        actionPtr->type2.mplsTtl                         = (GT_U32)inFields[9];
        actionPtr->type2.enableDecrementTtl              = (GT_BOOL)inFields[10];

        actionPtr->type2.passengerParsingOfTransitMplsTunnelMode = (CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_TYPE_ENT)inFields[11];
        actionPtr->type2.passengerParsingOfTransitNonMplsTransitTunnelEnable = (GT_BOOL)inFields[12];

        actionPtr->type2.command                         = (CPSS_PACKET_CMD_ENT)inFields[13];
        actionPtr->type2.redirectCommand                 = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[14];
        actionPtr->type2.egressInterface.type            = (CPSS_INTERFACE_TYPE_ENT)inFields[15];
        actionPtr->type2.egressInterface.devPort.hwDevNum  = (GT_HW_DEV_NUM)inFields[16];
        actionPtr->type2.egressInterface.devPort.portNum = (GT_PORT_NUM)inFields[17];
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionPtr->type2.egressInterface.devPort.hwDevNum, actionPtr->type2.egressInterface.devPort.portNum);
        actionPtr->type2.egressInterface.trunkId         = (GT_TRUNK_ID)inFields[18];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type2.egressInterface.trunkId);
        actionPtr->type2.egressInterface.vidx            = (GT_U16)inFields[19];
        actionPtr->type2.egressInterface.vlanId          = (GT_U16)inFields[20];
        actionPtr->type2.arpPtr                          = (GT_U32)inFields[21];
        actionPtr->type2.tunnelStart                     = (GT_BOOL)inFields[22];
        actionPtr->type2.tunnelStartPtr                  = (GT_U32)inFields[23];
        actionPtr->type2.routerLttPtr                    = (GT_U32)inFields[24];
        actionPtr->type2.vrfId                           = (GT_U32)inFields[25];
        actionPtr->type2.sourceIdSetEnable               = (GT_BOOL)inFields[26];
        actionPtr->type2.sourceId                        = (GT_U32)inFields[27];
        actionPtr->type2.tag0VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[28];
        actionPtr->type2.tag0VlanId                      = (GT_U16)inFields[29];
        actionPtr->type2.tag1VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[30];
        actionPtr->type2.tag1VlanId                      = (GT_U16)inFields[31];
        actionPtr->type2.tag0VlanPrecedence              = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[32];
        actionPtr->type2.nestedVlanEnable                = (GT_BOOL)inFields[33];
        actionPtr->type2.bindToPolicerMeter              = (GT_BOOL)inFields[34];
        actionPtr->type2.bindToPolicer                   = (GT_BOOL)inFields[35];
        actionPtr->type2.policerIndex                    = (GT_U32)inFields[36];
        actionPtr->type2.qosPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[37];
        actionPtr->type2.keepPreviousQoS                 = (GT_BOOL)inFields[38];
        actionPtr->type2.trustUp                         = (GT_BOOL)inFields[39];
        actionPtr->type2.trustDscp                       = (GT_BOOL)inFields[40];
        actionPtr->type2.trustExp                        = (GT_BOOL)inFields[41];
        actionPtr->type2.qosProfile                      = (GT_U32)inFields[42];
        actionPtr->type2.modifyTag0Up                    = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[43];
        actionPtr->type2.tag1UpCommand                   = (CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT)inFields[44];
        actionPtr->type2.modifyDscp                      = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[45];
        actionPtr->type2.tag0Up                          = (GT_U32)inFields[46];
        actionPtr->type2.tag1Up                          = (GT_U32)inFields[47];
        actionPtr->type2.remapDSCP                       = (GT_BOOL)inFields[48];

        actionPtr->type2.qosUseUpAsIndexEnable           = (GT_BOOL)inFields[49];
        actionPtr->type2.qosMappingTableIndex            = (GT_U32)inFields[50];
        actionPtr->type2.mplsLLspQoSProfileEnable        = (GT_BOOL)inFields[51];

        actionPtr->type2.pcl0OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[52];
        actionPtr->type2.pcl0_1OverrideConfigIndex       = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[53];
        actionPtr->type2.pcl1OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[54];
        actionPtr->type2.iPclConfigIndex                 = (GT_U32)inFields[55];

        actionPtr->type2.iPclUdbConfigTableEnable        = (GT_BOOL)inFields[56];
        actionPtr->type2.iPclUdbConfigTableIndex         = (CPSS_DXCH_PCL_PACKET_TYPE_ENT)inFields[57];

        actionPtr->type2.mirrorToIngressAnalyzerEnable   = (GT_BOOL)inFields[58];
        actionPtr->type2.mirrorToIngressAnalyzerIndex    = (GT_U32)inFields[59];

        actionPtr->type2.userDefinedCpuCode              = (CPSS_NET_RX_CPU_CODE_ENT)inFields[60];
        actionPtr->type2.bindToCentralCounter            = (GT_BOOL)inFields[61];
        actionPtr->type2.centralCounterIndex             = (GT_U32)inFields[62];
        actionPtr->type2.vntl2Echo                       = (GT_BOOL)inFields[63];
        actionPtr->type2.bridgeBypass                    = (GT_BOOL)inFields[64];
        actionPtr->type2.ingressPipeBypass               = (GT_BOOL)inFields[65];
        actionPtr->type2.actionStop                      = (GT_BOOL)inFields[66];
        actionPtr->type2.hashMaskIndex                   = (GT_U32)inFields[67];
        actionPtr->type2.modifyMacSa                     = (GT_BOOL)inFields[68];
        actionPtr->type2.modifyMacDa                     = (GT_BOOL)inFields[69];
        actionPtr->type2.ResetSrcPortGroupId             = (GT_BOOL)inFields[70];
        actionPtr->type2.multiPortGroupTtiEnable         = (GT_BOOL)inFields[71];

        actionPtr->type2.sourceEPortAssignmentEnable     = (GT_BOOL)inFields[72];
        actionPtr->type2.sourceEPort                     = (GT_PORT_NUM)inFields[73];
        actionPtr->type2.flowId                          = (GT_U32)inFields[74];
        actionPtr->type2.setMacToMe                      = (GT_BOOL)inFields[75];
        actionPtr->type2.rxProtectionSwitchEnable        = (GT_BOOL)inFields[76];
        actionPtr->type2.rxIsProtectionPath              = (GT_BOOL)inFields[77];
        actionPtr->type2.pwTagMode                       = (CPSS_DXCH_TTI_PW_TAG_MODE_ENT)inFields[78];

        actionPtr->type2.oamTimeStampEnable              = (GT_BOOL)inFields[79];
        actionPtr->type2.oamOffsetIndex                  = (GT_U32)inFields[80];
        actionPtr->type2.oamProcessEnable                = (GT_BOOL)inFields[81];
        actionPtr->type2.oamProfile                      = (GT_U32)inFields[82];
        actionPtr->type2.oamChannelTypeToOpcodeMappingEnable = (GT_BOOL)inFields[83];

        actionPtr->type2.isPtpPacket                     = (GT_BOOL)inFields[84];
        actionPtr->type2.ptpTriggerType                  = (CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_ENT)inFields[85];
        actionPtr->type2.ptpOffset                       = (GT_U32)inFields[86];

        actionPtr->type2.cwBasedPw                       = (GT_BOOL)inFields[87];
        actionPtr->type2.ttlExpiryVccvEnable             = (GT_BOOL)inFields[88];
        actionPtr->type2.pwe3FlowLabelExist              = (GT_BOOL)inFields[89];
        actionPtr->type2.pwCwBasedETreeEnable            = (GT_BOOL)inFields[90];
        actionPtr->type2.applyNonDataCwCommand           = (GT_BOOL)inFields[91];
        break;

    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_AGENT_ERROR;
    }

    /* call cpss api function - ruleIndex is an absolute index */
    result = pg_wrap_cpssDxChTtiRuleActionUpdate(devNum,ruleIndex,ttiActionDatabase[index].actionType,actionPtr,GT_TRUE,isNewIndex);

    /* if the rule action didn't succeed:  */
    if (result != GT_OK)
    {
        /* if this is a new action remove it from the database */
        if (isNewIndex)
        {
            ttiActionDatabase[index].valid = GT_FALSE;
            ttiActionDatabase[index].nextFree = firstFree;
            firstFree = index;
        }
        /* if this is an update for existing action restore previous data */
        else
        {
            cpssOsMemCpy(&ttiActionDatabase[index],&ttiTempAction,sizeof(ttiTempAction));
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_5Get
*
* DESCRIPTION:
*       This function gets single rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_5Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_ACTION_UNT    *actionPtr;
    GT_HW_DEV_NUM               tempHwDev;    /* used for port,dev converting */
    GT_PORT_NUM                 tempPort;   /* used for port,dev converting */

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* get next valid TTI action entry */
    while ((ttiActionGetIndex < ACTION_TABLE_SIZE) &&
           (ttiActionDatabase[ttiActionGetIndex].valid == GT_FALSE))
        ttiActionGetIndex++;

    if (ttiActionGetIndex == ACTION_TABLE_SIZE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    actionPtr = &(ttiActionDatabase[ttiActionGetIndex].actionEntry);

    switch (ttiActionDatabase[ttiActionGetIndex].actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:

        inArgs[0] = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

        inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
        inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
        inFields[2]  = actionPtr->type1.tunnelTerminate;
        inFields[3]  = actionPtr->type1.passengerPacketType;
        inFields[4]  = actionPtr->type1.copyTtlFromTunnelHeader;
        inFields[5]  = actionPtr->type1.command;
        inFields[6]  = actionPtr->type1.redirectCommand;
        inFields[7]  = actionPtr->type1.egressInterface.type;
        tempHwDev      = actionPtr->type1.egressInterface.devPort.hwDevNum;
        tempPort     = actionPtr->type1.egressInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempHwDev,tempPort);
        inFields[8]  = tempHwDev;
        inFields[9]  = tempPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type1.egressInterface.trunkId);
        inFields[10] = actionPtr->type1.egressInterface.trunkId;
        inFields[11] = actionPtr->type1.egressInterface.vidx;
        inFields[12] = actionPtr->type1.egressInterface.vlanId;
        inFields[13] = actionPtr->type1.tunnelStart;
        inFields[14] = actionPtr->type1.tunnelStartPtr;
        inFields[15] = actionPtr->type1.routerLookupPtr;
        inFields[16] = actionPtr->type1.vrfId;
        /* the fields targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
           from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be hardcoded  */
        inFields[17] = GT_FALSE;
        inFields[18] = 0;
        inFields[19] = 0;
        inFields[20] = 0;
        inFields[21] = actionPtr->type1.sourceIdSetEnable;
        inFields[22] = actionPtr->type1.sourceId;
        inFields[23] = actionPtr->type1.vlanCmd;
        inFields[24] = actionPtr->type1.vlanId;
        inFields[25] = actionPtr->type1.vlanPrecedence;
        inFields[26] = actionPtr->type1.nestedVlanEnable;
        inFields[27] = actionPtr->type1.bindToPolicer;
        inFields[28] = actionPtr->type1.policerIndex;
        inFields[29] = actionPtr->type1.qosPrecedence;
        inFields[30] = actionPtr->type1.qosTrustMode;
        inFields[31] = actionPtr->type1.qosProfile;
        inFields[32] = actionPtr->type1.modifyUpEnable;
        inFields[33] = actionPtr->type1.modifyDscpEnable;
        inFields[34] = actionPtr->type1.up;
        inFields[35] = actionPtr->type1.remapDSCP;
        inFields[36] = actionPtr->type1.mirrorToIngressAnalyzerEnable;
        inFields[37] = actionPtr->type1.userDefinedCpuCode;
        inFields[38] = actionPtr->type1.vntl2Echo;
        inFields[39] = actionPtr->type1.bridgeBypass;
        inFields[40] = actionPtr->type1.actionStop;
        inFields[41] = actionPtr->type1.activateCounter;
        inFields[42] = actionPtr->type1.counterIndex;

        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],   inFields[2],   inFields[3],
                    inFields[4],  inFields[5],   inFields[6],   inFields[7],
                    inFields[8],  inFields[9],   inFields[10],  inFields[11],
                    inFields[12], inFields[13],  inFields[14],  inFields[15],
                    inFields[16], inFields[17],  inFields[18],  inFields[19],
                    inFields[20], inFields[21],  inFields[22],  inFields[23],
                    inFields[24], inFields[25],  inFields[26],  inFields[27],
                    inFields[28], inFields[29],  inFields[30],  inFields[31],
                    inFields[32], inFields[33],  inFields[34],  inFields[35],
                    inFields[36], inFields[37],  inFields[38],  inFields[39],
                    inFields[40], inFields[41],  inFields[42]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d%f", 0);
        break;

    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:

        inArgs[0] = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
        inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
        inFields[2]  = actionPtr->type2.tunnelTerminate;
        inFields[3]  = actionPtr->type2.ttPassengerPacketType;
        inFields[4]  = actionPtr->type2.tsPassengerPacketType;
        inFields[5]  = actionPtr->type2.ttHeaderLength;
        inFields[6]  = actionPtr->type2.continueToNextTtiLookup;
        inFields[7]  = actionPtr->type2.copyTtlExpFromTunnelHeader;
        inFields[8]  = actionPtr->type2.mplsCommand;
        inFields[9]  = actionPtr->type2.mplsTtl;
        inFields[10]  = actionPtr->type2.enableDecrementTtl;
        inFields[11]  = actionPtr->type2.passengerParsingOfTransitMplsTunnelMode;
        inFields[12]  = actionPtr->type2.passengerParsingOfTransitNonMplsTransitTunnelEnable;
        inFields[13]  = actionPtr->type2.command;
        inFields[14] = actionPtr->type2.redirectCommand;
        inFields[15] = actionPtr->type2.egressInterface.type;
        tempHwDev      = actionPtr->type2.egressInterface.devPort.hwDevNum;
        tempPort     = actionPtr->type2.egressInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempHwDev,tempPort);
        inFields[16] = tempHwDev;
        inFields[17] = tempPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type2.egressInterface.trunkId);
        inFields[18] = actionPtr->type2.egressInterface.trunkId;
        inFields[19] = actionPtr->type2.egressInterface.vidx;
        inFields[20] = actionPtr->type2.egressInterface.vlanId;
        inFields[21] = actionPtr->type2.arpPtr;
        inFields[22] = actionPtr->type2.tunnelStart;
        inFields[23] = actionPtr->type2.tunnelStartPtr;
        inFields[24] = actionPtr->type2.routerLttPtr;
        inFields[25] = actionPtr->type2.vrfId;
        inFields[26] = actionPtr->type2.sourceIdSetEnable;
        inFields[27] = actionPtr->type2.sourceId;
        inFields[28] = actionPtr->type2.tag0VlanCmd;
        inFields[29] = actionPtr->type2.tag0VlanId;
        inFields[30] = actionPtr->type2.tag1VlanCmd;
        inFields[31] = actionPtr->type2.tag1VlanId;
        inFields[32] = actionPtr->type2.tag0VlanPrecedence;
        inFields[33] = actionPtr->type2.nestedVlanEnable;
        inFields[34] = actionPtr->type2.bindToPolicerMeter;
        inFields[35] = actionPtr->type2.bindToPolicer;
        inFields[36] = actionPtr->type2.policerIndex;
        inFields[37] = actionPtr->type2.qosPrecedence;
        inFields[38] = actionPtr->type2.keepPreviousQoS;
        inFields[39] = actionPtr->type2.trustUp;
        inFields[40] = actionPtr->type2.trustDscp;
        inFields[41] = actionPtr->type2.trustExp;
        inFields[42] = actionPtr->type2.qosProfile;
        inFields[43] = actionPtr->type2.modifyTag0Up;
        inFields[44] = actionPtr->type2.tag1UpCommand;
        inFields[45] = actionPtr->type2.modifyDscp;
        inFields[46] = actionPtr->type2.tag0Up;
        inFields[47] = actionPtr->type2.tag1Up;
        inFields[48] = actionPtr->type2.remapDSCP;
        inFields[49] = actionPtr->type2.qosUseUpAsIndexEnable;
        inFields[50] = actionPtr->type2.qosMappingTableIndex;
        inFields[51] = actionPtr->type2.mplsLLspQoSProfileEnable;
        inFields[52] = actionPtr->type2.pcl0OverrideConfigIndex;
        inFields[53] = actionPtr->type2.pcl0_1OverrideConfigIndex;
        inFields[54] = actionPtr->type2.pcl1OverrideConfigIndex;
        inFields[55] = actionPtr->type2.iPclConfigIndex;
        inFields[56] = actionPtr->type2.iPclUdbConfigTableEnable;
        inFields[57] = actionPtr->type2.iPclUdbConfigTableIndex;
        inFields[58] = actionPtr->type2.mirrorToIngressAnalyzerEnable;
        inFields[59] = actionPtr->type2.mirrorToIngressAnalyzerIndex;
        inFields[60] = actionPtr->type2.userDefinedCpuCode;
        inFields[61] = actionPtr->type2.bindToCentralCounter;
        inFields[62] = actionPtr->type2.centralCounterIndex;
        inFields[63] = actionPtr->type2.vntl2Echo;
        inFields[64] = actionPtr->type2.bridgeBypass;
        inFields[65] = actionPtr->type2.ingressPipeBypass;
        inFields[66] = actionPtr->type2.actionStop;
        inFields[67] = actionPtr->type2.hashMaskIndex;
        inFields[68] = actionPtr->type2.modifyMacSa;
        inFields[69] = actionPtr->type2.modifyMacDa;
        inFields[70] = actionPtr->type2.ResetSrcPortGroupId;
        inFields[71] = actionPtr->type2.multiPortGroupTtiEnable;
        inFields[72] = actionPtr->type2.sourceEPortAssignmentEnable;
        inFields[73] = actionPtr->type2.sourceEPort;
        inFields[74] = actionPtr->type2.flowId;
        inFields[75] = actionPtr->type2.setMacToMe;
        inFields[76] = actionPtr->type2.rxProtectionSwitchEnable;
        inFields[77] = actionPtr->type2.rxIsProtectionPath;
        inFields[78] = actionPtr->type2.pwTagMode;
        inFields[79] = actionPtr->type2.oamTimeStampEnable;
        inFields[80] = actionPtr->type2.oamOffsetIndex;
        inFields[81] = actionPtr->type2.oamProcessEnable;
        inFields[82] = actionPtr->type2.oamProfile;
        inFields[83] = actionPtr->type2.oamChannelTypeToOpcodeMappingEnable;
        inFields[84] = actionPtr->type2.isPtpPacket;
        inFields[85] = actionPtr->type2.ptpTriggerType;
        inFields[86] = actionPtr->type2.ptpOffset;
        inFields[87] = actionPtr->type2.cwBasedPw;
        inFields[88] = actionPtr->type2.ttlExpiryVccvEnable;
        inFields[89] = actionPtr->type2.pwe3FlowLabelExist;
        inFields[90] = actionPtr->type2.pwCwBasedETreeEnable;
        inFields[91] = actionPtr->type2.applyNonDataCwCommand;

        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],   inFields[2],   inFields[3],
                    inFields[4],  inFields[5],   inFields[6],   inFields[7],
                    inFields[8],  inFields[9],   inFields[10],  inFields[11],
                    inFields[12], inFields[13],  inFields[14],  inFields[15],
                    inFields[16], inFields[17],  inFields[18],  inFields[19],
                    inFields[20], inFields[21],  inFields[22],  inFields[23],
                    inFields[24], inFields[25],  inFields[26],  inFields[27],
                    inFields[28], inFields[29],  inFields[30],  inFields[31],
                    inFields[32], inFields[33],  inFields[34],  inFields[35],
                    inFields[36], inFields[37],  inFields[38],  inFields[39],
                    inFields[40], inFields[41],  inFields[42],  inFields[43],
                    inFields[44], inFields[45],  inFields[46],  inFields[47],
                    inFields[48], inFields[49],  inFields[50],  inFields[51],
                    inFields[52], inFields[53],  inFields[54],  inFields[55],
                    inFields[56], inFields[57],  inFields[58],  inFields[59],
                    inFields[60], inFields[61],  inFields[62],  inFields[63],
                    inFields[64], inFields[65],  inFields[66],  inFields[67],
                    inFields[68], inFields[69],  inFields[70],  inFields[71],
                    inFields[72], inFields[73],  inFields[74],  inFields[75],
                    inFields[76], inFields[77],  inFields[78],  inFields[79],
                    inFields[80], inFields[81],  inFields[82],  inFields[83],
                    inFields[84], inFields[85],  inFields[86],  inFields[87],
                    inFields[88], inFields[89],  inFields[90],  inFields[91]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d%f", 1);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI ACTION FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    ttiActionGetIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_5GetFirst
*
* DESCRIPTION:
*       This function gets first rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_5GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ttiActionGetIndex = 0;

    return wrCpssDxChTtiRuleAction_5Get(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRule_6_KEY_MPLS_Set
*
* DESCRIPTION:
*       This function maps input arguments to MPLS TTI rule pattern/mask.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_6_KEY_MPLS_Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;

    /* map input arguments to locals */

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (inFields[1] == 0) /* mask */
    {
        rulePtr = &maskData;
    }
    else /* pattern */
    {
        rulePtr = &patternData;
    }

    /* MPLS key in TtiRule_6 table is the same as in TtiRule_2 table except for field cwFirstNibble */
    wrCpssDxChTtiRule_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
    rulePtr->mpls.common.sourcePortGroupId = (GT_U32)inFields[19];
    rulePtr->mpls.cwFirstNibble = (GT_U32)inFields[20];

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_6SetFirst
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_6SetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));

    mask_set    = GT_FALSE;
    pattern_set = GT_FALSE;

    switch (inArgs[3]) /* key type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_6_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_2_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_6SetNext
*
* DESCRIPTION:
*       This function sets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_6SetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;
    GT_U8                           devNum;
    GT_U32                          ruleIndex;
    CPSS_DXCH_TTI_KEY_TYPE_ENT      keyType;
    GT_U32                          actionIndex;
    CPSS_DXCH_TTI_ACTION_TYPE_ENT   actionType;
    CPSS_DXCH_TTI_ACTION_UNT        *actionPtr;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Check Validity */
    if (inFields[1] > 1)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong value for data_type (should be MASK or PATTERN).\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 0) && mask_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    if ((inFields[1] == 1) && pattern_set)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Must be subsequent pair of {Rule,Mask}.\n");
        return CMD_AGENT_ERROR;
    }

    /* map input arguments to locals */
    devNum    = (GT_U8)inArgs[0];
    ruleIndex = (GT_U32)inFields[0];

    switch (inArgs[3]) /* key type tab */
    {
    case CPSS_DXCH_TTI_KEY_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MPLS_E:
        wrCpssDxChTtiRule_6_KEY_MPLS_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Set(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_KEY_MIM_E:
        wrCpssDxChTtiRule_2_KEY_MIM_Set(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (mask_set && pattern_set && (mask_ruleIndex != pattern_ruleIndex))
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : mask ruleIndex doesn't match pattern ruleIndex.\n");
        return CMD_AGENT_ERROR;
    }


    /* Get the action */
    ttiActionDatabaseGet(devNum,ruleIndex,&actionIndex);

    if ((mask_set && pattern_set) && (actionIndex != INVALID_RULE_INDEX))
    {
        actionPtr                    = &(ttiActionDatabase[actionIndex].actionEntry);
        keyType                      = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[3];
        ttiRuleFormat[ruleIndex]     = keyType;
        actionType                   = ttiActionDatabase[actionIndex].actionType;
        ttiRuleActionType[ruleIndex] = actionType;

        /* call cpss api function */
        result = pg_wrap_cpssDxChTtiRuleSet(devNum,ruleIndex,keyType,&patternData,&maskData,actionType,actionPtr,GT_FALSE);

        mask_set = GT_FALSE;
        pattern_set = GT_FALSE;

        cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
        cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_6_KEY_MPLS_Get
*
* DESCRIPTION:
*       This function maps MPLS TTI rule pattern/mask to output fields.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_6_KEY_MPLS_Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    CPSS_DXCH_TTI_RULE_UNT  *rulePtr;
    GT_U8 devNum;
    GT_PORT_NUM port;

    ruleFormat = CPSS_DXCH_TTI_KEY_MPLS_E;

    /* inFields[1] indicate if mask or pattern (0: mask, 1: pattern) */
    if (isMask)
    {
        rulePtr     = &maskData;
        inFields[0] = ruleIndex;
        inFields[1] = 0;
        isMask      = GT_FALSE;

        /* convert CPSS trunk fields to test trunk fields */
        ttiRuleTrunkInfoFromCpssToTest_1();
    }
    else
    {
        rulePtr     = &patternData;
        inFields[0] = ruleIndex;
        inFields[1] = 1;
        isMask      = GT_TRUE;
    }

    inFields[2]  = rulePtr->mpls.common.pclId;
    inFields[3]  = rulePtr->mpls.common.srcIsTrunk;
    inFields[4]  = rulePtr->mpls.common.srcPortTrunk;

    if (rulePtr->mpls.common.srcIsTrunk == GT_FALSE)
    {
        devNum = (GT_U8)inArgs[0];
        port = (GT_PORT_NUM)rulePtr->mpls.common.srcPortTrunk;

        /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
        if (inFields[1] == 1) /* pattern */
            CONVERT_DEV_PORT_U32_MAC(devNum,port);

        inFields[4] = (GT_U32)port;
    }

    inFields[6]  = rulePtr->mpls.common.vid;
    inFields[7]  = rulePtr->mpls.common.isTagged;
    inFields[8]  = rulePtr->mpls.label0;
    inFields[9]  = rulePtr->mpls.exp0 ;
    inFields[10] = rulePtr->mpls.label1;
    inFields[11] = rulePtr->mpls.exp1;
    inFields[12] = rulePtr->mpls.label2;
    inFields[13] = rulePtr->mpls.exp2;
    inFields[14] = rulePtr->mpls.numOfLabels;
    inFields[15] = rulePtr->mpls.protocolAboveMPLS;
    inFields[16] = rulePtr->mpls.common.dsaSrcIsTrunk;
    inFields[17] = rulePtr->mpls.common.dsaSrcPortTrunk;
    inFields[18] = rulePtr->mpls.common.dsaSrcDevice;
    inFields[19] = rulePtr->mpls.common.sourcePortGroupId;
    inFields[20] = rulePtr->mpls.cwFirstNibble;

    fieldOutput("%d%d%d%d%d%6b%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1],  inFields[2],  inFields[3],
                inFields[4], rulePtr->mpls.common.mac.arEther,  inFields[6],
                inFields[7], inFields[8], inFields[9],  inFields[10],
                inFields[11], inFields[12],inFields[13], inFields[14],
                inFields[15],inFields[16],inFields[17],inFields[18],inFields[19],
                inFields[20]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", ruleFormat);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_6Get
*
* DESCRIPTION:
*       This function gets TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_6Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;
    GT_U8                       devNum;
    CPSS_DXCH_TTI_ACTION_UNT    action;
    GT_BOOL                     ruleValid;

    /* if isMask == GT_TRUE it is mask's turn (the first one to get), so there need to refresh the data
    else the data is updated and it is pattern's turn (was updated earlier)*/
    if (isMask)
    {
        /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];

        /* look for another valid rule within the range */
        for ((ruleValid = GT_FALSE) ; ((ruleValid == GT_FALSE) && (ruleIndex < 3072) && (ruleIndex <= ruleIndexMaxGet)) ; ruleIndex++)
        {
            /* check if the rule is valid */
            result = pg_wrap_cpssDxChTtiRuleValidStatusGet(devNum,ruleIndex,&ruleValid,GT_FALSE);
            if (result != GT_OK)
            {
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }
        }

        /* check if no valid rule was found in the requested range */
        if ((ruleValid == GT_FALSE))
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        /* valid rule was found in the requested range */
        ruleIndex--;

        /* call cpss api function, note that it is assumed that rule action
           type is not modified after setting the rule */
        result = pg_wrap_cpssDxChTtiRuleGet(devNum,ruleIndex,ttiRuleFormat[ruleIndex],&patternData,&maskData,ttiRuleActionType[ruleIndex],&action,GT_FALSE);
    }

    switch (ttiRuleFormat[ruleIndex])
    {
    case CPSS_DXCH_TTI_RULE_IPV4_E:
        wrCpssDxChTtiRule_2_KEY_IPV4_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_RULE_MPLS_E:
        wrCpssDxChTtiRule_6_KEY_MPLS_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_RULE_ETH_E:
        wrCpssDxChTtiRule_2_KEY_ETH_Get(inArgs,inFields,numFields,outArgs);
        break;

    case CPSS_DXCH_TTI_RULE_MIM_E:
        wrCpssDxChTtiRule_2_KEY_MIM_Get(inArgs,inFields,numFields,outArgs);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    if (isMask)
        ruleIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRule_6GetFirst
*
* DESCRIPTION:
*       This function gets first TTI rule.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRule_6GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ruleIndex = 0;

    /* first to get is mask */
    isMask = GT_TRUE;

    /* check if the application request 0 entries */
    if (inArgs[2] == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    ruleIndex       = (GT_U32)inArgs[1];
    ruleIndexMaxGet = ruleIndex + (GT_U32)inArgs[2] - 1;

    return wrCpssDxChTtiRule_6Get(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_6Update
*
* DESCRIPTION:
*       This function updates rule action.
*       Note: the rule index is an absolute index in the TCAM.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_6Update
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result = GT_OK;
    GT_U8                               devNum;
    GT_U32                              ruleIndex;
    GT_U32                              index;
    CPSS_DXCH_TTI_ACTION_UNT            *actionPtr;
    GT_BOOL                             isNewIndex = GT_FALSE;
    ttiActionDB                         ttiTempAction = {0};

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum    = (GT_U8)inFields[0];
    ruleIndex = (GT_U32)inFields[1];

    /* in case it is first usage of TTI action database, initialize it */
    if (firstRun)
    {
        ttiActionDatabaseInit();
        firstRun = GT_FALSE;
    }

    /* look for the action in the TTI action database */
    ttiActionDatabaseGet(devNum, ruleIndex, &index);

    /* the action wasn't in the TTI action database */
    if (index == INVALID_RULE_INDEX)
    {
        if (firstFree == INVALID_RULE_INDEX)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, GT_NO_RESOURCE, "%d", -1);
            return CMD_AGENT_ERROR;
        }

        index = firstFree;
        firstFree = ttiActionDatabase[firstFree].nextFree;
        isNewIndex = GT_TRUE;
    }

    /* this is update of an existing action:
       save the old action parameters */
    if (isNewIndex == GT_FALSE)
    {
        cpssOsMemCpy(&ttiTempAction,&ttiActionDatabase[index],sizeof(ttiTempAction));
    }

    /* set action entry in TTI action database as valid */
    ttiActionDatabase[index].valid      = GT_TRUE;
    ttiActionDatabase[index].ruleIndex  = ruleIndex;
    ttiActionDatabase[index].devNum     = devNum;
    ttiActionDatabase[index].actionType = (CPSS_DXCH_TTI_ACTION_TYPE_ENT)inArgs[0];

    actionPtr = &(ttiActionDatabase[index].actionEntry);

    /* fill action fields */
    switch (ttiActionDatabase[index].actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:

        actionPtr->type1.tunnelTerminate                  = (GT_BOOL)inFields[2];
        actionPtr->type1.passengerPacketType              = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
        actionPtr->type1.copyTtlFromTunnelHeader          = (GT_BOOL)inFields[4];
        actionPtr->type1.command                          = (CPSS_PACKET_CMD_ENT)inFields[5];
        actionPtr->type1.redirectCommand                  = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[6];
        actionPtr->type1.egressInterface.type             = (CPSS_INTERFACE_TYPE_ENT)inFields[7];
        actionPtr->type1.egressInterface.devPort.hwDevNum   = (GT_HW_DEV_NUM)inFields[8];
        actionPtr->type1.egressInterface.devPort.portNum  = (GT_PORT_NUM)inFields[9];
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionPtr->type1.egressInterface.devPort.hwDevNum, actionPtr->type1.egressInterface.devPort.portNum);
        actionPtr->type1.egressInterface.trunkId          = (GT_TRUNK_ID)inFields[10];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type1.egressInterface.trunkId);
        actionPtr->type1.egressInterface.vidx             = (GT_U16)inFields[11];
        actionPtr->type1.egressInterface.vlanId           = (GT_U16)inFields[12];
        actionPtr->type1.tunnelStart                      = (GT_BOOL)inFields[13];
        actionPtr->type1.tunnelStartPtr                   = (GT_U32)inFields[14];
        actionPtr->type1.routerLookupPtr                  = (GT_U32)inFields[15];
        actionPtr->type1.vrfId                            = (GT_U32)inFields[16];
        /* the fields useVidx,targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
           from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be ignored */
        actionPtr->type1.sourceIdSetEnable                = (GT_BOOL)inFields[21];
        actionPtr->type1.sourceId                         = (GT_U32)inFields[22];
        actionPtr->type1.vlanCmd                          = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[23];
        actionPtr->type1.vlanId                           = (GT_U16)inFields[24];
        actionPtr->type1.vlanPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[25];
        actionPtr->type1.nestedVlanEnable                 = (GT_BOOL)inFields[26];
        actionPtr->type1.bindToPolicer                    = (GT_BOOL)inFields[27];
        actionPtr->type1.policerIndex                     = (GT_U32)inFields[28];
        actionPtr->type1.qosPrecedence                    = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[29];
        actionPtr->type1.qosTrustMode                     = (CPSS_DXCH_TTI_QOS_MODE_TYPE_ENT)inFields[30];
        actionPtr->type1.qosProfile                       = (GT_U32)inFields[31];
        actionPtr->type1.modifyUpEnable                   = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[32];
        actionPtr->type1.modifyDscpEnable                 = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[33];
        actionPtr->type1.up                               = (GT_U32)inFields[34];
        actionPtr->type1.remapDSCP                        = (GT_BOOL)inFields[35];
        actionPtr->type1.mirrorToIngressAnalyzerEnable    = (GT_BOOL)inFields[36];
        actionPtr->type1.userDefinedCpuCode               = (CPSS_NET_RX_CPU_CODE_ENT)inFields[37];
        actionPtr->type1.vntl2Echo                        = (GT_BOOL)inFields[38];
        actionPtr->type1.bridgeBypass                     = (GT_BOOL)inFields[39];
        actionPtr->type1.actionStop                       = (GT_BOOL)inFields[40];
        actionPtr->type1.activateCounter                  = (GT_BOOL)inFields[41];
        actionPtr->type1.counterIndex                     = (GT_U32)inFields[42];
        break;

    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:

        actionPtr->type2.tunnelTerminate                 = (GT_BOOL)inFields[2];
        actionPtr->type2.ttPassengerPacketType           = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inFields[3];
        actionPtr->type2.tsPassengerPacketType           = (CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT)inFields[4];

        actionPtr->type2.ttHeaderLength                  = (GT_U32)inFields[5];
        actionPtr->type2.continueToNextTtiLookup         = (GT_BOOL)inFields[6];

        actionPtr->type2.copyTtlExpFromTunnelHeader      = (GT_BOOL)inFields[7];
        actionPtr->type2.mplsCommand                     = (CPSS_DXCH_TTI_MPLS_CMD_ENT)inFields[8];
        actionPtr->type2.mplsTtl                         = (GT_U32)inFields[9];
        actionPtr->type2.enableDecrementTtl              = (GT_BOOL)inFields[10];

        actionPtr->type2.passengerParsingOfTransitMplsTunnelMode = (CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_TYPE_ENT)inFields[11];
        actionPtr->type2.passengerParsingOfTransitNonMplsTransitTunnelEnable = (GT_BOOL)inFields[12];

        actionPtr->type2.command                         = (CPSS_PACKET_CMD_ENT)inFields[13];
        actionPtr->type2.redirectCommand                 = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inFields[14];
        actionPtr->type2.egressInterface.type            = (CPSS_INTERFACE_TYPE_ENT)inFields[15];
        actionPtr->type2.egressInterface.devPort.hwDevNum  = (GT_HW_DEV_NUM)inFields[16];
        actionPtr->type2.egressInterface.devPort.portNum = (GT_PORT_NUM)inFields[17];
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionPtr->type2.egressInterface.devPort.hwDevNum, actionPtr->type2.egressInterface.devPort.portNum);
        actionPtr->type2.egressInterface.trunkId         = (GT_TRUNK_ID)inFields[18];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionPtr->type2.egressInterface.trunkId);
        actionPtr->type2.egressInterface.vidx            = (GT_U16)inFields[19];
        actionPtr->type2.egressInterface.vlanId          = (GT_U16)inFields[20];
        actionPtr->type2.arpPtr                          = (GT_U32)inFields[21];
        actionPtr->type2.tunnelStart                     = (GT_BOOL)inFields[22];
        actionPtr->type2.tunnelStartPtr                  = (GT_U32)inFields[23];
        actionPtr->type2.routerLttPtr                    = (GT_U32)inFields[24];
        actionPtr->type2.vrfId                           = (GT_U32)inFields[25];
        actionPtr->type2.sourceIdSetEnable               = (GT_BOOL)inFields[26];
        actionPtr->type2.sourceId                        = (GT_U32)inFields[27];
        actionPtr->type2.tag0VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[28];
        actionPtr->type2.tag0VlanId                      = (GT_U16)inFields[29];
        actionPtr->type2.tag1VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inFields[30];
        actionPtr->type2.tag1VlanId                      = (GT_U16)inFields[31];
        actionPtr->type2.tag0VlanPrecedence              = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[32];
        actionPtr->type2.nestedVlanEnable                = (GT_BOOL)inFields[33];
        actionPtr->type2.bindToPolicerMeter              = (GT_BOOL)inFields[34];
        actionPtr->type2.bindToPolicer                   = (GT_BOOL)inFields[35];
        actionPtr->type2.policerIndex                    = (GT_U32)inFields[36];
        actionPtr->type2.qosPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[37];
        actionPtr->type2.keepPreviousQoS                 = (GT_BOOL)inFields[38];
        actionPtr->type2.trustUp                         = (GT_BOOL)inFields[39];
        actionPtr->type2.trustDscp                       = (GT_BOOL)inFields[40];
        actionPtr->type2.trustExp                        = (GT_BOOL)inFields[41];
        actionPtr->type2.qosProfile                      = (GT_U32)inFields[42];
        actionPtr->type2.modifyTag0Up                    = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inFields[43];
        actionPtr->type2.tag1UpCommand                   = (CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT)inFields[44];
        actionPtr->type2.modifyDscp                      = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inFields[45];
        actionPtr->type2.tag0Up                          = (GT_U32)inFields[46];
        actionPtr->type2.tag1Up                          = (GT_U32)inFields[47];
        actionPtr->type2.remapDSCP                       = (GT_BOOL)inFields[48];

        actionPtr->type2.qosUseUpAsIndexEnable           = (GT_BOOL)inFields[49];
        actionPtr->type2.qosMappingTableIndex            = (GT_U32)inFields[50];
        actionPtr->type2.mplsLLspQoSProfileEnable        = (GT_BOOL)inFields[51];

        actionPtr->type2.pcl0OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[52];
        actionPtr->type2.pcl0_1OverrideConfigIndex       = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[53];
        actionPtr->type2.pcl1OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inFields[54];
        actionPtr->type2.iPclConfigIndex                 = (GT_U32)inFields[55];

        actionPtr->type2.iPclUdbConfigTableEnable        = (GT_BOOL)inFields[56];
        actionPtr->type2.iPclUdbConfigTableIndex         = (CPSS_DXCH_PCL_PACKET_TYPE_ENT)inFields[57];

        actionPtr->type2.mirrorToIngressAnalyzerEnable   = (GT_BOOL)inFields[58];
        actionPtr->type2.mirrorToIngressAnalyzerIndex    = (GT_U32)inFields[59];

        actionPtr->type2.userDefinedCpuCode              = (CPSS_NET_RX_CPU_CODE_ENT)inFields[60];
        actionPtr->type2.bindToCentralCounter            = (GT_BOOL)inFields[61];
        actionPtr->type2.centralCounterIndex             = (GT_U32)inFields[62];
        actionPtr->type2.vntl2Echo                       = (GT_BOOL)inFields[63];
        actionPtr->type2.bridgeBypass                    = (GT_BOOL)inFields[64];
        actionPtr->type2.ingressPipeBypass               = (GT_BOOL)inFields[65];
        actionPtr->type2.actionStop                      = (GT_BOOL)inFields[66];
        actionPtr->type2.hashMaskIndex                   = (GT_U32)inFields[67];
        actionPtr->type2.modifyMacSa                     = (GT_BOOL)inFields[68];
        actionPtr->type2.modifyMacDa                     = (GT_BOOL)inFields[69];
        actionPtr->type2.ResetSrcPortGroupId             = (GT_BOOL)inFields[70];
        actionPtr->type2.multiPortGroupTtiEnable         = (GT_BOOL)inFields[71];

        actionPtr->type2.sourceEPortAssignmentEnable     = (GT_BOOL)inFields[72];
        actionPtr->type2.sourceEPort                     = (GT_PORT_NUM)inFields[73];
        actionPtr->type2.flowId                          = (GT_U32)inFields[74];
        actionPtr->type2.setMacToMe                      = (GT_BOOL)inFields[75];
        actionPtr->type2.rxProtectionSwitchEnable        = (GT_BOOL)inFields[76];
        actionPtr->type2.rxIsProtectionPath              = (GT_BOOL)inFields[77];
        actionPtr->type2.pwTagMode                       = (CPSS_DXCH_TTI_PW_TAG_MODE_ENT)inFields[78];

        actionPtr->type2.oamTimeStampEnable              = (GT_BOOL)inFields[79];
        actionPtr->type2.oamOffsetIndex                  = (GT_U32)inFields[80];
        actionPtr->type2.oamProcessEnable                = (GT_BOOL)inFields[81];
        actionPtr->type2.oamProfile                      = (GT_U32)inFields[82];
        actionPtr->type2.oamChannelTypeToOpcodeMappingEnable = (GT_BOOL)inFields[83];

        actionPtr->type2.isPtpPacket                     = (GT_BOOL)inFields[84];
        actionPtr->type2.ptpTriggerType                  = (CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_ENT)inFields[85];
        actionPtr->type2.ptpOffset                       = (GT_U32)inFields[86];

        actionPtr->type2.cwBasedPw                       = (GT_BOOL)inFields[87];
        actionPtr->type2.ttlExpiryVccvEnable             = (GT_BOOL)inFields[88];
        actionPtr->type2.pwe3FlowLabelExist              = (GT_BOOL)inFields[89];
        actionPtr->type2.pwCwBasedETreeEnable            = (GT_BOOL)inFields[90];
        actionPtr->type2.applyNonDataCwCommand           = (GT_BOOL)inFields[91];

        actionPtr->type2.unknownSaCommandEnable          = (GT_BOOL)inFields[92];
        actionPtr->type2.unknownSaCommand                = (GT_BOOL)inFields[93];
        actionPtr->type2.sourceMeshIdSetEnable           = (GT_BOOL)inFields[94];
        actionPtr->type2.sourceMeshId                    = (GT_BOOL)inFields[95];
        break;

    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_AGENT_ERROR;
    }

    /* call cpss api function - ruleIndex is an absolute index */
    result = pg_wrap_cpssDxChTtiRuleActionUpdate(devNum,ruleIndex,ttiActionDatabase[index].actionType,actionPtr,GT_TRUE,isNewIndex);

    /* if the rule action didn't succeed:  */
    if (result != GT_OK)
    {
        /* if this is a new action remove it from the database */
        if (isNewIndex)
        {
            ttiActionDatabase[index].valid = GT_FALSE;
            ttiActionDatabase[index].nextFree = firstFree;
            firstFree = index;
        }
        /* if this is an update for existing action restore previous data */
        else
        {
            cpssOsMemCpy(&ttiActionDatabase[index],&ttiTempAction,sizeof(ttiTempAction));
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_6Get
*
* DESCRIPTION:
*       This function gets single rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_6Get
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    CPSS_DXCH_TTI_ACTION_UNT    *actionPtr;
    GT_HW_DEV_NUM               tempHwDev;    /* used for port,dev converting */
    GT_PORT_NUM                 tempPort;   /* used for port,dev converting */

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* get next valid TTI action entry */
    while ((ttiActionGetIndex < ACTION_TABLE_SIZE) &&
           (ttiActionDatabase[ttiActionGetIndex].valid == GT_FALSE))
        ttiActionGetIndex++;

    if (ttiActionGetIndex == ACTION_TABLE_SIZE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    actionPtr = &(ttiActionDatabase[ttiActionGetIndex].actionEntry);

    switch (ttiActionDatabase[ttiActionGetIndex].actionType)
    {
    case CPSS_DXCH_TTI_ACTION_TYPE1_ENT:

        inArgs[0] = CPSS_DXCH_TTI_ACTION_TYPE1_ENT;

        inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
        inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
        inFields[2]  = actionPtr->type1.tunnelTerminate;
        inFields[3]  = actionPtr->type1.passengerPacketType;
        inFields[4]  = actionPtr->type1.copyTtlFromTunnelHeader;
        inFields[5]  = actionPtr->type1.command;
        inFields[6]  = actionPtr->type1.redirectCommand;
        inFields[7]  = actionPtr->type1.egressInterface.type;
        tempHwDev      = actionPtr->type1.egressInterface.devPort.hwDevNum;
        tempPort     = actionPtr->type1.egressInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempHwDev,tempPort);
        inFields[8]  = tempHwDev;
        inFields[9]  = tempPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type1.egressInterface.trunkId);
        inFields[10] = actionPtr->type1.egressInterface.trunkId;
        inFields[11] = actionPtr->type1.egressInterface.vidx;
        inFields[12] = actionPtr->type1.egressInterface.vlanId;
        inFields[13] = actionPtr->type1.tunnelStart;
        inFields[14] = actionPtr->type1.tunnelStartPtr;
        inFields[15] = actionPtr->type1.routerLookupPtr;
        inFields[16] = actionPtr->type1.vrfId;
        /* the fields targetIsTrunk, virtualSrcPort and virtualSrcdev were removed
           from CPSS_DXCH_TTI_ACTION_STC so fields 17-19 will be hardcoded  */
        inFields[17] = GT_FALSE;
        inFields[18] = 0;
        inFields[19] = 0;
        inFields[20] = 0;
        inFields[21] = actionPtr->type1.sourceIdSetEnable;
        inFields[22] = actionPtr->type1.sourceId;
        inFields[23] = actionPtr->type1.vlanCmd;
        inFields[24] = actionPtr->type1.vlanId;
        inFields[25] = actionPtr->type1.vlanPrecedence;
        inFields[26] = actionPtr->type1.nestedVlanEnable;
        inFields[27] = actionPtr->type1.bindToPolicer;
        inFields[28] = actionPtr->type1.policerIndex;
        inFields[29] = actionPtr->type1.qosPrecedence;
        inFields[30] = actionPtr->type1.qosTrustMode;
        inFields[31] = actionPtr->type1.qosProfile;
        inFields[32] = actionPtr->type1.modifyUpEnable;
        inFields[33] = actionPtr->type1.modifyDscpEnable;
        inFields[34] = actionPtr->type1.up;
        inFields[35] = actionPtr->type1.remapDSCP;
        inFields[36] = actionPtr->type1.mirrorToIngressAnalyzerEnable;
        inFields[37] = actionPtr->type1.userDefinedCpuCode;
        inFields[38] = actionPtr->type1.vntl2Echo;
        inFields[39] = actionPtr->type1.bridgeBypass;
        inFields[40] = actionPtr->type1.actionStop;
        inFields[41] = actionPtr->type1.activateCounter;
        inFields[42] = actionPtr->type1.counterIndex;

        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],   inFields[2],   inFields[3],
                    inFields[4],  inFields[5],   inFields[6],   inFields[7],
                    inFields[8],  inFields[9],   inFields[10],  inFields[11],
                    inFields[12], inFields[13],  inFields[14],  inFields[15],
                    inFields[16], inFields[17],  inFields[18],  inFields[19],
                    inFields[20], inFields[21],  inFields[22],  inFields[23],
                    inFields[24], inFields[25],  inFields[26],  inFields[27],
                    inFields[28], inFields[29],  inFields[30],  inFields[31],
                    inFields[32], inFields[33],  inFields[34],  inFields[35],
                    inFields[36], inFields[37],  inFields[38],  inFields[39],
                    inFields[40], inFields[41],  inFields[42]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d%f", 0);
        break;

    case CPSS_DXCH_TTI_ACTION_TYPE2_ENT:

        inArgs[0] = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        inFields[0]  = ttiActionDatabase[ttiActionGetIndex].devNum;
        inFields[1]  = ttiActionDatabase[ttiActionGetIndex].ruleIndex;
        inFields[2]  = actionPtr->type2.tunnelTerminate;
        inFields[3]  = actionPtr->type2.ttPassengerPacketType;
        inFields[4]  = actionPtr->type2.tsPassengerPacketType;
        inFields[5]  = actionPtr->type2.ttHeaderLength;
        inFields[6]  = actionPtr->type2.continueToNextTtiLookup;
        inFields[7]  = actionPtr->type2.copyTtlExpFromTunnelHeader;
        inFields[8]  = actionPtr->type2.mplsCommand;
        inFields[9]  = actionPtr->type2.mplsTtl;
        inFields[10]  = actionPtr->type2.enableDecrementTtl;
        inFields[11]  = actionPtr->type2.passengerParsingOfTransitMplsTunnelMode;
        inFields[12]  = actionPtr->type2.passengerParsingOfTransitNonMplsTransitTunnelEnable;
        inFields[13]  = actionPtr->type2.command;
        inFields[14] = actionPtr->type2.redirectCommand;
        inFields[15] = actionPtr->type2.egressInterface.type;
        tempHwDev      = actionPtr->type2.egressInterface.devPort.hwDevNum;
        tempPort     = actionPtr->type2.egressInterface.devPort.portNum;
        CONVERT_BACK_DEV_PORT_DATA_MAC(tempHwDev,tempPort);
        inFields[16] = tempHwDev;
        inFields[17] = tempPort;
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(actionPtr->type2.egressInterface.trunkId);
        inFields[18] = actionPtr->type2.egressInterface.trunkId;
        inFields[19] = actionPtr->type2.egressInterface.vidx;
        inFields[20] = actionPtr->type2.egressInterface.vlanId;
        inFields[21] = actionPtr->type2.arpPtr;
        inFields[22] = actionPtr->type2.tunnelStart;
        inFields[23] = actionPtr->type2.tunnelStartPtr;
        inFields[24] = actionPtr->type2.routerLttPtr;
        inFields[25] = actionPtr->type2.vrfId;
        inFields[26] = actionPtr->type2.sourceIdSetEnable;
        inFields[27] = actionPtr->type2.sourceId;
        inFields[28] = actionPtr->type2.tag0VlanCmd;
        inFields[29] = actionPtr->type2.tag0VlanId;
        inFields[30] = actionPtr->type2.tag1VlanCmd;
        inFields[31] = actionPtr->type2.tag1VlanId;
        inFields[32] = actionPtr->type2.tag0VlanPrecedence;
        inFields[33] = actionPtr->type2.nestedVlanEnable;
        inFields[34] = actionPtr->type2.bindToPolicerMeter;
        inFields[35] = actionPtr->type2.bindToPolicer;
        inFields[36] = actionPtr->type2.policerIndex;
        inFields[37] = actionPtr->type2.qosPrecedence;
        inFields[38] = actionPtr->type2.keepPreviousQoS;
        inFields[39] = actionPtr->type2.trustUp;
        inFields[40] = actionPtr->type2.trustDscp;
        inFields[41] = actionPtr->type2.trustExp;
        inFields[42] = actionPtr->type2.qosProfile;
        inFields[43] = actionPtr->type2.modifyTag0Up;
        inFields[44] = actionPtr->type2.tag1UpCommand;
        inFields[45] = actionPtr->type2.modifyDscp;
        inFields[46] = actionPtr->type2.tag0Up;
        inFields[47] = actionPtr->type2.tag1Up;
        inFields[48] = actionPtr->type2.remapDSCP;
        inFields[49] = actionPtr->type2.qosUseUpAsIndexEnable;
        inFields[50] = actionPtr->type2.qosMappingTableIndex;
        inFields[51] = actionPtr->type2.mplsLLspQoSProfileEnable;
        inFields[52] = actionPtr->type2.pcl0OverrideConfigIndex;
        inFields[53] = actionPtr->type2.pcl0_1OverrideConfigIndex;
        inFields[54] = actionPtr->type2.pcl1OverrideConfigIndex;
        inFields[55] = actionPtr->type2.iPclConfigIndex;
        inFields[56] = actionPtr->type2.iPclUdbConfigTableEnable;
        inFields[57] = actionPtr->type2.iPclUdbConfigTableIndex;
        inFields[58] = actionPtr->type2.mirrorToIngressAnalyzerEnable;
        inFields[59] = actionPtr->type2.mirrorToIngressAnalyzerIndex;
        inFields[60] = actionPtr->type2.userDefinedCpuCode;
        inFields[61] = actionPtr->type2.bindToCentralCounter;
        inFields[62] = actionPtr->type2.centralCounterIndex;
        inFields[63] = actionPtr->type2.vntl2Echo;
        inFields[64] = actionPtr->type2.bridgeBypass;
        inFields[65] = actionPtr->type2.ingressPipeBypass;
        inFields[66] = actionPtr->type2.actionStop;
        inFields[67] = actionPtr->type2.hashMaskIndex;
        inFields[68] = actionPtr->type2.modifyMacSa;
        inFields[69] = actionPtr->type2.modifyMacDa;
        inFields[70] = actionPtr->type2.ResetSrcPortGroupId;
        inFields[71] = actionPtr->type2.multiPortGroupTtiEnable;
        inFields[72] = actionPtr->type2.sourceEPortAssignmentEnable;
        inFields[73] = actionPtr->type2.sourceEPort;
        inFields[74] = actionPtr->type2.flowId;
        inFields[75] = actionPtr->type2.setMacToMe;
        inFields[76] = actionPtr->type2.rxProtectionSwitchEnable;
        inFields[77] = actionPtr->type2.rxIsProtectionPath;
        inFields[78] = actionPtr->type2.pwTagMode;
        inFields[79] = actionPtr->type2.oamTimeStampEnable;
        inFields[80] = actionPtr->type2.oamOffsetIndex;
        inFields[81] = actionPtr->type2.oamProcessEnable;
        inFields[82] = actionPtr->type2.oamProfile;
        inFields[83] = actionPtr->type2.oamChannelTypeToOpcodeMappingEnable;
        inFields[84] = actionPtr->type2.isPtpPacket;
        inFields[85] = actionPtr->type2.ptpTriggerType;
        inFields[86] = actionPtr->type2.ptpOffset;
        inFields[87] = actionPtr->type2.cwBasedPw;
        inFields[88] = actionPtr->type2.ttlExpiryVccvEnable;
        inFields[89] = actionPtr->type2.pwe3FlowLabelExist;
        inFields[90] = actionPtr->type2.pwCwBasedETreeEnable;
        inFields[91] = actionPtr->type2.applyNonDataCwCommand;

        inFields[92] = actionPtr->type2.unknownSaCommandEnable;
        inFields[93] = actionPtr->type2.unknownSaCommand;
        inFields[94] = actionPtr->type2.sourceMeshIdSetEnable;
        inFields[95] = actionPtr->type2.sourceMeshId;

        fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],   inFields[2],   inFields[3],
                    inFields[4],  inFields[5],   inFields[6],   inFields[7],
                    inFields[8],  inFields[9],   inFields[10],  inFields[11],
                    inFields[12], inFields[13],  inFields[14],  inFields[15],
                    inFields[16], inFields[17],  inFields[18],  inFields[19],
                    inFields[20], inFields[21],  inFields[22],  inFields[23],
                    inFields[24], inFields[25],  inFields[26],  inFields[27],
                    inFields[28], inFields[29],  inFields[30],  inFields[31],
                    inFields[32], inFields[33],  inFields[34],  inFields[35],
                    inFields[36], inFields[37],  inFields[38],  inFields[39],
                    inFields[40], inFields[41],  inFields[42],  inFields[43],
                    inFields[44], inFields[45],  inFields[46],  inFields[47],
                    inFields[48], inFields[49],  inFields[50],  inFields[51],
                    inFields[52], inFields[53],  inFields[54],  inFields[55],
                    inFields[56], inFields[57],  inFields[58],  inFields[59],
                    inFields[60], inFields[61],  inFields[62],  inFields[63],
                    inFields[64], inFields[65],  inFields[66],  inFields[67],
                    inFields[68], inFields[69],  inFields[70],  inFields[71],
                    inFields[72], inFields[73],  inFields[74],  inFields[75],
                    inFields[76], inFields[77],  inFields[78],  inFields[79],
                    inFields[80], inFields[81],  inFields[82],  inFields[83],
                    inFields[84], inFields[85],  inFields[86],  inFields[87],
                    inFields[88], inFields[89],  inFields[90],  inFields[91],
                    inFields[92], inFields[93],  inFields[94],  inFields[95]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d%f", 1);
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI ACTION FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    ttiActionGetIndex++;

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiRuleAction_6GetFirst
*
* DESCRIPTION:
*       This function gets first rule action.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiRuleAction_6GetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    ttiActionGetIndex = 0;

    return wrCpssDxChTtiRuleAction_6Get(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet
*
* DESCRIPTION:
*       IPv4 Tunnel Termination Header Error exception is detected if ANY of the following criteria are NOT met:
*           - IPv4 header <Checksum> is correct
*           - IPv4 header <Version> = 4
*           - IPv4 header <IHL> (IP Header Length) >= 5 (32-bit words)
*           - IPv4 header <IHL> (IP Header Length) <= IPv4 header <Total Length> / 4
*           - IPv4 header <Total Length> + packet L3 Offset + 4 (CRC length) <= MAC layer packet byte count
*           - IPv4 header <SIP> != IPv4 header <DIP>
*
*       This function globally enables/disables bypassing IPv4 header length criteria checks as part of
*       IPv4 header exception checking.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE:  enable bypass header length check
*                       GT_FALSE: disable bypass header length check
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
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet
*
* DESCRIPTION:
*       IPv4 Tunnel Termination Header Error exception is detected if ANY of the following criteria are NOT met:
*           - IPv4 header <Checksum> is correct
*           - IPv4 header <Version> = 4
*           - IPv4 header <IHL> (IP Header Length) >= 5 (32-bit words)
*           - IPv4 header <IHL> (IP Header Length) <= IPv4 header <Total Length> / 4
*           - IPv4 header <Total Length> + packet L3 Offset + 4 (CRC length) <= MAC layer packet byte count
*           - IPv4 header <SIP> != IPv4 header <DIP>
*
*       This function gets the globally bypassing IPv4 header length criteria check as part of
*       IPv4 header exception checking.
*
* APPLICABLE DEVICES:
*        xCat; xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion; xCat2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable bypass header length check
*                       GT_TRUE:  enable bypass header length check
*                       GT_FALSE: disable bypass header length check
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPwCwSequencingSupportEnableSet
*
* DESCRIPTION:
*       This function enables/disables the check for Pseudo Wire-Control Word
*       Data Word format <Sequence Number>==0 in terminated Pseudo Wires.
*       < Sequence Number > are the 16 LSB of “CW as Data Word format".
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       enable        - GT_TRUE: PW-CW sequencing supported
*                       GT_FALSE: PW-CW sequencing is not supported
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPwCwSequencingSupportEnableSet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiPwCwSequencingSupportEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPwCwSequencingSupportEnableGet
*
* DESCRIPTION:
*       This function gets the current state (enable/disable) of the check
*       for Pseudo Wire-Control Word Data Word format <Sequence Number>==0
*       in terminated Pseudo Wires.
*       < Sequence Number > are the 16 LSB of “CW as Data Word format”.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*
* OUTPUTS:
*       enablePtr     - points to enable/disable PW-CW sequencing support
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong device id
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPwCwSequencingSupportEnableGet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTtiPwCwSequencingSupportEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiUserDefinedByteSet
*
* DESCRIPTION:
*   The function configures the User Defined Byte (UDB)
*
* INPUTS:
*       devNum       - device number
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*       offsetType   - the type of offset
*       offset       - The offset of the user-defined byte, in bytes,from the
*                      place in the packet indicated by the offset type. .(APPLICABLE RANGES: 0..127)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_OUT_OF_RANGE          - parameter not in valid range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiUserDefinedByteSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;
    GT_U8                                devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType;
    GT_U32                               udbIndex;
    CPSS_DXCH_TTI_OFFSET_TYPE_ENT        offsetType;
    GT_U8                                offset;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];
    udbIndex = (GT_U32)inArgs[2];
    offsetType = (CPSS_DXCH_TTI_OFFSET_TYPE_ENT)inArgs[3];
    offset = (GT_U8)inArgs[4];

    /* call cpss api function */
    result = cpssDxChTtiUserDefinedByteSet(devNum, keyType, udbIndex, offsetType, offset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiUserDefinedByteGet
*
* DESCRIPTION:
*   The function gets the User Defined Byte (UDB) configuration
*
* INPUTS:
*       devNum       - device number
*       keyType      - key Type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       udbIndex     - index of User Defined Byte to configure.(APPLICABLE RANGES: 0..29)
*
* OUTPUTS:
*       offsetTypePtr   - (pointer to) The type of offset
*       offsetPtr       - (pointer to) The offset of the user-defined byte,
*                         in bytes,from the place in the packet
*                         indicated by the offset type.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - otherwise*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiUserDefinedByteGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType;
    GT_U32                              udbIndex;
    CPSS_DXCH_TTI_OFFSET_TYPE_ENT       offsetType;
    GT_U8                               offset;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];
    udbIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiUserDefinedByteGet(devNum, keyType, udbIndex, &offsetType, &offset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", offsetType, offset);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPacketTypeKeySizeSet
*
* DESCRIPTION:
*       function sets key type size.
*
* INPUTS:
*       devNum        - device number
*       keyType       - key type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*       size          - key size in TCAM
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
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPacketTypeKeySizeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;
    GT_U8                                devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT           keyType;
    CPSS_DXCH_TTI_KEY_SIZE_ENT           size;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];
    size = (CPSS_DXCH_TTI_KEY_SIZE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiPacketTypeKeySizeSet(devNum, keyType, size);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPacketTypeKeySizeGet
*
* DESCRIPTION:
*       function gets key type size.
*
* INPUTS:
*       devNum        - device number
*       keyType       - key type; valid values:
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_MPLS_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_FRAGMENT_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV4_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_ETHERNET_OTHER_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_TCP_E
*                           CPSS_DXCH_TTI_KEY_UDB_IPV6_UDP_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE0_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE1_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE2_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE3_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE4_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE5_E
*                           CPSS_DXCH_TTI_KEY_UDB_UDE6_E
*
* OUTPUTS:
*       sizePtr      - points to key size in TCAM
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPacketTypeKeySizeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                           result;
    GT_U8                               devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT          keyType;
    CPSS_DXCH_TTI_KEY_SIZE_ENT          size;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiPacketTypeKeySizeGet(devNum, keyType, &size);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", size);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiExceptionCpuCodeSet
*
* DESCRIPTION:
*       Set tunnel termination exception CPU code.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to set CPU code for.
*                       valid values:
*                       CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*       code          - the code for the exception type
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiExceptionCpuCodeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;
    GT_U8                                devNum;
    CPSS_DXCH_TTI_EXCEPTION_ENT          exceptionType;
    CPSS_NET_RX_CPU_CODE_ENT             code;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_DXCH_TTI_EXCEPTION_ENT)inArgs[1];
    code = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiExceptionCpuCodeSet(devNum, exceptionType, code);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiExceptionCpuCodeGet
*
* DESCRIPTION:
*       Get tunnel termination exception CPU code.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType - tunnel termination exception type to get CPU code for.
*                       valid values:
*                       CPSS_DXCH_TTI_EXCEPTION_IPV4_SIP_ADDRESS_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_UNSUP_GRE_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HEADER_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_NON_HBH_ERROR_E
*                       CPSS_DXCH_TTI_EXCEPTION_IPV6_SIP_ADDRESS_ERROR_E
*
* OUTPUTS:
*       codePtr    - (points to) the code for the exception type
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiExceptionCpuCodeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;
    GT_U8                                devNum;
    CPSS_DXCH_TTI_EXCEPTION_ENT          exceptionType;
    CPSS_NET_RX_CPU_CODE_ENT             code;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_DXCH_TTI_EXCEPTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiExceptionCpuCodeGet(devNum, exceptionType, &code);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", code);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiTcamSegmentModeSet
*
* DESCRIPTION:
*       Sets a TTI TCAM segment mode for a specific key type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - key type
*       segmentMode   - TTI TCAM segment mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiParallelLookupEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT              keyType;
    GT_BOOL                                 enable;
    CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT segmentMode;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];
    if (PRV_CPSS_SIP_5_10_CHECK_MAC(devNum))
    {
        segmentMode = (enable == GT_FALSE ?
                       CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ONE_TCAM_E :
                       CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_FOUR_TCAM_E);
    }
    else
    {
        segmentMode = (enable == GT_FALSE ?
                       CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ONE_TCAM_E :
                       CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_TWO_TCAM_E);
    }

    /* call cpss api function */
    result = cpssDxChTtiTcamSegmentModeSet(devNum, keyType, segmentMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiTcamSegmentModeGet
*
* DESCRIPTION:
*       Gets a TTI TCAM segment mode for a specific key type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       keyType         - key type
*
* OUTPUTS:
*       segmentModePtr  - (pointer to) TTI TCAM segment mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on invalid HW value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiParallelLookupEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT              keyType;
    GT_BOOL                                 enable;
    CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT segmentMode;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiTcamSegmentModeGet(devNum, keyType, &segmentMode);
    enable = (segmentMode == CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ONE_TCAM_E ? GT_FALSE : GT_TRUE);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiTcamSegmentModeSet
*
* DESCRIPTION:
*       Sets a TTI TCAM segment mode for a specific key type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       keyType       - key type
*       segmentMode   - TTI TCAM segment mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiTcamSegmentModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT              keyType;
    CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT segmentMode;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];
    segmentMode = (CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiTcamSegmentModeSet(devNum, keyType, segmentMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiTcamSegmentModeGet
*
* DESCRIPTION:
*       Gets a TTI TCAM segment mode for a specific key type
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       keyType         - key type
*
* OUTPUTS:
*       segmentModePtr  - (pointer to) TTI TCAM segment mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong value in any of the parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on invalid HW value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiTcamSegmentModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    GT_U8                                   devNum;
    CPSS_DXCH_TTI_KEY_TYPE_ENT              keyType;
    CPSS_DXCH_TTI_KEY_TCAM_SEGMENT_MODE_ENT segmentMode;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    keyType = (CPSS_DXCH_TTI_KEY_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiTcamSegmentModeGet(devNum, keyType, &segmentMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", segmentMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMcTunnelDuplicationModeSet
*
* DESCRIPTION:
*       Set the tunnel duplication mode for a specific protocol on a specific
*       port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*       protocol      - the protocol
*       mode          - the tunnel duplication mode
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
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMcTunnelDuplicationModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                        result;
    GT_U8                                            devNum;
    GT_PORT_NUM                                      portNum;
    CPSS_TUNNEL_MULTICAST_TYPE_ENT                   protocol;
    CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT     mode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    protocol = (CPSS_TUNNEL_MULTICAST_TYPE_ENT)inArgs[2];
    mode = (CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTtiMcTunnelDuplicationModeSet(devNum, portNum, protocol, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMcTunnelDuplicationModeGet
*
* DESCRIPTION:
*       Get the tunnel duplication mode for a specific protocol on a specific
*       port
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum        - device number
*       portNum       - port number
*       protocol      - the protocol
*
* OUTPUTS:
*       modePtr       - (pointer to) the tunnel duplication mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMcTunnelDuplicationModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                        result;
    GT_U8                                            devNum;
    GT_PORT_NUM                                      portNum;
    CPSS_TUNNEL_MULTICAST_TYPE_ENT                   protocol;
    CPSS_DXCH_TTI_MULTICAST_DUPLICATION_MODE_ENT     mode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    protocol = (CPSS_TUNNEL_MULTICAST_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChTtiMcTunnelDuplicationModeGet(devNum, portNum, protocol, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMcTunnelDuplicationUdpDestPortSet
*
* DESCRIPTION:
*       Set the UDP destination port used to trigger IPv4/IPv6 Multicast
*       replication
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       udpPort   - the UDP destination port
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
*       This configuration is relevant only when default ePort
*       <IPv4 MC Duplication IP Protocol Mode> = UDP or Default ePort
*       <IPv6 MC Duplication IP Protocol Mode> = UDP
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMcTunnelDuplicationUdpDestPortSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U16    udpPort;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    udpPort = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiMcTunnelDuplicationUdpDestPortSet(devNum, udpPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMcTunnelDuplicationUdpDestPortGet
*
* DESCRIPTION:
*       Get the UDP destination port used to trigger IPv4/IPv6 Multicast
*       replication
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       devNum     - device number
*       udpPortPtr - (pointer to) the UDP destination port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       This configuration is relevant only when default ePort
*       <IPv4 MC Duplication IP Protocol Mode> = UDP or Default ePort
*       <IPv6 MC Duplication IP Protocol Mode> = UDP
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMcTunnelDuplicationUdpDestPortGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U16    udpPort;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTtiMcTunnelDuplicationUdpDestPortGet(devNum, &udpPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", udpPort);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMplsMcTunnelTriggeringMacDaSet
*
* DESCRIPTION:
*       Set the MPLS multicast MAC DA and mask used for duplication triggering
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       addressPtr  - (pointer to) the MAC DA
*       maskPtr     - (pointer to) the address mask
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
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMplsMcTunnelTriggeringMacDaSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8            devNum;
    GT_ETHERADDR     address;
    GT_ETHERADDR     mask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    galtisMacAddr(&address, (GT_U8*)inArgs[1]);
    galtisMacAddr(&mask, (GT_U8*)inArgs[2]);

    /* call cpss api function */
    result = cpssDxChTtiMplsMcTunnelTriggeringMacDaSet(devNum, &address, &mask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiMplsMcTunnelTriggeringMacDaGet
*
* DESCRIPTION:
*       Get the MPLS multicast MAC DA and mask used for duplication triggering
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       devNum     - device number
*       addressPtr - (pointer to) the MAC DA
*       maskPtr    - (pointer to) the address mask
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMplsMcTunnelTriggeringMacDaGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8            devNum;
    GT_ETHERADDR     address;
    GT_ETHERADDR     mask;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTtiMplsMcTunnelTriggeringMacDaGet(devNum, &address, &mask);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%6b%6b", address.arEther, mask.arEther);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPwCwExceptionCmdSet
*
* DESCRIPTION:
*       Set a PW CW exception command
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       exceptionType   - exception type
*       command         - the packet command
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
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPwCwExceptionCmdSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT   exceptionType;
    CPSS_PACKET_CMD_ENT                 command;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT)inArgs[1];
    command = (CPSS_PACKET_CMD_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiPwCwExceptionCmdSet(devNum, exceptionType, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPwCwExceptionCmdGet
*
* DESCRIPTION:
*       Get a PW CW exception command
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       exceptionType   - exception type
*
* OUTPUTS:
*       commandPtr - (pointer to) the packet command
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPwCwExceptionCmdGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT   exceptionType;
    CPSS_PACKET_CMD_ENT                 command;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_DXCH_TTI_PW_CW_EXCEPTION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiPwCwExceptionCmdGet(devNum, exceptionType, &command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", command);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPwCwCpuCodeBaseSet
*
* DESCRIPTION:
*       Set the base CPU code value for PWE3
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       cpuCodeBase - the base CPU code value for PWE3
*                     (APPLICABLE RANGES: 192..255)
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
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPwCwCpuCodeBaseSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    cpuCodeBase;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cpuCodeBase = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiPwCwCpuCodeBaseSet(devNum, cpuCodeBase);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPwCwCpuCodeBaseGet
*
* DESCRIPTION:
*       Get the base CPU code value for PWE3
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*
* OUTPUTS:
*       cpuCodeBasePtr - (pointer to) the base CPU code value for PWE3
*                        (APPLICABLE RANGES: 192..255)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPwCwCpuCodeBaseGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    cpuCodeBase;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTtiPwCwCpuCodeBaseGet(devNum, &cpuCodeBase);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cpuCodeBase);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiIPv6ExtensionHeaderSet
*
* DESCRIPTION:
*       Set one of the 2 configurable IPv6 extension headers.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum               - device number
*       extensionHeaderId    - the configurable extension header index
*                              (APPLICABLE RANGES: 0..1)
*       extensionHeaderValue - the configurable extension header value
*                              (APPLICABLE RANGES: 0..255)
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
*       The design assumes that the information following the IPv6 header is L4
*       unless there is a known next hop header. The design supports several
*       hard-coded next hop header values and two configurable ones.
*       If any of the following next hop values are identified, the <Is L4 Valid>
*       indication in the IPCL key is set to NOT VALID:
*       - HBH (0)
*       - IPv6 Routing header (43)
*       - IPv6 Fragment header (44)
*       - Encapsulation Security Payload (50)
*       - Authentication Header (51)
*       - IPv6 Destination Options (60)
*       - Mobility Header (135)
*       - <IPv6 Extension Value0>
*       - <IPv6 Extension Value1>
*       To disable a configurable header, set its value to one of the above
*       values.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiIPv6ExtensionHeaderSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    extensionHeaderId;
    GT_U32    extensionHeaderValue;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    extensionHeaderId = (GT_U32)inArgs[1];
    extensionHeaderValue = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiIPv6ExtensionHeaderSet(devNum, extensionHeaderId, extensionHeaderValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiIPv6ExtensionHeaderGet
*
* DESCRIPTION:
*       Get one of the 2 configurable IPv6 extension headers.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum                   - device number
*       extensionHeaderId        - the configurable extension header index
*                                  (APPLICABLE RANGES: 0..1)
*
* OUTPUTS:
*       extensionHeaderValuePtr  - the configurable extension header value
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The design assumes that the information following the IPv6 header is L4
*       unless there is a known next hop header. The design supports several
*       hard-coded next hop header values and two configurable ones.
*       If any of the following next hop values are identified, the <Is L4 Valid>
*       indication in the IPCL key is set to NOT VALID:
*       - HBH (0)
*       - IPv6 Routing header (43)
*       - IPv6 Fragment header (44)
*       - Encapsulation Security Payload (50)
*       - Authentication Header (51)
*       - IPv6 Destination Options (60)
*       - Mobility Header (135)
*       - <IPv6 Extension Value0>
*       - <IPv6 Extension Value1>
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiIPv6ExtensionHeaderGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    extensionHeaderId;
    GT_U32    extensionHeaderValue;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    extensionHeaderId = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiIPv6ExtensionHeaderGet(devNum, extensionHeaderId, &extensionHeaderValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", extensionHeaderValue);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiSourceIdBitsOverrideSet
*
* DESCRIPTION:
*       Set the SrcID bits that are overridden by the TTI action of SrcID
*       assignment.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       lookup          - the lookup number (APPLICABLE RANGES: 0..3)
*       overrideBitmap  - 12 bits bitmap. For each bit:
*                         0 - do not override
*                         1 - override
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
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiSourceIdBitsOverrideSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    lookup;
    GT_U32    overrideBitmap;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    lookup = (GT_U32)inArgs[1];
    overrideBitmap = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiSourceIdBitsOverrideSet(devNum, lookup, overrideBitmap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiSourceIdBitsOverrideGet
*
* DESCRIPTION:
*       Get the SrcID bits that are overridden by the TTI action of SrcID
*       assignment.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum              - device number
*       lookup              - the lookup number (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       overrideBitmapPtr   - 12 bits bitmap. For each bit:
*                             0 - do not override
*                             1 - override
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong value in any of the parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiSourceIdBitsOverrideGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    lookup;
    GT_U32    overrideBitmap;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    lookup = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiSourceIdBitsOverrideGet(devNum, lookup, &overrideBitmap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", overrideBitmap);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiGreExtensionsEnableSet
*
* DESCRIPTION:
*       Enable/Disable parsing of extensions (Checksum, Sequence, Key) on
*       IPv4/IPv6 GRE tunnels.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*       enable      - enable/disable GRE extensions
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiGreExtensionsEnableSet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiGreExtensionsEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiGreExtensionsEnableGet
*
* DESCRIPTION:
*       Gt the enabling status of the parsing of extensions (Checksum, Sequence,
*       Key) on IPv4/IPv6 GRE tunnels.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GRE extensions enabling status
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiGreExtensionsEnableGet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTtiGreExtensionsEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiTcamBaseIndexGet
*
* DESCRIPTION:
*       Gets TCAM base index for TTI rules
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       hitNum   - hit index, applicable range depends on device
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       The converted rule index.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiTcamBaseIndexGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8     devNum;
    GT_U32    hitNum;
    GT_U32    ruleBaseIndexOffset;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    hitNum = (GT_U32)inArgs[1];

    /* call appDemo api function */
    ruleBaseIndexOffset = appDemoDxChLion3TcamTtiBaseIndexGet(devNum, hitNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d", ruleBaseIndexOffset);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiTcamNumOfIndexsGet
*
* DESCRIPTION:
*       Gets TCAM number of indexes for TTI rules
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum   - device number
*       hitNum   - hit index, applicable range depends on device
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       TCAM number of indexes for TTI rules.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiTcamNumOfIndexsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8     devNum;
    GT_U32    hitNum;
    GT_U32    numOfIndexs;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    hitNum = (GT_U32)inArgs[1];

    /* call appDemo api function */
    numOfIndexs = appDemoDxChLion3TcamTtiNumOfIndexsGet(devNum, hitNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d", numOfIndexs);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortIpTotalLengthDeductionEnableSet
*
* DESCRIPTION:
*       For MACSEC packets over IPv4/6 tunnel, that are to be tunnel terminated,
*       this configuration enables aligning the IPv4/6 total header length to the
*       correct offset.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*                 In eArch devices portNum is default ePort.
*       enable  - Enable/disable Ipv4/6 Total Length Deduction.
*                 GT_TRUE: When enabled, and ePort default attribute
*                         <IPv4/6 Total Length Deduction Enable> == Enabled:
*                         - for IPv4:
*                           <IPv4 Total Length> = Packet IPv4 header <Total Length> -
*                           Global configuration < IPv4 Total Length Deduction Value>
*                         - for IPv6:
*                           <IPv6 Total Length> = Packet IPv6 header <Total Length> -
*                           Global configuration < IPv6 Total Length Deduction Value>
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPortIpTotalLengthDeductionEnableSet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortIpTotalLengthDeductionEnableSet(devNum, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiPortIpTotalLengthDeductionEnableGet
*
* DESCRIPTION:
*       Get if IPv4/6 total header length is aligned to the correct offset
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum    - device number
*       portNum   - port number
*                   In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       enablePtr -  Enable/disable Ipv4/6 Total Length Deduction.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiPortIpTotalLengthDeductionEnableGet
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
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, port);

    /* call cpss api function */
    result = cpssDxChTtiPortIpTotalLengthDeductionEnableGet(devNum, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiIpTotalLengthDeductionValueSet
*
* DESCRIPTION:
*       Set Global configuration IPv4 or IPv6 Total Length Deduction Value.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       ipType - IPv4 or Ipv6; valid values:
*                   CPSS_IP_PROTOCOL_IPV4_E
*                   CPSS_IP_PROTOCOL_IPV6_E
*       value  - IPv4 or IPv6 Total Length Deduction Value .(APPLICABLE RANGES: 0..63)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiIpTotalLengthDeductionValueSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IP_PROTOCOL_STACK_ENT ipType;
    GT_U32 value;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ipType = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    value = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTtiIpTotalLengthDeductionValueSet(devNum, ipType, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTtiIpTotalLengthDeductionValueGet
*
* DESCRIPTION:
*       Get Global configuration IPv4 or IPv6 Total Length Deduction Value.
*
* APPLICABLE DEVICES:
*       Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*       DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum - device number
*       ipType - IPv4 or Ipv6; valid values:
*                   CPSS_IP_PROTOCOL_IPV4_E
*                   CPSS_IP_PROTOCOL_IPV6_E
*
* OUTPUTS:
*       valuePtr -  (pointer to) IPv4 or IPv6 Total Length Deduction Value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong input parameters
*       GT_HW_ERROR              - failed to write to hardware
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant for Ethernet-over-IPv4/6-GRE packets
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiIpTotalLengthDeductionValueGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IP_PROTOCOL_STACK_ENT ipType;
    GT_U32 value;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    ipType = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTtiIpTotalLengthDeductionValueGet(devNum, ipType, &value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", value);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiMultiRuleActionSet
*
* DESCRIPTION:
*   The function configures the action to be used in multi rule set wrapper
*   wrCpssDxChTtiMultiEthRuleSet and wrCpssDxChTtiMultiUdbRuleSet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMultiRuleActionSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result = GT_OK;
    CPSS_DXCH_TTI_RULE_TYPE_ENT keyType;
    ttiMultiActionDB    *ttiMultiActionDatabaseParams;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    keyType = (CPSS_DXCH_TTI_RULE_TYPE_ENT)inArgs[0];

    switch (keyType) /* rule type */
    {
    case CPSS_DXCH_TTI_RULE_IPV4_E:
        /* not supported yet */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;

    case CPSS_DXCH_TTI_RULE_MPLS_E:
        /* not supported yet */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;

    case CPSS_DXCH_TTI_RULE_ETH_E:
        ttiMultiActionDatabaseParams = &ttiMultiActionDatabaseParamsForEthRule;
        break;

    case CPSS_DXCH_TTI_RULE_MIM_E:
        /* not supported yet */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;

    case CPSS_DXCH_TTI_RULE_UDB_10_E:
        ttiMultiActionDatabaseParams = &ttiMultiActionDatabaseParamsForUdbRuleArray[0];
        break;

    case CPSS_DXCH_TTI_RULE_UDB_20_E:
        ttiMultiActionDatabaseParams = &ttiMultiActionDatabaseParamsForUdbRuleArray[1];
        break;

    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        ttiMultiActionDatabaseParams = &ttiMultiActionDatabaseParamsForUdbRuleArray[2];
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

   /* set action entry in TTI action database as valid */
    ttiMultiActionDatabaseParams->actionEntry.tunnelTerminate                 = (GT_BOOL)inArgs[1];
    ttiMultiActionDatabaseParams->actionEntry.ttPassengerPacketType           = (CPSS_DXCH_TTI_PASSENGER_TYPE_ENT)inArgs[2];
    ttiMultiActionDatabaseParams->actionEntry.tsPassengerPacketType           = (CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT)inArgs[3];

    ttiMultiActionDatabaseParams->actionEntry.ttHeaderLength                  = (GT_U32)inArgs[4];
    ttiMultiActionDatabaseParams->actionEntry.continueToNextTtiLookup         = (GT_BOOL)inArgs[5];

    ttiMultiActionDatabaseParams->actionEntry.copyTtlExpFromTunnelHeader      = (GT_BOOL)inArgs[6];
    ttiMultiActionDatabaseParams->actionEntry.mplsCommand                     = (CPSS_DXCH_TTI_MPLS_CMD_ENT)inArgs[7];
    ttiMultiActionDatabaseParams->actionEntry.mplsTtl                         = (GT_U32)inArgs[8];
    ttiMultiActionDatabaseParams->actionEntry.enableDecrementTtl              = (GT_BOOL)inArgs[9];

    ttiMultiActionDatabaseParams->actionEntry.passengerParsingOfTransitMplsTunnelMode = (CPSS_DXCH_TTI_PASSENGER_PARSING_OF_TRANSIT_MPLS_TUNNEL_MODE_TYPE_ENT)inArgs[10];
    ttiMultiActionDatabaseParams->actionEntry.passengerParsingOfTransitNonMplsTransitTunnelEnable = (GT_BOOL)inArgs[11];

    ttiMultiActionDatabaseParams->actionEntry.command                         = (CPSS_PACKET_CMD_ENT)inArgs[12];
    ttiMultiActionDatabaseParams->actionEntry.redirectCommand                 = (CPSS_DXCH_TTI_REDIRECT_COMMAND_ENT)inArgs[13];
    ttiMultiActionDatabaseParams->actionEntry.egressInterface.type            = (CPSS_INTERFACE_TYPE_ENT)inArgs[14];
    ttiMultiActionDatabaseParams->actionEntry.egressInterface.devPort.hwDevNum  = (GT_HW_DEV_NUM)inArgs[15];
    ttiMultiActionDatabaseParams->actionEntry.egressInterface.devPort.portNum = (GT_PORT_NUM)inArgs[16];
    ttiMultiActionDatabaseParams->portNum_0  = (GT_PORT_NUM)inArgs[16];
    ttiMultiActionDatabaseParams->portNum_1  = (GT_PORT_NUM)inArgs[17];
    ttiMultiActionDatabaseParams->portNum_2  = (GT_PORT_NUM)inArgs[18];
    ttiMultiActionDatabaseParams->portNum_3  = (GT_PORT_NUM)inArgs[19];

    ttiMultiActionDatabaseParams->actionEntry.egressInterface.trunkId         = (GT_TRUNK_ID)inArgs[20];

    ttiMultiActionDatabaseParams->actionEntry.egressInterface.vidx            = (GT_U16)inArgs[21];
    ttiMultiActionDatabaseParams->actionEntry.egressInterface.vlanId          = (GT_U16)inArgs[22];
    ttiMultiActionDatabaseParams->actionEntry.arpPtr                          = (GT_U32)inArgs[23];
    ttiMultiActionDatabaseParams->actionEntry.tunnelStart                     = (GT_BOOL)inArgs[24];
    ttiMultiActionDatabaseParams->actionEntry.tunnelStartPtr                  = (GT_U32)inArgs[25];
    ttiMultiActionDatabaseParams->actionEntry.routerLttPtr                    = (GT_U32)inArgs[26];
    ttiMultiActionDatabaseParams->actionEntry.vrfId                           = (GT_U32)inArgs[27];
    ttiMultiActionDatabaseParams->actionEntry.sourceIdSetEnable               = (GT_BOOL)inArgs[28];
    ttiMultiActionDatabaseParams->actionEntry.sourceId                        = (GT_U32)inArgs[29];
    ttiMultiActionDatabaseParams->actionEntry.tag0VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inArgs[30];
    ttiMultiActionDatabaseParams->actionEntry.tag0VlanId                      = (GT_U16)inArgs[31];
    ttiMultiActionDatabaseParams->actionEntry.tag1VlanCmd                     = (CPSS_DXCH_TTI_VLAN_COMMAND_ENT)inArgs[32];
    ttiMultiActionDatabaseParams->actionEntry.tag1VlanId                      = (GT_U16)inArgs[33];
    ttiMultiActionDatabaseParams->actionEntry.tag0VlanPrecedence              = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inArgs[34];
    ttiMultiActionDatabaseParams->actionEntry.nestedVlanEnable                = (GT_BOOL)inArgs[35];
    ttiMultiActionDatabaseParams->actionEntry.bindToPolicerMeter              = (GT_BOOL)inArgs[36];
    ttiMultiActionDatabaseParams->actionEntry.bindToPolicer                   = (GT_BOOL)inArgs[37];
    ttiMultiActionDatabaseParams->actionEntry.policerIndex                    = (GT_U32)inArgs[38];
    ttiMultiActionDatabaseParams->actionEntry.qosPrecedence                   = (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inArgs[39];
    ttiMultiActionDatabaseParams->actionEntry.keepPreviousQoS                 = (GT_BOOL)inArgs[40];
    ttiMultiActionDatabaseParams->actionEntry.trustUp                         = (GT_BOOL)inArgs[41];
    ttiMultiActionDatabaseParams->actionEntry.trustDscp                       = (GT_BOOL)inArgs[42];
    ttiMultiActionDatabaseParams->actionEntry.trustExp                        = (GT_BOOL)inArgs[43];
    ttiMultiActionDatabaseParams->actionEntry.qosProfile                      = (GT_U32)inArgs[44];
    ttiMultiActionDatabaseParams->actionEntry.modifyTag0Up                    = (CPSS_DXCH_TTI_MODIFY_UP_ENT)inArgs[45];
    ttiMultiActionDatabaseParams->actionEntry.tag1UpCommand                   = (CPSS_DXCH_TTI_TAG1_UP_COMMAND_ENT)inArgs[46];
    ttiMultiActionDatabaseParams->actionEntry.modifyDscp                      = (CPSS_DXCH_TTI_MODIFY_DSCP_ENT)inArgs[47];
    ttiMultiActionDatabaseParams->actionEntry.tag0Up                          = (GT_U32)inArgs[48];
    ttiMultiActionDatabaseParams->actionEntry.tag1Up                          = (GT_U32)inArgs[49];
    ttiMultiActionDatabaseParams->actionEntry.remapDSCP                       = (GT_BOOL)inArgs[50];

    ttiMultiActionDatabaseParams->actionEntry.qosUseUpAsIndexEnable           = (GT_BOOL)inArgs[51];
    ttiMultiActionDatabaseParams->actionEntry.qosMappingTableIndex            = (GT_U32)inArgs[52];
    ttiMultiActionDatabaseParams->actionEntry.mplsLLspQoSProfileEnable        = (GT_BOOL)inArgs[53];

    ttiMultiActionDatabaseParams->actionEntry.pcl0OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inArgs[54];
    ttiMultiActionDatabaseParams->actionEntry.pcl0_1OverrideConfigIndex       = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inArgs[55];
    ttiMultiActionDatabaseParams->actionEntry.pcl1OverrideConfigIndex         = (CPSS_DXCH_PCL_LOOKUP_CONFIG_INDEX_ENT)inArgs[56];
    ttiMultiActionDatabaseParams->actionEntry.iPclConfigIndex                 = (GT_U32)inArgs[57];

    ttiMultiActionDatabaseParams->actionEntry.iPclUdbConfigTableEnable        = (GT_BOOL)inArgs[58];
    ttiMultiActionDatabaseParams->actionEntry.iPclUdbConfigTableIndex         = (CPSS_DXCH_PCL_PACKET_TYPE_ENT)inArgs[59];

    ttiMultiActionDatabaseParams->actionEntry.mirrorToIngressAnalyzerEnable   = (GT_BOOL)inArgs[60];
    ttiMultiActionDatabaseParams->actionEntry.mirrorToIngressAnalyzerIndex    = (GT_U32)inArgs[61];

    ttiMultiActionDatabaseParams->actionEntry.userDefinedCpuCode              = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[62];
    ttiMultiActionDatabaseParams->actionEntry.bindToCentralCounter            = (GT_BOOL)inArgs[63];
    ttiMultiActionDatabaseParams->actionEntry.centralCounterIndex             = (GT_U32)inArgs[64];
    ttiMultiActionDatabaseParams->actionEntry.vntl2Echo                       = (GT_BOOL)inArgs[65];
    ttiMultiActionDatabaseParams->actionEntry.bridgeBypass                    = (GT_BOOL)inArgs[66];
    ttiMultiActionDatabaseParams->actionEntry.ingressPipeBypass               = (GT_BOOL)inArgs[67];
    ttiMultiActionDatabaseParams->actionEntry.actionStop                      = (GT_BOOL)inArgs[68];
    ttiMultiActionDatabaseParams->actionEntry.hashMaskIndex                   = (GT_U32)inArgs[69];
    ttiMultiActionDatabaseParams->actionEntry.modifyMacSa                     = (GT_BOOL)inArgs[70];
    ttiMultiActionDatabaseParams->actionEntry.modifyMacDa                     = (GT_BOOL)inArgs[71];
    ttiMultiActionDatabaseParams->actionEntry.ResetSrcPortGroupId             = (GT_BOOL)inArgs[72];
    ttiMultiActionDatabaseParams->actionEntry.multiPortGroupTtiEnable         = (GT_BOOL)inArgs[73];

    ttiMultiActionDatabaseParams->actionEntry.sourceEPortAssignmentEnable     = (GT_BOOL)inArgs[74];
    ttiMultiActionDatabaseParams->actionEntry.sourceEPort                     = (GT_PORT_NUM)inArgs[75];
    ttiMultiActionDatabaseParams->actionEntry.flowId                          = (GT_U32)inArgs[76];
    ttiMultiActionDatabaseParams->actionEntry.setMacToMe                      = (GT_BOOL)inArgs[77];
    ttiMultiActionDatabaseParams->actionEntry.rxProtectionSwitchEnable        = (GT_BOOL)inArgs[78];
    ttiMultiActionDatabaseParams->actionEntry.rxIsProtectionPath              = (GT_BOOL)inArgs[79];
    ttiMultiActionDatabaseParams->actionEntry.pwTagMode                       = (CPSS_DXCH_TTI_PW_TAG_MODE_ENT)inArgs[80];

    ttiMultiActionDatabaseParams->actionEntry.oamTimeStampEnable              = (GT_BOOL)inArgs[81];
    ttiMultiActionDatabaseParams->actionEntry.oamOffsetIndex                  = (GT_U32)inArgs[82];
    ttiMultiActionDatabaseParams->actionEntry.oamProcessEnable                = (GT_BOOL)inArgs[83];
    ttiMultiActionDatabaseParams->actionEntry.oamProfile                      = (GT_U32)inArgs[84];
    ttiMultiActionDatabaseParams->actionEntry.oamChannelTypeToOpcodeMappingEnable = (GT_BOOL)inArgs[85];

    ttiMultiActionDatabaseParams->actionEntry.isPtpPacket                     = (GT_BOOL)inArgs[86];
    ttiMultiActionDatabaseParams->actionEntry.ptpTriggerType                  = (CPSS_DXCH_TTI_PTP_TRIGGER_TYPE_ENT)inArgs[87];
    ttiMultiActionDatabaseParams->actionEntry.ptpOffset                       = (GT_U32)inArgs[88];

    ttiMultiActionDatabaseParams->actionEntry.cwBasedPw                       = (GT_BOOL)inArgs[89];
    ttiMultiActionDatabaseParams->actionEntry.ttlExpiryVccvEnable             = (GT_BOOL)inArgs[90];
    ttiMultiActionDatabaseParams->actionEntry.pwe3FlowLabelExist              = (GT_BOOL)inArgs[91];
    ttiMultiActionDatabaseParams->actionEntry.pwCwBasedETreeEnable            = (GT_BOOL)inArgs[92];
    ttiMultiActionDatabaseParams->actionEntry.applyNonDataCwCommand           = (GT_BOOL)inArgs[93];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiMultiRuleActionGet
*
* DESCRIPTION:
*   The function get configured action (ttiMultiActionDatabaseParams)
*   to be used in multi rule set wrapper
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMultiRuleActionGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result = GT_OK;
    CPSS_DXCH_TTI_RULE_TYPE_ENT         keyType;
    ttiMultiActionDB                    *ttiMultiActionDatabaseParams;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    keyType = (CPSS_DXCH_TTI_RULE_TYPE_ENT)inArgs[0];

    switch (keyType) /* rule type */
    {
    case CPSS_DXCH_TTI_RULE_IPV4_E:
        /* not supported yet */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;

    case CPSS_DXCH_TTI_RULE_MPLS_E:
        /* not supported yet */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;

    case CPSS_DXCH_TTI_RULE_ETH_E:
        ttiMultiActionDatabaseParams = &ttiMultiActionDatabaseParamsForEthRule;
        break;

    case CPSS_DXCH_TTI_RULE_MIM_E:
        /* not supported yet */
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;

    case CPSS_DXCH_TTI_RULE_UDB_10_E:
        ttiMultiActionDatabaseParams = &ttiMultiActionDatabaseParamsForUdbRuleArray[0];
        break;
    case CPSS_DXCH_TTI_RULE_UDB_20_E:
        ttiMultiActionDatabaseParams = &ttiMultiActionDatabaseParamsForUdbRuleArray[1];
        break;
    case CPSS_DXCH_TTI_RULE_UDB_30_E:
        ttiMultiActionDatabaseParams = &ttiMultiActionDatabaseParamsForUdbRuleArray[2];
        break;

    default:
        galtisOutput(outArgs, (GT_STATUS)GT_ERROR, "\nERROR : Wrong TTI RULE FORMAT.\n");
        return CMD_AGENT_ERROR;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result,
                 "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                 "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                 "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                 "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                 "%d%d%d%d%d%d%d%d%d%d%d%d%d",
                 ttiMultiActionDatabaseParams->actionEntry.tunnelTerminate,
                 ttiMultiActionDatabaseParams->actionEntry.ttPassengerPacketType,
                 ttiMultiActionDatabaseParams->actionEntry.tsPassengerPacketType,
                 ttiMultiActionDatabaseParams->actionEntry.ttHeaderLength,
                 ttiMultiActionDatabaseParams->actionEntry.continueToNextTtiLookup,
                 ttiMultiActionDatabaseParams->actionEntry.copyTtlExpFromTunnelHeader,
                 ttiMultiActionDatabaseParams->actionEntry.mplsCommand,
                 ttiMultiActionDatabaseParams->actionEntry.mplsTtl,
                 ttiMultiActionDatabaseParams->actionEntry.enableDecrementTtl,
                 ttiMultiActionDatabaseParams->actionEntry.passengerParsingOfTransitMplsTunnelMode,
                 ttiMultiActionDatabaseParams->actionEntry.passengerParsingOfTransitNonMplsTransitTunnelEnable,
                 ttiMultiActionDatabaseParams->actionEntry.command,
                 ttiMultiActionDatabaseParams->actionEntry.redirectCommand,
                 ttiMultiActionDatabaseParams->actionEntry.egressInterface.type,
                 ttiMultiActionDatabaseParams->actionEntry.egressInterface.devPort.hwDevNum,
                 ttiMultiActionDatabaseParams->portNum_0,
                 ttiMultiActionDatabaseParams->portNum_1,
                 ttiMultiActionDatabaseParams->portNum_2,
                 ttiMultiActionDatabaseParams->portNum_3,
                 ttiMultiActionDatabaseParams->actionEntry.egressInterface.trunkId,
                 ttiMultiActionDatabaseParams->actionEntry.egressInterface.vidx,
                 ttiMultiActionDatabaseParams->actionEntry.egressInterface.vlanId,
                 ttiMultiActionDatabaseParams->actionEntry.arpPtr,
                 ttiMultiActionDatabaseParams->actionEntry.tunnelStart,
                 ttiMultiActionDatabaseParams->actionEntry.tunnelStartPtr,
                 ttiMultiActionDatabaseParams->actionEntry.routerLttPtr,
                 ttiMultiActionDatabaseParams->actionEntry.vrfId,
                 ttiMultiActionDatabaseParams->actionEntry.sourceIdSetEnable,
                 ttiMultiActionDatabaseParams->actionEntry.sourceId,
                 ttiMultiActionDatabaseParams->actionEntry.tag0VlanCmd,
                 ttiMultiActionDatabaseParams->actionEntry.tag0VlanId,
                 ttiMultiActionDatabaseParams->actionEntry.tag1VlanCmd,
                 ttiMultiActionDatabaseParams->actionEntry.tag1VlanId,
                 ttiMultiActionDatabaseParams->actionEntry.tag0VlanPrecedence,
                 ttiMultiActionDatabaseParams->actionEntry.nestedVlanEnable,
                 ttiMultiActionDatabaseParams->actionEntry.bindToPolicerMeter,
                 ttiMultiActionDatabaseParams->actionEntry.bindToPolicer,
                 ttiMultiActionDatabaseParams->actionEntry.policerIndex,
                 ttiMultiActionDatabaseParams->actionEntry.qosPrecedence,
                 ttiMultiActionDatabaseParams->actionEntry.keepPreviousQoS,
                 ttiMultiActionDatabaseParams->actionEntry.trustUp,
                 ttiMultiActionDatabaseParams->actionEntry.trustDscp,
                 ttiMultiActionDatabaseParams->actionEntry.trustExp,
                 ttiMultiActionDatabaseParams->actionEntry.qosProfile,
                 ttiMultiActionDatabaseParams->actionEntry.modifyTag0Up,
                 ttiMultiActionDatabaseParams->actionEntry.tag1UpCommand,
                 ttiMultiActionDatabaseParams->actionEntry.modifyDscp,
                 ttiMultiActionDatabaseParams->actionEntry.tag0Up,
                 ttiMultiActionDatabaseParams->actionEntry.tag1Up,
                 ttiMultiActionDatabaseParams->actionEntry.remapDSCP,
                 ttiMultiActionDatabaseParams->actionEntry.qosUseUpAsIndexEnable,
                 ttiMultiActionDatabaseParams->actionEntry.qosMappingTableIndex,
                 ttiMultiActionDatabaseParams->actionEntry.mplsLLspQoSProfileEnable,
                 ttiMultiActionDatabaseParams->actionEntry.pcl0OverrideConfigIndex,
                 ttiMultiActionDatabaseParams->actionEntry.pcl0_1OverrideConfigIndex,
                 ttiMultiActionDatabaseParams->actionEntry.pcl1OverrideConfigIndex,
                 ttiMultiActionDatabaseParams->actionEntry.iPclConfigIndex,
                 ttiMultiActionDatabaseParams->actionEntry.iPclUdbConfigTableEnable,
                 ttiMultiActionDatabaseParams->actionEntry.iPclUdbConfigTableIndex,
                 ttiMultiActionDatabaseParams->actionEntry.mirrorToIngressAnalyzerEnable,
                 ttiMultiActionDatabaseParams->actionEntry.mirrorToIngressAnalyzerIndex,
                 ttiMultiActionDatabaseParams->actionEntry.userDefinedCpuCode,
                 ttiMultiActionDatabaseParams->actionEntry.bindToCentralCounter,
                 ttiMultiActionDatabaseParams->actionEntry.centralCounterIndex,
                 ttiMultiActionDatabaseParams->actionEntry.vntl2Echo,
                 ttiMultiActionDatabaseParams->actionEntry.bridgeBypass,
                 ttiMultiActionDatabaseParams->actionEntry.ingressPipeBypass,
                 ttiMultiActionDatabaseParams->actionEntry.actionStop,
                 ttiMultiActionDatabaseParams->actionEntry.hashMaskIndex,
                 ttiMultiActionDatabaseParams->actionEntry.modifyMacSa,
                 ttiMultiActionDatabaseParams->actionEntry.modifyMacDa,
                 ttiMultiActionDatabaseParams->actionEntry.ResetSrcPortGroupId,
                 ttiMultiActionDatabaseParams->actionEntry.multiPortGroupTtiEnable,
                 ttiMultiActionDatabaseParams->actionEntry.sourceEPortAssignmentEnable,
                 ttiMultiActionDatabaseParams->actionEntry.sourceEPort,
                 ttiMultiActionDatabaseParams->actionEntry.flowId,
                 ttiMultiActionDatabaseParams->actionEntry.setMacToMe,
                 ttiMultiActionDatabaseParams->actionEntry.rxProtectionSwitchEnable,
                 ttiMultiActionDatabaseParams->actionEntry.rxIsProtectionPath,
                 ttiMultiActionDatabaseParams->actionEntry.pwTagMode,
                 ttiMultiActionDatabaseParams->actionEntry.oamTimeStampEnable,
                 ttiMultiActionDatabaseParams->actionEntry.oamOffsetIndex,
                 ttiMultiActionDatabaseParams->actionEntry.oamProcessEnable,
                 ttiMultiActionDatabaseParams->actionEntry.oamProfile,
                 ttiMultiActionDatabaseParams->actionEntry.oamChannelTypeToOpcodeMappingEnable,
                 ttiMultiActionDatabaseParams->actionEntry.isPtpPacket,
                 ttiMultiActionDatabaseParams->actionEntry.ptpTriggerType,
                 ttiMultiActionDatabaseParams->actionEntry.ptpOffset,
                 ttiMultiActionDatabaseParams->actionEntry.cwBasedPw,
                 ttiMultiActionDatabaseParams->actionEntry.ttlExpiryVccvEnable,
                 ttiMultiActionDatabaseParams->actionEntry.pwe3FlowLabelExist,
                 ttiMultiActionDatabaseParams->actionEntry.pwCwBasedETreeEnable,
                 ttiMultiActionDatabaseParams->actionEntry.applyNonDataCwCommand);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiMultiEthRuleSet
*
* DESCRIPTION:
*   The function configures Multiple number of TTI Ethernet Rules using the Multiple Actions
*   defined in wrCpssDxChTtiMultiRuleActionSet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMultiEthRuleSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result = GT_OK;
    GT_U8       devNum = 0;
    GT_U32      numOfRulesAdded=0;
    GT_U32      ruleIndex;
    GT_PORT_NUM port;
    GT_U32      i = 0;

    CPSS_DXCH_TTI_ACTION_UNT        actionEntry;
    GT_PORT_NUM                     portNum_0;      /* port interface to use when (ruleIndex % 4 == 0)                                      */
    GT_PORT_NUM                     portNum_1;      /* port interface to use when (ruleIndex % 4 == 1)                                      */
    GT_PORT_NUM                     portNum_2;      /* port interface to use when (ruleIndex % 4 == 2)                                      */
    GT_PORT_NUM                     portNum_3;      /* port interface to use when (ruleIndex % 4 == 3)                                      */
    GT_HW_DEV_NUM                   hwDevNum;

    GT_U32      startSec  = 0;
    GT_U32      startNsec = 0;
    GT_U32      endSec  = 0;
    GT_U32      endNsec = 0;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemCpy(&actionEntry.type2,&(ttiMultiActionDatabaseParamsForEthRule.actionEntry),sizeof(CPSS_DXCH_TTI_ACTION_2_STC));

    hwDevNum  = ttiMultiActionDatabaseParamsForEthRule.actionEntry.egressInterface.devPort.hwDevNum;
    portNum_0 = ttiMultiActionDatabaseParamsForEthRule.portNum_0;
    portNum_1 = ttiMultiActionDatabaseParamsForEthRule.portNum_1;
    portNum_2 = ttiMultiActionDatabaseParamsForEthRule.portNum_2;
    portNum_3 = ttiMultiActionDatabaseParamsForEthRule.portNum_3;

    devNum = ttiMultiActionDatabaseParamsForEthRule.devNum  = (GT_U8)inArgs[0];
    ttiMultiActionDatabaseParamsForEthRule.numOfRulesToAdd  = (GT_U32)inArgs[1];
    ttiMultiActionDatabaseParamsForEthRule.ruleIndex  = (GT_U32)inArgs[2];
    ttiMultiActionDatabaseParamsForEthRule.ruleIndexDelta  = (GT_U32)inArgs[3];

    ruleIndex=ttiMultiActionDatabaseParamsForEthRule.ruleIndex;/* set first ruleIndex for the loop */

    /* reset mask and pattern */
    cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));

    patternData.eth.common.pclId           = (GT_U32)inArgs[4];
    maskData.eth.common.pclId        = (GT_U32)inArgs[5];
    patternData.eth.common.srcIsTrunk      = (GT_U32)inArgs[6];
    maskData.eth.common.srcIsTrunk   = (GT_U32)inArgs[7];
    patternData.eth.common.srcPortTrunk    = (GT_U32)inArgs[8];
    maskData.eth.common.srcPortTrunk = (GT_U32)inArgs[9];
    galtisMacAddr(&patternData.eth.common.mac,(GT_U8*)inArgs[10]);
    galtisMacAddr(&maskData.eth.common.mac,(GT_U8*)inArgs[11]);
    patternData.eth.common.vid             = (GT_U16)inArgs[12];
    maskData.eth.common.vid          = (GT_U16)inArgs[13];
    patternData.eth.common.isTagged        = (GT_BOOL)inArgs[14];
    maskData.eth.common.isTagged     = (GT_BOOL)inArgs[15];
    patternData.eth.up0                    = (GT_U32)inArgs[16];
    maskData.eth.up0                 = (GT_U32)inArgs[17];
    patternData.eth.cfi0                   = (GT_U32)inArgs[18];
    maskData.eth.cfi0                = (GT_U32)inArgs[19];
    patternData.eth.isVlan1Exists          = (GT_BOOL)inArgs[20];
    maskData.eth.isVlan1Exists       = (GT_BOOL)inArgs[21];
    patternData.eth.vid1                   = (GT_U16)inArgs[22];
    maskData.eth.vid1                = (GT_U16)inArgs[23];
    patternData.eth.up1                    = (GT_U32)inArgs[24];
    maskData.eth.up1                 = (GT_U32)inArgs[25];
    patternData.eth.cfi1                   = (GT_U32)inArgs[26];
    maskData.eth.cfi1                = (GT_U32)inArgs[27];
    patternData.eth.etherType              = (GT_U32)inArgs[28];
    maskData.eth.etherType           = (GT_U32)inArgs[29];
    patternData.eth.macToMe                = (GT_BOOL)inArgs[30];
    maskData.eth.macToMe             = (GT_BOOL)inArgs[31];
    patternData.eth.common.dsaSrcIsTrunk   = inArgs[32];
    maskData.eth.common.dsaSrcIsTrunk= inArgs[33];
    patternData.eth.common.dsaSrcPortTrunk = (GT_U32)inArgs[34];
    maskData.eth.common.dsaSrcPortTrunk = (GT_U32)inArgs[35];
    patternData.eth.common.dsaSrcDevice    = (GT_U32)inArgs[36];
    maskData.eth.common.dsaSrcDevice = (GT_U32)inArgs[37];
    patternData.eth.srcId                  = (GT_U32)inArgs[38];
    maskData.eth.srcId               = (GT_U32)inArgs[39];
    patternData.eth.dsaQosProfile          = (GT_U32)inArgs[40];
    maskData.eth.dsaQosProfile       = (GT_U32)inArgs[41];
    patternData.eth.common.sourcePortGroupId = (GT_U32)inArgs[42];
    maskData.eth.common.sourcePortGroupId = (GT_U32)inArgs[43];

    cpssOsMemCpy(&ethRuleMaskData,&maskData, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cpssOsMemCpy(&ethRulePatternData,&patternData, sizeof(CPSS_DXCH_TTI_RULE_UNT));

    if (patternData.eth.common.srcIsTrunk == GT_FALSE)
    {
        port = (GT_PORT_NUM)patternData.eth.common.srcPortTrunk;

       /* Override Device and Port , should be done for pattern as in mask you cant know how to convert*/
       CONVERT_DEV_PORT_U32_MAC(devNum,port);

       patternData.eth.common.srcPortTrunk = (GT_U32)port;
    }

    /* call cpss api function */
    result = cpssOsTimeRT(&startSec, &startNsec);
    if(result != GT_OK)
    {
        result = cpssOsTimeRT(&endSec, &endNsec);
        if(result != GT_OK)
        {
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, result, "%d",numOfRulesAdded);


            galtisOutput(outArgs, result, "");
            return CMD_OK;
        }
        if (endNsec < startNsec)
        {
            endNsec += 1000000000;
            endSec--;
        }

        cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
        cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

        /* print result */
        if (numOfRulesAdded == 0)
        {
            cpssOsPrintf("\n    No TTI Rules were added at all.\n");
        }
        else
        {
            cpssOsPrintf("\n Rules index from %d to %d, in delta of %d.\n",
                         ttiMultiActionDatabaseParamsForEthRule.ruleIndex,
                         ruleIndex,
                         ttiMultiActionDatabaseParamsForEthRule.ruleIndexDelta);
            cpssOsPrintf(" Were added (%d/%d)\n",
                         numOfRulesAdded,
                         ttiMultiActionDatabaseParamsForEthRule.numOfRulesToAdd);
        }
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d",numOfRulesAdded);

        return CMD_OK;
    }

    /* add TTI rules entries */
    for(i=0; i<ttiMultiActionDatabaseParamsForEthRule.numOfRulesToAdd; i++)
    {
       /* the ruleIndex will be changed according to the ruleIndexDelta */
        ruleIndex = ttiMultiActionDatabaseParamsForEthRule.ruleIndex + (i * ttiMultiActionDatabaseParamsForEthRule.ruleIndexDelta);

       /* each entry should have a different action - the action is different
       in its port destination in case the destination is a port,
       else it will be the same action values for all entries */

        actionEntry.type2.egressInterface.devPort.hwDevNum = hwDevNum;

        switch(ruleIndex%4)
        {
            case 0:
                actionEntry.type2.egressInterface.devPort.portNum = portNum_0;
                break;
            case 1:
                actionEntry.type2.egressInterface.devPort.portNum = portNum_1;
                break;
            case 2:
                actionEntry.type2.egressInterface.devPort.portNum = portNum_2;
                break;
            case 3:
                actionEntry.type2.egressInterface.devPort.portNum = portNum_3;
                break;
            default:
                break;
        }
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionEntry.type2.egressInterface.devPort.hwDevNum,
                                  actionEntry.type2.egressInterface.devPort.portNum);

        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionEntry.type2.egressInterface.trunkId);

        /* call cpss api function - ruleIndex is an absolute index */
        result = pg_wrap_cpssDxChTtiRuleSet(devNum,
                                            ruleIndex,
                                            CPSS_DXCH_TTI_RULE_ETH_E,
                                            &patternData,&maskData
                                            ,CPSS_DXCH_TTI_ACTION_TYPE2_ENT,
                                            &actionEntry,
                                            GT_TRUE);
        if(result != GT_OK)
        {
            galtisOutput(outArgs, result, "numOfRulesAdded=%d",numOfRulesAdded);
            return CMD_OK;
        }

        ttiRuleFormat[ruleIndex]     = CPSS_DXCH_TTI_RULE_ETH_E;
        ttiRuleActionType[ruleIndex] = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        numOfRulesAdded++;

         /* each entry should have an incremented mac address (patternData) */
        if(patternData.eth.common.mac.arEther[5] < 0xFF)
            patternData.eth.common.mac.arEther[5]++;
        else
        {
            patternData.eth.common.mac.arEther[5] = 0;
            if (patternData.eth.common.mac.arEther[4] < 0xFF)
                patternData.eth.common.mac.arEther[4]++;
            else
            {
                patternData.eth.common.mac.arEther[4] = 0;
                if (patternData.eth.common.mac.arEther[3] < 0xFF)
                    patternData.eth.common.mac.arEther[3]++;
                else
                {
                    patternData.eth.common.mac.arEther[3] = 0;
                    if (patternData.eth.common.mac.arEther[2] < 0XFF)
                        patternData.eth.common.mac.arEther[2]++;
                    else
                    {
                        patternData.eth.common.mac.arEther[2] = 0;
                        if (patternData.eth.common.mac.arEther[1] < 0XFF)
                            patternData.eth.common.mac.arEther[1]++;
                        else
                        {
                            patternData.eth.common.mac.arEther[1] = 0;
                            if (patternData.eth.common.mac.arEther[0] < 0XFF)
                                patternData.eth.common.mac.arEther[0]++;
                            else
                            {
                                result = GT_NO_MORE;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    result = cpssOsTimeRT(&endSec, &endNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if (endNsec < startNsec)
    {
        endNsec += 1000000000;
        endSec--;
    }

    cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
    cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

    /* print result */
    if (numOfRulesAdded == 0)
    {
        cpssOsPrintf("\n    No TTI Rules were added at all.\n");
    }
    else
    {
        cpssOsPrintf("\n Rules index from %d to %d, in delta of %d.\n",
                     ttiMultiActionDatabaseParamsForEthRule.ruleIndex,
                     ruleIndex,
                     ttiMultiActionDatabaseParamsForEthRule.ruleIndexDelta);
        cpssOsPrintf(" Were added (%d/%d)\n",
                     numOfRulesAdded,
                     ttiMultiActionDatabaseParamsForEthRule.numOfRulesToAdd);
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfRulesAdded);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChTtiMultiEthRuleGet
*
* DESCRIPTION:
*   The function get configuration of Multiple number of TTI Ethernet Rules using the
*   Multiple Actions defined in wrCpssDxChTtiMultiRuleActionSet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMultiEthRuleGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result = GT_OK;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result,
                 "%d%d%d%d%d%d%d%d%d%d"
                 "%6b%6b%d%d%d%d%d%d%d%d"
                 "%d%d%d%d%d%d%d%d%d%d"
                 "%d%d%d%d%d%d%d%d%d%d"
                 "%d%d%d%d",
                 ttiMultiActionDatabaseParamsForEthRule.devNum,
                 ttiMultiActionDatabaseParamsForEthRule.numOfRulesToAdd,
                 ttiMultiActionDatabaseParamsForEthRule.ruleIndex,
                 ttiMultiActionDatabaseParamsForEthRule.ruleIndexDelta,
                 ethRulePatternData.eth.common.pclId,
                 ethRuleMaskData.eth.common.pclId,
                 ethRulePatternData.eth.common.srcIsTrunk,
                 ethRuleMaskData.eth.common.srcIsTrunk,
                 ethRulePatternData.eth.common.srcPortTrunk,
                 ethRuleMaskData.eth.common.srcPortTrunk,
                 ethRulePatternData.eth.common.mac.arEther,
                 ethRuleMaskData.eth.common.mac.arEther,
                 ethRulePatternData.eth.common.vid,
                 ethRuleMaskData.eth.common.vid,
                 ethRulePatternData.eth.common.isTagged,
                 ethRuleMaskData.eth.common.isTagged,
                 ethRulePatternData.eth.up0,
                 ethRuleMaskData.eth.up0,
                 ethRulePatternData.eth.cfi0,
                 ethRuleMaskData.eth.cfi0,
                 ethRulePatternData.eth.isVlan1Exists,
                 ethRuleMaskData.eth.isVlan1Exists,
                 ethRulePatternData.eth.vid1,
                 ethRuleMaskData.eth.vid1,
                 ethRulePatternData.eth.up1,
                 ethRuleMaskData.eth.up1,
                 ethRulePatternData.eth.cfi1,
                 ethRuleMaskData.eth.cfi1,
                 ethRulePatternData.eth.etherType,
                 ethRuleMaskData.eth.etherType,
                 ethRulePatternData.eth.macToMe,
                 ethRuleMaskData.eth.macToMe,
                 ethRulePatternData.eth.common.dsaSrcIsTrunk,
                 ethRuleMaskData.eth.common.dsaSrcIsTrunk,
                 ethRulePatternData.eth.common.dsaSrcPortTrunk,
                 ethRuleMaskData.eth.common.dsaSrcPortTrunk,
                 ethRulePatternData.eth.common.dsaSrcDevice,
                 ethRuleMaskData.eth.common.dsaSrcDevice,
                 ethRulePatternData.eth.srcId,
                 ethRuleMaskData.eth.srcId,
                 ethRulePatternData.eth.dsaQosProfile,
                 ethRuleMaskData.eth.dsaQosProfile,
                 ethRulePatternData.eth.common.sourcePortGroupId,
                 ethRuleMaskData.eth.common.sourcePortGroupId);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChTtiMultiEthRuleDelete
*
* DESCRIPTION:
*   The function delete configuration of Multiple number of TTI Ethernet Rules using the
*   Multiple Actions defined in wrCpssDxChTtiMultiRuleActionSet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMultiEthRuleDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;
    GT_U8                           devNum;         /* device associated with the action     */
    GT_U32                          ruleIndex;      /* rule index associated with the action */
    GT_U32                          ruleIndexDelta; /* delta between 2 indexes               */
    GT_U32                          numOfRulesToDelete;
    GT_U32                          i=0;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum =(GT_U8)inArgs[0];
    numOfRulesToDelete = (GT_U32)inArgs[1];
    ruleIndex = (GT_U32)inArgs[2];
    ruleIndexDelta = 3;

    /* go over all range defined in wrCpssDxChTtiMultiEthRuleSet and
       delete in from HW and from DB */
    for (i=0;i < numOfRulesToDelete; i++)
    {
        /* call cpss api function - routerTtiTcamRow is an absolute index */
        result = pg_wrap_cpssDxChTtiRuleValidStatusSet(devNum, ruleIndex, GT_FALSE, GT_TRUE);
        if(result != GT_OK)
        {
            galtisOutput(outArgs, result, "numOfRulesDeleted=%d",i+1);
            return CMD_OK;
        }
        ruleIndex+=ruleIndexDelta;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChTtiMultiUdbRuleSet
*
* DESCRIPTION:
*   The function configures Multiple number of TTI UDB Rules using the Multiple Actions
*   defined in wrCpssDxChTtiMultiRuleActionSet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMultiUdbRuleSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result = GT_OK;
    GT_U8       devNum = 0;
    GT_U32      numOfRulesAdded=0;
    GT_U32      ruleIndex;
    GT_U32      i = 0;

    CPSS_DXCH_TTI_ACTION_UNT        actionEntry;
    GT_PORT_NUM                     portNum_0;      /* port interface to use when (ruleIndex % 4 == 0)                                      */
    GT_PORT_NUM                     portNum_1;      /* port interface to use when (ruleIndex % 4 == 1)                                      */
    GT_PORT_NUM                     portNum_2;      /* port interface to use when (ruleIndex % 4 == 2)                                      */
    GT_PORT_NUM                     portNum_3;      /* port interface to use when (ruleIndex % 4 == 3)                                      */
    GT_HW_DEV_NUM                   hwDevNum;
    CPSS_DXCH_TTI_RULE_TYPE_ENT     ruleFormat;
    ttiMultiActionDB                *ttiMultiActionDatabaseParamsForUdbRule;
    CPSS_DXCH_TTI_KEY_SIZE_ENT      udbRuleSize;    /* size of the TTI UDB Rule              */

    GT_U32      startSec  = 0;
    GT_U32      startNsec = 0;
    GT_U32      endSec  = 0;
    GT_U32      endNsec = 0;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* reset mask and pattern */
    cmdOsMemSet(&maskData , 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));
    cmdOsMemSet(&patternData, 0, sizeof(CPSS_DXCH_TTI_RULE_UNT));


    udbRuleSize  = (CPSS_DXCH_TTI_KEY_SIZE_ENT)inArgs[4];

    switch (udbRuleSize) /* rule size */
    {
        case CPSS_DXCH_TTI_KEY_SIZE_10_B_E:
            ttiMultiActionDatabaseParamsForUdbRule = &ttiMultiActionDatabaseParamsForUdbRuleArray[0];
            cpssOsMemCpy(&actionEntry.type2,&(ttiMultiActionDatabaseParamsForUdbRuleArray[0].actionEntry),sizeof(CPSS_DXCH_TTI_ACTION_2_STC));

            ruleFormat = CPSS_DXCH_TTI_RULE_UDB_10_E;

            patternData.udbArray[0] = (GT_U8)inArgs[5];
            patternData.udbArray[1] = (GT_U8)inArgs[7];
            patternData.udbArray[2] = (GT_U8)inArgs[9];
            patternData.udbArray[3] = (GT_U8)inArgs[11];
            patternData.udbArray[4] = (GT_U8)inArgs[13];
            patternData.udbArray[5] = (GT_U8)inArgs[15];
            patternData.udbArray[6] = (GT_U8)inArgs[17];
            patternData.udbArray[7] = (GT_U8)inArgs[19];
            patternData.udbArray[8] = (GT_U8)inArgs[21];
            patternData.udbArray[9] = (GT_U8)inArgs[23];

            maskData.udbArray[0] = (GT_U8)inArgs[6];
            maskData.udbArray[1] = (GT_U8)inArgs[8];
            maskData.udbArray[2] = (GT_U8)inArgs[10];
            maskData.udbArray[3] = (GT_U8)inArgs[12];
            maskData.udbArray[4] = (GT_U8)inArgs[14];
            maskData.udbArray[5] = (GT_U8)inArgs[16];
            maskData.udbArray[6] = (GT_U8)inArgs[18];
            maskData.udbArray[7] = (GT_U8)inArgs[20];
            maskData.udbArray[8] = (GT_U8)inArgs[22];
            maskData.udbArray[9] = (GT_U8)inArgs[24];
            cpssOsMemCpy(&udbRuleMaskDataArray[0],&maskData, sizeof(CPSS_DXCH_TTI_RULE_UNT));
            cpssOsMemCpy(&udbRulePatternDataArray[0],&patternData, sizeof(CPSS_DXCH_TTI_RULE_UNT));

            break;

        case CPSS_DXCH_TTI_KEY_SIZE_20_B_E:
            ttiMultiActionDatabaseParamsForUdbRule = &ttiMultiActionDatabaseParamsForUdbRuleArray[1];
            cpssOsMemCpy(&actionEntry.type2,&(ttiMultiActionDatabaseParamsForUdbRuleArray[1].actionEntry),sizeof(CPSS_DXCH_TTI_ACTION_2_STC));

            ruleFormat = CPSS_DXCH_TTI_RULE_UDB_20_E;

            patternData.udbArray[0] = (GT_U8)inArgs[5];
            patternData.udbArray[1] = (GT_U8)inArgs[7];
            patternData.udbArray[2] = (GT_U8)inArgs[9];
            patternData.udbArray[3] = (GT_U8)inArgs[11];
            patternData.udbArray[4] = (GT_U8)inArgs[13];
            patternData.udbArray[5] = (GT_U8)inArgs[15];
            patternData.udbArray[6] = (GT_U8)inArgs[17];
            patternData.udbArray[7] = (GT_U8)inArgs[19];
            patternData.udbArray[8] = (GT_U8)inArgs[21];
            patternData.udbArray[9] = (GT_U8)inArgs[23];

            patternData.udbArray[10] = (GT_U8)inArgs[25];
            patternData.udbArray[11] = (GT_U8)inArgs[27];
            patternData.udbArray[12] = (GT_U8)inArgs[29];
            patternData.udbArray[13] = (GT_U8)inArgs[31];
            patternData.udbArray[14] = (GT_U8)inArgs[33];
            patternData.udbArray[15] = (GT_U8)inArgs[35];
            patternData.udbArray[16] = (GT_U8)inArgs[37];
            patternData.udbArray[17] = (GT_U8)inArgs[39];
            patternData.udbArray[18] = (GT_U8)inArgs[41];
            patternData.udbArray[19] = (GT_U8)inArgs[43];

            maskData.udbArray[0] = (GT_U8)inArgs[6];
            maskData.udbArray[1] = (GT_U8)inArgs[8];
            maskData.udbArray[2] = (GT_U8)inArgs[10];
            maskData.udbArray[3] = (GT_U8)inArgs[12];
            maskData.udbArray[4] = (GT_U8)inArgs[14];
            maskData.udbArray[5] = (GT_U8)inArgs[16];
            maskData.udbArray[6] = (GT_U8)inArgs[18];
            maskData.udbArray[7] = (GT_U8)inArgs[20];
            maskData.udbArray[8] = (GT_U8)inArgs[22];
            maskData.udbArray[9] = (GT_U8)inArgs[24];

            maskData.udbArray[10] = (GT_U8)inArgs[26];
            maskData.udbArray[11] = (GT_U8)inArgs[28];
            maskData.udbArray[12] = (GT_U8)inArgs[30];
            maskData.udbArray[13] = (GT_U8)inArgs[32];
            maskData.udbArray[14] = (GT_U8)inArgs[34];
            maskData.udbArray[15] = (GT_U8)inArgs[36];
            maskData.udbArray[16] = (GT_U8)inArgs[38];
            maskData.udbArray[17] = (GT_U8)inArgs[40];
            maskData.udbArray[18] = (GT_U8)inArgs[42];
            maskData.udbArray[19] = (GT_U8)inArgs[44];
            cpssOsMemCpy(&udbRuleMaskDataArray[1],&maskData, sizeof(CPSS_DXCH_TTI_RULE_UNT));
            cpssOsMemCpy(&udbRulePatternDataArray[1],&patternData, sizeof(CPSS_DXCH_TTI_RULE_UNT));
            break;

        case CPSS_DXCH_TTI_KEY_SIZE_30_B_E:
            ttiMultiActionDatabaseParamsForUdbRule = &ttiMultiActionDatabaseParamsForUdbRuleArray[2];
            cpssOsMemCpy(&actionEntry.type2,&(ttiMultiActionDatabaseParamsForUdbRuleArray[2].actionEntry),sizeof(CPSS_DXCH_TTI_ACTION_2_STC));

            ruleFormat = CPSS_DXCH_TTI_RULE_UDB_30_E;

             patternData.udbArray[0] = (GT_U8)inArgs[5];
            patternData.udbArray[1] = (GT_U8)inArgs[7];
            patternData.udbArray[2] = (GT_U8)inArgs[9];
            patternData.udbArray[3] = (GT_U8)inArgs[11];
            patternData.udbArray[4] = (GT_U8)inArgs[13];
            patternData.udbArray[5] = (GT_U8)inArgs[15];
            patternData.udbArray[6] = (GT_U8)inArgs[17];
            patternData.udbArray[7] = (GT_U8)inArgs[19];
            patternData.udbArray[8] = (GT_U8)inArgs[21];
            patternData.udbArray[9] = (GT_U8)inArgs[23];

            patternData.udbArray[10] = (GT_U8)inArgs[25];
            patternData.udbArray[11] = (GT_U8)inArgs[27];
            patternData.udbArray[12] = (GT_U8)inArgs[29];
            patternData.udbArray[13] = (GT_U8)inArgs[31];
            patternData.udbArray[14] = (GT_U8)inArgs[33];
            patternData.udbArray[15] = (GT_U8)inArgs[35];
            patternData.udbArray[16] = (GT_U8)inArgs[37];
            patternData.udbArray[17] = (GT_U8)inArgs[39];
            patternData.udbArray[18] = (GT_U8)inArgs[41];
            patternData.udbArray[19] = (GT_U8)inArgs[43];

            patternData.udbArray[20] = (GT_U8)inArgs[45];
            patternData.udbArray[21] = (GT_U8)inArgs[47];
            patternData.udbArray[22] = (GT_U8)inArgs[49];
            patternData.udbArray[23] = (GT_U8)inArgs[51];
            patternData.udbArray[24] = (GT_U8)inArgs[53];
            patternData.udbArray[25] = (GT_U8)inArgs[55];
            patternData.udbArray[26] = (GT_U8)inArgs[57];
            patternData.udbArray[27] = (GT_U8)inArgs[59];
            patternData.udbArray[28] = (GT_U8)inArgs[61];
            patternData.udbArray[29] = (GT_U8)inArgs[63];

            maskData.udbArray[0] = (GT_U8)inArgs[6];
            maskData.udbArray[1] = (GT_U8)inArgs[8];
            maskData.udbArray[2] = (GT_U8)inArgs[10];
            maskData.udbArray[3] = (GT_U8)inArgs[12];
            maskData.udbArray[4] = (GT_U8)inArgs[14];
            maskData.udbArray[5] = (GT_U8)inArgs[16];
            maskData.udbArray[6] = (GT_U8)inArgs[18];
            maskData.udbArray[7] = (GT_U8)inArgs[20];
            maskData.udbArray[8] = (GT_U8)inArgs[22];
            maskData.udbArray[9] = (GT_U8)inArgs[24];

            maskData.udbArray[10] = (GT_U8)inArgs[26];
            maskData.udbArray[11] = (GT_U8)inArgs[28];
            maskData.udbArray[12] = (GT_U8)inArgs[30];
            maskData.udbArray[13] = (GT_U8)inArgs[32];
            maskData.udbArray[14] = (GT_U8)inArgs[34];
            maskData.udbArray[15] = (GT_U8)inArgs[36];
            maskData.udbArray[16] = (GT_U8)inArgs[38];
            maskData.udbArray[17] = (GT_U8)inArgs[40];
            maskData.udbArray[18] = (GT_U8)inArgs[42];
            maskData.udbArray[19] = (GT_U8)inArgs[44];

            maskData.udbArray[20] = (GT_U8)inArgs[46];
            maskData.udbArray[21] = (GT_U8)inArgs[48];
            maskData.udbArray[22] = (GT_U8)inArgs[50];
            maskData.udbArray[23] = (GT_U8)inArgs[52];
            maskData.udbArray[24] = (GT_U8)inArgs[54];
            maskData.udbArray[25] = (GT_U8)inArgs[56];
            maskData.udbArray[26] = (GT_U8)inArgs[58];
            maskData.udbArray[27] = (GT_U8)inArgs[60];
            maskData.udbArray[28] = (GT_U8)inArgs[62];
            maskData.udbArray[29] = (GT_U8)inArgs[64];

            cpssOsMemCpy(&udbRuleMaskDataArray[2],&maskData, sizeof(CPSS_DXCH_TTI_RULE_UNT));
            cpssOsMemCpy(&udbRulePatternDataArray[2],&patternData, sizeof(CPSS_DXCH_TTI_RULE_UNT));

            break;

        default:
            galtisOutput(outArgs, result, "");
            return CMD_OK;
    }

    hwDevNum  = ttiMultiActionDatabaseParamsForUdbRule->actionEntry.egressInterface.devPort.hwDevNum;
    portNum_0 = ttiMultiActionDatabaseParamsForUdbRule->portNum_0;
    portNum_1 = ttiMultiActionDatabaseParamsForUdbRule->portNum_1;
    portNum_2 = ttiMultiActionDatabaseParamsForUdbRule->portNum_2;
    portNum_3 = ttiMultiActionDatabaseParamsForUdbRule->portNum_3;

    devNum = ttiMultiActionDatabaseParamsForUdbRule->devNum  = (GT_U8)inArgs[0];
    ttiMultiActionDatabaseParamsForUdbRule->numOfRulesToAdd  = (GT_U32)inArgs[1];
    ttiMultiActionDatabaseParamsForUdbRule->ruleIndex        = (GT_U32)inArgs[2];
    ttiMultiActionDatabaseParamsForUdbRule->ruleIndexDelta   = (GT_U32)inArgs[3];

    ruleIndex=ttiMultiActionDatabaseParamsForUdbRule->ruleIndex;/* set first ruleIndex for the loop */

    /* call cpss api function */
    result = cpssOsTimeRT(&startSec, &startNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* add TTI rules entries */
    for(i=0; i<ttiMultiActionDatabaseParamsForUdbRule->numOfRulesToAdd; i++)
    {
       /* the ruleIndex will be changed according to the ruleIndexDelta */
        ruleIndex = ttiMultiActionDatabaseParamsForUdbRule->ruleIndex + (i * ttiMultiActionDatabaseParamsForUdbRule->ruleIndexDelta);

       /* each entry should have a different action - the action is different
       in its port destination in case the destination is a port,
       else it will be the same action values for all entries */

        actionEntry.type2.egressInterface.devPort.hwDevNum = hwDevNum;

        switch(ruleIndex%4)
        {
            case 0:
                actionEntry.type2.egressInterface.devPort.portNum = portNum_0;
                break;
            case 1:
                actionEntry.type2.egressInterface.devPort.portNum = portNum_1;
                break;
            case 2:
                actionEntry.type2.egressInterface.devPort.portNum = portNum_2;
                break;
            case 3:
                actionEntry.type2.egressInterface.devPort.portNum = portNum_3;
                break;
            default:
                break;
        }
        /* Override Device and Port */
        CONVERT_DEV_PORT_DATA_MAC(actionEntry.type2.egressInterface.devPort.hwDevNum,
                                  actionEntry.type2.egressInterface.devPort.portNum);

        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(actionEntry.type2.egressInterface.trunkId);

        /* call cpss api function - ruleIndex is an absolute index */
        result = pg_wrap_cpssDxChTtiRuleSet(devNum,
                                            ruleIndex,
                                            ruleFormat,
                                            &patternData,&maskData
                                            ,CPSS_DXCH_TTI_ACTION_TYPE2_ENT,
                                            &actionEntry,
                                            GT_TRUE);
        if(result != GT_OK)
        {

            result = cpssOsTimeRT(&endSec, &endNsec);
            if(result != GT_OK)
            {
                galtisOutput(outArgs, result, "%d",numOfRulesAdded);
                return CMD_OK;
            }
            if (endNsec < startNsec)
            {
                endNsec += 1000000000;
                endSec--;
            }

            cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
            cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

            /* print result */
            if (numOfRulesAdded == 0)
            {
                cpssOsPrintf("\n    No TTI Rules were added at all.\n");
            }
            else
            {
                cpssOsPrintf("\n Rules index from %d to %d, in delta of %d.\n",
                             ttiMultiActionDatabaseParamsForUdbRule->ruleIndex,
                             ruleIndex,
                             ttiMultiActionDatabaseParamsForUdbRule->ruleIndexDelta);
                cpssOsPrintf(" Were added (%d/%d)\n",
                             numOfRulesAdded,
                             ttiMultiActionDatabaseParamsForUdbRule->numOfRulesToAdd);
            }
            /* pack output arguments to galtis string */
            galtisOutput(outArgs, result, "%d",numOfRulesAdded);

            return CMD_OK;
        }

        ttiRuleFormat[ruleIndex]     = ruleFormat;
        ttiRuleActionType[ruleIndex] = CPSS_DXCH_TTI_ACTION_TYPE2_ENT;

        numOfRulesAdded++;

         /* each entry should have an incremented udb (patternData) */
        if(patternData.udbArray[0] < 0xFF)
            patternData.udbArray[0]++;
        else
        {
            patternData.udbArray[0] = 0;
            if (patternData.udbArray[1] < 0xFF)
                patternData.udbArray[1]++;
            else
            {
                patternData.udbArray[1] = 0;
                if (patternData.udbArray[2] < 0xFF)
                    patternData.udbArray[2]++;
                else
                {
                    patternData.udbArray[2] = 0;
                    if (patternData.udbArray[3] < 0XFF)
                        patternData.udbArray[3]++;
                    else
                    {
                        patternData.udbArray[3] = 0;
                        if (patternData.udbArray[4] < 0XFF)
                            patternData.udbArray[4]++;
                        else
                        {
                            patternData.udbArray[4] = 0;
                            if (patternData.udbArray[5] < 0XFF)
                                patternData.udbArray[5]++;
                            else
                            {
                                result = GT_NO_MORE;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    result = cpssOsTimeRT(&endSec, &endNsec);
    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
    if (endNsec < startNsec)
    {
        endNsec += 1000000000;
        endSec--;
    }

    cpssOsPrintf("\n processTimeSec = %d\n",endSec - startSec);
    cpssOsPrintf("\n processTimeNsec = %d\n",endNsec - startNsec);

    /* print result */
    if (numOfRulesAdded == 0)
    {
        cpssOsPrintf("\n    No TTI Rules were added at all.\n");
    }
    else
    {
        cpssOsPrintf("\n Rules index from %d to %d, in delta of %d.\n",
                     ttiMultiActionDatabaseParamsForUdbRule->ruleIndex,
                     ruleIndex,
                     ttiMultiActionDatabaseParamsForUdbRule->ruleIndexDelta);
        cpssOsPrintf(" Were added (%d/%d)\n",
                     numOfRulesAdded,
                     ttiMultiActionDatabaseParamsForUdbRule->numOfRulesToAdd);
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",numOfRulesAdded);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChTtiMultiUdbRuleGet
*
* DESCRIPTION:
*   The function get configuration of Multiple number of TTI UDB Rules using the
*   Multiple Actions defined in wrCpssDxChTtiMultiRuleActionSet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMultiUdbRuleGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                    result = GT_OK;
    CPSS_DXCH_TTI_KEY_SIZE_ENT   udbRuleSize;    /* size of the TTI UDB Rule              */
    ttiMultiActionDB             *ttiMultiActionDatabaseParamsForUdbRule;
    CPSS_DXCH_TTI_RULE_UNT       *udbRuleMaskData;
    CPSS_DXCH_TTI_RULE_UNT       *udbRulePatternData;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    udbRuleSize  = (CPSS_DXCH_TTI_KEY_SIZE_ENT)inArgs[0];

    switch (udbRuleSize) /* rule size */
    {
        case CPSS_DXCH_TTI_KEY_SIZE_10_B_E:
            ttiMultiActionDatabaseParamsForUdbRule = &ttiMultiActionDatabaseParamsForUdbRuleArray[0];
            udbRuleMaskData = &udbRuleMaskDataArray[0];
            udbRulePatternData = &udbRulePatternDataArray[0];
            break;

        case CPSS_DXCH_TTI_KEY_SIZE_20_B_E:
            ttiMultiActionDatabaseParamsForUdbRule = &ttiMultiActionDatabaseParamsForUdbRuleArray[1];
            udbRuleMaskData = &udbRuleMaskDataArray[1];
            udbRulePatternData = &udbRulePatternDataArray[1];
            break;

        case CPSS_DXCH_TTI_KEY_SIZE_30_B_E:
            ttiMultiActionDatabaseParamsForUdbRule = &ttiMultiActionDatabaseParamsForUdbRuleArray[2];
            udbRuleMaskData = &udbRuleMaskDataArray[2];
            udbRulePatternData = &udbRulePatternDataArray[2];
            break;

        default:
            galtisOutput(outArgs, result, "");
            return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result,
                 "%d%d%d%d%d%d%d%d%d%d"
                 "%d%d%d%d%d%d%d%d%d%d"
                 "%d%d%d%d%d%d%d%d%d%d"
                 "%d%d%d%d%d%d%d%d%d%d"
                 "%d%d%d%d%d%d%d%d%d%d"
                 "%d%d%d%d%d%d%d%d%d%d"
                 "%d%d%d%d",
                 ttiMultiActionDatabaseParamsForUdbRule->devNum,
                 ttiMultiActionDatabaseParamsForUdbRule->numOfRulesToAdd,
                 ttiMultiActionDatabaseParamsForUdbRule->ruleIndex,
                 ttiMultiActionDatabaseParamsForUdbRule->ruleIndexDelta,
                 udbRulePatternData->udbArray[0],
                 udbRuleMaskData->udbArray[0],
                 udbRulePatternData->udbArray[1],
                 udbRuleMaskData->udbArray[1],
                 udbRulePatternData->udbArray[2],
                 udbRuleMaskData->udbArray[2],
                 udbRulePatternData->udbArray[3],
                 udbRuleMaskData->udbArray[3],
                 udbRulePatternData->udbArray[4],
                 udbRuleMaskData->udbArray[4],
                 udbRulePatternData->udbArray[5],
                 udbRuleMaskData->udbArray[5],
                 udbRulePatternData->udbArray[6],
                 udbRuleMaskData->udbArray[6],
                 udbRulePatternData->udbArray[7],
                 udbRuleMaskData->udbArray[7],
                 udbRulePatternData->udbArray[8],
                 udbRuleMaskData->udbArray[8],
                 udbRulePatternData->udbArray[9],
                 udbRuleMaskData->udbArray[9],
                 udbRulePatternData->udbArray[10],
                 udbRuleMaskData->udbArray[10],
                 udbRulePatternData->udbArray[11],
                 udbRuleMaskData->udbArray[11],
                 udbRulePatternData->udbArray[12],
                 udbRuleMaskData->udbArray[12],
                 udbRulePatternData->udbArray[13],
                 udbRuleMaskData->udbArray[13],
                 udbRulePatternData->udbArray[14],
                 udbRuleMaskData->udbArray[14],
                 udbRulePatternData->udbArray[15],
                 udbRuleMaskData->udbArray[15],
                 udbRulePatternData->udbArray[16],
                 udbRuleMaskData->udbArray[16],
                 udbRulePatternData->udbArray[17],
                 udbRuleMaskData->udbArray[17],
                 udbRulePatternData->udbArray[18],
                 udbRuleMaskData->udbArray[18],
                 udbRulePatternData->udbArray[19],
                 udbRuleMaskData->udbArray[19],
                 udbRulePatternData->udbArray[20],
                 udbRuleMaskData->udbArray[20],
                 udbRulePatternData->udbArray[21],
                 udbRuleMaskData->udbArray[21],
                 udbRulePatternData->udbArray[22],
                 udbRuleMaskData->udbArray[22],
                 udbRulePatternData->udbArray[23],
                 udbRuleMaskData->udbArray[23],
                 udbRulePatternData->udbArray[24],
                 udbRuleMaskData->udbArray[24],
                 udbRulePatternData->udbArray[25],
                 udbRuleMaskData->udbArray[25],
                 udbRulePatternData->udbArray[26],
                 udbRuleMaskData->udbArray[26],
                 udbRulePatternData->udbArray[27],
                 udbRuleMaskData->udbArray[27],
                 udbRulePatternData->udbArray[28],
                 udbRuleMaskData->udbArray[28],
                 udbRulePatternData->udbArray[29],
                 udbRuleMaskData->udbArray[29]);

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChTtiMultiUdbRuleDelete
*
* DESCRIPTION:
*   The function delete configuration of Multiple number of TTI UDB Rules using the
*   Multiple Actions defined in wrCpssDxChTtiMultiRuleActionSet
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTtiMultiUdbRuleDelete
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result = GT_OK;
    GT_U8                           devNum;         /* device associated with the action     */
    GT_U32                          ruleIndex;      /* rule index associated with the action */
    GT_U32                          ruleIndexDelta; /* delta between 2 indexes               */
    GT_U32                          numOfRulesToDelete;
    GT_U32                          i=0;
    CPSS_DXCH_TTI_KEY_SIZE_ENT      udbRuleSize;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum =(GT_U8)inArgs[0];
    numOfRulesToDelete = (GT_U32)inArgs[1];
    udbRuleSize  = (CPSS_DXCH_TTI_KEY_SIZE_ENT)inArgs[2];
    ruleIndex = (GT_U32)inArgs[3];


    switch (udbRuleSize) /* rule size */
    {
        case CPSS_DXCH_TTI_KEY_SIZE_10_B_E:
            ruleIndexDelta = 1;
            break;

        case CPSS_DXCH_TTI_KEY_SIZE_20_B_E:
            ruleIndexDelta = 2;
            break;

        case CPSS_DXCH_TTI_KEY_SIZE_30_B_E:
            ruleIndexDelta = 3;
            break;

        default:
            galtisOutput(outArgs, GT_NOT_SUPPORTED, "udbRuleSize=%d, is not supported",udbRuleSize);
            return CMD_OK;
    }

    /* go over all range delete in from HW - the DB  is not deleted and will show the last added block */
    for (i=0;i < numOfRulesToDelete; i++)
    {
        /* call cpss api function - routerTtiTcamRow is an absolute index */
        result = pg_wrap_cpssDxChTtiRuleValidStatusSet(devNum, ruleIndex, GT_FALSE, GT_TRUE);
        if(result != GT_OK)
        {
            galtisOutput(outArgs, result, "numOfRulesDeleted=%d",i+1);
            return CMD_OK;
        }
        ruleIndex+=ruleIndexDelta;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChTtiMacToMeSet",
        &wrCpssDxChTtiMacToMeSet,
        1, 5},
    {"cpssDxChTtiMacToMeGetFirst",
        &wrCpssDxChTtiMacToMeGetFirst,
        1, 0},
    {"cpssDxChTtiMacToMeGetNext",
        &wrCpssDxChTtiMacToMeGetNext,
        1, 0},
    {"cpssDxChTtiMacToMe_1Set",
        &wrCpssDxChTtiMacToMe_1Set,
        3, 9},
    {"cpssDxChTtiMacToMe_1GetFirst",
        &wrCpssDxChTtiMacToMe_1GetFirst,
        3, 0},
    {"cpssDxChTtiMacToMe_1GetNext",
        &wrCpssDxChTtiMacToMe_1GetNext,
        3, 0},
    {"cpssDxChTtiPortLookupEnableSet",
        &wrCpssDxChTtiPortLookupEnableSet,
        4, 0},
    {"cpssDxChTtiPortLookupEnableGet",
        &wrCpssDxChTtiPortLookupEnableGet,
        3, 0},
    {"cpssDxChTtiPortIpv4OnlyTunneledEnableSet",
        &wrCpssDxChTtiPortIpv4OnlyTunneledEnableSet,
        3, 0},
    {"cpssDxChTtiPortIpv4OnlyTunneledEnableGet",
        &wrCpssDxChTtiPortIpv4OnlyTunneledEnableGet,
        2, 0},
    {"cpssDxChTtiPortIpv4OnlyMac2MeEnableSet",
        &wrCpssDxChTtiPortIpv4OnlyMac2MeEnableSet,
        3, 0},
    {"cpssDxChTtiPortIpv4OnlyMac2MeEnableGet",
        &wrCpssDxChTtiPortIpv4OnlyMac2MeEnableGet,
        2, 0},
    {"cpssDxChTtiIpv4McEnableSet",
        &wrCpssDxChTtiIpv4McEnableSet,
        2, 0},
    {"cpssDxChTtiIpv4McEnableGet",
        &wrCpssDxChTtiIpv4McEnableGet,
        1, 0},
    {"cpssDxChTtiPortMplsOnlyMac2MeEnableSet",
        &wrCpssDxChTtiPortMplsOnlyMac2MeEnableSet,
        3, 0},
    {"cpssDxChTtiPortMplsOnlyMac2MeEnableGet",
        &wrCpssDxChTtiPortMplsOnlyMac2MeEnableGet,
        2, 0},
    {"cpssDxChTtiPortMimOnlyMacToMeEnableSet",
        &wrCpssDxChTtiPortMimOnlyMacToMeEnableSet,
        3, 0},
    {"cpssDxChTtiPortMimOnlyMacToMeEnableGet",
        &wrCpssDxChTtiPortMimOnlyMacToMeEnableGet,
        2, 0},
    {"cpssDxChTtiRuleSetFirst",
        &wrCpssDxChTtiRuleSetFirst,
        2, 19},
    {"cpssDxChTtiRuleSetNext",
        &wrCpssDxChTtiRuleSetNext,
        2, 19},
    {"cpssDxChTtiRuleEndSet",
        &wrCpssDxChTtiRuleEndSet,
        0, 0},
    {"cpssDxChTtiRuleGetFirst",
        &wrCpssDxChTtiRuleGetFirst,
        1, 0},
    {"cpssDxChTtiRuleGetNext",
        &wrCpssDxChTtiRuleGet,
        1, 0},
    {"cpssDxChTtiRuleActionSet",
        &wrCpssDxChTtiRuleActionUpdate,
        0, 43},
    {"cpssDxChTtiRuleActionGetFirst",
        &wrCpssDxChTtiRuleActionGetFirst,
        0, 0},
    {"cpssDxChTtiRuleActionGetNext",
        &wrCpssDxChTtiRuleActionGet,
        0, 0},
    {"cpssDxChTtiRuleActionHwGetFirst",
        &wrCpssDxChTtiRuleActionHwGetFirst,
        4, 0},
    {"cpssDxChTtiRuleActionHwGetNext",
        &wrCpssDxChTtiRuleActionHwGet,
        4, 0},
    {"cpssDxChTtiRuleActionClear",
        &wrCpssDxChTtiRuleActionClear,
        0, 0},
    {"cpssDxChTtiRuleActionDelete",
        &wrCpssDxChTtiRuleActionDelete,
        0, 2},
    {"cpssDxChTtiRule_1SetFirst",
        &wrCpssDxChTtiRule_1SetFirst,
        4, 19},
    {"cpssDxChTtiRule_1SetNext",
        &wrCpssDxChTtiRule_1SetNext,
        4, 19},
    {"cpssDxChTtiRule_1EndSet",
        &wrCpssDxChTtiRule_1EndSet,
        0, 0},
    {"cpssDxChTtiRule_1GetFirst",
        &wrCpssDxChTtiRule_1GetFirst,
        4, 0},
    {"cpssDxChTtiRule_1GetNext",
        &wrCpssDxChTtiRule_1Get,
        4, 0},
    {"cpssDxChTtiRuleAction_1Set",
        &wrCpssDxChTtiRuleAction_1Update,
        1, 60},
    {"cpssDxChTtiRuleAction_1GetFirst",
        &wrCpssDxChTtiRuleAction_1GetFirst,
        1, 0},
    {"cpssDxChTtiRuleAction_1GetNext",
        &wrCpssDxChTtiRuleAction_1Get,
        1, 0},
    {"cpssDxChTtiRuleAction_1Clear",
        &wrCpssDxChTtiRuleActionClear,
        1, 0},
    {"cpssDxChTtiRuleAction_1Delete",
        &wrCpssDxChTtiRuleActionDelete,
        1, 2},

    {"cpssDxChTtiMacModeSet",
        &wrCpssDxChTtiMacModeSet,
        3, 0},
    {"cpssDxChTtiMacModeGet",
        &wrCpssDxChTtiMacModeGet,
        2, 0},
    {"cpssDxChTtiPclIdSet",
        &wrCpssDxChTtiPclIdSet,
        3, 0},
    {"cpssDxChTtiPclIdGet",
        &wrCpssDxChTtiPclIdGet,
        2, 0},
    {"cpssDxChTtiIpv4GreEthTypeSet",
        &wrCpssDxChTtiIpv4GreEthTypeSet,
        3, 0},
    {"cpssDxChTtiIpv4GreEthTypeGet",
        &wrCpssDxChTtiIpv4GreEthTypeGet,
        2, 0},
    {"cpssDxChTtiEthernetTypeSet",
        &wrCpssDxChTtiEthernetTypeSet,
        3, 0},
    {"cpssDxChTtiEthernetTypeGet",
        &wrCpssDxChTtiEthernetTypeGet,
        2, 0},
    {"cpssDxChTtiMimEthTypeSet",
        &wrCpssDxChTtiMimEthTypeSet,
        2, 0},
    {"cpssDxChTtiMimEthTypeGet",
        &wrCpssDxChTtiMimEthTypeGet,
        1, 0},
    {"cpssDxChTtiMplsEthTypeSet",
        &wrCpssDxChTtiMplsEthTypeSet,
        3, 0},
    {"cpssDxChTtiMplsEthTypeGet",
        &wrCpssDxChTtiMplsEthTypeGet,
        2, 0},
    {"cpssDxChTtiExceptionCmdSet",
        &wrCpssDxChTtiExceptionCmdSet,
        3, 0},
    {"cpssDxChTtiExceptionCmdGet",
        &wrCpssDxChTtiExceptionCmdGet,
        2, 0},
    {"cpssDxChTtiExceptionCmdSet_1",
        &wrCpssDxChTtiExceptionCmdSet,
        3, 0},
    {"cpssDxChTtiExceptionCmdGet_1",
        &wrCpssDxChTtiExceptionCmdGet,
        2, 0},

    {"cpssDxChTtiRuleValidStatusSet",
        &wrCpssDxChTtiRuleValidStatusSet,
        3, 0},
    {"cpssDxChTtiRuleValidStatus_2Set",
        &wrCpssDxChTtiRuleValidStatus_2Set,
        3, 0},
    {"cpssDxChTtiRuleValidStatusGet",
        &wrCpssDxChTtiRuleValidStatusGet,
        2, 0},
    {"cpssDxChTtiRuleValidStatus_2Get",
        &wrCpssDxChTtiRuleValidStatus_2Get,
        2, 0},

    {"cpssDxChTtiRule_2SetFirst",
        &wrCpssDxChTtiRule_2SetFirst,
        4, 20},
    {"cpssDxChTtiRule_2SetNext",
        &wrCpssDxChTtiRule_2SetNext,
        4, 20},
    {"cpssDxChTtiRule_2EndSet",
        &wrCpssDxChTtiRule_2EndSet,
        0, 0},
    {"cpssDxChTtiRule_2CancelSet",
        &wrCpssDxChTtiRule_2CancelSet,
        0, 0},
    {"cpssDxChTtiRule_2GetFirst",
        &wrCpssDxChTtiRule_2GetFirst,
        4, 0},
    {"cpssDxChTtiRule_2GetNext",
        &wrCpssDxChTtiRule_2Get,
        4, 0},

    {"cpssDxChTtiRule_4SetFirst",
        &wrCpssDxChTtiRule_4SetFirst,
        4, 36},
    {"cpssDxChTtiRule_4SetNext",
        &wrCpssDxChTtiRule_4SetNext,
        4, 36},
    {"cpssDxChTtiRule_4EndSet",
        &wrCpssDxChTtiRule_2EndSet,
        0, 0},
    {"cpssDxChTtiRule_4CancelSet",
        &wrCpssDxChTtiRule_2CancelSet,
        0, 0},
    {"cpssDxChTtiRule_4GetFirst",
        &wrCpssDxChTtiRule_4GetFirst,
        4, 0},
    {"cpssDxChTtiRule_4GetNext",
        &wrCpssDxChTtiRule_4Get,
        4, 0},

    {"cpssDxChTtiRule_5SetFirst",
        &wrCpssDxChTtiRule_5SetFirst,
        4, 36},
    {"cpssDxChTtiRule_5SetNext",
        &wrCpssDxChTtiRule_5SetNext,
        4, 36},
    {"cpssDxChTtiRule_5EndSet",
        &wrCpssDxChTtiRule_2EndSet,
        0, 0},
    {"cpssDxChTtiRule_5CancelSet",
        &wrCpssDxChTtiRule_2CancelSet,
        0, 0},

    {"cpssDxChTtiRule_5GetFirst",
        &wrCpssDxChTtiRule_5GetFirst,
        4, 0},
    {"cpssDxChTtiRule_5GetNext",
        &wrCpssDxChTtiRule_5Get,
        4, 0},

    {"cpssDxChTtiRuleAction_2Set",
        &wrCpssDxChTtiRuleAction_2Update,
        1, 65},
    {"cpssDxChTtiRuleAction_2GetFirst",
        &wrCpssDxChTtiRuleAction_2GetFirst,
        1, 0},
    {"cpssDxChTtiRuleAction_2GetNext",
        &wrCpssDxChTtiRuleAction_2Get,
        1, 0},
    {"cpssDxChTtiRuleAction_2Clear",
        &wrCpssDxChTtiRuleActionClear,
        1, 0},
    {"cpssDxChTtiRuleAction_2Delete",
        &wrCpssDxChTtiRuleActionDelete,
        1, 2},

    {"cpssDxChTtiRuleAction_4Set",
        &wrCpssDxChTtiRuleAction_2Update,
        1, 65},
    {"cpssDxChTtiRuleAction_4GetFirst",
        &wrCpssDxChTtiRuleAction_2GetFirst,
        1, 0},
    {"cpssDxChTtiRuleAction_4GetNext",
        &wrCpssDxChTtiRuleAction_2Get,
        1, 0},
    {"cpssDxChTtiRuleAction_4Clear",
        &wrCpssDxChTtiRuleActionClear,
        1, 0},
    {"cpssDxChTtiRuleAction_4Delete",
        &wrCpssDxChTtiRuleActionDelete,
        1, 2},

    {"cpssDxChTtiRuleAction_5Set",
        &wrCpssDxChTtiRuleAction_5Update,
        1, 95},
    {"cpssDxChTtiRuleAction_5GetFirst",
        &wrCpssDxChTtiRuleAction_5GetFirst,
        1, 0},
    {"cpssDxChTtiRuleAction_5GetNext",
        &wrCpssDxChTtiRuleAction_5Get,
        1, 0},
    {"cpssDxChTtiRuleAction_5Clear",
        &wrCpssDxChTtiRuleActionClear,
        1, 0},
    {"cpssDxChTtiRuleAction_5Delete",
        &wrCpssDxChTtiRuleActionDelete,
        1, 2},

    {"cpssDxChTtiRule_6SetFirst",
        &wrCpssDxChTtiRule_6SetFirst,
        4, 20},
    {"cpssDxChTtiRule_6SetNext",
        &wrCpssDxChTtiRule_6SetNext,
        4, 20},
    {"cpssDxChTtiRule_6EndSet",
        &wrCpssDxChTtiRule_2EndSet,
        0, 0},
    {"cpssDxChTtiRule_6CancelSet",
        &wrCpssDxChTtiRule_2CancelSet,
        0, 0},
    {"cpssDxChTtiRule_6GetFirst",
        &wrCpssDxChTtiRule_6GetFirst,
        4, 0},
    {"cpssDxChTtiRule_6GetNext",
        &wrCpssDxChTtiRule_6Get,
        4, 0},

    {"cpssDxChTtiRuleAction_6Set",
        &wrCpssDxChTtiRuleAction_6Update,
        1, 95},
    {"cpssDxChTtiRuleAction_6GetFirst",
        &wrCpssDxChTtiRuleAction_6GetFirst,
        1, 0},
    {"cpssDxChTtiRuleAction_6GetNext",
        &wrCpssDxChTtiRuleAction_6Get,
        1, 0},
    {"cpssDxChTtiRuleAction_6Clear",
        &wrCpssDxChTtiRuleActionClear,
        1, 0},
    {"cpssDxChTtiRuleAction_6Delete",
        &wrCpssDxChTtiRuleActionDelete,
        1, 2},

    {"cpssDxChTtiBypassHeaderLenCheckIpv4TtiExpEnableSet",
        &wrCpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableSet,
        2, 0},
    {"cpssDxChTtiBypassHeaderLenCheckIpv4TtiExpEnableGet",
        &wrCpssDxChTtiBypassHeaderLengthCheckInIpv4TtiHeaderExceptionEnableGet,
        1, 0},

    {"cpssDxChTtiPwCwSequencingSupportEnableSet",
        &wrCpssDxChTtiPwCwSequencingSupportEnableSet,
        2, 0},
    {"cpssDxChTtiPwCwSequencingSupportEnableGet",
        &wrCpssDxChTtiPwCwSequencingSupportEnableGet,
        1, 0},

    {"cpssDxChTtiUserDefinedByteSet",
        &wrCpssDxChTtiUserDefinedByteSet,
        5, 0},
    {"cpssDxChTtiUserDefinedByteGet",
        &wrCpssDxChTtiUserDefinedByteGet,
        3, 0},

    {"cpssDxChTtiPacketTypeKeySizeSet",
        &wrCpssDxChTtiPacketTypeKeySizeSet,
        3, 0},
    {"cpssDxChTtiPacketTypeKeySizeGet",
        &wrCpssDxChTtiPacketTypeKeySizeGet,
        2, 0},

    {"cpssDxChTtiExceptionCpuCodeSet1",
        &wrCpssDxChTtiExceptionCpuCodeSet,
        3, 0},
    {"cpssDxChTtiExceptionCpuCodeGet1",
        &wrCpssDxChTtiExceptionCpuCodeGet,
        2, 0},
    {"cpssDxChTtiTcamSegmentModeSet",
        &wrCpssDxChTtiParallelLookupEnableSet,
        3, 0},
    {"cpssDxChTtiTcamSegmentModeGet",
        &wrCpssDxChTtiParallelLookupEnableGet,
        2, 0},
    {"cpssDxChTtiTcamSegmentMode_1Set",
        &wrCpssDxChTtiTcamSegmentModeSet,
        3, 0},
    {"cpssDxChTtiTcamSegmentMode_1Get",
        &wrCpssDxChTtiTcamSegmentModeGet,
        2, 0},
    {"cpssDxChTtiMcTunnelDuplicationModeSet",
        &wrCpssDxChTtiMcTunnelDuplicationModeSet,
        4, 0},
    {"cpssDxChTtiMcTunnelDuplicationModeGet",
        &wrCpssDxChTtiMcTunnelDuplicationModeGet,
        3, 0},
    {"cpssDxChTtiMcTunnelDuplicationUdpDestPortSet",
        &wrCpssDxChTtiMcTunnelDuplicationUdpDestPortSet,
        2, 0},
    {"cpssDxChTtiMcTunnelDuplicationUdpDestPortGet",
        &wrCpssDxChTtiMcTunnelDuplicationUdpDestPortGet,
        1, 0},
    {"cpssDxChTtiMplsMcTunnelTriggeringMacDaSet",
        &wrCpssDxChTtiMplsMcTunnelTriggeringMacDaSet,
        3, 0},
    {"cpssDxChTtiMplsMcTunnelTriggeringMacDaGet",
        &wrCpssDxChTtiMplsMcTunnelTriggeringMacDaGet,
        1, 0},
    {"cpssDxChTtiPwCwExceptionCmdSet",
        &wrCpssDxChTtiPwCwExceptionCmdSet,
        3, 0},
    {"cpssDxChTtiPwCwExceptionCmdGet",
        &wrCpssDxChTtiPwCwExceptionCmdGet,
        2, 0},
    {"cpssDxChTtiPwCwCpuCodeBaseSet",
        &wrCpssDxChTtiPwCwCpuCodeBaseSet,
        2, 0},
    {"cpssDxChTtiPwCwCpuCodeBaseGet",
        &wrCpssDxChTtiPwCwCpuCodeBaseGet,
        1, 0},
    {"cpssDxChTtiIPv6ExtensionHeaderSet",
        &wrCpssDxChTtiIPv6ExtensionHeaderSet,
        3, 0},
    {"cpssDxChTtiIPv6ExtensionHeaderGet",
        &wrCpssDxChTtiIPv6ExtensionHeaderGet,
        2, 0},
    {"cpssDxChTtiSourceIdBitsOverrideSet",
        &wrCpssDxChTtiSourceIdBitsOverrideSet,
        3, 0},
    {"cpssDxChTtiSourceIdBitsOverrideGet",
        &wrCpssDxChTtiSourceIdBitsOverrideGet,
        2, 0},
    {"cpssDxChTtiGreExtensionsEnableSet",
        &wrCpssDxChTtiGreExtensionsEnableSet,
        2, 0},
    {"cpssDxChTtiGreExtensionsEnableGet",
        &wrCpssDxChTtiGreExtensionsEnableGet,
        1, 0},
    {"appDemoDxChLion3TcamTtiBaseIndexGet",
        &wrCpssDxChTtiTcamBaseIndexGet,
        2, 0},
    {"appDemoDxChLion3TcamTtiNumOfIndexsGet",
        &wrCpssDxChTtiTcamNumOfIndexsGet,
        2, 0},
    {"cpssDxChTtiPortIpTotalLengthDeductionEnableSet",
        &wrCpssDxChTtiPortIpTotalLengthDeductionEnableSet,
        3, 0},
    {"cpssDxChTtiPortIpTotalLengthDeductionEnableGet",
        &wrCpssDxChTtiPortIpTotalLengthDeductionEnableGet,
        2, 0},
    {"cpssDxChTtiIpTotalLengthDeductionValueSet",
        &wrCpssDxChTtiIpTotalLengthDeductionValueSet,
        3, 0},
    {"cpssDxChTtiIpTotalLengthDeductionValueGet",
        &wrCpssDxChTtiIpTotalLengthDeductionValueGet,
        2, 0},
    {"cpssDxChTtiMultiRuleActionSet",
        &wrCpssDxChTtiMultiRuleActionSet,
        94, 0},
    {"cpssDxChTtiMultiRuleActionGet",
        &wrCpssDxChTtiMultiRuleActionGet,
        1, 0},
    {"cpssDxChTtiMultiEthRuleSet",
        &wrCpssDxChTtiMultiEthRuleSet,
        44, 0},
    {"cpssDxChTtiMultiEthRuleGet",
        &wrCpssDxChTtiMultiEthRuleGet,
        0, 0},
    {"cpssDxChTtiMultiEthRuleDelete",
        &wrCpssDxChTtiMultiEthRuleDelete,
        3, 0},
    {"cpssDxChTtiMultiUdbRuleSet",
        &wrCpssDxChTtiMultiUdbRuleSet,
        65, 0},
    {"cpssDxChTtiMultiUdbRuleGet",
        &wrCpssDxChTtiMultiUdbRuleGet,
        1, 0},
    {"cpssDxChTtiMultiUdbRuleDelete",
        &wrCpssDxChTtiMultiUdbRuleDelete,
        4, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibResetCpssExMxPmTti
*
* DESCRIPTION:
*     Library database reset function.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     none
*
* COMMENTS:
*     none
*
*******************************************************************************/
static GT_VOID cmdLibResetCpssDxChTti
(
    GT_VOID
)
{

    firstRun = GT_TRUE;


    mask_set = GT_FALSE;    /* is mask set   */
    pattern_set = GT_FALSE; /* is pattern set*/
    mask_ruleIndex = 0;
    pattern_ruleIndex = 0;

}

/*******************************************************************************
* cmdLibInitCpssDxChTti
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
GT_STATUS cmdLibInitCpssDxChTti
(
GT_VOID
)
{
    GT_STATUS rc;

    rc = wrCpssRegisterResetCb(cmdLibResetCpssDxChTti);
    if(rc != GT_OK)
    {
        return rc;
    }

    return cmdInitLibrary(dbCommands, numCommands);
}

