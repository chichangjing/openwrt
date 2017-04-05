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
* wrDDR3.c
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

/* Common for all HWS Galtis wrappers */
#include <galtisAgent/wrapCpss/systemLab/wrHwsCommon.h>

/* General H Files */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/port/private/portHwConfig/ddr3/private/mvDdr3PrvIf.h>
#include <cpss/generic/port/private/portHwConfig/ddr3/mvHwsDdr3InitIf.h>
#include <cpss/generic/port/private/portHwConfig/ddr3/private/mvDdr3Regs.h>
#include <cpss/generic/port/private/portHwConfig/ddr3/private/mvDdr3StaticCfg.h>
#include <cpss/generic/port/private/portHwConfig/common/siliconIf/mvSiliconIf.h>
#include <cpss/generic/port/private/portHwConfig/ddr3/private/mvDdr3TrainingIf.h>
#include <cpss/generic/port/private/portHwConfig/ddr3/private/mvDdr3SdramIf.h>



typedef enum
{
    SLM_1230,
    SLM_1236
}MV_DDR3_BOARD_TYPE;

GT_U32 hwsDdr5APattern[] = {0x555,0xAAA,0x555, 0xAAA};
GT_U32 hwsDdrSso9Pattern[] = {0xFFF, 0x0, 0xFFF, 0x0};



/* static configuration data for dunit of LU IF, board SLM 1236, 400Mhg */
HWS_DRAM_STATIC_CFG dunit_LU_SLM1230_400[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,        0x8,       0xa,        0x1a},
    {1,        0,        0x6,       0xa,        0x219},
    {2,        0,        0x8,       0xa,        0x18},
    {3,        0,        0x6,       0xa,        0x217},
    {4,        0,        0x8,       0xa,        0x17},
    {5,        0,        0x6,       0xa,        0x217},
    {6,        0,        0x8,       0xa,        0x1a},
    {7,        0,        0x6,       0xa,        0x21c},
    {8,        0,        0x8,       0xa,        0x17},
    {9,        0,        0x6,       0xa,        0x217},
    {10,       0,        0x8,       0xa,        0x10},
    {11,       0,        0x6,       0xa,        0x211},
    {12,       0,        0x8,       0xa,        0x10},
    {13,       0,        0x6,       0xa,        0x20F}
};

/* static configuration data for dunit of FWD IF, board SLM 1236, 400Mhg */
HWS_DRAM_STATIC_CFG dunit_FWD_SLM1230_400[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,      0x7,        0x9,        0x112}, /* 0 */
    {0,        1,      0x7,        0x9,        0x111}, /* 0 */
    {1,        0,      0x7,        0xB,       0x311}, /* 1 */
    {1,        1,      0x7,        0xB,       0x311}, /* 1 */
    {2,        0,      0x7,        0x9,        0x116}, /* 2 */
    {2,        1,      0x7,        0x9,        0x116}, /* 2 */
    {3,        0,      0xD,        0xB,        0x316}, /* 3 */
    {3,        1,      0xD,        0xB,        0x315}, /* 3 */
    {4,        0,      0x7,        0x9,        0x11D},  /* 4 */
    {4,        1,      0x7,        0x9,        0x11B},  /* 4 */
    {5,        0,      0xD,         0xB,        0x31B},  /* 5 */
    {5,        1,      0xD,         0xB,        0x31A},  /* 5 */
    {6,        0,      0x7,        0x9,        0x118},  /* 6 */
    {6,        1,      0x7,        0x9,       0x117},  /* 6 */
    {7,        0,      0xD,         0xB,        0x318},  /* 7 */
    {7,        1,      0xD,         0xB,        0x317},  /* 7 */
};

/* static configuration data for dunit of FWD IF, board SLM 1230, 400Mhg */
HWS_DRAM_STATIC_CFG dunit_FWD_SLM1236_400[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,      0x7,        0xB,        0x20E}, /* 0 */
    {0,        1,      0x7,        0xB,        0x20F}, /* 0 */
    {1,        0,      0x7,        0x9,       0xE}, /* 1 */
    {1,        1,      0x7,        0x9,       0xE}, /* 1 */
    {2,        0,      0x7,        0xB,        0x210}, /* 2 */
    {2,        1,      0x7,        0xB,        0x210}, /* 2 */
    {3,        0,      0xD,        0x9,        0x10}, /* 3 */
    {3,        1,      0xD,        0x9,        0x10}, /* 3 */
    {4,        0,      0x7,        0xB,        0x218},  /* 4 */
    {4,        1,      0x7,        0xB,        0x218},  /* 4 */
    {5,        0,      0xD,         0x9,        0x17},  /* 5 */
    {5,        1,      0xD,         0x9,        0x17},  /* 5 */
    {6,        0,      0x7,        0xB,        0x219},  /* 6 */
    {6,        1,      0x7,        0xB,        0x21A},  /* 6 */
    {7,        0,      0xD,         0x9,        0x1A},  /* 7 */
    {7,        1,      0xD,         0x9,        0x1A},  /* 7 */
};

/* static configuration data for dunit of LU IF, board SLM 1230, 400Mhg */
HWS_DRAM_STATIC_CFG dunit_LU_SLM1236_400[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,        0x6,       0xa,        0x312},
    {1,        0,        0x6,       0x8,        0x111},
    {2,        0,        0x6,       0xa,        0x30f},
    {3,        0,        0x6,       0x8,        0x110},
    {4,        0,        0x6,       0xa,        0x311},
    {5,        0,        0x6,       0x8,        0x110},
    {6,        0,        0x6,       0xa,        0x312},
    {7,        0,        0x6,       0x8,        0x114},
    {8,        0,        0x6,       0xa,        0x315},
    {9,        0,        0x6,       0x8,        0x115},
    {10,       0,        0x6,       0xa,        0x312},
    {11,       0,        0x6,       0x8,        0x110},
    {12,       0,        0x6,       0xa,        0x310},
    {13,       0,        0x6,       0x8,        0x111}
};


/* static configuration data for dunit of LU IF, board SLM 1230, 800Mhg */
HWS_DRAM_STATIC_CFG dunit_LU_SLM1230_800[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,        0xD,       0xF,        0x113},
    {1,        0,        0xD,       0x11,       0x313},
    {2,        0,        0xD,       0xF,        0x10E},
    {3,        0,        0xD,       0x11,       0x30E},
    {4,        0,        0xD,       0xF,        0x10E},
    {5,        0,        0xD,       0x11,       0x30E},
    {6,        0,        0xD,       0xF,        0x115},
    {7,        0,        0xD,       0x11,       0x315},
    {8,        0,        0xD,       0xF,        0x10E},
    {9,        0,        0xD,       0x11,       0x30E},
    {10,       0,        0xD,       0xF,        0x106},
    {11,       0,        0xD,       0x11,       0x306},
    {12,       0,        0xD,       0xF,        0x102},
    {13,       0,        0xD,       0x11,       0x302}
};

/* static configuration data for dunit of FWD IF, board SLM 1230, 800Mhg */
HWS_DRAM_STATIC_CFG dunit_FWD_SLM1230_800[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,      0xD,         0xF,        0x100}, /* 0 */
    {0,        1,      0xD,         0xF,        0x100}, /* 0 */
    {1,        0,      0xD,         0x11,        0x21F}, /* 1 */
    {1,        1,      0xD,         0x11,        0x300}, /* 1 */
    {2,        0,      0xD,         0xF,        0x103}, /* 2 */
    {2,        1,      0xD,         0xF,        0x104}, /* 2 */
    {3,        0,      0xD,         0x11,        0x303}, /* 3 */
    {3,        1,      0xD,         0x11,        0x304}, /* 3 */
    {4,        0,      0xD,         0xF,        0x113},  /* 4 */
    {4,        1,      0xD,         0xF,        0x113},  /* 4 */
    {5,        0,      0xD,         0x11,        0x312},  /* 5 */
    {5,        1,      0xD,         0x11,        0x312},  /* 5 */
    {6,        0,      0xD,         0xF,        0x116},  /* 6 */
    {6,        1,      0xD,         0xF,        0x117},  /* 6 */
    {7,        0,      0xD,         0x11,        0x317},  /* 7 */
    {7,        1,      0xD,         0x11,        0x317},  /* 7 */
};

