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
* mvCfgSeqExecuter.c
*
* DESCRIPTION:
*       Run configuration sequence API
*
* FILE REVISION NUMBER:
*       $Revision: 24 $
*
*******************************************************************************/

#include <common/configElementDb/mvCfgElementDb.h>
#include <common/siliconIf/mvSiliconIf.h>

static MV_OPER_RUN_FUNCPTR operArr[LAST_NON_OP];
static MV_PARALLEL_OPER_RUN_FUNCPTR parallelOperArr[LAST_NON_OP];

static MV_SERDES_OPER_RUN_FUNCPTR           serdesOperArr[LAST_NON_OP];
static MV_PARALLEL_SERDES_OPER_RUN_FUNCPTR  serdesParallelOperArr[LAST_NON_OP];

static MV_CFG_SEQ *hwsSeqDb = NULL;
static GT_U8 numberOfSequences = 0;

/******************************* pre-declaration  **************************************/

GT_STATUS mvCfgSeqRunWriteOp
(
    GT_U8            devNum,
    GT_U32           portGroup,
    GT_U32           index,
    MV_CFG_ELEMENT  *element
);

GT_STATUS mvCfgSeqRunDelayOp
(
    GT_U8            devNum,
    GT_U32           portGroup,
    GT_U32           unitBaseAddr,
    MV_CFG_ELEMENT  *element
);

GT_STATUS mvCfgSeqRunPolOp
(
    GT_U8            devNum,
    GT_U32           portGroup,
    GT_U32           index,
    MV_CFG_ELEMENT  *element
);

GT_STATUS mvCfgSeqRunParallelSerdesPolOp
(
	GT_U8            devNum,
	GT_U32           portGroup,
	GT_U32           numOfSer,
	GT_U32           *serdesArr,
	MV_CFG_ELEMENT   *element
);

GT_STATUS mvCfgSeqRunParallelSerdesWriteOp
(
	GT_U8            devNum,
	GT_U32           portGroup,
	GT_U32           numOfSer,
	GT_U32           *serdesArr,
	MV_CFG_ELEMENT  *element
);

GT_STATUS mvCfgSeqRunParallelDelayOp
(
	GT_U8            devNum,
	GT_U32           portGroup,
	GT_U32           numOfSer,
	GT_U32           *serdesArr,
	MV_CFG_ELEMENT   *element
);

GT_STATUS mvCfgSeqRunNewParallelSerdesWriteOp
(
    GT_U8            devNum,
    GT_UOPT          portGroup,
    GT_UOPT          numOfSer,
    GT_UOPT          *serdesArr,
    MV_OP_PARAMS     *writeLine
);

GT_STATUS mvCfgSeqRunNewParallelSerdesPolOp
(
    GT_U8            devNum,
    GT_UOPT          portGroup,
    GT_UOPT          numOfSer,
    GT_UOPT          *serdesArr,
    MV_OP_PARAMS     *polLine
);

GT_STATUS mvCfgSerdesNewSeqParallelExec
(
    GT_U8           devNum,
    GT_UOPT         portGroup,
    GT_UOPT         numOfSer,
    GT_UOPT         *serdesArr,
    GT_UOPT         seqId
);

/***************************************************************************************/

/*******************************************************************************
* mvCfgSeqRunWriteOp
*
* DESCRIPTION:
*      Run write operation
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSeqRunWriteOp
(
    GT_U8            devNum,
    GT_U32           portGroup,
    GT_U32           index,
    MV_CFG_ELEMENT  *element
)
{
    MV_WRITE_OP_PARAMS  *writeLine;
    GT_U32              accessAddr;
    GT_U32              baseAddr;
    GT_U32              unitIndex;

    writeLine = (MV_WRITE_OP_PARAMS *)element->params;

    mvUnitExtInfoGet(devNum, writeLine->indexOffset, index, &baseAddr, &unitIndex , &index);

    accessAddr = baseAddr + index*unitIndex + writeLine->regOffset;

	if ((writeLine->indexOffset == INTLKN_RF_UNIT) || (writeLine->indexOffset == ETI_ILKN_RF_UNIT))
    {
        CHECK_STATUS(genInterlakenRegSet(devNum, portGroup, accessAddr, writeLine->data, writeLine->mask));
    }
    else
    {
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, writeLine->data, writeLine->mask));
    }

    return GT_OK;
}

/*******************************************************************************
* mvCfgSeqRunDelayOp
*
* DESCRIPTION:
*      Run delay operation
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSeqRunDelayOp
(
    GT_U8            devNum,
    GT_U32           portGroup,
    GT_U32           unitBaseAddr,
    MV_CFG_ELEMENT  *element
)
{
    MV_DELAY_OP_PARAMS      *wateLine;

    /* to avoid "unreferenced formal parameter" warning */
    devNum = devNum;
    portGroup = portGroup;
    unitBaseAddr = unitBaseAddr;

    wateLine = (MV_DELAY_OP_PARAMS *)element->params;
    hwsOsExactDelayPtr(devNum, portGroup, wateLine->delay);

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run delay %d msec.\n",wateLine->delay);
    osFclose(fdHws);
