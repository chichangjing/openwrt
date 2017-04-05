/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
********************************************************************************
* appDemoExMxPmExternalUtils.c
*
* DESCRIPTION:
*       App demo ExMxPm External TCAM utilities API.
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*******************************************************************************/

#include <appDemo/utils/extTcam/appDemoExMxPmExternalTcamUtils.h>
#include <appDemo/sysHwConfig/gtAppDemoSysConfig.h>

/*******************************************************************************
* appDemoWaitOnOtherCommandBusy
*
* DESCRIPTION:
*   The function waits for ETC other command execution to end
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - timout occured but device still busy
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS appDemoWaitOnOtherCommandBusy
(
    IN  GT_U8       devNum
)
{
    GT_STATUS rc;         /* return status                               */
    GT_U32 i;             /* loop parameter                              */
    GT_BOOL cmdCompleted; /* previous command execution completion state */

    for ( i = 0 ; i < 10000 ; i++ )
    {
        rc = cpssExMxPmExternalTcamIndirectCmdStatusGet(devNum, &cmdCompleted);
        if( rc != GT_OK)
            return rc;
        if(cmdCompleted == GT_TRUE)
            break;
    }

    if( i == 10000 )
        return GT_FAIL;

    return GT_OK;
}

/*******************************************************************************
* appDemoCovertDataWordsToBitLines
*
* DESCRIPTION:
*   The function convert the raw data of 5 words to 2 80 bit lines
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       dataWordPtr     - (pointer to) the 5 words data
*
* OUTPUTS:
*       hbitLinesPtr    - (pointer to) the 80 bit lines
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*
*******************************************************************************/
GT_STATUS appDemoCovertDataWordsToBitLines
(
    IN GT_U32 *dataWordPtr,
    OUT CPSS_EXMXPM_EXTERNAL_TCAM_REG* bitLinesPtr
)
{
    CPSS_NULL_PTR_CHECK_MAC(dataWordPtr);
    CPSS_NULL_PTR_CHECK_MAC(bitLinesPtr);

    bitLinesPtr[0][0] = dataWordPtr[0];
    bitLinesPtr[0][1] = dataWordPtr[1];
    bitLinesPtr[0][2] = dataWordPtr[2] & 0x0000FFFF;

    bitLinesPtr[1][0] = ( (dataWordPtr[2] & 0xFFFF0000) >> 16 ) |
                          ( (dataWordPtr[3] & 0x0000FFFF) << 16 ) ;
    bitLinesPtr[1][1] = ( (dataWordPtr[3] & 0xFFFF0000) >> 16 ) |
                          ( (dataWordPtr[4] & 0x0000FFFF) << 16 ) ;
    bitLinesPtr[1][2] = (dataWordPtr[4] & 0xFFFF0000) >> 16 ;

    return GT_OK;
}

/*******************************************************************************
* appDemoCovertMailboxToWords
*
* DESCRIPTION:
*   The function convert mailbox entry result to array of word results
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       mailboxData - mailbox entry
*       numOfWords  - the number of words in the array
*
* OUTPUTS:
*       wordPtr     - (pointer to) word array
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on null pointer
*       GT_BAD_PARAM             - on wrong parameters
*
*******************************************************************************/
GT_STATUS appDemoCovertMailboxToWords
(
    IN CPSS_EXMXPM_EXTERNAL_MAILBOX_LINE  mailboxData,
    IN GT_U32 numOfWords,
    OUT GT_U32* wordPtr
)
{
    GT_U32 i;   /* loop index */
    CPSS_NULL_PTR_CHECK_MAC(wordPtr);

    /* mailbox line is 100 bit long and each result is 25 bit long */
    for( i = 0 ; i < numOfWords ; i++ )
    {
        switch (i)
        {
            case 0: wordPtr[0] = mailboxData[0] & 0x01FFFFFF;
                    break;
            case 1: wordPtr[1] = ((mailboxData[0] & 0xFE000000) >> 25) |
                                  ((mailboxData[1] & 0x0003FFFF) << 7) ;
                    break;
            case 2: wordPtr[2] = ((mailboxData[1] & 0xFFFC0000) >> 18) |
                                  ((mailboxData[2] & 0x000007FF) << 14) ;
                    break;
            case 3: wordPtr[3] = ((mailboxData[2] & 0xFFFFF800) >> 11) |
                                  ((mailboxData[3] & 0x0000000F) << 21) ;
                    break;
            default:return GT_BAD_PARAM;
                    break;
        }
    }

    return GT_OK;
 }