/* static configuration data for dunit of LU IF, board SLM 1236, 800Mhg */
HWS_DRAM_STATIC_CFG dunit_LU_SLM1236_800[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,        0xC,       0x10,        0x30B},
    {1,        0,        0xD,       0xF,        0x10B},
    {2,        0,        0xC,       0x10,        0x307},
    {3,        0,        0xD,       0xF,        0x107},
    {4,        0,        0xC,       0x10,        0x309},
    {5,        0,        0xD,       0xF,        0x108},
    {6,        0,        0xC,       0x10,        0x310},
    {7,        0,        0xD,       0xF,        0x10E},
    {8,        0,        0xC,       0x10,        0x312},
    {9,        0,        0xD,       0xF,        0x113},
    {10,       0,        0xC,       0x10,        0x30C},
    {11,       0,        0xD,       0xF,        0x10B},
    {12,       0,        0xC,       0x10,        0x306},
    {13,       0,        0xD,       0xF,        0x106}
};

/* static configuration data for dunit of FWD IF, board SLM 1236, 800Mhg */
HWS_DRAM_STATIC_CFG dunit_FWD_SLM1236_800[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,      0xC,         0x10,        0x307}, /* 0 */
    {0,        1,      0xC,         0x10,        0x305}, /* 0 */
    {1,        0,      0xD,         0xF,        0x106}, /* 1 */
    {1,        1,      0xD,         0xF,        0x105}, /* 1 */
    {2,        0,      0xC,         0x10,        0x310}, /* 2 */
    {2,        1,      0xC,         0x10,        0x30F}, /* 2 */
    {3,        0,      0xD,         0xF,        0x10F}, /* 3 */
    {3,        1,      0xD,         0xF,        0x10E}, /* 3 */
    {4,        0,      0xC,         0x10,        0x31D},  /* 4 */
    {4,        1,      0xC,         0x10,        0x31A},  /* 4 */
    {5,        0,      0xD,         0xF,        0x11A},  /* 5 */
    {5,        1,      0xD,         0xF,        0x118},  /* 5 */
    {6,        0,      0xC,         0x10,        0x314},  /* 6 */
    {6,        1,      0xC,         0x10,        0x311},  /* 6 */
    {7,        0,      0xD,         0xF,        0x112},  /* 7 */
    {7,        1,      0xD,         0xF,        0x112},  /* 7 */
};

/* static configuration data for dunit of LU IF, board SLM 1230, 667Mhg */
HWS_DRAM_STATIC_CFG dunit_LU_SLM1230_667[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,        0xB,       0xD,        0x15},
    {1,        0,        0xB,       0xF,        0x215},
    {2,        0,        0xB,       0xD,        0X11},
    {3,        0,        0xB,       0xF,        0x211},
    {4,        0,        0xB,       0xD,        0x11},
    {5,        0,        0xB,       0xF,        0x211},
    {6,        0,        0xB,       0xD,        0x17},
    {7,        0,        0xB,       0xF,        0x217},
    {8,        0,        0xB,       0xD,        0x10},
    {9,        0,        0xB,       0xF,        0x210},
    {10,       0,        0xB,       0xD,        0xA},
    {11,       0,        0xB,       0xF,        0x20A},
    {12,       0,        0xB,       0xD,        0x7},
    {13,       0,        0xB,       0xF,        0x207}
};

/* static configuration data for dunit of FWD IF, board SLM 1230, 667Mhg */
HWS_DRAM_STATIC_CFG dunit_FWD_SLM1230_667[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,      0xB,         0xD,        0x5}, /* 0 */
    {0,        1,      0xB,         0xD,        0x5}, /* 0 */
    {1,        0,      0xB,         0xF,        0x204}, /* 1 */
    {1,        1,      0xB,         0xF,        0x204}, /* 1 */
    {2,        0,      0xB,         0xD,        0x8}, /* 2 */
    {2,        1,      0xB,         0xD,        0x8}, /* 2 */
    {3,        0,      0xB,         0xF,        0x208}, /* 3 */
    {3,        1,      0xB,         0xF,        0x208}, /* 3 */
    {4,        0,      0xB,         0xD,        0x14},  /* 4 */
    {4,        1,      0xB,         0xD,        0x15},  /* 4 */
    {5,        0,      0xB,         0xF,        0x214},  /* 5 */
    {5,        1,      0xB,         0xF,        0x214},  /* 5 */
    {6,        0,      0xB,         0xD,        0x17},  /* 6 */
    {6,        1,      0xB,         0xD,        0x18},  /* 6 */
    {7,        0,      0xB,         0xF,        0x218},  /* 7 */
    {7,        1,      0xB,         0xF,        0x218},  /* 7 */
};

/* static configuration data for dunit of LU IF, board SLM 1236, 667Mhg */
HWS_DRAM_STATIC_CFG dunit_LU_SLM1236_667[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,        0xA,       0xE,        0x219},
    {1,        0,        0xB,       0xD,        0x19},
    {2,        0,        0xA,       0xE,        0x215},
    {3,        0,        0xB,       0xD,        0x15},
    {4,        0,        0xA,       0xE,        0x217},
    {5,        0,        0xB,       0xD,        0x16},
    {6,        0,        0xA,       0xE,        0x21D},
    {7,        0,        0xB,       0xD,        0x1B},
    {8,        0,        0xA,       0xE,        0x21F},
    {9,        0,        0xB,       0xD,        0x1F},
    {10,       0,        0xA,       0xE,        0x21A},
    {11,       0,        0xB,       0xD,        0x19},
    {12,       0,        0xA,       0xE,        0x215},
    {13,       0,        0xB,       0xD,        0x14}
};

/* static configuration data for dunit of FWD IF, board SLM 1236, 667Mhg */
HWS_DRAM_STATIC_CFG dunit_FWD_SLM1236_667[] =
{
    /* IF Num, pupNum, rdSampleDly, rdReadyDel, readLevelData */
    {0,        0,      0xA,         0xE,        0x215}, /* 0 */
    {0,        1,      0xA,         0xE,        0x214}, /* 0 */
    {1,        0,      0xB,         0xD,        0x14}, /* 1 */
    {1,        1,      0xB,         0xD,        0x13}, /* 1 */
    {2,        0,      0xA,         0xE,        0x21D}, /* 2 */
    {2,        1,      0xA,         0xE,        0x21C}, /* 2 */
    {3,        0,      0xB,         0xD,        0x1C}, /* 3 */
    {3,        1,      0xB,         0xD,        0x1B}, /* 3 */
    {4,        0,      0xA,         0xE,        0x308},  /* 4 */
    {4,        1,      0xA,         0xE,        0x305},  /* 4 */
    {5,        0,      0xB,         0xD,        0x105},  /* 5 */
    {5,        1,      0xB,         0xD,        0x104},  /* 5 */
    {6,        0,      0xA,         0xE,        0x300},  /* 6 */
    {6,        1,      0xA,         0xE,        0x21D},  /* 6 */
    {7,        0,      0xB,         0xD,        0x1F},  /* 7 */
    {7,        1,      0xB,         0xD,        0x1E},  /* 7 */
};

/* read leveling windows for all interface 0-13 (LU), 14-21(FWD) */
static GT_U32 ddr3RLWindowSize[22];

