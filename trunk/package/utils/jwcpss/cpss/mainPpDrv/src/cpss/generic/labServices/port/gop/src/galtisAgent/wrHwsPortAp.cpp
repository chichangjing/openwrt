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
* wrPortAp.c
*                                                                               
* DESCRIPTION:
*       Implementation of the Galtis Wrapper functions
*                                                               
* DEPENDENCIES:
*       None.            
*                                  
* FILE REVISION NUMBER:
*       $Revision: 1.0 $          
*                                          
*******************************************************************************/

/* General H Files */
#include <common/os/gtOs.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <common/macros/gtCommonFuncs.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <galtisAgent/wrapUtil/galtisValues.h>
#include <galtisAgent/wrapUtil/cmdUtils.h>

#include <port/mvHwsPortApInitIf.h>

#ifndef WIN32
#include <api/interrupt/gtInterrupts.h>
#include <api/interrupt/gtIntEventsCauseTypes.h>
#endif


extern "C" {
GT_STATUS cmdLibInitPortAp
(
	GT_VOID
);

/* this global parameter disables 40G/10G WA when enabled */
extern GT_U32 apEnableMask;

/* in Lion2/Hooper, PCS un-reset (after training) is executed in mvHwsPortFixAlign90Stop.
   this variable allows PCS un-reset during training stop if set to FALSE */
extern GT_BOOL skipPcsUnreset;
}

#ifndef WIN32
GT_U32 lion2MmPcsIntVec[] = {
	MV_LN2_MMPCS0IntSum_Reserved_B12,
	MV_LN2_MMPCS1IntSum_Reserved_B12,
	MV_LN2_MMPCS2IntSum_Reserved_B12,
	MV_LN2_MMPCS3IntSum_Reserved_B12,
	MV_LN2_MMPCS4IntSum_Reserved_B12,
	MV_LN2_MMPCS5IntSum_Reserved_B12,
	MV_LN2_MMPCS6IntSum_Reserved_B12,
	MV_LN2_MMPCS7IntSum_Reserved_B12,
	MV_LN2_MMPCS8IntSum_Reserved_B12,
	MV_LN2_MMPCS9IntSum_Reserved_B12,
	MV_LN2_MMPCS10IntSum_Reserved_B12,
	MV_LN2_MMPCS11IntSum_Reserved_B12,
	MV_LN2_MMPCS12IntSum_Reserved_B12
};
#endif

GT_STATUS getPortStatus(GT_U8 DevNum, GT_U8 PortNum, GT_BOOL* portEnabled, MV_HWS_AP_PORT_STATUS* apResult);

GT_U32 currentPort;

