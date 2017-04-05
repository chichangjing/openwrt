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
* wrapCpssDxChOam.c
*
* DESCRIPTION:
*       Wrapper functions for Cscd cpss.dxCh functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 13 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/dxCh/dxChxGen/oam/cpssDxChOam.h>

#include <galtisAgent/wrapUtil/dxCh/utils/cmdCpssDxChIncrEntry.h>

/* wrapper for cpssDxChOamEntrySet */
static CMD_STATUS wrCpssDxChOamEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       rc;
    GT_U8                          devNum;
    CPSS_DXCH_OAM_ENTRY_STC         oamEntry;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    GT_U32                              entryIndex;
    GT_U32  ii;/*index in inFields */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /*support options that the number of fields will grow ... give default to all fields */
    cpssOsMemSet(&oamEntry,0,sizeof(oamEntry));

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    entryIndex      = (GT_U32)inArgs[2];

    ii = 0;
    oamEntry.opcodeParsingEnable                    = inFields[ii++];
    oamEntry.megLevelCheckEnable                    = inFields[ii++];
    oamEntry.megLevel                               = (GT_U32)inFields[ii++];
    oamEntry.packetCommandProfile                   = (GT_U32)inFields[ii++];
    oamEntry.cpuCodeOffset                          = (GT_U32)inFields[ii++];
    oamEntry.sourceInterfaceCheckEnable             = inFields[ii++];
    oamEntry.sourceInterfaceCheckMode               = inFields[ii++];
    oamEntry.sourceInterface.type                   = inFields[ii++];
    oamEntry.sourceInterface.devPort.hwDevNum       = (GT_HW_DEV_NUM)inFields[ii++];
    oamEntry.sourceInterface.devPort.portNum        = (GT_PORT_NUM)inFields[ii++];
    oamEntry.sourceInterface.trunkId                = (GT_TRUNK_ID)inFields[ii++];
    oamEntry.lmCounterCaptureEnable                 = inFields[ii++];
    oamEntry.dualEndedLmEnable                      = inFields[ii++];
    oamEntry.lmCountingMode                         = inFields[ii++];
    oamEntry.oamPtpOffsetIndex                      = (GT_U32)inFields[ii++];
    oamEntry.timestampEnable                        = inFields[ii++];
    oamEntry.keepaliveAgingEnable                   = inFields[ii++];
    oamEntry.ageState                               = (GT_U32)inFields[ii++];
    oamEntry.agingPeriodIndex                       = (GT_U32)inFields[ii++];
    oamEntry.agingThreshold                         = (GT_U32)inFields[ii++];
    oamEntry.hashVerifyEnable                       = inFields[ii++];
    oamEntry.lockHashValueEnable                    = inFields[ii++];
    oamEntry.protectionLocUpdateEnable              = inFields[ii++];
    oamEntry.flowHash                               = (GT_U32)inFields[ii++];
    oamEntry.excessKeepaliveDetectionEnable         = inFields[ii++];
    oamEntry.excessKeepalivePeriodCounter           = (GT_U32)inFields[ii++];
    oamEntry.excessKeepalivePeriodThreshold         = (GT_U32)inFields[ii++];
    oamEntry.excessKeepaliveMessageCounter          = (GT_U32)inFields[ii++];
    oamEntry.excessKeepaliveMessageThreshold        = (GT_U32)inFields[ii++];
    oamEntry.rdiCheckEnable                         = inFields[ii++];
    oamEntry.rdiStatus                              = (GT_U32)inFields[ii++];
    oamEntry.periodCheckEnable                      = inFields[ii++];
    oamEntry.keepaliveTxPeriod                      = (GT_U32)inFields[ii++];

    if(oamEntry.sourceInterface.type == CPSS_INTERFACE_PORT_E)
    {
        CONVERT_DEV_PORT_DATA_MAC(
            oamEntry.sourceInterface.devPort.hwDevNum,
            oamEntry.sourceInterface.devPort.portNum);
    }
    else /*trunk*/
    {
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(oamEntry.sourceInterface.trunkId);
    }

    /* call cpss api function */
    rc = cpssDxChOamEntrySet(devNum,stage,entryIndex,&oamEntry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    return CMD_OK;


}
/* index to use for 'refresh' on cpssDxChOamEntryGet */
static GT_U32 oamEntryGlobalIndex = 0;
static GT_U32 oamEntryNumOfEntries = 0;