/*******************************************************************************
* wrHwsDdr3PhyRegAccessSet
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
static CMD_STATUS wrHwsDdr3PhyRegAccessSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;
    GT_U32     emcBaseAddr, mask;
    GT_BOOL    bcEn, memBc;
    GT_BOOL    isCntrlPup;


    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    MV_HWS_EMC_TYPE  memoryType   = (MV_HWS_EMC_TYPE)inArgs[1];
    GT_U32  memoryNumber = (GT_U32)inArgs[2];
    GT_U32  pupType      = (GT_U32)inArgs[3];
    GT_U32  pupNum       = (GT_U32)inArgs[4];
    GT_U32  regAddress   = (GT_U32)inArgs[5];

    /* Fields List */
    GT_U32  data = (GT_U32)inFields[0];

    isCntrlPup = (pupType == 1) ? GT_FALSE : GT_TRUE;
    switch (memoryType)
    {
    case MV_LOOKUP_EMC:
        emcBaseAddr = LU_EMC_BASE;
        mask = 0x3FFF;
        break;
    case MV_FORWARD_EMC:
        emcBaseAddr = FW_EMC_BASE;
        mask = 0xFF;
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "%f");
        return CMD_OK;
    }
    /* memory interface number config */
    memBc = (memoryNumber == 0xFFFF) ? GT_TRUE : GT_FALSE;
    mvDunitAccessCfg(globalDevNum,devNum, emcBaseAddr, memBc, mask, memoryNumber);

    bcEn = (pupNum == 0xFF) ? GT_TRUE : GT_FALSE;
    res = hwsDunitPhyRegWrite(globalDevNum,devNum,emcBaseAddr,pupNum,regAddress, bcEn, isCntrlPup, data);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, GT_FAIL, "%f");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3PhyRegAccessGetFirst
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
static CMD_STATUS wrHwsDdr3PhyRegAccessGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;
    GT_U32     emcBase;
    GT_BOOL    bcEn;
    GT_BOOL    isCntrlPup;


    /* Parameters List */
    GT_U32 devNum        = (GT_U32)inArgs[0];
    MV_HWS_EMC_TYPE  MemoryType   = (MV_HWS_EMC_TYPE)inArgs[1];
    GT_U32  MemoryNumber = (GT_U32)inArgs[2];
    GT_U32  PupType      = (GT_U32)inArgs[3];
    GT_U32  PupNum       = (GT_U32)inArgs[4];
    GT_U32  RegAddress   = (GT_U32)inArgs[5];

    /* Fields List */
    GT_U32  Data;

    isCntrlPup = (PupType == 1) ? GT_FALSE : GT_TRUE;
    switch (MemoryType)
    {
    case MV_LOOKUP_EMC:
        emcBase = 0x03000000;
        break;
    case MV_FORWARD_EMC:
        emcBase = 0x03200000;
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "%f");
        return CMD_OK;
    }
    if ((PupNum == 0xFF) || (MemoryNumber == 0xFFFF))
    {
        galtisOutput(outArgs, GT_NOT_SUPPORTED, "%f");
        return CMD_OK;
    }
    bcEn = GT_FALSE;
    res = hwsDunitPhyRegRead(globalDevNum,devNum,emcBase, MemoryNumber, PupNum,RegAddress, isCntrlPup, &Data);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, GT_FAIL, "%f");
        return CMD_OK;
    }

    /* Fields List */
    /* Note: You should un-comment this part and fill fields value */

    inFields[0] = (GT_32)Data;

    /* pack and output table fields */
    fieldOutput("%d", inFields[0]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3PhyRegAccessGetNext
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
static CMD_STATUS wrHwsDdr3PhyRegAccessGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    galtisOutput(outArgs, CMD_OK, "%d", -1);

    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3AdllControlSet
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
static CMD_STATUS wrHwsDdr3AdllControlSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32              devNum       = (GT_U32)inArgs[0];
    MV_HWS_EMC_TYPE     MemoryType   = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    GT_U32              MemoryNumber = (GT_U32)inArgs[2];
    GT_U32              PupNum       = (GT_U32)inArgs[3];

    /* Fields List */
    GT_U32  WR_DQ_Delay  = (GT_U32)inFields[2];
    GT_U32  RD_DQS_Delay = (GT_U32)inFields[3];
    GT_BOOL                 allMemIf = GT_FALSE;
    GT_BOOL                 allDataPups = GT_FALSE;

    if (MemoryNumber == 0xFFFF)
    {
        allMemIf = GT_TRUE;
    }
    if (PupNum == 0xFF)
    {
        allDataPups = GT_TRUE;
    }

    res = mvHwsDdr3AdllControlSet(globalDevNum,devNum, MemoryType, MemoryNumber, allMemIf, PupNum, allDataPups,WR_DQ_Delay,RD_DQS_Delay);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, GT_FAIL, "%f");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3AdllControlGetFirst
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
static CMD_STATUS wrHwsDdr3AdllControlGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;

    /* Parameters List */
    GT_U32             devNum       = (GT_U32)inArgs[0];
    MV_HWS_EMC_TYPE     MemoryType   = (MV_HWS_EMC_TYPE)inArgs[1];
    GT_U32              MemoryNumber = (GT_U32)inArgs[2];

    /* Fields List */
    GT_U32  WR_DQ_Delay;
    GT_U32  RD_DQS_Delay;

    res = mvHwsDdr3AdllControlGet(globalDevNum, devNum, MemoryType, MemoryNumber, 0, &WR_DQ_Delay, &RD_DQS_Delay);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, GT_FAIL, "%f");
        return CMD_OK;
    }

    /* Fields List */
    inFields[0] = (GT_32)MemoryNumber;
    inFields[1] = (GT_32)0;
    inFields[2] = (GT_32)WR_DQ_Delay;
    inFields[3] = (GT_32)RD_DQS_Delay;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3AdllControlGetNext
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
static CMD_STATUS wrHwsDdr3AdllControlGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  res;
    /* Parameters List */
    GT_U32              devNum       = (GT_U32)inArgs[0];
    MV_HWS_EMC_TYPE     MemoryType   = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    GT_U32              MemoryNumber = (GT_U32)inArgs[2];

    /* Fields List */
    GT_U32  WR_DQ_Delay;
    GT_U32  RD_DQS_Delay;

    if (MemoryType != MV_FORWARD_EMC)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    res = mvHwsDdr3AdllControlGet(globalDevNum, devNum, MemoryType, MemoryNumber, 1, &WR_DQ_Delay, &RD_DQS_Delay);
    if (res != GT_OK)
    {
        galtisOutput(outArgs, GT_FAIL, "%f");
        return CMD_OK;
    }

    /* Fields List */
    inFields[0] = (GT_32)MemoryNumber;
    inFields[1] = (GT_32)1;
    inFields[2] = (GT_32)WR_DQ_Delay;
    inFields[3] = (GT_32)RD_DQS_Delay;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

static GT_U32 currrentLine;     /* table line */

