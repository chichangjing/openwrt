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
* mvHwsMMPcsDb.c
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 5 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <pcs/mmPcs/mvHwsMMPcs28nmDb.h>

#define MMPCS_LANE2_RESET_SEQ_PARAMS_MAC  \
    {MMPCS_UNIT, PCS_RESET_REG,                         0x800,  0xFFF7, 0, 0}, \
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x800,  0xFFF7, 0, 0}

#define MMPCS_LANE4_RESET_SEQ_PARAMS_MAC  \
    {MMPCS_UNIT, PCS_RESET_REG,                         0x800,  0xFFF7, 0, 0}, \
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x800,  0xFFF7, 0, 0}, \
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,                0x800,  0xFFF7, 0, 0}, \
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,                0x800,  0xFFF7, 0, 0}

#define MMPCS_FABRIC_RESET_LANE2_SEQ_PARAMS_MAC  \
    {MMPCS_UNIT, PCS_RESET_REG,                         0x800,  0xFFFF, 0, 0}, \
    {MMPCS_UNIT, PCS_RESET_REG + (0x1000 * 21),         0x800,  0xFFFF, 0, 0}

MV_CFG_SEQ hwsMmPsc28nmSeqDb[MV_MM_PCS_LAST_SEQ];

static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_PARAMS mmpcsLane1ModeSeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                 0x3820, 0xFBFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                 0xA0,   0xFFFF, 0, 0}
};

static MV_OP_PARAMS mmpcsLane2ModeSeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                 0x3920, 0xFBFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                 0xA0,   0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x1000,        0x1A0,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS mmpcsLane4ModeSeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                 0x3A20, 0xFBFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                 0xA0,   0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x1000,        0x1A0,  0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x2000,        0x2A0,  0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + 0x3000,        0x3A0,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS mmpcsLane1ResetSeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                         0x840,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS mmpcsLane2ResetSeqParams[] = {
    MMPCS_LANE2_RESET_SEQ_PARAMS_MAC
};

static MV_OP_PARAMS mmpcsLane4ResetSeqParams[] = {
    MMPCS_LANE4_RESET_SEQ_PARAMS_MAC
};

static MV_OP_PARAMS mmpcsLane2PowerDownSeqParams[] = {
    MMPCS_LANE2_RESET_SEQ_PARAMS_MAC,
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                 0x0000, 0x0300, 0, 0}
};

static MV_OP_PARAMS mmpcsLane4PowerDownSeqParams[] = {
    MMPCS_LANE4_RESET_SEQ_PARAMS_MAC,
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                 0x0000, 0x0300, 0, 0}
};

static MV_OP_PARAMS mmpcsLane1UnResetSeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                         0x880,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x080,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x880,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x881,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x885,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x887,  0xFFF7, 0, 0}
};

static MV_OP_PARAMS mmpcsLane2UnResetSeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                         0x840,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x840,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x40,   0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x40,   0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x840,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x840,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x841,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x841,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x845,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x845,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x847,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x847,  0xFFF7, 0, 0}
};

static MV_OP_PARAMS mmpcsLane4UnResetSeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                         0x820,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x820,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,                0x820,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,                0x820,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x20,   0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x20,   0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,                0x20,   0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,                0x20,   0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x820,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x820,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,                0x820,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,                0x820,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x821,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x821,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,                0x821,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,                0x821,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x825,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x825,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,                0x825,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,                0x825,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x827,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x827,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,                0x827,  0xFFF7, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,                0x827,  0xFFF7, 0, 0}
};

/* GOP Rev 3 (BobK) */
/* ================ */
static MV_OP_PARAMS mmpcsGopRev3Lane1UnResetSeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                         0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x40,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x841,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x845,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x847,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS mmpcsGopRev3Lane2UnResetSeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                         0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x20,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x20,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x820,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x821,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x821,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x825,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x825,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x827,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x827,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS mmpcsGopRev3Lane4UnResetSeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                         0x810,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x810,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,                0x810,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,                0x810,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x10,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x10,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,                0x10,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,                0x10,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x810,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x810,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,                0x810,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,                0x810,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x811,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x811,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,                0x811,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,                0x811,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x815,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x815,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,                0x815,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,                0x815,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x817,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x1000,                0x817,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x2000,                0x817,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + 0x3000,                0x817,  0xFFFF, 0, 0}
};

