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
* wrapCosCpssDxCh.c
*
* DESCRIPTION:
*       Wrapper functions for Cos cpss.dxCh functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 22 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/cos/cpssDxChCos.h>
#include <cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>

/*******************************************************************************
* cpssDxChCosProfileEntrySet
*
* DESCRIPTION:
*       Configures the Profile Table Entry.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev           - packet processor number.
*       profileIndex  - index of a profile in the profile table. Values 0 - 71
*       dscp          - new DSCP value for packet with assign profile equal
*                       profileIndex. The new DSCP is changed in the packet only
*                       if 'modify DSCP flag' is set in the packet descriptor
*       cos           - Pointer to new CoS values for packet - TC, UP and DP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev,dscp or cos.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosProfileEntrySet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;

    GT_U8                            dev;
    GT_U32                           profileIndex;
    CPSS_DXCH_COS_PROFILE_STC        cosPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    profileIndex = (GT_U32)inFields[0];

    cosPtr.dropPrecedence = (CPSS_DP_LEVEL_ENT)inFields[1];
    cosPtr.userPriority = (GT_U32)inFields[2];
    cosPtr.trafficClass = (GT_U32)inFields[3];
    cosPtr.dscp = (GT_U32)inFields[4];
    cosPtr.exp = (GT_U32)inFields[5];

    /* call cpss api function */
    result = cpssDxChCosProfileEntrySet(dev, profileIndex, &cosPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*cpssDxChCosProfileEntry Table*/
static GT_U32   profileCnt;


/*******************************************************************************
* cpssDxChCosProfileEntryGet
*
* DESCRIPTION:
*       Get Profile Table Entry configuration.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev           - packet processor number.
*       profileIndex  - index of a profile in the profile table. Values 0 - 71
*
* OUTPUTS:
*       dscpPtr       - (pointer to) DSCP value for packet with assign profile equal
*                       profileIndex. The new DSCP is changed in the packet only
*                       if 'modify DSCP flag' is set in the packet descriptor
*       cosPtr        - Pointer to new CoS values for packet - TC, UP and DP
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev,dscp or cos.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosProfileEntryGet
(
    IN  GT_U8 dev,
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;
    CPSS_DXCH_COS_PROFILE_STC        cosPtr;

    /* call cpss api function */
    result = cpssDxChCosProfileEntryGet(dev, profileCnt, &cosPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = profileCnt;
    inFields[1] = cosPtr.dropPrecedence;
    inFields[2] = cosPtr.userPriority;
    inFields[3] = cosPtr.trafficClass;
    inFields[4] = cosPtr.dscp;
    inFields[5] = cosPtr.exp;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                inFields[3], inFields[4], inFields[5]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosProfileEntryGet
*
* DESCRIPTION:
*       Get Profile Table Entry configuration.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev           - packet processor number.
*       profileIndex  - index of a profile in the profile table. Values 0 - 71
*
* OUTPUTS:
*       dscpPtr       - (pointer to) DSCP value for packet with assign profile equal
*                       profileIndex. The new DSCP is changed in the packet only
*                       if 'modify DSCP flag' is set in the packet descriptor
*       cosPtr        - Pointer to new CoS values for packet - TC, UP and DP
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev,dscp or cos.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosProfileEntryGetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                            dev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    profileCnt = 0;


    return wrCpssDxChCosProfileEntryGet(dev, inFields, outArgs);
}


/*******************************************************************************/
static CMD_STATUS wrCpssDxChCosProfileEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                            dev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    profileCnt++;

    if(profileCnt >= (GT_U32)PRV_CPSS_DXCH_QOS_PROFILE_MAX_MAC(dev))
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    return wrCpssDxChCosProfileEntryGet(dev, inFields, outArgs);
}

/*******************************************************************************
* cpssDxChCosDscpToProfileMapSet
*
* DESCRIPTION:
*       Maps the packet DSCP (or remapped DSCP) to a QoS Profile.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev           - packet processor number.
*       dscp          - DSCP of a IP packet
*       profileIndex  - profile index, which is assigned to a IP packet with
*                       the DSCP on ports with enabled trust L3 or trust L2-L3.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, dscp or profileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosDscpToProfileMapSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U8     dscp;
    GT_U32    profileIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    dscp = (GT_U8)inArgs[1];
    profileIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosDscpToProfileMapSet(dev, 0/*mappingTableIndex*/,dscp, profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosDscpToProfileMapGet
*
* DESCRIPTION:
*       Get the Mapping: packet DSCP (or remapped DSCP) to a QoS Profile.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev           - packet processor number.
*       dscp          - DSCP of a IP packet
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index, which is assigned to a IP packet with
*                       the DSCP on ports with enabled trust L3 or trust L2-L3.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, dscp or profileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosDscpToProfileMapGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U8     dscp;
    GT_U32    profileIndexPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    dscp = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCosDscpToProfileMapGet(dev, 0/*mappingTableIndex*/,dscp, &profileIndexPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileIndexPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosDscpToProfileMapSet
*
* DESCRIPTION:
*       Maps the packet DSCP (or remapped DSCP) to a QoS Profile.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev           - packet processor number.
*       mappingTableIndex - mapping table index (see API cpssDxChCosPortTrustQosMappingTableIndexSet)
*                           (APPLICABLE RANGES : Bobcat2; Caelum; Bobcat3 0..11)
*                           APPLICABLE DEVICES : Bobcat2; Caelum; Bobcat3.
*       dscp          - DSCP of a IP packet
*       profileIndex  - profile index, which is assigned to a IP packet with
*                       the DSCP on ports with enabled trust L3 or trust L2-L3.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, dscp or profileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosDscpToProfileMapSet_1

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U8     dscp;
    GT_U32    profileIndex;
    GT_U32    mappingTableIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev               =  (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    dscp              =  (GT_U8)inArgs[2];
    profileIndex      = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChCosDscpToProfileMapSet(dev, mappingTableIndex,dscp, profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosDscpToProfileMapGet
*
* DESCRIPTION:
*       Get the Mapping: packet DSCP (or remapped DSCP) to a QoS Profile.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev           - packet processor number.
*       mappingTableIndex - mapping table index (see API cpssDxChCosPortTrustQosMappingTableIndexSet)
*                           (APPLICABLE RANGES : Bobcat2; Caelum; Bobcat3 0..11)
*                           APPLICABLE DEVICES : Bobcat2; Caelum; Bobcat3.
*       dscp          - DSCP of a IP packet
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index, which is assigned to a IP packet with
*                       the DSCP on ports with enabled trust L3 or trust L2-L3.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, dscp or profileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosDscpToProfileMapGet_1

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U8     dscp;
    GT_U32    profileIndexPtr;
    GT_U32    mappingTableIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev               =  (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    dscp              =  (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosDscpToProfileMapGet(dev, mappingTableIndex,dscp, &profileIndexPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileIndexPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosDscpMutationEntrySet
*
* DESCRIPTION:
*       Maps the packet DSCP to a new, mutated DSCP.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev      - packet processor number.
*       dscp     - DSCP of a IP packet
*       newDscp  - new DSCP, which is assigned to a IP packet with the DSCP on
*          ports with enabled trust L3 or trust L2-L3 and enabled DSCP mutation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, dscp or newDscp
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosDscpMutationEntrySet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U8     dscp;
    GT_U8     newDscp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    dscp = (GT_U8)inArgs[1];
    newDscp = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosDscpMutationEntrySet(dev, 0,dscp, newDscp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosDscpMutationEntryGet
*
* DESCRIPTION:
*       Get the Mapping: packet DSCP to a new, mutated DSCP.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev      - packet processor number.
*       dscp     - DSCP of a IP packet
*
* OUTPUTS:
*       newDscp  - new DSCP, which is assigned to a IP packet with the DSCP on
*          ports with enabled trust L3 or trust L2-L3 and enabled DSCP mutation.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, dscp or newDscp
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosDscpMutationEntryGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U8     dscp;
    GT_U8     newDscpPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    dscp = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCosDscpMutationEntryGet(dev, 0,dscp, &newDscpPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", newDscpPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosDscpMutationEntrySet
*
* DESCRIPTION:
*       Maps the packet DSCP to a new, mutated DSCP.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev      - packet processor number.
*       mappingTableIndex - mapping table index (see API cpssDxChCosPortTrustQosMappingTableIndexSet)
*                           (APPLICABLE RANGES : Bobcat2; Caelum; Bobcat3 0..11)
*                           APPLICABLE DEVICES : Bobcat2; Caelum; Bobcat3.
*       dscp     - DSCP of a IP packet
*       newDscp  - new DSCP, which is assigned to a IP packet with the DSCP on
*          ports with enabled trust L3 or trust L2-L3 and enabled DSCP mutation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, dscp or newDscp
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosDscpMutationEntrySet_1

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U8     dscp;
    GT_U8     newDscp;
    GT_U32    mappingTableIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev               =  (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    dscp              =  (GT_U8)inArgs[2];
    newDscp           =  (GT_U8)inArgs[3];

    /* call cpss api function */
    result = cpssDxChCosDscpMutationEntrySet(dev, mappingTableIndex,dscp, newDscp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosDscpMutationEntryGet
*
* DESCRIPTION:
*       Get the Mapping: packet DSCP to a new, mutated DSCP.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev      - packet processor number.
*       mappingTableIndex - mapping table index (see API cpssDxChCosPortTrustQosMappingTableIndexSet)
*                           (APPLICABLE RANGES : Bobcat2; Caelum; Bobcat3 0..11)
*                           APPLICABLE DEVICES : Bobcat2; Caelum; Bobcat3.
*       dscp     - DSCP of a IP packet
*
* OUTPUTS:
*       newDscp  - new DSCP, which is assigned to a IP packet with the DSCP on
*          ports with enabled trust L3 or trust L2-L3 and enabled DSCP mutation.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, dscp or newDscp
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosDscpMutationEntryGet_1

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U8     dscp;
    GT_U8     newDscpPtr;
    GT_U32    mappingTableIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev               =  (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    dscp              =  (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosDscpMutationEntryGet(dev, mappingTableIndex,dscp, &newDscpPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", newDscpPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosUpToProfileMapSet
*
* DESCRIPTION:
*       Maps the packet User Priority to a QoS Profile.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev             - packet processor number.
*       up              - user priority of a VLAN or Marvell tagged packet.
*       profileIndex    - profile index, which is assigned to a VLAN or
*                         Marvell tagged packet with the up on ports with
*                         enabled trust L2 or trust L2-L3.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, up or profileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosUpToProfileMapSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U8     up;
    GT_U32    profileIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    up = (GT_U8)inArgs[1];
    profileIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosUpCfiDeiToProfileMapSet(dev,0,  0/* backward compatible */ ,up, 0 /* backward compatible */ , profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosUpToProfileMapGet
*
* DESCRIPTION:
*       Returns the mapping of packet User Priority to a QoS Profile.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev             - packet processor number.
*       up              - user priority of a VLAN or Marvell tagged packet.
*       profileIndexPtr - (pointer to)profile index, which is assigned to a VLAN or
*                         Marvell tagged packet with the up on ports with
*                         enabled trust L2 or trust L2-L3.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, up or profileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosUpToProfileMapGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U8     up;
    GT_U32    profileIndexPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    up = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCosUpCfiDeiToProfileMapGet(dev, 0, 0/* backward compatible */ ,up, 0 /* backward compatible */, &profileIndexPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileIndexPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosExpToProfileMapSet
*
* DESCRIPTION:
*       Maps the packet MPLS exp to a QoS Profile.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       dev             - packet processor number.
*       exp             - mpls exp [0..7].
*       profileIndex    - profile index, which is assigned to a VLAN or
*                         Marvell tagged packet with the up on ports with
*                         enabled trust L2 or trust L2-L3.
*                         Ch values (0..71); Ch2 values (0..127)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, exp or profileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosExpToProfileMapSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U32     exp;
    GT_U32     profileIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    exp = (GT_U32)inArgs[1];
    profileIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosExpToProfileMapSet(dev, 0, exp, profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosExpToProfileMapGet
*
* DESCRIPTION:
*       Returns the mapping of packet MPLS exp to a QoS Profile.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       dev             - packet processor number.
*       exp             - mpls exp.
*
* OUTPUTS:
*       profileIndexPtr - (pointer to)profile index, which is assigned to a VLAN or
*                         Marvell tagged packet with the up on ports with
*                         enabled trust L2 or trust L2-L3.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, exp or profileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosExpToProfileMapGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U32     exp;
    GT_U32     profileIndexPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    exp = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCosExpToProfileMapGet(dev, 0, exp, &profileIndexPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileIndexPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosExpToProfileMapSet
*
* DESCRIPTION:
*       Maps the packet MPLS exp to a QoS Profile.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       dev             - packet processor number.
*       exp             - mpls exp [0..7].
*       profileIndex    - profile index, which is assigned to a VLAN or
*                         Marvell tagged packet with the up on ports with
*                         enabled trust L2 or trust L2-L3.
*                         Ch values (0..71); Ch2 values (0..127)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, exp or profileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosExpToProfileMapSet_1

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8      dev;
    GT_U32     mappingTableIndex;
    GT_U32     exp;
    GT_U32     profileIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    exp = (GT_U32)inArgs[2];
    profileIndex = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChCosExpToProfileMapSet(dev, mappingTableIndex, exp, profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosExpToProfileMapGet
*
* DESCRIPTION:
*       Returns the mapping of packet MPLS exp to a QoS Profile.
*
* APPLICABLE DEVICES:  All DxCh2 devices
*
* INPUTS:
*       dev             - packet processor number.
*       exp             - mpls exp.
*
* OUTPUTS:
*       profileIndexPtr - (pointer to)profile index, which is assigned to a VLAN or
*                         Marvell tagged packet with the up on ports with
*                         enabled trust L2 or trust L2-L3.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, exp or profileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosExpToProfileMapGet_1

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U32     mappingTableIndex;
    GT_U32     exp;
    GT_U32     profileIndexPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    exp = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosExpToProfileMapGet(dev, mappingTableIndex, exp, &profileIndexPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileIndexPtr);
    return CMD_OK;
}



/************** Table: cpssDxChCosPortQosConfig ****************/
static GT_PORT_NUM gPortGet;

/*******************************************************************************
* cpssDxChCosPortQosConfigSet
*
* DESCRIPTION:
*       Configures the port's QoS attributes.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev             - packet processor number.
*       port            - logical port number.
*       portQosCfg_PTR  - Pointer to QoS related configuration of a port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, port or portQosCfg_PTR
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortQosConfigSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result;

    GT_U8                 dev;
    GT_PORT_NUM                 port;
    CPSS_QOS_ENTRY_STC    portQosCfg_PTR;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    port = (GT_PORT_NUM)inFields[0];

    portQosCfg_PTR.qosProfileId = (GT_U32)inFields[1];
    portQosCfg_PTR.assignPrecedence =
                       (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[2];
    portQosCfg_PTR.enableModifyUp =
                             (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[3];
    portQosCfg_PTR.enableModifyDscp =
                             (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosPortQosConfigSet(dev, port, &portQosCfg_PTR);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortQosConfigGet
*
* DESCRIPTION:
*       Get the port's QoS attributes configuration.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev             - packet processor number.
*       port            - logical port number.
*
* OUTPUTS:
*       portQosCfg_PTR  - Pointer to QoS related configuration of a port.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, port or portQosCfg_PTR
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortQosConfigGetNext

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                 dev;
    GT_STATUS             result;
    CPSS_QOS_ENTRY_STC    portQosCfg_PTR;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    while(gPortGet < PRV_CPSS_MAX_PP_PORTS_NUM_CNS)
    {
        /* call cpss api function */
        result = cpssDxChCosPortQosConfigGet(dev, gPortGet, &portQosCfg_PTR);

        gPortGet++;

        if (GT_OK == result)
        {
            inFields[0] = (gPortGet - 1);
            inFields[1] = portQosCfg_PTR.qosProfileId;
            inFields[2] = portQosCfg_PTR.assignPrecedence;
            inFields[3] = portQosCfg_PTR.enableModifyUp;
            inFields[4] = portQosCfg_PTR.enableModifyDscp;

            /* pack and output table fields */
            fieldOutput("%d%d%d%d%d", inFields[0], inFields[1],
                                    inFields[2], inFields[3], inFields[4]);

            /* pack output arguments to galtis string */
            galtisOutput(outArgs, result, "%f");

            return CMD_OK;
        }
    }

    galtisOutput(outArgs, CMD_OK, "%d", -1);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortQosConfigGet
*
* DESCRIPTION:
*       Get the port's QoS attributes configuration.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev             - packet processor number.
*       port            - logical port number.
*
* OUTPUTS:
*       portQosCfg_PTR  - Pointer to QoS related configuration of a port.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, port or portQosCfg_PTR
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortQosConfigGetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gPortGet = 0;

    return  wrCpssDxChCosPortQosConfigGetNext(inArgs, inFields,
                                              numFields,outArgs);
}

/*******************************************************************************
* cpssDxChCosMacQosEntrySet
*
* DESCRIPTION:
*       Set the QoS Attribute of the MAC QoS Table Entr
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev            - packet processor number.
*       entryIdx       - Index of a MAC QoS table's entry. Valid values 0 - 6.
*       macQosCfgPTR   - Pointer to QoS related configuration of a MAC QoS table's entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, entryIndex or macQosCfg_PTR
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
******************************************************************************/
static CMD_STATUS wrCpssDxChCosMacQosEntrySet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;

    GT_U8                            dev;
    GT_U32                           entryIdx;
    CPSS_QOS_ENTRY_STC               macQosCfg_PTR;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    entryIdx = (GT_U8)inFields[0];

    macQosCfg_PTR.qosProfileId = (GT_U32)inFields[1];
    macQosCfg_PTR.assignPrecedence =
                       (CPSS_PACKET_ATTRIBUTE_ASSIGN_PRECEDENCE_ENT)inFields[2];
    macQosCfg_PTR.enableModifyUp =
                             (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[3];
    macQosCfg_PTR.enableModifyDscp =
                             (CPSS_PACKET_ATTRIBUTE_MODIFY_TYPE_ENT)inFields[4];

    /* call cpss api function */
    result = cpssDxChCosMacQosEntrySet(dev, entryIdx, &macQosCfg_PTR);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*cpssDxChCosMacQosEntry Table*/
static GT_U8   entryCnt;


/*******************************************************************************
* internalCpssDxChCosMacQosEntryGet
*
* DESCRIPTION:
*       Returns the QoS Attribute of the MAC QoS Table Entr
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev            - packet processor number.
*       entryIdx       - Index of a MAC QoS table's entry. Valid values 0 - 6.
*       macQosCfgPTR   - Pointer to QoS related configuration of a MAC QoS table's entry.
*
* OUTPUTS:
*       macQosCfgPTR   - Pointer to QoS related configuration of a MAC QoS table's entry.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, entryIndex or macQosCfg_PTR
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
******************************************************************************/
static CMD_STATUS internalCpssDxChCosMacQosEntryGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;
    GT_U8                            dev;
    CPSS_QOS_ENTRY_STC               macQosCfg_PTR;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];

    entryCnt++;

    if(entryCnt > 7)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }


    /* call cpss api function */
    result = cpssDxChCosMacQosEntryGet(dev, entryCnt, &macQosCfg_PTR);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = entryCnt;
    inFields[1] = macQosCfg_PTR.qosProfileId;
    inFields[2] = macQosCfg_PTR.assignPrecedence;
    inFields[3] = macQosCfg_PTR.enableModifyUp;
    inFields[4] = macQosCfg_PTR.enableModifyDscp;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0],  inFields[1],
               inFields[2], inFields[3],  inFields[4]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosMacQosEntryGet
*
* DESCRIPTION:
*       Returns the QoS Attribute of the MAC QoS Table Entr
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev            - packet processor number.
*       entryIdx       - Index of a MAC QoS table's entry. Valid values 0 - 6.
*       macQosCfgPTR   - Pointer to QoS related configuration of a MAC QoS table's entry.
*
* OUTPUTS:
*       macQosCfgPTR   - Pointer to QoS related configuration of a MAC QoS table's entry.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, entryIndex or macQosCfg_PTR
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
******************************************************************************/
static CMD_STATUS wrCpssDxChCosMacQosEntryGetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    entryCnt = 0;

    return internalCpssDxChCosMacQosEntryGet(inArgs,inFields,numFields,outArgs);
}


/*******************************************************************************/
static CMD_STATUS wrCpssDxChCosMacQosEntryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    return internalCpssDxChCosMacQosEntryGet(inArgs,inFields,numFields,outArgs);
}


/*******************************************************************************
* cpssDxChCosMacQosConflictResolutionSet
*
* DESCRIPTION:
*       Configure QoS Marking Conflict Mode.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev                 - packet processor number.
*       macQosResolvType    - Enum holding two attributes for selecting the
*               SA command or the DA command (0 - DA command, 1 - SA caommand).
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev or macQosResolvType
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosMacQosConflictResolutionSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;

    GT_U8                       dev;
    CPSS_MAC_QOS_RESOLVE_ENT    macQosResolvType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    macQosResolvType = (CPSS_MAC_QOS_RESOLVE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCosMacQosConflictResolutionSet(dev, macQosResolvType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosMacQosConflictResolutionGet
*
* DESCRIPTION:
*       Get Configured QoS Marking Conflict Mode.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev                 - packet processor number.
*
* OUTPUTS:
*       macQosResolvTypePtr - Pointer Enum holding two optional attributes for
*                             selecting the SA command or the DA
*                             command (0 - DA command, 1 - SA caommand).
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev or macQosResolvType
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosMacQosConflictResolutionGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                   result;

    GT_U8                       dev;
    CPSS_MAC_QOS_RESOLVE_ENT    macQosResolvTypePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChCosMacQosConflictResolutionGet(dev, &macQosResolvTypePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", macQosResolvTypePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosRemarkTblEntrySet
*
* DESCRIPTION:
*       Sets new profile index for out profile packets.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev             - packet processor number.
*       profileIndex    - profile index in the QoS Profile Table
*       newProfileIndex - new profile index, which is assigned for out-profile
*                         packets according to Traffic Conditioner command.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, profileIndex or newProfileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosRemarkTblEntrySet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U32    profileIndex;
    GT_U32    newProfileIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    profileIndex = (GT_U32)inArgs[1];
    newProfileIndex = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosRemarkTblEntrySet(dev, profileIndex, newProfileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosRemarkTblEntryGet
*
* DESCRIPTION:
*       Get new profile index setting for out profile packets.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev             - packet processor number.
*       profileIndex    - profile index in the QoS Profile Table
*
* OUTPUTS:
*       newProfileIndexPtr - (pointer to) new profile index, which will be
*                            assigned for out-profile packets according to
*                            Traffic Conditioner command.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, profileIndex or newProfileIndex
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosRemarkTblEntryGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_U32    profileIndex;
    GT_U32    newProfileIndexPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    profileIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCosRemarkTblEntryGet(dev, profileIndex,
                                        &newProfileIndexPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", newProfileIndexPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortDefaultUPSet
*
* DESCRIPTION:
*       Set default user priority (VPT) for untagged packet to a given port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev       - packet processor device number.
*       port      - port number.
*       defaultUserPrio  - default user priority (VPT).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortDefaultUPSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_PORT_NUM     port;
    GT_U8     defaultUserPrio;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    defaultUserPrio = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChPortDefaultUPSet(dev, port, defaultUserPrio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortDefaultUPGet
*
* DESCRIPTION:
*       Get default user priority (VPT) for untagged packet to a given port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev       - packet processor device number.
*       port      - port number.
*
* OUTPUTS:
*       defaultUserPrioPtr  - (pointer to) default user priority (VPT).
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortDefaultUPGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
    GT_PORT_NUM     port;
    GT_U8     defaultUserPrioPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChPortDefaultUPGet(dev, port, &defaultUserPrioPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", defaultUserPrioPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortModifyUPSet
*
* DESCRIPTION:
*       Enable/Disable overriding a tagged packet's User Priority by
*       other assignment mechanisms.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev         - packet processor device number.
*       upOverrideEnable - GT_TRUE, enable overriding a tagged packet's
*                                   User Priority by other assignment
*                                   mechanisms.
*                          GT_FALSE for disable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - wrong dev or port.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortModifyUPSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_PORT_NUM      port;
    GT_BOOL    upOverrideEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    upOverrideEnable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChPortModifyUPSet(dev, port, upOverrideEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortModifyUPGet
*
* DESCRIPTION:
*       Get Enable/Disable overriding a tagged packet's User Priority by
*       other assignment mechanisms setting.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev         - packet processor device number.
*       upOverrideEnable - GT_TRUE, enable overriding a tagged packet's
*                                   User Priority by other assignment
*                                   mechanisms.
*                          GT_FALSE for disable.
*
* OUTPUTS:
*       upOverrideEnablePtr - GT_TRUE, enable overriding a tagged packet's
*                                   User Priority by other assignment
*                                   mechanisms.
*                          GT_FALSE for disable.
*
* RETURNS:
*       GT_OK          - on success
*       GT_BAD_PARAM   - wrong dev or port.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortModifyUPGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_PORT_NUM      port;
    GT_BOOL    upOverrideEnablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChPortModifyUPGet(dev, port, &upOverrideEnablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", upOverrideEnablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosPortQosTrustModeSet
*
* DESCRIPTION:
*       Configures port's Trust Mode.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev                 - packet processor number.
*       port                - logical port number.
*       portQosTrustMode    - QoS trust mode of a port.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, port or trustMode.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortQosTrustModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           dev;
    GT_PORT_NUM                           port;
    CPSS_QOS_PORT_TRUST_MODE_ENT    portQosTrustMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    portQosTrustMode = (CPSS_QOS_PORT_TRUST_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosPortQosTrustModeSet(dev, port, portQosTrustMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosPortQosTrustModeGet
*
* DESCRIPTION:
*       Get Configured port's Trust Mode.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev                 - packet processor number.
*       port                - logical port number.
*
* OUTPUTS:
*       portQosTrustModePtr  - QoS trust mode of a port.
*
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev, port or trustMode.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortQosTrustModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           dev;
    GT_PORT_NUM                           port;
    CPSS_QOS_PORT_TRUST_MODE_ENT    portQosTrustModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosPortQosTrustModeGet(dev, port, &portQosTrustModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", portQosTrustModePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosPortTrustQosMappingTableIndexSet
*
* DESCRIPTION:
*       Set per ingress port the 'Trust Qos Mapping Table Index' (table selector).
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - ingress port number including CPU port.
*                         In eArch devices portNum is default ePort.
*       useUpAsIndex    - indication if the 'Trust Qos Mapping Table Index' selected
*                           according to the UP (APPLICABLE RANGES : 0..7) of the packet or
*                           according the mappingTableIndex parameter.
*                         GT_TRUE - according to the UP (user priority) of the packet.
*                         GT_FALSE - according the mappingTableIndex parameter.
*       mappingTableIndex  - the  'Trust Qos Mapping Table Index' (table selector).
*                         the value is ignored when useUpAsIndex == GT_TRUE
*                       (APPLICABLE RANGES : 0..11)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - wrong devNum, portNum or up
*       GT_OUT_OF_RANGE    - mappingTableIndex  > 11 and useUpAsIndex == GT_FALSE
*       GT_HW_ERROR        - on HW error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortTrustQosMappingTableIndexSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8            dev;
    GT_PORT_NUM      port;
    GT_BOOL          useUpAsIndex;
    GT_U32           mappingTableIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    useUpAsIndex = (GT_BOOL)inArgs[2];
    mappingTableIndex = (GT_U32)inArgs[3];
    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosPortTrustQosMappingTableIndexSet(dev, port, useUpAsIndex,mappingTableIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssDxChCosPortTrustQosMappingTableIndexGet
*
* DESCRIPTION:
*       Get per ingress port the 'Trust Qos Mapping Table Index' (table selector).
*
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - ingress port number including CPU port.
*                         In eArch devices portNum is default ePort.
*
* OUTPUTS:
*       useUpAsIndexPtr    - indication if the 'Trust Qos Mapping Table Index' selected
*                           according to the UP (APPLICABLE RANGES : 0..7) of the packet or
*                           according the mappingTableIndex parameter.
*                         GT_TRUE - according to the UP (user priority) of the packet.
*                         GT_FALSE - according the (*mappingTableIndexPtr) parameter.
*       mappingTableIndexPtr  - the  'Trust Qos Mapping Table Index' (table selector).
*                         the value is relevant only when (*useUpAsIndexPtr) == GT_FALSE
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - wrong devNum, portNum or up
*       GT_BAD_PTR         - on NULL pointer.
*       GT_HW_ERROR        - on HW error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortTrustQosMappingTableIndexGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           dev;
    GT_PORT_NUM                     port;
    GT_BOOL                         useUpAsIndex;
    GT_U32                          mappingTableIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosPortTrustQosMappingTableIndexGet(dev, port, &useUpAsIndex,&mappingTableIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", useUpAsIndex,mappingTableIndex);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosPortReMapDSCPSet
*
* DESCRIPTION:
*       DiffServ domain boundary.
*       Relevant if the packet’s QoS Profile is assigned according to the packet’s
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev                 - packet processor number.
*       port                - logical port number.
*       enableDscpMutation  - GT_TRUE: Packet's DSCP is remapped.
*                             GT_FALSE: Packet's DSCP is not remapped.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev or port.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortReMapDSCPSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_PORT_NUM      port;
    GT_BOOL    enableDscpMutation;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    enableDscpMutation = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosPortReMapDSCPSet(dev, port, enableDscpMutation);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosPortReMapDSCPGet
*
* DESCRIPTION:
*       DiffServ domain boundary.
*       Relevant if the packet’s QoS Profile is assigned according to the packet’s
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev                 - packet processor number.
*       port                - logical port number.
*
* OUTPUTS:
*       enableDscpMutationPtr- GT_TRUE: Packet's DSCP is remapped.
*                             GT_FALSE: Packet's DSCP is not remapped.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev or port.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortReMapDSCPGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_PORT_NUM      port;
    GT_BOOL    enableDscpMutationPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosPortReMapDSCPGet(dev, port, &enableDscpMutationPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enableDscpMutationPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosProtBasedQosEn
*
* DESCRIPTION:
*       Enables QoS assignment to packets received on this port, according to
*       its Layer2 protocol.
*       This QoS assignment mechanism uses the same mechanism as protocol
*       based VLANs for QoS assignment.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev     - packet processor number.
*       port    - logical port number.
*       enable  - enable/disable QoS assignment.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev or port.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosProtBasedQosEn

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_PORT_NUM      port;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChBrgVlanPortProtoClassQosEnable(dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosPortVlanQoSCfgEntryGet
*
* DESCRIPTION:
*       Read an entry from HW from Ports VLAN and QoS Configuration Table.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*        devNum  - device number
*        index   - pointer to iterate - to get the first set the value to 0.
*
* OUTPUT:
*        entry_PTR  - pointer to memory where will be placed next entry. size 32 Bytes
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM - wrong dev or port.
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortVlanQoSCfgEntryGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_PORT_NUM      port;
    GT_U32     entryArr[4];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosPortVlanQoSCfgEntryGet(dev, port, entryArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", port, entryArr[0]);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosRemarkEntryGet
*
* DESCRIPTION:
*       Read an entry from HW Policers QoS Remarking and Initial DP Table.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*        devNum  - device number
*        index   - pointer to iterate - to get the first set the value to 0.
*
* OUTPUT:
*        entry_PTR  - pointer to memory where will be placed next entry. size 4 Bytes
*
* RETURNS:
*       GT_OK       - on success
*       GT_BAD_PARAM - wrong dev or entry index.
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosRemarkEntryGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_U32     index;
    GT_U32     entryArr[1];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCosRemarkEntryGet(dev, index, entryArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", index, entryArr[0]);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChCosQoSProfileEntryGetNext
*
* DESCRIPTION:
*       Read an entry from HW QoS Profile Table on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*        devNum  - device number
*        index   - pointer to iterate - to get the first set the value to 0.
*
* OUTPUT:
*        entry_PTR  - pointer to memory where will be placed next entry. size 4 Bytes
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - wrong dev or entry index.
*
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosQoSProfileEntryGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_U32     index;
    GT_U32     entryArr[1];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCosQoSProfileEntryGet(dev, index, entryArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", index, entryArr[0]);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosTrustDsaTagQosModeSet
*
* DESCRIPTION:
*       Set trust DSA tag QoS mode. To enable end to end QoS in cascade system,
*       the DSA carries QoS profile assigned to the packet in previous device.
*       When the mode is set to GT_TRUE and the packet's DSA tag contains QoS
*       profile, the QoS profile assigned to the packet is assigned from the
*       DSA tag.
*       Relevant for cascading port only.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev     - packet processor number.
*       port    -  physical port number, CPU port.
*       enable  - trust DSA tag QoS mode.
*                 GT_FALSE - The QoS profile isn't assigned from DSA tag.
*                 GT_TRUE  - If the packets DSA tag contains a QoS profile,
*                     it is assigned to the packet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev or port.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosTrustDsaTagQosModeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_PORT_NUM      port;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosTrustDsaTagQosModeSet(dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosTrustDsaTagQosModeGet
*
* DESCRIPTION:
*       Get trust DSA tag QoS mode. To enable end to end QoS in cascade system,
*       the DSA carries QoS profile assigned to the packet in previous device.
*       When the mode is  GT_TRUE and the packet's DSA tag contains QoS
*       profile, the QoS profile assigned to the packet is assigned from the
*       DSA tag.
*       Relevant for cascading port only.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev        - packet processor number.
*       port       -  physical port number, CPU port.
*
* OUTPUTS:
*       enablePtr  - trust DSA tag QoS mode.
*                    GT_FALSE - The QoS profile isn't assigned from DSA tag.
*                    GT_TRUE  - If the packets DSA tag contains a QoS profile,
*                               it is assigned to the packet.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev or port.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosTrustDsaTagQosModeGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_PORT_NUM      port;
    GT_BOOL    enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    port = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosTrustDsaTagQosModeGet(dev, port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",enablePtr);
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChCosUpCfiDeiToProfileMapSet
*
* DESCRIPTION:
*       Maps the packet's User Priority and CFI/DEI bit (Canonical Format
*       Indicator/Drop Eligibility Indicator) to QoS Profile.
*       The mapping relevant for L2 and L2-L3 QoS Trust modes for 802.1Q
*       or DSA tagged packets.
*
* APPLICABLE DEVICES:   DxCh3 and above.
*
* INPUTS:
*       dev             - packet processor number.
*       up              - User Priority of a VLAN or DSA tagged packet [0..7].
*       cfiDeiBit       - value of CFI/DEI bit[0..1].
*       profileIndex    - QoS Profile index, which is assigned to a VLAN or DSA
*                         tagged packet with the UP on ports with enabled trust
*                         L2 or trust L2-L3. Range is [0..127].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong dev, up or cfiDeiBit.
*       GT_OUT_OF_RANGE - on profileIndex out of range.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosUpCfiDeiToProfileMapSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8  dev;
    GT_U8  up;
    GT_U8  cfiDeiBit;
    GT_U32  profileIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];
    up = (GT_U8)inArgs[1];
    cfiDeiBit = (GT_U8)inArgs[2];
    profileIndex = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChCosUpCfiDeiToProfileMapSet(dev, 0, 0/* backward compatible */ ,up, cfiDeiBit, profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosUpCfiDeiToProfileMapGet
*
* DESCRIPTION:
*       Gets the mapping of packet's User Priority and CFI/DEI bit
*       (Canonical Format Indicator/Drop Eligibility Indicator) to QoS Profile.
*       The mapping relevant for L2 and L2-L3 QoS Trust modes for 802.1Q
*       or DSA tagged packets.
*
* APPLICABLE DEVICES:   DxCh3 and above.
*
* INPUTS:
*       dev             - packet processor number.
*       up              - User Priority of a VLAN or DSA tagged packet [0..7].
*       cfiDeiBit       - value of CFI/DEI bit[0..1].
*
* OUTPUTS:
*       profileIndexPtr - (pointer to)QoS Profile index, which is assigned to a
*                         VLAN or DSA tagged packet with the UP on ports with
*                         enabled trust L2 or trust L2-L3. Range is [0..127].
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong dev, up or cfiDeiBit.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosUpCfiDeiToProfileMapGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     dev;
    GT_U8     up;
    GT_U8     cfiDeiBit;
    GT_U32    profileIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];
    up = (GT_U8)inArgs[1];
    cfiDeiBit = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosUpCfiDeiToProfileMapGet(dev, 0, 0/* backward compatible */ ,up , cfiDeiBit, &profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileIndex);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChCosPortDpToCfiDeiMapEnableSet
*
* DESCRIPTION:
*       Enables or disables mapping of Drop Precedence to Drop Eligibility
*       Indicator bit.
*       When enabled on egress port the DEI(CFI) bit at the outgoing VLAN
*       tag is set according to the packet DP(after mapping).
*
* APPLICABLE DEVICES:   DxCh3 and above.
*
* INPUTS:
*       dev             - packet processor number.
*       port            - egress port number including CPU port.
*       enable          - enable/disable DP-to-CFI/DEI mapping on the
*                         egress port:
*                             GT_TRUE  - DP-to-CFI/DEI mapping enable on port.
*                             GT_FALSE - DP-to-CFI/DEI mapping disable on port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong dev or port.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortDpToCfiDeiMapEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PORT_NUM portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortDpToCfiDeiMapEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortDpToCfiDeiMapEnableGet
*
* DESCRIPTION:
*       Gets status (Enabled/Disabled) of Drop Precedence mapping to
*       Drop Eligibility Indicator bit.
*       When enabled on egress port the DEI(CFI) bit at the outgoing VLAN
*       tag is set according to the packet DP(after mapping).
*
* APPLICABLE DEVICES:   DxCh3 and above.
*
* INPUTS:
*       dev             - packet processor number.
*       port            - egress port number including CPU port.
*
* OUTPUTS:
*       enablePtr       - (pointer to) value of DP-to-CFI/DEI mapping on the
*                         egress port (enable/disable):
*                             GT_TRUE  - DP-to-CFI/DEI mapping enable on port.
*                             GT_FALSE - DP-to-CFI/DEI mapping disable on port.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong dev or port.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortDpToCfiDeiMapEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PORT_NUM portNum;
    GT_BOOL dpToCfiDeiEnable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortDpToCfiDeiMapEnableGet(devNum, portNum, &dpToCfiDeiEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dpToCfiDeiEnable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortDpToCfiDei1MapEnableSet
*
* DESCRIPTION:
*       Enables or disables mapping of Drop Precedence to Drop Eligibility
*       Indicator 1 bit.
*       When enabled on egress port the DEI1(CFI1) bit at the outgoing VLAN
*       tag is set according to the packet DP(after mapping).
*
 APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - egress port number including CPU port.
*       enable          - enable/disable DP-to-CFI1/DEI1 mapping on the
*                         egress port:
*                             GT_TRUE  - DP-to-CFI1/DEI1 mapping enable on port.
*                             GT_FALSE - DP-to-CFI1/DEI1 mapping disable on port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortDpToCfiDei1MapEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PORT_NUM portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortDpToCfiDei1MapEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortDpToCfiDei1MapEnableGet
*
* DESCRIPTION:
*       Gets status (Enabled/Disabled) of Drop Precedence mapping to
*       Drop Eligibility Indicator 1 bit.
*       When enabled on egress port the DEI1(CFI1) bit at the outgoing VLAN
*       tag is set according to the packet DP(after mapping).
*
 APPLICABLE DEVICES:
*        xCat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2; Lion2; Bobcat2; Caelum; Bobcat3.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - egress port number including CPU port.
*
* OUTPUTS:
*       enablePtr       - (pointer to) value of DP-to-CFI1/DEI1 mapping on the
*                         egress port (enable/disable):
*                             GT_TRUE  - DP-to-CFI1/DEI1 mapping enable on port.
*                             GT_FALSE - DP-to-CFI1/DEI1 mapping disable on port.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortDpToCfiDei1MapEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_PORT_NUM portNum;
    GT_BOOL dpToCfiDeiEnable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortDpToCfiDei1MapEnableGet(devNum, portNum, &dpToCfiDeiEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dpToCfiDeiEnable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosDpToCfiDeiMapSet
*
* DESCRIPTION:
*       Maps Drop Precedence to Drop Eligibility Indicator bit.
*       Ports that support S-Tags can be enabled to map the packet's DP
*       (derived from QoS Profile assignment) to the DEI bit(appeared
*       in the outgoing S-Tag) by cpssDxChCosPortDpToCfiDeiMapEnableSet().
*
* APPLICABLE DEVICES:   DxCh3 and above.
*
* INPUTS:
*       dev             - packet processor number.
*       dp              - Drop Precedence [Green, Yellow, Red].
*       cfiDeiBit       - value of CFI/DEI bit[0..1].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_OUT_OF_RANGE - on cfiDeiBit out of range.
*       GT_BAD_PARAM    - on wrong dev or dp.
*
* COMMENTS:
*       Three DP levels are mapped to only two values of CFI/DEI bit.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosDpToCfiDeiMapSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 dev;
    CPSS_DP_LEVEL_ENT dp;
    GT_U8 cfiDeiBit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];
    dp = (CPSS_DP_LEVEL_ENT)inArgs[1];
    cfiDeiBit = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosDpToCfiDeiMapSet(dev, dp, cfiDeiBit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosDpToCfiDeiMapGet
*
* DESCRIPTION:
*       Gets Drop Precedence mapping to Drop Eligibility Indicator bit.
*       Ports that support S-Tags can be enabled to map the packet's DP
*       (derived from QoS Profile assignment) to the DEI bit(appeared
*       in the outgoing S-Tag) by cpssDxChCosPortDpToCfiDeiMapEnableSet().
*
* APPLICABLE DEVICES:   DxCh3 and above.
*
* INPUTS:
*       dev             - packet processor number.
*       dp              - Drop Precedence [Green, Yellow, Red].
*
* OUTPUTS:
*       cfiDeiBitPtr    - (pointer to) value of CFI/DEI bit[0..1].
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong dev or dp.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       Three DP levels are mapped to only two values of CFI/DEI bit.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosDpToCfiDeiMapGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 dev;
    CPSS_DP_LEVEL_ENT dp;
    GT_U8 cfiDeiBit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    dev = (GT_U8)inArgs[0];
    dp = (CPSS_DP_LEVEL_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChCosDpToCfiDeiMapGet(dev, dp, &cfiDeiBit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cfiDeiBit);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosTrustExpModeSet
*
* DESCRIPTION:
*       Enable/Disable trust EXP mode for MPLS packets.
*
* APPLICABLE DEVICES:  DxChXCat and above devices.
*
* INPUTS:
*       devNum  - device number.
*       port    - port number, CPU port.
*       enable  - trust the MPLS header's EXP field.
*                 GT_FALSE - QoS profile from Exp To Qos Profile table
*                            not assigned to the MPLS packet.
*                 GT_TRUE  - QoS profile from Exp To Qos Profile table
*                            assigned to the MPLS packet.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosTrustExpModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     dev;
    GT_PORT_NUM port;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev    = (GT_U8)inArgs[0];
    port   = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosTrustExpModeSet(dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosTrustExpModeGet
*
* DESCRIPTION:
*       Get Enable/Disable state of trust EXP mode for MPLS packets.
*
* APPLICABLE DEVICES:  DxChXCat and above devices.
*
*
* INPUTS:
*       devNum     - device number.
*       port       - port number, CPU port.
*
* OUTPUTS:
*       enablePtr  - (pointer to) trust the MPLS header's EXP field.
*                    GT_FALSE - QoS profile from Exp To Qos Profile table
*                               not assigned to the MPLS packet.
*                    GT_TRUE  - QoS profile from Exp To Qos Profile table
*                               assigned to the MPLS packet.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_TIMEOUT               - after max number of retries checking if PP ready.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosTrustExpModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     dev;
    GT_PORT_NUM     port;
    GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev    = (GT_U8)inArgs[0];
    port   = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosTrustExpModeGet(dev, port, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosL2TrustModeVlanTagSelectSet
*
* DESCRIPTION:
*       Select type of VLAN tag (either Tag0 or Tag1) for Trust L2 Qos mode.
*
* APPLICABLE DEVICES:  DxChXCat and above devices.
*
* INPUTS:
*       devNum          - device number.
*       port            - port number, CPU port.
*       vlanTagType     - Vlan Tag Type (tag0 or tag1)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port or vlanTagType.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosL2TrustModeVlanTagSelectSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;
    GT_U8                     dev;
    GT_U32                    port;
    CPSS_VLAN_TAG_TYPE_ENT    vlanTagType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev         = (GT_U8)inArgs[0];
    port        = (GT_PORT_NUM)inArgs[1];
    vlanTagType = (CPSS_VLAN_TAG_TYPE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosL2TrustModeVlanTagSelectSet(dev, port, GT_TRUE,vlanTagType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosL2TrustModeVlanTagSelectGet
*
* DESCRIPTION:
*       Get Selected type of VLAN tag (either Tag0 or Tag1) for Trust L2 Qos mode.
*
* APPLICABLE DEVICES:  DxChXCat and above devices.
*
*
* INPUTS:
*       devNum     - device number.
*       port       - port number, CPU port.
*
* OUTPUTS:
*       vlanTagTypePtr  - (pointer to)Vlan Tag Type (tag0 or tag1)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosL2TrustModeVlanTagSelectGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;
    GT_U8                     dev;
    GT_PORT_NUM                     port;
    CPSS_VLAN_TAG_TYPE_ENT    vlanTagType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev    = (GT_U8)inArgs[0];
    port   = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosL2TrustModeVlanTagSelectGet(dev, port, GT_TRUE,&vlanTagType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vlanTagType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosL2TrustModeVlanTagSelectSet
*
* DESCRIPTION:
*       Select type of VLAN tag (either Tag0 or Tag1) for Trust L2 Qos mode.
*
* APPLICABLE DEVICES:  DxChXCat and above devices.
*
* INPUTS:
*       devNum          - device number.
*       port            - port number, CPU port.
*       vlanTagType     - Vlan Tag Type (tag0 or tag1)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port or vlanTagType.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosL2TrustModeVlanTagSelectSet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;
    GT_U8                     dev;
    GT_PORT_NUM               port;
    CPSS_VLAN_TAG_TYPE_ENT    vlanTagType;
    GT_BOOL                   isDefaultVlanTagType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev         = (GT_U8)inArgs[0];
    port        = (GT_PORT_NUM)inArgs[1];
    isDefaultVlanTagType = (GT_BOOL)inArgs[2];
    vlanTagType = (CPSS_VLAN_TAG_TYPE_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosL2TrustModeVlanTagSelectSet(dev, port, isDefaultVlanTagType,vlanTagType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosL2TrustModeVlanTagSelectGet
*
* DESCRIPTION:
*       Get Selected type of VLAN tag (either Tag0 or Tag1) for Trust L2 Qos mode.
*
* APPLICABLE DEVICES:  DxChXCat and above devices.
*
*
* INPUTS:
*       devNum     - device number.
*       port       - port number, CPU port.
*
* OUTPUTS:
*       vlanTagTypePtr  - (pointer to)Vlan Tag Type (tag0 or tag1)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - wrong devNum or port.
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosL2TrustModeVlanTagSelectGet_1
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;
    GT_U8                     dev;
    GT_PORT_NUM                     port;
    CPSS_VLAN_TAG_TYPE_ENT    vlanTagType;
    GT_BOOL                   isDefaultVlanTagType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev    = (GT_U8)inArgs[0];
    port   = (GT_PORT_NUM)inArgs[1];
    isDefaultVlanTagType = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, port);

    /* call cpss api function */
    result = cpssDxChCosL2TrustModeVlanTagSelectGet(dev, port, isDefaultVlanTagType,&vlanTagType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vlanTagType);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortUpProfileIndexSet
*
* DESCRIPTION:
*       Set per ingress port and per UP (user priority) the 'UP profile index' (table selector).
*       See also API cpssDxChCosUpCfiDeiToProfileMapSet
*
* APPLICABLE DEVICES:   Lion and above.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - ingress port number including CPU port.
*       up              - user priority of a VLAN or DSA tagged packet [0..7].
*       upProfileIndex  - the UP profile index (table selector) , value 0..1
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - wrong devNum, portNum or up
*       GT_OUT_OF_RANGE    - upProfileIndex  > 1
*       GT_HW_ERROR        - on HW error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortUpProfileIndexSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8                 dev;
    GT_PHYSICAL_PORT_NUM                 portNum;
    GT_U8                 up;
    GT_U32                upProfileIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    portNum = (GT_PHYSICAL_PORT_NUM)inFields[0];

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(dev, portNum);

    for (up = 0; up < 8; up++)
    {
        /* index for UP0 starts from field #1 */
        upProfileIndex = (GT_U32)inFields[1 + up];

        /* call cpss api function */
        result = cpssDxChCosPortUpProfileIndexSet(dev, portNum,up, upProfileIndex);
        if (result != GT_OK)
        {
            break;
        }
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

static GT_U8    wrUpProfileIndex_portNum = 0;/* port number iterator for
    'get next' of table cpssDxChCosPortUpProfileIndex */
/*******************************************************************************
* cpssDxChCosPortUpProfileIndexGet
*
* DESCRIPTION:
*       Get per ingress port and per UP (user priority) the 'UP profile index' (table selector).
*       See also API cpssDxChCosUpCfiDeiToProfileMapSet
*
* APPLICABLE DEVICES:   Lion and above.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - ingress port number including CPU port.
*       up              - user priority of a VLAN or DSA tagged packet [0..7].
*
* OUTPUTS:
*       upProfileIndexPtr - (pointer to)the UP profile index (table selector) , value 0..1
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum, portNum or up.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS internalCpssDxChCosPortUpProfileIndexGet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result;
    GT_U8                 dev,origDev;
    GT_U32                upProfileIndex[CPSS_USER_PRIORITY_RANGE_CNS];
    GT_PHYSICAL_PORT_NUM  portNum;
    GT_U8                 up;
    GT_BOOL               skip;
    GT_PHYSICAL_PORT_NUM  port;
    GT_U8                 tmpDev;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    origDev = dev;

    portNum = 0;/* for device mapping */
    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(dev, portNum);

    if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(dev))
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d", -1);
        return CMD_OK;
    }

    if(wrUpProfileIndex_portNum > CPSS_CPU_PORT_NUM_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }


    /* look for the next phy port */
    while((wrUpProfileIndex_portNum < CPSS_CPU_PORT_NUM_CNS) &&
          (0 == PRV_CPSS_PHY_PORT_IS_EXIST_MAC(dev, wrUpProfileIndex_portNum)))
    {
        wrUpProfileIndex_portNum++;
    }

    /* find if current port has converted pair.
     e.g. using port convertion
      0   0
      8  17
      18 34
      23 54
      need to filter out ports 17,34 and 54 from refresh output */
    skip = GT_FALSE;
    for (port = 0; port < CPSS_CPU_PORT_NUM_CNS; port++)
    {
        tmpDev = origDev;
        portNum = port;
        CONVERT_DEV_PHYSICAL_PORT_MAC(tmpDev, portNum);
        if ((tmpDev == dev) && (portNum == wrUpProfileIndex_portNum) &&
            /* check that either device or port were converted */
            ((tmpDev != origDev) || (portNum != port)))
        {
            /* found port that converted to current wrUpProfileIndex_portNum.
             need to skip it. */
            skip = GT_TRUE;
            break;
        }
    }

    portNum = (GT_U8)(wrUpProfileIndex_portNum);

    /* Override Device and Port */
    CONVERT_DEV_PHYSICAL_PORT_MAC(origDev, portNum);

    dev = origDev;

    for(up = 0 ; up < CPSS_USER_PRIORITY_RANGE_CNS ; up++)
    {
        if (skip == GT_FALSE)
        {
            /* call cpss api function */
            result = cpssDxChCosPortUpProfileIndexGet(dev, portNum, up ,  &upProfileIndex[up]);

            if (result != GT_OK)
            {
                galtisOutput(outArgs, result, "%d", -1);
                return CMD_OK;
            }

        }
        else/* use default value 0 for skipped ports */
            upProfileIndex[up] = 0;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d",
                wrUpProfileIndex_portNum,
                upProfileIndex[0],
                upProfileIndex[1],
                upProfileIndex[2],
                upProfileIndex[3],
                upProfileIndex[4],
                upProfileIndex[5],
                upProfileIndex[6],
                upProfileIndex[7]
                );

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

static CMD_STATUS wrCpssDxChCosPortUpProfileIndexGetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    wrUpProfileIndex_portNum = 0;

    return internalCpssDxChCosPortUpProfileIndexGet(inArgs,inFields,numFields,outArgs);
}


/*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortUpProfileIndexGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    wrUpProfileIndex_portNum++;

    return internalCpssDxChCosPortUpProfileIndexGet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChCosUpCfiDeiToProfileMapSet
*
* DESCRIPTION:
*       Maps the UP Profile Index and packet's User Priority and CFI/DEI bit (Canonical Format
*       Indicator/Drop Eligibility Indicator) to QoS Profile.
*       The mapping relevant for L2 and L2-L3 QoS Trust modes for 802.1Q
*       or DSA tagged packets.
*
* APPLICABLE DEVICES:   All DxCh Devices.
*
* INPUTS:
*       devNum          - device number.
*       upProfileIndex - the UP profile index (table selector)
*                        this parameter not relevant for ch1,2,3,xcat.
*                       see also API : cpssDxChCosPortUpProfileIndexSet
*       up              - User Priority of a VLAN or DSA tagged packet [0..7].
*       cfiDeiBit       - value of CFI/DEI bit[0..1].
*                         this parameter not relevant for ch1,2.
*       profileIndex    - QoS Profile index, which is assigned to a VLAN or DSA
*                         tagged packet with the UP on ports with enabled trust
*                         L2 or trust L2-L3. Range is [0..127].(for ch1 range is [0..71])
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum, up or cfiDeiBit.
*       GT_OUT_OF_RANGE - on profileIndex out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS internalWrCpssDxChCosUpCfiDeiToProfileMap1Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL   useMappingTableIndex
)
{
    GT_STATUS             result;

    GT_U8                 dev;
    GT_U8                 up,cfi;
    GT_U32                upProfileIndex,qosProfileIndex;
    GT_U32                mappingTableIndex = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    upProfileIndex = (GT_U32)inArgs[1];
    if (useMappingTableIndex)
    {
        mappingTableIndex = (GT_U32)inArgs[1];
        upProfileIndex    = (GT_U32)inArgs[2];
    }

    up  = (GT_U8)inFields[0];
    cfi = (GT_U8)inFields[1];
    qosProfileIndex = (GT_U32)inFields[2];

    /* call cpss api function */
    result = cpssDxChCosUpCfiDeiToProfileMapSet(dev ,mappingTableIndex, upProfileIndex ,up ,cfi ,qosProfileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosUpCfiDeiToProfileMapSet
*
* DESCRIPTION:
*       Maps the UP Profile Index and packet's User Priority and CFI/DEI bit (Canonical Format
*       Indicator/Drop Eligibility Indicator) to QoS Profile.
*       The mapping relevant for L2 and L2-L3 QoS Trust modes for 802.1Q
*       or DSA tagged packets.
*
* APPLICABLE DEVICES:   All DxCh Devices.
*
* INPUTS:
*       devNum          - device number.
*       upProfileIndex - the UP profile index (table selector)
*                        this parameter not relevant for ch1,2,3,xcat.
*                       see also API : cpssDxChCosPortUpProfileIndexSet
*       up              - User Priority of a VLAN or DSA tagged packet [0..7].
*       cfiDeiBit       - value of CFI/DEI bit[0..1].
*                         this parameter not relevant for ch1,2.
*       profileIndex    - QoS Profile index, which is assigned to a VLAN or DSA
*                         tagged packet with the UP on ports with enabled trust
*                         L2 or trust L2-L3. Range is [0..127].(for ch1 range is [0..71])
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum, up or cfiDeiBit.
*       GT_OUT_OF_RANGE - on profileIndex out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosUpCfiDeiToProfileMap1Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalWrCpssDxChCosUpCfiDeiToProfileMap1Set(inArgs,inFields,numFields,outArgs,GT_FALSE);
}

/*******************************************************************************
* cpssDxChCosUpCfiDeiToProfileMapSet
*
* DESCRIPTION:
*       Maps the UP Profile Index and packet's User Priority and CFI/DEI bit (Canonical Format
*       Indicator/Drop Eligibility Indicator) to QoS Profile.
*       The mapping relevant for L2 and L2-L3 QoS Trust modes for 802.1Q
*       or DSA tagged packets.
*
* APPLICABLE DEVICES:   All DxCh Devices.
*
* INPUTS:
*       devNum          - device number.
*       upProfileIndex - the UP profile index (table selector)
*                        this parameter not relevant for ch1,2,3,xcat.
*                       see also API : cpssDxChCosPortUpProfileIndexSet
*       up              - User Priority of a VLAN or DSA tagged packet [0..7].
*       cfiDeiBit       - value of CFI/DEI bit[0..1].
*                         this parameter not relevant for ch1,2.
*       profileIndex    - QoS Profile index, which is assigned to a VLAN or DSA
*                         tagged packet with the UP on ports with enabled trust
*                         L2 or trust L2-L3. Range is [0..127].(for ch1 range is [0..71])
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum, up or cfiDeiBit.
*       GT_OUT_OF_RANGE - on profileIndex out of range.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosUpCfiDeiToProfileMap2Set
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalWrCpssDxChCosUpCfiDeiToProfileMap1Set(inArgs,inFields,numFields,outArgs,GT_TRUE);
}


static GT_U8    wrUpCfiDei_up  = 0;/* up iterator for
    'get next' of table cpssDxChCosUpCfiDeiToProfileMap1*/
static GT_U8    wrUpCfiDei_cfi = 0;/* cfi iterator for
    'get next' of table cpssDxChCosUpCfiDeiToProfileMap1*/
/*******************************************************************************
* cpssDxChCosUpCfiDeiToProfileMapGet
*
* DESCRIPTION:
*       Get the Map from UP Profile Index and packet's User Priority and CFI/DEI bit (Canonical Format
*       Indicator/Drop Eligibility Indicator) to QoS Profile.
*       The mapping relevant for L2 and L2-L3 QoS Trust modes for 802.1Q
*       or DSA tagged packets.
*
* APPLICABLE DEVICES:   All DxCh Devices.
*
* INPUTS:
*       devNum          - device number.
*       upProfileIndex - the UP profile index (table selector)
*                        this parameter not relevant for ch1,2,3,xcat.
*                       see also API : cpssDxChCosPortUpProfileIndexSet
*       up              - User Priority of a VLAN or DSA tagged packet [0..7].
*       cfiDeiBit       - value of CFI/DEI bit[0..1].
*                         this parameter not relevant for ch1,2.
*
* OUTPUTS:
*       profileIndexPtr - (pointer to)QoS Profile index, which is assigned to a
*                         VLAN or DSA tagged packet with the UP on ports with
*                         enabled trust L2 or trust L2-L3. Range is [0..127]. (for ch1 range is [0..71])
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum, up or cfiDeiBit.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS internalCpssDxChCosUpCfiDeiToProfileMap1Get

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL   useMappingTableIndex
)
{
    GT_STATUS             result;
    GT_U8                 dev;
    GT_U32                upProfileIndex;
    GT_U32                qosProfileIndex;
    GT_U8                 up,cfi;
    GT_U32                mappingTableIndex = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    upProfileIndex = (GT_U32)inArgs[1];
    if (useMappingTableIndex)
    {
        mappingTableIndex = (GT_U32)inArgs[1];
        upProfileIndex    = (GT_U32)inArgs[2];
    }

    if(wrUpCfiDei_cfi >= 2)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    up  = wrUpCfiDei_up;
    cfi = wrUpCfiDei_cfi;
    wrUpCfiDei_up++;

    if(wrUpCfiDei_up == CPSS_USER_PRIORITY_RANGE_CNS)
    {
        wrUpCfiDei_up = 0;
        wrUpCfiDei_cfi++;
    }

    /* call cpss api function */
    result = cpssDxChCosUpCfiDeiToProfileMapGet(dev, mappingTableIndex, upProfileIndex, up ,cfi, &qosProfileIndex);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d",
                up,
                cfi,
                qosProfileIndex
                );

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

static CMD_STATUS wrCpssDxChCosUpCfiDeiToProfileMap1GetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    wrUpCfiDei_up  = 0;
    wrUpCfiDei_cfi = 0;

    return internalCpssDxChCosUpCfiDeiToProfileMap1Get(inArgs,inFields,numFields,outArgs, GT_FALSE);
}


/*******************************************************************************/
static CMD_STATUS wrCpssDxChCosUpCfiDeiToProfileMap1GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalCpssDxChCosUpCfiDeiToProfileMap1Get(inArgs,inFields,numFields,outArgs, GT_FALSE);
}

static CMD_STATUS wrCpssDxChCosUpCfiDeiToProfileMap2GetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    wrUpCfiDei_up  = 0;
    wrUpCfiDei_cfi = 0;

    return internalCpssDxChCosUpCfiDeiToProfileMap1Get(inArgs,inFields,numFields,outArgs, GT_TRUE);
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChCosUpCfiDeiToProfileMap2GetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return internalCpssDxChCosUpCfiDeiToProfileMap1Get(inArgs,inFields,numFields,outArgs, GT_TRUE);
}

static GT_U32 maxProfileCnt;

/*******************************************************************************
* wrCpssDxChCosProfileEntriesRangeGetFirst
*
* DESCRIPTION:
*       Get Profile Table Entry configuration.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev           - packet processor number.
*       profileIndex  - index of a profile in the profile table. Values 0 - 71
*
* OUTPUTS:
*       dscpPtr       - (pointer to) DSCP value for packet with assign profile equal
*                       profileIndex. The new DSCP is changed in the packet only
*                       if 'modify DSCP flag' is set in the packet descriptor
*       cosPtr        - Pointer to new CoS values for packet - TC, UP and DP
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev,dscp or cos.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosProfileEntriesRangeGetFirst

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                            dev;
    GT_U32                           minIndex;
    GT_U32                           maxIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    minIndex = (GT_U32)inArgs[1];
    maxIndex = (GT_U32)inArgs[2];

    profileCnt = minIndex;
    maxProfileCnt = maxIndex;

    if((profileCnt >= (GT_U32)PRV_CPSS_DXCH_QOS_PROFILE_MAX_MAC(dev)) || (maxIndex < minIndex))
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    return wrCpssDxChCosProfileEntryGet(dev, inFields, outArgs);
}


/*******************************************************************************/
static CMD_STATUS wrCpssDxChCosProfileEntriesRangeGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                            dev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    profileCnt++;

    if((profileCnt >= maxProfileCnt) ||
       (profileCnt >= (GT_U32)PRV_CPSS_DXCH_QOS_PROFILE_MAX_MAC(dev)))
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    return wrCpssDxChCosProfileEntryGet(dev, inFields, outArgs);
}

/*******************************************************************************
* cpssDxChCosProfileEntrySet
*
* DESCRIPTION:
*       Configures the Profile Table Entry.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev           - packet processor number.
*       profileIndex  - index of a profile in the profile table. Values 0 - 71
*       dscp          - new DSCP value for packet with assign profile equal
*                       profileIndex. The new DSCP is changed in the packet only
*                       if 'modify DSCP flag' is set in the packet descriptor
*       cos           - Pointer to new CoS values for packet - TC, UP and DP
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong dev,dscp or cos.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosProfileEntriesRangeSet

(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChCosProfileEntrySet(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChCosPortEgressQosMappingTableIndexSet
*
* DESCRIPTION:
*       Set the table set index for egress port QoS remapping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number.
*       portNum            - egress port number.
*       mappingTableIndex  - Egress Qos Mapping Table Index (table set selector).
*                            (APPLICABLE RANGES : 0..11).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - wrong devNum, portNum or mappingTableIndex
*       GT_HW_ERROR        - on HW error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortEgressQoSMappingTableIndexSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8                 dev;
    GT_PORT_NUM           portNum;
    GT_U32                mappingTableIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    mappingTableIndex = (GT_PORT_NUM)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortEgressQosMappingTableIndexSet(dev, portNum, mappingTableIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortEgressQosMappingTableIndexGet
*
* DESCRIPTION:
*       Get the table set index for egress port QoS remapping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum             - device number.
*       portNum            - egress port number.
*
* OUTPUTS:
*       mappingTableIndexPtr  - pointer to Egress Qos Mapping Table Index (table set selector).
*
*
* RETURNS:
*       GT_OK              - on success.
*       GT_BAD_PARAM       - wrong devNum, portNum or mappingTableIndex
*       GT_HW_ERROR        - on HW error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortEgressQoSMappingTableIndexGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8                 dev;
    GT_PORT_NUM           portNum;
    GT_U32                mappingTableIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortEgressQosMappingTableIndexGet(dev, portNum, &mappingTableIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mappingTableIndex);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortEgressQosExpMappingEnableSet
*
* DESCRIPTION:
*       Enable/disable Egress port EXP Mapping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - egress port number.
*       enable          - enable/disable Egress port EXP Mapping.
*                         GT_TRUE  - enable mapping.
*                         GT_FALSE - disable mapping.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortEgressQoSExpMappingEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8                 dev;
    GT_PORT_NUM           portNum;
    GT_BOOL               enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortEgressQosExpMappingEnableSet(dev, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortEgressQosExpMappingEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of Egress port EXP Mapping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - egress port number.
*
* OUTPUTS:
*       enablePtr        - pointer to enable/disable Egress EXP Mapping status on the egress port.
*                         GT_TRUE  -  mapping is enabled.
*                         GT_FALSE -  mapping is disabled.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortEgressQoSExpMappingEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;
    GT_U8                 dev;
    GT_PORT_NUM           portNum;
    GT_BOOL               enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortEgressQosExpMappingEnableGet(dev, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortEgressQosTcDpMappingEnableSet
*
* DESCRIPTION:
*       Enable/disable Egress port {TC, DP} mapping to {UP,EXP,DSCP}.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - egress port number.
*       enable          - enable/disable Egress port {TC, DP} mapping .
*                         GT_TRUE  - enable mapping.
*                         GT_FALSE - disable mapping.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortEgressQoSTcDpMappingEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8                 dev;
    GT_PORT_NUM           portNum;
    GT_BOOL               enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortEgressQosTcDpMappingEnableSet(dev, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortEgressQosTcDpMappingEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of Egress port {TC, DP} mapping to {UP,EXP,DSCP}.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - egress port number.
*
* OUTPUTS:
*       enablePtr        - pointer to enable/disable Egress port {TC, DP} mapping status to {UP,EXP,DSCP} .
*                         GT_TRUE  -   mapping  is enabled.
*                         GT_FALSE -  mapping is disabled.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortEgressQoSTcDpMappingEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;
    GT_U8                 dev;
    GT_PORT_NUM           portNum;
    GT_BOOL               enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortEgressQosTcDpMappingEnableGet(dev, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortEgressQosUpMappingEnableSet

* DESCRIPTION:
*       Enable/disable Egress port UP Mapping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - egress port number.
*       enable          - enable/disable Egress port UP Mapping.
*                         GT_TRUE  - enable mapping.
*                         GT_FALSE - disable mapping.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortEgressQoSUpMappingEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8                 dev;
    GT_PORT_NUM           portNum;
    GT_BOOL               enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortEgressQosUpMappingEnableSet(dev, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortEgressQosUpMappingEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of Egress UP Mapping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - egress port number.
*
* OUTPUTS:
*       enablePtr        - pointer to enable/disable Egress UP Mapping status.
*                         GT_TRUE  -  mapping is enabled.
*                         GT_FALSE -  mapping is disabled.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortEgressQoSUpMappingEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8                 dev;
    GT_PORT_NUM           portNum;
    GT_BOOL               enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortEgressQosUpMappingEnableGet(dev, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortEgressQoSDscpMappingEnableSet
*
* DESCRIPTION:
*       Enable/disable Egress port DSCP Mapping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - egress port number.
*       enable          - enable/disable Egress port DSCP Mapping.
*                         GT_TRUE  - enable mapping.
*                         GT_FALSE - disable mapping.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortEgressQoSDscpMappingEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8                 dev;
    GT_PORT_NUM           portNum;
    GT_BOOL               enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortEgressQosDscpMappingEnableSet(dev, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosPortEgressQosDscpMappingEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of Egress Port DSCP Mapping.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum          - device number.
*       portNum         - egress port number.
*
* OUTPUTS:
*       enablePtr        - pointer to enable/disable Egress port DSCP Mapping status.
*                         GT_TRUE  -  mapping is enabled.
*                         GT_FALSE -  mappingis disabled.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or port.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosPortEgressQoSDscpMappingEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8                 dev;
    GT_PORT_NUM           portNum;
    GT_BOOL               enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_PORT_NUM)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_U32_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChCosPortEgressQosDscpMappingEnableGet(dev, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosEgressDscp2DscpMappingEntrySet
*
* DESCRIPTION:
*       Egress mapping of current DSCP to a new DSCP value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number.
*       mappingTableIndex - egress mapping table index (see API cpssDxChCosPortEgressQoSMappingTableIndexSet)
*                           (APPLICABLE RANGES : 0..11)
*       dscp              - current DSCP  (APPLICABLE RANGES: 0..63).
*       newDscp           - new DSCP assigned to packet(APPLICABLE RANGES: 0..63).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, or exp, or newExp, or mappingTableIndex .
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosEgressDscp2DscpMappingEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8   dev;
    GT_U32  mappingTableIndex;
    GT_U32  dscp;
    GT_U32  newDscp;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    dscp = (GT_U32)inArgs[2];
    newDscp = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChCosEgressDscp2DscpMappingEntrySet(dev, mappingTableIndex, dscp, newDscp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosEgressDscp2DscpMappingEntryGet
*
* DESCRIPTION:
*       Get egress mapped packet DSCP value for current dscp.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number.
*       mappingTableIndex - egress mapping table index (see API cpssDxChCosPortEgressQoSMappingTableIndexSet)
*                           (APPLICABLE RANGES : 0..11)
*       dscp              - current  DSCP  (APPLICABLE RANGES: 0..63).
*
* OUTPUTS:
*       newDscpPtr        - pointer to mapped DSCP value.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, or exp, or newExp, or mappingTableIndex .
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosEgressDscp2DscpMappingEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8   dev;
    GT_U32  mappingTableIndex;
    GT_U32  dscp;
    GT_U32  newDscp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    dscp = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosEgressDscp2DscpMappingEntryGet(dev, mappingTableIndex, dscp, &newDscp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", newDscp);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosEgressExp2ExpMappingEntrySet
*
* DESCRIPTION:
*       Egress mapping of current packet Exp to a new Exp value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number.
*       mappingTableIndex - egress mapping table index (see API cpssDxChCosPortEgressQoSMappingTableIndexSet)
*                           (APPLICABLE RANGES : 0..11)
*       exp               - current exp (APPLICABLE RANGES: 0..7).
*       newExp            - new Exp assigned to packet (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, or exp, or newExp, or mappingTableIndex .
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosEgressExp2ExpMappingEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8   dev;
    GT_U32  mappingTableIndex;
    GT_U32  exp;
    GT_U32  newExp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    exp = (GT_U32)inArgs[2];
    newExp = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChCosEgressExp2ExpMappingEntrySet(dev, mappingTableIndex, exp, newExp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosEgressExp2ExpMappingEntryGet
*
* DESCRIPTION:
*       Get egress mapped packet exp value for current exp.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number.
*       mappingTableIndex - egress mapping table index (see API cpssDxChCosPortEgressQoSMappingTableIndexSet)
*                           (APPLICABLE RANGES : 0..11)
*       exp               - current packet exp   (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       newExpPtr        - pointer to mapped Exp value.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, or exp, or newExp, or mappingTableIndex .
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosEgressExp2ExpMappingEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8   dev;
    GT_U32  mappingTableIndex;
    GT_U32  exp;
    GT_U32  newExp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    exp = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosEgressExp2ExpMappingEntryGet(dev, mappingTableIndex, exp, &newExp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", newExp);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosEgressUp2UpMappingEntrySet
*
* DESCRIPTION:
*       Egress mapping of current packet up to a new up value.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number.
*       mappingTableIndex - egress mapping table index (see API cpssDxChCosPortEgressQoSMappingTableIndexSet)
*                           (APPLICABLE RANGES : 0..11)
*       up                - current packet up   (APPLICABLE RANGES: 0..7).
*       newUp             - new Up assigned to packet (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, or up, or newUp, or mappingTableIndex .
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosEgressUp2UpMappingEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8   dev;
    GT_U32  mappingTableIndex;
    GT_U32  up;
    GT_U32  newUp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    up = (GT_U32)inArgs[2];
    newUp = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChCosEgressUp2UpMappingEntrySet(dev, mappingTableIndex, up, newUp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosEgressUp2UpMappingEntryGet
*
* DESCRIPTION:
*       Get egress mapped packet Up value for current Up.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number.
*       mappingTableIndex - egress mapping table index (see API cpssDxChCosPortEgressQoSMappingTableIndexSet)
*                           (APPLICABLE RANGES : 0..11)
*       up                - current packet up   (APPLICABLE RANGES: 0..7).
*
* OUTPUTS:
*       newUpPtr          - pointer to mapped up value.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, or up, or newUp, or mappingTableIndex .
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosEgressUp2UpMappingEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8   dev;
    GT_U32  mappingTableIndex;
    GT_U32  up;
    GT_U32  newUp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    up = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChCosEgressUp2UpMappingEntryGet(dev, mappingTableIndex, up, &newUp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", newUp);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosEgressTcDp2UpExpDscpMappingEntrySet
*
* DESCRIPTION:
*      Set Egress {TC, DP} mapping to {UP,EXP,DSCP}.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number.
*       mappingTableIndex - egress mapping table index (see API cpssDxChCosPortEgressQoSMappingTableIndexSet)
*                           (APPLICABLE RANGES : 0..11).
*       tc                - traffic class assigned to packet  (APPLICABLE RANGES: 0..7).
*       dp                - drop precedence (APPLICABLE RANGES: 0..2).
*       up                - up value (APPLICABLE RANGES: 0..7).
*       exp               - exp value (APPLICABLE RANGES: 0..7).
*       dscp              - dscp value (APPLICABLE RANGES: 0..63).
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, or tc,dp, or up,exp,dscp or mappingTableIndex .
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosEgressTcDp2UpExpDscpMappingEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS             result = GT_OK;

    GT_U8              dev;
    GT_U32             mappingTableIndex;
    GT_U32             tc;
    CPSS_DP_LEVEL_ENT  dp;
    GT_U32             up;
    GT_U32             exp;
    GT_U32             dscp;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    tc = (GT_U32)inArgs[2];
    dp = (CPSS_DP_LEVEL_ENT)inArgs[3];
    up = (GT_U32)inArgs[4];
    exp = (GT_U32)inArgs[5];
    dscp = (CPSS_DP_LEVEL_ENT)inArgs[6];


    /* call cpss api function */
    result = cpssDxChCosEgressTcDp2UpExpDscpMappingEntrySet(dev, mappingTableIndex, tc, dp, up, exp, dscp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChCosEgressTcDp2UpExpDscpMappingEntryGet
*
* DESCRIPTION:
*      Get Egress up,exp,dscp values mapped for current {tc, dp}.
*
* APPLICABLE DEVICES:
*        Bobcat2; Caelum; Bobcat3.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat3; Lion; xCat2; Lion2.
*
* INPUTS:
*       devNum            - device number.
*       mappingTableIndex - egress mapping table index (see API cpssDxChCosPortEgressQoSMappingTableIndexSet)
*                           (APPLICABLE RANGES : 0..11).
*       tc                - traffic class assigned to packet  (APPLICABLE RANGES: 0..7).
*       dp                - drop precedence (APPLICABLE RANGES: 0..2).
*
* OUTPUTS:
*       upPtr             - pointer to up value.
*       expPtr            - pointer to exp value.
*       dscpPtr           - pointer to dscp value.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_BAD_PARAM             - on wrong devNum, or tc, or dp, or mappingTableIndex .
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChCosEgressTcDp2UpExpDscpMappingEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS          result = GT_OK;

    GT_U8              dev;
    GT_U32             mappingTableIndex;
    GT_U32             tc;
    CPSS_DP_LEVEL_ENT  dp;
    GT_U32             up;
    GT_U32             exp;
    GT_U32             dscp;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    mappingTableIndex = (GT_U32)inArgs[1];
    tc = (GT_U32)inArgs[2];
    dp = (CPSS_DP_LEVEL_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChCosEgressTcDp2UpExpDscpMappingEntryGet(dev, mappingTableIndex, tc, dp, &up, &exp, &dscp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", up, exp, dscp);
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChCosProfileEntrySet",
        &wrCpssDxChCosProfileEntrySet,
        1, 6},

    {"cpssDxChCosProfileEntryGetFirst",
        &wrCpssDxChCosProfileEntryGetFirst,
        1, 0},

    {"cpssDxChCosProfileEntryGetNext",
        &wrCpssDxChCosProfileEntryGetNext,
        1, 0},

    {"cpssDxChCosDscpToProfileMapSet",
        &wrCpssDxChCosDscpToProfileMapSet,
        3, 0},

    {"cpssDxChCosDscpToProfileMapGet",
        &wrCpssDxChCosDscpToProfileMapGet,
        2, 0},

    {"cpssDxChCosDscpToProfileMapSet_1",
        &wrCpssDxChCosDscpToProfileMapSet_1,
        4, 0},

    {"cpssDxChCosDscpToProfileMapGet_1",
        &wrCpssDxChCosDscpToProfileMapGet_1,
        3, 0},

    {"cpssDxChCosDscpMutationEntrySet",
        &wrCpssDxChCosDscpMutationEntrySet,
        3, 0},

    {"cpssDxChCosDscpMutationEntryGet",
        &wrCpssDxChCosDscpMutationEntryGet,
        2, 0},

    {"cpssDxChCosDscpMutationEntrySet_1",
        &wrCpssDxChCosDscpMutationEntrySet_1,
        4, 0},

    {"cpssDxChCosDscpMutationEntryGet_1",
        &wrCpssDxChCosDscpMutationEntryGet_1,
        3, 0},

    {"cpssDxChCosUpToProfileMapSet",
        &wrCpssDxChCosUpToProfileMapSet,
        3, 0},

    {"cpssDxChCosUpToProfileMapGet",
        &wrCpssDxChCosUpToProfileMapGet,
        2, 0},

    {"cpssDxChCosExpToProfileMapSet",
        &wrCpssDxChCosExpToProfileMapSet,
        3, 0},

    {"cpssDxChCosExpToProfileMapGet",
        &wrCpssDxChCosExpToProfileMapGet,
        2, 0},
    {"cpssDxChCosExpToProfileMapSet_1",
        &wrCpssDxChCosExpToProfileMapSet_1,
        4, 0},

    {"cpssDxChCosExpToProfileMapGet_1",
        &wrCpssDxChCosExpToProfileMapGet_1,
        3, 0},

    {"cpssDxChCosPortQosConfigSet",
        &wrCpssDxChCosPortQosConfigSet,
        1, 5},

    {"cpssDxChCosPortQosConfigGetFirst",
        &wrCpssDxChCosPortQosConfigGetFirst,
        1, 0},

    {"cpssDxChCosPortQosConfigGetNext",
        &wrCpssDxChCosPortQosConfigGetNext,
        1, 0},

    {"cpssDxChCosMacQosEntrySet",
        &wrCpssDxChCosMacQosEntrySet,
        1, 5},

    {"cpssDxChCosMacQosEntryGetFirst",
        &wrCpssDxChCosMacQosEntryGetFirst,
        1, 0},

    {"cpssDxChCosMacQosEntryGetNext",
        &wrCpssDxChCosMacQosEntryGetNext,
        1, 0},

    {"cpssDxChCosMacQosConflictResolutionSet",
        &wrCpssDxChCosMacQosConflictResolutionSet,
        2, 0},

    {"cpssDxChCosMacQosConflictResolutionGet",
        &wrCpssDxChCosMacQosConflictResolutionGet,
        1, 0},

    {"cpssDxChCosRemarkTblEntrySet",
        &wrCpssDxChCosRemarkTblEntrySet,
        3, 0},

    {"cpssDxChCosRemarkTblEntryGet",
        &wrCpssDxChCosRemarkTblEntryGet,
        2, 0},

    {"cpssDxChPortDefaultUPSet",
        &wrCpssDxChPortDefaultUPSet,
        3, 0},

    {"cpssDxChPortDefaultUPGet",
        &wrCpssDxChPortDefaultUPGet,
        2, 0},

    {"cpssDxChPortModifyUPSet",
        &wrCpssDxChPortModifyUPSet,
        3, 0},

    {"cpssDxChPortModifyUPGet",
        &wrCpssDxChPortModifyUPGet,
        2, 0},

    {"cpssDxChCosPortQosTrustModeSet",
        &wrCpssDxChCosPortQosTrustModeSet,
        3, 0},

    {"cpssDxChCosPortQosTrustModeGet",
        &wrCpssDxChCosPortQosTrustModeGet,
        2, 0},

   {"cpssDxChCosPortTrustQosMappingTableIndexSet",
        &wrCpssDxChCosPortTrustQosMappingTableIndexSet,
        4, 0},

   {"cpssDxChCosPortTrustQosMappingTableIndexGet",
       &wrCpssDxChCosPortTrustQosMappingTableIndexGet,
       2, 0},

    {"cpssDxChCosPortReMapDSCPSet",
        &wrCpssDxChCosPortReMapDSCPSet,
        3, 0},

    {"cpssDxChCosPortReMapDSCPGet",
        &wrCpssDxChCosPortReMapDSCPGet,
        2, 0},

    {"cpssDxChCosProtBasedQosEn",
        &wrCpssDxChCosProtBasedQosEn,
        3, 0},

    {"cpssDxChCosPortVlanQoSCfgEntryGet",
        &wrCpssDxChCosPortVlanQoSCfgEntryGet,
        2, 0},

    {"cpssDxChCosRemarkEntryGet",
        &wrCpssDxChCosRemarkEntryGet,
        2, 0},

    {"cpssDxChCosQoSProfileEntryGet",
        &wrCpssDxChCosQoSProfileEntryGet,
        2, 0},

    {"cpssDxChCosTrustDsaTagQosModeSet",
        &wrCpssDxChCosTrustDsaTagQosModeSet,
        3, 0},

    {"cpssDxChCosTrustDsaTagQosModeGet",
        &wrCpssDxChCosTrustDsaTagQosModeGet,
        2, 0},
    {"cpssDxChCosUpCfiDeiToProfileMapSet",
         &wrCpssDxChCosUpCfiDeiToProfileMapSet,
         4, 0},
    {"cpssDxChCosUpCfiDeiToProfileMapGet",
         &wrCpssDxChCosUpCfiDeiToProfileMapGet,
         3, 0},
    {"cpssDxChCosPortDpToCfiDeiMapEnableSet",
         &wrCpssDxChCosPortDpToCfiDeiMapEnableSet,
         3, 0},
    {"cpssDxChCosPortDpToCfiDeiMapEnableGet",
         &wrCpssDxChCosPortDpToCfiDeiMapEnableGet,
         2, 0},
    {"cpssDxChCosPortDpToCfiDei1MapEnableSet",
         &wrCpssDxChCosPortDpToCfiDei1MapEnableSet,
         3, 0},
    {"cpssDxChCosPortDpToCfiDei1MapEnableGet",
         &wrCpssDxChCosPortDpToCfiDei1MapEnableGet,
         2, 0},
    {"cpssDxChCosDpToCfiDeiMapSet",
         &wrCpssDxChCosDpToCfiDeiMapSet,
         3, 0},
    {"cpssDxChCosDpToCfiDeiMapGet",
         &wrCpssDxChCosDpToCfiDeiMapGet,
         2, 0},

    {"cpssDxChCosTrustExpModeSet",
         &wrCpssDxChCosTrustExpModeSet,
         3, 0},
    {"cpssDxChCosTrustExpModeGet",
         &wrCpssDxChCosTrustExpModeGet,
         2, 0},

    {"cpssDxChCosL2TrustModeVlanTagSelectSet",
         &wrCpssDxChCosL2TrustModeVlanTagSelectSet,
         3, 0},
    {"cpssDxChCosL2TrustModeVlanTagSelectGet",
         &wrCpssDxChCosL2TrustModeVlanTagSelectGet,
         2, 0},

    {"cpssDxChCosL2TrustModeVlanTagSelectSet_1",
         &wrCpssDxChCosL2TrustModeVlanTagSelectSet_1,
         4, 0},
    {"cpssDxChCosL2TrustModeVlanTagSelectGet_1",
         &wrCpssDxChCosL2TrustModeVlanTagSelectGet_1,
         3, 0},

    /* start table - cpssDxChCosPortUpProfileIndex */
    {"cpssDxChCosPortUpProfileIndexSet",
        &wrCpssDxChCosPortUpProfileIndexSet,
        1, 9},

    {"cpssDxChCosPortUpProfileIndexGetFirst",
        &wrCpssDxChCosPortUpProfileIndexGetFirst,
        1, 0},

    {"cpssDxChCosPortUpProfileIndexGetNext",
        &wrCpssDxChCosPortUpProfileIndexGetNext,
        1, 0},
     /* end table - cpssDxChCosPortUpProfileIndex */

     /* start table - cpssDxChCosUpCfiDeiToProfileMap1 */
    {"cpssDxChCosUpCfiDeiToProfileMap1Set",
        &wrCpssDxChCosUpCfiDeiToProfileMap1Set,
        2, 3},

    {"cpssDxChCosUpCfiDeiToProfileMap1GetFirst",
        &wrCpssDxChCosUpCfiDeiToProfileMap1GetFirst,
        2, 0},

    {"cpssDxChCosUpCfiDeiToProfileMap1GetNext",
        &wrCpssDxChCosUpCfiDeiToProfileMap1GetNext,
        2, 0},
    /* end table - cpssDxChCosUpCfiDeiToProfileMap1 */

     /* start table - cpssDxChCosUpCfiDeiToProfileMap2 */
    {"cpssDxChCosUpCfiDeiToProfileMap2Set",
        &wrCpssDxChCosUpCfiDeiToProfileMap2Set,
        3, 3},

    {"cpssDxChCosUpCfiDeiToProfileMap2GetFirst",
        &wrCpssDxChCosUpCfiDeiToProfileMap2GetFirst,
        3, 0},

    {"cpssDxChCosUpCfiDeiToProfileMap2GetNext",
        &wrCpssDxChCosUpCfiDeiToProfileMap2GetNext,
        3, 0},
    /* end table - cpssDxChCosUpCfiDeiToProfileMap2 */

    {"cpssDxChCosProfileEntriesRangeGetFirst",
        &wrCpssDxChCosProfileEntriesRangeGetFirst,
        3, 0},

    {"cpssDxChCosProfileEntriesRangeGetNext",
        &wrCpssDxChCosProfileEntriesRangeGetNext,
        3, 0},

    {"cpssDxChCosProfileEntriesRangeSet",
        &wrCpssDxChCosProfileEntriesRangeSet,
        1, 6},

    {"cpssDxChCosPortEgressQoSMappingTableIndexSet",
        &wrCpssDxChCosPortEgressQoSMappingTableIndexSet,
        3, 0},

    {"cpssDxChCosPortEgressQoSMappingTableIndexGet",
        &wrCpssDxChCosPortEgressQoSMappingTableIndexGet,
        2, 0},

    {"cpssDxChCosPortEgressQoSExpMappingEnableSet",
        &wrCpssDxChCosPortEgressQoSExpMappingEnableSet,
        3, 0},

    {"cpssDxChCosPortEgressQoSExpMappingEnableGet",
        &wrCpssDxChCosPortEgressQoSExpMappingEnableGet,
        2, 0},

    {"cpssDxChCosPortEgressQoSTcDpMappingEnableSet",
        &wrCpssDxChCosPortEgressQoSTcDpMappingEnableSet,
        3, 0},

    {"cpssDxChCosPortEgressQoSTcDpMappingEnableGet",
        &wrCpssDxChCosPortEgressQoSTcDpMappingEnableGet,
        2, 0},

    {"cpssDxChCosPortEgressQoSUpMappingEnableSet",
        &wrCpssDxChCosPortEgressQoSUpMappingEnableSet,
        3, 0},

    {"cpssDxChCosPortEgressQoSUpMappingEnableGet",
        &wrCpssDxChCosPortEgressQoSUpMappingEnableGet,
        2, 0},

    {"cpssDxChCosPortEgressQoSDscpMappingEnableSet",
        &wrCpssDxChCosPortEgressQoSDscpMappingEnableSet,
        3, 0},

    {"cpssDxChCosPortEgressQoSDscpMappingEnableGet",
        &wrCpssDxChCosPortEgressQoSDscpMappingEnableGet,
        2, 0},

    {"cpssDxChCosEgressDscp2DscpMappingEntrySet",
        &wrCpssDxChCosEgressDscp2DscpMappingEntrySet,
        4, 0},

    {"cpssDxChCosEgressDscp2DscpMappingEntryGet",
        &wrCpssDxChCosEgressDscp2DscpMappingEntryGet,
        3, 0},

    {"cpssDxChCosEgressExp2ExpMappingEntrySet",
        &wrCpssDxChCosEgressExp2ExpMappingEntrySet,
        4, 0},

    {"cpssDxChCosEgressExp2ExpMappingEntryGet",
        &wrCpssDxChCosEgressExp2ExpMappingEntryGet,
        3, 0},

    {"cpssDxChCosEgressUp2UpMappingEntrySet",
        &wrCpssDxChCosEgressUp2UpMappingEntrySet,
        4, 0},

    {"cpssDxChCosEgressUp2UpMappingEntryGet",
        &wrCpssDxChCosEgressUp2UpMappingEntryGet,
        3, 0},

    {"cpssDxChCosEgressTcDp2UpExpDscpMappingEntrySet",
        &wrCpssDxChCosEgressTcDp2UpExpDscpMappingEntrySet,
        7, 0},

    {"cpssDxChCosEgressTcDp2UpExpDscpMappingEntryGet",
        &wrCpssDxChCosEgressTcDp2UpExpDscpMappingEntryGet,
        4, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChCos
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
GT_STATUS cmdLibInitCpssDxChCos
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