/*******************************************************************************
* wrHwsDdr3ExternalMemAccessSet
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
static CMD_STATUS wrHwsDdr3ExternalMemAccessSet
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32    accessSize, j;
    GT_U32    emcBaseAddr, mask;
    GT_BOOL   memBc;

    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    GT_U32  memType      = (GT_U32)inArgs[1];
    GT_U32  memoryNumber = (GT_U32)inArgs[2];
    GT_U32  bank         = (GT_U32)inArgs[3];
    /*GT_U32  size         = (GT_U32)inArgs[5];*/ /* in lines */
    GT_U32  burstType    = (GT_U32)inArgs[6];

    /* Fields List */
    GT_U32 baseAddress  = (GT_U32)inFields[0];
    GT_U32 wordArr[5];

    wordArr[0]  = (GT_U32)inFields[1];
    wordArr[1]  = (GT_U32)inFields[2];
    wordArr[2]  = (GT_U32)inFields[3];
    wordArr[3]  = (GT_U32)inFields[4];
    wordArr[4]  = (GT_U32)inFields[5];

    if (memType)
    {
        /* lookup memory (narrow) */
        accessSize = 2;
        emcBaseAddr = LU_EMC_BASE;
        mask = 0x3FFF;
        /* burst size config */
        genRegisterSet(globalDevNum,devNum, emcBaseAddr + MG_ACCESS_CONTROL_1,(burstType << 21), (1 << 21));
    }
    else
    {
        /* forward memory (wide) */
        emcBaseAddr = FW_EMC_BASE;
        accessSize = 5;
        mask = 0xFF;
    }
    /* memory interface number config */
    memBc = (memoryNumber == 0xFFFF) ? GT_TRUE : GT_FALSE;
    mvDunitAccessCfg(globalDevNum,devNum, emcBaseAddr, memBc, mask, memoryNumber);

    /* write bank number */
    genRegisterSet(globalDevNum,devNum, emcBaseAddr + MG_ACCESS_CONTROL_2,((1 << bank) << 24), 0xFF000000);

    for (j = 0; j < accessSize; j++)
    {
        genRegisterSet(globalDevNum,devNum, baseAddress + j*4, wordArr[j], 0);
    }
    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3ExternalMemAccessGetFirst
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
static CMD_STATUS wrHwsDdr3ExternalMemAccessGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_U32    i, accessSize, addrInd;
    GT_U32    emcBaseAddr;

    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    GT_U32  memType      = (GT_U32)inArgs[1];
    GT_U32  memoryNumber = (GT_U32)inArgs[2];
    GT_U32  bank         = (GT_U32)inArgs[3];
    GT_U32  baseAddress  = (GT_U32)inArgs[4];
    /*GT_U32  size         = (GT_U32)inArgs[5];*/
    GT_U32  burstType    = (GT_U32)inArgs[6];
    /* Fields List */
    GT_U32 wordArr[5];

    cpssOsMemSet(wordArr, 0, sizeof(wordArr));

    if (memType)
    {
        accessSize = 2;
        addrInd = 0x10;
        emcBaseAddr = LU_EMC_BASE;
        /* birst size config */
        genRegisterSet(globalDevNum,devNum, emcBaseAddr + MG_ACCESS_CONTROL_1,(burstType << 21), (1 << 21));
    }
    else
    {
        accessSize = 5;
        addrInd = 0x20;
        emcBaseAddr = FW_EMC_BASE;
    }
    genRegisterSet(globalDevNum,devNum, emcBaseAddr + MG_ACCESS_CONTROL_1,(bank << 4) + memoryNumber, 0x7F);

    for (i = 0; i < accessSize; i++)
    {
        genRegisterGet(globalDevNum,devNum, baseAddress + i*4, &wordArr[i], 0);
    }

    currrentLine = 1;
    inFields[0] = baseAddress;
    if (memType)
    {
        /* lookup memory */
        wordArr[1] = wordArr[1] & 0xFF;
    }
    else
    {
        wordArr[4] = wordArr[4] & 0xFFFF;
    }
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], wordArr[0], wordArr[1], wordArr[2], wordArr[3], wordArr[4]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3ExternalMemAccessGetNext
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
static CMD_STATUS wrHwsDdr3ExternalMemAccessGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32    i, accessSize, addrInd;
    GT_U32    emcBaseAddr;

    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    GT_U32  memType      = (GT_U32)inArgs[1];
    GT_U32  baseAddress  = (GT_U32)inArgs[4];
    GT_U32  size         = (GT_U32)inArgs[5];
    /* Fields List */
    GT_U32 wordArr[5];

    if (currrentLine >= size)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    cpssOsMemSet(wordArr, 0, sizeof(wordArr));

    if (memType)
    {
        accessSize = 2;
        addrInd = 0x10;
        emcBaseAddr = LU_EMC_BASE;
    }
    else
    {
        accessSize = 5;
        addrInd = 0x20;
        emcBaseAddr = FW_EMC_BASE;
    }

    for (i = 0; i < accessSize; i++)
    {
        genRegisterGet(globalDevNum,devNum, baseAddress + currrentLine*addrInd + i*4, &wordArr[i], 0);
    }

    inFields[0] = baseAddress + currrentLine*addrInd;
    currrentLine++;
    if (memType)
    {
        /* lookup memory */
        wordArr[1] = wordArr[1] & 0xFF;
    }
    else
    {
        wordArr[4] = wordArr[4] & 0xFFFF;
    }
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], wordArr[0], wordArr[1], wordArr[2], wordArr[3], wordArr[4]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

GT_U32 enAutoPrecharge = 1;
GT_STATUS hwsDdr3IfcLoadPattern
(
    GT_U8           devNum,
    MV_HWS_EMC_TYPE memType,
    GT_U32          patternType,
    GT_U32          sramEntriesNum
)
{
    GT_U32 emcBaseAddr;
    GT_U32 patterns[2];
    GT_U32 patternMask;
    GT_U32 startBit[2];
    GT_U32 data;
    GT_U32 i;

    switch (memType)
    {
    case MV_LOOKUP_EMC:
        /* lookup memory (narrow) */
        emcBaseAddr = LU_EMC_BASE;
        patternMask = 0x1F;
        startBit[0] = 11;
        startBit[1] = 27;
        break;
    case MV_FORWARD_EMC:
        /* forward memory (wide) */
        emcBaseAddr = FW_EMC_BASE;
        patternMask = 0x1FF;
        startBit[0] = 7;
        startBit[1] = 23;
        break;
    default:
        return GT_BAD_PARAM;
    }
    for (i = 0; i < 2; i++)
    {
        patterns[i] = ((hwsDdr5APattern[i*2] & patternMask) << startBit[0]) +
                      ((hwsDdr5APattern[i*2 + 1] & patternMask) << startBit[1]) ;
    }
    /* Cancel or not AutoPrecharge */
    data = (enAutoPrecharge) ? 2 : 0;
    genRegisterSet(globalDevNum,devNum, emcBaseAddr + SDRAM_OPEN_PAGES_CONTROL, data, 2);
    /* Initialize ODPG pattern memory */
    genRegisterSet(globalDevNum,devNum, emcBaseAddr + DUNIT_ODPG_DATA_CONTROL, 1,1);

    genRegisterSet(globalDevNum,devNum, emcBaseAddr + DUNIT_ODPG_DATA_WR_HIGH, patterns[1],0);
    genRegisterSet(globalDevNum,devNum, emcBaseAddr + DUNIT_ODPG_DATA_WR_LOW,  patterns[0],0);

    /* check write error */
    genRegisterGet(globalDevNum,devNum, emcBaseAddr + DUNIT_ODPG_DATA_WR_ERROR,  &data, 0);
    if (data & 1)
    {
        for (i = 0; i < sramEntriesNum; i++)
        {
            genRegisterSet(globalDevNum,devNum, emcBaseAddr + DUNIT_ODPG_DATA_WR_ADDR, i,0x3F);
        }
    }
    else
    {
        return GT_FAIL;
    }

    return GT_OK;
}

