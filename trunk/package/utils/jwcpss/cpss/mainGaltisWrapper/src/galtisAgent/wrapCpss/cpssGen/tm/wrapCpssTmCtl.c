/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssTmCtl.c
*
* DESCRIPTION:
*       Wrapper functions for TM configuration library control interface. 
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
#include <cpss/generic/tm/cpssTmCtl.h>
#include <cpss/generic/tm/prvCpssTmCtl.h>

/*******************************************************************************
* prvCpssTmRegisterIncrement
*
* DESCRIPTION:
*       Increment 64 bit. 
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       regAddrPtr       - Register address to increment.
*       number        - the number to be added
*
*
* RETURNS:
*       None.
*       
* COMMENTS:
*       None.
*******************************************************************************/
static GT_VOID prvCpssTmRegisterIncrement
(
    IN  GT_U64     *regAddrPtr,
    IN  GT_U32      number
)
{
    GT_U32 newLow;
    GT_U32 i;
    for(i=0; i<number; i++)
    {
        newLow = regAddrPtr->l[0] + 1;
        if (newLow < regAddrPtr->l[0])
        {
            regAddrPtr->l[0] = 0;
            regAddrPtr->l[1] += 1;
        }
        else
        {
            regAddrPtr->l[0] = newLow;
        }
    }
}

/*******************************************************************************
* cpssTmInit
*
* DESCRIPTION:
*       Initialize the TM configuration library.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on hardware error.
*       GT_NO_RESOURCE              - on out of memory space.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmCtlLibInit
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssTmInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmCtlLibClose
*
* DESCRIPTION:
*       Close the TM configuration library.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on hardware error.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmCtlLibClose
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssTmClose(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssTmCtlReadRegister
*
* DESCRIPTION:
*       Read register.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       regAddr         - Register address.
*       dataPtr         - Pointer to read data.
* 
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
static CMD_STATUS wrCpssTmCtlReadRegister
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U64    regAddr;
    GT_U64    dataPtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    regAddr.l[0]= (GT_U32)inArgs[1];
    regAddr.l[1]= (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssTmCtlReadRegister(devNum, regAddr, &dataPtr);

    /* pack output arguments to galtis string */
     galtisOutput(outArgs, result, "%d%d", 
                 dataPtr.l[0],dataPtr.l[1]);

    return CMD_OK;
}
/*******************************************************************************
* cpssTmCtlWriteRegister
*
* DESCRIPTION:
*       Write register.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       regAddr         - Register address.
*       dataPtr         - Pointer to write data.
* 
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
static CMD_STATUS wrCpssTmCtlWriteRegister
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U64    regAddr;
    GT_U64    dataPtr;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    regAddr.l[0]= (GT_U32)inArgs[1];
    regAddr.l[1]= (GT_U32)inArgs[2];
    dataPtr.l[0]= (GT_U32)inArgs[3];
    dataPtr.l[1]= (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssTmCtlWriteRegister(devNum, regAddr, &dataPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/********************************************************************/

