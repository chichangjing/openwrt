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
* wrDDRTrainingIP.cpp
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
#include <common/system/system.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdCommon.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <common/macros/gtCommonFuncs.h>
#include <galtisAgent/wrapUtil/galtisValues.h>
#include <galtisAgent/wrapUtil/cmdUtils.h>

#include <TrainingIp/mvDdr3TrainingIpPrvIf.h>
#include <TrainingIp/mvDdr3TrainingIpBist.h>
#include <TrainingIp/mvDdr3TrainingIpEngine.h>

extern "C" {
GT_STATUS cmdLibInitDDR3TrainingIP
(
    GT_VOID
);
}

static GT_U32 currentLine;         /* table line */
static GT_U32 currentInterface;     /* for BistReadResult */
              
/*******************************************************************************
* wrHwsDdr3TrainingIpPbsTx
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
CMD_STATUS wrHwsDdr3TrainingIpPbsTx
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


    res = mvHwsTrainingIpPbsTx(DevNum);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpSelectDdrController
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
CMD_STATUS wrHwsDdr3TrainingIpSelectDdrController
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
    GT_U32  Enable = (GT_U32)inArgs[1];


    res = mvHwsTrainingIpSelectController(DevNum, (GT_BOOL)Enable);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpPbsRx
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
CMD_STATUS wrHwsDdr3TrainingIpPbsRx
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


    res = mvHwsTrainingIpPbsRx(DevNum);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpInitController
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
CMD_STATUS wrHwsDdr3TrainingIpInitController
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;
    InitCntrParam   initCntrPrm;
    /* Parameters List */
    GT_U32  DevNum = (GT_U32)inArgs[0];

	initCntrPrm.doMrsPhy = GT_TRUE;
 	initCntrPrm.do64Bit = GT_FALSE;
    res = mvHwsTrainingIpInitController(DevNum, &initCntrPrm);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpFreqSet
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
CMD_STATUS wrHwsDdr3TrainingIpFreqSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum      = (GT_U32)inArgs[0];
    GT_U32  AccessType  = (GT_U32)inArgs[1];
	GT_U32  InterfaceId = (GT_U32)inArgs[2];
	GT_U32  MemoryFreq  = (GT_U32)inArgs[3];

    res = mvHwsTrainingIpSetFreq(DevNum, (MV_HWS_ACCESS_TYPE)AccessType, InterfaceId, 
								 (MV_HWS_DDR_FREQ)MemoryFreq);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpStaticConfig
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
CMD_STATUS wrHwsDdr3TrainingIpStaticConfig
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum      = (GT_U32)inArgs[0];
    GT_U32  MemoryFreq  = (GT_U32)inArgs[1];
    GT_U32  Type        = (GT_U32)inArgs[2];
    GT_U32  InterfaceID = (GT_U32)inArgs[3];


    res = mvHwsTrainingIpStaticConfig(DevNum, (MV_HWS_DDR_FREQ)MemoryFreq, (MV_HWS_STATIC_COFIG_TYPE)Type, InterfaceID);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpAlgoRun
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
CMD_STATUS wrHwsDdr3TrainingIpAlgoRun
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum   = (GT_U32)inArgs[0];
    MV_HWS_ALGO_TYPE  AlgoType = (MV_HWS_ALGO_TYPE)inArgs[1];


    res = mvHwsTrainingIpAlgoRun(DevNum, AlgoType);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpPhyRegAccessSet
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
CMD_STATUS wrHwsDdr3TrainingIpPhyRegAccessSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum              = (GT_U32)inArgs[0];
    GT_U32  InterfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  InterfaceID         = (GT_U32)inArgs[2];
    GT_U32  PhyAccessType       = (GT_U32)inArgs[3];
    GT_U32  PhyNum              = (GT_U32)inArgs[4];
	GT_U32  PhyType             = (GT_U32)inArgs[5];
    GT_U32  RegAddress          = (GT_U32)inArgs[6];

	GT_U32  Data;


    /* Fields List */

	if(PhyAccessType == 0)
	{
		Data = (GT_U32)inFields[PhyNum + 1];
	}
	else
	{
		Data = (GT_U32)inFields[1];
	}


	res = mvHwsTrainingIpBusWrite(DevNum, (MV_HWS_ACCESS_TYPE)InterfaceAccessType, InterfaceID, 
								  (MV_HWS_ACCESS_TYPE)PhyAccessType, PhyNum, (MV_HWS_DDR_PHY)PhyType, RegAddress, Data);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpPhyRegAccessGetFirst
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
CMD_STATUS wrHwsDdr3TrainingIpPhyRegAccessGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_U32 data[4] = {0};

    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum              = (GT_U32)inArgs[0];
    GT_U32  InterfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  InterfaceID         = (GT_U32)inArgs[2];
    GT_U32  PhyAccessType       = (GT_U32)inArgs[3];
    GT_U32  PhyNum              = (GT_U32)inArgs[4];
	GT_U32  PhyType             = (GT_U32)inArgs[5];
    GT_U32  RegAddress          = (GT_U32)inArgs[6];
	
	if (PhyAccessType == 0)
	{
		res = mvHwsTrainingIpBusRead(DevNum, InterfaceID, (MV_HWS_ACCESS_TYPE)PhyAccessType, PhyNum, 
		                         (MV_HWS_DDR_PHY)PhyType, RegAddress, &data[PhyNum]);
	}
	else
	{
		res = mvHwsTrainingIpBusRead(DevNum, InterfaceID, (MV_HWS_ACCESS_TYPE)PhyAccessType, PhyNum, 
		                         (MV_HWS_DDR_PHY)PhyType, RegAddress, data);
	}
	
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    /* Fields List */
    inFields[0] = InterfaceID; // Data

    /* pack and output table fields */
	fieldOutput("%d%d%d%d%d", inFields[0], data[0],data[1],data[2],data[3] );
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpPhyRegAccessGetNext
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
CMD_STATUS wrHwsDdr3TrainingIpPhyRegAccessGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpDunitRegAccessSet
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
CMD_STATUS wrHwsDdr3TrainingIpDunitRegAccessSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum              = (GT_U32)inArgs[0];
    GT_U32  InterfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  InterfaceID         = (GT_U32)inArgs[2];
    GT_U32  RegAddress          = (GT_U32)inArgs[3];
	GT_U32  RegMask             = (GT_U32)inArgs[4];
	GT_U32  Data;

	Data = (GT_U32)inFields[0];

	if (RegMask == 0)
	{
		RegMask = 0xffffffff;
	}
	res = mvHwsTrainingIpIfWrite(DevNum, (MV_HWS_ACCESS_TYPE)InterfaceAccessType, InterfaceID, RegAddress, Data, RegMask);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpDunitRegAccessGetFirst
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
CMD_STATUS wrHwsDdr3TrainingIpDunitRegAccessGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_U32 data;
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum              = (GT_U32)inArgs[0];
    GT_U32  InterfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  InterfaceID         = (GT_U32)inArgs[2];
    GT_U32  RegAddress          = (GT_U32)inArgs[3];
	GT_U32  RegMask             = (GT_U32)inArgs[4];

	if (RegMask == 0)
	{
		RegMask = 0xffffffff;
	}

    res = mvHwsTrainingIpIfRead(DevNum, (MV_HWS_ACCESS_TYPE)InterfaceAccessType, InterfaceID, RegAddress, &data, RegMask);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    /* Fields List */
    inFields[0] = data; // Data
    /* pack and output table fields */
    fieldOutput("%d", inFields[0]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpDunitRegAccessGetNext
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
CMD_STATUS wrHwsDdr3TrainingIpDunitRegAccessGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpSetTopologySet
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
CMD_STATUS wrHwsDdr3TrainingIpSetTopologySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#pragma message ("*** TODO: wrHwsDdr3TrainingIpSetTopologySet is not implemented ***")
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum = (GT_U32)inArgs[0];

    /* Fields List */
    GT_U32  InterfaceID = (GT_U32)inFields[0];
    GT_U32  Active      = (GT_U32)inFields[1];
    GT_U32  SpeedBin    = (GT_U32)inFields[2];
    GT_U32  BusWidth    = (GT_U32)inFields[3];
    GT_U32  MemSize     = (GT_U32)inFields[4];
    GT_U32  TargetFreq  = (GT_U32)inFields[5];
    GT_U32  CasWL       = (GT_U32)inFields[6];
    GT_U32  CasL        = (GT_U32)inFields[7];
    GT_U32  Temperature = (GT_U32)inFields[8];

    /*res = <FUNC_CALL>;*/
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpSetTopologyGetFirst
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
CMD_STATUS wrHwsDdr3TrainingIpSetTopologyGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#pragma message ("*** TODO: wrHwsDdr3TrainingIpSetTopologyGetFirst is not implemented ***")
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum = (GT_U32)inArgs[0];

    /*res = <FUNC_CALL>;*/
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    /* Fields List */
    /* Note: You should un-comment this part and fill fields' value */
    /*
    inFields[0] = (GT_32)<<Put Value Here>>; // InterfaceID
    inFields[1] = (GT_32)<<Put Value Here>>; // Active
    inFields[2] = (GT_32)<<Put Value Here>>; // SpeedBin
    inFields[3] = (GT_32)<<Put Value Here>>; // BusWidth
    inFields[4] = (GT_32)<<Put Value Here>>; // MemSize
    inFields[5] = (GT_32)<<Put Value Here>>; // TargetFreq
    inFields[6] = (GT_32)<<Put Value Here>>; // CasWL
    inFields[7] = (GT_32)<<Put Value Here>>; // CasL
    inFields[8] = (GT_32)<<Put Value Here>>; // Temperature
    */

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4], inFields[5], inFields[6], inFields[7], inFields[8]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpSetTopologyGetNext
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
CMD_STATUS wrHwsDdr3TrainingIpSetTopologyGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
#pragma message ("*** TODO: wrHwsDdr3TrainingIpSetTopologyGetNext is not implemented ***")
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum = (GT_U32)inArgs[0];

    /*res = <FUNC_CALL>;*/
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    /* Fields List */
    /* Note: You should un-comment this part and fill fields' value */
    /*
    inFields[0] = (GT_32)<<Put Value Here>>; // InterfaceID
    inFields[1] = (GT_32)<<Put Value Here>>; // Active
    inFields[2] = (GT_32)<<Put Value Here>>; // SpeedBin
    inFields[3] = (GT_32)<<Put Value Here>>; // BusWidth
    inFields[4] = (GT_32)<<Put Value Here>>; // MemSize
    inFields[5] = (GT_32)<<Put Value Here>>; // TargetFreq
    inFields[6] = (GT_32)<<Put Value Here>>; // CasWL
    inFields[7] = (GT_32)<<Put Value Here>>; // CasL
    inFields[8] = (GT_32)<<Put Value Here>>; // Temperature
    */

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4], inFields[5], inFields[6], inFields[7], inFields[8]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpStaticConfigSet
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
CMD_STATUS wrHwsDdr3TrainingIpStaticConfigSet
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
    GT_U32  Enable = (GT_U32)inArgs[1];

    /* Fields List */
    GT_U32  Data = (GT_U32)inFields[0];

    res = mvHwsTrainingIpSelectController(DevNum, (GT_BOOL)Enable);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}



