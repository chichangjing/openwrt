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
* mvhwsComPhyHRev2Db.c
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 59 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <port/serdes/comPhyHRev2/mvComPhyHRev2Db.h>
#include <serdes/mvHwsSerdesPrvIf.h>

MV_CFG_SEQ hwsSerdesRev2SeqDb[MV_SERDES_LAST_SEQ];
static GT_U32 serdesSpeedSeqSize;
static MV_OP_PARAMS *speed_1_25G_SeqParams;
static MV_OP_PARAMS *speed_3_125G_SeqParams;
static MV_OP_PARAMS *speed_3_3G_SeqParams;
static MV_OP_PARAMS *speed_3_75G_SeqParams;
static MV_OP_PARAMS *speed_4_25G_SeqParams;
static MV_OP_PARAMS *speed_5G_SeqParams;
static MV_OP_PARAMS *speed_6_25G_SeqParams;
static MV_OP_PARAMS *speed_7_5G_SeqParams;
static MV_OP_PARAMS *speed_10_3125G_SeqParams;
static MV_OP_PARAMS *speed_11_5625G_SeqParams;
static MV_OP_PARAMS *speed_12_5G_SeqParams;
static MV_OP_PARAMS *speed_12_1875G_SeqParams;

static GT_U32 countInitCalls = 0; /* count registered instances */

