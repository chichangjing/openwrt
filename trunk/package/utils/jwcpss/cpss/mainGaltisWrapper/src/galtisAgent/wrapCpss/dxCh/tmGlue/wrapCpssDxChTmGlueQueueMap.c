/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChTmGlueQueueMap.c
*
* DESCRIPTION:
*       Traffic Manager Queue Mapping.
*
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/tmGlue/cpssDxChTmGlueQueueMap.h>

/*******************************************************************************
* cpssDxChTmGlueQueueMapBypassEnableSet
*
* DESCRIPTION:
*   The function sets enable/disable state of Bypass TM Queue Mapping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       bypass             - GT_TRUE - bypass, GT_FALSE - mapping.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapBypassEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   bypass;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    bypass = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTmGlueQueueMapBypassEnableSet(
        devNum, bypass);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapBypassEnableGet
*
* DESCRIPTION:
*   The function gets enable/disable state of Bypass TM Queue Mapping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       bypassPtr          - (pointer to)GT_TRUE - bypass, GT_FALSE - mapping.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapBypassEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   bypass;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTmGlueQueueMapBypassEnableGet(
        devNum, &bypass);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", bypass);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapBitSelectTableEntrySet
*
* DESCRIPTION:
*   The function sets the Queue Id Map Bit Select Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       entryIndex     - entry index
*                        (APPLICABLE RANGES: 0..255).
*       entryPtr       - (pointer to) entry contents
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapBitSelectTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                          rc;
    GT_U8                                              devNum;
    GT_U32                                             entryIndex;
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC   entry;
    GT_U32                                             i;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&entry, 0, sizeof(entry));

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];
	entryIndex         = (GT_U32)inFields[0];
	entry.queueIdBase  = (GT_U32)inFields[1];
    for (i = 0; (i < 14); i++)
    {
        entry.bitSelectArr[i].selectType  =
            (CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_TYPE_ENT)inFields[2 + (2 * i)];
        entry.bitSelectArr[i].bitSelector = (GT_U32)inFields[3 + (2 * i)];
    }

	/* call cpss api function */
    rc = cpssDxChTmGlueQueueMapBitSelectTableEntrySet(
		devNum, entryIndex, &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapBitSelectTableEntryGet
*
* DESCRIPTION:
*   The function gets the Queue Id Map Bit Select Table Entry.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       entryIndex     - entry index
*                        (APPLICABLE RANGES: 0..255).
*
* OUTPUTS:
*       entryPtr       - (pointer to) entry contents
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on unexpcted HW value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapBitSelectTableGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
	IN  GT_BOOL    isGetFirst
)
{
    GT_STATUS                                          rc;
    GT_U8                                              devNum;
    GT_U32                                             entryIndex;
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_BIT_SELECT_ENTRY_STC   entry;
    GT_U32                                             i;
    static GT_U32                                      startEntryIndex = 0;
    static GT_U32                                      numOfEntriesRequred = 0;
    static GT_U32                                      numOfEntriesDone = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];

    if (isGetFirst != GT_FALSE)
    {
        startEntryIndex       = (GT_U32)inArgs[1];
        numOfEntriesRequred   = (GT_U32)inArgs[2];
        numOfEntriesDone      = 0;
    }

    if (numOfEntriesDone >= numOfEntriesRequred)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

	entryIndex = (startEntryIndex + numOfEntriesDone);
    /* increment for next call */
    numOfEntriesDone ++;

    /* call cpss api function */
    rc = cpssDxChTmGlueQueueMapBitSelectTableEntryGet(
        devNum, entryIndex, &entry);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChTmGlueQueueMapBitSelectTableEntryGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d", entryIndex, entry.queueIdBase);
    for (i = 0; (i < 14); i++)
    {
        fieldOutputSetAppendMode();
        fieldOutput(
            "%d%d", entry.bitSelectArr[i].selectType,
            entry.bitSelectArr[i].bitSelector);
    }
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChTmGlueQueueMapBitSelectTableGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChTmGlueQueueMapBitSelectTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE /*isGetFirst*/);

}