/*******************************************************************************
* appDemoExMxPmExtTcamIdtLookupExecute
*
* DESCRIPTION:
*   The function execute lookup in IDT External TCAM device
*
* APPLICABLE DEVICES:  Puma2 devices
*
* INPUTS:
*       devNum              - device number
*       lookupConfigPtr     - (pointer to) search scheme parameters
*       ruleSize            - size of rule entry.
*       tcamLinesPtr        - (pointer to) data arranged in 80 bit lines.
*       resultsNum          - the number of possible results based on lookup type.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
static GT_STATUS appDemoExMxPmExtTcamIdtLookupExecute
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC *lookupConfigPtr,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_REG               *tcamLinesPtr,
    IN  GT_U32                                      resultsNum
)
{
    GT_STATUS rc;            /* return status                       */
    GT_U32    luType;        /* for ltin0 lookup value              */
    GT_U32 numOfDataLines;   /* the number of input TCAM data lines */
    /* generic instruction format */
    CPSS_EXMXPM_EXTERNAL_TCAM_GENERIC_CMD_INST_UNT  instructionFormat;

    CPSS_NULL_PTR_CHECK_MAC(lookupConfigPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamLinesPtr);

    switch(ruleSize)
    {
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E:  luType = 0x1;
                                                        numOfDataLines = 1;
                                                        break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E: luType = 0x2;
                                                        numOfDataLines = 2;
                                                        break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E: luType = 0x3;
                                                        numOfDataLines = 4;
                                                        break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E: luType = 0x5;
                                                        numOfDataLines = 8;
                                                        break;
        default: return GT_BAD_PARAM;
                 break;
    }

    instructionFormat.idtInstructionSet.ltin0 = luType;
    /* IDT lookup instruction code 3'b010 */
    instructionFormat.idtInstructionSet.inst0 = 0x2;
    instructionFormat.idtInstructionSet.ss_sel0 = lookupConfigPtr->lookupSchemeIndex;
    instructionFormat.idtInstructionSet.ltin1 = lookupConfigPtr->globalMask;
    instructionFormat.idtInstructionSet.inst1 = 0;
    instructionFormat.idtInstructionSet.ss_sel1 = lookupConfigPtr->tcamDeviceSelectBmp;

    rc = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(devNum,
                       &instructionFormat, 0, (resultsNum+1)/2, (resultsNum+1)/2,
                       numOfDataLines, &tcamLinesPtr[0]);
    if( rc != GT_OK)
        return rc;

    rc = cpssExMxPmExternalTcamIndirectCmdStart(devNum);

    return rc;
}

