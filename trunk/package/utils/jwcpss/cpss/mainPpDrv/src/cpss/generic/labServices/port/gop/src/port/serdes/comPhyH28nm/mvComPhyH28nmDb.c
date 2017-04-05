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
* mvserdes28nmDb.c
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 20 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <serdes/comPhyH28nm/mvComPhyH28nmDb.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <serdes/mvHwsSerdes28nmPrvIf.h>

#include <port/private/mvHwsPortApCoCpuIf.h>
#include <common/siliconIf/siliconGpSram.h>

/******************************** enums ***************************************/

/******************************** structures **********************************/

/******************************** globals *************************************/
MV_CFG_SEQ hwsSerdes28nmSeqDb[MV_SERDES_LAST_SEQ];

MV_CFG_SEQ* hwsSerdesSeqDbPtr; /* used for sequence set/get */

static MV_HWS_COM_PHY_H_SUB_SEQ hwsSerdesSpeedToSeqMap[SERDES_LAST_SPEED] =
{   SERDES_SPEED_1_25G_SEQ,
    SERDES_SPEED_3_125G_SEQ,
    SERDES_SPEED_3_75G_SEQ,
    SERDES_SPEED_4_25G_SEQ,
    SERDES_SPEED_5G_SEQ,
    SERDES_SPEED_6_25G_SEQ,
    SERDES_SPEED_7_5G_SEQ,
    SERDES_SPEED_10_3125G_SEQ,
    SERDES_SPEED_11_25G_SEQ,
    SERDES_SPEED_11_5625G_SEQ,
    SERDES_SPEED_12_5G_SEQ,
    SERDES_SPEED_10_9375G_SEQ,
    SERDES_SPEED_12_1875G_SEQ,
    SERDES_SPEED_5_625G_SEQ
};

#ifndef CO_CPU_RUN
/* run this on host CPU only */
static MV_SPEED_CONFIG_REG_DATA hwSerdesSpeedExtConfigInit[] =
{
    /*                      1.25G   3.125G  3.75G   4.25G   5G      6.25G   7.5G   10.3125G 11.25G  11.5625G 12.5G   10.9375G  12.1875G     5.625G      */
    { 0x000,    0x7F8,    { 0x330,  0x440,  0x4C8,  0x550,  0x198,  0x5D8,  0x6E8,  0x770,  0x7F8,  0x7F8,   0x7F8,  0,        0x7F8,           0   } }, /* Setting PIN_GEN_TX, PIN_GEN_RX */
    { 0x028,    0x1F,     { 0xC,    0xC,    0xC,    0xC,    0xC,    0xC,    0xC,    0xC,    0xC,    0xC,     0xC,    0,        0xC,             0   } }, /* PIN_FREF_SEL=C (156.25MHz) */
};

/* array that hold the SERDES initialization process, each entry hold
   the MV_CONFIG_REG_DATA register and the value to write per baud rate */