GT_STATUS hwsDdr3IfcBistSetup
(
    GT_U8           devNum,
    GT_U32          emcBaseAddr,
    GT_U32          stopCondition,
    GT_BOOL         txRun,
    GT_U32          patternNum,
    GT_U32          sramEntriesNum,
    GT_U32          cbSize
)
{
    GT_U32 data;

    /* register values */
    cbSize--;
    patternNum--;

    if (txRun)
    {
        /* run TX */
        data = (cbSize << 11) + (2 << 15) + (cbSize << 21) + (1 << 25);
    }
    else
    {
        /* run RX */
        data = (1 << 1) + (cbSize << 21);
    }

    /* ODPG DATA Control */
    data += ((stopCondition & 7) << 2) + (((sramEntriesNum-1) & 0x3f) << 5);
    genRegisterSet(globalDevNum,devNum, emcBaseAddr + DUNIT_ODPG_DATA_CONTROL, data,0xFFFFFFE);

    /* ODPG DATA Phase Length, Relevant only if stop condition is N patterns */
    if (stopCondition == 3)
    {
        genRegisterSet(globalDevNum,devNum, emcBaseAddr + DUNIT_ODPG_PHASE_LENGTH, patternNum,0);
    }
    return GT_OK;
}

/*******************************************************************************
* wrHwsDdr3IfcBistCfg
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
static CMD_STATUS wrHwsDdr3IfcBistCfg
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32    emcBaseAddr, mask;
    GT_BOOL   memBc;

    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    MV_HWS_EMC_TYPE  memType = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    GT_U32  memoryNumber = (GT_U32)inArgs[2];
    GT_U32  patternType  = (GT_U32)inArgs[3];
    GT_U32  patternEntNum = (GT_U32)inArgs[4];
    GT_U32  stopCondition = (GT_U32)inArgs[5];
    GT_U32  nPatternNum = (GT_U32)inArgs[6];
    /* Stop condition:
    0x0 – Single pattern
    0x1 – First error (relevant only for Rx test)
    0x2 - Predefined address space
    0x3 – N patterns
    0x4 – Continuous */

    GT_U32  operMode = (GT_U32)inArgs[7]; /* tx or rx, or both */
    GT_U32  cbSize = (GT_U32)inArgs[9];   /* command burst size */

    /* not in used yet */
    /*GT_U32  offset = (GT_U32)inArgs[10];*/

    GT_U32  i, memStart, memStop;
    GT_U32  address;
    GT_BOOL runTx;

    /* burst size optional inputs are 2,4 & 6 */
    if ((cbSize != 2) && (cbSize != 4) && (cbSize != 6))
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }
    /* NPatternNum should always be CBSize x integer */
    if ((nPatternNum % cbSize) && (patternEntNum % cbSize))
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }
    switch (memType)
    {
    case MV_LOOKUP_EMC:
        /* lookup memory (narrow) */
        emcBaseAddr = LU_EMC_BASE;
        mask = 0x3FFF;
        memStop = 14;
        break;
    case MV_FORWARD_EMC:
        /* forward memory (wide) */
        emcBaseAddr = FW_EMC_BASE;
        mask = 0xFF;
        memStop = 8;
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }

    /* memory interface number config */
    if (memoryNumber == 0xFFFF)
    {
        memStart = 0;
    }
    else
    {
        memStop = memoryNumber+1;
        memStart = memoryNumber;
    }
    /* memory interface number config */
    memBc = (memoryNumber == 0xFFFF) ? GT_TRUE : GT_FALSE;
    mvDunitAccessCfg(globalDevNum,devNum, emcBaseAddr, memBc, mask, memoryNumber);

    runTx = (operMode == 1) ? GT_TRUE : GT_FALSE;

    /* load pattern */
    hwsDdr3IfcLoadPattern(globalDevNum, memType, patternType, patternEntNum);
    /* test setup */
    if (stopCondition == 3)
    {
        /* N pattern Run Tx */
        hwsDdr3IfcBistSetup(globalDevNum, emcBaseAddr, stopCondition, GT_TRUE, nPatternNum, patternEntNum, cbSize);
        /* trigger test for all choosen memories */
        for (i = memStart; i < memStop; i++)
        {
            address = emcBaseAddr + DRAM_INITIALIZATION_CONTROL_AND_STATUS + 0x4*i;
            genRegisterSet(globalDevNum,devNum, address, (1 << 17), (1 << 17));
        }
        cpssOsTimerWkAfter(10);
        /* stop test */
        for (i = memStart; i < memStop; i++)
        {
            address = emcBaseAddr + DRAM_INITIALIZATION_CONTROL_AND_STATUS + 0x4*i;
            genRegisterSet(globalDevNum,devNum, address, (1 << 18), (1 << 18));
            genRegisterSet(globalDevNum,devNum, address, 0, (3 << 17));
        }
        /* N pattern Run Rx */
        hwsDdr3IfcBistSetup(globalDevNum, emcBaseAddr, stopCondition, GT_FALSE, nPatternNum, patternEntNum, cbSize);
        /* trigger test for all choosen memories */
        for (i = memStart; i < memStop; i++)
        {
            address = emcBaseAddr + DRAM_INITIALIZATION_CONTROL_AND_STATUS + 0x4*i;
            genRegisterSet(globalDevNum,devNum, address, (1 << 17), (1 << 17));
        }
    }
    else
    {
        hwsDdr3IfcBistSetup(globalDevNum, emcBaseAddr, stopCondition, runTx, nPatternNum, patternEntNum, cbSize);
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

static GT_U32 glCurrentMem;
/*******************************************************************************
* wrHwsDdr3IfcBistReadStatusGetFirst
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
static CMD_STATUS wrHwsDdr3IfcBistReadStatusGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32  emcBaseAddr, mask;
    GT_U32  i, memStart, memStop;
    GT_U32  address;
    GT_BOOL memBc;

    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    MV_HWS_EMC_TYPE  memoryType   = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    GT_U32  memoryNumber = (GT_U32)inArgs[2];

    /* Fields List */
    GT_U32  data;

    glCurrentMem = 0;
    switch (memoryType)
    {
    case MV_LOOKUP_EMC:
        /* lookup memory (narrow) */
        emcBaseAddr = LU_EMC_BASE;
        mask = 0x3FFF;
        memStop = 14;
        break;
    case MV_FORWARD_EMC:
        /* forward memory (wide) */
        emcBaseAddr = FW_EMC_BASE;
        mask = 0xFF;
        memStop = 8;
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "%f");
        return CMD_OK;
    }
    /* memory interface number config */
    if (memoryNumber == 0xFFFF)
    {
        memStart = 0;
    }
    else
    {
        memStop = memoryNumber+1;
        memStart = memoryNumber;
    }
    /* memory interface number config */
    memBc = (memoryNumber == 0xFFFF) ? GT_TRUE : GT_FALSE;
    mvDunitAccessCfg(globalDevNum,devNum, emcBaseAddr, memBc, mask, memoryNumber);
    /* stop test */
    for (i = memStart; i < memStop; i++)
    {
        address = emcBaseAddr + DRAM_INITIALIZATION_CONTROL_AND_STATUS + 0x4*i;
        genRegisterSet(globalDevNum,devNum, address, (1 << 18), (1 << 18));
        genRegisterSet(globalDevNum,devNum, address, 0, (3 << 17));
    }
    /* Read test status in bit 19; not relevant for Puma3A */
    address = emcBaseAddr + DRAM_INITIALIZATION_CONTROL_AND_STATUS + 0x4*memStart;
    genRegisterGet(globalDevNum,devNum, address, &data, 0);

    genRegisterSet(globalDevNum,devNum, emcBaseAddr + MG_ACCESS_CONTROL_1, memoryNumber, 0xF);
    /* read error on first interfaces */
    genRegisterGet(globalDevNum,devNum, emcBaseAddr + 0x16C0, &data, 0);

    inFields[0] = (GT_32)memoryNumber;
    inFields[1] = 1;
    inFields[2] = (GT_32)data;
    /* read last Failed address on first interfaces */
    genRegisterGet(globalDevNum,devNum, emcBaseAddr + 0x16BC, &data, 0);
    inFields[3] = (GT_32)data;
    /* read Last pattern failed high data on first interfaces */
    genRegisterGet(globalDevNum,devNum, emcBaseAddr + 0x16C4, &data, 0);
    inFields[4] = (GT_32)data;
    /* read Last pattern failed low data on first interfaces */
    genRegisterGet(globalDevNum,devNum, emcBaseAddr + 0x16C8, &data, 0);
    inFields[5] = (GT_32)data;

    glCurrentMem++;
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0],inFields[1],inFields[2],inFields[3],inFields[4],inFields[5]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3IfcBistReadStatusGetNext
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
static CMD_STATUS wrHwsDdr3IfcBistReadStatusGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32  emcBaseAddr;
    GT_U32  memStop;

    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    MV_HWS_EMC_TYPE  memoryType   = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    GT_U32  memoryNumber = (GT_U32)inArgs[2];

    /* Fields List */
    GT_U32  data;

    if (memoryNumber != 0xFFFF)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    switch (memoryType)
    {
    case MV_LOOKUP_EMC:
        /* lookup memory (narrow) */
        emcBaseAddr = LU_EMC_BASE;
        memStop = 14;
        break;
    case MV_FORWARD_EMC:
        /* forward memory (wide) */
        emcBaseAddr = FW_EMC_BASE;
        memStop = 8;
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "%f");
        return CMD_OK;
    }

    if (glCurrentMem >= memStop)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    genRegisterSet(globalDevNum,devNum, emcBaseAddr + MG_ACCESS_CONTROL_1, glCurrentMem, 0xF);
    /* read error on next interfaces */
    genRegisterGet(globalDevNum,devNum, emcBaseAddr + 0x16C0, &data, 0);

    inFields[0] = (GT_32)glCurrentMem;
    inFields[1] = 1;
    inFields[2] = (GT_32)data;
    /* read last Failed address on next interfaces */
    genRegisterGet(globalDevNum,devNum, emcBaseAddr + 0x16BC, &data, 0);
    inFields[3] = (GT_32)data;
    /* read Last pattern failed high data on next interfaces */
    genRegisterGet(globalDevNum,devNum, emcBaseAddr + 0x16C4, &data, 0);
    inFields[4] = (GT_32)data;
    /* read Last pattern failed low data on next interfaces */
    genRegisterGet(globalDevNum,devNum, emcBaseAddr + 0x16C8, &data, 0);
    inFields[5] = (GT_32)data;

    glCurrentMem++;
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0],inFields[1],inFields[2],inFields[3],inFields[4],inFields[5]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3PupReset
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
static CMD_STATUS wrHwsDdr3PupReset
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32    emcBaseAddr, mask;
    GT_BOOL   memBc;

    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    GT_U32  memType      = (GT_U32)inArgs[1];
    GT_U32  memoryNumber = (GT_U32)inArgs[2];

    if (memType)
    {
        /* lookup memory (narrow) */
        emcBaseAddr = LU_EMC_BASE;
        mask = 0x3FFF;
    }
    else
    {
        /* forward memory (wide) */
        emcBaseAddr = FW_EMC_BASE;
        mask = 0xFF;
    }

    /* memory interface number config */
    memBc = (memoryNumber == 0xFFFF) ? GT_TRUE : GT_FALSE;
    mvDunitAccessCfg(globalDevNum,devNum, emcBaseAddr, memBc, mask, memoryNumber);

    genRegisterSet(globalDevNum,devNum, emcBaseAddr + SDRAM_CONFIGURATION, 0, (3 << 29));
    genRegisterSet(globalDevNum,devNum, emcBaseAddr + SDRAM_CONFIGURATION, (3 << 29), (3 << 29));

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3InitConfiguration
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
static CMD_STATUS wrHwsDdr3InitConfiguration
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32  devNum = (GT_U32)inArgs[0];
    GT_U32  freq  = (GT_U32)inArgs[2];
    MV_HWS_EMC_TYPE  memType = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    MV_HWS_CWL  casWrL = (MV_HWS_CWL)inArgs[3];
    MV_HWS_CAS_L  casL = (MV_HWS_CAS_L)(inArgs[4]+1);
    GT_STATUS rc;
    MV_HWS_TARGET_FREQ targetFreq;
    MV_HWS_MEMORY_CFG_INFO memCfg;
    GT_U32 emcBase;

    emcBase = (memType == MV_FORWARD_EMC) ? FW_EMC_BASE : LU_EMC_BASE;

    if (freq <= 667)
    {
        targetFreq = FREQ_667;
        casWrL = CWL7;
        casL = CL_9;
    }
    else
    {
        targetFreq = FREQ_800;
        casWrL = CWL8;
        casL = CL_11;
    }

    memCfg.casL = casL;
    memCfg.casWL = casWrL;
    memCfg.size = DDR3_2Gb;

    rc = mvHwsDdr3Puma3Init(globalDevNum,devNum,memType,memCfg,targetFreq);

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3ReadLevelingStaticCfg
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
static CMD_STATUS wrHwsDdr3ReadLevelingStaticCfg
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32  devNum = (GT_U32)inArgs[0];
    GT_U32  freq   = (GT_U32)inArgs[2];
    MV_HWS_EMC_TYPE  memType = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    MV_DDR3_BOARD_TYPE board = (inArgs[3] == 1) ? SLM_1236 : SLM_1230;
    MV_HWS_TARGET_FREQ targetFreq;
    HWS_DRAM_STATIC_CFG *staticParams;
    GT_U32 numOfCfg;
    GT_STATUS rc;

    if (freq <= 667)
    {
        targetFreq = FREQ_667;
    }
    else
    {
        targetFreq = FREQ_800;
    }


    switch (memType)
    {
    case MV_LOOKUP_EMC:
        switch(board)
        {
        case SLM_1230:
            switch(targetFreq)
            {
            case FREQ_667:
                numOfCfg = sizeof(dunit_LU_SLM1230_667)/ sizeof(HWS_DRAM_STATIC_CFG);
                staticParams = dunit_LU_SLM1230_667;
                break;
            case FREQ_800:
                numOfCfg = sizeof(dunit_LU_SLM1230_800)/ sizeof(HWS_DRAM_STATIC_CFG);
                staticParams = dunit_LU_SLM1230_800;
                break;
            default:
                galtisOutput(outArgs, GT_NOT_SUPPORTED, "%f");
                return CMD_OK;
            }
            break;
        case SLM_1236:
            switch(targetFreq)
            {
            case FREQ_667:
                numOfCfg = sizeof(dunit_LU_SLM1236_667)/ sizeof(HWS_DRAM_STATIC_CFG);
                staticParams = dunit_LU_SLM1236_667;
                break;
            case FREQ_800:
                numOfCfg = sizeof(dunit_LU_SLM1236_800)/ sizeof(HWS_DRAM_STATIC_CFG);
                staticParams = dunit_LU_SLM1236_800;
                break;
            default:
                galtisOutput(outArgs, GT_NOT_SUPPORTED, "%f");
                return CMD_OK;
            }
            break;
        default:
            galtisOutput(outArgs, GT_NOT_SUPPORTED, "%f");
            return CMD_OK;
        }
        break;
    case MV_FORWARD_EMC:
        switch(board)
        {
        case SLM_1230:
            switch(targetFreq)
            {
            case FREQ_667:
                numOfCfg = sizeof(dunit_FWD_SLM1230_667)/ sizeof(HWS_DRAM_STATIC_CFG);
                staticParams = dunit_FWD_SLM1230_667;
                break;
            case FREQ_800:
                numOfCfg = sizeof(dunit_FWD_SLM1230_800)/ sizeof(HWS_DRAM_STATIC_CFG);
                staticParams = dunit_FWD_SLM1230_800;
                break;
            default:
                galtisOutput(outArgs, GT_NOT_SUPPORTED, "%f");
                return CMD_OK;
            }
            break;
        case SLM_1236:
            switch(targetFreq)
            {
            case FREQ_667:
                numOfCfg = sizeof(dunit_FWD_SLM1236_667)/ sizeof(HWS_DRAM_STATIC_CFG);
                staticParams = dunit_FWD_SLM1236_667;
                break;
            case FREQ_800:
                numOfCfg = sizeof(dunit_FWD_SLM1236_800)/ sizeof(HWS_DRAM_STATIC_CFG);
                staticParams = dunit_FWD_SLM1236_800;
                break;
            default:
                galtisOutput(outArgs, GT_NOT_SUPPORTED, "%f");
                return CMD_OK;
            }
            break;
        default:
            galtisOutput(outArgs, GT_NOT_SUPPORTED, "%f");
            return CMD_OK;
        }
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "%f");
        return CMD_OK;
    }

    rc = mvHwsDdr3StaticCfg(globalDevNum,devNum,memType,staticParams, numOfCfg);

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3ChangeFreq
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
static CMD_STATUS wrHwsDdr3ChangeFreq
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32  devNum = (GT_U32)inArgs[0];
    GT_U32  freq   = (GT_U32)inArgs[2];
    MV_HWS_EMC_TYPE  memType = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    GT_BOOL dllOff = (inArgs[3] == 1) ? GT_TRUE : GT_FALSE;
    GT_BOOL swTune = (inArgs[4] == 1) ? GT_TRUE : GT_FALSE;
    GT_STATUS rc;

    switch (freq)
    {
    case 0: /* low */
        rc = mvHwsDdr3SlowFreqSetup(globalDevNum,devNum,memType,dllOff,swTune);
        break;
    case 1: /* medium */
        rc = mvHwsDdr3MedFreqSetup(globalDevNum,devNum,memType,dllOff,swTune);
        break;
    case 2: /* high */
        rc = mvHwsDdr3HighFreqSetup(globalDevNum,devNum,memType,dllOff,swTune);
        break;
    default:
        galtisOutput(outArgs, GT_NOT_SUPPORTED, "%f");
        return CMD_OK;
    }

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3ReadLeveling
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
static CMD_STATUS wrHwsDdr3ReadLeveling
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32     j;
    GT_U32    numOfIf;
    GT_U32    typeBase;
    GT_STATUS rc = GT_OK;

    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    GT_U32  memoryNumber = (GT_U32)inArgs[2];
    GT_U32  rdReadyDel   = (GT_U32)inArgs[3];
    GT_U32  rdSmplDel    = (GT_U32)inArgs[4];
    GT_U32  phases       = (GT_U32)inArgs[5];
    GT_U32  aDll         = (GT_U32)inArgs[6];
    MV_HWS_EMC_TYPE  memType = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;

    numOfIf = 1;
    typeBase = (memType == MV_FORWARD_EMC) ? 14 : 0;
    if (memoryNumber == 0xFFFF)
    {
        /* run RL on all interfaces */
        switch (memType)
        {
        case MV_FORWARD_EMC:
            numOfIf = 8;
            memoryNumber = 0;
            break;
        case MV_LOOKUP_EMC:
            numOfIf = 14;
            memoryNumber = 0;
            break;
        default:
            galtisOutput(outArgs, GT_BAD_PARAM, "%f");
            return CMD_OK;
        }
    }

    for (j = memoryNumber; j < memoryNumber + numOfIf; j++)
    {
        rc = mvHwsDdr3SwReadLeveling(globalDevNum,devNum,memType,j,rdReadyDel, rdSmplDel,
                                     phases,aDll, &ddr3RLWindowSize[j + typeBase]);
    }

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