/*******************************************************************************
* appDemoExMxPmExtTcamNlLookupExecute
*
* DESCRIPTION:
*   The function execute lookup in Nl External TCAM device
*
* APPLICABLE DEVICES:  Puma2 devices
*
* INPUTS:
*       devNum              - device number
*       lookupConfigPtr     - (pointer to) search scheme parameters
*       ruleSize            - size of rule entry.
*       tcamLinesPtr        - (pointer to) data arranged in 80 bit lines.
*       resultsNum          - the number of possible results based on lookup type.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
static GT_STATUS appDemoExMxPmExtTcamNlLookupExecute
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC *lookupConfigPtr,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_REG               *tcamLinesPtr,
    IN  GT_U32                                      resultsNum
)
{
    GT_STATUS rc;           /* return status                            */
    GT_U32 dibusLow;        /* dibus_low signal value for the operation */
    GT_U32 numOfDataLines;  /* the number of input TCAM data lines      */
    /* generic instruction format */
    CPSS_EXMXPM_EXTERNAL_TCAM_GENERIC_CMD_INST_UNT  instructionFormat;

    CPSS_NULL_PTR_CHECK_MAC(lookupConfigPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamLinesPtr);

    /* in case of 640 bit rule size 320 bit of data should be placed in the context */
    /* buffer perior to executing the lookup command.                               */
    if( ruleSize == CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E)
    {
        /* write operation to context buffer line 2 */
        instructionFormat.nlInstructionSet.dibus_low = 0;
        instructionFormat.nlInstructionSet.dibus_high = 0;
        instructionFormat.nlInstructionSet.dctx_init = 2;

        rc = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(devNum,
                           &instructionFormat, 0, 0, 0, 2, &tcamLinesPtr[4]);
        if( rc != GT_OK)
            return rc;

        rc = cpssExMxPmExternalTcamIndirectCmdStart(devNum);
        if( rc != GT_OK)
            return rc;

        /* write operation to context buffer line 3 */
        instructionFormat.nlInstructionSet.dibus_low = 0;
        instructionFormat.nlInstructionSet.dibus_high = 0;
        instructionFormat.nlInstructionSet.dctx_init = 3;

        /* wait between adjacent commands */
        rc = appDemoWaitOnOtherCommandBusy(devNum);
        if( rc != GT_OK)
            return rc;

        rc = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(devNum,
                           &instructionFormat, 0, 0, 0, 2, &tcamLinesPtr[6]);
        if( rc != GT_OK)
            return rc;


        rc = cpssExMxPmExternalTcamIndirectCmdStart(devNum);
        if( rc != GT_OK)
            return rc;
    }

    switch(ruleSize)
    {
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E:  dibusLow = 0x4;
                                                        numOfDataLines = 1;
                                                        break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E: dibusLow = 0x4;
                                                        numOfDataLines = 2;
                                                        break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E: dibusLow = 0x8;
                                                        numOfDataLines = 4;
                                                        break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E: dibusLow = 0x8;
                                                        numOfDataLines = 4;
                                                        break;
        default: return GT_BAD_PARAM;
                 break;
    }

    /* lookup operation */
    instructionFormat.nlInstructionSet.dibus_low = dibusLow;
    /* LTR for lookup */
    instructionFormat.nlInstructionSet.dibus_high =
                    lookupConfigPtr->lookupSchemeIndex;
    instructionFormat.nlInstructionSet.dctx_init = 0;

    /* wait between adjacent commands */
    rc = appDemoWaitOnOtherCommandBusy(devNum);
        if( rc != GT_OK)
            return rc;

    rc = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(devNum,
                       &instructionFormat, 0, (resultsNum+1)/2, (resultsNum+1)/2,
                       numOfDataLines, &tcamLinesPtr[0]);
    if( rc != GT_OK)
        return rc;

    rc = cpssExMxPmExternalTcamIndirectCmdStart(devNum);

    return rc;
}

/*******************************************************************************
* appDemoExMxPmExtTcamNl11LookupExecute
*
* DESCRIPTION:
*   The function execute lookup in Nl11 External TCAM device
*
* APPLICABLE DEVICES:  Puma3 and above devices
*
* INPUTS:
*       devNum              - device number
*       lookupConfigPtr     - (pointer to) search scheme parameters
*       ruleSize            - size of rule entry.
*       tcamLinesPtr        - (pointer to) data arranged in 80 bit lines.
*       resultsNum          - the number of possible results based on lookup type.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
static GT_STATUS appDemoExMxPmExtTcamNl11LookupExecute
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC *lookupConfigPtr,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_REG               *tcamLinesPtr,
    IN  GT_U32                                      resultsNum
)
{
    GT_STATUS rc;           /* return status                            */
    GT_U32 numOfDataLines;  /* the number of input TCAM data lines      */
    GT_U32 commandOpcode;   /* operatin code                            */
    /* generic instruction format */
    CPSS_EXMXPM_EXTERNAL_TCAM_GENERIC_CMD_INST_UNT  instructionFormat;

    CPSS_NULL_PTR_CHECK_MAC(lookupConfigPtr);
    CPSS_NULL_PTR_CHECK_MAC(tcamLinesPtr);



    switch(ruleSize)
    {
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E:
            commandOpcode  = 5;
            numOfDataLines = 1;
            break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E:
            commandOpcode  = 4;
            numOfDataLines = 2;
            break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E:
            commandOpcode  = 4;
            numOfDataLines = 4;
            break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E:
            commandOpcode  = 4;
            numOfDataLines = 8;
            break;
        default: return GT_BAD_PARAM;
    }

    /* lookup operation */
    instructionFormat.nl11InstructionSet.commandOpcode = commandOpcode;
    /* LTR for lookup */
    instructionFormat.nl11InstructionSet.ltr = lookupConfigPtr->lookupSchemeIndex;
    instructionFormat.nl11InstructionSet.vBit = 0;
    instructionFormat.nl11InstructionSet.wrMode = 0;
    /* wait between adjacent commands */
    rc = appDemoWaitOnOtherCommandBusy(devNum);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(
        devNum, &instructionFormat,
        0 /*numOfNops*/,
        ((resultsNum+1)/2) /*numOfTcamCyclesResult*/,
        ((resultsNum+1)/2) /*numOfMailboxCyclesResult*/,
        numOfDataLines, &tcamLinesPtr[0]);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssExMxPmExternalTcamIndirectCmdStart(devNum);

    return rc;
}

