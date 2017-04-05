/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssTmEligPrioFunc.c
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
#include <cmdShell/common/cmdWrapUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>

/* Feature specific includes. */
#include <cpss/generic/tm/cpssTmEligPrioFunc.h>
#include <cpss/generic/tm/cpssTmPublicDefs.h>

static GT_U32                          index = 0;
static CPSS_TM_ELIG_PRIO_FUNC_STC      funcOutArr;
static GT_STATUS   res;
/*******************************************************************************
* cpssTmEligPrioFuncQueueConfig
*
* DESCRIPTION:
*       Configure the Queue Eligible Priority Function according 
*       to the User Application parameters
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*       eligPrioFuncPtr   - The new configured eligible function pointer.
*       funcOutArr        - The Eligible Priority Function structure array pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       	The use of Eligible Function Pointer 63 on both Queue and Node level is forbidden. 
*			Eligible Function 63 is reserved to indicate that Queue/Node is not eligible.
*******************************************************************************/
static CMD_STATUS wrCpssTmEligPrioFuncQueueConfig
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8               devNum;
    GT_U32       eligPrioFunc;    
    
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (index >= 4)
    {
        galtisOutput(outArgs, GT_FULL, "%d",-1);/* Error ! */
        return CMD_OK;
    }

    /* map input arguments to locals */
    funcOutArr.queueEligPrioFunc[index].maxTb = (GT_U32)inFields[0];
    funcOutArr.queueEligPrioFunc[index].minTb = (GT_U32)inFields[1];
    funcOutArr.queueEligPrioFunc[index].propPrio = (GT_U32)inFields[2];
    funcOutArr.queueEligPrioFunc[index].schedPrio = (GT_U32)inFields[3];
    funcOutArr.queueEligPrioFunc[index].elig = (GT_U32)inFields[4];
    
    index++;

    if(index == 4)/* end registration*/
    {
         /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];
        eligPrioFunc = (GT_U32)inArgs[1];
       
        /* call cpss api function */
        result = cpssTmEligPrioFuncQueueConfig(devNum,eligPrioFunc,&funcOutArr);
        res= result;

        galtisOutput(outArgs, GT_OK, "");
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


static CMD_STATUS wrCpssTmEligPrioFuncQueueConfigFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    index=0;
    return wrCpssTmEligPrioFuncQueueConfig(inArgs,inFields,numFields,outArgs);
}


static CMD_STATUS wrCpssTmEligPrioFuncQueueConfigEnd
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     /* pack output arguments to galtis string */
    if (res!= GT_OK)
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmEligPrioFuncNodeConfig
*
* DESCRIPTION:
*       Configure the Node Eligible Priority Function according 
*       to the User Application parameters
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*       eligPrioFuncPtr   - The new configured eligible function pointer.
*       level             - A level to configure the Eligible function with.
*       funcOutArr        - The Eligible Priority Function structure array pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       	The use of Eligible Function Pointer 63 on both Queue and Node level is forbidden. 
*			Eligible Function 63 is reserved to indicate that Queue/Node is not eligible.
*******************************************************************************/
static CMD_STATUS wrCpssTmEligPrioFuncNodeConfig
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                        devNum;
    GT_U32      eligPrioFunc;
    CPSS_TM_LEVEL_ENT    level;
    
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (index >= 8)
    {
        galtisOutput(outArgs, GT_FULL, "%d",-1);/* Error ! */
        return CMD_OK;
    }

    /* map input arguments to locals */
    funcOutArr.nodeEligPrioFunc[index][0].maxTb = (GT_U32)inFields[0];
    funcOutArr.nodeEligPrioFunc[index][0].minTb = (GT_U32)inFields[1];
    funcOutArr.nodeEligPrioFunc[index][0].propPrio = (GT_U32)inFields[2];
    funcOutArr.nodeEligPrioFunc[index][0].schedPrio = (GT_U32)inFields[3];
    funcOutArr.nodeEligPrioFunc[index][0].elig = (GT_U32)inFields[4];
    funcOutArr.nodeEligPrioFunc[index][1].maxTb = (GT_U32)inFields[5];
    funcOutArr.nodeEligPrioFunc[index][1].minTb = (GT_U32)inFields[6];
    funcOutArr.nodeEligPrioFunc[index][1].propPrio = (GT_U32)inFields[7];
    funcOutArr.nodeEligPrioFunc[index][1].schedPrio = (GT_U32)inFields[8];
    funcOutArr.nodeEligPrioFunc[index][1].elig = (GT_U32)inFields[9];
    funcOutArr.nodeEligPrioFunc[index][2].maxTb = (GT_U32)inFields[10];
    funcOutArr.nodeEligPrioFunc[index][2].minTb = (GT_U32)inFields[11];
    funcOutArr.nodeEligPrioFunc[index][2].propPrio = (GT_U32)inFields[12];
    funcOutArr.nodeEligPrioFunc[index][2].schedPrio = (GT_U32)inFields[13];
    funcOutArr.nodeEligPrioFunc[index][2].elig = (GT_U32)inFields[14];
    funcOutArr.nodeEligPrioFunc[index][3].maxTb = (GT_U32)inFields[15];
    funcOutArr.nodeEligPrioFunc[index][3].minTb = (GT_U32)inFields[16];
    funcOutArr.nodeEligPrioFunc[index][3].propPrio = (GT_U32)inFields[17];
    funcOutArr.nodeEligPrioFunc[index][3].schedPrio = (GT_U32)inFields[18];
    funcOutArr.nodeEligPrioFunc[index][3].elig = (GT_U32)inFields[19];

    index++;

    if(index == 8)/* end registration*/
    {
         /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];
        eligPrioFunc = (GT_U32)inArgs[1];
        level = (CPSS_TM_LEVEL_ENT)inArgs[2];
       

        /* call cpss api function */
        result = cpssTmEligPrioFuncNodeConfig(devNum,eligPrioFunc, level, &funcOutArr);
        res= result;

        galtisOutput(outArgs, GT_OK, "");
        return CMD_OK;
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


