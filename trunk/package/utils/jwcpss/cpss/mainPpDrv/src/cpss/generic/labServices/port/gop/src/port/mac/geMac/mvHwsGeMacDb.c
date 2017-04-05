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
* mvHwsGeMacDb.c
*
* DESCRIPTION:
*       
*
* FILE REVISION NUMBER:
*       $Revision: 21 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <mac/geMac/mvHwsGeMacDb.h>

MV_CFG_SEQ hwsGeMacSeqDb[MV_MAC_LAST_SEQ];

static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS unresetSeqParams[] = {
    {GEMAC_UNIT, GIG_PORT_MAC_CONTROL_REGISTER2,            0x0,      (1 << 6),  0, 0}
};

static MV_OP_PARAMS resetSeqParams[] = {
    {GEMAC_UNIT, GIG_PORT_MAC_CONTROL_REGISTER2,           (1 << 6),  (1 << 6),  0, 0}
};

static MV_OP_PARAMS xModeSeqParams[] = {
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER3,           0x0,      (7 << 13), 0, 0},
    {GEMAC_UNIT,  GIG_PORT_MAC_CONTROL_REGISTER0,          (1 << 1),  (1 << 1),  0, 0},
    {GEMAC_UNIT,  GIG_Port_Auto_Negotiation_Configuration,  0x9268,    0xFFFF,   0, 0},
    {GEMAC_UNIT,  GIG_PORT_MAC_CONTROL_REGISTER1,           0x0,      (1 << 1),  0, 0},
    {GEMAC_UNIT,  MSM_PORT_MAC_CONTROL_REGISTER4,          (1 << 12), (1 << 12), 0, 0}
};

static MV_OP_PARAMS sgModeSeqParams[] = {
    {XLGMAC_UNIT, MSM_PORT_MAC_CONTROL_REGISTER3,           0x0,      (7 << 13), 0, 0},
    {GEMAC_UNIT,  GIG_PORT_MAC_CONTROL_REGISTER0,           0x0,      (1 << 1),  0, 0},
    {GEMAC_UNIT,  GIG_Port_Auto_Negotiation_Configuration,  0xB0EC,    0xFFFF,   0, 0},  /* Disable the AN-FlowControl, bit #11=0 */
    {GEMAC_UNIT,  GIG_PORT_MAC_CONTROL_REGISTER1,           0x0,      (1 << 1),  0, 0}
};

static MV_OP_PARAMS lbNormalSeqParams[] = {
    {GEMAC_UNIT,  GIG_PORT_MAC_CONTROL_REGISTER1,           0x0,      (1 << 5),  0, 0}
};

static MV_OP_PARAMS lbTx2RxSeqParams[] = {
    {GEMAC_UNIT,  GIG_PORT_MAC_CONTROL_REGISTER1,          (1 << 5),  (1 << 5),  0, 0}
};

static MV_OP_PARAMS netxModeSeqParams[] = {
    {GEMAC_UNIT,  GIG_PORT_MAC_CONTROL_REGISTER0,          (1 << 1),  (1 << 1),  0, 0},
    {GEMAC_UNIT,  GIG_Port_Auto_Negotiation_Configuration,  0x9268,    0xFFFF,   0, 0},
    {GEMAC_UNIT,  GIG_PORT_MAC_CONTROL_REGISTER1,           0x0,      (1 << 1),  0, 0},
    {GEMAC_UNIT,  MSM_PORT_MAC_CONTROL_REGISTER4,          (1 << 12), (1 << 12), 0, 0}
};