static CMD_STATUS wrCpssDxChTmGlueQueueMapBitSelectTableGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChTmGlueQueueMapBitSelectTableGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE /*isGetFirst*/);

}

/*******************************************************************************
* cpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet
*
* DESCRIPTION:
*   The function sets the Queue Id Map Bit Select Table Entry index
*   mapped to the given target device.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       targetDevId    - target device Id
*                        (APPLICABLE RANGES: 0..1023).
*       entryIndex     - index of Queue Id Map Bit Select Table Entry
*                        being mapped to the given target device.
*                        (APPLICABLE RANGES: 0..255).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_HW_DEV_NUM   targetHwDevId;
    GT_U32          entryIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    targetHwDevId = (GT_HW_DEV_NUM)inArgs[1];
    entryIndex  = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet(
        devNum, targetHwDevId, entryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet
*
* DESCRIPTION:
*   The function gets the Queue Id Map Bit Select Table Entry index
*   mapped to the given target device.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       targetDevId    - target device Id
*                        (APPLICABLE RANGES: 0..1023).
*
* OUTPUTS:
*       entryIndexPtr  - (pointer to)index of Queue Id Map Bit Select Table Entry
*                        being mapped to the given target device.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_HW_DEV_NUM   targetHwDevId;
    GT_U32          entryIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    targetHwDevId = (GT_HW_DEV_NUM)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet(
        devNum, targetHwDevId, &entryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", entryIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapCpuCodeToTcMapSet
*
* DESCRIPTION:
*   The function sets the mapped TM Traffic Class for CPU Code.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       cpuCode        - CPU code
*                        CPSS_NET_ALL_CPU_OPCODES_E - to configure all table entries.
*       tmTc           - mapped TM Traffic Class.
*                        (APPLICABLE RANGES: 0..15).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapCpuCodeToTcMapSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                  result;
    GT_U8                      devNum;
    CPSS_NET_RX_CPU_CODE_ENT   cpuCode;
    GT_U32                     tmTc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[1];
    tmTc    = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTmGlueQueueMapCpuCodeToTcMapSet(
        devNum, cpuCode, tmTc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapCpuCodeToTcMapGet
*
* DESCRIPTION:
*   The function gets the mapped TM Traffic Class for CPU Code.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       cpuCode        - CPU code
*
* OUTPUTS:
*       tmTcPtr        - (pointer to) mapped TM Traffic Class.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapCpuCodeToTcMapGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                  result;
    GT_U8                      devNum;
    CPSS_NET_RX_CPU_CODE_ENT   cpuCode;
    GT_U32                     tmTc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTmGlueQueueMapCpuCodeToTcMapGet(
        devNum, cpuCode, &tmTc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", tmTc);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapTcToTcMapSet
*
* DESCRIPTION:
*   The function sets mapping of Traffic Class To TM Traffic Class.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       packetDsaCommand  - packet command.
*                           Forward, From CPU and Mirror only valid
*       isUnicast         - GT_TRUE - unicast, GT_FALSE - multicast
*       tc                - Traffic Class
*                           (APPLICABLE RANGES: 0..7).
*       tmTc              - Traffic Manager Traffic Class
*                           (APPLICABLE RANGES: 0..15).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapTcToTcMapSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   rc;
    GT_U8                       devNum;
    CPSS_DXCH_NET_DSA_CMD_ENT   packetDsaCommand;
    GT_BOOL                     isUnicast;
    GT_U32                      tc;
    GT_U32                      tmTc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];
    packetDsaCommand   = (CPSS_DXCH_NET_DSA_CMD_ENT)inArgs[1];
    isUnicast          = (GT_BOOL)inArgs[2];
	tc                 = (GT_U32)inFields[0];
	tmTc               = (GT_U32)inFields[1];

	/* call cpss api function */
    rc = cpssDxChTmGlueQueueMapTcToTcMapSet(
		devNum, packetDsaCommand, isUnicast, tc, tmTc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapTcToTcMapGet
*
* DESCRIPTION:
*   The function gets mapping of Traffic Class To TM Traffic Class.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number
*       packetDsaCommand  - packet command.
*                           Forward, From CPU and Mirror only valid
*       isUnicast         - GT_TRUE - unicast, GT_FALSE - multicast
*       tc                - Traffic Class
*                           (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       tmTcPtr        - (pointer to)Traffic Manager Traffic Class
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapTcToTcMapGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
	IN  GT_BOOL    isGetFirst
)
{
    GT_STATUS                   rc;
    GT_U8                       devNum;
    CPSS_DXCH_NET_DSA_CMD_ENT   packetDsaCommand;
    GT_BOOL                     isUnicast;
    GT_U32                      tc;
    GT_U32                      tmTc;
    static GT_U32               numOfEntriesDone = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];
    packetDsaCommand   = (CPSS_DXCH_NET_DSA_CMD_ENT)inArgs[1];
    isUnicast          = (GT_BOOL)inArgs[2];

    if (isGetFirst != GT_FALSE)
    {
        numOfEntriesDone = 0;
    }

    if (numOfEntriesDone >= 8)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

	tc = numOfEntriesDone;
    /* increment for next call */
    numOfEntriesDone ++;

	/* call cpss api function */
    rc = cpssDxChTmGlueQueueMapTcToTcMapGet(
		devNum, packetDsaCommand, isUnicast, tc, &tmTc);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChTmGlueQueueMapTcToTcMapGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d", tc, tmTc);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChTmGlueQueueMapTcToTcMapGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChTmGlueQueueMapTcToTcMapGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE /*isGetFirst*/);

}