static CMD_STATUS wrCpssTmEligPrioFuncNodeConfigFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    index=0;
    return wrCpssTmEligPrioFuncNodeConfig(inArgs,inFields,numFields,outArgs);
}


static CMD_STATUS wrCpssTmEligPrioFuncNodeConfigEnd
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     /* pack output arguments to galtis string */
    if (res!= GT_OK)
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssTmEligPrioFuncConfigAllLevels
*
* DESCRIPTION:
*       Configure the Eligible Priority Function according 
*       to the User Application parameters
*
* APPLICABLE DEVICES:
*       Bobcat2.
*
* NOT APPLICABLE DEVICES:
*       xCat; Lion; xCat2; DxCh1; DxCh1_Diamond; DxCh2; DxCh3; Lion2; Lion3; Puma2; Puma3; ExMx.
* 
* INPUTS:
*       devNum            - Device number.
*       eligPrioFuncPtr   - The new configured eligible function pointer.
*       funcOutArr        - The Eligible Priority Function structure array pointer.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on hardware error.
*       GT_BAD_PARAM             - on wrong device number or wrong parameter value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device.
*
* COMMENTS:
*       	The following API configures the same Eligible Priority Functions 
*	        at all nodes (A, B, C, and Port) levels Elig. Prio. Tables 
*	        according to the user’s parameters.
*	        It has the same functionality as tm_elig_prio_func_config() 
*	        and can be used at the user convenience to configure the 
*	        same eligible function to all the Nodes levels (except for Q level)
*	
*	Note: 	The use of Eligible Function Pointer 63 on both Queue and Node level is forbidden. 
*			Eligible Function 63 is reserved to indicate that Queue/Node is not eligible.
*******************************************************************************/
static CMD_STATUS wrCpssTmEligPrioFuncConfigAllLevels
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8       devNum;
    GT_U32      eligPrioFunc;
    
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (index >= 8)
    {
        galtisOutput(outArgs, GT_FULL, "%d",-1);/* Error ! */
        return CMD_OK;
    }

    /* map input arguments to locals */
    funcOutArr.nodeEligPrioFunc[index][0].maxTb = (GT_U32)inFields[0];
    funcOutArr.nodeEligPrioFunc[index][0].minTb = (GT_U32)inFields[1];
    funcOutArr.nodeEligPrioFunc[index][0].propPrio = (GT_U32)inFields[2];
    funcOutArr.nodeEligPrioFunc[index][0].schedPrio = (GT_U32)inFields[3];
    funcOutArr.nodeEligPrioFunc[index][0].elig = (GT_U32)inFields[4];
    funcOutArr.nodeEligPrioFunc[index][1].maxTb = (GT_U32)inFields[5];
    funcOutArr.nodeEligPrioFunc[index][1].minTb = (GT_U32)inFields[6];
    funcOutArr.nodeEligPrioFunc[index][1].propPrio = (GT_U32)inFields[7];
    funcOutArr.nodeEligPrioFunc[index][1].schedPrio = (GT_U32)inFields[8];
    funcOutArr.nodeEligPrioFunc[index][1].elig = (GT_U32)inFields[9];
    funcOutArr.nodeEligPrioFunc[index][2].maxTb = (GT_U32)inFields[10];
    funcOutArr.nodeEligPrioFunc[index][2].minTb = (GT_U32)inFields[11];
    funcOutArr.nodeEligPrioFunc[index][2].propPrio = (GT_U32)inFields[12];
    funcOutArr.nodeEligPrioFunc[index][2].schedPrio = (GT_U32)inFields[13];
    funcOutArr.nodeEligPrioFunc[index][2].elig = (GT_U32)inFields[14];
    funcOutArr.nodeEligPrioFunc[index][3].maxTb = (GT_U32)inFields[15];
    funcOutArr.nodeEligPrioFunc[index][3].minTb = (GT_U32)inFields[16];
    funcOutArr.nodeEligPrioFunc[index][3].propPrio = (GT_U32)inFields[17];
    funcOutArr.nodeEligPrioFunc[index][3].schedPrio = (GT_U32)inFields[18];
    funcOutArr.nodeEligPrioFunc[index][3].elig = (GT_U32)inFields[19];

    index++;

    if(index == 8)/* end registration*/
    {
         /* map input arguments to locals */
        devNum = (GT_U8)inArgs[0];
        eligPrioFunc = (GT_U32)inArgs[1];

       
        /* call cpss api function */
        result = cpssTmEligPrioFuncConfigAllLevels(devNum,eligPrioFunc,&funcOutArr);
        res= result;

        galtisOutput(outArgs, GT_OK, "");
        return CMD_OK;
    }


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