/*******************************************************************************
* appDemoExMxPmExtTcamMgLookup
*
* DESCRIPTION:
*   The function preforms management client lookup in External TCAM device
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       lookupConfigPtr     - (pointer to) search scheme parameters
*       ruleSize            - size of rule entry.
*       dataPtr             - (pointer to) data\key for lookup action
*       maxNumOfResults     - the maximal number of results that can
*                             be retrieved.
*
* OUTPUTS:
*       resultIndexPtr      - (pointer to) array of hit\miss results.
*                             24 bit format according to TCAM datasheet
*                             (the 24th bit indicates a hit).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExtTcamMgLookup
(
    IN  GT_U8                                       devNum,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_CLIENT_CONFIG_STC *lookupConfigPtr,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_U32                                      *dataPtr,
    IN  GT_U32                                      maxNumOfResults,
    OUT GT_U32                                      *resultIndexPtr
)
{
    GT_STATUS rc;                                   /* return status                */
    CPSS_EXMXPM_EXTERNAL_TCAM_REG tcamLines[8];     /* tcam lines data              */
    CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_ENT tcamType;    /* external TCAM type           */
    CPSS_EXMXPM_EXTERNAL_MAILBOX_LINE  mailboxLine; /* mailbox entry for results    */


    CPSS_NULL_PTR_CHECK_MAC(lookupConfigPtr);
    CPSS_NULL_PTR_CHECK_MAC(dataPtr);
    CPSS_NULL_PTR_CHECK_MAC(resultIndexPtr);

    switch(ruleSize)
    {
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E:
            appDemoCovertDataWordsToBitLines(&dataPtr[15],&tcamLines[6]);
            appDemoCovertDataWordsToBitLines(&dataPtr[10],&tcamLines[4]);

        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E:
            appDemoCovertDataWordsToBitLines(&dataPtr[5],&tcamLines[2]);

        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E:
            appDemoCovertDataWordsToBitLines(&dataPtr[0],&tcamLines[0]);
            break;

        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E:
            tcamLines[0][0] = dataPtr[0];
            tcamLines[0][1] = dataPtr[1];
            tcamLines[0][2] = dataPtr[2] & 0x0000FFFF;
            break;

        default:
            return GT_BAD_PARAM;

    }

    rc = cpssExMxPmExternalTcamTypeGet(devNum, 0, &tcamType);
    if( rc != GT_OK)
        return rc;

    if (PRV_CPSS_PP_MAC(devNum)->devFamily <= CPSS_PP_FAMILY_PUMA_E)
    {
        /* Puma2 only */
        if(tcamType == CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_NL_E)
            rc = appDemoExMxPmExtTcamNlLookupExecute(devNum,
                                                     lookupConfigPtr,
                                                     ruleSize,
                                                     tcamLines,
                                                     maxNumOfResults);
        else /* tcamType == CPSS_EXMXPM_EXTERNAL_TCAM_TYPE_IDT_E */
            rc = appDemoExMxPmExtTcamIdtLookupExecute(devNum,
                                                      lookupConfigPtr,
                                                      ruleSize,
                                                      tcamLines,
                                                      maxNumOfResults);
    }
    else
    {
        /* Puma3 and above */
        rc = appDemoExMxPmExtTcamNl11LookupExecute(devNum,
                                                 lookupConfigPtr,
                                                 ruleSize,
                                                 tcamLines,
                                                 maxNumOfResults);
    }
    if( rc != GT_OK)
        return rc;

    /* wait for lookup execution end */
    rc = appDemoWaitOnOtherCommandBusy(devNum);
    if( rc != GT_OK)
        return rc;

    rc = cpssExMxPmExternalTcamIndirectCmdResultGet(devNum, 1, &mailboxLine);
    if( rc != GT_OK)
        return rc;

    rc = appDemoCovertMailboxToWords(mailboxLine, maxNumOfResults, resultIndexPtr);

    return rc;
}