static MV_SPEED_CONFIG_REG_DATA hwSerdesSpeedIntConfigInit[] =
{
    /*                  1.25G       3.125G      3.75G       4.25G       5G         6.25G      7.5G      10.3125G    11.25G      11.5625G   12.5G    10.9375G    12.1875G    5.625G      */
    { 0x018, 0xFFFF,   { 0x028F,    0x028F,     0x028F,     0x028F,     0x028F,    0x028F,    0x028F,    0x028F,    0x028F,    0x028F,    0x028F,       0,      0x028F,         0  } }, /* FFE_R=0x0, FFE_C=0xF, SQ_THRESH=0x2 */
    { 0x01C, 0xFFFF,   { 0xF247,    0xF247,     0xF247,     0xF247,     0xF147,    0xF147,    0xF047,    0xf047,    0xF047,    0xF047,    0xF047,       0,      0xF047,         0  } }, /* DFE_RES = 3.3mv */
    { 0x020, 0xFFFF,   { 0xAFC0,    0xAFC0,     0xAFC0,     0xAFC0,     0xAFC0,    0xAFC0,    0xAFC0,    0xAFC0,    0xAFC0,    0xAFC0,    0xAFC0,       0,      0xAFC0,         0  } }, /* DFE UPDAE all coefficient */
    { 0x034, 0xFFFF,   { 0x0DFE,    0x0DFE,     0x0DFE,     0x0DFE,     0x0DFE,    0x0DFE,    0x0DFE,    0x0DFE,    0x0DFE,    0x0DFE,    0x0DFE,       0,      0x0DFE,         0  } }, /* TX_AMP=31, AMP_ADJ=1 */
    { 0x038, 0xFFFF,   { 0x17C0,    0x17C9,     0x17C9,     0x17C9,     0x17C9,    0x17C9,    0x17D2,    0x17D2,    0x17D2,    0x17D2,    0x17D2,       0,      0x17D2,         0  } }, /* MUPI/F=2, rx_digclkdiv=2 */
    { 0x084, 0xFFFF,   { 0x4030,    0x4030,     0x4030,     0x4030,     0x4030,    0x4030,    0x4030,    0x4030,    0x4030,    0x4030,    0x4030,       0,      0x4030,         0  } },
    { 0x088, 0xFFFF,   { 0x1026,    0x1026,     0x1026,     0x1026,     0x1026,    0x1026,    0x1026,    0x1026,    0x1026,    0x1026,    0x1026,       0,      0x1026,         0  } }, /* set password */
    { 0x094, 0xFFFF,   { 0x1FFF,    0x1FFF,     0x1FFF,     0x1FFF,     0x1FFF,    0x1FFF,    0x1FFF,    0x1FFF,    0x1FFF,    0x1FFF,    0x1FFF,       0,      0x1FFF,         0  } },
    { 0x098, 0xFFFF,   { 0x66,      0x88,       0x99,       0xAA,       0x33,      0xBB,      0xDD,      0xEE,      0xFF,      0xFF,      0xFF,         0,      0xFF,           0  } }, /* Set Gen_RX/TX */
    { 0x104, 0xFFFF,   { 0x2208,    0x2208,     0x2208,     0x2208,     0x2208,    0x2208,    0x2208,    0x2208,    0x2208,    0x2208,    0x2208,       0,      0x2208,         0  } }, /* EMPH0 enable, EMPH_mode=2 */
    { 0x134, 0xFFFF,   { 0x0052,    0x0052,     0x0052,     0x0052,     0x0052,    0x0052,    0x0052,    0x0052,    0x0052,    0x0052,    0x0052,       0,      0x0052,         0  } }, /* RX_IMP_VTH=2, TX_IMP_VTH=2 */
    { 0x13C, 0xFBFF,   { 0xA026,    0xA026,     0xA026,     0xA026,     0xA026,    0xA026,    0xA026,    0xA026,    0xA026,    0xA026,    0xA026,       0,      0xA026,         0  } }, /* clock set    */
    { 0x140, 0xFFFF,   { 0x800,     0x800,      0x800,      0x800,      0x800,     0x800,     0x800,     0x1800,    0x1800,    0x1800,    0x1800,       0,      0x1800,         0  } }, /* clk 8T enable for 10G and up */
    { 0x154, 0xFFFF,   { 0x87,      0x87,       0x87,       0x87,       0x87,      0x87,      0x87,      0x87,      0x87,      0x87,      0x87,         0,      0x87,           0  } }, /* rxdigclk_dv_force=1  */
    { 0x16C, 0xC0,     { 0x40,      0x40,       0x40,       0x40,       0x40,      0x40,      0x40,      0x40,      0x40,      0x40,      0x40,         0,      0x40,           0  } },
    { 0x184, 0xFFFF,   { 0x1093,    0x1093,     0x1093,     0x1093,     0x1093,    0x1093,    0x1093,    0x1093,    0x1093,    0x1093,    0x1093,       0,      0x1093,         0  } }, /* DTL_FLOOP_EN=0 */
    { 0x1A8, 0xFFFF,   { 0x4000,    0x4000,     0x4000,     0x4000,     0x4000,    0x4000,    0x4000,    0x4000,    0x4000,    0x4000,    0x4000,       0,      0x4000,         0  } }, /* Force ICP=7 */
    { 0x1C8, 0xFFFF,   { 0x07fe,    0x07fe,     0x07fe,     0x07fe,     0x07fe,    0x07fe,    0x07fe,    0x07fe,    0x07fe,    0x07fe,    0x07fe,       0,      0x07fe,         0  } }, /* rxdigclk_dv_force=1*/
    { 0x39C, 0xFFFF,   { 0x0558,    0x0558,     0x0558,     0x0558,     0x0558,    0x0558,    0x0558,    0x0558,    0x0558,    0x0558,    0x0558,       0,      0x0558,         0  } },
    { 0x3BC, 0xFFFF,   { 0x4200,    0x4200,     0x4200,     0x4200,     0x4200,    0x4200,    0x4200,    0x4200,    0x4200,    0x4200,    0x4200,       0,      0x4200,         0  } },
    { 0x004, 0xFFFF,   { 0xFC8C,    0xFC8C,     0xFC8C,     0xFC8C,     0xFC8C,    0xFC8C,    0xFC8C,    0xFC8C,    0xFC8C,    0xFC8C,    0xFC8C,       0,      0xFC8C,         0  } }, /* PHY_MODE=0x4,FREF_SEL=0xC */
    { 0x370, 0x4,      { 0x0,       0x0,        0x0,        0x0,        0x0,       0x0,       0x0,       0x0,       0x0,       0x0,       0x0,          0,      0x0,            0  } }, /* PHY_MODE=0x4,FREF_SEL=0xC */
    { 0x108, 0xFFFF,   { 0xB0BA,    0xB0BA,     0xB0BA,     0xB0BA,     0xB0BA,    0xB0BA,    0xB0BA,    0xB0BA,    0xB0BA,    0xB0BA,    0xB0BA,       0,      0xB0BA,         0  } },
    { 0x10C, 0xFFFF,   { 0xE08C,    0xE08C,     0xE08C,     0xE08C,     0xE08C,    0xE08C,    0xE08C,    0xE08C,    0xE08C,    0xE08C,    0xE08C,       0,      0xE08C,         0  } },
    { 0x110, 0xFFFF,   { 0xCC8C,    0xCC8C,     0xCC8C,     0xCC8C,     0xCC8C,    0xCC8C,    0xCC8C,    0xCC8C,    0xCC8C,    0xCC8C,    0xCC8C,       0,      0xCC8C,         0  } },
    { 0x114, 0xFFFF,   { 0xA745,    0xA745,     0xA745,     0xA745,     0xA745,    0xA745,    0xA745,    0xA745,    0xA745,    0xA745,    0xA745,       0,      0xA745,         0  } },
};

GT_STATUS hwsComPhyH28nmSpeedSeqInit
(
	MV_CFG_SEQ*				  hwSerdesSeqDbArr,
	MV_SPEED_CONFIG_REG_DATA* hwSerdesSpeedExtConfig,
	GT_U32					  extConfigNum,
	MV_SPEED_CONFIG_REG_DATA* hwSerdesSpeedIntConfig,
	GT_U32					  intConfigNum
)
{
    MV_HWS_COM_PHY_H_SUB_SEQ    speedSeq;
    MV_OP_PARAMS *writeLine;
    GT_U32 i, speed;



    for (speed = 0; speed < SERDES_LAST_SPEED; speed++ )
    {
        speedSeq = hwsSerdesSpeedToSeqMap[speed];

        hwSerdesSeqDbArr[speedSeq].seqId = speedSeq;
        hwSerdesSeqDbArr[speedSeq].cfgSeqSize = extConfigNum + intConfigNum;
        hwSerdesSeqDbArr[speedSeq].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[speedSeq].cfgSeqSize);

        if (hwSerdesSeqDbArr[speedSeq].cfgSeq == NULL)
        {
            return GT_OUT_OF_CPU_MEM;
        }

        /* external serdes regs */
        for (i = 0; i < extConfigNum; i++)
        {
            writeLine = (MV_OP_PARAMS *)hwsOsMallocFuncPtr(sizeof(MV_OP_PARAMS));
            if (writeLine != NULL)
            {
                writeLine->unitId = SERDES_UNIT;
                writeLine->regOffset = hwSerdesSpeedExtConfig[i].regOffset;
                writeLine->operData = hwSerdesSpeedExtConfig[i].regData[speed];
                writeLine->mask = hwSerdesSpeedExtConfig[i].mask;
            }
            else
            {
                return GT_OUT_OF_CPU_MEM;
            }
            hwSerdesSeqDbArr[speedSeq].cfgSeq[i].op = WRITE_OP;
            hwSerdesSeqDbArr[speedSeq].cfgSeq[i].params = (void *)writeLine;
        }

        /* internal serdes regs */
        for (i = 0; i < intConfigNum; i++)
        {
            writeLine = (MV_OP_PARAMS *)hwsOsMallocFuncPtr(sizeof(MV_OP_PARAMS));
            if (writeLine != NULL)
            {
                writeLine->unitId = SERDES_PHY_UNIT;
                writeLine->regOffset = hwSerdesSpeedIntConfig[i].regOffset;
                writeLine->operData = hwSerdesSpeedIntConfig[i].regData[speed];
                writeLine->mask = hwSerdesSpeedIntConfig[i].mask;
            }
            else
            {
                return GT_OUT_OF_CPU_MEM;
            }

            hwSerdesSeqDbArr[speedSeq].cfgSeq[extConfigNum+i].op = WRITE_OP;
            hwSerdesSeqDbArr[speedSeq].cfgSeq[extConfigNum+i].params = (void *)writeLine;
        }
    }

    return GT_OK;
}