/*******************************************************************************
* wrHwsApEngineEnable
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
CMD_STATUS wrHwsApEngineEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum         = (GT_U32)inArgs[0];
    GT_U32  EnableApEngine = (GT_U32)inArgs[1];

	if (EnableApEngine)
	{
		res = mvHwsApEngineInit(DevNum, DevNum);

		/* this global parameter disables 40G/10G WA */
		osPrintf("Disable 40G/10G WA\n");
		apEnableMask |= (1 << DevNum);

		osPrintf("Enable PCS Reset during training stop\n");
		skipPcsUnreset = GT_FALSE;
	}
	else
	{
		res = mvHwsApEngineStop(DevNum, DevNum);

		apEnableMask &= ~(1 << DevNum);

		if(apEnableMask == 0)
		{
			osPrintf("No core is is enabled - Interrupts are Enbled\n");
		}
		else
		{
			osPrintf("At lease one more core is enabled - Interrupts are still disabled\n");
		}
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
* wrHwsApPortInitSet
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
CMD_STATUS wrHwsApPortInitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	MV_HWS_AP_CFG   apCfg;
    GT_STATUS		res;

    /* Parameters List */
    GT_U32  DevNum = (GT_U32)inArgs[0];

    /* Fields List */
    GT_U32  PortNum          = (GT_U32)inFields[0];
    GT_U32  SerdesNum        = (GT_U32)inFields[1];
    GT_U32  ModesVector      = (GT_U32)inFields[2];
    GT_BOOL  FecSupport       = (GT_BOOL)inFields[3];
    GT_BOOL  FecRequest       = (GT_BOOL)inFields[4];
    GT_BOOL  FcPause          = (GT_BOOL)inFields[5];
    GT_BOOL  FcAsmDir         = (GT_BOOL)inFields[6];
    GT_BOOL  NonceDisable     = (GT_BOOL)inFields[7];
    GT_BOOL  RxPolarityInvert = (GT_BOOL)inFields[8];
    GT_BOOL  TxPolarityInvert = (GT_BOOL)inFields[9];
    GT_U32  RefClock         = (GT_U32)inFields[10];
    GT_U32  RefClockSource   = (GT_U32)inFields[11];


	osPrintf("Parameters:\n");
	osPrintf("-----------\n\n");
	osPrintf("\tDev Num:       %d\n", DevNum);
	osPrintf("\tPort Num:      %d\n", PortNum);
	osPrintf("\tLane Num:      %d\n", SerdesNum);
	osPrintf("\tMode Mask:     0x%x\n", ModesVector);
	osPrintf("\tLoopback:      %s\n", (NonceDisable) ? "Enabled" : "Disabled");
	osPrintf("\tFEC:           %s\n", (FecSupport) ? "Enabled" : "Disabled");
	osPrintf("\tFC:            %s\n", (FcPause) ? "Enabled" : "Disabled");
	osPrintf("\tFC Asymmetric: %s\n", (FcAsmDir) ? "Enabled" : "Disabled");
	osPrintf("\tRx Polarity:   %s\n", (RxPolarityInvert) ? "Enabled" : "Disabled");
	osPrintf("\tTx Polarity:   %s\n", (TxPolarityInvert) ? "Enabled" : "Disabled");
	osPrintf("\tRef Clock:     %d\n", RefClock);
	osPrintf("\tRef Clock Src: %d\n", RefClockSource);


	apCfg.apLaneNum = SerdesNum;
	apCfg.modesVector = ModesVector;
	
	apCfg.fecSup = FecSupport;
	apCfg.fecReq = FecRequest;
	
	apCfg.fcPause = FcPause;
	apCfg.fcAsmDir = FcAsmDir;
	
	apCfg.nonceDis = NonceDisable;
	
	apCfg.refClockCfg.refClockFreq = (MV_HWS_REF_CLOCK_SUP_VAL)RefClock;
	apCfg.refClockCfg.refClockSource = (MV_HWS_REF_CLOCK_SOURCE)RefClockSource;

#ifndef WIN32
	if ((HWS_DEV_SILICON_TYPE(DevNum) == Lion2B0) && 
		(PortNum >= sizeof(lion2MmPcsIntVec)/sizeof(GT_U32)))
	{
		osPrintf("Error: PortNum (%d)exceeds max port num\n", PortNum);
		galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
	}

	osPrintf("Disable MMPCS WA\n");
	gtIntMask(DevNum, lion2MmPcsIntVec[PortNum], GT_FALSE);
#endif

    res = mvHwsApPortStart(DevNum, DevNum, PortNum, &apCfg);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsApPortInitDelete
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
CMD_STATUS wrHwsApPortInitDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum = (GT_U32)inArgs[0];

    /* Fields List */
	GT_U32  PortNum = (GT_U32)inFields[0];

    res = mvHwsApPortStop(DevNum, DevNum, PortNum, PORT_POWER_DOWN);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

#ifndef WIN32
	if ((HWS_DEV_SILICON_TYPE(DevNum) == Lion2B0) && 
		(PortNum >= sizeof(lion2MmPcsIntVec)/sizeof(GT_U32)))
	{
		osPrintf("Error: PortNum (%d)exceeds max port num\n", PortNum);
		galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
	}
	
	osPrintf("Enable MMPCS WA\n");
	gtIntMask(DevNum, lion2MmPcsIntVec[PortNum], GT_TRUE);
#endif

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsApPortStatusGetFirst
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
CMD_STATUS wrHwsApPortStatusGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	MV_HWS_AP_PORT_STATUS   apResult;
	GT_BOOL					portEnabled;
    GT_STATUS				res;
	
    /* Parameters List */
    GT_U32  DevNum    = (GT_U32)inArgs[0];
    GT_U32  PortStart = (GT_U32)inArgs[1];
    GT_U32  PortEnd   = (GT_U32)inArgs[2];

	if (PortStart > PortEnd)
	{
		osPrintf("PortStart (%d) cannot be lower than PortEnd (%d)\n",PortStart ,PortEnd );
		galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
	}

	currentPort = PortStart;
	
    res = getPortStatus(DevNum, currentPort, &portEnabled, &apResult);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    /* Fields List */
    inFields[0] = (GT_32)currentPort;						/* PortNum		 */
    inFields[1] = (GT_32)portEnabled;						/* Enabled		 */
	inFields[2] = (GT_32)apResult.hcdResult.hcdFound;		/* HcdFound		 */
    inFields[3] = (GT_32)apResult.postApPortMode;			/* PortMode		 */
    inFields[4] = (GT_32)apResult.hcdResult.hcdLinkStatus;	/* LinkStatus	 */
    inFields[5] = (GT_32)apResult.hcdResult.hcdFecEn;		/* FecEnable	 */
    inFields[6] = (GT_32)apResult.hcdResult.hcdFcTxPauseEn; /* FcTxPause	 */
    inFields[7] = (GT_32)apResult.hcdResult.hcdFcRxPauseEn; /* FcRxPause	 */

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsApPortStatusGetNext
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
CMD_STATUS wrHwsApPortStatusGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	MV_HWS_AP_PORT_STATUS   apResult;
	GT_BOOL					portEnabled;
    GT_STATUS				res;
	
    /* Parameters List */
    GT_U32  DevNum    = (GT_U32)inArgs[0];
    GT_U32  PortEnd   = (GT_U32)inArgs[2];
	
	currentPort++;

	if (currentPort > PortEnd)
	{
		galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
	}
	
    res = getPortStatus(DevNum, currentPort, &portEnabled, &apResult);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }
	
    /* Fields List */
    inFields[0] = (GT_32)currentPort;						/* PortNum		*/
    inFields[1] = (GT_32)portEnabled;						/* Enabled		*/
	inFields[2] = (GT_32)apResult.hcdResult.hcdFound;		/* HcdFound		*/
    inFields[3] = (GT_32)apResult.postApPortMode;			/* PortMode		*/
    inFields[4] = (GT_32)apResult.hcdResult.hcdLinkStatus;	/* LinkStatus	*/
    inFields[5] = (GT_32)apResult.hcdResult.hcdFecEn;		/* FecEnable	*/
    inFields[6] = (GT_32)apResult.hcdResult.hcdFcTxPauseEn; /* FcTxPause	*/
    inFields[7] = (GT_32)apResult.hcdResult.hcdFcRxPauseEn; /* FcRxPause	*/
	
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4], inFields[5], inFields[6], inFields[7]);
	
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}