/*************************************************************************
* appDemoExMxPmExtIdtTcamRepeat
*
* DESCRIPTION:
*       Execute IDT TCAM repeat operation
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       repeatCmd       - configuration for the repeat operation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_OUT_OF_RANGE          - value is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExtIdtTcamRepeatOperationExecute
(
    IN  GT_U8                                        devNum,
    IN  GT_U32                                       tcamId,
    IN  APP_DEMO_EXTERNAL_TCAM_IDT_REPEAT_CMD_STC    repeatCmd
)
{
    GT_STATUS rc;                               /* return status                              */
    CPSS_EXMXPM_EXTERNAL_TCAM_REG  repeatData;  /* repeat command data                        */
    /* loop parameters - used for number of idle cycles needed after command */
    CPSS_EXMXPM_EXTERNAL_TCAM_LOOP_PARAMS_STC loopParams;
    /* generic instruction format */
    CPSS_EXMXPM_EXTERNAL_TCAM_GENERIC_CMD_INST_UNT  instructionFormat;


    /* repeat instruction format */
    instructionFormat.idtInstructionSet.ltin0 = 0x0;
    /* IDT lookup instruction code 3'b100 */
    instructionFormat.idtInstructionSet.inst0 = 0x4;
    instructionFormat.idtInstructionSet.ss_sel0 = 0x0;
    instructionFormat.idtInstructionSet.ltin1 = 0x0;
    instructionFormat.idtInstructionSet.inst1 = 0x0;
    instructionFormat.idtInstructionSet.ss_sel1 = (1<<tcamId);

    if( repeatCmd.srcAddr >= BIT_17 )
    {
        return GT_OUT_OF_RANGE;
    }
    /* bits[16:0] - SrcAdr */
    repeatData[0] = repeatCmd.srcAddr & 0x1FFFF;
    /* bit[30] - CntDir */
    repeatData[0] |= ((repeatCmd.countDir ==
                      APP_DEMO_EXTERNAL_TCAM_IDT_REP_DIRECTION_DEC_E) ? ( 1 << 30 ) : 0 );
    if( repeatCmd.dstAddr >= BIT_17 )
    {
        return GT_OUT_OF_RANGE;
    }
    /* bits[56:40] - DesAdr */
    repeatData[1] = ((repeatCmd.dstAddr & 0x1FFFF) << 8);
    if( repeatCmd.repeatCount >= BIT_12 )
    {
        return GT_OUT_OF_RANGE;
    }
    /* bits[75:64] - LoopCnt */
    repeatData[2] = repeatCmd.repeatCount & 0xFFF;

    /* bits[29:28] -  OpType */
    /* Idle Cycle Requirements:                */
    /* Copy                - 10+(loopCnt+1)*4  */
    /* Move                - 10+(loopCnt+1)*7  */
    /* Clear               -  0+(loopCnt+1)*2  */
    /* Validate\invalidate - 10+(loopCnt+1)*4  */
    switch(repeatCmd.cmdOpCode)
    {
        case APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_COPY_E:
                                                       loopParams.loopCount = 10+(repeatData[2]+1)*4;
                                                       break;

        case APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_MOVE_E: repeatData[0] |= ( 1 << 28);
                                                       loopParams.loopCount = 10+(repeatData[2]+1)*7;
                                                       break;

        case APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_CLEAR_E: repeatData[0] |= ( 2 << 28);
                                                        loopParams.loopCount = (repeatData[2]+1)*2;
                                                        break;

        case APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_VALIDATE_E: repeatData[0] |= (1<<31); /* bit[31] - Validate */
        case APP_DEMO_EXTERNAL_TCAM_IDT_REP_OP_INVALIDATE_E:
                                                            repeatData[0] |= ( 3 << 28);
                                                            loopParams.loopCount = 10+(repeatData[2]+1)*4;
                                                            break;
        default: return GT_BAD_PARAM;
                 break;
    }

    rc = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(devNum,
                       &instructionFormat, 0, 0, 0, 1, &repeatData);
    if( rc != GT_OK)
        return rc;

    rc = cpssExMxPmExternalTcamIndirectCmdStart(devNum);
    if( rc != GT_OK)
        return rc;

    /* wait before idle cycles */
    rc = appDemoWaitOnOtherCommandBusy(devNum);
    if( rc != GT_OK)
        return rc;

    /* nop command params */
    rc = cpssExMxPmExternalTcamIndirectCmdParamsSet(devNum, tcamId,
                       CPSS_EXMXPM_EXTERNAL_TCAM_OP_CODE_NOP_E, 0, NULL);
    if( rc != GT_OK)
        return rc;

    /* loop params for nop command */
    loopParams.loopInfinitely = GT_FALSE;
    rc = cpssExMxPmExternalTcamIndirectCmdLoopParamsSet(devNum,
                                                        &loopParams);
    if( rc != GT_OK)
        return rc;

    rc = cpssExMxPmExternalTcamIndirectCmdStart(devNum);
    if( rc != GT_OK)
        return rc;

    /* wait before clearing loop param */
    rc = appDemoWaitOnOtherCommandBusy(devNum);
    if( rc != GT_OK)
        return rc;

    loopParams.loopInfinitely = GT_FALSE;
    loopParams.loopCount = 1;
    rc = cpssExMxPmExternalTcamIndirectCmdLoopParamsSet(devNum,
                                                        &loopParams);

    return rc;
}

