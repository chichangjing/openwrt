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
* wrDdrTrainingIP.cpp
*
* DESCRIPTION:
*       Implementation of the Galtis Wrapper functions
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 4.*
*******************************************************************************/

/* Common for all HWS Galtis wrappers */
#include <galtisAgent/wrapCpss/systemLab/wrHwsCommon.h>

/* General H Files */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <mvDdr3TrainingIpPrvIf.h>
#include <mvDdr3TrainingIpBist.h>



/******************************* Globals ******************************/
GT_U32 LastActiveInterfaces;
GT_U32 CurrActiveInterfaces;

GT_U32 CurrActiveBus;

/************************************************************************/



/************************************************************************/
/*	This functionality was added to support access to DDR3 using CPSS when DDR
	is used by MSYS only. 
	It was done to allow HW validation.
*/

#include <common/siliconIf/mvSiliconIf.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>

extern MV_HWS_TIP_CONFIG_FUNC_DB configFuncInfo[HWS_MAX_DEVICE_NUM];

MV_HWS_TOPOLOGY_MAP TopologyMap =
{
	0x1, /* active interfaces */
		/*cs_mask, mirror, dqs_swap, ck_swap X PUPs                        speed_bin        memory_width  mem_size frequency   casL  casWL  temperature */
	{{{{0x1,0,0,0}, {0x1,0,0,0}, {0x3,0,0,0}, {0x3,0,0,0}, {0x0,0,0,0}}, SPEED_BIN_DDR_1866L, BUS_WIDTH_8, MEM_4G, DDR_FREQ_400, 0 ,   0 , MV_HWS_TEMP_LOW}},
	0x7  /* Buses mask */
};

void setAc3BusMask(GT_U32 busMask)
{
    cpssOsPrintf("Bus mask was changed to 0x%x (was 0x%x)\n", busMask, TopologyMap.activeBusMask);
    TopologyMap.activeBusMask = (GT_U8)busMask;
}

GT_STATUS    hwsDdr3TipIFWrite
( 
	GT_U8                 devNum,
	MV_HWS_ACCESS_TYPE    interfaceAccess,
	GT_U32                interfaceId, 
	GT_U32                regAddr, 
	GT_U32                dataValue,
	GT_U32                mask 
)
{
	return cpssDrvPpHwInternalPciRegWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr, dataValue);
}

GT_STATUS    hwsDdr3TipIFRead
( 
    GT_U8                 devNum,
    MV_HWS_ACCESS_TYPE    interfaceAccess,
    GT_U32                interfaceId, 
    GT_U32                regAddr, 
    GT_U32                *data,
    GT_U32                mask 
)
{
	return cpssDrvPpHwInternalPciRegRead(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, regAddr, data);
}

GT_STATUS hwsInitTrainingIpIfInit(GT_U32 devNum)
{
	static GT_BOOL initDone = GT_FALSE;
	MV_HWS_TRAINING_IP_FUNC_PTRS *funcPtrArray;

	if (initDone == GT_TRUE)
	{
		return GT_OK;
	}

	cpssOsPrintf("Init DDR DB for AC3\n");
	
	mvHwsTrainingIpGetFuncPtr(&funcPtrArray);

	funcPtrArray->trainingIpIfRead = (MV_HWS_TRAINING_IP_IF_READ_FUNC_PTR)hwsDdr3TipIFRead;
	funcPtrArray->trainingIpIfWrite = (MV_HWS_TRAINING_IP_IF_WRITE_FUNC_PTR)hwsDdr3TipIFWrite;
	funcPtrArray->trainingIpBusRead = mvHwsDdr3TipBUSRead;
	funcPtrArray->trainingIpBusWrite = mvHwsDdr3TipBUSWrite;

	configFuncInfo[devNum].tipDunitReadFunc = hwsDdr3TipIFRead;
	configFuncInfo[devNum].tipDunitWriteFunc = hwsDdr3TipIFWrite;

	/* load topology map (for AC3 DB) */
	CHECK_STATUS(mvHwsDdr3TipLoadTopologyMap(devNum, &TopologyMap));
    cpssOsPrintf("Active bus mask is 0x%x\n", TopologyMap.activeBusMask);

	initDone = GT_TRUE;

	return GT_OK;
}