static MV_OP_SERDES_PARAMS hwsSerdesSpeedSeqParams[] =
{
/*                                            1.25G    3.125G   3.3G     3.75G    4.25G    5G       6.25G    7.5G     10_3125G 11.56G   12.5G    10_9375G 12_1875G 5.625G */
    { SERDES_UNIT, 0x000,                   { 0x330,   0x440,   0x440,   0x4C8,   0x550,   0x198,   0x5D8,   0x6E8,   0x770,   0x7F8,   0x7F8,   0x0000,  0x7F8,   0x0000},  0x7F8,  0, 0 }, /* Setting PIN_GEN_TX, PIN_GEN_RX */
    { SERDES_UNIT, 0x028,                   { 0xC,     0xC,     0xC,     0xC,     0xC,     0xC,     0xC,     0xC,     0xC,     0xC,     0xC,     0x0000,  0xC,     0x0000},  0x1F,   0, 0 }, /* PIN_FREF_SEL=C (156.25MHz) */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x004, { 0xFC8C,  0xFC8C,  0xFC8C,  0xFC8C,  0xFC8C,  0xFC8C,  0xFC8C,  0xFC8C,  0xFC8C,  0xFC8C,  0xFC8C,  0x0000,  0xFC8C,  0x0000},  0xFFFF, 0, 0 }, /* PHY_MODE=0x4,FREF_SEL=0xC */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x018, { 0x06BF,  0x06BF,  0x06BF,  0x06BF,  0x06BF,  0x06BF,  0x06BF,  0x06BF,  0x06BF,  0x06BF,  0x06BF,  0x0000,  0x06BF,  0x0000},  0xFFFF, 0, 0 }, /* SQ_THERSH=6, FFE_R=3, FFE_C=0xF */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x01C, { 0xF247,  0x7047,  0x7047,  0x7047,  0x7047,  0x7047,  0x7047,  0x7047,  0x7047,  0x7047,  0x7047,  0x0000,  0x7047,  0x0000},  0xFFFF, 0, 0 }, /* DFE_RES=3mV */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x020, { 0xAFC0,  0xAFC0,  0xAFC0,  0xAFC0,  0xAFC0,  0xAFC0,  0xAFC0,  0xAFC0,  0xAFC0,  0xAFC0,  0xAFC0,  0x0000,  0xAFC0,  0x0000},  0xFFFF, 0, 0 }, /* DFE update enable */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x034, { 0x79F1,  0xBA79,  0xBA79,  0xBA79,  0x0DFF,  0x0DFF,  0x0DFF,  0x0DFF,  0x0DFF,  0x0DFF,  0x0DFF,  0x0000,  0x0DFF,  0x0000},  0xFFFF, 0, 0 }, /* G1_TX_AMP=0x1F, AMP_SHFT=1, AMP_ADJ=1, EMPH1 = 0xB */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x038, { 0x0FC9,  0x0FD2,  0x0FD2,  0x0FD2,  0x0FD2,  0x0FD2,  0x0FD2,  0x0FD2,  0x17D2,  0x17D2,  0x17D2,  0x0000,  0x17D2,  0x0000},  0xFFFF, 0, 0 }, /* G1_RX_DFE_EN=1, G1_SELMUPF/I=2 */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x084, { 0x30,    0x30,    0x30,    0x30,    0x30,    0x30,    0x30,    0x30,    0x30,    0x30,    0x30,    0x0000,  0x30,    0x0000},  0xFFFF, 0, 0 }, /* PT_PHYREADY_FORCE=1, PT_TESTMODE=2 */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x088, { 0x1026,  0x1026,  0x1026,  0x1026,  0x1026,  0x1026,  0x1026,  0x1026,  0x1026,  0x1026,  0x1026,  0x0000,  0x1026,  0x0000},  0xFFFF, 0, 0 }, /* Password un protect */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x098, { 0x1866,  0x1088,  0x1088,  0x1099,  0x10AA,  0x1033,  0x10BB,  0x10DD,  0x10EE,  0x10FF,  0x10FF,  0x0000,  0x10FF,  0x0000},  0xFFFF, 0, 0 }, /* Normal mode, seting TX/RX GEN Table */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x0F4, { 0x52,    0x52,    0x52,    0x52,    0x52,    0x52,    0x52,    0x52,    0x52,    0x52,    0x52,    0x0000,  0x52,    0x0000},  0xFFFF, 0, 0 }, /* G1_EMPH0=7 */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x104, { 0x2600,  0x2600,  0x2600,  0x2600,  0x2600,  0x2600,  0x2600,  0x2600,  0x2600,  0x2600,  0x2600,  0x0000,  0x2600,  0x0000},  0xFFFF, 0, 0 }, /* tx/rx_impcalth=1 (94?) */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x108, { 0x1DF,   0x81DF,  0x81DF,  0x81DF,  0xb0b4,  0x8039,  0x8039,  0xb0b4,  0xb0b6,  0xb0bf,  0xb0bf,  0x0000,  0xb0b6,  0x0000},  0xFFFF, 0, 0 }, /* cal_vdd_continuas_mode=1, rxdig=0x16 for 10.3G and 12.1875G, rxdig=0x14 for lower */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x10C, { 0x8CC0,  0x0CF4,  0x0CF4,  0x0CF4,  0x2494,  0x24B8,  0x24B8,  0x2494,  0x24B4,  0x24B4,  0x24B4,  0x0000,  0x24B4,  0x0000},  0xFFFF, 0, 0 }, /* VDD calibration, only for 10.3G and 12.5G txclk=20 */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x110, { 0xF0C0,  0xF0DA,  0xF0DA,  0xF0DA,  0x2094,  0xF0DA,  0xF0DA,  0x2094,  0x2094,  0x2094,  0x2094,  0x0000,  0x2094,  0x0000},  0xFFFF, 0, 0 }, /* VDD calibration */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x114, { 0x35F4,  0x35F0,  0x35F0,  0x35F0,  0x03a4,  0x35F0,  0x35F0,  0x03a4,  0x0364,  0x03f4,  0x03f4,  0x0000,  0x0364,  0x0000},  0xFFFF, 0, 0 }, /* VDD calibration, rxsampl=0x1f for 10.3G and 12.1875G, rxsampl=0x16 for lower */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x128, { 0x600,   0x600,   0x600,   0x600,   0x600,   0x600,   0x600,   0x600,   0x600,   0x600,   0x600,   0x0000,  0x600,   0x0000},  0xFFFF, 0, 0 }, /* ck100k_in_pll_freq_sel=2 */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x134, { 0x3040,  0x3050,  0x3060,  0x3060,  0x3070,  0x3050,  0x3050,  0x3070,  0x3050,  0x3050,  0x3050,  0x0000,  0x3050,  0x0000},  0xFFFF, 0, 0 }, /* sellv_rxintp=0, vregrxtx=3,selvthvco_cont=1 */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x138, { 0x105,   0x105,   0x105,   0x105,   0x105,   0x105,   0x105,   0x105,   0x105,   0x105,   0x105,   0x0000,  0x105,   0x0000},  0xFFFF, 0, 0 }, /* vregvco=1 */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x13C, { 0xE026,  0xA026,  0xA026,  0xA026,  0xA026,  0xA026,  0xA026,  0xA026,  0xA026,  0xA026,  0xA026,  0x0000,  0xA026,  0x0000},  0xFBFF, 0, 0 }, /* ICP=6 */

    { SERDES_UNIT, SERDES_PHY_REGS + 0x140, { 0x800,   0x800,   0x800,   0x800,   0x800,   0x800,   0x800,   0x800,   0x1800,  0x1800,  0x1800,  0x0000,  0x1800,  0x0000},  0xFFFF, 0, 0 }, /* CLK_8T=1 in 10.3 and 12.5G */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x14C, { 0x04B4,  0x04B4,  0x04B4,  0x04B4,  0x04B4,  0x04B4,  0x04B4,  0x04B4,  0x04B4,  0x04B4,  0x04B4,  0x0000,  0x04B4,  0x0000},  0xFFFF, 0, 0 }, /* force fbdiv for 3.33G */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x150, { 0x8001,  0x8001,  0x8001,  0x8001,  0x8001,  0x8001,  0x8001,  0x8001,  0x8001,  0x8001,  0x8001,  0x0000,  0x8001,  0x0000},  0xFFFF, 0, 0 }, /* force fbdiv for 3.33G */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x154, { 0x0007,  0x0007,  0x0007,  0x0007,  0x0007,  0x0007,  0x0007,  0x0007,  0x0087,  0x0087,  0x0087,  0x0000,  0x0087,  0x0000},  0xFFFF, 0, 0 }, /* force rx dig clock divider */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x160, { 0x0100,  0x010A,  0x010A,  0x010A,  0x010A,  0x010A,  0x010A,  0x010A,  0x010A,  0x010A,  0x010A,  0x0000,  0x010A,  0x0000},  0xFFFF, 0, 0 }, /* force refdiv for 3.33G */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x170, { 0x722C,  0x722C,  0x722C,  0x722C,  0x722C,  0x722C,  0x722C,  0x722C,  0x722C,  0x722C,  0x722C,  0x0000,  0x722C,  0x0000},  0xFFFF, 0, 0 }, /* update process calibration threshold */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x1D0, { 0xD,     0xD,     0xD,     0xD,     0xD,     0xD,     0xD,     0xD,     0xD,     0xD,     0xD,     0x0000,  0xD,     0x0000},  0xFFFF, 0, 0 }, /* increase current to 300 uA */

    { SERDES_UNIT, SERDES_PHY_REGS + 0x184, { 0x1293,  0x1293,  0x1293,  0x1293,  0x1293,  0x1293,  0x1293,  0x1293,  0x1293,  0x1293,  0x1293,  0x0000,  0x1293,  0x0000},  0xFFFF, 0, 0 }, /* DTL_FLOOP_EN=0 */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x168, { 0xE028,  0xD428,  0xD428,  0xD428,  0xD428,  0x5428,  0x5428,  0x5428,  0x5428,  0x5428,  0x5428,  0x0000,  0xE028,  0x0000},  0xFFFF, 0, 0 }, /* Align90=84, for 12_1875G: Align90_Calib = Force */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x1C8, { 0x0FFE,  0x0FFE,  0x0FFE,  0x0FFE,  0x0FFE,  0x0FFE,  0x0FFE,  0x0FFE,  0x0FFE,  0x0FFE,  0x0FFE,  0x0000,  0x0FFE,  0x0000},  0xFFFF, 0, 0 }, /* sq_lpf_en=1, sq_lpf=0x3fe */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x0F0, { 0x4688,  0x4688,  0x4688,  0x4688,  0x4688,  0x4688,  0x4688,  0x4688,  0x4688,  0x4688,  0x4688,  0x0000,  0x4688,  0x0000},  0xFFFF, 0, 0 }, /* DFE Timer coarse EO_UP =6, EO_DN=4 */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x1CC, { 0xB20,   0x42C,   0x42C,   0x42C,   0x62C,   0x62C,   0x62C,   0x72C,   0x72C,   0x72C,   0x72C,   0x0000,  0x72C,   0x0000},  0xFFFF, 0, 0 }, /* samplr_vcm=0 */

    { SERDES_UNIT, SERDES_PHY_REGS + 0x1FC, { 0x1,     0x1,     0x1,     0x1,     0x1,     0x1,     0x1,     0x1,     0x1,     0x1,     0x1,     0x0000,  0x1,     0x0000},  0xFFFF, 0, 0 }, /* page 1 */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x0F0, { 0x38,    0x38,    0x38,    0x38,    0x38,    0x38,    0x38,    0x38,    0x38,    0x38,    0x38,    0x0000,  0x38,    0x0000},  0xFFFF, 0, 0 }, /* ffe_cal_vth[1:0]=3 */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x0FC, { 0xFD68,  0xFD68,  0xFD68,  0xFD68,  0xFD68,  0xFD68,  0xFD68,  0xFD68,  0xFE68,  0xFE68,  0xFE68,  0x0000,  0xFE68,  0x0000},  0xFFFF, 0, 0 }, /* change gain for FFE0-2 */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x100, { 0x5A4C,  0x5A4C,  0x5A4C,  0x5A4C,  0x5A4C,  0x5A4C,  0x5A4C,  0x5A4C,  0x5A4C,  0x5A4C,  0x5A4C,  0x0000,  0x5A4C,  0x0000},  0xFFFF, 0, 0 }, /* change gain for FFE3-5 */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x104, { 0x28C0,  0x28C0,  0x28C0,  0x28C0,  0x28C0,  0x28C0,  0x28C0,  0x28C0,  0x28C0,  0x28C0,  0x28C0,  0x0000,  0x28C0,  0x0000},  0xFFFF, 0, 0 }, /* ffe_gain_in6,7 changes */
    { SERDES_UNIT, SERDES_PHY_REGS + 0x1FC, { 0x0,     0x0,     0x0,     0x0,     0x0,     0x0,     0x0,     0x0,     0x0,     0x0,     0x0,     0x0000,  0x0,     0x0000},  0xFFFF, 0, 0 }, /* page 0 */
};