static CMD_STATUS wrCpssDxChTmGlueQueueMapTcToTcMapGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChTmGlueQueueMapTcToTcMapGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE /*isGetFirst*/);

}

/*******************************************************************************
* cpssDxChTmGlueQueueMapEgressPolicerForceSet
*
* DESCRIPTION:
*   The function sets force state for TM queue Id based policing.
*   If force the previous policer settings of the packet overridden.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       queueId        - Traffic Manager Queue Id
*                        (APPLICABLE RANGES: 0..0x3FFF).
*       forceMeter     - force meter,
*                        GT_TRUE - force, GT_FALSE - don't force.
*       forceCounting  - force counting
*                        GT_TRUE - force, GT_FALSE - don't force.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapEgressPolicerForceSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  rc;
    GT_U8      devNum;
    GT_U32     queueId;
    GT_BOOL    forceMeter;
    GT_BOOL    forceCounting;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum        = (GT_U8)inArgs[0];
	queueId       = (GT_U32)inFields[0];
	forceMeter    = (GT_BOOL)inFields[1];
	forceCounting = (GT_BOOL)inFields[2];

	/* call cpss api function */
    rc = cpssDxChTmGlueQueueMapEgressPolicerForceSet(
		devNum, queueId, forceMeter, forceCounting);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapEgressPolicerForceGet
*
* DESCRIPTION:
*   The function gets force state for TM queue Id based policing.
*   If force the previous policer settings of the packet overridden.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       queueId        - Traffic Manager Queue Id
*                        (APPLICABLE RANGES: 0..0x3FFF).
*
* OUTPUTS:
*       forceMeterPtr     - (pointer to)force meter
*                        GT_TRUE - force, GT_FALSE - don't force.
*       forceCountingPtr  - (pointer to)force counting
*                        GT_TRUE - force, GT_FALSE - don't force.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapEgressPolicerForceGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
	IN  GT_BOOL    isGetFirst
)
{
    GT_STATUS      rc;
    GT_U8          devNum;
    GT_U32         queueId;
    GT_BOOL        forceMeter;
    GT_BOOL        forceCounting;
    static GT_U32  startQueueId = 0;
    static GT_U32  numOfEntriesReqired = 0;
    static GT_U32  numOfEntriesDone = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum        = (GT_U8)inArgs[0];

    if (isGetFirst != GT_FALSE)
    {
        startQueueId        = (GT_U32)inArgs[1];
        numOfEntriesReqired = (GT_U32)inArgs[2];
        numOfEntriesDone    = 0;
    }

    if (numOfEntriesDone >= numOfEntriesReqired)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

	queueId = (startQueueId + numOfEntriesDone);
    /* increment for next call */
    numOfEntriesDone ++;

	/* call cpss api function */
    rc = cpssDxChTmGlueQueueMapEgressPolicerForceGet(
		devNum, queueId, &forceMeter, &forceCounting);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChTmGlueQueueMapEgressPolicerForceGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d", queueId, forceMeter, forceCounting);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChTmGlueQueueMapEgressPolicerForceGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChTmGlueQueueMapEgressPolicerForceGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE /*isGetFirst*/);

}