/************************************************************************/


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
static CMD_STATUS wrHwsDdr3TrainingIpPbsTx
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];


    res = mvHwsTrainingIpPbsTx(globalDevNum);
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
static CMD_STATUS wrHwsDdr3TrainingIpSelectDdrController
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];
    GT_U32  Enable = (GT_U32)inArgs[1];


    res = mvHwsTrainingIpSelectController(globalDevNum, (GT_BOOL)Enable);
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
static CMD_STATUS wrHwsDdr3TrainingIpPbsRx
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];


    res = mvHwsTrainingIpPbsRx(globalDevNum);
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
static CMD_STATUS wrHwsDdr3TrainingIpInitController
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;
	InitCntrParam  initCntrPrm;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];

	initCntrPrm.isCtrl64Bit = GT_FALSE;
	initCntrPrm.doMrsPhy = GT_TRUE;

    res = mvHwsTrainingIpInitController(globalDevNum, &initCntrPrm);
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
static CMD_STATUS wrHwsDdr3TrainingIpFreqSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum    = (GT_U32)inArgs[0];
    GT_U32  accessType      = (GT_U32)inArgs[1];
    GT_U32  interfaceId     = (GT_U32)inArgs[2];
    GT_U32  memoryFreq      = (GT_U32)inArgs[3];

    res = mvHwsTrainingIpSetFreq(globalDevNum, (MV_HWS_ACCESS_TYPE)accessType, interfaceId,
                                 (MV_HWS_DDR_FREQ)memoryFreq);
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
static CMD_STATUS wrHwsDdr3TrainingIpStaticConfig
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum    = (GT_U32)inArgs[0];
    GT_U32  memoryFreq      = (GT_U32)inArgs[1];
    GT_U32  type            = (GT_U32)inArgs[2];
    GT_U32  interfaceID     = (GT_U32)inArgs[3];


    res = mvHwsTrainingIpStaticConfig(globalDevNum, (MV_HWS_DDR_FREQ)memoryFreq, (MV_HWS_STATIC_COFIG_TYPE)type, interfaceID);
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
static CMD_STATUS wrHwsDdr3TrainingIpAlgoRun
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum    = (GT_U32)inArgs[0];
    GT_U32  algoType        = (GT_U32)inArgs[1];


    res = mvHwsTrainingIpAlgoRun(globalDevNum, (MV_HWS_ALGO_TYPE)algoType);
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
static CMD_STATUS wrHwsDdr3TrainingIpPhyRegAccessSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum        = (GT_U32)inArgs[0];
    GT_U32  interfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  interfaceID         = (GT_U32)inArgs[2];
    GT_U32  phyAccessType       = (GT_U32)inArgs[3];
    GT_U32  phyNum              = (GT_U32)inArgs[4];
    GT_U32  phyType             = (GT_U32)inArgs[5];
    GT_U32  regAddress          = (GT_U32)inArgs[6];

    GT_U32  data;

    /* Fields List */

    if(phyAccessType == 0)
    {
        data = (GT_U32)inFields[phyNum + 1];
    }
    else
    {
        data = (GT_U32)inFields[1];
    }

    if(phyNum == 4) /* ECC Pup */
    {
        phyAccessType = 0; /* UNICAST */
        data = (GT_U32)inFields[1];
    }

	if(HWS_DEV_SILICON_TYPE(globalDevNum) == Alleycat3A0)
	{
        res = hwsInitTrainingIpIfInit(globalDevNum);
		if (res != GT_OK)
		{
			galtisOutput(outArgs, res, "");
			return CMD_OK;
		}
	}

	res = mvHwsTrainingIpBusWrite(globalDevNum, (MV_HWS_ACCESS_TYPE)interfaceAccessType, interfaceID,
					(MV_HWS_ACCESS_TYPE)phyAccessType, phyNum, (MV_HWS_DDR_PHY)phyType, regAddress, data);
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
static CMD_STATUS wrHwsDdr3TrainingIpPhyRegAccessGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 data[4] = {0};
	GT_U32 *dataPtr;

    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum        = (GT_U32)inArgs[0];
    GT_U32  interfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  interfaceID         = (GT_U32)inArgs[2];
    GT_U32  phyAccessType       = (GT_U32)inArgs[3];
    GT_U32  phyNum              = (GT_U32)inArgs[4];
    GT_U32  phyType             = (GT_U32)inArgs[5];
    GT_U32  regAddress          = (GT_U32)inArgs[6];

    /* Warnings */
    interfaceAccessType = interfaceAccessType;

	dataPtr = (phyAccessType == 0 /*UNICAST*/) ? &data[phyNum] : data;

    if(phyNum == 4) /* ECC Pup */
    {
        /* cpssOsPrintf("Reading from Pup 4\n"); */
        phyAccessType = 0; /* UNICAST */
        dataPtr = &data[0];
    }

	if(HWS_DEV_SILICON_TYPE(globalDevNum) == Alleycat3A0)
	{
		res = hwsInitTrainingIpIfInit(globalDevNum);
		if (res != GT_OK)
		{
			galtisOutput(outArgs, res, "");
			return CMD_OK;
		}
	}

	res = mvHwsTrainingIpBusRead(globalDevNum, interfaceID, (MV_HWS_ACCESS_TYPE)phyAccessType, phyNum,
									 (MV_HWS_DDR_PHY)phyType, regAddress, dataPtr);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    /* cpssOsPrintf("Data is 0x%x\n", *dataPtr); */

    /* Fields List */
    inFields[0] = interfaceID; /* Data */

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
static CMD_STATUS wrHwsDdr3TrainingIpPhyRegAccessGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
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
static CMD_STATUS wrHwsDdr3TrainingIpDunitRegAccessSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum        = (GT_U32)inArgs[0];
    GT_U32  interfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  interfaceID         = (GT_U32)inArgs[2];
    GT_U32  regAddress          = (GT_U32)inArgs[3];
    GT_U32  regMask             = (GT_U32)inArgs[4];
    GT_U32  data;

    data = (GT_U32)inFields[0];

    if (regMask == 0)
    {
        regMask = 0xffffffff;
    }

	if(HWS_DEV_SILICON_TYPE(globalDevNum) == Alleycat3A0)
	{
		res = hwsInitTrainingIpIfInit(globalDevNum);
		if (res != GT_OK)
		{
			galtisOutput(outArgs, res, "");
			return CMD_OK;
		}
	}
	
	res = mvHwsTrainingIpIfWrite(globalDevNum, (MV_HWS_ACCESS_TYPE)interfaceAccessType, interfaceID, regAddress, data, regMask);
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
static CMD_STATUS wrHwsDdr3TrainingIpDunitRegAccessGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 data;
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum        = (GT_U32)inArgs[0];
    GT_U32  interfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  interfaceID         = (GT_U32)inArgs[2];
    GT_U32  regAddress          = (GT_U32)inArgs[3];
    GT_U32  regMask             = (GT_U32)inArgs[4];

    if (regMask == 0)
    {
        regMask = 0xffffffff;
    }

	if(HWS_DEV_SILICON_TYPE(globalDevNum) == Alleycat3A0)
	{
		res = hwsInitTrainingIpIfInit(globalDevNum);
		if (res != GT_OK)
		{
			galtisOutput(outArgs, res, "");
			return CMD_OK;
		}
	}
	
	res = mvHwsTrainingIpIfRead(globalDevNum, (MV_HWS_ACCESS_TYPE)interfaceAccessType, interfaceID, regAddress, &data, regMask);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    /* Fields List */
    inFields[0] = data; /* Data */
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
static CMD_STATUS wrHwsDdr3TrainingIpDunitRegAccessGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    galtisOutput(outArgs, GT_OK, "%d", -1);
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
static CMD_STATUS wrHwsDdr3TrainingIpStaticConfigSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];
    GT_U32  enable = (GT_U32)inArgs[1];

    /* Fields List */
    GT_U32  data = (GT_U32)inFields[0];

    /* Warnings */
    data = data;

    res = mvHwsTrainingIpSelectController(globalDevNum, (GT_BOOL)enable);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}