/* SERDES_PT_AFTER_PATTERN_NORMAL */
static MV_OP_PARAMS serdesPtAfterPatternNormalParams[] =
{
    {SERDES_PHY_UNIT, COM_H_28NM_PT_PATTERN_SEL_Reg,      0xE0,   0xF0,   0,  0},
    {SERDES_PHY_UNIT, COM_H_28NM_PT_PATTERN_SEL_Reg,      0x0,    0x8000, 0,  0},
    {SERDES_PHY_UNIT, COM_H_28NM_PT_PATTERN_SEL_Reg,      0x4000, 0x4000, 0,  0},
    {SERDES_PHY_UNIT, COM_H_28NM_PT_PATTERN_SEL_Reg,      0x0,    0x4000, 0,  0},
    {SERDES_PHY_UNIT, COM_H_28NM_SEL_BITS_Reg,			  0x800,  0x800,  0,  0},
    {SERDES_PHY_UNIT, COM_H_28NM_SEL_BITS_Reg,			  0x0,    0x800,  0,  0}
};

/* SERDES_PT_AFTER_PATTERN_TEST_SEQ */
static MV_OP_PARAMS serdesPtAfterPatternTestParams[] =
{
    {SERDES_PHY_UNIT, COM_H_28NM_PT_PATTERN_SEL_Reg,  0x80E0, 0x80F0,    0,     0}
};

/* SERDES_SD_RESET_SEQ Sequence init */
static MV_OP_PARAMS serdesSdResetParams[] =
{
    {SERDES_UNIT, SERDES_EXTERNAL_CONFIGURATION_1,  0x0,    0x8,    0,  0}
};

/* SERDES_SD_UNRESET_SEQ Sequence init */
static MV_OP_PARAMS serdesSdUnResetParams[] =
{
    {SERDES_UNIT, SERDES_EXTERNAL_CONFIGURATION_1,  0x8,    0x8,    0,  0}
};

/* SERDES_RF_RESET Sequence init */
static MV_OP_PARAMS serdesRfResetParams[] =
{
    {SERDES_UNIT, SERDES_EXTERNAL_CONFIGURATION_1,  0x0,    0x40,    0, 0}
};

/* SERDES_RF_UNRESET Sequence init */
static MV_OP_PARAMS serdesRfUnResetParams[] =
{
    {SERDES_UNIT, SERDES_EXTERNAL_CONFIGURATION_1,  0x40,   0x40,   0,  0}
};

/* SERDES_SYNCE_RESET_SEQ Sequence init */
static MV_OP_PARAMS serdesSynceResetParams[] =
{
    {SERDES_UNIT, SERDES_EXTERNAL_CONFIGURATION_2,  0x0,    0x8,    0,  0}
};

/* SERDES_SYNCE_UNRESET_SEQ Sequence init */
static MV_OP_PARAMS serdesSynceUnResetParams[] =
{
    {SERDES_UNIT, SERDES_EXTERNAL_CONFIGURATION_1,  0xDD00, 0xFF00, 0,  0},
    {SERDES_UNIT, SERDES_EXTERNAL_CONFIGURATION_2,  0xB,    0xB,    0,  0}
};

/* SERDES_SERDES_POWER_UP_CTRL_SEQ Sequence init */
static MV_OP_PARAMS serdesPowerUpCtrlParams[] =
{
    {SERDES_UNIT,       0x0,    0x1802, 0x1802, 0,      0  },
    {SERDES_UNIT,       0x8,    0x10,   0x10,   0,      0  },
    {SERDES_PHY_UNIT,   0x8,    0x8000, 0x8000, 0,      0  },
    {SERDES_PHY_UNIT,   0x8,    0x4000, 0x4000, 1,      100},
    {SERDES_PHY_UNIT,   0x8,    0x0,    0x8000, 0,      0  },
    {0,                 0,      0,      0,      6,      0  },
};

/* SERDES_SERDES_POWER_DOWN_CTRL_SEQ Sequence init */
static MV_OP_PARAMS serdesPowerDownCtrlParams[] =
{
    {SERDES_UNIT,   SERDES_EXTERNAL_CONFIGURATION_0,    0x0,    0x1802,     0,  0},
    {SERDES_UNIT,   SD_METAL_FIX,                       0x100,  0xFFFF,     0,  0},
    {SERDES_UNIT,   SERDES_RESERVED_REGISTER_2,         0x0,    0xFFFF,     0,  0},
};

/* SERDES_SERDES_RXINT_UP Sequence init */
static MV_OP_PARAMS serdesRxIntUpParams[] =
{
    {SERDES_PHY_UNIT,   COM_H_28NM_PHY_Isolate_Reg,     0x400,  0x400,  0,     0},
    {0,                 0,                              0,      0,      1,     0},
    {SERDES_PHY_UNIT,   COM_H_28NM_SEL_BITS_Reg,        0x40,   0x40,   1,     100},
    {SERDES_PHY_UNIT,   COM_H_28NM_PHY_Isolate_Reg,     0,      0x400,  0,     0},
};