GT_STATUS getPortStatus(GT_U8 DevNum, GT_U8 PortNum, GT_BOOL* portEnabled, MV_HWS_AP_PORT_STATUS* apResult)
{
	GT_BOOL engineEnabled;

	mvHwsApEngineInitGet(DevNum, DevNum, &engineEnabled);
	if(engineEnabled == GT_FALSE)
	{
		osPrintf("AP Engine is not initialized for device %d\n", DevNum);
		return GT_NOT_INITIALIZED;
	}
	
	CHECK_STATUS(mvHwsApPortConfigGet(DevNum, DevNum, PortNum, portEnabled, 0));
	
    CHECK_STATUS(mvHwsApPortStatusGet(DevNum, DevNum, PortNum, apResult));

	return GT_OK;
}

/*******************************************************************************
*
* dbCommands - Galtis database galtis commands
*
*******************************************************************************/
static CMD_COMMAND dbCommands[] = {
{"HwsApEngineEnable", &wrHwsApEngineEnable, 2, 0},
{"HwsApPortInitSet", &wrHwsApPortInitSet, 1, 12},
{"HwsApPortInitDelete", &wrHwsApPortInitDelete, 1, 12},
{"HwsApPortStatusGetFirst", &wrHwsApPortStatusGetFirst, 3, 0},
{"HwsApPortStatusGetNext", &wrHwsApPortStatusGetNext, 3, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))


/*******************************************************************************
* cmdLibInitPortAp
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
GT_STATUS cmdLibInitPortAp()
{
    return cmdInitLibrary(dbCommands, numCommands);
}