/*************************************************************************
* appDemoExMxPmExtIdtTcamNfa
*
* DESCRIPTION:
*       Execute IDT TCAM Next Free Address (NFA) operation
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       tcamId              - cascaded TCAM Id 0..3
*       sssRegNum           - selected Search Scheme register.
*       maxNumOfResults     - the maximal number of NFA
*                             results that can be retrieved.
*
* OUTPUTS:
*       nfaResultPtr        - (pointer to) array of NFA results.
*                             24 bit format according to TCAM datasheet
*                             (the 24th bit indicates a "NFA hit").
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExtIdtTcamNfa
(
    IN  GT_U8       devNum,
    IN  GT_U32      tcamId,
    IN  GT_U8       sssRegNum,
    IN  GT_U32      maxNumOfResults,
    OUT GT_U32      *nfaResultPtr
)
{
    GT_STATUS rc;                                   /* return status                */
    CPSS_EXMXPM_EXTERNAL_MAILBOX_LINE  mailboxLine; /* mailbox entry for results    */
    /* generic instruction format */
    CPSS_EXMXPM_EXTERNAL_TCAM_GENERIC_CMD_INST_UNT  instructionFormat;

    CPSS_NULL_PTR_CHECK_MAC(nfaResultPtr);

    instructionFormat.idtInstructionSet.ltin0 = 0x0;
    /* IDT NFA instruction code 3'b110 */
    instructionFormat.idtInstructionSet.inst0 = 0x6;
    instructionFormat.idtInstructionSet.ss_sel0 = sssRegNum;
    instructionFormat.idtInstructionSet.ltin1 = 0x0;
    instructionFormat.idtInstructionSet.inst1 = 0x0;
    instructionFormat.idtInstructionSet.ss_sel1 = (1 << tcamId);

    rc = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(devNum,
                       &instructionFormat, 0, (maxNumOfResults+1)/2, (maxNumOfResults+1)/2, 0, NULL);
    if( rc != GT_OK)
        return rc;

    rc = cpssExMxPmExternalTcamIndirectCmdStart(devNum);
    if( rc != GT_OK)
        return rc;

    /* wait for NFA execution end */
    rc = appDemoWaitOnOtherCommandBusy(devNum);
    if( rc != GT_OK)
        return rc;

    rc = cpssExMxPmExternalTcamIndirectCmdResultGet(devNum, 1, &mailboxLine);
    if( rc != GT_OK)
        return rc;

    rc = appDemoCovertMailboxToWords(mailboxLine, maxNumOfResults, nfaResultPtr);

    return rc;
}