static MV_OP_PARAMS ptAfterPatternTestSeqParams[] = {
    {SERDES_UNIT, SERDES_PHY_REGS + 0x54, 0x80E0, 0xFFFF, 0, 0}
};

static MV_OP_PARAMS rxtrainingEnSeqParams[] = {
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x188,  0x200,  0x200,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x94,   0x1000, 0x1000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x18,   0x0,    0x70,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x18,   0xF,    0xF,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x18,   0x80,   0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1C,   0x2000, 0x2000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x20,   0xFC0,  0xFC0,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x38,   0x400,  0x400,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x40,   0x400,  0x400,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x48,   0x400,  0x400,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x50,   0x400,  0x400,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x98,   0x0,    0x100,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1A8,  0x4000, 0x4000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x14,   0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x2C,   0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x2C,   0x800,  0x2802, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x130,  0x80,   0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x16C,  0x3C4,  0x3FF,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x80,   0x233,  0x7FF,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xBC,   0x2,    0x2,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xBC,   0x0,    0x2,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xBC,   0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xBC,   0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x78,   0x0,    0x400,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x28,   0x2000, 0x2000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x120,  0x40,   0x40,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x120,  0x80,   0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x18,   0x0,    0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x130,  0x0,    0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xC,    0x200,  0x200,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xC,    0x0,    0x3F,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x184,  0x0,    0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x198,  0x0,    0xC00,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x184,  0x70,   0x70,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xF0,   0x4000, 0xF000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xF0,   0x600,  0xF00,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xEC,   0x6000, 0xF000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xEC,   0xA00,  0xF00,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1C,   0x40,   0xF0,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1C,   0x8,    0xF,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xF0,   0x8,    0xF,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xF0,   0x40,   0xF0,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xEC,   0x80,   0xF0,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xF0,   0x80,   0xF0,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x140,  0x1000, 0x1C00, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x78,   0x7800, 0x7800, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1AC,  0x1C0,  0x1C0,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1AC,  0x38,   0x38,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x7C,   0x7,    0x7,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x140,  0x7,    0x7,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x7C,   0x1C0,  0x1C0,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x140,  0x1C0,  0x1C0,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x7C,   0xE00,  0xE00,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x7C,   0x38,   0x38,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x140,  0x38,   0x38,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x120,  0x14,   0x1F,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x40,   0xC0,   0xF7E0, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x168,  0x0,    0x8000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x64,   0x0,    0xC000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x2C,   0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x84,   0x4874, 0xC87C, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x12C,  0x4000, 0x4000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x188,  0x0,    0x80,   0, 0},
    {SERDES_UNIT, 0xC,                      0x80,   0x80,   0, 0}
};

