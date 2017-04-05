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
* mvCfgSeqDbIf.c
*
* DESCRIPTION:
*       Run configuration sequence API
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*
*******************************************************************************/

#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/mvSiliconIf.h>

/*******************************************************************************
* mvCreateWriteOp
*
* DESCRIPTION:
*      Create write operation
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
void mvCreateWriteOp
(
    MV_CFG_SEQ          *seq,
    GT_U32              curSeqNum,
    MV_WRITE_OP_PARAMS  oper
)
{
  MV_WRITE_OP_PARAMS *setTmp;

  if (curSeqNum >= seq->cfgSeqSize)
  {
    return;
  }
  seq->cfgSeq[curSeqNum].op = WRITE_OP;
  seq->cfgSeq[curSeqNum].params = (void *)hwsOsMallocFuncPtr(sizeof(MV_WRITE_OP_PARAMS));
  if (seq->cfgSeq[curSeqNum].params != NULL)
  {
      setTmp = (MV_WRITE_OP_PARAMS*)seq->cfgSeq[curSeqNum].params;
      setTmp->indexOffset = oper.indexOffset;
      setTmp->regOffset = oper.regOffset;
      setTmp->data = oper.data;
      setTmp->mask = oper.mask;
  }
  return;
}

/*******************************************************************************
* mvCreateDelayOp
*
* DESCRIPTION:
*      Create delay operation
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
void mvCreateDelayOp
(
    MV_CFG_SEQ *seq,
    GT_U32   curSeqNum,
    GT_U32   delay
)
{
  MV_DELAY_OP_PARAMS *setTmp;

  if (curSeqNum >= seq->cfgSeqSize)
  {
    return;
  }
  seq->cfgSeq[curSeqNum].op = DELAY_OP;
  seq->cfgSeq[curSeqNum].params = (void *)hwsOsMallocFuncPtr(sizeof(MV_DELAY_OP_PARAMS));
  if (seq->cfgSeq[curSeqNum].params != NULL)
  {
      setTmp = (MV_DELAY_OP_PARAMS*)seq->cfgSeq[curSeqNum].params;
      setTmp->delay = (GT_U16)delay;
  }
  return;
}

/*******************************************************************************
* mvCreatePollingOp
*
* DESCRIPTION:
*      Create polling operation
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
void mvCreatePollingOp
(
    MV_CFG_SEQ            *seq,
    GT_U32                curSeqNum,
    MV_POLLING_OP_PARAMS  oper
)
{
  MV_POLLING_OP_PARAMS *setTmp;

  if (curSeqNum >= seq->cfgSeqSize)
  {
    return;
  }
  seq->cfgSeq[curSeqNum].op = POLLING_OP;
  seq->cfgSeq[curSeqNum].params = (void *)hwsOsMallocFuncPtr(sizeof(MV_POLLING_OP_PARAMS));
  if (seq->cfgSeq[curSeqNum].params != NULL)
  {
      setTmp = (MV_POLLING_OP_PARAMS*)seq->cfgSeq[curSeqNum].params;
      setTmp->indexOffset = oper.indexOffset;
      setTmp->regOffset = oper.regOffset;
      setTmp->data = oper.data;
      setTmp->mask = oper.mask;
      setTmp->numOfLoops = oper.numOfLoops;
      setTmp->waitTime = oper.waitTime;
  }
  return;
}

/*******************************************************************************
* mvGetSeqOperation
*
* DESCRIPTION:
*      Get sequence operation
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
GT_STATUS mvGetSeqOperation
(
    MV_CFG_SEQ     *seq,
    MV_CFG_ELEMENT *seqLine,
    GT_U32         lineNum
)
{
    if (seqLine == NULL)
    {
        return GT_BAD_PTR;
    }

    if (seq->cfgSeqSize <= lineNum)
    {
        return GT_NO_MORE;
    }

    *seqLine = seq->cfgSeq[lineNum];
    return GT_OK;
}

/*******************************************************************************
* mvSetSeqOperation
*
* DESCRIPTION:
*      Set sequence operation
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
GT_STATUS mvSetSeqOperation
(
    GT_BOOL        firstLine,
    MV_CFG_SEQ     *seq,
    GT_U32         curSeqNum,
    MV_CFG_ELEMENT *seqLine
)
{
    GT_U32 seqNum;

    if (seqLine == NULL)
    {
        return GT_BAD_PTR;
    }

    if (seq->cfgSeqSize > curSeqNum)
    {
        (firstLine == GT_TRUE) ? (seqNum = 0) : (seqNum = curSeqNum);
        mvUpdateSeqOperation(seq, seqNum, seqLine);
    }
    else
    {
        if (mvReAllocateSeqOperation(seq, curSeqNum, seq->cfgSeqSize, seqLine) != GT_OK)
        {
            return GT_OUT_OF_CPU_MEM;
        }
        mvCreateSeqOperation(seq, curSeqNum, seqLine);
    }

    return GT_OK;
}

/*******************************************************************************
* mvCreateSeqOperation
*
* DESCRIPTION:
*      Create any operation
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
void mvCreateSeqOperation
(
    MV_CFG_SEQ     *seq,
    GT_U32         curSeqNum,
    MV_CFG_ELEMENT *seqLine
)
{
  MV_WRITE_OP_PARAMS wrOp;
  MV_POLLING_OP_PARAMS polOp;
  GT_U32 delay;

  if ((seq == NULL) || (seqLine == NULL))
  {
    return;
  }

  /* if operation already exists, free memory */
  if (seq->cfgSeq[curSeqNum].params != NULL)
  {
    hwsOsFreeFuncPtr(seq->cfgSeq[curSeqNum].params);
  }

  switch(seqLine->op)
  {
  case WRITE_OP:
    wrOp = *((MV_WRITE_OP_PARAMS*)(seqLine->params));
    mvCreateWriteOp(seq, curSeqNum, wrOp);
    break;
  case DELAY_OP:
    delay = *((GT_U32 *)(seqLine->params));
    mvCreateDelayOp(seq, curSeqNum, delay);
    break;
  case POLLING_OP:
    polOp = *((MV_POLLING_OP_PARAMS*)(seqLine->params));
    mvCreatePollingOp(seq, curSeqNum, polOp);
    break;
  default:
    return ;
  }

  return;
}