/*************************************************************************
* appDemoExMxPmExternalIdtTcamAgeFifoEntryGet
*
* DESCRIPTION:
*       Retrieve one index of entry to be aged out for each TCAM device
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - device number
*       invalidateMode      - how entry to be aged out should be invalidate.
*       maxNumOfEntries     - the maximal number of aged out indexes that
*                             can be retrieved.
*
* OUTPUTS:
*       agesOutPtr          - (pointer to) array which contains the indexes
*                             of entries to be aged out.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalIdtTcamAgeFifoEntryGet
(
    IN  GT_U8                                        devNum,
    IN  APP_DEMO_EXTERNAL_TCAM_IDT_AGED_OUT_OP_ENT   invalidateMode,
    IN  GT_U32                                       maxNumOfEntries,
    OUT GT_U32                                       *agesOutPtr
)
{
    GT_STATUS rc;                                   /* return status                */
    CPSS_EXMXPM_EXTERNAL_MAILBOX_LINE  mailboxLine; /* mailbox entry for results    */
    /* generic instruction format */
    CPSS_EXMXPM_EXTERNAL_TCAM_GENERIC_CMD_INST_UNT  instructionFormat;

    CPSS_NULL_PTR_CHECK_MAC(agesOutPtr);

    if(invalidateMode == APP_DEMO_EXTERNAL_TCAM_IDT_AGED_OUT_OP_BY_PP)
       instructionFormat.idtInstructionSet.ltin0 = 0x1;
    else /* invalidateMode == APP_DEMO_EXTERNAL_TCAM_IDT_AGED_OUT_OP_BY_TCAM */
       instructionFormat.idtInstructionSet.ltin0 = 0x0;
    /* IDT Aging instruction code 3'b111*/
    instructionFormat.idtInstructionSet.inst0 = 0x7;
    instructionFormat.idtInstructionSet.ss_sel0 = 0x0;
    instructionFormat.idtInstructionSet.ltin1 = 0x0;
    instructionFormat.idtInstructionSet.inst1 = 0x0;
    instructionFormat.idtInstructionSet.ss_sel1 = 0xF;

    rc = cpssExMxPmExternalTcamIndirectCmdGenericParamsSet(devNum,
                       &instructionFormat, 1, 2, 2, 0, NULL);
    if( rc != GT_OK)
        return rc;

    rc = cpssExMxPmExternalTcamIndirectCmdStart(devNum);
    if( rc != GT_OK)
        return rc;

    /* wait for Aging execution end */
    rc = appDemoWaitOnOtherCommandBusy(devNum);
    if( rc != GT_OK)
        return rc;

    rc = cpssExMxPmExternalTcamIndirectCmdResultGet(devNum, 1, &mailboxLine);
    if( rc != GT_OK)
        return rc;

    rc = appDemoCovertMailboxToWords(mailboxLine, maxNumOfEntries, agesOutPtr);

    return rc;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamRuleStateGet
*
* DESCRIPTION:
*   The function gets rule status in External TCAM device
*
* APPLICABLE DEVICES:
*        Puma2; Puma3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       ruleStartIndex  - index of rule in external TCAM.
*
* OUTPUTS:
*       validPtr        - (pointer to)Rule's valid status:
*                         GT_TRUE - valid rule
*                         GT_FALSE - invalid rule
*       ageEnablePtr    - (pointer to)Rule's aging enable status
*                         Relevant only if aging function is enabled and configured:
*                         GT_TRUE - aging is enabled for rule
*                         GT_FALSE - aging is disabled for rule.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
 COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamRuleStateGet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      tcamId,
    IN  GT_U32                                      ruleStartIndex,
    OUT GT_BOOL                                     *validPtr,
    OUT GT_BOOL                                     *ageEnablePtr
)
{
    GT_U32    xRuleBuf[3];     /* X rule buffer */
    GT_U32    yRuleBuf[3];     /* Y rule buffer */

    return cpssExMxPmExternalTcamRuleRead(
        devNum, tcamId, ruleStartIndex,
        CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E,
        80 /*dataSize*/,
        CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E,
        validPtr, ageEnablePtr,
        xRuleBuf, yRuleBuf);
}