#endif

    return GT_OK;
}

/*******************************************************************************
* mvCfgSeqRunPolOp
*
* DESCRIPTION:
*      Run polling operation
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSeqRunPolOp
(
    GT_U8            devNum,
    GT_U32           portGroup,
    GT_U32           index,
    MV_CFG_ELEMENT  *element
)
{
    GT_U32 polData;
    GT_U32 polCntr;
    MV_POLLING_OP_PARAMS    *polLine;
    GT_U32 accessAddr;
    GT_U32              baseAddr;
    GT_U32              unitIndex;

    polLine = (MV_POLLING_OP_PARAMS *)element->params;
    mvUnitExtInfoGet(devNum, polLine->indexOffset, index, &baseAddr, &unitIndex , &index);

    accessAddr = baseAddr + index*unitIndex + polLine->regOffset;

    polCntr = 0;
    do
    {
#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run polling with delay %d msec.\n",polLine->waitTime);
    osFclose(fdHws);
#endif
        CHECK_STATUS(genRegisterGet(devNum, portGroup, accessAddr, &polData, 0));
        if ((polData & polLine->mask) != polLine->data)
        {
          hwsOsExactDelayPtr(devNum, portGroup, polLine->waitTime);
          polCntr++;
        }

    } while (((polData & polLine->mask) != polLine->data) && (polCntr < polLine->numOfLoops));

    if ((polCntr == polLine->numOfLoops) && ((polData & polLine->mask) != polLine->data))
    {
      return GT_TIMEOUT;
    }
    return GT_OK;
}

/*******************************************************************************
* mvCfgSeqExec
*
* DESCRIPTION:
*      Run configuration sequence
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSeqExec
(
    GT_U8            devNum,
    GT_U32           portGroup,
    GT_U32           index,
    MV_CFG_ELEMENT  *element,
    GT_U32           elementSize
)
{
    GT_U32 i;

    if (element == NULL)
    {
        return GT_BAD_PARAM;
    }

    for (i = 0; i < elementSize; i++)
    {
        operArr[element[i].op](devNum,portGroup,index,&element[i]);
    }

    return GT_OK;
}

/*******************************************************************************
* mvCfgSeqRunParallelSerdesPolOp
*
* DESCRIPTION:
*      Run polling operation
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSeqRunParallelSerdesPolOp
(
    GT_U8            devNum,
    GT_U32           portGroup,
    GT_U32           numOfSer,
    GT_U32           *serdesArr,
    MV_CFG_ELEMENT   *element
)
{
    GT_U32					i;
	GT_U32					polData;
    GT_U32					polCntr;
	GT_U32					laneDoneCntr;
	GT_U32					serdesNum;
	GT_U32					curPortGroup;
    MV_POLLING_OP_PARAMS    *polLine;
    GT_U32					accessAddr;
    GT_U32					baseAddr;
    GT_U32					unitIndex;

    polLine = (MV_POLLING_OP_PARAMS *)element->params;
    mvUnitInfoGet(devNum, polLine->indexOffset, &baseAddr, &unitIndex);

    polCntr = 0;
	
    do
    {
		laneDoneCntr = 0;
		
		for (i = 0; i < numOfSer; i++)
		{
			serdesNum = (serdesArr[i] & 0xFFFF);
			curPortGroup = portGroup + ((serdesArr[i] >> 16) & 0xFF);

			accessAddr = baseAddr + serdesNum*unitIndex + polLine->regOffset;

			CHECK_STATUS(genRegisterGet(devNum, curPortGroup, accessAddr, &polData, 0));
			if ((polData & polLine->mask) == polLine->data)
			{
				laneDoneCntr++;
			}
		}

		if (laneDoneCntr == numOfSer)
		{
			break;
		}

		polCntr++;
		hwsOsExactDelayPtr(devNum, portGroup, polLine->waitTime);

    } while (laneDoneCntr != numOfSer && (polCntr < polLine->numOfLoops));

    if ((polCntr == polLine->numOfLoops) && (laneDoneCntr != numOfSer))
    {
		return GT_TIMEOUT;
    }

    return GT_OK;
}

/*******************************************************************************
* mvCfgSeqRunParallelWriteOp
*
* DESCRIPTION:
*      Run write operation
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSeqRunParallelSerdesWriteOp
(
    GT_U8            devNum,
    GT_U32           portGroup,
    GT_U32           numOfSer,
    GT_U32           *serdesArr,
    MV_CFG_ELEMENT  *element
)
{
    MV_WRITE_OP_PARAMS  *writeLine;
	GT_U32				i;
	GT_U32				serdesNum;
	GT_U32				curPortGroup;
    GT_U32              accessAddr;
    GT_U32              baseAddr;
    GT_U32              unitIndex;

    writeLine = (MV_WRITE_OP_PARAMS *)element->params;

    mvUnitInfoGet(devNum, writeLine->indexOffset, &baseAddr, &unitIndex);

	for (i = 0; i < numOfSer; i++)
	{
		serdesNum = (serdesArr[i] & 0xFFFF);
		curPortGroup = portGroup + ((serdesArr[i] >> 16) & 0xFF);
		accessAddr = baseAddr + serdesNum*unitIndex + writeLine->regOffset;

		if ((writeLine->indexOffset == INTLKN_RF_UNIT) || (writeLine->indexOffset == ETI_ILKN_RF_UNIT))
		{
			CHECK_STATUS(genInterlakenRegSet(devNum, curPortGroup, accessAddr, writeLine->data, writeLine->mask));
		}
		else
		{
			CHECK_STATUS(genRegisterSet(devNum, curPortGroup, accessAddr, writeLine->data, writeLine->mask));
		}
	}

    return GT_OK;
}

/*******************************************************************************
* mvCfgSeqRunParallelDelayOp
*
* DESCRIPTION:
*      Run delay operation
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSeqRunParallelDelayOp
(
    GT_U8            devNum,
    GT_U32           portGroup,
    GT_U32           numOfSer,
    GT_U32           *serdesArr,
    MV_CFG_ELEMENT   *element
)
{
    MV_DELAY_OP_PARAMS      *wateLine;

    /* to avoid "unreferenced formal parameter" warning */
    devNum = devNum;
    portGroup = portGroup;
	numOfSer = numOfSer;
    serdesArr = serdesArr;

    wateLine = (MV_DELAY_OP_PARAMS *)element->params;
    hwsOsExactDelayPtr(devNum, portGroup, wateLine->delay);