/*******************************************************************************
* mvAllocateSeqOperation
*
* DESCRIPTION:
*      Allocate memory for all operation
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
GT_STATUS mvAllocateAllSeqOperation
(
    MV_CFG_SEQ       *seqArray,
    MV_SEQ_OP_PARAMS *seqParam,
    GT_U32           seqLength
)
{
    GT_U32 seqId;

    for(seqId = 0; seqId < seqLength; seqId++)
    {
        if (seqParam[seqId].seqParams == NULL)
        {
            continue;
        }
        seqArray[seqId].seqId = seqId;
        seqArray[seqId].cfgSeqSize = seqParam[seqId].seqSize;
        seqArray[seqId].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * seqArray[seqId].cfgSeqSize);
        if (seqArray[seqId].cfgSeq == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }
    }

    return GT_OK;
}

/*******************************************************************************
* mvReAllocateSeqOperation
*
* DESCRIPTION:
*      ReAllocate memory for all operation (incase new operation was added)
*      Release previous memory allocation
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
GT_STATUS mvReAllocateSeqOperation
(
    MV_CFG_SEQ     *seq,
    GT_U32         curSeqNum,
    GT_U32         curSeqSize,
    MV_CFG_ELEMENT *seqLine
)
{
    MV_CFG_ELEMENT* newSeq;
    GT_U32 i;

    /* fix warning compilation */
    seqLine = seqLine;

    newSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * (curSeqSize + 1));
    if (newSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    for (i = 0; i <= curSeqSize; i++)
    {
        newSeq[i].op     = seq->cfgSeq[i].op;
        newSeq[i].params = seq->cfgSeq[i].params;
    }
    hwsOsFreeFuncPtr(seq->cfgSeq);

    seq->cfgSeqSize++;
    seq->cfgSeq = newSeq;
    seq->cfgSeq[curSeqNum].params = NULL;

    return GT_OK;
}

/*******************************************************************************
* mvInitSeqOperation
*
* DESCRIPTION:
*      Init any operation
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
void mvInitSeqOperation
(
    MV_CFG_SEQ       *seqArray,
    MV_SEQ_OP_PARAMS *seqParam,
    GT_U32           seqLength
)
{
    GT_U32 seqId, i;

    for(seqId = 0; seqId < seqLength; seqId++)
    {
        if (seqParam[seqId].seqParams == NULL)
        {
            continue;
        }

        for (i = 0; i < seqArray[seqId].cfgSeqSize; i++)
        {
            seqArray[seqId].cfgSeq[i].op = getCfgSeqOp(seqParam[seqId].seqParams[i].waitTime, seqParam[seqId].seqParams[i].numOfLoops);
            if (seqArray[seqId].cfgSeq[i].op == DELAY_OP)
            {
                /* backward compatible with Rev1 DB */
                seqParam[seqId].seqParams[i].unitId = seqParam[seqId].seqParams[i].waitTime;
            }
            seqArray[seqId].cfgSeq[i].params = &(seqParam[seqId].seqParams[i]);
        }
    }
}

/*******************************************************************************
* mvUpdateSeqOperation
*
* DESCRIPTION:
*      Update any operation
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
void mvUpdateSeqOperation
(
    MV_CFG_SEQ     *seq,
    GT_U32         curSeqNum,
    MV_CFG_ELEMENT *seqLine
)
{
    MV_OP_PARAMS *tmp, *params;

    /* update operation */
    seq->cfgSeq[curSeqNum].op = seqLine->op;
    /* update parameters */
    params = (MV_OP_PARAMS *)(seq->cfgSeq[curSeqNum].params);
    tmp    = (MV_OP_PARAMS *)(seqLine->params);

    params->mask = tmp->mask;
    params->numOfLoops = tmp->numOfLoops;
    params->operData = tmp->operData;
    params->regOffset = tmp->regOffset;
    params->unitId = tmp->unitId;
    params->waitTime = tmp->waitTime;
}

/*******************************************************************************
* getCfgSeqOp
*
* DESCRIPTION:
*       Returns the configuration operation,
*       according to the waitTime and the numOfLoops fields in params
*
* INPUTS:
*       waitTime  - the wait time, in case of a delay or polling operation
*       numOfLoops - the number of loops, in case of a polling operation
*
* RETURNS:
*       WRITE_OP    - in case there is no wait (waitTime = 0)
*       DELAY_OP    - in case there are no loops (numOfLoops = 0)
*       POLLING_OP  - otherwise
*
*******************************************************************************/
MV_EL_DB_OPERATION getCfgSeqOp(GT_U32 waitTime, GT_U32 numOfLoops)
{
    if (waitTime == 0)
    {
        return WRITE_OP;
    }
    else if (numOfLoops == 0)
    {
        return DELAY_OP;
    }
    return POLLING_OP;
}