/* wrapper for cpssDxChOamEntryGet (allow refresh of multi entries)*/
static CMD_STATUS wrCpssDxChOamEntryGet
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       rc;
    GT_U8                          devNum;
    CPSS_DXCH_OAM_ENTRY_STC         oamEntry;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    GT_U32  ii;/*index in inFields */
    GT_U32  actualNumFields;/* number of fields */

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];

    if((oamEntryGlobalIndex >= _2K) || (oamEntryNumOfEntries >= (GT_U32)inArgs[3]))
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    rc = cpssDxChOamEntryGet(devNum,stage,oamEntryGlobalIndex,&oamEntry);
    oamEntryGlobalIndex++;
    oamEntryNumOfEntries++;

    if (rc != GT_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    if(oamEntry.sourceInterface.type == CPSS_INTERFACE_PORT_E)
    {
        CONVERT_BACK_DEV_PORT_DATA_MAC(oamEntry.sourceInterface.devPort.hwDevNum,
                                       oamEntry.sourceInterface.devPort.portNum);
        oamEntry.sourceInterface.trunkId = 0;
    }
    else /*trunk*/
    {
        CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(oamEntry.sourceInterface.trunkId);
        oamEntry.sourceInterface.devPort.hwDevNum = 0;
        oamEntry.sourceInterface.devPort.portNum = 0;
    }

    ii = 0;
    inFields[ii++] = oamEntry.opcodeParsingEnable                 ;
    inFields[ii++] = oamEntry.megLevelCheckEnable                 ;
    inFields[ii++] = oamEntry.megLevel                            ;
    inFields[ii++] = oamEntry.packetCommandProfile                ;
    inFields[ii++] = oamEntry.cpuCodeOffset                       ;
    inFields[ii++] = oamEntry.sourceInterfaceCheckEnable          ;
    inFields[ii++] = oamEntry.sourceInterfaceCheckMode            ;
    inFields[ii++] = oamEntry.sourceInterface.type                ;
    inFields[ii++] = oamEntry.sourceInterface.devPort.hwDevNum    ;
    inFields[ii++] = oamEntry.sourceInterface.devPort.portNum     ;
    inFields[ii++] = oamEntry.sourceInterface.trunkId             ;
    inFields[ii++] = oamEntry.lmCounterCaptureEnable              ;
    inFields[ii++] = oamEntry.dualEndedLmEnable                   ;
    inFields[ii++] = oamEntry.lmCountingMode                      ;
    inFields[ii++] = oamEntry.oamPtpOffsetIndex                   ;
    inFields[ii++] = oamEntry.timestampEnable                     ;
    inFields[ii++] = oamEntry.keepaliveAgingEnable                ;
    inFields[ii++] = oamEntry.ageState                            ;
    inFields[ii++] = oamEntry.agingPeriodIndex                    ;
    inFields[ii++] = oamEntry.agingThreshold                      ;
    inFields[ii++] = oamEntry.hashVerifyEnable                    ;
    inFields[ii++] = oamEntry.lockHashValueEnable                 ;
    inFields[ii++] = oamEntry.protectionLocUpdateEnable           ;
    inFields[ii++] = oamEntry.flowHash                            ;
    inFields[ii++] = oamEntry.excessKeepaliveDetectionEnable      ;
    inFields[ii++] = oamEntry.excessKeepalivePeriodCounter        ;
    inFields[ii++] = oamEntry.excessKeepalivePeriodThreshold      ;
    inFields[ii++] = oamEntry.excessKeepaliveMessageCounter       ;
    inFields[ii++] = oamEntry.excessKeepaliveMessageThreshold     ;
    inFields[ii++] = oamEntry.rdiCheckEnable                      ;
    inFields[ii++] = oamEntry.rdiStatus                           ;
    inFields[ii++] = oamEntry.periodCheckEnable                   ;
    inFields[ii++] = oamEntry.keepaliveTxPeriod                   ;

    actualNumFields = ii;
    /* pack and output table fields */
    for (ii = 0; ii < actualNumFields; ii++)
    {
        fieldOutput("%d", inFields[ii]);
        fieldOutputSetAppendMode();
    }

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/* wrapper for cpssDxChOamEntryGet (allow refresh of multi entries)*/
static CMD_STATUS wrCpssDxChOamEntryGetFirst
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    oamEntryGlobalIndex = (GT_U32)inArgs[2];
    oamEntryNumOfEntries = 0;

    return wrCpssDxChOamEntryGet(inArgs,inFields,numFields,outArgs);
}
/* wrapper for cpssDxChOamEntryGet (allow refresh of multi entries)*/
static CMD_STATUS wrCpssDxChOamEntryGetNext
(
    IN  GT_UINTPTR  inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR  inFields[CMD_MAX_FIELDS],
    IN  GT_32  numFields,
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChOamEntryGet(inArgs,inFields,numFields,outArgs);
}