static CMD_STATUS wrCpssTmEligPrioFuncConfigAllLevelsFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    index=0;
    return wrCpssTmEligPrioFuncConfigAllLevels(inArgs,inFields,numFields,outArgs);
}


static CMD_STATUS wrCpssTmEligPrioFuncConfigAllLevelsEnd
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     /* pack output arguments to galtis string */
    if (res!= GT_OK)
        galtisOutput(outArgs, GT_BAD_STATE, "");
    else
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
   
        {"cpssTmEligPrioFuncQueueConfigSetFirst",
        &wrCpssTmEligPrioFuncQueueConfigFirst,
        2, 5},
        {"cpssTmEligPrioFuncQueueConfigSetNext",
        &wrCpssTmEligPrioFuncQueueConfig,
        2, 5},
        {"cpssTmEligPrioFuncQueueConfigEndSet",
        &wrCpssTmEligPrioFuncQueueConfigEnd,
        0, 0},
        {"cpssTmEligPrioFuncNodeConfigSetFirst",
        &wrCpssTmEligPrioFuncNodeConfigFirst,
        3, 20},
        {"cpssTmEligPrioFuncNodeConfigSetNext",
        &wrCpssTmEligPrioFuncNodeConfig,
        3, 20},
        {"cpssTmEligPrioFuncNodeConfigEndSet",
        &wrCpssTmEligPrioFuncNodeConfigEnd,
        0, 0},
        {"cpssTmEligPrioFuncConfigAllLevelsSetFirst",
        &wrCpssTmEligPrioFuncConfigAllLevelsFirst,
        2, 20},
        {"cpssTmEligPrioFuncConfigAllLevelsSetNext",
        &wrCpssTmEligPrioFuncConfigAllLevels,
        2, 20},
        {"cpssTmEligPrioFuncConfigAllLevelsEndSet",
        &wrCpssTmEligPrioFuncConfigAllLevelsEnd,
        0, 0} 
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssTmEligPrioFunc
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
GT_STATUS cmdLibInitCpssTmEligPrioFunc
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}