static GT_U64 currentAddress;
static GT_U32 numOfWords;
/*******************************************************************************
* cpssTmDumpMemorySet
*
* DESCRIPTION:
*       set data to memory.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       regAddr         - Register address.
*       numOfWords      - Number of Words (4 byte) to write.
* 
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
static CMD_STATUS wrCpssTmDumpMemorySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8 devNum;
    GT_U32 numOfWords;
    GT_U64 regAddr;
    GT_STATUS status;
    GT_U64 data1;
    GT_U64 data2;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];
    regAddr.l[0] = (GT_U32)inArgs[1];
    regAddr.l[1] = (GT_U32)inArgs[2];
    numOfWords = (GT_U32)inArgs[3];

    /* https://jirail.marvell.com/browse/CPSS-4037 
       round num of words to multiplication of 4 */
    numOfWords = (numOfWords + 3) & 0xFFFFFFFC;

    data1.l[0] = (GT_U32)inFields[2];
    data1.l[1] = (GT_U32)inFields[3];
    data2.l[0] = (GT_U32)inFields[4];
    data2.l[1] = (GT_U32)inFields[5];

    status = cpssTmCtlWriteRegister(devNum, regAddr, &data1);
    if (status) 
    {
        galtisOutput(outArgs, status, "");
        return CMD_OK;
    } 

    if (numOfWords > 4) 
    {
        prvCpssTmRegisterIncrement(&regAddr, 8);
        status = cpssTmCtlWriteRegister(devNum, regAddr, &data2);
    }

    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssTmDumpMemoryGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8  devNum;
    GT_STATUS status;
    GT_U64 data1;
    GT_U64 data2;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (numOfWords <= 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    devNum = (GT_U8)inArgs[0];

    status = cpssTmCtlReadRegister(devNum, currentAddress, &data1);
    if(status != GT_OK)
    {
        galtisOutput(outArgs, status, "");
        return CMD_OK;
    }

    inFields[0] = currentAddress.l[0];
    inFields[1] = currentAddress.l[1];

    prvCpssTmRegisterIncrement(&currentAddress, 4);

    status = cpssTmCtlReadRegister(devNum, currentAddress, &data2);

    if(status != GT_OK)
    {
        galtisOutput(outArgs, status, "");
        return CMD_OK;
    }

    prvCpssTmRegisterIncrement(&currentAddress, 4);

    
    inFields[2] = data1.l[0];
    inFields[3] = data1.l[1];
    inFields[4] = data2.l[0];
    inFields[5] = data2.l[1];
    numOfWords -= 4;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                inFields[3], inFields[4], inFields[5]);
    galtisOutput(outArgs, status, "%f");

    return CMD_OK;
}
/*******************************************************************************
* wrCpssTmDumpMemoryGetFirst
*
* DESCRIPTION:
*       get data from memory.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum          - Device number.
*       regAddr         - Register address.
*       numOfWords      - Number of Words (4 byte) to write.
* 
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*******************************************************************************/
static CMD_STATUS wrCpssTmDumpMemoryGetFirst
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

    currentAddress.l[0] = (GT_U32)inArgs[1];
    currentAddress.l[1] = (GT_U32)inArgs[2];
    numOfWords = (GT_U32)inArgs[3];

    /* https://jirail.marvell.com/browse/CPSS-4037 
       round num of words to multiplication of 4 */
    numOfWords = (numOfWords + 3) & 0xFFFFFFFC;

    return wrCpssTmDumpMemoryGetNext(
        inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssTmCtlHwInit
*
* DESCRIPTION:
*       Initialize the TM HW configuration.
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on hardware error.
*       GT_NO_RESOURCE              - on out of memory space.
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssTmCtlHwInit
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8                   numOfLad;
/*
	CPSS_DRAM_FREQUENCY_ENT                   frequency;
*/
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    numOfLad= (GT_U8)inArgs[1];
/*
    frequency = (CPSS_DRAM_FREQUENCY_ENT)inArgs[2];
*/
    /* call cpss api function */
    result = prvCpssTmCtlHwInit(devNum, numOfLad);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{


    {"cpssTmCtlLibInit",
        &wrCpssTmCtlLibInit,
        1, 0},

    {"cpssTmCtlLibClose",
        &wrCpssTmCtlLibClose,
        1, 0},
    {"cpssTmCtlReadRegister",
        &wrCpssTmCtlReadRegister,
        3, 0},
    {"cpssTmCtlWriteRegister",
        &wrCpssTmCtlWriteRegister,
        5, 0},
    {"cpssTmDumpMemorySet",
        &wrCpssTmDumpMemorySet,
        4, 6},
    {"cpssTmDumpMemoryGetFirst",
        &wrCpssTmDumpMemoryGetFirst,
        4, 0},
    {"cpssTmDumpMemoryGetNext",
        &wrCpssTmDumpMemoryGetNext,
        4, 0},
    {"cpssTmCtlHwInit",
        &wrCpssTmCtlHwInit,
        3, 0}
   

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssTmCtl
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
GT_STATUS cmdLibInitCpssTmCtl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