/* wrapper for cpssDxChOamOpcodeSet */
static CMD_STATUS wrCpssDxChOamOpcodeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                                    devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT             stage;
    CPSS_DXCH_OAM_OPCODE_TYPE_ENT            opcodeType;
    GT_U32                                   opcodeIndex;
    GT_U32                                   opcodeValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    stage       = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    opcodeType  = (CPSS_DXCH_OAM_OPCODE_TYPE_ENT)inArgs[2];
    opcodeIndex = (GT_U32)inArgs[3];
    opcodeValue = (GT_U32)inArgs[4];

    /* call cpss api function */
    rc = cpssDxChOamOpcodeSet(devNum,stage,opcodeType,opcodeIndex,opcodeValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamOpcodeGet */
static CMD_STATUS wrCpssDxChOamOpcodeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                                    devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT             stage;
    CPSS_DXCH_OAM_OPCODE_TYPE_ENT            opcodeType;
    GT_U32                                   opcodeIndex;
    GT_U32                                   opcodeValue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum      = (GT_U8)inArgs[0];
    stage       = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    opcodeType  = (CPSS_DXCH_OAM_OPCODE_TYPE_ENT)inArgs[2];
    opcodeIndex = (GT_U32)inArgs[3];

    /* call cpss api function */
    rc = cpssDxChOamOpcodeGet(devNum,stage,opcodeType,opcodeIndex,&opcodeValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", opcodeValue);

    return CMD_OK;
}

/* wrapper for cpssDxChOamOpcodeProfilePacketCommandEntrySet */
static CMD_STATUS wrCpssDxChOamOpcodeProfilePacketCommandEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    GT_U32                              opcode;
    GT_U32                              profile;
    CPSS_PACKET_CMD_ENT                 command;
    GT_U32                              cpuCodeOffset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    opcode          = (GT_U32)inArgs[2];
    profile         = (GT_U32)inArgs[3];
    command         = (CPSS_PACKET_CMD_ENT)inArgs[4];
    cpuCodeOffset   = (GT_U32)inArgs[5];

    /* call cpss api function */
    rc = cpssDxChOamOpcodeProfilePacketCommandEntrySet(devNum,stage,opcode,profile,command,cpuCodeOffset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamOpcodeProfilePacketCommandEntryGet */
static CMD_STATUS wrCpssDxChOamOpcodeProfilePacketCommandEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    GT_U32                              opcode;
    GT_U32                              profile;
    CPSS_PACKET_CMD_ENT                 command;
    GT_U32                              cpuCodeOffset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    opcode          = (GT_U32)inArgs[2];
    profile         = (GT_U32)inArgs[3];

    /* call cpss api function */
    rc = cpssDxChOamOpcodeProfilePacketCommandEntryGet(devNum,stage,opcode,profile,&command,&cpuCodeOffset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", command,cpuCodeOffset);

    return CMD_OK;
}


/* wrapper for cpssDxChOamTableBaseFlowIdSet */
static CMD_STATUS wrCpssDxChOamTableBaseFlowIdSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    GT_U32                              baseFlowId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    baseFlowId      = (GT_U32)inArgs[2];

    /* call cpss api function */
    rc = cpssDxChOamTableBaseFlowIdSet(devNum,stage,baseFlowId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamTableBaseFlowIdGet */
static CMD_STATUS wrCpssDxChOamTableBaseFlowIdGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    GT_U32                              baseFlowId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    rc = cpssDxChOamTableBaseFlowIdGet(devNum,stage,&baseFlowId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d",baseFlowId);

    return CMD_OK;
}

/* wrapper for cpssDxChOamDualEndedLmPacketCommandSet */
static CMD_STATUS wrCpssDxChOamDualEndedLmPacketCommandSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    CPSS_PACKET_CMD_ENT                          command;
    GT_U32                                       cpuCodeLsBits;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    command         = (CPSS_PACKET_CMD_ENT)inArgs[2];
    cpuCodeLsBits   = (GT_U32)inArgs[3];

    /* call cpss api function */
    rc = cpssDxChOamDualEndedLmPacketCommandSet(devNum,stage,command,cpuCodeLsBits);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamDualEndedLmPacketCommandGet */
static CMD_STATUS wrCpssDxChOamDualEndedLmPacketCommandGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    CPSS_PACKET_CMD_ENT                          command;
    GT_U32                                       cpuCodeLsBits;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    rc = cpssDxChOamDualEndedLmPacketCommandGet(devNum,stage,&command,&cpuCodeLsBits);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d",command,cpuCodeLsBits);

    return CMD_OK;
}

/* wrapper for cpssDxChOamTimeStampParsingEnableSet */
static CMD_STATUS wrCpssDxChOamTimeStampParsingEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    GT_BOOL      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    enable          = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    rc = cpssDxChOamTimeStampParsingEnableSet(devNum,enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamTimeStampParsingEnableGet */
static CMD_STATUS wrCpssDxChOamTimeStampParsingEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    GT_BOOL      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    enable          = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    rc = cpssDxChOamTimeStampParsingEnableGet(devNum,&enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d",enable);

    return CMD_OK;
}

/* wrapper for cpssDxChOamTimeStampEtherTypeSet */
static CMD_STATUS wrCpssDxChOamTimeStampEtherTypeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    GT_U32      etherType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    etherType       = (GT_U32)inArgs[1];

    /* call cpss api function */
    rc = cpssDxChOamTimeStampEtherTypeSet(devNum,etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamTimeStampEtherTypeGet */
static CMD_STATUS wrCpssDxChOamTimeStampEtherTypeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    GT_U32      etherType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    etherType       = (GT_U32)inArgs[1];

    /* call cpss api function */
    rc = cpssDxChOamTimeStampEtherTypeGet(devNum,&etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d",etherType);

    return CMD_OK;
}

/* wrapper for cpssDxChOamOpcodeProfileDedicatedMcProfileEnableSet */
static CMD_STATUS wrcpssDxChOamOpcodeProfileDedicatedMcProfileEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage;
    GT_BOOL     enable;
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    ii = 0;
    devNum          = (GT_U8)inArgs[ii++];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[ii++];
    enable          = (GT_BOOL)inArgs[ii++];

    /* call cpss api function */
    rc = cpssDxChOamOpcodeProfileDedicatedMcProfileEnableSet(devNum,stage,enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamOpcodeProfileDedicatedMcProfileEnableGet */
static CMD_STATUS wrcpssDxChOamOpcodeProfileDedicatedMcProfileEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage;
    GT_BOOL     enable;
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    ii = 0;
    devNum          = (GT_U8)inArgs[ii++];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[ii++];

    /* call cpss api function */
    rc = cpssDxChOamOpcodeProfileDedicatedMcProfileEnableGet(devNum,stage,&enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d",enable);

    return CMD_OK;
}

/* wrapper for cpssDxChOamExceptionConfigSet */
static CMD_STATUS wrCpssDxChOamExceptionConfigSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage;
    CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT        exceptionType;
    CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC      exceptionConfig;
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    ii = 0;
    devNum          = (GT_U8)inArgs[ii++];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[ii++];
    exceptionType   = (CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT)inArgs[ii++];
    exceptionConfig.command = (CPSS_PACKET_CMD_ENT)inArgs[ii++];
    exceptionConfig.cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[ii++];
    exceptionConfig.summaryBitEnable = (GT_BOOL)inArgs[ii++];

    /* call cpss api function */
    rc = cpssDxChOamExceptionConfigSet(devNum,stage,exceptionType,&exceptionConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamExceptionConfigGet */
static CMD_STATUS wrCpssDxChOamExceptionConfigGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT            stage;
    CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT        exceptionType;
    CPSS_DXCH_OAM_EXCEPTION_CONFIG_STC      exceptionConfig;
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    ii = 0;
    devNum          = (GT_U8)inArgs[ii++];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[ii++];
    exceptionType   = (CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT)inArgs[ii++];

    /* call cpss api function */
    rc = cpssDxChOamExceptionConfigGet(devNum,stage,exceptionType,&exceptionConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d%d",
        exceptionConfig.command,
        exceptionConfig.cpuCode,
        exceptionConfig.summaryBitEnable);

    return CMD_OK;
}


/* wrapper for cpssDxChOamExceptionGroupStatusGet */
static CMD_STATUS wrCpssDxChOamExceptionGroupStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT    exceptionType;
    GT_U32                              groupStatusArr[CPSS_DXCH_OAM_GROUP_STATUS_SIZE_IN_WORDS_CNS];
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    ii = 0;
    devNum          = (GT_U8)inArgs[ii++];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[ii++];
    exceptionType   = (CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT)inArgs[ii++];

    /* call cpss api function */
    rc = cpssDxChOamExceptionGroupStatusGet(devNum,stage,exceptionType,groupStatusArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d",
        groupStatusArr[0],
        groupStatusArr[1]);

    return CMD_OK;
}

/* wrapper for cpssDxChOamExceptionStatusGet */
static CMD_STATUS wrCpssDxChOamExceptionStatusGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT                stage;
    CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT            exceptionType;
    GT_U32                                      entryIndex;
    GT_U32                                      entryBmp;
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    ii = 0;
    devNum          = (GT_U8)inArgs[ii++];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[ii++];
    entryIndex      = (GT_U32)inArgs[ii++];
    exceptionType   = (CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT)inArgs[ii++];

    /* call cpss api function */
    rc = cpssDxChOamExceptionStatusGet(devNum,stage,entryIndex,exceptionType,&entryBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d",entryBmp);

    return CMD_OK;
}



/* wrapper for cpssDxChOamAgingPeriodEntrySet */
static CMD_STATUS wrCpssDxChOamAgingPeriodEntrySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    GT_U32                              entryIndex;
    GT_U64                              agingPeriodValue;
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    ii = 0;
    devNum          = (GT_U8)inArgs[ii++];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[ii++];
    entryIndex      = (GT_U32)inArgs[ii++];
    agingPeriodValue.l[0] = (GT_U32)inArgs[ii++];
    agingPeriodValue.l[1] = (GT_U32)inArgs[ii++];

    /* call cpss api function */
    rc = cpssDxChOamAgingPeriodEntrySet(devNum,stage,entryIndex,agingPeriodValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamAgingPeriodEntryGet */
static CMD_STATUS wrCpssDxChOamAgingPeriodEntryGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    GT_U32                              entryIndex;
    GT_U64                              agingPeriodValue;
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    ii = 0;
    devNum          = (GT_U8)inArgs[ii++];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[ii++];
    entryIndex      = (GT_U32)inArgs[ii++];

    /* call cpss api function */
    rc = cpssDxChOamAgingPeriodEntryGet(devNum,stage,entryIndex,&agingPeriodValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d",
        agingPeriodValue.l[0],
        agingPeriodValue.l[1]);

    return CMD_OK;
}

/* wrapper for cpssDxChOamAgingDaemonEnableSet */
static CMD_STATUS wrCpssDxChOamAgingDaemonEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    GT_BOOL                             enable;
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    ii = 0;
    devNum          = (GT_U8)inArgs[ii++];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[ii++];
    enable          = (GT_BOOL)inArgs[ii++];

    /* call cpss api function */
    rc = cpssDxChOamAgingDaemonEnableSet(devNum,stage,enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamAgingDaemonEnableGet */
static CMD_STATUS wrCpssDxChOamAgingDaemonEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    GT_BOOL                             enable;
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    ii = 0;
    devNum          = (GT_U8)inArgs[ii++];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[ii++];

    /* call cpss api function */
    rc = cpssDxChOamAgingDaemonEnableGet(devNum,stage,&enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d",enable);

    return CMD_OK;
}


/* wrapper for cpssDxChOamExceptionCounterGet */
static CMD_STATUS wrCpssDxChOamExceptionCounterGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT    exceptionType;
    GT_U32                              counterValue;
    GT_U32      ii;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    ii = 0;
    devNum          = (GT_U8)inArgs[ii++];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[ii++];
    exceptionType   = (CPSS_DXCH_OAM_EXCEPTION_TYPE_ENT)inArgs[ii++];

    /* call cpss api function */
    rc = cpssDxChOamExceptionCounterGet(devNum,stage,exceptionType,&counterValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d",counterValue);

    return CMD_OK;
}

/* wrapper for cpssDxChOamPduCpuCodeSet */
static CMD_STATUS wrCpssDxChOamPduCpuCodeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_NET_RX_CPU_CODE_ENT            cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    cpuCode         = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[1];

    /* call cpss api function */
    rc = cpssDxChOamPduCpuCodeSet(devNum,cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamPduCpuCodeGet */
static CMD_STATUS wrCpssDxChOamPduCpuCodeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8                               devNum;
    CPSS_NET_RX_CPU_CODE_ENT            cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];

    /* call cpss api function */
    rc = cpssDxChOamPduCpuCodeGet(devNum, &cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", cpuCode);

    return CMD_OK;
}

/* wrapper for cpssDxChOamMplsCwChannelTypeProfileSet */
static CMD_STATUS wrCpssDxChOamMplsCwChannelTypeProfileSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8     devNum;
    GT_U32    profileIndex;
    GT_U32    channelTypeId;
    GT_BOOL   profileEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum         = (GT_U8)inArgs[0];
    profileIndex   = (GT_U32)inArgs[1];
    channelTypeId  = (GT_U32)inArgs[2];
    profileEnable  = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    rc = cpssDxChOamMplsCwChannelTypeProfileSet(
        devNum, profileIndex, channelTypeId, profileEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamMplsCwChannelTypeProfileGet */
static CMD_STATUS wrCpssDxChOamMplsCwChannelTypeProfileGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;
    GT_U8     devNum;
    GT_U32    profileIndex;
    GT_U32    channelTypeId;
    GT_BOOL   profileEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum         = (GT_U8)inArgs[0];
    profileIndex   = (GT_U32)inArgs[1];

    /* call cpss api function */
    rc = cpssDxChOamMplsCwChannelTypeProfileGet(
        devNum, profileIndex, &channelTypeId, &profileEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", channelTypeId, profileEnable);

    return CMD_OK;
}

/* wrapper for cpssDxChOamAgingBitmapUpdateModeSet */
static CMD_STATUS wrCpssDxChOamAgingBitmapUpdateModeSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   rc;
    GT_U8                                       devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT                stage;
    CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT  mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    stage   = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    mode    = (CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT)inArgs[2];

    /* call cpss api function */
    rc = cpssDxChOamAgingBitmapUpdateModeSet(devNum,stage,mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamAgingBitmapUpdateModeGet */
static CMD_STATUS wrCpssDxChOamAgingBitmapUpdateModeGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   rc;
    GT_U8                                       devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT                stage;
    CPSS_DXCH_OAM_AGING_BITMAP_UPDATE_MODE_ENT  mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    stage   = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    rc = cpssDxChOamAgingBitmapUpdateModeGet(devNum,stage,&mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d",mode);

    return CMD_OK;
}

/* wrapper for cpssDxChOamHashBitSelectionSet */
static CMD_STATUS wrCpssDxChOamHashBitSelectionSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   rc;
    GT_U8                                       devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT                stage;
    GT_U32                                      hashFirstBit;
    GT_U32                                      hashLastBit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum       = (GT_U8)inArgs[0];
    stage        = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    hashFirstBit = (GT_U32)inArgs[2];
    hashLastBit  = (GT_U32)inArgs[3];

    /* call cpss api function */
    rc = cpssDxChOamHashBitSelectionSet(devNum,stage,hashFirstBit,hashLastBit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamHashBitSelectionGet */
static CMD_STATUS wrCpssDxChOamHashBitSelectionGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   rc;
    GT_U8                                       devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT                stage;
    GT_U32                                      hashFirstBit;
    GT_U32                                      hashLastBit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    hashFirstBit    = (GT_U32)inArgs[2];
    hashLastBit     = (GT_U32)inArgs[3];

    /* call cpss api function */
    rc = cpssDxChOamHashBitSelectionGet(devNum,stage,&hashFirstBit,&hashLastBit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", hashFirstBit, hashLastBit);

    return CMD_OK;
}

/* wrapper for cpssDxChOamEnableSet */
static CMD_STATUS wrCpssDxChOamEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       rc;
    GT_U8                           devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT    stage;
    GT_BOOL                         enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    enable          = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    rc = cpssDxChOamEnableSet(devNum, stage, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamEnableGet */
static CMD_STATUS wrCpssDxChOamEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       rc;
    GT_U8                           devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT    stage;
    GT_BOOL                         enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    rc = cpssDxChOamEnableGet(devNum, stage, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d",enable);

    return CMD_OK;
}

/* wrapper for cpssDxChOamCpuCodeBaseSet */
static CMD_STATUS wrCpssDxChOamCpuCodeBaseSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       rc;
    GT_U8                           devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT    stage;
    CPSS_NET_RX_CPU_CODE_ENT        baseCpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    baseCpuCode     = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[2];

    /* call cpss api function */
    rc = cpssDxChOamCpuCodeBaseSet(devNum, stage, baseCpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamCpuCodeBaseGet */
static CMD_STATUS wrCpssDxChOamCpuCodeBaseGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       rc;
    GT_U8                           devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT    stage;
    CPSS_NET_RX_CPU_CODE_ENT        baseCpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    rc = cpssDxChOamCpuCodeBaseGet(devNum, stage, &baseCpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d",baseCpuCode);

    return CMD_OK;
}

/* wrapper for cpssDxChOamLmOffsetTableSet */
static CMD_STATUS wrCpssDxChOamLmOffsetTableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           rc;
    GT_U8               devNum;
    GT_U32              entryIndex;
    GT_U32              offset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    entryIndex      = (GT_U32)inArgs[1];
    offset          = (GT_U32)inArgs[2];

    /* call cpss api function */
    rc = cpssDxChOamLmOffsetTableSet(devNum, entryIndex, offset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamLmOffsetTableGet */
static CMD_STATUS wrCpssDxChOamLmOffsetTableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS           rc;
    GT_U8               devNum;
    GT_U32              entryIndex;
    GT_U32              offset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    entryIndex      = (GT_U32)inArgs[1];

    /* call cpss api function */
    rc = cpssDxChOamLmOffsetTableGet(devNum, entryIndex, &offset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d",offset);

    return CMD_OK;
}

/* wrapper for cpssDxChOamKeepaliveForPacketCommandEnableSet */
static CMD_STATUS wrCpssDxChOamKeepaliveForPacketCommandEnableSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    CPSS_PACKET_CMD_ENT                 command;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    command         = (CPSS_PACKET_CMD_ENT)inArgs[2];
    enable          = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    rc = cpssDxChOamKeepaliveForPacketCommandEnableSet(devNum,stage,command,enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/* wrapper for cpssDxChOamKeepaliveForPacketCommandEnableGet */
static CMD_STATUS wrCpssDxChOamKeepaliveForPacketCommandEnableGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    CPSS_PACKET_CMD_ENT                 command;
    GT_BOOL                             enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    stage           = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    command         = (CPSS_PACKET_CMD_ENT)inArgs[2];

    /* call cpss api function */
    rc = cpssDxChOamKeepaliveForPacketCommandEnableGet(devNum,stage,command,&enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d",enable);

    return CMD_OK;
}

/* wrapper for cmdCpssDxChOamEntrySequenceWrite */
static CMD_STATUS wrCmdCpssDxChOamEntrySequenceWrite
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           rc;
    GT_U8                               devNum;
    CPSS_DXCH_OAM_STAGE_TYPE_ENT        stage;
    GT_U32                              basePatternIndex;
    GT_U32                              incrPatternIndex;
    GT_U32                              amountToWrite;
    GT_32                               writeIndexIncr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum           = (GT_U8)inArgs[0];
    stage            = (CPSS_DXCH_OAM_STAGE_TYPE_ENT)inArgs[1];
    basePatternIndex = (GT_U32)inArgs[2];
    incrPatternIndex = (GT_U32)inArgs[3];
    amountToWrite    = (GT_U32)inArgs[4];
    writeIndexIncr   = (GT_32)inArgs[5];

    /* call cpss api function */
    rc = cmdCpssDxChOamEntrySequenceWrite(
        devNum, stage, basePatternIndex, incrPatternIndex,
        amountToWrite, writeIndexIncr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    /* start table: cpssDxChOamEntry */

    {"cpssDxChOamEntrySet",
        &wrCpssDxChOamEntrySet,
        4, 33},

    {"cpssDxChOamEntryGetFirst",
        &wrCpssDxChOamEntryGetFirst,
        4, 0},

    {"cpssDxChOamEntryGetNext",
        &wrCpssDxChOamEntryGetNext,
        4, 0},

    /* end table: cpssDxChOamEntry */

    {"cpssDxChOamOpcodeSet",
        &wrCpssDxChOamOpcodeSet,
        5, 0},
    {"cpssDxChOamOpcodeGet",
        &wrCpssDxChOamOpcodeGet,
        4, 0},

    {"cpssDxChOamOpcodeProfilePacketCommandEntrySet",
        &wrCpssDxChOamOpcodeProfilePacketCommandEntrySet,
        6, 0},
    {"cpssDxChOamOpcodeProfilePacketCommandEntryGet",
        &wrCpssDxChOamOpcodeProfilePacketCommandEntryGet,
        4, 0},

    {"cpssDxChOamTableBaseFlowIdSet",
        &wrCpssDxChOamTableBaseFlowIdSet,
        3, 0},
    {"cpssDxChOamTableBaseFlowIdGet",
        &wrCpssDxChOamTableBaseFlowIdGet,
        2, 0},

    {"cpssDxChOamDualEndedLmPacketCommandSet",
        &wrCpssDxChOamDualEndedLmPacketCommandSet,
        4, 0},
    {"cpssDxChOamDualEndedLmPacketCommandGet",
        &wrCpssDxChOamDualEndedLmPacketCommandGet,
        3, 0},

    {"cpssDxChOamTimeStampParsingEnableSet",
        &wrCpssDxChOamTimeStampParsingEnableSet,
        2, 0},
    {"cpssDxChOamTimeStampParsingEnableGet",
        &wrCpssDxChOamTimeStampParsingEnableGet,
        1, 0},

    {"cpssDxChOamTimeStampEtherTypeSet",
        &wrCpssDxChOamTimeStampEtherTypeSet,
        2, 0},
    {"cpssDxChOamTimeStampEtherTypeGet",
        &wrCpssDxChOamTimeStampEtherTypeGet,
        1, 0},

    {"cpssDxChOamOpcodeProfileDedicatedMcProfileEnSet",/* galtis length limit */
        &wrcpssDxChOamOpcodeProfileDedicatedMcProfileEnableSet,
        3, 0},
    {"cpssDxChOamOpcodeProfileDedicatedMcProfileEnGet",/* galtis length limit */
        &wrcpssDxChOamOpcodeProfileDedicatedMcProfileEnableGet,
        2, 0},

    {"cpssDxChOamExceptionConfigSet",
        &wrCpssDxChOamExceptionConfigSet,
        6, 0},
    {"cpssDxChOamExceptionConfigGet",
        &wrCpssDxChOamExceptionConfigGet,
        3, 0},

    {"cpssDxChOamExceptionGroupStatusGet",
        &wrCpssDxChOamExceptionGroupStatusGet,
        3, 0},

    {"cpssDxChOamExceptionStatusGet",
        &wrCpssDxChOamExceptionStatusGet,
        4, 0},

    {"cpssDxChOamAgingPeriodEntrySet",
        &wrCpssDxChOamAgingPeriodEntrySet,
        5, 0},

    {"cpssDxChOamAgingPeriodEntryGet",
        &wrCpssDxChOamAgingPeriodEntryGet,
        3, 0},

    {"cpssDxChOamAgingDaemonEnableSet",
        &wrCpssDxChOamAgingDaemonEnableSet,
        3, 0},

    {"cpssDxChOamAgingDaemonEnableGet",
        &wrCpssDxChOamAgingDaemonEnableGet,
        2, 0},

    {"cpssDxChOamExceptionCounterGet",
        &wrCpssDxChOamExceptionCounterGet,
        3, 0},

    {"cpssDxChOamPduCpuCodeSet",
        &wrCpssDxChOamPduCpuCodeSet,
        2, 0},

    {"cpssDxChOamPduCpuCodeGet",
        &wrCpssDxChOamPduCpuCodeGet,
        1, 0},

    {"cpssDxChOamMplsCwChannelTypeProfileSet",
        &wrCpssDxChOamMplsCwChannelTypeProfileSet,
        4, 0},

    {"cpssDxChOamMplsCwChannelTypeProfileGet",
        &wrCpssDxChOamMplsCwChannelTypeProfileGet,
        2, 0},

    {"cpssDxChOamAgingBitmapUpdateModeSet",
        &wrCpssDxChOamAgingBitmapUpdateModeSet,
        3, 0},

    {"cpssDxChOamAgingBitmapUpdateModeGet",
        wrCpssDxChOamAgingBitmapUpdateModeGet,
        1, 0},

    {"cpssDxChOamHashBitSelectionSet",
        wrCpssDxChOamHashBitSelectionSet,
        4, 0},

    {"cpssDxChOamHashBitSelectionGet",
        wrCpssDxChOamHashBitSelectionGet,
        2, 0},

    {"cpssDxChOamEnableSet",
        wrCpssDxChOamEnableSet,
        3, 0},

    {"cpssDxChOamEnableGet",
        wrCpssDxChOamEnableGet,
        2, 0},

    {"cpssDxChOamCpuCodeBaseSet",
        wrCpssDxChOamCpuCodeBaseSet,
        3, 0},

    {"cpssDxChOamCpuCodeBaseGet",
        wrCpssDxChOamCpuCodeBaseGet,
        2, 0},

    {"cpssDxChOamLmOffsetTableSet",
        wrCpssDxChOamLmOffsetTableSet,
        3, 0},

    {"cpssDxChOamLmOffsetTableGet",
        wrCpssDxChOamLmOffsetTableGet,
        2, 0},

    {"cpssDxChOamKeepaliveForPacketCommandEnableSet",
        wrCpssDxChOamKeepaliveForPacketCommandEnableSet,
        4, 0},

    {"cpssDxChOamKeepaliveForPacketCommandEnableGet",
        wrCpssDxChOamKeepaliveForPacketCommandEnableGet,
        3, 0},

    {"cmdCpssDxChOamEntrySequenceWrite",
        wrCmdCpssDxChOamEntrySequenceWrite,
        6, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChOam
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
GT_STATUS cmdLibInitCpssDxChOam
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