/*******************************************************************************
* wrHwsDdr3TrainingIpExternalMemAccessSet
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
CMD_STATUS wrHwsDdr3TrainingIpExternalMemAccessSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    GT_U32  memoryNumber = (GT_U32)inArgs[1];
    GT_U32  size         = (GT_U32)inArgs[3]; /* in lines */

	/* Fields List */
    GT_U32 baseAddress  = (GT_U32)inFields[0];
    GT_U32 wordArr[8];

    wordArr[0]  = (GT_U32)inFields[1];
    wordArr[1]  = (GT_U32)inFields[2];
    wordArr[2]  = (GT_U32)inFields[3];
    wordArr[3]  = (GT_U32)inFields[4];
    wordArr[4]  = (GT_U32)inFields[5];
    wordArr[5]  = (GT_U32)inFields[6];
    wordArr[6]  = (GT_U32)inFields[7];
    wordArr[7]  = (GT_U32)inFields[8];

	res = mvHwsTrainingIpExtWrite(devNum, memoryNumber, baseAddress, size, wordArr);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpExternalMemAccessGetFirst
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
CMD_STATUS wrHwsDdr3TrainingIpExternalMemAccessGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

	GT_STATUS  res;

    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    GT_U32  memoryNumber = (GT_U32)inArgs[1];
    GT_U32  baseAddress  = (GT_U32)inArgs[2];
    GT_U32  size         = (GT_U32)inArgs[3];
    /* Fields List */
    GT_U32 wordArr[8];

    osMemSet(wordArr, 0, sizeof(wordArr));
	currentLine = 1;
    inFields[0] = baseAddress;

    res = mvHwsTrainingIpExtRead(devNum, memoryNumber, baseAddress, 1, wordArr);
	if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0], wordArr[0], wordArr[1], wordArr[2], wordArr[3], 
												   wordArr[4],wordArr[5], wordArr[6], wordArr[7]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpExternalMemAccessGetNext
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
CMD_STATUS wrHwsDdr3TrainingIpExternalMemAccessGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32    addrInd=0x20;
	GT_STATUS  res;


    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    GT_U32  memoryNumber = (GT_U32)inArgs[1];
    GT_U32  baseAddress  = (GT_U32)inArgs[2];
    GT_U32  size         = (GT_U32)inArgs[3];
    /* Fields List */
    GT_U32 wordArr[8];

    if (currentLine >= size)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    osMemSet(wordArr, 0, sizeof(wordArr));

	inFields[0] = baseAddress + currentLine*addrInd;

	res = mvHwsTrainingIpExtRead(devNum, memoryNumber, inFields[0], 1, wordArr);
  	if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }
    
    currentLine++;
  
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0], wordArr[0], wordArr[1], wordArr[2], wordArr[3], 
		                              wordArr[4],wordArr[5], wordArr[6], wordArr[7]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}