static MV_OP_PARAMS netsgModeSeqParams[] = {
    {GEMAC_UNIT,  GIG_PORT_MAC_CONTROL_REGISTER0,           0x0,      (1 << 1),  0, 0},
    {GEMAC_UNIT,  GIG_Port_Auto_Negotiation_Configuration,  0xB0EC,    0xFFFF,   0, 0},  /* Disable the AN-FlowControl, bit #11=0 */
    {GEMAC_UNIT,  GIG_PORT_MAC_CONTROL_REGISTER1,           0x0,      (1 << 1),  0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
    {unresetSeqParams,   MV_SEQ_SIZE(unresetSeqParams)},	/* GEMAC_UNRESET_SEQ     */
    {resetSeqParams,     MV_SEQ_SIZE(resetSeqParams)},		/* GEMAC_RESET_SEQ       */
    {xModeSeqParams,     MV_SEQ_SIZE(xModeSeqParams)},		/* GEMAC_X_MODE_SEQ      */
    {sgModeSeqParams,    MV_SEQ_SIZE(sgModeSeqParams)},		/* GEMAC_SG_MODE_SEQ     */
    {lbNormalSeqParams,  MV_SEQ_SIZE(lbNormalSeqParams)},	/* GEMAC_LPBK_NORMAL_SEQ */
    {NULL,               MV_SEQ_SIZE(0)},					/* GEMAC_LPBK_RX2TX_SEQ  */
    {lbTx2RxSeqParams,   MV_SEQ_SIZE(lbTx2RxSeqParams)},	/* GEMAC_LPBK_TX2RX_SEQ  */
    {netxModeSeqParams,  MV_SEQ_SIZE(netxModeSeqParams)},	/* GEMAC_NET_X_MODE_SEQ  */
    {netsgModeSeqParams, MV_SEQ_SIZE(netsgModeSeqParams)}	/* GEMAC_NET_SG_MODE_SEQ */
};

#ifndef CO_CPU_RUN

GT_STATUS hwsGeMacSeqInit(GT_U8 devNum)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* GE MAC Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsGeMacSeqDb[0], seqIdToParams, MV_MAC_LAST_SEQ));

    /* GE MAC Sequences update */
    if (HWS_DEV_GOP_REV(devNum) < GOP_28NM_REV2)
    {
        hwsGeMacSeqDb[GEMAC_X_MODE_SEQ].cfgSeqSize = 4;
        hwsGeMacSeqDb[GEMAC_NET_X_MODE_SEQ].cfgSeqSize = 3;
    }

    /* GE MAC Sequences operation & parameters update */
    mvInitSeqOperation(&hwsGeMacSeqDb[0], seqIdToParams, MV_MAC_LAST_SEQ);

    return GT_OK;
}

#endif /* CO_CPU_RUN */

GT_STATUS hwsGeMacSgSeqInit(GT_U8 devNum)
{
    return hwsGeMacSeqInit(devNum);
}

#ifndef CO_CPU_RUN
void hwsGeMacSeqFree(void)
{
    GT_U32 i;

    if (countInitCalls == 0)
    {
        return;
    }
    countInitCalls--;
    for (i = 0; (countInitCalls == 0) && (i < MV_MAC_LAST_SEQ); i++)
    {
        if (hwsGeMacSeqDb[i].cfgSeq != NULL)
        {
            hwsOsFreeFuncPtr(hwsGeMacSeqDb[i].cfgSeq);
        }
    }
}

GT_STATUS hwsGeMacSeqGet(MV_HWS_GE_MAC_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
    if ((seqType >= MV_MAC_LAST_SEQ) ||
        (hwsGeMacSeqDb[seqType].cfgSeq == NULL))
    {
        return GT_NO_SUCH;
    }

    return mvGetSeqOperation(&hwsGeMacSeqDb[seqType], seqLine, lineNum);
}

GT_STATUS hwsGeMacSeqSet(GT_BOOL firstLine, MV_HWS_GE_MAC_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
    if (seqType < MV_MAC_LAST_SEQ)
    {
        return mvSetSeqOperation(firstLine, &hwsGeMacSeqDb[seqType], numOfOp, seqLine);
    }

    return GT_NO_SUCH;
}
#else
/* on CO CPU code */
GT_STATUS hwsGeMacSeqInit(GT_U8 devNum)
{
    hwsGenSeqInit(hwsGeMacSeqDb, MV_MAC_LAST_SEQ);
    return GT_OK;
}
#endif