static  GT_U32  rlMemoryNumber;
/*******************************************************************************
* wrHwsDdr3ReadLevelingGetFirst
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
static CMD_STATUS wrHwsDdr3ReadLevelingGetFirst
    (
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32    numOfIf;
    GT_U32    typeBase;

    /* Parameters List */
    GT_U32  memoryNumber = (GT_U32)inArgs[2];
    MV_HWS_EMC_TYPE  memType = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;

    switch (memType)
    {
    case MV_FORWARD_EMC:
        numOfIf = 8;
        break;
    case MV_LOOKUP_EMC:
        numOfIf = 14;
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "%f");
        return CMD_OK;
    }
    if ((memoryNumber != 0xFFFF) && (memoryNumber >= numOfIf))
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    typeBase = (memType == MV_FORWARD_EMC) ? 14 : 0;
    if (memoryNumber == 0xFFFF)
    {
        memoryNumber = 0;
    }
    else if (memoryNumber >= numOfIf)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }


    inFields[0] = memoryNumber;
    inFields[1] = ddr3RLWindowSize[memoryNumber + typeBase];
    rlMemoryNumber = memoryNumber;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3ReadLevelingGetNext
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
static CMD_STATUS wrHwsDdr3ReadLevelingGetNext
    (
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32    numOfIf;
    GT_U32    typeBase;

    /* Parameters List */
    GT_U32  memoryNumber = (GT_U32)inArgs[2];
    MV_HWS_EMC_TYPE  memType = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;

    if (memoryNumber != 0xFFFF)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    typeBase = (memType == MV_FORWARD_EMC) ? 14 : 0;
    /* run RL on all interfaces */
    switch (memType)
    {
    case MV_FORWARD_EMC:
        numOfIf = 8;
        memoryNumber = 0;
        break;
    case MV_LOOKUP_EMC:
        numOfIf = 14;
        memoryNumber = 0;
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "%f");
        return CMD_OK;
    }
    rlMemoryNumber++;

    if (rlMemoryNumber >= numOfIf)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = rlMemoryNumber;
    inFields[1] = ddr3RLWindowSize[rlMemoryNumber + typeBase];

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3LoadPatterns
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
static CMD_STATUS wrHwsDdr3LoadPatterns
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc = GT_OK;
    GT_BOOL   memBc;    /* memory broadcast access */

    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    GT_U32  memoryNumber = (GT_U32)inArgs[2];
    GT_U32  accessMode   = (GT_U32)inArgs[3];
    GT_U32  pattern      = (GT_U32)inArgs[4];
    MV_HWS_EMC_TYPE  memType = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    HWS_DDR3_SDRAM_PATTERNS cfgPattern;

    if (accessMode != 0)
    {
        galtisOutput(outArgs, GT_NOT_SUPPORTED, "%f");
        return CMD_OK;
    }
    switch (pattern)
    {
    case 0:
        cfgPattern = DDR3_LAST_PATTERN;
        break;
    case 1:
        cfgPattern = DDR3_READ_LEVELING_PATTERN;
        break;
    case 2:
        cfgPattern = DDR3_PBS_PATTERN;
        break;
    case 3:
        cfgPattern = DDR3_DQS_BASIC_PATTERN;
        break;
    case 4:
        cfgPattern = DDR3_DQS_ADVANCE_PATTERN;
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "%f");
        return CMD_OK;
    }

    /* memory interface number config */
    memBc = (memoryNumber == 0xFFFF) ? GT_TRUE : GT_FALSE;

    rc = ddr3SdramWriteAllPatterns(globalDevNum,devNum, memType, memoryNumber, memBc, cfgPattern);
    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}