/* SERDES_SERDES_RXINT_DOWN Sequence init */
static MV_OP_PARAMS serdesRxIntDownParams[] =
{
    {SERDES_UNIT, SERDES_EXTERNAL_CONFIGURATION_1,  0,  0x10,   0,      0},
    {SERDES_PHY_UNIT, COM_H_28NM_PHY_Isolate_Reg,   0,  0x400,  0,      0},
};

/* SERDES_SERDES_WAIT_PLL Sequence init */
static MV_OP_PARAMS serdesWaitPllParams[] =
{
    {SERDES_PHY_UNIT,   COM_H_28NM_KVCO_CAL_Reg,    0X8000, 0x8000,     0,  0},
    {SERDES_PHY_UNIT,   COM_H_28NM_KVCO_CAL_Reg,    0x4000, 0x4000,     1,  20},
    {SERDES_PHY_UNIT,   COM_H_28NM_KVCO_CAL_Reg,    0,      0x8000,     0,  0},
    {0,                 0,                          0,      0,          8,  0},
};

/* SERDES_SD_LPBK_NORMAL_SEQ */
static MV_OP_PARAMS serdesLpbkNormalParams[] =
{
    {SERDES_PHY_UNIT, COM_H_28NM_SEL_BITS_Reg,       	0,      0xF000,		0,	0},
    {SERDES_PHY_UNIT, COM_H_28NM_PT_PATTERN_SEL_Reg, 	0,      0x8000,		0,	0},
	{SERDES_PHY_UNIT, COM_H_28NM_PU_PLL_Reg,			0,  	0x800,		0,	0},
};

/* SERDES_SD_ANA_TX_2_RX_SEQ */
static MV_OP_PARAMS serdesLpbkAnaTx2RxParams[] =
{
    {SERDES_PHY_UNIT, COM_H_28NM_SEL_BITS_Reg,       0x2000, 0x2000,    0,       0},
};

/* SERDES_SD_DIG_RX_2_TX_SEQ */
static MV_OP_PARAMS serdesLpbkDigRx2TxParams[] =
{
    {SERDES_PHY_UNIT, COM_H_28NM_PU_PLL_Reg,		      0x800,  0x800,     0,   0},
    {SERDES_PHY_UNIT, COM_H_28NM_PT_PATTERN_SEL_Reg,      0xD0,   0xF0,      0,   0},
    {SERDES_PHY_UNIT, COM_H_28NM_PT_PATTERN_SEL_Reg,      0x8000, 0x8000,    0,   0},
    {SERDES_PHY_UNIT, COM_H_28NM_SEL_BITS_Reg,			  0x9000, 0x9000,    0,   0},
};

/* SERDES_SD_DIG_TX_2_RX_SEQ */
static MV_OP_PARAMS serdesLpbkDigTx2RxParams[] =
{
    {SERDES_PHY_UNIT, COM_H_28NM_SEL_BITS_Reg,    0x4000, 0x4000,    0,       0},
};

/* SERDES_TX_TRAINING_ENABLE_SEQ Sequence init */
static MV_OP_PARAMS serdesTxTrainingEnParams[] =
{
    {SERDES_PHY_UNIT, 0x188,    0x2A60, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x94,     0x1FFF, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x18,     0x28F,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x1C,     0x6047, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x20,     0xAFC0, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x38,     0xFD2,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x1A8,    0x4000, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x214,    0x2118, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x22C,    0x7470, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x330,    0xD8F,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x36C,    0x3C4,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x280,    0xC8F3, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x2BC,    0x2,    0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x2BC,    0x0,    0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x2BC,    0x1,    0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x2BC,    0x0,    0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x278,    0x21F3, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x228,    0x2B40, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x320,    0x1658, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x320,    0x16D8, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x294,    0x484,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x294,    0xFFF,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x18,     0x20F,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x330,    0xD0F,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x228,    0x2BC0, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x31C,    0x21B,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x228,    0x2FC0, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x228,    0x2F80, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x31C,    0x31B,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x214,    0x4418, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x8C,     0x7A,   0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x20C,    0x300,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x360,    0x5903, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x360,    0x1903, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x184,    0x1013, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x198,    0x1000, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x184,    0x1073, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xF0,     0x4688, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xEC,     0x3A62, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xEC,     0x3A62, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x1C,     0x6047, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x1C,     0x6048, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xF0,     0x4688, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xF0,     0x4688, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xEC,     0x3A82, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xF0,     0x4688, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x340,    0x5093, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x278,    0x79F3, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x1AC,    0x45D8, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x1AC,    0x45F8, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x27C,    0xB656, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x340,    0x5096, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x27C,    0xB796, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x340,    0x5196, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x27C,    0xBD96, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x27C,    0xBDB6, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x340,    0x51B6, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x29C,    0x3FF,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x320,    0x16D8, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x264,    0xBAD,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x284,    0x840,  0xC87C,     0,      0},
    {SERDES_PHY_UNIT, 0x32C,    0x4400, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x168,    0x5028, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x188,    0x2A60, 0xFFFF,     0,      0},
};

/* SERDES_TX_TRAINING_DISABLE_SEQ Sequence init */
static MV_OP_PARAMS serdesTxTrainingDisParams[] =
{
    {SERDES_UNIT,       0x8,    0x0,    0x20,       0,      0},
    {SERDES_UNIT,       0xC,    0x0,    0x100,      0,      0},
    {SERDES_PHY_UNIT,   0x22C,  0x7470, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT,   0x94,   0x1FFF, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT,   0x8C,   0x72,   0xFFFF,     0,      0},
    {SERDES_PHY_UNIT,   0x31C,  0,      0x200,      0,      0},
};

/* SERDES_RX_TRAINING_DISABLE_SEQ Sequence init */
static MV_OP_PARAMS serdesRxTrainingDisParams[] =
{
    {SERDES_PHY_UNIT, 0x54,     0x0,    0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x6C,     0x0,    0xFFFF,     0,      0},
    {SERDES_UNIT,     0x8,      0x0,    0x20,       0,      0},
    {SERDES_UNIT,     0xC,      0x0,    0x80,       0,      0},
    {SERDES_PHY_UNIT, 0x22C,    0x7470, 0xFFFF,     0,      0},
};