static GT_U32 currrentLine;     /* table line */

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
static CMD_STATUS wrHwsDdr3TrainingIpExternalMemAccessSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS res;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];
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

    res = mvHwsTrainingIpExtWrite(globalDevNum, memoryNumber, baseAddress, size, wordArr);
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
static CMD_STATUS wrHwsDdr3TrainingIpExternalMemAccessGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];
    GT_U32  memoryNumber = (GT_U32)inArgs[1];
    GT_U32  baseAddress  = (GT_U32)inArgs[2];
    GT_U32  size         = (GT_U32)inArgs[3];
    /* Fields List */
    GT_U32 wordArr[8];

    /* Warnings */
    size = size;

    cpssOsMemSet(wordArr, 0, sizeof(wordArr));
    currrentLine = 1;
    inFields[0] = baseAddress;

    res = mvHwsTrainingIpExtRead(globalDevNum, memoryNumber, baseAddress, 1, wordArr);
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
static CMD_STATUS wrHwsDdr3TrainingIpExternalMemAccessGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32      addrInd=0x20;
    GT_STATUS   res;


    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];
    GT_U32  memoryNumber = (GT_U32)inArgs[1];
    GT_U32  baseAddress  = (GT_U32)inArgs[2];
    GT_U32  size         = (GT_U32)inArgs[3];
    /* Fields List */
    GT_U32 wordArr[8];

    if (currrentLine >= size)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    cpssOsMemSet(wordArr, 0, sizeof(wordArr));

    inFields[0] = baseAddress + currrentLine*addrInd;

    res = mvHwsTrainingIpExtRead(globalDevNum, memoryNumber, (GT_U32)inFields[0], 1, wordArr);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    currrentLine++;

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
static CMD_STATUS wrHwsDdr3TrainingIpDunitRegAccessExtSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum        = (GT_U32)inArgs[0];
    GT_U32  interfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  interfaceID         = (GT_U32)inArgs[2];
    GT_U32  regAddress          = (GT_U32)inArgs[3];
    GT_U32  regMask             = (GT_U32)inArgs[4];
    GT_U32  data;

    if(interfaceAccessType == 0)
    {
        data = (GT_U32)inFields[interfaceID + 1];
    }
    else
    {
        data = (GT_U32)inFields[1];
    }

    if (regMask == 0)
    {
        regMask = 0xffffffff;
    }

	if(HWS_DEV_SILICON_TYPE(globalDevNum) == Alleycat3A0)
	{
		res = hwsInitTrainingIpIfInit(globalDevNum);
		if (res != GT_OK)
		{
			galtisOutput(outArgs, res, "");
			return CMD_OK;
		}
	}

    res = mvHwsTrainingIpIfWrite(globalDevNum, (MV_HWS_ACCESS_TYPE)interfaceAccessType, interfaceID, regAddress, data, regMask);
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
static CMD_STATUS wrHwsDdr3TrainingIpDunitRegAccessExtGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 data[12] = {0};
    GT_STATUS  res;

    /* Parameters List */
    GT_U32  globalDevNum        = (GT_U32)inArgs[0];
    GT_U32  interfaceAccessType = (GT_U32)inArgs[1];
    GT_U32  interfaceID         = (GT_U32)inArgs[2];
    GT_U32  regAddress          = (GT_U32)inArgs[3];
    GT_U32  regMask             = (GT_U32)inArgs[4];

    if (regMask == 0)
    {
        regMask = 0xffffffff;
    }

	if(HWS_DEV_SILICON_TYPE(globalDevNum) == Alleycat3A0)
	{
		res = hwsInitTrainingIpIfInit(globalDevNum);
		if (res != GT_OK)
		{
			galtisOutput(outArgs, res, "");
			return CMD_OK;
		}
	}

    if (interfaceAccessType == 0)
    {
        res = mvHwsTrainingIpIfRead(globalDevNum, (MV_HWS_ACCESS_TYPE)interfaceAccessType, interfaceID, regAddress, &data[interfaceID], regMask);
    }
    else
    {
        res = mvHwsTrainingIpIfRead(globalDevNum, (MV_HWS_ACCESS_TYPE)interfaceAccessType, interfaceID, regAddress, data, regMask);
    }
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    inFields[0] = interfaceID;
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
static CMD_STATUS wrHwsDdr3TrainingIpDunitRegAccessExtGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
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
static CMD_STATUS wrHwsDdr3TrainingIpBistActivate
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
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
                                      offset, 0, patternLength);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, res, "");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

