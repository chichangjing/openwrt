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
* mvCfgSeqDbIf.h
*
* DESCRIPTION:
*       Create configuration sequence DB APIs
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/

#ifndef __mvCfgSeqDbIf_H
#define __mvCfgSeqDbIf_H

#include <common/configElementDb/mvCfgElementDb.h>

#ifndef CO_CPU_RUN
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
);

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
);

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
);

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
);

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
);

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
    MV_SEQ_OP_PARAMS *seqParams,
    GT_U32           seqLength
);

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
);

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
    MV_CFG_SEQ            *seqArray,
    MV_SEQ_OP_PARAMS      *seqParam,
    GT_U32                seqLength
);

/*******************************************************************************
* mvReAllocateSeqOperation
*
* DESCRIPTION:
*      ReAllocate memory for any operation
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
);

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
);

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
MV_EL_DB_OPERATION getCfgSeqOp
(
    GT_U32 waitTime,
    GT_U32 numOfLoops
);
#endif /* CO_CPU_RUN */
#endif /* __mvCfgSeqDbIf_H */