/* SERDES_RX_TRAINING_ENABLE_SEQ Sequence init */
static MV_OP_PARAMS serdesRxTrainingEnParams[] =
{
    {SERDES_PHY_UNIT, 0x188,    0x2A60, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x94,     0x1FFF, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x18,     0x468F, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x1C,     0x6047, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x20,     0xAFC0, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x40,     0xFC0,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x48,     0x1788, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x50,     0x1689, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x1A8,    0x4000, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x214,    0x2118, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x22C,    0x7470, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x330,    0xD8F,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x36C,    0x3C4,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x280,    0xCA33, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x2BC,    0x2,    0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x2BC,    0x0,    0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x2BC,    0x1,    0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x2BC,    0x0,    0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x278,    0x21F3, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x228,    0x2B40, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x320,    0x1658, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x320,    0x16D8, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x18,     0x20F,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x330,    0xD0F,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x20C,    0x300,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x184,    0x1013, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x198,    0x1000, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x184,    0x1073, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xF0,     0x4688, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xF0,     0x4688, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xEC,     0x6A62, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xEC,     0x6A62, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x1C,     0x6047, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x1C,     0x6048, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xF0,     0x4688, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xF0,     0x4648, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xEC,     0x6A82, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0xF0,     0x4688, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x340,    0x5093, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x278,    0x79F3, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x1AC,    0x45D8, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x1AC,    0x45F8, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x27C,    0xB657, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x340,    0x5097, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x27C,    0xB7D7, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x340,    0x51D7, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x27C,    0xBFD7, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x27C,    0xBFFF, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x340,    0x51FF, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x320,    0x16D4, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x168,    0x5028, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x264,    0xBAD,  0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x284,    0x4874, 0xC87C,     0,      0},
    {SERDES_PHY_UNIT, 0x32C,    0x4400, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x188,    0x2A60, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x94,     0x1FFF, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x8C,     0x72,   0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x54,     0x80E0, 0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x6C,     0xC4,   0xFFFF,     0,      0},
    {SERDES_PHY_UNIT, 0x22C,    0x7471, 0xFFFF,     0,      0},
    {SERDES_UNIT,     0xC,      0x80,   0x80,       0,      0},
};

/* SERDES_CORE_RESET_SEQ Sequence init */
static MV_OP_PARAMS serdesCoreResetParams[] =
{
    {SERDES_UNIT, SERDES_EXTERNAL_CONFIGURATION_1,  0x0,    0x20,    0,     0},
};

/* SERDES_CORE_UNRESET_SEQ Sequence init */
static MV_OP_PARAMS serdesCoreUnResetParams[] =
{
    {SERDES_UNIT, SERDES_EXTERNAL_CONFIGURATION_1,  0x20,   0x20,    0,     0},
};