static MV_OP_PARAMS txtrainingEnSeqParams[] = {
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x34,   0xBDE,  0xFFFF, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xF4,   0x51,   0xFFFF, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xDC,   0xF91,  0x3FFF, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x138,  0x1C00, 0x3FFF, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x13C,  0xC71,  0x3FFF, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x188,  0x200,  0x200,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x94,   0x1000, 0x1000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x18,   0x0,    0x70,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x18,   0xF,    0xF,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x18,   0x80,   0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1C,   0x2000, 0x2000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x20,   0xFC0,  0xFC0,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x38,   0x400,  0x400,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x40,   0x400,  0x400,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x48,   0x400,  0x400,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x50,   0x400,  0x400,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x98,   0x0,    0x100,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1A8,  0x4000, 0x4000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x14,   0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x2c,   0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x2C,   0x800,  0x2802, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x130,  0x80,   0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x16C,  0x3C4,  0x3FF,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x80,   0xB3,   0x7FF,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xBC,   0x2,    0x2,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xBC,   0x0,    0x2,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xBC,   0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xBC,   0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x78,   0x0,    0x400,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x28,   0x2000, 0x2003, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x120,  0x40,   0x40,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x120,  0x80,   0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x9C,   0x27F,  0xFFF,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x94,   0xCC0,  0xFF8,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x94,   0x0,    0x1000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x18,   0x0,    0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x130,  0x0,    0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x70,   0x800,  0x8800, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x28,   0x80,   0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x11C,  0x202,  0x207,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x28,   0x400,  0x400,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x28,   0x0,    0x40,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x11C,  0x100,  0x100,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x14,   0x4400, 0xFF80, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x8C,   0xA,    0xE,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xC,    0x300,  0x300,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xC,    0x7,    0x3F,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x160,  0x0,    0x8000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x160,  0x0,    0x4000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x184,  0x0,    0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x198,  0x0,    0xC00,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x184,  0x70,   0x70,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xF0,   0x4000, 0xF000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xF0,   0x600,  0xF00,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xEC,   0x3000, 0xF000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xEC,   0xA00,  0xF00,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1C,   0x40,   0xF0,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1C,   0x8,    0xF,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xF0,   0x8,    0xF,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xF0,   0x80,   0xF0,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xEC,   0x80,   0xF0,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xF0,   0x80,   0xF0,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x140,  0x1000, 0x1C00, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x78,   0x7800, 0x7800, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1AC,  0x80,   0x1C0,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1AC,  0x18,   0x38,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x7C,   0x6,    0x7,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x140,  0x7,    0x7,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x7C,   0x80,   0x1C0,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x140,  0x1C0,  0x1C0,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x7C,   0x0E00, 0x7E00, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x7C,   0x18,   0x38,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x140,  0x38,   0x38,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x120,  0x18,   0x1F,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x40,   0xC0,   0xF7E0, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x64,   0x0,    0xC000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x84,   0x840,  0xC87C, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x12C,  0x4000, 0x4000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x168,  0x0,    0x8000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x188,  0x0,    0x80,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xB8,   0xFFFF, 0xFFFF, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xB8,   0x0,    0xFFFF, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xBC,   0x3F,   0x3F,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0xBC,   0x0,    0x3F,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0},
};

