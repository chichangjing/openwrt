/******************************************************************************
*              Copyright (c) Marvell International Ltd. and its affiliates
*                                                                         
* This software file (the "File") is owned and distributed by Marvell  
* International Ltd. and/or its affiliates ("Marvell") under the following
* alternative licensing terms.                                         
* If you received this File from Marvell, you may opt to use, redistribute 
* and/or modify this File under the following licensing terms.
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are met:
*  -   Redistributions of source code must retain the above copyright notice,
*       this list of conditions and the following disclaimer.             
*  -   Redistributions in binary form must reproduce the above copyright  
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*  -    Neither the name of Marvell nor the names of its contributors may be
*       used to endorse or promote products derived from this software without
*       specific prior written permission.                                
*                                                                        
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR    
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF    
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;            
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                             
*******************************************************************************
* wrHWDebug.cpp
*                                                                               
* DESCRIPTION:
*       Implementation of the Galtis Wrapper functions
*                                                               
* DEPENDENCIES:
*       None.            
*                                  
* FILE REVISION NUMBER:
*       $Revision: 2.*                                          
*******************************************************************************/

/* Common for all HWS Galtis wrappers */
#include <galtisAgent/wrapCpss/systemLab/wrHwsCommon.h>

/* General H Files */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <mvDdr3TrainingIpFlow.h>
#include <mvDdr3TrainingIpDb.h>

GT_STATUS cmdLibInitHWDebug
(
    GT_VOID
);

typedef enum
{
	MISC_FLAG 		= 0x0,
	SERDES_FLAG 	= 0x1000,
	DDR3_FLAG   	= 0x2000,
	BAP_FLAG    	= 0x4000,
	GENERAL_FLAG    = 0x8000
} HWS_FLAG_TYPE;

#define FLAG_MASK			0xFFF
#define FLAG_TYPE_MASK		0xF000


/************************** global data ***************************************/
/* ddr tunning mask*/
extern GT_U32 maskTuneFunc;

/* BAP */
extern GT_U32 bapInitBitMask;
extern GT_U32 bapDelay;
extern GT_U32 debugBapAccess;

/* BAP */
extern GT_U32 bapInitBitMask;

/* Serdes */
/*
extern GT_U32 portInitMask;
extern GT_U32 serdesMask;
extern GT_BOOL laneRevert;
*/
/******************************************************************************/

extern int interfaceTestRun(int deviceId, int testId);

GT_STATUS mvHwsSetSerdesFlags(GT_U32 flagId, GT_U32 val)
{
	switch(flagId)
	{
		/*case 0:*/
		default:
			/*cpssOsPrintf("Error: (Serdes) Un-known flag ID 0x%x\n", flagId);*/
			cpssOsPrintf("Error: (Serdes) falgID 0x%x is not supported\n", flagId);
			return GT_NOT_SUPPORTED;
	}

	/*return GT_OK;*/
}

GT_STATUS mvHwsSetDdr3Flags(GT_U32 flagId, GT_U32 val)
{
	GT_U8 devNum = 0;
	cpssOsPrintf("call ddr3TipSetAtr with flag %d val %d\n", flagId, val);
	/*return ddr3TipInitDebug(flagId, val);*/
	return ddr3TipSetAtr(devNum, flagId, val);
}

GT_STATUS mvHwsSetBapFlags(GT_U32 flagId, GT_U32 val)
{
	switch(flagId)
	{
		case 0:
			cpssOsPrintf("Set bapInitBitMask to 0x%x (previous was 0x%x)\n", val, bapInitBitMask);
			bapInitBitMask = val;
			break;
		case 1:
			cpssOsPrintf("Set bapDelay to 0x%x (previous was 0x%x)\n", val, bapDelay);
			bapDelay = val;
			break;
		case 2:
			cpssOsPrintf("Set debugBapAccess to 0x%x (previous was 0x%x)\n", val, debugBapAccess);
			debugBapAccess = val;
			break;
		default:
			cpssOsPrintf("Error: (BAP) Un-known flag ID 0x%x\n", flagId);
			return GT_NOT_SUPPORTED;
	}
	
	return GT_OK;
}


GT_STATUS mvHwsSetGeneralFlags(GT_U32 flagId, GT_U32 val)
{
	switch(flagId)
	{
		case 0:
			cpssOsPrintf("Error: (General) falgID 0x%x is not supported\n", flagId);
			break;
		default:
			cpssOsPrintf("Error: (General) Un-known flag ID 0x%x\n", flagId);
			return GT_NOT_SUPPORTED;
	}
	
	return GT_OK;
}

/*******************************************************************************
* wrHwsDebugFlag
*
* DESCRIPTION:
*
* INPUTS:
*  
* OUTPUTS:
*       None
*
* RETURNS:
*       CMD_OK              - on success.
*       CMD_AGENT_ERROR     - on failure.
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*       CMD_SYNTAX_ERROR    - on fail
* 
* COMMENTS:
*       None
*
*******************************************************************************/
CMD_STATUS wrHwsDebugFlag
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res = GT_OK;

    /* Parameters List */
    /*GT_U32  DevNum = (GT_U32)inArgs[0];*/
    GT_U32  FlagID = (GT_U32)inArgs[1];
    GT_U32  Value  = (GT_U32)inArgs[2];

	if((FlagID & FLAG_TYPE_MASK) == MISC_FLAG)
	{
#ifdef MV_HWS_INCLUDE_INTERFACE_TEST
		interfaceTestRun(0, 0);
#endif
	}
	else if((FlagID & FLAG_TYPE_MASK) == SERDES_FLAG)
	{
		res = mvHwsSetSerdesFlags(FlagID & FLAG_MASK, Value);
	}
	else if((FlagID & FLAG_TYPE_MASK) == DDR3_FLAG)
	{
		res = mvHwsSetDdr3Flags(FlagID & FLAG_MASK, Value);
	}
	else if((FlagID & FLAG_TYPE_MASK) == BAP_FLAG)
	{
		res = mvHwsSetBapFlags(FlagID & FLAG_MASK, Value);
	}
	else if((FlagID & FLAG_TYPE_MASK) == GENERAL_FLAG)
	{
		res = mvHwsSetGeneralFlags(FlagID & FLAG_MASK, Value);
	}

    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
*
* dbCommands - Galtis database galtis commands
*
*******************************************************************************/
static CMD_COMMAND dbCommands[] = {
{"HwsDebugFlag", &wrHwsDebugFlag, 3, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitHwsDebug
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
GT_STATUS cmdLibInitHwsDebug()
{
    return cmdInitLibrary(dbCommands, numCommands);
}