#ifdef HWS_DEBUG
    if (fdHws == 0)
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "w");
    }
    else
    {
        fdHws = osFopen("C:\\Temp\\Lion2Log.txt", "a");
    }
    osFprintf(fdHws, "\n;Run delay %d msec.\n",wateLine->delay);
    osFclose(fdHws);
#endif

    return GT_OK;
}

/*******************************************************************************
* mvCfgSerdesSeqparallelExec
*
* DESCRIPTION:
*      Run Serdes parallel configuration sequence (in the manner of lanes)
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSerdesSeqParallelExec
(
    GT_U8            devNum,
    GT_U32           portGroup,
    GT_U32           numOfSer,
    GT_U32           *serdesArr,
    MV_CFG_ELEMENT   *element,
    GT_U32           elementSize
)
{
    GT_U32 i;

    if (element == NULL)
    {
        return GT_BAD_PARAM;
    }

    for (i = 0; i < elementSize; i++)
    {
        parallelOperArr[element[i].op](devNum,portGroup,numOfSer, serdesArr,&element[i]);
    }

    return GT_OK;
}
/*******************************************************************************
* mvCfgSeqExecInit
*
* DESCRIPTION:
*      Init configuration sequence executer
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void mvCfgSeqExecInit ()
{
    static GT_BOOL engineInitDone = GT_FALSE;

    if (!engineInitDone)
    {
        operArr[WRITE_OP] = mvCfgSeqRunWriteOp;
        operArr[DELAY_OP] = mvCfgSeqRunDelayOp;
        operArr[POLLING_OP] = mvCfgSeqRunPolOp;

        parallelOperArr[WRITE_OP] = mvCfgSeqRunParallelSerdesWriteOp;
        parallelOperArr[DELAY_OP] = mvCfgSeqRunParallelDelayOp;
        parallelOperArr[POLLING_OP] = mvCfgSeqRunParallelSerdesPolOp;
       engineInitDone = GT_TRUE;
    }
}

/*******************************************************************************
* mvCfgSerdesSeqRunWriteOp
*
* DESCRIPTION:
*      Run write operation
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSerdesSeqRunWriteOp
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       serdesNum,
    MV_OP_PARAMS  *writeLine
)
{
    GT_U8           regType;
    GT_UREG_DATA    data;
    GT_UREG_DATA    mask;

    regType = (writeLine->unitId == SERDES_PHY_UNIT) ? INTERNAL_REG : EXTERNAL_REG;
    mask = (GT_U16) writeLine->mask;
    data = (GT_U16) writeLine->operData;

    CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, regType, (GT_U8)serdesNum,
                                        writeLine->regOffset, data, mask));

    return GT_OK;
}

/*******************************************************************************
* mvCfgSerdesSeqRunDelayOp
*
* DESCRIPTION:
*      Run write operation
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSerdesSeqRunDelayOp
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       serdesNum,
    MV_OP_PARAMS  *wateLine
)
{
	serdesNum = serdesNum; /* avoid warnings */

    hwsOsExactDelayPtr(devNum, portGroup, wateLine->waitTime);

    return GT_OK;
}