/*******************************************************************************
* wrHwsDdr3TrainingIpTestSet
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
CMD_STATUS wrHwsDdr3TrainingIpDunitRegAccessExtSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum              = (GT_U32)inArgs[0];
    GT_U32  InterfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  InterfaceID         = (GT_U32)inArgs[2];
    GT_U32  RegAddress          = (GT_U32)inArgs[3];
	GT_U32  RegMask             = (GT_U32)inArgs[4];
	GT_U32  Data;

	if(InterfaceAccessType == 0)
	{
		Data = (GT_U32)inFields[InterfaceID + 1];
	}
	else
	{
		Data = (GT_U32)inFields[1];
	}

	if (RegMask == 0)
	{
		RegMask = 0xffffffff;
	}
	res = mvHwsTrainingIpIfWrite(DevNum, (MV_HWS_ACCESS_TYPE)InterfaceAccessType, InterfaceID, RegAddress, Data, RegMask);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpDunitRegAccessExtGetFirst
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
CMD_STATUS wrHwsDdr3TrainingIpDunitRegAccessExtGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	GT_U32 data[12] = {0};
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum              = (GT_U32)inArgs[0];
    GT_U32  InterfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  InterfaceID         = (GT_U32)inArgs[2];
    GT_U32  RegAddress          = (GT_U32)inArgs[3];
	GT_U32  RegMask             = (GT_U32)inArgs[4];

	if (RegMask == 0)
	{
		RegMask = 0xffffffff;
	}
	res = mvHwsTrainingIpIfRead(DevNum, (MV_HWS_ACCESS_TYPE)InterfaceAccessType, InterfaceID, RegAddress, data, RegMask);	
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    inFields[0] = InterfaceID;  
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10],data[11]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpTestGetNext
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
CMD_STATUS wrHwsDdr3TrainingIpDunitRegAccessExtGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	galtisOutput(outArgs, GT_OK, "%d", -1);
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpBistActivate
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
CMD_STATUS wrHwsDdr3TrainingIpBistActivate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  devNum                      = (GT_U32)inArgs[0];
    MV_HWS_ACCESS_TYPE  accessType      = (MV_HWS_ACCESS_TYPE)inArgs[1];
    GT_U32  interfaceId                 = (GT_U32)inArgs[2];
    MV_HWS_PATTERN  pattern             = (MV_HWS_PATTERN)inArgs[3];
    MV_HWS_DIRECTION  direction         = (MV_HWS_DIRECTION)inArgs[4];
    MV_HWS_STRESS_JUMP  stress          = (MV_HWS_STRESS_JUMP)inArgs[5];
    MV_HWS_PATTERN_DURATION  duration   = (MV_HWS_PATTERN_DURATION)inArgs[6];
    MV_HWS_BIST_OPERATION  operation    = (MV_HWS_BIST_OPERATION)inArgs[7];
    GT_U32  offset                      = (GT_U32)inArgs[8];
    GT_U32  patternLength               = (GT_U32)inArgs[9];

	res = mvHwsTrainingIpBistActivate(devNum, pattern, accessType, interfaceId, 
                                      direction, stress, duration, operation, 
                                      offset, /*0,*/ patternLength);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}