/*******************************************************************************
* appDemoExMxPmExternalTcamRuleValidStatusSet
*
* DESCRIPTION:
*   The function sets rule status in External TCAM device
*
* APPLICABLE DEVICES:
*        Puma2; Puma3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum          - device number
*       tcamId          - cascaded TCAM Id 0..3
*       ruleStartIndex  - index of rule in external TCAM.
*       ruleSize        - size of rule entry.
*       valid           - Rule's valid status:
*                         GT_TRUE - valid rule
*                         GT_FALSE - invalid rule
*       ageEnable       - Rule's aging enable status
*                         Relevant only if aging function is enabled and configured:
*                         GT_TRUE - aging is enabled for rule
*                         GT_FALSE - aging is disabled for rule.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
 COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamRuleValidStatusSet
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      tcamId,
    IN  GT_U32                                      ruleStartIndex,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_BOOL                                     valid,
    IN  GT_BOOL                                     ageEnable
)
{
    GT_STATUS rc;              /* return code             */
    GT_U32    xRuleBuf[3];     /* X rule buffer           */
    GT_U32    yRuleBuf[3];     /* Y rule buffer           */
    GT_BOOL   foundValid;      /* found Valid state       */
    GT_BOOL   foundAgeEnable;  /* found Age Enable state  */
    GT_U32    ruleSegIdx;      /* rule segment index      */
    GT_U32    ruleSegNum;      /* number of rule segments */

    switch (ruleSize)
    {
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E:
            ruleSegNum = 1;
            break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E:
            ruleSegNum = 2;
            break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E:
            ruleSegNum = 4;
            break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E:
            ruleSegNum = 8;
            break;
        default: return GT_BAD_PARAM;
    }

    for (ruleSegIdx = 0; (ruleSegIdx < ruleSegNum); ruleSegIdx++)
    {
        rc = cpssExMxPmExternalTcamRuleRead(
            devNum, tcamId, (ruleStartIndex + ruleSegIdx),
            CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E,
            80 /*dataSize*/,
            CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E,
            &foundValid, &foundAgeEnable,
            xRuleBuf, yRuleBuf);
        if (rc != GT_OK)
        {
            return rc;
        }

        rc = cpssExMxPmExternalTcamRuleWrite(
            devNum, tcamId, (ruleStartIndex + ruleSegIdx),
            CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_80_E,
            80 /*dataSize*/,
            valid, ageEnable,
            CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E,
            xRuleBuf, yRuleBuf);
        if (rc != GT_OK)
        {
            return rc;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* appDemoExMxPmExternalTcamRuleAndActionCopy
*
* DESCRIPTION:
*   The function copies Rule and Action in External TCAM device to the new place
*
* APPLICABLE DEVICES:
*        Puma2; Puma3.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum              - device number
*       tcamId              - cascaded TCAM Id 0..3
*       ruleSize            - size of rule entry.
*       actionSize          - action size in bits
*       srcRuleStartIndex   - index of source rule in external TCAM.
*       dstRuleStartIndex   - index of destination rule in external TCAM.
*       srcActionStartIndex - index of source rule action.
*       dstActionStartIndex - index of destination rule action.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
 COMMENTS:
*
*******************************************************************************/
GT_STATUS appDemoExMxPmExternalTcamRuleAndActionCopy
(
    IN  GT_U8                                       devNum,
    IN  GT_U32                                      tcamId,
    IN  CPSS_EXMXPM_CONTROL_MEMORY_ENT              actionCsu,
    IN  CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_ENT     ruleSize,
    IN  GT_U32                                      actionSize,
    IN  GT_U32                                      srcRuleStartIndex,
    IN  GT_U32                                      dstRuleStartIndex,
    IN  GT_U32                                      srcActionStartIndex,
    IN  GT_U32                                      dstActionStartIndex
)
{
    GT_STATUS rc;              /* return code            */
    GT_U32    xRuleBuf[20];    /* X rule buffer          */
    GT_U32    yRuleBuf[20];    /* Y rule buffer          */
    GT_U32    actionBuf[12];   /* Y rule buffer          */
    GT_BOOL   foundValid;      /* found Valid state      */
    GT_BOOL   foundAgeEnable;  /* found Age Enable state */
    GT_U32    dataSize;        /* dataSize in bits       */

    switch (ruleSize)
    {
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_160_E:
            dataSize = 160;
            break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_320_E:
            dataSize = 320;
            break;
        case CPSS_EXMXPM_EXTERNAL_TCAM_RULE_SIZE_640_E:
            dataSize = 640;
            break;
        default: return GT_BAD_PARAM;
    }

    rc = cpssExMxPmExternalTcamRuleRead(
        devNum, tcamId, srcRuleStartIndex, ruleSize, dataSize,
        CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E,
        &foundValid, &foundAgeEnable,
        xRuleBuf, yRuleBuf);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssExMxPmExternalTcamActionRead(
        devNum, actionCsu, srcActionStartIndex, actionSize, actionBuf);
    if (rc != GT_OK)
    {
        return rc;
    }

    rc = cpssExMxPmExternalTcamRuleWrite(
        devNum, tcamId, dstRuleStartIndex, ruleSize, dataSize,
        foundValid, foundAgeEnable,
        CPSS_EXMXPM_EXTERNAL_TCAM_RULE_FORMAT_X_Y_E,
        xRuleBuf, yRuleBuf);
    if (rc != GT_OK)
    {
        return rc;
    }

    return cpssExMxPmExternalTcamActionWrite(
        devNum, actionCsu, dstActionStartIndex, actionSize, actionBuf);
}