/*******************************************************************************
* mvCfgSerdesSeqRunPolOp
*
* DESCRIPTION:
*      Run SERDES polling operation
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSerdesSeqRunPolOp
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       serdesNum,
    MV_OP_PARAMS  *polLine
)
{
    GT_U8        regType;
    GT_UREG_DATA polData;
    GT_UREG_DATA polCntr;

    regType = (polLine->unitId == SERDES_PHY_UNIT) ? INTERNAL_REG : EXTERNAL_REG;

    polCntr = 0;
    do
    {
        CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, (GT_U8)portGroup, regType, (GT_U8)serdesNum,
                                            polLine->regOffset, &polData, 0));

        if ((polData & polLine->mask) != polLine->operData)
        {
          hwsOsExactDelayPtr(devNum, portGroup, polLine->waitTime);
          polCntr++;
        }

    } while (((polData & polLine->mask) != polLine->operData) && (polCntr < polLine->numOfLoops));

    if ((polCntr == polLine->numOfLoops) && ((polData & polLine->mask) != polLine->operData))
    {
      return GT_TIMEOUT;
    }
    return GT_OK;
}

/*******************************************************************************
* mvCfgSeqExec
*
* DESCRIPTION:
*      Run configuration sequence
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSerdesSeqExec
(
    GT_U8         devNum,
    GT_UOPT       portGroup,
    GT_UOPT       serdesNum,
    GT_U8         seqId
)
{
    MV_CFG_SEQ *seq;
    GT_U8 i;

    if (seqId >= numberOfSequences)
    {
        return GT_BAD_PARAM;
    }

    seq = &hwsSeqDb[seqId];
	if (seq->cfgSeqSize == 0)
	{
		return GT_NOT_INITIALIZED;
	}
    for (i = 0; i < seq->cfgSeqSize; i++)
    {
        serdesOperArr[seq->cfgSeq[i].op](devNum, portGroup, serdesNum, seq->cfgSeq[i].params);
    }

    return GT_OK;
}

/*******************************************************************************
* mvCfgSeqRunNewParallelSerdesWriteOp
*
* DESCRIPTION:
*      Run write operation
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSeqRunNewParallelSerdesWriteOp
(
    GT_U8            devNum,
    GT_UOPT          portGroup,
    GT_UOPT          numOfSer,
    GT_UOPT          *serdesArr,
    MV_OP_PARAMS     *writeLine
)
{
    GT_U8           regType;
    GT_UREG_DATA    data;
    GT_UREG_DATA    mask;
    GT_U8  i;


    regType = (writeLine->unitId == SERDES_PHY_UNIT) ? INTERNAL_REG : EXTERNAL_REG;
    mask = (GT_U16) writeLine->mask;
    data = (GT_U16) writeLine->operData;

    for (i = 0; i < numOfSer; i++)
    {
        CHECK_STATUS(hwsSerdesRegSetFuncPtr(devNum, (GT_U8)portGroup, regType, (GT_U8)serdesArr[i],
                                            writeLine->regOffset, data, mask));
    }

    return GT_OK;
}

/*******************************************************************************
* mvCfgSeqRunNewParallelSerdesPolOp
*
* DESCRIPTION:
*      Run polling operation
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSeqRunNewParallelSerdesPolOp
(
    GT_U8           devNum,
    GT_UOPT         portGroup,
    GT_UOPT         numOfSer,
    GT_UOPT         *serdesArr,
    MV_OP_PARAMS    *polLine
)
{
    GT_U8			i;
    GT_U8           regType;
    GT_UREG_DATA    polData;
    GT_U8           polCntr;
    GT_U8           laneDoneCntr;

    regType = (polLine->unitId == SERDES_PHY_UNIT) ? INTERNAL_REG : EXTERNAL_REG;

    polCntr = 0;
    do
    {
        laneDoneCntr = 0;

        for (i = 0; i < numOfSer; i++)
        {

            CHECK_STATUS(hwsSerdesRegGetFuncPtr(devNum, (GT_U8)portGroup, regType, (GT_U8)serdesArr[i],
                                                polLine->regOffset, &polData, 0));

            if ((polData & polLine->mask) == polLine->operData)
            {
                laneDoneCntr++;
            }
        }

        if (laneDoneCntr == numOfSer)
        {
            break;
        }

        polCntr++;
        hwsOsExactDelayPtr(devNum, portGroup, polLine->waitTime);

    } while (laneDoneCntr != numOfSer && (polCntr < polLine->numOfLoops));

    if ((polCntr == polLine->numOfLoops) && (laneDoneCntr != numOfSer))
    {
        return GT_TIMEOUT;
    }

    return GT_OK;
}

/*******************************************************************************
* mvCfgSerdesNewSeqParallelExec
*
* DESCRIPTION:
*      Run configuration sequence
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSerdesNewSeqParallelExec
(
    GT_U8           devNum,
    GT_UOPT         portGroup,
    GT_UOPT         numOfSer,
    GT_UOPT         *serdesArr,
    GT_UOPT         seqId
)
{
    MV_CFG_SEQ *seq;
    GT_U8 i;

    if (seqId >= numberOfSequences)
    {
        return GT_BAD_PARAM;
    }

    seq = &hwsSeqDb[seqId];

    for (i = 0; i < seq->cfgSeqSize; i++)
    {
        serdesParallelOperArr[seq->cfgSeq[i].op](devNum, portGroup, numOfSer, serdesArr, seq->cfgSeq[i].params);
    }

    return GT_OK;
}

/*******************************************************************************
* mvCfgSerdesSeqRunDelayOp
*
* DESCRIPTION:
*      Run write operation
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvCfgSerdesSeqParallelRunDelayOp
(
    GT_U8           devNum,
    GT_UOPT         portGroup,
    GT_UOPT         numOfSer,
    GT_UOPT         *serdesArr,
    MV_OP_PARAMS    *wateLine
)
{
	/* avoid warnings */
	numOfSer = numOfSer;
	*serdesArr = *serdesArr;

    hwsOsExactDelayPtr(devNum, portGroup, wateLine->waitTime);

    return GT_OK;
}


/*******************************************************************************
* mvCfgSerdesSeqExecInit
*
* DESCRIPTION:
*      Init SERDES configuration sequence executer
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
void mvCfgSerdesSeqExecInit
(
    MV_CFG_SEQ *seqDb,
    GT_U8      numberOfSeq
)
{
    hwsSeqDb = seqDb;
    numberOfSequences = numberOfSeq;

    serdesOperArr[WRITE_OP] = mvCfgSerdesSeqRunWriteOp;
    serdesOperArr[DELAY_OP] = mvCfgSerdesSeqRunDelayOp;
    serdesOperArr[POLLING_OP] = mvCfgSerdesSeqRunPolOp;

    serdesParallelOperArr[WRITE_OP] = mvCfgSeqRunNewParallelSerdesWriteOp;
    serdesParallelOperArr[DELAY_OP] = mvCfgSerdesSeqParallelRunDelayOp;
    serdesParallelOperArr[POLLING_OP] = mvCfgSeqRunNewParallelSerdesPolOp;
}