/* seqId to params array mapping */
static MV_OP_PARAMS *seqIdToParams[] =
{
    serdesSdResetParams,
    serdesSdUnResetParams,
    serdesRfResetParams,
    serdesRfUnResetParams,
    serdesSynceResetParams,
    serdesSynceUnResetParams,
    serdesPowerUpCtrlParams,
    serdesPowerDownCtrlParams,
    serdesRxIntUpParams,
    serdesRxIntDownParams,
    serdesWaitPllParams,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    serdesLpbkNormalParams,
    serdesLpbkAnaTx2RxParams,
    serdesLpbkDigTx2RxParams,
    serdesLpbkDigRx2TxParams,
    serdesPtAfterPatternNormalParams,
    serdesPtAfterPatternTestParams,
    serdesRxTrainingEnParams,
    serdesRxTrainingDisParams,
    serdesTxTrainingEnParams,
    serdesTxTrainingDisParams,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    serdesCoreResetParams,
    serdesCoreUnResetParams,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

GT_STATUS hwsComPhyH28nmSeqInit(void)
{
	GT_U32 intNum = sizeof(hwSerdesSpeedIntConfigInit)/sizeof(MV_SPEED_CONFIG_REG_DATA);
	GT_U32 extNum = sizeof(hwSerdesSpeedExtConfigInit)/sizeof(MV_SPEED_CONFIG_REG_DATA);

    hwsOsMemSetFuncPtr(hwsSerdes28nmSeqDb, 0, sizeof(hwsSerdes28nmSeqDb));

    /* Init configuration sequence executer */
    mvCfgSeqExecInit();
    mvCfgSerdesSeqExecInit(hwsSerdes28nmSeqDb, (sizeof(hwsSerdes28nmSeqDb) / sizeof(MV_CFG_SEQ)));

	/* Init speed DB */
    CHECK_STATUS(hwsComPhyH28nmSpeedSeqInit(hwsSerdes28nmSeqDb, hwSerdesSpeedExtConfigInit, extNum, hwSerdesSpeedIntConfigInit, intNum ));
	
	/* Init Serdes sequences */
	CHECK_STATUS(hwsComPhyH28nmInitSeqFlow(hwsSerdes28nmSeqDb));

	/* this global parameter is used for sequence set/get */
	hwsSerdesSeqDbPtr = hwsSerdes28nmSeqDb;

	return GT_OK;
}

GT_STATUS hwsComPhyH28nmInitSeqFlow(MV_CFG_SEQ* hwSerdesSeqDbArr)
{
    GT_U32 i, seqId;
    GT_U32 sizeSEQ = MV_SERDES_LAST_SEQ;

	if ((sizeof(seqIdToParams)/sizeof(MV_OP_PARAMS*)) != sizeSEQ)
	{
		osPrintf("Error: hwsComPhyH28nmInitSeqFlow: the size of seqIdToParams is different than MV_HWS_COM_PHY_H_SUB_SEQ size!!\n");
		return GT_BAD_SIZE;
	}

    /* SERDES_SD_RESET_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_SD_RESET_SEQ].seqId = SERDES_SD_RESET_SEQ;
    hwSerdesSeqDbArr[SERDES_SD_RESET_SEQ].cfgSeqSize =  sizeof(serdesSdResetParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_SD_RESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_SD_RESET_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_SD_RESET_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SD_UNRESET_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_SD_UNRESET_SEQ].seqId = SERDES_SD_UNRESET_SEQ;
    hwSerdesSeqDbArr[SERDES_SD_UNRESET_SEQ].cfgSeqSize =  sizeof(serdesSdUnResetParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_SD_UNRESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_SD_UNRESET_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_SD_UNRESET_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_RF_RESET_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_RF_RESET_SEQ].seqId = SERDES_RF_RESET_SEQ;
    hwSerdesSeqDbArr[SERDES_RF_RESET_SEQ].cfgSeqSize =  sizeof(serdesRfResetParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_RF_RESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_RF_RESET_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_RF_RESET_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_RF_UNRESET_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_RF_UNRESET_SEQ].seqId = SERDES_RF_UNRESET_SEQ;
    hwSerdesSeqDbArr[SERDES_RF_UNRESET_SEQ].cfgSeqSize =  sizeof(serdesRfUnResetParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_RF_UNRESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_RF_UNRESET_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_RF_UNRESET_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_CORE_RESET_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_CORE_RESET_SEQ].seqId = SERDES_CORE_RESET_SEQ;
    hwSerdesSeqDbArr[SERDES_CORE_RESET_SEQ].cfgSeqSize =  sizeof(serdesCoreResetParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_CORE_RESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_CORE_RESET_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_CORE_RESET_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_CORE_UNRESET_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_CORE_UNRESET_SEQ].seqId = SERDES_CORE_UNRESET_SEQ;
    hwSerdesSeqDbArr[SERDES_CORE_UNRESET_SEQ].cfgSeqSize =  sizeof(serdesCoreUnResetParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_CORE_UNRESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_CORE_UNRESET_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_CORE_UNRESET_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SYNCE_RESET_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_SYNCE_RESET_SEQ].seqId = SERDES_SYNCE_RESET_SEQ;
    hwSerdesSeqDbArr[SERDES_SYNCE_RESET_SEQ].cfgSeqSize =  sizeof(serdesSynceResetParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_SYNCE_RESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_SYNCE_RESET_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_SYNCE_RESET_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SYNCE_UNRESET_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_SYNCE_UNRESET_SEQ].seqId = SERDES_SYNCE_UNRESET_SEQ;
    hwSerdesSeqDbArr[SERDES_SYNCE_UNRESET_SEQ].cfgSeqSize =  sizeof(serdesSynceUnResetParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_SYNCE_UNRESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_SYNCE_UNRESET_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_SYNCE_UNRESET_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SERDES_POWER_UP_CTRL_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_SERDES_POWER_UP_CTRL_SEQ].seqId = SERDES_SERDES_POWER_UP_CTRL_SEQ;
    hwSerdesSeqDbArr[SERDES_SERDES_POWER_UP_CTRL_SEQ].cfgSeqSize =  sizeof(serdesPowerUpCtrlParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_SERDES_POWER_UP_CTRL_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_SERDES_POWER_UP_CTRL_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_SERDES_POWER_UP_CTRL_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SERDES_POWER_DOWN_CTRL_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_SERDES_POWER_DOWN_CTRL_SEQ].seqId = SERDES_SERDES_POWER_DOWN_CTRL_SEQ;
    hwSerdesSeqDbArr[SERDES_SERDES_POWER_DOWN_CTRL_SEQ].cfgSeqSize =  sizeof(serdesPowerDownCtrlParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_SERDES_POWER_DOWN_CTRL_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_SERDES_POWER_DOWN_CTRL_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_SERDES_POWER_DOWN_CTRL_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SERDES_RXINT_UP_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_SERDES_RXINT_UP_SEQ].seqId = SERDES_SERDES_RXINT_UP_SEQ;
    hwSerdesSeqDbArr[SERDES_SERDES_RXINT_UP_SEQ].cfgSeqSize =  sizeof(serdesRxIntUpParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_SERDES_RXINT_UP_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_SERDES_RXINT_UP_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_SERDES_RXINT_UP_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SERDES_RXINT_DOWN_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_SERDES_RXINT_DOWN_SEQ].seqId = SERDES_SERDES_RXINT_DOWN_SEQ;
    hwSerdesSeqDbArr[SERDES_SERDES_RXINT_DOWN_SEQ].cfgSeqSize =  sizeof(serdesRxIntDownParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_SERDES_RXINT_DOWN_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_SERDES_RXINT_DOWN_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_SERDES_RXINT_DOWN_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SERDES_WAIT_PLL_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_SERDES_WAIT_PLL_SEQ].seqId = SERDES_SERDES_WAIT_PLL_SEQ;
    hwSerdesSeqDbArr[SERDES_SERDES_WAIT_PLL_SEQ].cfgSeqSize =  sizeof(serdesWaitPllParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_SERDES_WAIT_PLL_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_SERDES_WAIT_PLL_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_SERDES_WAIT_PLL_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SD_LPBK_NORMAL_SEQ */
    hwSerdesSeqDbArr[SERDES_SD_LPBK_NORMAL_SEQ].seqId = SERDES_SD_LPBK_NORMAL_SEQ;
    hwSerdesSeqDbArr[SERDES_SD_LPBK_NORMAL_SEQ].cfgSeqSize =  sizeof(serdesLpbkNormalParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_SD_LPBK_NORMAL_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_SD_LPBK_NORMAL_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_SD_LPBK_NORMAL_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SD_ANA_TX_2_RX_SEQ */
    hwSerdesSeqDbArr[SERDES_SD_ANA_TX_2_RX_SEQ].seqId = SERDES_SD_ANA_TX_2_RX_SEQ;
    hwSerdesSeqDbArr[SERDES_SD_ANA_TX_2_RX_SEQ].cfgSeqSize =  sizeof(serdesLpbkAnaTx2RxParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_SD_ANA_TX_2_RX_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_SD_ANA_TX_2_RX_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_SD_ANA_TX_2_RX_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SD_DIG_TX_2_RX_SEQ */
    hwSerdesSeqDbArr[SERDES_SD_DIG_TX_2_RX_SEQ].seqId = SERDES_SD_DIG_TX_2_RX_SEQ;
    hwSerdesSeqDbArr[SERDES_SD_DIG_TX_2_RX_SEQ].cfgSeqSize =  sizeof(serdesLpbkDigTx2RxParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_SD_DIG_TX_2_RX_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_SD_DIG_TX_2_RX_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_SD_DIG_TX_2_RX_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SD_DIG_RX_2_TX_SEQ */
    hwSerdesSeqDbArr[SERDES_SD_DIG_RX_2_TX_SEQ].seqId = SERDES_SD_DIG_RX_2_TX_SEQ;
    hwSerdesSeqDbArr[SERDES_SD_DIG_RX_2_TX_SEQ].cfgSeqSize =  sizeof(serdesLpbkDigRx2TxParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_SD_DIG_RX_2_TX_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_SD_DIG_RX_2_TX_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_SD_DIG_RX_2_TX_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_PT_AFTER_PATTERN_NORMAL_SEQ */
    hwSerdesSeqDbArr[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ].seqId = SERDES_PT_AFTER_PATTERN_NORMAL_SEQ;
    hwSerdesSeqDbArr[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ].cfgSeqSize =  sizeof(serdesPtAfterPatternNormalParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_PT_AFTER_PATTERN_TEST_SEQ */
    hwSerdesSeqDbArr[SERDES_PT_AFTER_PATTERN_TEST_SEQ].seqId = SERDES_PT_AFTER_PATTERN_TEST_SEQ;
    hwSerdesSeqDbArr[SERDES_PT_AFTER_PATTERN_TEST_SEQ].cfgSeqSize =  sizeof(serdesPtAfterPatternTestParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_PT_AFTER_PATTERN_TEST_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_PT_AFTER_PATTERN_TEST_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_PT_AFTER_PATTERN_TEST_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_RX_TRAINING_ENABLE_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_RX_TRAINING_ENABLE_SEQ].seqId = SERDES_RX_TRAINING_ENABLE_SEQ;
    hwSerdesSeqDbArr[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeqSize = sizeof(serdesRxTrainingEnParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_RX_TRAINING_DISABLE_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_RX_TRAINING_DISABLE_SEQ].seqId = SERDES_RX_TRAINING_DISABLE_SEQ;
    hwSerdesSeqDbArr[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeqSize = sizeof(serdesRxTrainingDisParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_TX_TRAINING_ENABLE_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_TX_TRAINING_ENABLE_SEQ].seqId = SERDES_TX_TRAINING_ENABLE_SEQ;
    hwSerdesSeqDbArr[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeqSize = sizeof(serdesTxTrainingEnParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_TX_TRAINING_DISABLE_SEQ Sequence init */
    hwSerdesSeqDbArr[SERDES_TX_TRAINING_DISABLE_SEQ].seqId = SERDES_TX_TRAINING_DISABLE_SEQ;
    hwSerdesSeqDbArr[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeqSize = sizeof(serdesTxTrainingDisParams)/sizeof(MV_OP_PARAMS);
    hwSerdesSeqDbArr[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwSerdesSeqDbArr[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeqSize );
    if (hwSerdesSeqDbArr[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* update op and params fields for each sequence */
    for(seqId = 0; seqId < MV_SERDES_LAST_SEQ; seqId++)
    {
        if (seqIdToParams[seqId] == NULL)
        {
            continue;
        }
        for (i = 0; i < hwSerdesSeqDbArr[seqId].cfgSeqSize; i++)
        {
            hwSerdesSeqDbArr[seqId].cfgSeq[i].op = getCfgSeqOp(seqIdToParams[seqId][i].waitTime, seqIdToParams[seqId][i].numOfLoops);
            if (hwSerdesSeqDbArr[seqId].cfgSeq[i].op == DELAY_OP)
            {
                seqIdToParams[seqId][i].operData = seqIdToParams[seqId][i].waitTime;
            }
            hwSerdesSeqDbArr[seqId].cfgSeq[i].params = &(seqIdToParams[seqId][i]);
        }
    }

    return GT_OK;
}

#else
/* on CO CPU code */
GT_STATUS hwsComPhyH28nmSeqInit(void)
{
    hwsGenSeqInit(hwsSerdes28nmSeqDb, MV_SERDES_LAST_SEQ);
    return GT_OK;
}

#endif /* CO_CPU_RUN */

#ifdef CO_CPU_RUN
void coCpuSeqRead(GT_U8 devNum, MV_HWS_COM_PHY_H_SUB_SEQ seqId, GT_U32 lineNum, MV_EL_DB_OPERATION *oper, MV_OP_PARAMS *params)
{
    MV_CFG_SEQ *seq;
    GT_U32 nextOperAddr, data;
    GT_U8 i;

    seq = &hwsSerdes28nmSeqDb[seqId];
    nextOperAddr = (GT_U32)(seq->cfgSeq);

    for (i = 0; i < lineNum; i++)
    {
        /* find an operation N lineNum */
        mvApGetSharedMem(devNum, devNum,nextOperAddr + 4, &data);
        nextOperAddr = data & 0xFFFF;
    }
    /* read operaion line from GP_SRAM; */
    coCpuSeqReadOperation(devNum,nextOperAddr,oper,params);

    return;
}
#endif


GT_STATUS hwsComPhy28nmSeqGet(MV_HWS_COM_PHY_H_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
    if (seqLine == NULL)
    {
    return GT_BAD_PTR;
    }
    if (seqType >= MV_SERDES_LAST_SEQ)
    {
    return GT_NO_SUCH;
    }

    if (hwsSerdesSeqDbPtr[seqType].cfgSeqSize <= lineNum)
    {
    return GT_NO_MORE;
    }
#ifdef CO_CPU_RUN
    {
        MV_EL_DB_OPERATION oper;

        hwsOsMemSetFuncPtr(&curSerdesOpParams, 0, sizeof(curSerdesOpParams));

        coCpuSeqRead(0, seqType, lineNum, &oper, &curSerdesOpParams);
        seqLine->op = oper;
        seqLine->params = &curSerdesOpParams;
    }
#else
    {
      *seqLine = hwsSerdesSeqDbPtr[seqType].cfgSeq[lineNum];
    }
#endif
    return GT_OK;
}

#ifndef CO_CPU_RUN
GT_STATUS hwsComPhy28nmSeqSet(GT_BOOL firstLine, MV_HWS_COM_PHY_H_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
    MV_OP_PARAMS    *params, *tmp;
    MV_CFG_ELEMENT* newSeq;
    GT_U32 i;

    firstLine = firstLine; /* avoid warnings */

    if (seqLine == NULL)
    {
        return GT_BAD_PTR;
    }
    if (seqType >= MV_SERDES_LAST_SEQ)
    {
        return GT_NO_SUCH;
    }
    if (hwsSerdesSeqDbPtr[seqType].cfgSeq == NULL)
    {
        return GT_NOT_INITIALIZED;
    }
    if (hwsSerdesSeqDbPtr[seqType].cfgSeqSize > numOfOp)
    {
        /* change line */
        hwsSerdesSeqDbPtr[seqType].cfgSeq[numOfOp].op = seqLine->op;
        params = (MV_OP_PARAMS *)hwsSerdesSeqDbPtr[seqType].cfgSeq[numOfOp].params;
        tmp = (MV_OP_PARAMS *)(seqLine->params);
        params->mask = tmp->mask;
        params->numOfLoops = tmp->numOfLoops;
        params->operData = tmp->operData;
        params->regOffset = tmp->regOffset;
        params->unitId = tmp->unitId;
        params->waitTime = tmp->waitTime;
    }
    else
    {
        /* add line */
        newSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * (hwsSerdesSeqDbPtr[seqType].cfgSeqSize+1));
        params = (MV_OP_PARAMS*)hwsOsMallocFuncPtr(sizeof(MV_OP_PARAMS));

        for (i = 0; i < hwsSerdesSeqDbPtr[seqType].cfgSeqSize; i++)
        {
            newSeq[i].op = hwsSerdesSeqDbPtr[seqType].cfgSeq[i].op;
            newSeq[i].params = hwsSerdesSeqDbPtr[seqType].cfgSeq[i].params;
        }
        newSeq[hwsSerdesSeqDbPtr[seqType].cfgSeqSize].op = seqLine->op;
        newSeq[hwsSerdesSeqDbPtr[seqType].cfgSeqSize].params = params;
        tmp = (MV_OP_PARAMS *)(seqLine->params);
        params->mask = tmp->mask;
        params->numOfLoops = tmp->numOfLoops;
        params->operData = tmp->operData;
        params->regOffset = tmp->regOffset;
        params->unitId = tmp->unitId;
        params->waitTime = tmp->waitTime;

        hwsSerdesSeqDbPtr[seqType].cfgSeqSize++;
        hwsSerdesSeqDbPtr[seqType].cfgSeq = newSeq;
    }
    return GT_OK;
}


/* array of pointers to serdes sequences in scratch memory */
/* MV_SERDES_LAST_SEQ is number of pointers */
/* functions used to init sequence in shared memory for usage by internal CPU */
GT_STATUS hwsComPhyH28nmSpeedSeqIntCpuInit(GT_U8 devNum)
{
    GT_U32 i, j;

    GT_U32 seqLine[3];
    MV_OP_PARAMS *curParams;

    GT_U32 serdesSeqPtr = CO_CPU_SERDES_SEQ_PTR;
    GT_U32 currAddr;


    currAddr = serdesSeqPtr + (MV_SERDES_LAST_SEQ * 4); /* 4 bytes to store seq size [15:0] and seq start [31:16] */
    /* copy all relevant sequences to shared memory from hwsSerdes28nmSeqDb[MV_SERDES_LAST_SEQ] */
    for (i = 0; i < MV_SERDES_LAST_SEQ; i++) {

        /* init sequence array with size and pointer*/
        seqLine[0] = (hwsSerdes28nmSeqDb[i].cfgSeqSize & 0xFFFF) + ((currAddr & 0xFFFF) << 16);
        mvApSetSharedMem(devNum, devNum, serdesSeqPtr, seqLine[0]);
        serdesSeqPtr += 4;

        for (j = 0; j < hwsSerdes28nmSeqDb[i].cfgSeqSize; j++) {

            curParams = (MV_OP_PARAMS *)hwsSerdes28nmSeqDb[i].cfgSeq[j].params;
            seqLine[0] = (hwsSerdes28nmSeqDb[i].cfgSeq[j].op & 3) +    /* [1:0] */
                         ((curParams->unitId & 0xF) << 2)         +    /* [5:2] */
                         ((curParams->regOffset & 0xFFFF) << 16);      /* [31:16] */
            seqLine[1] = ((curParams->operData & 0xFFFF) << 16) + (j+1); /* [63:32] */
            seqLine[2] = ((curParams->waitTime & 0xFF) << 23)   +
                         ((curParams->numOfLoops & 0xFF) << 16) +
                          (curParams->mask & 0xFFFF);
            mvApSetSharedMem(devNum, devNum, currAddr, seqLine[0]);
            currAddr += 4;
            mvApSetSharedMem(devNum, devNum, currAddr, seqLine[1]);
            currAddr += 4;
            mvApSetSharedMem(devNum, devNum, currAddr, seqLine[2]);
            currAddr += 4;
        }
    }

    return GT_OK;
}
#endif

#ifdef CO_CPU_RUN
GT_STATUS hwsSeqIntCpuInit(void)
{
    GT_U8 i;

    GT_U32 data;

    GT_U32 serdesSeqPtr = CO_CPU_SERDES_SEQ_PTR;

    /* copy all relevant sequences to shared memory from hwsSerdes28nmSeqDb[MV_SERDES_LAST_SEQ] */
    for (i = 0; i < MV_SERDES_LAST_SEQ; i++) {

        /* init sequence array with size and pointer*/
        mvApSetSharedMem(0, 0, serdesSeqPtr, data);
        hwsSerdes28nmSeqDb[i].cfgSeqSize = data & 0xFFFF;
        hwsSerdes28nmSeqDb[i].cfgSeq = (MV_CFG_ELEMENT  *)(data >> 16);
        serdesSeqPtr += 4;
    }

    return GT_OK;
}
#endif