static GT_U32 bistCurrentInterface;

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
static CMD_STATUS wrHwsDdr3TrainingIpBistResultsGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
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
        bistCurrentInterface =  MAX_INTERFACE_NUM;     
    }
    else                            /* multicast */
    {
        bistCurrentInterface = 0;
	    res = mvHwsTrainingIpBistReadResult(devNum, bistCurrentInterface, &bistResult);
        inFields[0] = bistCurrentInterface; 
        bistCurrentInterface++;
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
static CMD_STATUS wrHwsDdr3TrainingIpBistResultsGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;
    BistResult bistResult;

    /* Parameters List */
    GT_U32  devNum              = (GT_U32)inArgs[0];

    if (bistCurrentInterface >= MAX_INTERFACE_NUM)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

	inFields[0] = bistCurrentInterface;

	res = mvHwsTrainingIpBistReadResult(devNum, bistCurrentInterface, &bistResult);
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
    
    bistCurrentInterface++;

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
* wrHwsDdr3TipTopologyOfBusSet
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
static CMD_STATUS wrHwsDdr3TipTopologyOfBusSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    MV_HWS_TOPOLOGY_MAP *topologyMap;
    BusParams* busParams;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];
    GT_U32  InterfaceID  = (GT_U32)inArgs[1];

    /* Fields List */
    GT_U32  BusNum          = (GT_U32)inFields[0];
    GT_U32  Active          = (GT_U32)inFields[1];
    GT_U32  CsBitMask       = (GT_U32)inFields[2];
    GT_U32  MirrorEnBitMask = (GT_U32)inFields[3];
    GT_U32  DqsSwap         = (GT_U32)inFields[4];
    GT_U32  CkSwap          = (GT_U32)inFields[5];

    topologyMap = ddr3TipGetTopologyMap(globalDevNum);
    if (topologyMap == NULL)
    {
        cpssOsPrintf("Error: Topology Map is not initialized for device %d\n", globalDevNum);
        galtisOutput(outArgs, GT_NOT_INITIALIZED, "");
        return CMD_OK;
    }

    if(IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, InterfaceID) == GT_FALSE) 
    {
        cpssOsPrintf("Error: Interface %d is not activated (interface mask is 0x%x)\n", 
                     InterfaceID, topologyMap->interfaceActiveMask);
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    /* enable/Disable bus in/from bus mask */
    topologyMap->activeBusMask = (GT_U8)((topologyMap->activeBusMask & ~(1 << BusNum)) | 
                                         (Active << BusNum));

    /* get bus params */
    busParams = &(topologyMap->interfaceParams[InterfaceID].asBusParams[BusNum]);

    busParams->csBitmask = (GT_U8)CsBitMask;          
    busParams->mirrorEnableBitmask = (GT_BOOL)MirrorEnBitMask;
    busParams->isDqsSwap = (GT_BOOL)DqsSwap;          
    busParams->isCkSwap = (GT_BOOL)CkSwap;           

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TipTopologyOfBusGetFirst
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
static CMD_STATUS wrHwsDdr3TipTopologyOfBusGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    MV_HWS_TOPOLOGY_MAP *topologyMap;
    BusParams* busParams;
    GT_U32 isActive;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];
    GT_U32  InterfaceID  = (GT_U32)inArgs[1];
    
    topologyMap = ddr3TipGetTopologyMap(globalDevNum);
    if (topologyMap == NULL)
    {
        cpssOsPrintf("Error: Topology Map is not initialized for device %d\n", globalDevNum);
        galtisOutput(outArgs, GT_NOT_INITIALIZED, "");
        return CMD_OK;
    }

    if(IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, InterfaceID) == GT_FALSE) 
    {
        cpssOsPrintf("Error: Interface %d is not activated (interface mask is 0x%x)\n", 
            InterfaceID, topologyMap->interfaceActiveMask);
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    CurrActiveBus = 0;

    busParams = &(topologyMap->interfaceParams[InterfaceID].asBusParams[CurrActiveBus]);
    isActive = (topologyMap->activeBusMask >> CurrActiveBus) & 0x1;

    /* Fields List */
    inFields[0] = (GT_32)CurrActiveBus;                  /* BusNum */
    inFields[1] = (GT_32)isActive;                       /* Active */
    inFields[2] = (GT_32)busParams->csBitmask;           /* CsBitMask */
    inFields[3] = (GT_32)busParams->mirrorEnableBitmask; /* MirrorEnBitMask */
    inFields[4] = (GT_32)busParams->isDqsSwap;           /* DqsSwap */
    inFields[5] = (GT_32)busParams->isCkSwap;            /* CkSwap */

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4], inFields[5]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TipTopologyOfBusGetNext
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
static CMD_STATUS wrHwsDdr3TipTopologyOfBusGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    MV_HWS_TOPOLOGY_MAP *topologyMap;
    BusParams* busParams;
    GT_U32 isActive;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];
    GT_U32  InterfaceID  = (GT_U32)inArgs[1];

    topologyMap = ddr3TipGetTopologyMap(globalDevNum);
    if (topologyMap == NULL)
    {
        cpssOsPrintf("Error: Topology Map is not initialized for device %d\n", globalDevNum);
        galtisOutput(outArgs, GT_NOT_INITIALIZED, "");
        return CMD_OK;
    }

    if(IS_INTERFACE_ACTIVE(topologyMap->interfaceActiveMask, InterfaceID) == GT_FALSE) 
    {
        cpssOsPrintf("Error: Interface %d is not activated (interface mask is 0x%x)\n", 
            InterfaceID, topologyMap->interfaceActiveMask);
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    /* move to next bus */
    CurrActiveBus++;

    /* check if no more buses to read */
    if (CurrActiveBus == ddr3TipDevAttrGet(globalDevNum, MV_ATTR_OCTET_PER_INTERFACE))
    {
        /* done iterating interfaces */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    busParams = &(topologyMap->interfaceParams[InterfaceID].asBusParams[CurrActiveBus]);
    isActive = (topologyMap->activeBusMask >> CurrActiveBus) & 0x1;

    /* Fields List */
    inFields[0] = (GT_32)CurrActiveBus;                  /* BusNum */
    inFields[1] = (GT_32)isActive;                       /* Active */
    inFields[2] = (GT_32)busParams->csBitmask;           /* CsBitMask */
    inFields[3] = (GT_32)busParams->mirrorEnableBitmask; /* MirrorEnBitMask */
    inFields[4] = (GT_32)busParams->isDqsSwap;           /* DqsSwap */
    inFields[5] = (GT_32)busParams->isCkSwap;            /* CkSwap */

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4], inFields[5]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TipTopologyOfInterfaceSet
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
static CMD_STATUS wrHwsDdr3TipTopologyOfInterfaceSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    MV_HWS_TOPOLOGY_MAP *topologyMap;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];

    /* Fields List */
    GT_U32  InterfaceID = (GT_U32)inFields[0];
    GT_U32  Active      = (GT_U32)inFields[1];
    GT_U32  SpeedBin    = (GT_U32)inFields[2];
    GT_U32  MemBusWidth = (GT_U32)inFields[3];
    GT_U32  MemSize     = (GT_U32)inFields[4];
    GT_U32  TargetFreq  = (GT_U32)inFields[5];
    GT_U32  CasWL       = (GT_U32)inFields[6];
    GT_U32  CasL        = (GT_U32)inFields[7];
    GT_U32  Temperature = (GT_U32)inFields[8];

    topologyMap = ddr3TipGetTopologyMap(globalDevNum);
    if (topologyMap == NULL)
    {
        cpssOsPrintf("Error: Topology Map is not initialized for device %d\n", globalDevNum);
        galtisOutput(outArgs, GT_NOT_INITIALIZED, "");
        return CMD_OK;
    }

    topologyMap->interfaceActiveMask = (GT_U8)((topologyMap->interfaceActiveMask & ~(1 << InterfaceID)) | 
                                               (Active << InterfaceID));

    topologyMap->interfaceParams[InterfaceID].speedBinIndex = (MV_HWS_SPEED_BIN)SpeedBin;
    topologyMap->interfaceParams[InterfaceID].busWidth = (MV_HWS_BUS_WIDTH)MemBusWidth;
    topologyMap->interfaceParams[InterfaceID].memorySize = (MV_HWS_MEM_SIZE)MemSize;
    topologyMap->interfaceParams[InterfaceID].memoryFreq = (MV_HWS_DDR_FREQ)TargetFreq;
    topologyMap->interfaceParams[InterfaceID].casWL = (GT_U8)CasWL;
    topologyMap->interfaceParams[InterfaceID].casL = (GT_U8)CasL;
    topologyMap->interfaceParams[InterfaceID].interfaceTemp = (MV_HWS_TEMPERTURE)Temperature;

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TipTopologyOfInterfaceGetFirst
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
static CMD_STATUS wrHwsDdr3TipTopologyOfInterfaceGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    MV_HWS_TOPOLOGY_MAP *topologyMap;
    InterfaceParams* interfaceParams;
    GT_U32  InterfaceID;
    GT_U32 i;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];

    topologyMap = ddr3TipGetTopologyMap(globalDevNum);
    if (topologyMap == NULL)
    {
        cpssOsPrintf("Error: Topology Map is not initialized for device %d\n", globalDevNum);
        galtisOutput(outArgs, GT_NOT_INITIALIZED, "");
        return CMD_OK;
    }

    /* calc the last active interfaces */
    LastActiveInterfaces = 0;

    for(i = 0; i < 32; i++)
    {
        if (topologyMap->interfaceActiveMask & (1 << i))
        {
            LastActiveInterfaces = i;
        }
    }

    /* set current interface ID */
    InterfaceID = CurrActiveInterfaces = 0;

    /* get interface params */
    interfaceParams = &topologyMap->interfaceParams[InterfaceID];

    /* Fields List */
    inFields[0] = (GT_32)InterfaceID;                       /* InterfaceID */
    inFields[1] = (GT_32)(topologyMap->interfaceActiveMask >> InterfaceID) & 0x1; /* Active */
    inFields[2] = (GT_32)interfaceParams->speedBinIndex ;   /* SpeedBin */
    inFields[3] = (GT_32)interfaceParams->busWidth;         /* MemBusWidth */
    inFields[4] = (GT_32)interfaceParams->memorySize ;      /* MemSize */
    inFields[5] = (GT_32)interfaceParams->memoryFreq;       /* TargetFreq */
    inFields[6] = (GT_32)interfaceParams->casWL;            /* CasWL */
    inFields[7] = (GT_32)interfaceParams->casL;             /* CasL */
    inFields[8] = (GT_32)interfaceParams->interfaceTemp;    /* Temperature */

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4], inFields[5], inFields[6], inFields[7], inFields[8]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TipTopologyOfInterfaceGetNext
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
static CMD_STATUS wrHwsDdr3TipTopologyOfInterfaceGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    MV_HWS_TOPOLOGY_MAP *topologyMap;
    InterfaceParams* interfaceParams;
    GT_U32  InterfaceID;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];
    
    if (CurrActiveInterfaces == LastActiveInterfaces)
    {
        /* done iterating interfaces */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    topologyMap = ddr3TipGetTopologyMap(globalDevNum);
    if (topologyMap == NULL)
    {
        cpssOsPrintf("Error: Topology Map is not initialized for device %d\n", globalDevNum);
        galtisOutput(outArgs, GT_NOT_INITIALIZED, "");
        return CMD_OK;
    }

    /* set current interface ID */
    InterfaceID = ++CurrActiveInterfaces;

    /* get interface params */
    interfaceParams = &topologyMap->interfaceParams[InterfaceID];

    /* Fields List */
    inFields[0] = (GT_32)InterfaceID;                       /* InterfaceID */
    inFields[1] = (GT_32)(topologyMap->interfaceActiveMask >> InterfaceID) & 0x1; /* Active */
    inFields[2] = (GT_32)interfaceParams->speedBinIndex ;   /* SpeedBin */
    inFields[3] = (GT_32)interfaceParams->busWidth;         /* MemBusWidth */
    inFields[4] = (GT_32)interfaceParams->memorySize ;      /* MemSize */
    inFields[5] = (GT_32)interfaceParams->memoryFreq;       /* TargetFreq */
    inFields[6] = (GT_32)interfaceParams->casWL;            /* CasWL */
    inFields[7] = (GT_32)interfaceParams->casL;             /* CasL */
    inFields[8] = (GT_32)interfaceParams->interfaceTemp;    /* Temperature */

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4], inFields[5], inFields[6], inFields[7], inFields[8]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TipTopologySet
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
static CMD_STATUS wrHwsDdr3TipTopologySet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    MV_HWS_TOPOLOGY_MAP *topologyMap;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];

    /* Fields List */
    GT_U32  InterfaceActiveMask  = (GT_U32)inFields[0];
    /*GT_U32  NumOfBusPerInterface = (GT_U32)inFields[1];*/
    GT_U32  ActiveBusMask        = (GT_U32)inFields[2];

    topologyMap = ddr3TipGetTopologyMap(globalDevNum);
    if (topologyMap == NULL)
    {
        cpssOsPrintf("Error: Topology Map is not initialized for device %d\n", globalDevNum);
        galtisOutput(outArgs, GT_NOT_INITIALIZED, "");
        return CMD_OK;
    }

    topologyMap->interfaceActiveMask = (GT_U8)InterfaceActiveMask;
    /*numOfBusPerInterface = (GT_U8)NumOfBusPerInterface;*/
    topologyMap->activeBusMask = (GT_U8)ActiveBusMask;

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TipTopologyGetFirst
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
static CMD_STATUS wrHwsDdr3TipTopologyGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    MV_HWS_TOPOLOGY_MAP *topologyMap;

    /* Parameters List */
    GT_U32  globalDevNum = (GT_U32)inArgs[0];

    topologyMap = ddr3TipGetTopologyMap(globalDevNum);
    if (topologyMap == NULL)
    {
        cpssOsPrintf("Error: Topology Map is not initialized for device %d\n", globalDevNum);
        galtisOutput(outArgs, GT_NOT_INITIALIZED, "");
        return CMD_OK;
    }

    /* Fields List */
    inFields[0] = (GT_32)topologyMap->interfaceActiveMask;   /* InterfaceActiveMask  */
    /*inFields[1] = (GT_32)topologyMap->numOfBusPerInterface ;*/ /* NumOfBusPerInterface */
    inFields[2] = (GT_32)topologyMap->activeBusMask;         /* ActiveBusMask        */

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3TipTopologyGetNext
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
static CMD_STATUS wrHwsDdr3TipTopologyGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    galtisOutput(outArgs, GT_OK, "%d", -1);
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
{"HwsDdr3TipTopologyOfBusSet", &wrHwsDdr3TipTopologyOfBusSet, 2, 6},
{"HwsDdr3TipTopologyOfBusGetFirst", &wrHwsDdr3TipTopologyOfBusGetFirst, 2, 0},
{"HwsDdr3TipTopologyOfBusGetNext", &wrHwsDdr3TipTopologyOfBusGetNext, 2, 0},
{"HwsDdr3TipTopologyOfInterfaceSet", &wrHwsDdr3TipTopologyOfInterfaceSet, 1, 9},
{"HwsDdr3TipTopologyOfInterfaceGetFirst", &wrHwsDdr3TipTopologyOfInterfaceGetFirst, 1, 0},
{"HwsDdr3TipTopologyOfInterfaceGetNext", &wrHwsDdr3TipTopologyOfInterfaceGetNext, 1, 0},
{"HwsDdr3TipTopologySet", &wrHwsDdr3TipTopologySet, 1, 3},
{"HwsDdr3TipTopologyGetFirst", &wrHwsDdr3TipTopologyGetFirst, 1, 0},
{"HwsDdr3TipTopologyGetNext", &wrHwsDdr3TipTopologyGetNext, 1, 0},
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



