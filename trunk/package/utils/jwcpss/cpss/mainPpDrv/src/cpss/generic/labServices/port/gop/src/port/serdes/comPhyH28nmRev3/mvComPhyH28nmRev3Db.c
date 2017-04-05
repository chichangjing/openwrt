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
*       $Revision: 11 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <serdes/comPhyH28nmRev3/mvComPhyH28nmRev3Db.h>
#include <serdes/comPhyH28nm/mvComPhyH28nmDb.h>
#include <serdes/mvHwsSerdesPrvIf.h>
#include <serdes/mvHwsSerdes28nmPrvIf.h>

#include <port/private/mvHwsPortApCoCpuIf.h>
#include <common/siliconIf/siliconGpSram.h>

/******************************** enums ***************************************/

/******************************** pre-declaration *****************************/

GT_STATUS hwsComPhyH28nmRev3InitSeqFlow(MV_CFG_SEQ* hwSerdesSeqDbArr);

/******************************** globals **************************************/

static MV_CFG_SEQ hwsSerdes28nmRev3SeqDb[MV_SERDES_LAST_SEQ];
extern MV_CFG_SEQ *hwsSerdesSeqDbPtr; /* used for sequence set/get */

#ifndef CO_CPU_RUN
/* Note: when changing register 0x0 values, SerdesSpeedGet function should be updated as well */
static MV_SPEED_CONFIG_REG_DATA hwSerdesSpeedExtConfigInit[] =
{
    /*                   1.25G      3.125G  3.75G   4.25G   5G      6.25G   7.5G    10.3125G    11.25G      11.5625G    12.5G   10.9375G    12.1875G    5.625G  */
    { 0x000,    0x7F8, { 0x330,     0x440,  0x4C8,  0x550,  0x198,  0x5D8,  0x6E8,  0x770,      0x7F8,      0x7F8,      0x7F8,  0x7F8,      0x7F8,      0x198   } }, /* Setting PIN_GEN_TX, PIN_GEN_RX */
    { 0x028,    0x1F,  { 0xC,       0xC,    0xC,    0xC,    0xC,    0xC,    0xC,    0xC,        0xC,        0xC,        0xC,    0xC,        0xC,        0xC     } }, /* PIN_FREF_SEL=C (156.25MHz) */
};

/* array that hold the SERDES initialization process, each entry hold
   the MV_CONFIG_REG_DATA register and the value to write per baud rate */