static CMD_STATUS wrCpssDxChTmGlueQueueMapEgressPolicerForceGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChTmGlueQueueMapEgressPolicerForceGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE /*isGetFirst*/);

}

/*******************************************************************************
* cpssDxChTmGlueQueueMapToCpuModeSet
*
* DESCRIPTION:
*   The function sets mode of index’s calculation in Queue Id Bit Select Table
*   for TO CPU packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*       toCpuMode          - mode of index’s calculation in
*                            Queue Id Bit Select Table for TO CPU packets.
*       toCpuSelectorIndex - index in Queue Id Bit Select Table
*                            for TO CPU packets.
*                            Always stored to HW, but used only when
*                            toCpuMode is "Use TO_CPU Queue Bit Selector configuration".
*                            (APPLICABLE RANGES: 0..0xFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapToCpuModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                     result;
    GT_U8                                         devNum;
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT   toCpuMode;
    GT_U32                                        toCpuSelectorIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];
    toCpuMode          = (CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT)inArgs[1];
    toCpuSelectorIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChTmGlueQueueMapToCpuModeSet(
        devNum, toCpuMode, toCpuSelectorIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapToCpuModeGet
*
* DESCRIPTION:
*   The function gets mode of index’s calculation in Queue Id Bit Select Table
*   for TO CPU packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number
*
* OUTPUTS:
*       toCpuModePtr          - (pointer to)mode of index’s calculation in
*                               Queue Id Bit Select Table for TO CPU packets.
*       toCpuSelectorIndexPtr - (pointer to)index in Queue Id Bit Select Table
*                               for TO CPU packets.
*                               Always retrieved from HW, but used only when
*                               toCpuMode is "Use TO_CPU Queue Bit Selector configuration".
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapToCpuModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                     result;
    GT_U8                                         devNum;
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_TO_CPU_MODE_ENT   toCpuMode;
    GT_U32                                        toCpuSelectorIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum             = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTmGlueQueueMapToCpuModeGet(
        devNum, &toCpuMode, &toCpuSelectorIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", toCpuMode, toCpuSelectorIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapMcModeSet
*
* DESCRIPTION:
*   The function sets mode of index’s calculation in Queue Id Bit Select Table
*   for Multicast packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       mcMode          - mode of index’s calculation in
*                         Queue Id Bit Select Table for Multicast packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapMcModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                 result;
    GT_U8                                     devNum;
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT   mcMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mcMode = (CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTmGlueQueueMapMcModeSet(
        devNum, mcMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapMcModeGet
*
* DESCRIPTION:
*   The function gets mode of index’s calculation in Queue Id Bit Select Table
*   for Multicast packets.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*
* OUTPUTS:
*       mcMode          - (pointer to)mode of index’s calculation in
*                         Queue Id Bit Select Table for Multicast packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapMcModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                 result;
    GT_U8                                     devNum;
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_MC_MODE_ENT   mcMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTmGlueQueueMapMcModeGet(
        devNum, &mcMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mcMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapSelectorTableAccessModeSet
*
* DESCRIPTION:
*   The function sets Bit Selector Table Access Mode.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum  - device number
*       mode    - Bit Selector Table Access Mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapSelectorTableAccessModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                             result;
    GT_U8                                                 devNum;
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT  mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode   = (CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChTmGlueQueueMapSelectorTableAccessModeSet(
        devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapSelectorTableAccessModeGet
*
* DESCRIPTION:
*   The function gets Bit Selector Table Access Mode.
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
*       modePtr - (pointer to)Bit Selector Table Access Mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapSelectorTableAccessModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                             result;
    GT_U8                                                 devNum;
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_SELECTOR_ACCESS_MODE_ENT  mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTmGlueQueueMapSelectorTableAccessModeGet(
        devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapL1PacketLengthOffsetSet
*
* DESCRIPTION:
*   The function sets Ingress L1 Packet Length Offset Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum         - device number
*       physicalPort   - physical port number.
*       lengthOssetPtr - (pointer to)Ingress L1 Packet Length Offset Configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_OUT_OF_RANGE          - on out-of-range parameters
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapL1PacketLengthOffsetSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                                rc;
    GT_U8                                                    devNum;
    GT_PHYSICAL_PORT_NUM                                     physicalPort;
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC  lengthOffset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&lengthOffset, 0, sizeof(lengthOffset));

    /* map input arguments to locals */
    devNum                   = (GT_U8)inArgs[0];
	physicalPort             = (GT_PHYSICAL_PORT_NUM)inFields[0];
	lengthOffset.offsetValue = (GT_U32)inFields[1];
	lengthOffset.offsetCmd   =
        (CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_CMD_ENT)inFields[2];

	/* call cpss api function */
    rc = cpssDxChTmGlueQueueMapL1PacketLengthOffsetSet(
		devNum, physicalPort, &lengthOffset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTmGlueQueueMapL1PacketLengthOffsetGet
*
* DESCRIPTION:
*   The function gets Ingress L1 Packet Length Offset Configuration.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number
*       physicalPort    - physical port number.
*       lengthOffsetPtr - (pointer to)Ingress L1 Packet Length Offset Configuration.
*
* OUTPUTS:
*       lengthOffsetPtr - (pointer to)Ingress L1 Packet Length Offset Configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTmGlueQueueMapL1PacketLengthOffsetGet_util
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
	IN  GT_BOOL    isGetFirst
)
{
    GT_STATUS                                                rc;
    GT_U8                                                    devNum;
    GT_PHYSICAL_PORT_NUM                                     physicalPort;
    CPSS_DXCH_TM_GLUE_QUEUE_MAP_L1_PACKET_LENGTH_OFFSET_STC  lengthOffset;
    static GT_U32                                            startPhyPortNum = 0;
    static GT_U32                                            numOfEntriesReqired = 0;
    static GT_U32                                            numOfEntriesDone = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum        = (GT_U8)inArgs[0];

    if (isGetFirst != GT_FALSE)
    {
        startPhyPortNum     = (GT_U32)inArgs[1];
        numOfEntriesReqired = (GT_U32)inArgs[2];
        numOfEntriesDone    = 0;
    }

    if (numOfEntriesDone >= numOfEntriesReqired)
    {
        /* no entries more */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

	physicalPort = (startPhyPortNum + numOfEntriesDone);
    /* increment for next call */
    numOfEntriesDone ++;

	/* call cpss api function */
    rc = cpssDxChTmGlueQueueMapL1PacketLengthOffsetGet(
		devNum, physicalPort, &lengthOffset);
    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "cpssDxChTmGlueQueueMapL1PacketLengthOffsetGet");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput(
        "%d%d%d", physicalPort, lengthOffset.offsetValue, lengthOffset.offsetCmd);
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChTmGlueQueueMapL1PacketLengthOffsetGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChTmGlueQueueMapL1PacketLengthOffsetGet_util(
        inArgs, inFields, numFields, outArgs, GT_TRUE /*isGetFirst*/);

}