static MV_OP_PARAMS txtrainingDisSeqParams[] = {
    {SERDES_UNIT, 0x8,                      0x0,    0x20,   0, 0},
    {SERDES_UNIT, 0xC,                      0x0,    0x100,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x94,   0x1FFF, 0xFFFF, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x8C,   0x72,   0xFFFF, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x2C,   0x0,    0x2,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x11C,  0x0,    0x200,  0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0}
};

static MV_OP_PARAMS waitPllSeqParams[] = {
    {SERDES_UNIT, SERDES_PHY_REGS + 0x8,    0x8000, 0x8000, 0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x8,    0x4000, 0x4000, 1, 20},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x8,    0x0,    0x8000, 0, 0},
    {0,           0,                        0,      0,      8, 0}
};

static MV_OP_PARAMS ffeTableLrSeqParams[] = {
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x44,   0xF0E,  0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x48,   0xD0C,  0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x4C,   0x1F1E, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x50,   0x1D1C, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x54,   0x1b1a, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x58,   0x1918, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x5C,   0x1716, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x60,   0x1514, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0}
};

static MV_OP_PARAMS ffeTableSrSeqParams[] = {
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x1,    0x1,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x44,   0x2F2E, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x48,   0x2D2C, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x4C,   0x3F3E, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x50,   0x3D3C, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x54,   0x3b3a, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x58,   0x3938, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x5C,   0x3736, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x60,   0x3534, 0x0,    0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x130,  0x2F,   0x7F,   0, 0},
    {SERDES_UNIT, SERDES_PHY_REGS + 0x1FC,  0x0,    0x1,    0, 0}
};

