/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssTmNodesStatus.c
*
* DESCRIPTION:
*       Wrapper functions for TM Shaping APIs. 
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
#include <cpss/generic/tm/cpssTmNodesStatus.h>
#include <cpss/generic/tm/cpssTmPublicDefs.h>

/*******************************************************************************
* cpssTmQueueStatusRead
*
* DESCRIPTION:
*       Read Queue status. 
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*
* OUTPUTS:
*       statusPtr       - (pointer to) Node status structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmQueueStatusRead
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                        devNum;
    GT_U32            index;
    CPSS_TM_NODE_STATUS_STC      status;

   
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
   
    /* call cpss api function */
    result = cpssTmQueueStatusRead(devNum, index, &status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", status.maxBucketLevel,status.minBucketLevel,status.deficit );

    return CMD_OK;
}
/*******************************************************************************
* cpssTmAnodeStatusRead
*
* DESCRIPTION:
*       Read A-Node status. 
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*
* OUTPUTS:
*       statusPtr       - (pointer to) Node status structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmAnodeStatusRead
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                        devNum;
    GT_U32            index;
    CPSS_TM_NODE_STATUS_STC      status;

   
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
   
    /* call cpss api function */
    result = cpssTmAnodeStatusRead(devNum, index, &status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", status.maxBucketLevel,status.minBucketLevel,status.deficit );

    return CMD_OK;
}


/*******************************************************************************
* cpssTmBnodeStatusRead
*
* DESCRIPTION:
*       Read B-Node status. 
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*
* OUTPUTS:
*       statusPtr       - (pointer to) Node status structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmBnodeStatusRead
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                        devNum;
    GT_U32            index;
    CPSS_TM_NODE_STATUS_STC      status;

   
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
   
    /* call cpss api function */
    result = cpssTmBnodeStatusRead(devNum, index, &status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", status.maxBucketLevel,status.minBucketLevel,status.deficit );

    return CMD_OK;
}

/*******************************************************************************
* cpssTmCnodeStatusRead
*
* DESCRIPTION:
*       Read C-Node status. 
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*
* OUTPUTS:
*       statusPtr       - (pointer to) Node status structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmCnodeStatusRead
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                        devNum;
    GT_U32            index;
    CPSS_TM_NODE_STATUS_STC      status;

   
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
   
    /* call cpss api function */
    result = cpssTmCnodeStatusRead(devNum, index, &status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", status.maxBucketLevel,status.minBucketLevel,status.deficit );

    return CMD_OK;
}

/*******************************************************************************
* cpssTmPortStatusRead
*
* DESCRIPTION:
*       Read Port status. 
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       index           - Node index.
*
* OUTPUTS:
*       statusPtr       - (pointer to) Node status structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmPortStatusRead
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                        devNum;
    GT_U32            index;
    CPSS_TM_PORT_STATUS_STC      status;
  
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
   
    /* call cpss api function */
    result = cpssTmPortStatusRead(devNum, index, &status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d%d", status.maxBucketLevel,status.minBucketLevel,status.deficitArr[0],
                 status.deficitArr[1],status.deficitArr[2],status.deficitArr[3],status.deficitArr[4],
                 status.deficitArr[5],status.deficitArr[6],status.deficitArr[7]);

    return CMD_OK;
}
/*******************************************************************************
* cpssTmDropQueueLengthGet
*
* DESCRIPTION:
*       Read Queue drop length. 
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       level           - Node level (Port/C/B/A/Queue).
*       index           - Node index.
*
* OUTPUTS:
*       instantQueueLengthPtr       - (pointer to) Current Queue length structure.
*       averageQueueLengthPtr       - (pointer to) Average Queue length structure.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmDropQueueLengthGet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                       devNum;
    CPSS_TM_LEVEL_ENT           level;
    GT_U32                      index;
    GT_U32                      instantQueueLength;
    GT_U32                      averageQueueLength;

  
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    level = (GT_U32)inArgs[1];
    index = (GT_U32)inArgs[2];
   
    /* call cpss api function */
    result = cpssTmDropQueueLengthGet(devNum, level, index, &instantQueueLength, &averageQueueLength);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", instantQueueLength,averageQueueLength);
 
    return CMD_OK;
}




/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssTmQueueStatusRead",
        &wrCpssTmQueueStatusRead,
        2, 0},

    {"cpssTmAnodeStatusRead",
        &wrCpssTmAnodeStatusRead,
        2, 0},

    {"cpssTmBnodeStatusRead",
        &wrCpssTmBnodeStatusRead,
        2, 0},

    {"cpssTmCnodeStatusRead",
        &wrCpssTmCnodeStatusRead,
        2, 0},

    {"cpssTmPortStatusRead",
        &wrCpssTmPortStatusRead,
        2, 0},

    {"cpssTmDropQueueLengthGet",
        &wrCpssTmDropQueueLengthGet,
        3, 0}      
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssTmNodesStatus
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
GT_STATUS cmdLibInitCpssTmNodesStatus
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}