static CMD_STATUS wrCpssDxChTmGlueQueueMapL1PacketLengthOffsetGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChTmGlueQueueMapL1PacketLengthOffsetGet_util(
        inArgs, inFields, numFields, outArgs, GT_FALSE /*isGetFirst*/);

}

/****************************** database initialization ******************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssDxChTmGlueQueueMapBypassEnableSet",
         &wrCpssDxChTmGlueQueueMapBypassEnableSet,
         2, 0},
        {"cpssDxChTmGlueQueueMapBypassEnableGet",
         &wrCpssDxChTmGlueQueueMapBypassEnableGet,
         1, 0},
        {"cpssDxChTmGlueQueueMapBitSelectTableSet",
         &wrCpssDxChTmGlueQueueMapBitSelectTableSet,
         3, 31},
        {"cpssDxChTmGlueQueueMapBitSelectTableGetFirst",
         &wrCpssDxChTmGlueQueueMapBitSelectTableGetFirst,
         3, 0},
        {"cpssDxChTmGlueQueueMapBitSelectTableGetNext",
         &wrCpssDxChTmGlueQueueMapBitSelectTableGetNext,
         3, 0},
        {"cpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet",
         &wrCpssDxChTmGlueQueueMapTargetDeviceToIndexMapSet,
         3, 0},
        {"cpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet",
         &wrCpssDxChTmGlueQueueMapTargetDeviceToIndexMapGet,
         2, 0},
        {"cpssDxChTmGlueQueueMapCpuCodeToTcMapSet",
         &wrCpssDxChTmGlueQueueMapCpuCodeToTcMapSet,
         3, 0},
        {"cpssDxChTmGlueQueueMapCpuCodeToTcMapGet",
         &wrCpssDxChTmGlueQueueMapCpuCodeToTcMapGet,
         2, 0},
        {"cpssDxChTmGlueQueueMapTcToTcMapTableSet",
         &wrCpssDxChTmGlueQueueMapTcToTcMapSet,
         3, 2},
        {"cpssDxChTmGlueQueueMapTcToTcMapTableGetFirst",
         &wrCpssDxChTmGlueQueueMapTcToTcMapGetFirst,
         3, 0},
        {"cpssDxChTmGlueQueueMapTcToTcMapTableGetNext",
         &wrCpssDxChTmGlueQueueMapTcToTcMapGetNext,
         3, 0},
        {"cpssDxChTmGlueQueueMapEgressPolicerForceSet",
         &wrCpssDxChTmGlueQueueMapEgressPolicerForceSet,
         3, 3},
        {"cpssDxChTmGlueQueueMapEgressPolicerForceGetFirst",
         &wrCpssDxChTmGlueQueueMapEgressPolicerForceGetFirst,
         3, 0},
        {"cpssDxChTmGlueQueueMapEgressPolicerForceGetNext",
         &wrCpssDxChTmGlueQueueMapEgressPolicerForceGetNext,
         3, 0},
        {"cpssDxChTmGlueQueueMapToCpuModeSet",
         &wrCpssDxChTmGlueQueueMapToCpuModeSet,
         3, 0},
        {"cpssDxChTmGlueQueueMapToCpuModeGet",
         &wrCpssDxChTmGlueQueueMapToCpuModeGet,
         1, 0},
        {"cpssDxChTmGlueQueueMapMcModeSet",
         &wrCpssDxChTmGlueQueueMapMcModeSet,
         2, 0},
        {"cpssDxChTmGlueQueueMapMcModeGet",
         &wrCpssDxChTmGlueQueueMapMcModeGet,
         1, 0},
        {"cpssDxChTmGlueQueueMapSelectorTableAccessModeSet",
         &wrCpssDxChTmGlueQueueMapSelectorTableAccessModeSet,
         2, 0},
        {"cpssDxChTmGlueQueueMapSelectorTableAccessModeGet",
         &wrCpssDxChTmGlueQueueMapSelectorTableAccessModeGet,
         1, 0},
        {"cpssDxChTmGlueQueueMapL1PckLengthOffsetSet",
         &wrCpssDxChTmGlueQueueMapL1PacketLengthOffsetSet,
         3, 3},
        {"cpssDxChTmGlueQueueMapL1PckLengthOffsetGetFirst",
         &wrCpssDxChTmGlueQueueMapL1PacketLengthOffsetGetFirst,
         3, 0},
        {"cpssDxChTmGlueQueueMapL1PckLengthOffsetGetNext",
         &wrCpssDxChTmGlueQueueMapL1PacketLengthOffsetGetNext,
         3, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChTmGlueQueueMap
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
GT_STATUS cmdLibInitCpssDxChTmGlueQueueMap
(
    GT_VOID
)
{
    return  cmdInitLibrary(dbCommands, numCommands);
}