/* AlleyCat3 Port 29 only */
/* ====================== */
static MV_OP_PARAMS mmpcsFabricModeLane2SeqParams[] = {
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                 0x3920, 0xFBFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION,                 0xA0,   0xFFFF, 0, 0},
    {MMPCS_UNIT, CHANNEL_CONFIGURATION + (0x1000 * 21), 0x1A0,  0xFFFF, 0, 0}
};

static MV_OP_PARAMS mmpcsFabricResetLane2SeqParams[] = {
    MMPCS_FABRIC_RESET_LANE2_SEQ_PARAMS_MAC
};

static MV_OP_PARAMS mmpcsFabricPowerDownLane2SeqParams[] = {
    MMPCS_FABRIC_RESET_LANE2_SEQ_PARAMS_MAC,
    {MMPCS_UNIT, PCS40G_COMMON_CONTROL,                 0x0000, 0x0300, 0, 0}
};

static MV_OP_PARAMS mmpcsFabricUnResetLane2SeqParams[] = {
    {MMPCS_UNIT, PCS_RESET_REG,                         0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + (0x1000 * 21),         0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x40,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + (0x1000 * 21),         0x40,   0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + (0x1000 * 21),         0x840,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x841,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + (0x1000 * 21),         0x841,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x845,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + (0x1000 * 21),         0x845,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG,                         0x847,  0xFFFF, 0, 0},
    {MMPCS_UNIT, PCS_RESET_REG + (0x1000 * 21),         0x847,  0xFFFF, 0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
    {NULL,                             MV_SEQ_SIZE(0)},                                /* MMPCS_RESET_SEQ                 */
    {NULL,                             MV_SEQ_SIZE(0)},                                /* MMPCS_UNRESET_SEQ               */
    {mmpcsLane1ModeSeqParams,          MV_SEQ_SIZE(mmpcsLane1ModeSeqParams)},          /* MMPCS_MODE_1_LANE_SEQ           */
    {mmpcsLane2ModeSeqParams,          MV_SEQ_SIZE(mmpcsLane2ModeSeqParams)},          /* MMPCS_MODE_2_LANE_SEQ           */
    {mmpcsLane4ModeSeqParams,          MV_SEQ_SIZE(mmpcsLane4ModeSeqParams)},          /* MMPCS_MODE_4_LANE_SEQ           */
    {NULL,                             MV_SEQ_SIZE(0)},                                /* MMPCS_LPBK_NORMAL_SEQ           */
    {NULL,                             MV_SEQ_SIZE(0)},                                /* MMPCS_LPBK_RX2TX_SEQ            */
    {NULL,                             MV_SEQ_SIZE(0)},                                /* MMPCS_LPBK_TX2RX_SEQ            */
    {NULL,                             MV_SEQ_SIZE(0)},                                /* MMPCS_GEN_NORMAL_SEQ            */
    {NULL,                             MV_SEQ_SIZE(0)},                                /* MMPCS_GEN_KRPAT_SEQ             */
    {NULL,                             MV_SEQ_SIZE(0)},                                /* MMPCS_FABRIC_MODE_1_LANE_SEQ    */
    {mmpcsFabricModeLane2SeqParams,    MV_SEQ_SIZE(mmpcsFabricModeLane2SeqParams)},    /* MMPCS_FABRIC_MODE_2_LANE_SEQ    */
    {NULL,                             MV_SEQ_SIZE(0)},                                /* MMPCS_FABRIC_MODE_4_LANE_SEQ    */
    {mmpcsLane1ResetSeqParams,         MV_SEQ_SIZE(mmpcsLane1ResetSeqParams)},         /* MMPCS_RESET_1_LANE_SEQ          */
    {mmpcsLane2ResetSeqParams,         MV_SEQ_SIZE(mmpcsLane2ResetSeqParams)},         /* MMPCS_RESET_2_LANE_SEQ          */
    {mmpcsLane4ResetSeqParams,         MV_SEQ_SIZE(mmpcsLane4ResetSeqParams)},         /* MMPCS_RESET_4_LANE_SEQ          */
    {mmpcsLane1UnResetSeqParams,       MV_SEQ_SIZE(mmpcsLane1UnResetSeqParams)},       /* MMPCS_UNRESET_1_LANE_SEQ        */
    {mmpcsLane2UnResetSeqParams,       MV_SEQ_SIZE(mmpcsLane2UnResetSeqParams)},       /* MMPCS_UNRESET_2_LANE_SEQ        */
    {mmpcsLane4UnResetSeqParams,       MV_SEQ_SIZE(mmpcsLane4UnResetSeqParams)},       /* MMPCS_UNRESET_4_LANE_SEQ        */
    {mmpcsFabricResetLane2SeqParams,   MV_SEQ_SIZE(mmpcsFabricResetLane2SeqParams)},   /* MMPCS_FABRIC_RESET_2_LANE_SEQ   */
    {NULL,                             MV_SEQ_SIZE(0)},                                /* MMPCS_FABRIC_RESET_4_LANE_SEQ   */
    {mmpcsFabricUnResetLane2SeqParams, MV_SEQ_SIZE(mmpcsFabricUnResetLane2SeqParams)}, /* MMPCS_FABRIC_UNRESET_2_LANE_SEQ */
    {NULL,                             MV_SEQ_SIZE(0)},                                /* MMPCS_FABRIC_UNRESET_4_LANE_SEQ */

    {mmpcsGopRev3Lane1UnResetSeqParams, MV_SEQ_SIZE(mmpcsGopRev3Lane1UnResetSeqParams)}, /* MMPCS_GOP_REV3_UNRESET_1_LANE_SEQ */
    {mmpcsGopRev3Lane2UnResetSeqParams, MV_SEQ_SIZE(mmpcsGopRev3Lane2UnResetSeqParams)}, /* MMPCS_GOP_REV3_UNRESET_2_LANE_SEQ */
    {mmpcsGopRev3Lane4UnResetSeqParams, MV_SEQ_SIZE(mmpcsGopRev3Lane4UnResetSeqParams)}, /* MMPCS_GOP_REV3_UNRESET_4_LANE_SEQ */
    {mmpcsLane2PowerDownSeqParams,       MV_SEQ_SIZE(mmpcsLane2PowerDownSeqParams)},      /* MMPCS_POWER_DOWN_2_LANE_SEQ        */
    {mmpcsLane4PowerDownSeqParams,       MV_SEQ_SIZE(mmpcsLane4PowerDownSeqParams)},      /* MMPCS_POWER_DOWN_4_LANE_SEQ        */
    {mmpcsFabricPowerDownLane2SeqParams, MV_SEQ_SIZE(mmpcsFabricPowerDownLane2SeqParams)} /* MMPCS_FABRIC_POWER_DOWN_2_LANE_SEQ */
};

#ifndef CO_CPU_RUN

GT_STATUS hwsMMPsc28nmSeqInit(void)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* MM PCS Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsMmPsc28nmSeqDb[0], seqIdToParams, MV_MM_PCS_LAST_SEQ));

    /* MM PCS Sequences operation & parameters update */
    mvInitSeqOperation(&hwsMmPsc28nmSeqDb[0], seqIdToParams, MV_MM_PCS_LAST_SEQ);

    return GT_OK;
}