static MV_SPEED_CONFIG_REG_DATA hwSerdesSpeedIntConfigInit[] =
{
    /*                   1.25G      3.125G      3.75G       4.25G       5G          6.25G       7.5G        10.3125G    11.25G      11.5625G    12.5G       10.9375G    12.1875G    5.625G      */
    { 0x4,   0xFFFF,   { 0xFD8C,    0xFD8C,     0xFD8C,     0xFD8C,     0xFD8C,     0xFD8C,     0xFD8C,     0xFD8C,     0xFD8C,     0xFD8C,     0xFD8C,     0xFD8C,     0xFD8C,     0xFD8C      } },
    { 0x18,  0xFFFF,   { 0x4F00,    0x4F00,     0x4F00,     0x4F00,     0x4F00,     0x4F00,     0x4F00,     0x4F00,     0x4F00,     0x4F00,     0x4F00,     0x4F00,     0x4F00,     0x4F00      } },  /* squelch in */
    { 0x1C,  0xFFFF,   { 0xF047,    0xF047,     0xF047,     0xF047,     0xF047,     0xF047,     0xFC26,     0xFC2A,     0xFC2A,     0xFC2A,     0xFC2A,     0xFC2A,     0xFC2A,     0xF047      } },  /* DFE UPDAE all coefficient DFE_RES = 3.3mv */
    { 0x34,  0xFFFF,   { 0x406C,    0x406C,     0x406C,     0x406C,     0x406C,     0x406C,     0x406C,     0x406C,     0x406C,     0x406C,     0x406C,     0x406C,     0x406C,     0x406C      } },  /* TX_AMP=22, AMP_ADJ=0 */
    { 0x38,  0xFFFF,   { 0x1E40,    0x1E40,     0x1E40,     0x1E40,     0x1E40,     0x1E49,     0x1E52,     0x1E52,     0x1E52,     0x1E52,     0x1E52,     0x1E52,     0x1E52,     0x1E40      } },  /* MUPI/F=2, rx_digclkdiv=3 */
    { 0x94,  0xFFFF,   { 0x1FFF,    0x1FFF,     0x1FFF,     0x1FFF,     0x1FFF,     0x1FFF,     0x1FFF,     0x1FFF,     0x1FFF,     0x1FFF,     0x1FFF,     0x1FFF,     0x1FFF,     0x1FFF      } },
    { 0x98,  0xFFFF,   { 0x66,      0x88,       0x99,       0xAA,       0x33,       0xBB,       0xDD,       0xEE,       0xFF,       0xFF,       0xFF,       0xEE,       0xFF,       0x33        } },  /* set password */
    { 0x104, 0xFFFF,   { 0x2208,    0x2208,     0x2208,     0x2208,     0x2208,     0x2208,     0x2208,     0x2208,     0x2208,     0x2208,     0x2208,     0x2208,     0x2208,     0x2208      } },
    { 0x108, 0xFFFF,   { 0x243F,    0x243F,     0x243F,     0x243F,     0x243F,     0x243F,     0x243F,     0x243F,     0x243F,     0x243F,     0x243F,     0x243F,     0x243F,     0x243F      } },  /* continues vdd_calibration */
    { 0x114, 0xFFFF,   { 0x46CF,    0x46CF,     0x46CF,     0x46CF,     0x46CF,     0x46CF,     0x46CF,     0x46CF,     0x46CF,     0x46CF,     0x46CF,     0x46CF,     0x46CF,     0x46CF      } },  /* EMPH0 enable, EMPH_mode=2 */
    { 0x134, 0xFFFF,   { 0x004A,    0x004A,     0x004A,     0x004A,     0x004A,     0x004A,     0x004A,     0x004A,     0x004A,     0x004A,     0x004A,     0x004A,     0x004A,     0x004A      } },  /* RX_IMP_VTH=2, TX_IMP_VTH=2 */
    { 0x13C, 0xFBFF,   { 0xE028,    0xE028,     0xE028,     0xE028,     0xE028,     0xE028,     0xE028,     0xE028,     0xE028,     0xE028,     0xE028,     0xE028,     0xE028,     0xE028      } },  /* Force ICP=8 */
    { 0x140, 0xFFFF,   { 0x800,     0x800,      0x800,      0x800,      0x800,      0x800,      0x1800,     0x1800,     0x1800,     0x1800,     0x1800,     0x1800,     0x1800,     0x1800      } },  /* clk 8T enable for 10G and up */
    { 0x154, 0xFFFF,   { 0x87,      0x87,       0x87,       0x87,       0x87,       0x87,       0x87,       0x87,       0x87,       0x87,       0x87,       0x87,       0x87,       0x87        } },  /* rxdigclk_div_force=1  */
    { 0x168, 0xFFFF,   { 0xE014,    0xE014,     0xE014,     0xE014,     0xE014,     0x6014,     0x6014,     0x6014,     0x6014,     0x6014,     0x6014,     0x6014,     0x6014,     0xE014      } },
    { 0x16C, 0xFFFF,   { 0x14,      0x14,       0x14,       0x14,       0x14,       0x14,       0x14,       0x14,       0x14,       0x14,       0x14,       0x14,       0x14,       0x14        } },  /*  DTL_FLOOP_EN=0  */
    { 0x184, 0xFFFF,   { 0x10F3,    0x10F3,     0x10F3,     0x10F3,     0x10F3,     0x10F3,     0x10F3,     0x10F3,     0x10F3,     0x10F3,     0x10F3,     0x10F3,     0x10F3,     0x10F3      } },
    { 0x1A8, 0xFFFF,   { 0x4000,    0x4000,     0x4000,     0x4000,     0x4000,     0x4000,     0x4000,     0x4000,     0x4000,     0x4000,     0x4000,     0x4000,     0x4000,     0x4000      } },
    { 0x1AC, 0xFFFF,   { 0x8498,    0x8498,     0x8498,     0x8498,     0x8498,     0x8498,     0x8498,     0x8498,     0x8498,     0x8498,     0x8498,     0x8498,     0x8498,     0x8498      } },
    { 0x1DC, 0xFFFF,   { 0x780,     0x780,      0x780,      0x780,      0x780,      0x780,      0x780,      0x780,      0x780,      0x780,      0x780,      0x780,      0x780,      0x780       } },
    { 0x1E0, 0xFFFF,   { 0x03FE,    0x03FE,     0x03FE,     0x03FE,     0x03FE,     0x03FE,     0x03FE,     0x03FE,     0x03FE,     0x03FE,     0x03FE,     0x03FE,     0x03FE,     0x03FE      } },  /* PHY_MODE=0x4,FREF_SEL=0xC   */
    { 0x214, 0xFFFF,   { 0x4418,    0x4418,     0x4418,     0x4418,     0x4418,     0x4418,     0x4418,     0x4418,     0x4418,     0x4418,     0x4418,     0x4418,     0x4418,     0x4418      } },  /* PHY_MODE=0x4,FREF_SEL=0xC   */
    { 0x220, 0xFFFF,   { 0x400,     0x400,      0x400,      0x400,      0x400,      0x400,      0x400,      0x400,      0x400,      0x400,      0x400,      0x400,      0x400,      0x400       } },
    { 0x228, 0xFFFF,   { 0x2FC0,    0x2FC0,     0x2FC0,     0x2FC0,     0x2FC0,     0x2FC0,     0x2FC0,     0x2FC0,     0x2FC0,     0x2FC0,     0x2FC0,     0x2FC0,     0x2FC0,     0x2FC0      } },
    { 0x268, 0xFFFF,   { 0x8C02,    0x8C02,     0x8C02,     0x8C02,     0x8C02,     0x8C02,     0x8C02,     0x8C02,     0x8C02,     0x8C02,     0x8C02,     0x8C02,     0x8C02,     0x8C02      } },
    { 0x278, 0xFFFF,   { 0x21F3,    0x21F3,     0x21F3,     0x21F3,     0x21F3,     0x21F3,     0x21F3,     0x21F3,     0x21F3,     0x21F3,     0x21F3,     0x21F3,     0x21F3,     0x21F3      } },
    { 0x280, 0xFFFF,   { 0xC9F8,    0xC9F8,     0xC9F0,     0xC9F8,     0xC9F8,     0xC9F8,     0xC9F8,     0xC9F8,     0xC9F8,     0xC9F8,     0xC9F8,     0xC9F8,     0xC9F8,     0xC9F8      } },
    { 0x29C, 0xFFFF,   { 0x05BC,    0x05BC,     0x05BC,     0x05BC,     0x05BC,     0x05BC,     0x05BC,     0x05BC,     0x05BC,     0x05BC,     0x05BC,     0x05BC,     0x05BC,     0x05BC      } },
    { 0x2DC, 0xFFFF,   { 0x2233,    0x2233,     0x2233,     0x2233,     0x2233,     0x2233,     0x2233,     0x2119,     0x2119,     0x2119,     0x2119,     0x2119,     0x2119,     0x2233      } },
    { 0x31C, 0xFFFF,   { 0x318,     0x318,      0x318,      0x318,      0x318,      0x318,      0x318,      0x318,      0x318,      0x318,      0x318,      0x318,      0x318,      0x318       } },
    { 0x330, 0xFFFF,   { 0x013F,    0x013F,     0x013F,     0x012F,     0x012F,     0x012F,     0x012F,     0x012F,     0x012F,     0x012F,     0x012F,     0x012F,     0x012F,     0x012F      } },
    { 0x334, 0xFFFF,   { 0x0C03,    0x0C03,     0x0C03,     0x0C03,     0x0C03,     0x0C03,     0x0C03,     0x0C03,     0x0C03,     0x0C03,     0x0C03,     0x0C03,     0x0C03,     0x0C03      } },
    { 0x338, 0xFFFF,   { 0x3C00,    0x3C00,     0x3C00,     0x3C00,     0x3C00,     0x3C00,     0x3C00,     0x1B00,     0x1B00,     0x1B00,     0x1B00,     0x1B00,     0x1B00,     0x3C00      } }, /* default_2 */
    { 0x33C, 0xFFFF,   { 0x3C00,    0x3C00,     0x3C00,     0x3C00,     0x3C00,     0x3C00,     0x3C00,     0x1B00,     0x1B00,     0x1B00,     0x1B00,     0x1B00,     0x1B00,     0x3C00      } }, /* default_3 */
    { 0x368, 0xFFFF,   { 0x1000,    0x1000,     0x1000,     0x1000,     0x1000,     0x1000,     0x1000,     0x1000,     0x1000,     0x1000,     0x1000,     0x1000,     0x1000,     0x1000      } },
    { 0x36C, 0xFFFF,   { 0x0AD9,    0x0AD9,     0x0AD9,     0x0AD9,     0x0AD9,     0x0AD9,     0x0AD9,     0x0AD9,     0x0AD9,     0x0AD9,     0x0AD9,     0x0AD9,     0x0AD9,     0x0AD9      } },
    { 0x378, 0xFFFF,   { 0x1800,    0x1800,     0x1800,     0x1800,     0x1800,     0x1800,     0x1800,     0x1800,     0x1800,     0x1800,     0x1800,     0x1800,     0x1800,     0x1800      } },
    { 0x418, 0xFFFF,   { 0xE737,    0xE737,     0xE737,     0xE737,     0xE737,     0xE737,     0xE737,     0xE737,     0xE737,     0xE737,     0xE737,     0xE737,     0xE737,     0xE737      } },
    { 0x420, 0xFFFF,   { 0x9CE0,    0x9CE0,     0x9CE0,     0x9CE0,     0x9CE0,     0x9CE0,     0x9CE0,     0x9CE0,     0x9CE0,     0x9CE0,     0x9CE0,     0x9CE0,     0x9CE0,     0x9CE0      } },
    { 0x440, 0xFFFF,   { 0x003E,    0x503E,     0x503E,     0x503E,     0x503E,     0x503E,     0x503E,     0x603E,     0xD03E,     0xD03E,     0xD03E,     0xD03E,     0xD03E,     0x503E      } },
    { 0x444, 0xFFFF,   { 0x2681,    0x2561,     0x2561,     0x2561,     0x2561,     0x2541,     0x2541,     0x2421,     0x2421,     0x2421,     0x2421,     0x2421,     0x2421,     0x2561      } },
    { 0x468, 0xFFFF,   { 0x1,       0x1,        0x1,        0x1,        0x1,        0x1,        0x1,        0x1,        0x1,        0x1,        0x1,        0x1,        0x1,        0x1         } },
    { 0x46C, 0xFFFF,   { 0xFC7C,    0xFC7C,     0xFC7C,     0xFC7C,     0xFC7C,     0xFC7C,     0xFC7C,     0xFC7C,     0xFC7C,     0xFC7C,     0xFC7C,     0xFC7C,     0xFC7C,     0xFC7C      } },
    { 0x4A0, 0xFFFF,   { 0x0104,    0x0104,     0x0104,     0x0104,     0x0104,     0x0104,     0x0104,     0x0104,     0x0104,     0x0104,     0x0104,     0x0104,     0x0104,     0x0104      } },
    { 0x4A4, 0xFFFF,   { 0x0302,    0x0302,     0x0302,     0x0302,     0x0302,     0x0302,     0x0302,     0x0302,     0x0302,     0x0302,     0x0302,     0x0302,     0x0302,     0x0302      } },
    { 0x4A8, 0xFFFF,   { 0x0202,    0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202      } },
    { 0x4AC, 0xFFFF,   { 0x0202,    0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202,     0x0202      } },
    { 0x10C, 0xFFFF,   { 0x0830,    0x0830,     0x0830,     0x0830,     0x0830,     0x0830,     0x0830,     0x0830,     0x0830,     0x0830,     0x0830,     0x0830,     0x0830,     0x0830      } }, /* txclk regulator threshold set */
    { 0x110, 0xFFFF,   { 0x0F30,    0x0F30,     0x0F30,     0x0F30,     0x0F30,     0x0F30,     0x0F30,     0x0F30,     0x0F30,     0x0F30,     0x0F30,     0x0F30,     0x0F30,     0x0F30      } }, /* txdata regulator threshold set */
    { 0x424, 0x0060,   { 0x0,       0x0,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0,        0x0         } }, /* os_ph_step_size[1:0]= 0 */
    { 0x39C, 0xFFFF,   { 0x0559,    0x0559,     0x0559,     0x0559,     0x0559,     0x0559,     0x0559,     0x0559,     0x0559,     0x0559,     0x0559,     0x0559,     0x0559,     0x0559      } },
    { 0x244, 0xFFFF,   { 0x3F3E,    0x3F3E,     0x3F3E,     0x3F3E,     0x3F3E,     0x3F3E,     0x3F3E,     0x3F3E,     0x3F3E,     0x3F3E,     0x3F3E,     0x3F3E,     0x3F3E,     0x3F3E      } }, /*  FFE table RC= 0F/1F    */
    { 0x248, 0xFFFF,   { 0x4F4E,    0x4F4E,     0x4F4E,     0x4F4E,     0x4F4E,     0x4F4E,     0x4F4E,     0x4F4E,     0x4F4E,     0x4F4E,     0x4F4E,     0x4F4E,     0x4F4E,     0x4F4E      } }, /*  FFE table RC= 2F/3F    */
    { 0x24C, 0xFFFF,   { 0x5F5E,    0x5F5E,     0x5F5E,     0x5F5E,     0x5F5E,     0x5F5E,     0x5F5E,     0x5F5E,     0x5F5E,     0x5F5E,     0x5F5E,     0x5F5E,     0x5F5E,     0x5F5E      } }, /*  FFE table RC= 4F/0E    */
    { 0x250, 0xFFFF,   { 0x6F6E,    0x6F6E,     0x6F6E,     0x6F6E,     0x6F6E,     0x6F6E,     0x6F6E,     0x6F6E,     0x6F6E,     0x6F6E,     0x6F6E,     0x6F6E,     0x6F6E,     0x6F6E      } }, /*  FFE table RC= 1E/2E    */
    { 0x254, 0xFFFF,   { 0x3D3C,    0x3D3C,     0x3D3C,     0x3D3C,     0x3D3C,     0x3D3C,     0x3D3C,     0x3D3C,     0x3D3C,     0x3D3C,     0x3D3C,     0x3D3C,     0x3D3C,     0x3D3C      } }, /*  FFE table RC= 3E/4E    */
    { 0x258, 0xFFFF,   { 0x4D4C,    0x4D4C,     0x4D4C,     0x4D4C,     0x4D4C,     0x4D4C,     0x4D4C,     0x4D4C,     0x4D4C,     0x4D4C,     0x4D4C,     0x4D4C,     0x4D4C,     0x4D4C      } }, /*  FFE table RC= 0D/1D    */
    { 0x25C, 0xFFFF,   { 0x5D5C,    0x5D5C,     0x5D5C,     0x5D5C,     0x5D5C,     0x5D5C,     0x5D5C,     0x5D5C,     0x5D5C,     0x5D5C,     0x5D5C,     0x5D5C,     0x5D5C,     0x5D5C      } }, /*  FFE table RC= 2D/3D    */
    { 0x260, 0xFFFF,   { 0x2F2E,    0x2F2E,     0x2F2E,     0x2F2E,     0x2F2E,     0x2F2E,     0x2F2E,     0x2F2E,     0x2F2E,     0x2F2E,     0x2F2E,     0x2F2E,     0x2F2E,     0x2F2E      } }, /*  FFE table RC= 4D/0C    */
    { 0x22C, 0x0800,   { 0x0000,    0x0000,     0x0000,     0x0000,     0x0000,     0x0800,     0x0800,     0x0800,     0x0800,     0x0800,     0x0800,     0x0800,     0x0800,     0x0000      } }, /*  select ffe table mode  */
    { 0x1CC, 0xFFFF,   { 0x0451,    0x0451,     0x0451,     0x0451,     0x0451,     0x0451,     0x0451,     0x0451,     0x0451,     0x0451,     0x0451,     0x0451,     0x0451,     0x0451      } }, /*  select smplr vcm=0.65V */
    { 0x170, 0x00FF,   { 0xCD8E,    0xCD8E,     0xCD8E,     0xCD8E,     0xCD8E,     0xCD8E,     0xCD8E,     0xCD8E,     0xCD8E,     0xCD8E,     0xCD8E,     0xCD8E,     0xCD8E,     0xCD8E      } }, /*  update process calibration */
    { 0x41C, 0x003F,   { 0x003F,    0x003F,     0x003F,     0x003F,     0x003F,     0x003F,     0x003F,     0x003F,     0x003F,     0x003F,     0x003F,     0x003F,     0x003F,     0x003F      } }, /*  update cdr_EO_bypass_trash[0:5]=63 */
};