/*******************************************************************************
* wrHwsDdr3DqsCentralization
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
static CMD_STATUS wrHwsDdr3DqsCentralization
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;

    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    GT_U32  memoryNumber = (GT_U32)inArgs[2];
    MV_HWS_EMC_TYPE  memType = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    MV_DDR3_DIRECTION ddr3RxTx = (inArgs[3] == 1) ? MV_DDR3_RX : MV_DDR3_TX;

    rc = ddr3DqsIfCentralization(globalDevNum,devNum, memType, memoryNumber, ddr3RxTx);

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

static  GT_U32  dqsPupNumber;
/*******************************************************************************
* wrHwsDdr3DqsCentralizationGetFirst
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
static CMD_STATUS wrHwsDdr3DqsCentralizationGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    ddr3DqsCentralizationResult(0, (GT_32 *)&inFields[1], (GT_32 *)&inFields[2]);

    inFields[0] = 0; /* first pup number */
    dqsPupNumber = 0;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3DqsCentralizationGetNext
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
static CMD_STATUS wrHwsDdr3DqsCentralizationGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* Parameters List */
    MV_HWS_EMC_TYPE  memType = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    GT_U32 maxPupNum;

    dqsPupNumber++;
    switch (memType)
    {
    case MV_FORWARD_EMC:
        maxPupNum = 2;
        break;
    case MV_LOOKUP_EMC:
        maxPupNum = 0;
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "%f");
        return CMD_OK;
    }
    if (dqsPupNumber >= maxPupNum)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    ddr3DqsCentralizationResult(dqsPupNumber, (GT_32 *)(&inFields[1]), (GT_32 *)(&inFields[2]));

    inFields[0] = dqsPupNumber; /* next pup number */

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3PbsCalc
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
static CMD_STATUS wrHwsDdr3PbsCalc
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS rc;

    /* Parameters List */
    GT_U32  devNum       = (GT_U32)inArgs[0];
    GT_U32  memoryNumber = (GT_U32)inArgs[2];
    MV_HWS_EMC_TYPE  memType = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    MV_DDR3_DIRECTION ddr3RxTx = (inArgs[3] == 1) ? MV_DDR3_RX : MV_DDR3_TX;

    if (ddr3RxTx == MV_DDR3_RX)
    {
        rc = ddr3PbsRx(globalDevNum,devNum,memType,memoryNumber);
    }
    else
    {
        rc = ddr3PbsTx(globalDevNum,devNum,memType,memoryNumber);
    }

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