void hwsMMPcs28nmSeqFree(void)
{
  GT_U32 i;

  if (countInitCalls == 0)
  {
      return;
  }
  countInitCalls--;
  for (i = 0; (countInitCalls == 0) && (i < MV_MM_PCS_LAST_SEQ); i++)
  {
      if (hwsMmPsc28nmSeqDb[i].cfgSeq != NULL)
      {
          hwsOsFreeFuncPtr(hwsMmPsc28nmSeqDb[i].cfgSeq);
      }
  }
}

GT_STATUS hwsMMPcs28nmSeqGet(MV_HWS_MM_PCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if (seqType >= MV_MM_PCS_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  if (hwsMmPsc28nmSeqDb[seqType].cfgSeq == NULL)
  {
      /* check prev version */
      return hwsMMPcsSeqGet(seqType, seqLine, lineNum);
  }

  return mvGetSeqOperation(&hwsMmPsc28nmSeqDb[seqType], seqLine, lineNum);
}

GT_STATUS hwsMMPcs28nmSeqSet(GT_BOOL firstLine, MV_HWS_MM_PCS_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  if (seqType >= MV_MM_PCS_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  if (hwsMmPsc28nmSeqDb[seqType].cfgSeq == NULL)
  {
      /* check prev version */
      return hwsMMPcsSeqSet(firstLine, seqType, seqLine, numOfOp);
  }

  return mvSetSeqOperation(firstLine, &hwsMmPsc28nmSeqDb[seqType], numOfOp, seqLine);
}
#else
GT_STATUS hwsMMPsc28nmSeqInit(void)
{
    hwsGenSeqInit(hwsMmPsc28nmSeqDb, MV_MM_PCS_LAST_SEQ);
    return GT_OK;
}
#endif

