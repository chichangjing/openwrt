/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChPortPip.c
*
* DESCRIPTION:
*       Wrapper functions for CPSS DXCH Port PIP API
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortPip.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpssCommon/private/prvCpssMath.h>

/*******************************************************************************
* cpssDxChPortPipGlobalEnableSet
*
* DESCRIPTION:
*       Enable/disable the PIP engine. (Global to the device)
*       if enabled packets are dropped according to their parsed priority if FIFOs
*       before the control pipe are above a configurable threshold
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       enable  - GT_TRUE  - enable  PIP
*                 GT_FALSE - disable PIP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS result;
    GT_U8    devNum;
    GT_BOOL             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    enable  = (GT_BOOL)inArgs[ii++];

    /* call cpss api function */
    result = cpssDxChPortPipGlobalEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPipGlobalEnableGet
*
* DESCRIPTION:
*       Get the state Enable/disable of the PIP engine. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                 GT_TRUE  - enable  PIP
*                 GT_FALSE - disable PIP
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS  result;
    GT_U8      devNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];

    /* call cpss api function */
    result = cpssDxChPortPipGlobalEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPipGlobalVidClassificationSet
*
* DESCRIPTION:
*       Set PIP vid classification value per index. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum   - physical device number
*       index    - one of 4 indexes.
*                   (APPLICABLE RANGES: 0..3)
*       vid      - the vid to match
*                   (APPLICABLE RANGES: 0..(4k-1))
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or index
*       GT_OUT_OF_RANGE          - on out of range of vid
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalVidClassificationSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS result;
    GT_U8    devNum;
    GT_U32   index;
    GT_U16   vid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    index  = (GT_U32)inArgs[ii++];
    vid  = (GT_U16)inArgs[ii++];

    /* call cpss api function */
    result = cpssDxChPortPipGlobalVidClassificationSet(devNum, index , vid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPipGlobalVidClassificationGet
*
* DESCRIPTION:
*       Get PIP vid classification value per index. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum   - physical device number
*       index    - one of 4 indexes.
*                   (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       vidPtr   - (pointer to)the vid to match
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or index
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalVidClassificationGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS  result;
    GT_U8      devNum;
    GT_U32   index;
    GT_U16   vid;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    index  = (GT_U32)inArgs[ii++];

    /* call cpss api function */
    result = cpssDxChPortPipGlobalVidClassificationGet(devNum, index , &vid);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vid);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPipGlobalMacDaClassificationEntrySet
*
* DESCRIPTION:
*       Set PIP Mac Da classification entry. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum   - physical device number
*       index    - index to the 'mac DA classification table'.
*                   (APPLICABLE RANGES: 0..3)
*       entryPtr - (pointer to) the entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or index
*       GT_BAD_PTR               - on NULL pointer
*       GT_OUT_OF_RANGE          - on out of value in entry
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalMacDaClassificationEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS result;
    GT_U8    devNum;
    GT_U32   index;
    CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&entry,0,sizeof(entry));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    index  = (GT_U32)inArgs[ii++];
    galtisMacAddr(&entry.macAddrValue, (GT_U8*)inArgs[ii++]);
    galtisMacAddr(&entry.macAddrMask, (GT_U8*)inArgs[ii++]);

    /* call cpss api function */
    result = cpssDxChPortPipGlobalMacDaClassificationEntrySet(devNum, index , &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPipGlobalMacDaClassificationEntryGet
*
* DESCRIPTION:
*       Get PIP Mac Da classification entry. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum   - physical device number
*       index    - index to the 'mac DA classification table'.
*                   (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       entryPtr - (pointer to) the entry.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or index
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalMacDaClassificationEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS result;
    GT_U8    devNum;
    GT_U32   index;
    CPSS_DXCH_PORT_PIP_MAC_DA_CLASSIFICATION_STC entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cpssOsMemSet(&entry,0,sizeof(entry));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    index  = (GT_U32)inArgs[ii++];

    /* call cpss api function */
    result = cpssDxChPortPipGlobalMacDaClassificationEntryGet(devNum, index , &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%6b%6b",
                            entry.macAddrValue.arEther,
                            entry.macAddrMask.arEther);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPipGlobalBurstFifoThresholdsSet
*
* DESCRIPTION:
*       Set Burst FIFO Threshold for specific PIP priority. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       priority  - the priority to set the threshold.
*                   (APPLICABLE RANGES: 0..2)
*       threshold  - the threshold value. (number of packets)
*                   (APPLICABLE RANGES: 0..0x3ff)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or priority
*       GT_OUT_OF_RANGE          - on out of range threshold
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalBurstFifoThresholdsSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS result;
    GT_U8    devNum;
    GT_U32   priority;
    GT_U32   threshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    priority  = (GT_U32)inArgs[ii++];
    threshold  = (GT_U32)inArgs[ii++];

    /* call cpss api function */
    result = cpssDxChPortPipGlobalBurstFifoThresholdsSet(devNum, priority , threshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChPortPipGlobalBurstFifoThresholdsGet
*
* DESCRIPTION:
*       Get Burst FIFO Threshold for specific PIP priority. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       priority  - the priority to set the threshold.
*                   (APPLICABLE RANGES: 0..2)
*
* OUTPUTS:
*       thresholdPtr - (pointer to)the threshold value. (number of packets)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or priority
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalBurstFifoThresholdsGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS result;
    GT_U8    devNum;
    GT_U32   priority;
    GT_U32   threshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    priority  = (GT_U32)inArgs[ii++];

    /* call cpss api function */
    result = cpssDxChPortPipGlobalBurstFifoThresholdsGet(devNum, priority , &threshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", threshold);
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChPortPipGlobalEtherTypeProtocolSet
*
* DESCRIPTION:
*       Set the PIP etherTypes for the protocol. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       protocol  - the protocol for which we define the etherType
*       index     - relevant to 'vlan tag' that supports 4 etherTypes (TPIDs)
*                   relevant to 'mpls' that supports 2 etherTypes
*                   relevant to 'user defined' that supports 4 etherTypes
*                   ignored for other protocols.
*                   (APPLICABLE RANGES: 0..3)
*       etherType - etherType to recognize the protocols packets
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or protocol or index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalEtherTypeProtocolSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS result;
    GT_U8    devNum;
    CPSS_DXCH_PORT_PIP_PROTOCOL_ENT  protocol;
    GT_U32   index;
    GT_U16   etherType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    protocol  = (CPSS_DXCH_PORT_PIP_PROTOCOL_ENT)inArgs[ii++];
    index  = (GT_U32)inArgs[ii++];
    etherType  = (GT_U16)inArgs[ii++];

    /* call cpss api function */
    result = cpssDxChPortPipGlobalEtherTypeProtocolSet(devNum, protocol , index , etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChPortPipGlobalEtherTypeProtocolGet
*
* DESCRIPTION:
*       Get the PIP etherTypes for the protocol. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       protocol  - the protocol for which we define the etherType
*       index     - relevant to 'mpls' that supports 2 etherTypes
*                   relevant to 'user defined' that supports 4 etherTypes
*                   ignored for other protocols.
*                   (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       etherType - (pointer to) etherType to recognize the protocols packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or protocol or index
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalEtherTypeProtocolGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS result;
    GT_U8    devNum;
    CPSS_DXCH_PORT_PIP_PROTOCOL_ENT  protocol;
    GT_U32   index;
    GT_U16   etherType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    protocol  = (CPSS_DXCH_PORT_PIP_PROTOCOL_ENT)inArgs[ii++];
    index  = (GT_U32)inArgs[ii++];

    /* call cpss api function */
    result = cpssDxChPortPipGlobalEtherTypeProtocolGet(devNum, protocol , index , &etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", etherType);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPipGlobalDropCounterGet
*
* DESCRIPTION:
*      Get PIP drop counter of specific PIP priority. (Global to the device)
*      NOTE: the counters are 'clear on read'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       priority  - the priority to set the threshold.
*                   (APPLICABLE RANGES: 0..2)
*
* OUTPUTS:
*       counterPtr - (pointer to) the counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or priority
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalDropCounterGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS result;
    GT_U8    devNum;
    GT_U32   priority;
    GT_U64   counter;
    static  GT_U64   saved_counterArr[3];
    GT_U64   new_counterArr[3];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    priority  = (GT_U32)inArgs[ii++];

    if(priority > 2)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_BAD_PARAM, "%d%d", 0,0);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChPortPipGlobalDropCounterGet(devNum, new_counterArr);
    if(result == GT_OK)
    {
        /* accumulate the new and the saved values */
        saved_counterArr[0] = prvCpssMathAdd64(new_counterArr[0],saved_counterArr[0]);
        saved_counterArr[1] = prvCpssMathAdd64(new_counterArr[1],saved_counterArr[1]);
        saved_counterArr[2] = prvCpssMathAdd64(new_counterArr[2],saved_counterArr[2]);

        /* get the needed counter */
        counter = saved_counterArr[priority];

        /* reset the counter in th DB */
        saved_counterArr[priority].l[0] = 0;
        saved_counterArr[priority].l[1] = 0;
    }
    else
    {
        counter.l[0] = 0;
        counter.l[1] = 0;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", counter.l[0],counter.l[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPipGlobalDropCounterGet
*
* DESCRIPTION:
*      Get PIP drop counter of specific PIP priority. (Global to the device)
*      NOTE: the counters are 'clear on read'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       priority  - the priority to set the threshold.
*                   (APPLICABLE RANGES: 0..2)
*
* OUTPUTS:
*       counterPtr - (pointer to) the counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or priority
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalDropCounterGet1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS result;
    GT_U8    devNum;
    GT_U64   counterArr[3];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];

    /* call cpss api function */
    result = cpssDxChPortPipGlobalDropCounterGet(devNum, counterArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d",
        counterArr[0].l[0],counterArr[0].l[1],
        counterArr[1].l[0],counterArr[1].l[1],
        counterArr[2].l[0],counterArr[2].l[1]
        );
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortPipTrustEnableSet
*
* DESCRIPTION:
*       Set port as trusted/not trusted PIP port.
*       A 'trusted' port will set priority level for ingress packets according to
*       packet's fields.
*       Otherwise, for the 'not trusted' port will use the port’s default priority
*       level.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - GT_TRUE  - set port as 'trusted'
*                   GT_FALSE - set port as 'not trusted'
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipTrustEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_BOOL                         enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];
    enable  = (GT_BOOL)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPipTrustEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPipTrustEnableGet
*
* DESCRIPTION:
*       Get port's trusted/not trusted .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       enablePtr   -  (pointer to)
*                       GT_TRUE  - set port as 'trusted'
*                       GT_FALSE - set port as 'not trusted'
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipTrustEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS  result;
    GT_U8      devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPipTrustEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChPortPipProfileSet
*
* DESCRIPTION:
*       Set port's PIP profile.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       pipProfile   - the profile of the port.
*                   (APPLICABLE RANGES: 0..3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipProfileSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_U32                          pipProfile;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];
    pipProfile  = (GT_U32)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPipProfileSet(devNum, portNum, pipProfile);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPipProfileGet
*
* DESCRIPTION:
*       Get port's PIP profile.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       pipProfilePtr - (pointer to) the profile of the port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipProfileGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_U32                          pipProfile;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPipProfileGet(devNum, portNum, &pipProfile);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", pipProfile);
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChPortPipVidClassificationEnableSet
*
* DESCRIPTION:
*       Set port PIP enable/disable VID classification .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable   - GT_TRUE   - enable VID classification
*                  GT_FALSE  - disable VID classification
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipVidClassificationEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_BOOL                         enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];
    enable  = (GT_BOOL)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPipVidClassificationEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChPortPipVidClassificationEnableGet
*
* DESCRIPTION:
*       Get port PIP enable/disable VID classification .
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                  GT_TRUE   - enable VID classification
*                  GT_FALSE  - disable VID classification
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipVidClassificationEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_BOOL                         enable;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPipVidClassificationEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChPortPipPrioritySet
*
* DESCRIPTION:
*       Set priority per Port for 'classification' type.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       type      - 'classification' type
*       priority  - the priority value to set. (0-high,1-mid,2-low)
*                   (APPLICABLE RANGES: 0..2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum or type
*       GT_OUT_OF_RANGE          - on out of range priority
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipPrioritySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT type;
    GT_U32                          priority;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];
    type  = (CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT)inArgs[ii++];
    priority  = (GT_U32)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPipPrioritySet(devNum, portNum, type , priority);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPipPriorityGet
*
* DESCRIPTION:
*       Get priority per Port for 'classification' type.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       type      - 'classification' type
*
* OUTPUTS:
*       priorityPtr  - (pointer to)the priority value to set. (0-high,1-mid,2-low)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum or type
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipPriorityGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT type;
    GT_U32                          priority;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    portNum  = (GT_PHYSICAL_PORT_NUM)inArgs[ii++];
    type  = (CPSS_DXCH_PORT_PIP_CLASSIFICATION_TYPE_ENT)inArgs[ii++];

    CONVERT_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPipPriorityGet(devNum, portNum, type , &priority);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", priority);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortPipGlobalProfilePrioritySet
*
* DESCRIPTION:
*       Set priority per PIP profile Per 'classification' type. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum          - physical device number
*       pipProfile    - index to the 'profile table'.
*                   (APPLICABLE RANGES: 0..3)
*       type            - 'classification' type
*       fieldIndex      - index of the priority field in the 'profile entry'
*                          see details in description of relevant field in
*                          CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT
*       priority        - the priority value to set. (0-high,1-mid,2-low)
*                   (APPLICABLE RANGES: 0..2)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or type or pipProfile or fieldIndex
*       GT_OUT_OF_RANGE          - on out of range priority
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalProfilePrioritySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          pipProfile;
    CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT type;
    GT_U32                          fieldIndex;
    GT_U32                          priority;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    pipProfile  = (GT_U32)inArgs[ii++];
    type  = (CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT)inArgs[ii++];

    ii = 0;/* now start with fields*/
    fieldIndex  = (GT_U32)inFields[ii++];
    priority  = (GT_U32)inFields[ii++];

    /* call cpss api function */
    result = cpssDxChPortPipGlobalProfilePrioritySet(devNum, pipProfile, type , fieldIndex , priority);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/* cpssDxChPortPipGlobalProfilePriorityGetFirst Table */
static GT_U32   indexCnt;
/*******************************************************************************
* cpssDxChPortPipGlobalProfilePriorityGet
*
* DESCRIPTION:
*       Get priority per PIP profile Per 'classification' type. (Global to the device)
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum          - physical device number
*       pipProfile    - index to the 'profile table'.
*                   (APPLICABLE RANGES: 0..3)
*       type            - 'classification' type
*       fieldIndex      - index of the priority field in the 'profile entry'
*                          see details in description of relevant field in
*                          CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT
*
* OUTPUTS:
*       priorityPtr        - (pointer to)the priority value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or type or pipProfile or fieldIndex
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipGlobalProfilePriorityGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_U32                          pipProfile;
    CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT type;
    GT_U32                          priority;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];
    pipProfile  = (GT_U32)inArgs[ii++];
    type  = (CPSS_DXCH_PORT_PIP_PROFILE_CLASSIFICATION_TYPE_ENT)inArgs[ii++];

    /* call cpss api function */
    result = cpssDxChPortPipGlobalProfilePriorityGet(devNum, pipProfile, type , indexCnt , &priority);
    if (result == GT_BAD_PARAM)
    {
        /* no more indexes */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",indexCnt,priority);

    indexCnt++;

    return CMD_OK;
}

/* get first */
static CMD_STATUS wrCpssDxChPortPipGlobalProfilePriorityGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    indexCnt = 0;
    return wrCpssDxChPortPipGlobalProfilePriorityGetNext(inArgs,inFields,numFields,outArgs);
}

/* cpssDxChPortPipDropCounter Table */
static GT_PHYSICAL_PORT_NUM   portIndex;
/* look for valid portIndex that hold 'RxDma' */
static GT_STATUS getPortWithRxDma(IN GT_U8    devNum)
{
    GT_U32  portRxdmaNum;
    GT_STATUS rc;

    do{
        rc = prvCpssDxChPortPhysicalPortMapCheckAndConvert(devNum, portIndex, PRV_CPSS_DXCH_PORT_TYPE_RxDMA_E, &portRxdmaNum);
        if(rc != GT_OK)
        {
            portIndex++;
            continue;
        }

        return GT_OK;
    }while(portIndex < 256);

    return GT_NO_MORE;
}

/*******************************************************************************
* cpssDxChPortPipDropCounterGet
*
* DESCRIPTION:
*      Get PIP drop counter of specific port.
*      NOTE: the counters are 'clear on read'
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       counterPtr - (pointer to) the counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum or portNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPipDropCounterGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32   ii = 0;
    GT_STATUS                       result;
    GT_U8                           devNum;
    GT_PHYSICAL_PORT_NUM            portNum;
    GT_U64                          counter;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[ii++];

    if (GT_OK != getPortWithRxDma(devNum))
    {
        /* no more ports with RxDma */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChPortPipDropCounterGet(devNum, portIndex, &counter);

    portNum = portIndex;
    CONVERT_BACK_DEV_PHYSICAL_PORT_MAC(devNum, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",portNum,counter.l[0],counter.l[1]);

    portIndex++;

    return CMD_OK;
}

/* get first */
static CMD_STATUS wrCpssDxChPortPipDropCounterGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    portIndex = 0;
    return wrCpssDxChPortPipDropCounterGetNext(inArgs,inFields,numFields,outArgs);
}



/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    /*********************/
    /* commands - global */
    /*********************/
    {"cpssDxChPortPipGlobalEnableSet",
        &wrCpssDxChPortPipGlobalEnableSet,
        2, 0},
    {"cpssDxChPortPipGlobalEnableGet",
        &wrCpssDxChPortPipGlobalEnableGet,
        1, 0},

    {"cpssDxChPortPipGlobalVidClassificationSet",
        &wrCpssDxChPortPipGlobalVidClassificationSet,
        3, 0},
    {"cpssDxChPortPipGlobalVidClassificationGet",
        &wrCpssDxChPortPipGlobalVidClassificationGet,
        2, 0},

    {"cpssDxChPortPipGlobalMacDaClassificationEntrySet",
        &wrCpssDxChPortPipGlobalMacDaClassificationEntrySet,
        4, 0},
    {"cpssDxChPortPipGlobalMacDaClassificationEntryGet",
        &wrCpssDxChPortPipGlobalMacDaClassificationEntryGet,
        2, 0},

    {"cpssDxChPortPipGlobalBurstFifoThresholdsSet",
        &wrCpssDxChPortPipGlobalBurstFifoThresholdsSet,
        3, 0},
    {"cpssDxChPortPipGlobalBurstFifoThresholdsGet",
        &wrCpssDxChPortPipGlobalBurstFifoThresholdsGet,
        2, 0},

    {"cpssDxChPortPipGlobalEtherTypeProtocolSet",
        &wrCpssDxChPortPipGlobalEtherTypeProtocolSet,
        4, 0},
    {"cpssDxChPortPipGlobalEtherTypeProtocolGet",
        &wrCpssDxChPortPipGlobalEtherTypeProtocolGet,
        3, 0},

    {"cpssDxChPortPipGlobalDropCounterGet",
        &wrCpssDxChPortPipGlobalDropCounterGet,
        2, 0},
    {"cpssDxChPortPipGlobalDropCounterGet1",
        &wrCpssDxChPortPipGlobalDropCounterGet1,
        1, 0},


    /***********************/
    /* commands - per port */
    /***********************/
    {"cpssDxChPortPipTrustEnableSet",
        &wrCpssDxChPortPipTrustEnableSet,
        3, 0},
    {"cpssDxChPortPipTrustEnableGet",
        &wrCpssDxChPortPipTrustEnableGet,
        2, 0},

    {"cpssDxChPortPipProfileSet",
        &wrCpssDxChPortPipProfileSet,
        3, 0},
    {"cpssDxChPortPipProfileGet",
        &wrCpssDxChPortPipProfileGet,
        2, 0},

    {"cpssDxChPortPipVidClassificationEnableSet",
        &wrCpssDxChPortPipVidClassificationEnableSet,
        3, 0},
    {"cpssDxChPortPipVidClassificationEnableGet",
        &wrCpssDxChPortPipVidClassificationEnableGet,
        2, 0},

    {"cpssDxChPortPipPrioritySet",
        &wrCpssDxChPortPipPrioritySet,
        4, 0},
    {"cpssDxChPortPipPriorityGet",
        &wrCpssDxChPortPipPriorityGet,
        3, 0},

    /**********/
    /* tables */
    /**********/

    /* table : cpssDxChPortPipGlobalProfilePrioritySet */
            {"cpssDxChPortPipGlobalProfilePrioritySet",
                &wrCpssDxChPortPipGlobalProfilePrioritySet,
                3, /*dev,profile,type*/
                2},/*fieldIndex, priority*/

            {"cpssDxChPortPipGlobalProfilePriorityGetFirst",
                &wrCpssDxChPortPipGlobalProfilePriorityGetFirst,
                3, /*dev,profile,type*/
                0},/*fieldIndex, priority*/

            {"cpssDxChPortPipGlobalProfilePriorityGetNext",
                &wrCpssDxChPortPipGlobalProfilePriorityGetNext,
                3, /*dev,profile,type*/
                0},/*fieldIndex, priority*/

    /* table : cpssDxChPortPipDropCounter */
            {"cpssDxChPortPipDropCounterGetFirst",
                &wrCpssDxChPortPipDropCounterGetFirst,
                1, /*dev*/
                0},/*port , counter*/

            {"cpssDxChPortPipDropCounterGetNext",
                &wrCpssDxChPortPipDropCounterGetNext,
                1, /*dev*/
                0},/*port , counter*/

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChPortPip
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
GT_STATUS cmdLibInitCpssDxChPortPip
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