/*******************************************************************************
* wrHwsDdr3TrainingIpBistResultsGetFirst
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
CMD_STATUS wrHwsDdr3TrainingIpBistResultsGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;
    BistResult bistResult;

    /* Parameters List */
    GT_U32  devNum      = (GT_U32)inArgs[0];
    GT_U32  interfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  interfaceNum = (GT_U32)inArgs[2];

    if(interfaceAccessType == 0)    /* unicast */
    {
	    res = mvHwsTrainingIpBistReadResult(devNum, interfaceNum, &bistResult); 
        inFields[0] = interfaceNum;  
        currentInterface =  MAX_INTERFACE_NUM;     
    }
    else                            /* multicast */
    {
        currentInterface = 0;
	    res = mvHwsTrainingIpBistReadResult(devNum, currentInterface, &bistResult);
        inFields[0] = currentInterface; 
        currentInterface++;
    }

    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    inFields[1] = (GT_32)bistResult.bistErrorCnt;
    inFields[2] = (GT_32)bistResult.bistFailLow;
    inFields[3] = (GT_32)bistResult.bistFailHigh; 
    inFields[4] = (GT_32)bistResult.bistLastFailAddr;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], 
                inFields[2], inFields[3], inFields[4]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpBistResultsGetNext
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
CMD_STATUS wrHwsDdr3TrainingIpBistResultsGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;
    BistResult bistResult;

    /* Parameters List */
    GT_U32  devNum              = (GT_U32)inArgs[0];
    GT_U32  interfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  interfaceNum        = (GT_U32)inArgs[2];

    if (currentInterface >= MAX_INTERFACE_NUM)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

	inFields[0] = currentInterface;

	res = mvHwsTrainingIpBistReadResult(devNum, currentInterface, &bistResult);
	if (res == GT_NOT_SUPPORTED)
    {
		/* this means we got to the last interface. no need to go on */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
  	else if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }
    
    currentInterface++;

    inFields[1] = (GT_32)bistResult.bistErrorCnt;
    inFields[2] = (GT_32)bistResult.bistFailLow;
    inFields[3] = (GT_32)bistResult.bistFailHigh; 
    inFields[4] = (GT_32)bistResult.bistLastFailAddr;
  
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], 
                inFields[2], inFields[3], inFields[4]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TrainingIpActivate
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
CMD_STATUS wrHwsDdr3TrainingIpActivate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
	char* status;
	MV_HWS_TrainingIpStatus trainStatusArr[MAX_INTERFACE_NUM];
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  DevNum                          = (GT_U32)inArgs[0];
    MV_HWS_ACCESS_TYPE  AccessType          = (MV_HWS_ACCESS_TYPE)inArgs[1];
    GT_U32  InterfaceId                     = (GT_U32)inArgs[2];
    MV_HWS_ACCESS_TYPE  PupAccessType       = (MV_HWS_ACCESS_TYPE)inArgs[3];
    GT_U32  PupId                           = (GT_U32)inArgs[4];
    MV_HWS_TRAINING_RESULT  ResultType      = (MV_HWS_TRAINING_RESULT)inArgs[5];
    MV_HWS_ControlElement  ControlElement   = (MV_HWS_ControlElement)inArgs[6];
    MV_HWS_SearchDirection  SearchDirection = (MV_HWS_SearchDirection)inArgs[7];
    MV_HWS_DIRECTION  Direction             = (MV_HWS_DIRECTION)inArgs[8];
    GT_U32  InitValue                       = (GT_U32)inArgs[9];
    GT_U32  NumberOfIterations              = (GT_U32)inArgs[10];
    MV_HWS_PATTERN  Pattern                 = (MV_HWS_PATTERN)inArgs[11];
    MV_HWS_EdgeCompare  EdgeCompare         = (MV_HWS_EdgeCompare)inArgs[12];
    MV_HWS_DDR_CS  CsType                   = (MV_HWS_DDR_CS)inArgs[13];
    GT_U32  CsNum                           = (GT_U32)inArgs[14];

    res = ddr3TipIpTraining(DevNum, AccessType, InterfaceId, PupAccessType, PupId, ResultType, 
							ControlElement, SearchDirection, Direction, (1 << InterfaceId), InitValue,
							NumberOfIterations, Pattern, EdgeCompare, CsType, CsNum, trainStatusArr);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

	switch(trainStatusArr[InterfaceId])
	{
	case 0:
		status = "FAIL";
		break;
	case 1:
		status = "SUCCESS";
		break;
	case 2:
		status = "TIMEOUT";
		break;
	default:
		status = "Un-Known";
		break;
	}
	
	osPrintf("Activation status for ifc %d is %s\n", InterfaceId, status);

	if (trainStatusArr[InterfaceId] == MV_HWS_TrainingIpStatus_SUCCESS)
    {
        galtisOutput(outArgs, GT_BAD_STATE, "");
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
{"HwsDdr3TrainingIpPbsTx", &wrHwsDdr3TrainingIpPbsTx, 1, 0},
{"HwsDdr3TrainingIpSelectDdrController", &wrHwsDdr3TrainingIpSelectDdrController, 2, 0},
{"HwsDdr3TrainingIpPbsRx", &wrHwsDdr3TrainingIpPbsRx, 1, 0},
{"HwsDdr3TrainingIpInitController", &wrHwsDdr3TrainingIpInitController, 1, 0},
{"HwsDdr3TrainingIpFreqSet", &wrHwsDdr3TrainingIpFreqSet, 4, 0},
{"HwsDdr3TrainingIpStaticConfig", &wrHwsDdr3TrainingIpStaticConfig, 4, 0},
{"HwsDdr3TrainingIpAlgoRun", &wrHwsDdr3TrainingIpAlgoRun, 2, 0},
{"HwsDdr3TrainingIpPhyRegAccessSet", &wrHwsDdr3TrainingIpPhyRegAccessSet, 7, 5},
{"HwsDdr3TrainingIpPhyRegAccessGetFirst", &wrHwsDdr3TrainingIpPhyRegAccessGetFirst, 7, 0},
{"HwsDdr3TrainingIpPhyRegAccessGetNext", &wrHwsDdr3TrainingIpPhyRegAccessGetNext, 7, 0},
{"HwsDdr3TrainingIpDunitRegAccessSet", &wrHwsDdr3TrainingIpDunitRegAccessSet, 5, 1},
{"HwsDdr3TrainingIpDunitRegAccessGetFirst", &wrHwsDdr3TrainingIpDunitRegAccessGetFirst, 5, 0},
{"HwsDdr3TrainingIpDunitRegAccessGetNext", &wrHwsDdr3TrainingIpDunitRegAccessGetNext, 5, 0},
{"HwsDdr3TrainingIpSetTopologySet", &wrHwsDdr3TrainingIpSetTopologySet, 1, 9},
{"HwsDdr3TrainingIpSetTopologyGetFirst", &wrHwsDdr3TrainingIpSetTopologyGetFirst, 1, 0},
{"HwsDdr3TrainingIpSetTopologyGetNext", &wrHwsDdr3TrainingIpSetTopologyGetNext, 1, 0},
{"HwsDdr3TrainingIpStaticConfigSet", &wrHwsDdr3TrainingIpStaticConfigSet, 4, 1},
{"HwsDdr3TrainingIpExternalMemAccessSet", &wrHwsDdr3TrainingIpExternalMemAccessSet, 4, 9},
{"HwsDdr3TrainingIpExternalMemAccessGetFirst", &wrHwsDdr3TrainingIpExternalMemAccessGetFirst, 4, 0},
{"HwsDdr3TrainingIpExternalMemAccessGetNext", &wrHwsDdr3TrainingIpExternalMemAccessGetNext, 4, 0},
{"HwsDdr3TrainingIpDunitRegAccessExtSet", &wrHwsDdr3TrainingIpDunitRegAccessExtSet, 5, 13},
{"HwsDdr3TrainingIpDunitRegAccessExtGetFirst", &wrHwsDdr3TrainingIpDunitRegAccessExtGetFirst, 5, 0},
{"HwsDdr3TrainingIpDunitRegAccessExtGetNext", &wrHwsDdr3TrainingIpDunitRegAccessExtGetNext, 5, 0},
{"HwsDdr3TrainingIpBistActivate", &wrHwsDdr3TrainingIpBistActivate, 10, 0},
{"HwsDdr3TrainingIpBistResultsGetFirst", &wrHwsDdr3TrainingIpBistResultsGetFirst, 3, 0},
{"HwsDdr3TrainingIpBistResultsGetNext", &wrHwsDdr3TrainingIpBistResultsGetNext, 3, 0},
{"HwsDdr3TrainingIpActivate", &wrHwsDdr3TrainingIpActivate, 15, 0},
};


#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitDDRTrainingIP
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
GT_STATUS cmdLibInitDDR3TrainingIP()
{
    return cmdInitLibrary(dbCommands, numCommands);
}