/* seqId to params array mapping */
static MV_SEQ_OP_PARAMS seqIdToParams[] =
{
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SD_RESET_SEQ                  */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SD_UNRESET_SEQ                */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_RF_RESET_SEQ                  */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_RF_UNRESET_SEQ                */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SYNCE_RESET_SEQ               */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SYNCE_UNRESET_SEQ             */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SERDES_POWER_UP_CTRL_SEQ      */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SERDES_POWER_DOWN_CTRL_SEQ    */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SERDES_RXINT_UP_SEQ           */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SERDES_RXINT_DOWN_SEQ         */
    {waitPllSeqParams,            MV_SEQ_SIZE(waitPllSeqParams)},            /* SERDES_SERDES_WAIT_PLL_SEQ           */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_1_25G_SEQ               */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_3_125G_SEQ              */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_3_75G_SEQ               */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_4_25G_SEQ               */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_5G_SEQ                  */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_6_25G_SEQ               */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_7_5G_SEQ                */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_10_3125G_SEQ            */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SD_LPBK_NORMAL_SEQ            */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SD_ANA_TX_2_RX_SEQ            */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SD_DIG_TX_2_RX_SEQ            */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SD_DIG_RX_2_TX_SEQ            */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_PT_AFTER_PATTERN_NORMAL_SEQ   */
    {ptAfterPatternTestSeqParams, MV_SEQ_SIZE(ptAfterPatternTestSeqParams)}, /* SERDES_PT_AFTER_PATTERN_TEST_SEQ     */
    {rxtrainingEnSeqParams,       MV_SEQ_SIZE(rxtrainingEnSeqParams)},       /* SERDES_RX_TRAINING_ENABLE_SEQ        */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_RX_TRAINING_DISABLE_SEQ       */
    {txtrainingEnSeqParams,       MV_SEQ_SIZE(txtrainingEnSeqParams)},       /* SERDES_TX_TRAINING_ENABLE_SEQ        */
    {txtrainingDisSeqParams,      MV_SEQ_SIZE(txtrainingDisSeqParams)},      /* SERDES_TX_TRAINING_DISABLE_SEQ       */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_12_5G_SEQ               */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_3_3G_SEQ                */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_11_5625G_SEQ            */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SERDES_PARTIAL_POWER_DOWN_SEQ */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SERDES_PARTIAL_POWER_UP_SEQ   */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_11_25G_SEQ              */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_CORE_RESET_SEQ                */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_CORE_UNRESET_SEQ              */
    {ffeTableLrSeqParams,         MV_SEQ_SIZE(ffeTableLrSeqParams)},         /* SERDES_FFE_TABLE_LR_SEQ              */
    {ffeTableSrSeqParams,         MV_SEQ_SIZE(ffeTableSrSeqParams)},         /* SERDES_FFE_TABLE_SR_SEQ              */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_10_9375G_SEQ            */
    {NULL,                        MV_SEQ_SIZE(0)},                           /* SERDES_SPEED_12_1875G_SEQ            */
    {NULL,                        MV_SEQ_SIZE(0)}                            /* SERDES_SPEED_5_625G_SEQ              */
};

GT_STATUS hwsComPhyHRev2SpeedSeqInit(MV_OP_PARAMS *speedSeqParams, MV_SERDES_SPEED speed, MV_HWS_COM_PHY_H_SUB_SEQ seqSpeed)
{
    GT_U32 speedParams;

    /* Allocate sequence operation parameters */
    speedSeqParams = (MV_OP_PARAMS*)(hwsOsMallocFuncPtr(sizeof(MV_OP_PARAMS) * serdesSpeedSeqSize));
    if (speedSeqParams == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }
    hwsOsMemSetFuncPtr(speedSeqParams, 0, (sizeof(MV_OP_PARAMS) * serdesSpeedSeqSize));

    /* Init sequence operation parameters from configuration table */
    for (speedParams = 0; speedParams < serdesSpeedSeqSize; speedParams++)
    {
        speedSeqParams[speedParams].unitId     = hwsSerdesSpeedSeqParams[speedParams].unitId;
        speedSeqParams[speedParams].regOffset  = hwsSerdesSpeedSeqParams[speedParams].regOffset;
        speedSeqParams[speedParams].operData   = hwsSerdesSpeedSeqParams[speedParams].regData[speed];
        speedSeqParams[speedParams].mask       = hwsSerdesSpeedSeqParams[speedParams].mask;
        speedSeqParams[speedParams].waitTime   = hwsSerdesSpeedSeqParams[speedParams].waitTime;
        speedSeqParams[speedParams].numOfLoops = hwsSerdesSpeedSeqParams[speedParams].numOfLoops;
    }

    /* Update seqIdToParams table with new structure */
    seqIdToParams[seqSpeed].seqSize   = serdesSpeedSeqSize;
    seqIdToParams[seqSpeed].seqParams = speedSeqParams;

    return GT_OK;
}