/* SERDES_TX_TRAINING_ENABLE_SEQ Sequence init */
static MV_OP_PARAMS serdesTxTrainingEnParams[] =
{
    { SERDES_PHY_UNIT,  0x1C,     0x8000,    0x8000,     0,  0 },
    { SERDES_PHY_UNIT,  0x440,    0,         0x80,       0,  0 },
    { SERDES_PHY_UNIT,  0x168,    0x8000,    0x8000,     0,  0 },
    { SERDES_PHY_UNIT,  0x188,    0,         0x1FF,      0,  0 },
    { SERDES_PHY_UNIT,  0x94,     0xFFF,     0xFFFF,     0,  0 },
    { SERDES_PHY_UNIT,  0x31C,    0,         4,          0,  0 },
};

/* SERDES_TX_TRAINING_DISABLE_SEQ Sequence init */
static MV_OP_PARAMS serdesTxTrainingDisParams[] =
{
    {SERDES_UNIT,       0x8,    0x0,        0x20,       0,  0 },
    {SERDES_UNIT,       0xC,    0x0,        0x100,      0,  0 },
    {SERDES_PHY_UNIT,   0x22C,  0x0,        0x2,        0,  0 },
    {SERDES_PHY_UNIT,   0x94,   0x1FFF,     0xFFFF,     0,  0 },
    {SERDES_PHY_UNIT,   0x8C,   0x72,       0xFFFF,     0,  0 },
};