static  GT_U32  pbsPupNumber;
static  GT_U32  pbsDqNumber;
/*******************************************************************************
* wrHwsDdr3PbsResultGetFirst
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
static CMD_STATUS wrHwsDdr3PbsResultGetFirst
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* Parameters List */

    ddr3PbsCalcResult(0, 0, (GT_32 *)(&inFields[2]));

    inFields[0] = 0; /* first pup number */
    inFields[1] = 0; /* DQ number */
    pbsPupNumber = 0;
    pbsDqNumber = 0;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDdr3PbsResultGetNext
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
static CMD_STATUS wrHwsDdr3PbsResultGetNext
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* Parameters List */
    MV_HWS_EMC_TYPE  memType = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    GT_U32 maxPupNum;

    pbsDqNumber++;

    switch (memType)
    {
    case MV_FORWARD_EMC:
        maxPupNum = 2;
        break;
    case MV_LOOKUP_EMC:
        maxPupNum = 0;
        break;
    default:
        galtisOutput(outArgs, GT_BAD_PARAM, "%f");
        return CMD_OK;
    }
    if ((pbsPupNumber == 0) && (pbsDqNumber >= 5))
    {
        pbsPupNumber++;
        if (pbsPupNumber >= maxPupNum)
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
        if ((pbsPupNumber == 1) && (pbsDqNumber >= 4))
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
    }
    ddr3PbsCalcResult(pbsPupNumber, pbsDqNumber, (GT_32 *)(&inFields[2]));

    inFields[0] = pbsPupNumber;
    inFields[1] = pbsDqNumber;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* wrHwsDDR3AutoTune
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
static CMD_STATUS wrHwsDDR3AutoTune
(
    IN  GT_UINTPTR inArgs[CMD_MAX_ARGS],
    IN  GT_UINTPTR inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32  devNum = (GT_U32)inArgs[0];
    MV_HWS_EMC_TYPE  memType = (inArgs[1] == 1) ? MV_LOOKUP_EMC : MV_FORWARD_EMC;
    GT_BOOL dllOff = (inArgs[2] == 1) ? GT_TRUE : GT_FALSE;
    GT_32  rlMode = inArgs[4];
    GT_STATUS rc;

    rc = mvHwsDdr3Puma3TuneAlg(globalDevNum,devNum,memType,dllOff,rlMode);

    galtisOutput(outArgs, rc, "");
    return CMD_OK;
}

/*******************************************************************************
*
* dbCommands - Galtis database galtis commands
*
*******************************************************************************/
static CMD_COMMAND dbCommands[] = {
{"HwsDdr3PhyRegAccessSet", &wrHwsDdr3PhyRegAccessSet, 6, 1},
{"HwsDdr3PhyRegAccessGetFirst", &wrHwsDdr3PhyRegAccessGetFirst, 6, 0},
{"HwsDdr3PhyRegAccessGetNext", &wrHwsDdr3PhyRegAccessGetNext, 6, 0},
{"HwsDdr3AdllControlSet", &wrHwsDdr3AdllControlSet, 4, 4},
{"HwsDdr3AdllControlGetFirst", &wrHwsDdr3AdllControlGetFirst, 4, 0},
{"HwsDdr3AdllControlGetNext", &wrHwsDdr3AdllControlGetNext, 4, 0},
{"HwsDdr3ExternalMemAccessSet",&wrHwsDdr3ExternalMemAccessSet, 7, 6},
{"HwsDdr3ExternalMemAccessGetFirst",&wrHwsDdr3ExternalMemAccessGetFirst, 7, 0},
{"HwsDdr3ExternalMemAccessGetNext",&wrHwsDdr3ExternalMemAccessGetNext, 7, 0},
{"HwsDdr3PupReset", &wrHwsDdr3PupReset, 3,0},
{"HwsDdr3IfcBistConfiguration", &wrHwsDdr3IfcBistCfg, 11, 0},
{"HwsDdr3IfcBistReadStatusGetFirst", &wrHwsDdr3IfcBistReadStatusGetFirst, 3, 0},
{"HwsDdr3IfcBistReadStatusGetNext", &wrHwsDdr3IfcBistReadStatusGetNext, 3, 0},
{"HwsDdr3InitConfiguration", &wrHwsDdr3InitConfiguration, 5, 0},
{"HwsDdr3ReadLevelingStaticCfg", &wrHwsDdr3ReadLevelingStaticCfg,4, 0},

{"HwsDdr3ChangeFreq", &wrHwsDdr3ChangeFreq, 5, 0},
{"HwsDdr3LoadPatterns", &wrHwsDdr3LoadPatterns, 5, 0},
{"HwsDdr3ReadLeveling", &wrHwsDdr3ReadLeveling, 7, 0},
{"HwsDdr3ReadLevelingResultGetFirst", &wrHwsDdr3ReadLevelingGetFirst, 3, 0},
{"HwsDdr3ReadLevelingResultGetNext", &wrHwsDdr3ReadLevelingGetNext, 3, 0},
{"HwsDdr3DqsCentralization", &wrHwsDdr3DqsCentralization, 4, 0},
{"HwsDdr3DqsCetralizationResultGetFirst", &wrHwsDdr3DqsCentralizationGetFirst, 2, 0},
{"HwsDdr3DqsCetralizationResultGetNext", &wrHwsDdr3DqsCentralizationGetNext, 2, 0},
{"HwsDdr3PbsCalc", &wrHwsDdr3PbsCalc, 4, 0},
{"HwsDdr3PbsResultGetFirst", &wrHwsDdr3PbsResultGetFirst, 2, 0},
{"HwsDdr3PbsResultGetNext", &wrHwsDdr3PbsResultGetNext, 2, 0},
{"HwsDDR3AutoTune", &wrHwsDDR3AutoTune, 4,0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))


/*******************************************************************************
* cmdLibInitDDR3
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
GT_STATUS cmdLibInitDDR3()
{
    return cmdInitLibrary(dbCommands, numCommands);
}