GT_STATUS hwsComPhyHRev2SeqInit(void)
{
    MV_VALIDATE_SEQ_OPER(&countInitCalls);

    /* Calc Serdes Speed Sequence size */
    serdesSpeedSeqSize = sizeof(hwsSerdesSpeedSeqParams) / sizeof(MV_OP_SERDES_PARAMS);

    /* Init Speed parameters */
    CHECK_STATUS(hwsComPhyHRev2SpeedSeqInit(speed_1_25G_SeqParams,    SERDES_SPEED_1_25G,    SERDES_SPEED_1_25G_SEQ));
    CHECK_STATUS(hwsComPhyHRev2SpeedSeqInit(speed_3_125G_SeqParams,   SERDES_SPEED_3_125G,   SERDES_SPEED_3_125G_SEQ));
    CHECK_STATUS(hwsComPhyHRev2SpeedSeqInit(speed_3_3G_SeqParams,     SERDES_SPEED_3_3G,     SERDES_SPEED_3_3G_SEQ));
    CHECK_STATUS(hwsComPhyHRev2SpeedSeqInit(speed_3_75G_SeqParams,    SERDES_SPEED_3_75G,    SERDES_SPEED_3_75G_SEQ));
    CHECK_STATUS(hwsComPhyHRev2SpeedSeqInit(speed_4_25G_SeqParams,    SERDES_SPEED_4_25G,    SERDES_SPEED_4_25G_SEQ));
    CHECK_STATUS(hwsComPhyHRev2SpeedSeqInit(speed_5G_SeqParams,       SERDES_SPEED_5G,       SERDES_SPEED_5G_SEQ));
    CHECK_STATUS(hwsComPhyHRev2SpeedSeqInit(speed_6_25G_SeqParams,    SERDES_SPEED_6_25G,    SERDES_SPEED_6_25G_SEQ));
    CHECK_STATUS(hwsComPhyHRev2SpeedSeqInit(speed_7_5G_SeqParams,     SERDES_SPEED_7_5G,     SERDES_SPEED_7_5G_SEQ));
    CHECK_STATUS(hwsComPhyHRev2SpeedSeqInit(speed_10_3125G_SeqParams, SERDES_SPEED_10_3125G, SERDES_SPEED_10_3125G_SEQ));
    CHECK_STATUS(hwsComPhyHRev2SpeedSeqInit(speed_11_5625G_SeqParams, SERDES_SPEED_11_5625G, SERDES_SPEED_11_5625G_SEQ));
    CHECK_STATUS(hwsComPhyHRev2SpeedSeqInit(speed_12_5G_SeqParams,    SERDES_SPEED_12_5G,    SERDES_SPEED_12_5G_SEQ));
    CHECK_STATUS(hwsComPhyHRev2SpeedSeqInit(speed_12_1875G_SeqParams, SERDES_SPEED_12_1875G, SERDES_SPEED_12_1875G_SEQ));

    /* SERDES Sequences create */
    CHECK_STATUS(mvAllocateAllSeqOperation(&hwsSerdesRev2SeqDb[0], seqIdToParams, MV_SERDES_LAST_SEQ));

    /* SERDES Sequences operation & parameters update */
    mvInitSeqOperation(&hwsSerdesRev2SeqDb[0], seqIdToParams, MV_SERDES_LAST_SEQ);

    return GT_OK;
}