/* SERDES_RX_TRAINING_DISABLE_SEQ Sequence init */
static MV_OP_PARAMS serdesRxTrainingDisParams[] =
{
    {SERDES_UNIT,       0x8,    0x0,        0x20,       0,  0 },
    {SERDES_UNIT,       0xC,    0x0,        0x80,       0,  0 },
    {SERDES_PHY_UNIT,   0x22C,  0x0,        0x1,        0,  0 },
};

/* SERDES_RX_TRAINING_ENABLE_SEQ Sequence init */
static MV_OP_PARAMS serdesRxTrainingEnParams[] =
{
    { SERDES_PHY_UNIT,  0x1C,   0x8000,     0x8000,     0,  0 },
    { SERDES_PHY_UNIT,  0x440,  0,          0x80,       0,  0 },
    { SERDES_PHY_UNIT,  0x168,  0x8000,     0x8000,     0,  0 },
    { SERDES_PHY_UNIT,  0x188,  0,          0x1FF,      0,  0 },
    { SERDES_PHY_UNIT,  0x22C,  0x1,        0x1,        0,  0 },
    { SERDES_UNIT,      0xC,    0x80,       0x80,       0,  0 },
};


/* seqId to params array mapping */
static MV_OP_PARAMS *seqIdToParams[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
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
    NULL,
    NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

GT_STATUS hwsComPhyH28nmRev3SeqInit(void)
{
	GT_U32 intNum = sizeof(hwSerdesSpeedIntConfigInit)/sizeof(MV_SPEED_CONFIG_REG_DATA);
	GT_U32 extNum = sizeof(hwSerdesSpeedExtConfigInit)/sizeof(MV_SPEED_CONFIG_REG_DATA);

    hwsOsMemSetFuncPtr(hwsSerdes28nmRev3SeqDb, 0, sizeof(hwsSerdes28nmRev3SeqDb));

    /* Init configuration sequence executer */
    mvCfgSeqExecInit();
    mvCfgSerdesSeqExecInit(hwsSerdes28nmRev3SeqDb, (sizeof(hwsSerdes28nmRev3SeqDb) / sizeof(MV_CFG_SEQ)));

    /* SERDES_SPEED Sequence */
	CHECK_STATUS(hwsComPhyH28nmSpeedSeqInit(hwsSerdes28nmRev3SeqDb, hwSerdesSpeedExtConfigInit, extNum, hwSerdesSpeedIntConfigInit, intNum));

	/* SERDES_SPEED Sequence */
	CHECK_STATUS(hwsComPhyH28nmRev3InitSeqFlow(hwsSerdes28nmRev3SeqDb));

	hwsSerdesSeqDbPtr = hwsSerdes28nmRev3SeqDb; /* used for sequence set/get */

	return GT_OK;
}

GT_STATUS hwsComPhyH28nmRev3InitSeqFlow(MV_CFG_SEQ* hwSerdesSeqDbArr)
{
	GT_U32 i, seqId;
	GT_U32 sizeSEQ = MV_SERDES_LAST_SEQ;

    (void)hwSerdesSeqDbArr; /* avoid warnings */

	if ((sizeof(seqIdToParams)/sizeof(MV_OP_PARAMS*)) != sizeSEQ)
	{
		osPrintf("Error: hwsComPhyH28nmRev3InitSeqFlow: the size of seqIdToParams is different than MV_HWS_COM_PHY_H_SUB_SEQ size!!\n");
		return GT_BAD_SIZE;
	}

	/* First, initialize the DB with 28nm Sequences */
	CHECK_STATUS(hwsComPhyH28nmInitSeqFlow(hwsSerdes28nmRev3SeqDb));

	/* now, the changed sequences for Rev3 can be written */

    /* SERDES_RX_TRAINING_ENABLE_SEQ Sequence init */
    hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_ENABLE_SEQ].seqId = SERDES_RX_TRAINING_ENABLE_SEQ;
    hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeqSize = sizeof(serdesRxTrainingEnParams)/sizeof(MV_OP_PARAMS);
	if(hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeq != NULL)
	{
		/* delete rev 0 initlizations */
		hwsOsFreeFuncPtr(hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeq);
	}
    hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeqSize );
    if (hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_RX_TRAINING_DISABLE_SEQ Sequence init */
    hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_DISABLE_SEQ].seqId = SERDES_RX_TRAINING_DISABLE_SEQ;
    hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeqSize = sizeof(serdesRxTrainingDisParams)/sizeof(MV_OP_PARAMS);
	if(hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeq != NULL)
	{
		/* delete rev 0 initlizations */
		hwsOsFreeFuncPtr(hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeq);
	}
    hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeqSize );
    if (hwsSerdes28nmRev3SeqDb[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_TX_TRAINING_ENABLE_SEQ Sequence init */
    hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_ENABLE_SEQ].seqId = SERDES_TX_TRAINING_ENABLE_SEQ;
    hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeqSize = sizeof(serdesTxTrainingEnParams)/sizeof(MV_OP_PARAMS);
	if(hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeq != NULL)
	{
		/* delete rev 0 initlizations */
		hwsOsFreeFuncPtr(hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeq);
	}
    hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeqSize );
    if (hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeq == NULL)
    {
        return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_TX_TRAINING_DISABLE_SEQ Sequence init */
    hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_DISABLE_SEQ].seqId = SERDES_TX_TRAINING_DISABLE_SEQ;
    hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeqSize = sizeof(serdesTxTrainingDisParams)/sizeof(MV_OP_PARAMS);
	if(hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeq != NULL)
	{
		/* delete rev 0 initlizations */
		hwsOsFreeFuncPtr(hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeq);
	}
    hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeqSize );
    if (hwsSerdes28nmRev3SeqDb[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeq == NULL)
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
        for (i = 0; i < hwsSerdes28nmRev3SeqDb[seqId].cfgSeqSize; i++)
        {
            hwsSerdes28nmRev3SeqDb[seqId].cfgSeq[i].op = getCfgSeqOp(seqIdToParams[seqId][i].waitTime, seqIdToParams[seqId][i].numOfLoops);
            if (hwsSerdes28nmRev3SeqDb[seqId].cfgSeq[i].op == DELAY_OP)
            {
                seqIdToParams[seqId][i].operData = seqIdToParams[seqId][i].waitTime;
            }
            hwsSerdes28nmRev3SeqDb[seqId].cfgSeq[i].params = &(seqIdToParams[seqId][i]);
        }
    }

    return GT_OK;
}

#endif /* CO_CPU_RUN */