void hwsComPhyHRev2SeqFree(void)
{
    GT_U32 i;

    for (i = 0; i < MV_SERDES_LAST_SEQ; i++)
    {
        if (hwsSerdesRev2SeqDb[i].cfgSeq != NULL)
        {
            hwsOsFreeFuncPtr(hwsSerdesRev2SeqDb[i].cfgSeq);
        }
    }

    countInitCalls = 0;
}

GT_STATUS hwsComPhyHRev2SeqGet(MV_HWS_COM_PHY_H_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if (seqType >= MV_SERDES_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  if (hwsSerdesRev2SeqDb[seqType].cfgSeq == NULL)
  {
      /* check prev version */
      if (hwsSerdesSeqDb[seqType].cfgSeq == NULL)
      {
          return GT_NOT_INITIALIZED;
      }
      return hwsComPhyHSeqGet(seqType, seqLine, lineNum);
  }

  return mvGetSeqOperation(&hwsSerdesRev2SeqDb[seqType], seqLine, lineNum);
}

GT_STATUS hwsComPhyHRev2SeqSet(GT_BOOL firstLine, MV_HWS_COM_PHY_H_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  if (seqType >= MV_SERDES_LAST_SEQ)
  {
      return GT_NO_SUCH;
  }

  if (hwsSerdesRev2SeqDb[seqType].cfgSeq == NULL)
  {
      /* check prev version */
      if (hwsSerdesSeqDb[seqType].cfgSeq == NULL)
      {
          return GT_NOT_INITIALIZED;
      }
      return hwsComPhyHSeqSet(firstLine, seqType, seqLine, numOfOp);
  }

  return mvSetSeqOperation(firstLine, &hwsSerdesRev2SeqDb[seqType], numOfOp, seqLine);
}

void hwsChange1GSpeedCfgV43(void)
{
    speed_1_25G_SeqParams[/*4 */6 ].operData = 0x0DFF;
    speed_1_25G_SeqParams[/*11*/13].operData = 0xb0b4;
    speed_1_25G_SeqParams[/*12*/14].operData = 0x2494;
    speed_1_25G_SeqParams[/*13*/15].operData = 0x2094;
    speed_1_25G_SeqParams[/*14*/16].operData = 0x03a0;
    speed_1_25G_SeqParams[/*16*/18].operData = 0x3070;
    speed_1_25G_SeqParams[/*28*/30].operData = 0x0B28;
}

void hwsChangeSpeedCfgParams(MV_SERDES_SPEED speed, GT_U32 regOffset, GT_U32 regValue)
{
    GT_U32 i;
    GT_U32 speedSize;
    MV_OP_PARAMS *speedParams;

    speedParams = seqIdToParams[speed].seqParams;
    speedSize   = seqIdToParams[speed].seqSize;

    for (i = 0; i < speedSize; i++)
    {
        if (regOffset != speedParams[i].regOffset)
            continue;

        speedParams[i].operData = regValue;
        return;
    }
    return;
}

/*******************************************************************************
* hwsSetSeqParams
*
*       update parameter of sequence according to received ID
*       The function will search the received register offset in the sequence parameters
*       if the register offset was found, the function will check
*       if mask of this offset contain the bits of the received mask.
*       If it contain the received mask,
*       function will update the received data in the sequence parameter
*       If it not found the function will return no such.
*
* INPUTS:
*       seqId:      ID of sequence
*       offset:     register offset, number of the register to update
*       data:       data to update
*       dataMask:   bits to update
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK - on success
*       GT_NO_SUCH/GT_NOT_SUPPORTED - on error
*
*******************************************************************************/
GT_STATUS hwsSetSeqParams
(
    GT_U32  seqId,
    GT_U32  offset,
    GT_U32  data,
    GT_U32  dataMask
)
{
    GT_U32 i;

    if (seqIdToParams[seqId].seqParams == NULL)
    {
        return GT_NOT_SUPPORTED;
    }

    for (i = 0; i < seqIdToParams[seqId].seqSize; i++)
    {
        if ((seqIdToParams[seqId].seqParams[i].regOffset == offset)&&
            /* check if the mask of this entry appropriate to the write data */
            ((seqIdToParams[seqId].seqParams[i].mask & dataMask) == dataMask))
        {
            /* reset data bits */
            seqIdToParams[seqId].seqParams[i].operData &= ~dataMask;
            /* update data value */
            seqIdToParams[seqId].seqParams[i].operData |= data;

            return GT_OK;
        }
    }

    return GT_NO_SUCH;
}
