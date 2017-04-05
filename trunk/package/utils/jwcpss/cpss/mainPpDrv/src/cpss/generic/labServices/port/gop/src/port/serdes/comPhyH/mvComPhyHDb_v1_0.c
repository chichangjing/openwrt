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
* mvHwsComPhyHDb.c
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 50 $
*
*******************************************************************************/
#include <common/configElementDb/mvCfgSeqDbIf.h>
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>
#include <serdes/comPhyH/mvComPhyHDb.h>

MV_CFG_SEQ hwsSerdesSeqDb[MV_SERDES_LAST_SEQ];

static GT_U32 serdesSpeedAddr[] =
{
0x4,
0x14,
0x18,
0x1C,
0x20,
0x34,
0x38,
0x88,
0x98,
0xF4,
0x104,
0x108,
0x10C,
0x110,
0x114,
0x134,
0x138,
0x13C,
0x140,
0x168,
0x184,
0x1CC,
0x1fc,
0x008,
0x00C,
0x02c,
0x1fc
};

static GT_U32 speed1Data[] =
{
0xFC8C,
0x07FE,
0x00BF,  /* Squelch threshold = 0 */
0xD247,  /* set DFE resolution to 4mV */
0xAFC0,
0x0DFE,
0x0BD2,
0x1026,  /* password unprotected */
0x0066,
0x0012,
0x0200,  /* tximpcal_th[2:0] 0 : For TX_IMP, Rximpcal_th[2:0] 0 For RX_IMP */
0xC0FA,
0x36D6,
0x30D6,
0x33A0,
0x3113, /* VREGTXRX[1:0] 0 : For Saving the power (lower regulator voltage), VTHVCOCAL[1:0] 00  : For PLL CAL, SEL_VTHVCOCONT 1*/
0x0104, /* VREGVCO[1:0] 0 : For Saving the power (lower regulator voltage) */
0x0026, /* ICP = 6 */
0x0800,
0xD528, /* set align90 to 85d */
0x1093, /* MSI recommend leaving dtl_sq_ploop_en at 1.*/
0x0B28, /* SEL_SMPLR_VCM[1:0]=0.5 V */
0x0001,
0x0600,
0x0120,
0x7470,
0x0000
};

static GT_U32 speed31Data[] =
{
0xFC8C,
0x07FE,
0x00BF,  /* Squelch threshold = 0 */
0xD247,  /* set DFE resolution to 4mV */
0xAFC0,
0x0DFE,
0x0BD2,
0x1026,  /* password unprotected */
0x0088,
0x0012,
0x0200, /* tximpcal_th[2:0] 0 : For TX_IMP, Rximpcal_th[2:0] 0 For RX_IMP */
0xC0FA,
0x36D6,
0x30D6,
0x33A0,
0x3113, /* VREGTXRX[1:0] 0 : For Saving the power (lower regulator voltage), VTHVCOCAL[1:0] 00  : For PLL CAL, SEL_VTHVCOCONT 1*/
0x0104, /* VREGVCO[1:0] 0 : For Saving the power (lower regulator voltage) */
0x0026, /* ICP = 6 */
0x0800,
0xD528, /* set align90 to 85d */
0x1093, /* MSI recommend leaving dtl_sq_ploop_en at 1.*/
0x0B28, /* SEL_SMPLR_VCM[1:0]=0.5 V */
0x0001,
0x0600,
0x0120,
0x7470,
0x0000
};

static GT_U32 speed37Data[] =
{
0xFC8C,
0x07FE,
0x00BF,  /* Squelch threshold = 0 */
0xD247,  /* set DFE resolution to 4mV */
0xAFC0,
0x0DFE,
0x0BD2,
0x1026,  /* password unprotected */
0x0099,
0x0012,
0x0200, /* tximpcal_th[2:0] 0 : For TX_IMP, Rximpcal_th[2:0] 0 For RX_IMP */
0xC0FA,
0x36D6,
0x30D6,
0x33A0,
0x3113, /* VREGTXRX[1:0] 0 : For Saving the power (lower regulator voltage), VTHVCOCAL[1:0] 00  : For PLL CAL, SEL_VTHVCOCONT 1*/
0x0104, /* VREGVCO[1:0] 0 : For Saving the power (lower regulator voltage) */
0x0026, /* ICP = 6 */
0x0800,
0xD528, /* set align90 to 85d */
0x1093, /* MSI recommend leaving dtl_sq_ploop_en at 1.*/
0x0B28, /* SEL_SMPLR_VCM[1:0]=0.5 V */
0x0001,
0x0600,
0x0120,
0x7470,
0x0000
};

static GT_U32 speed4Data[] =
{
0xFC8C,
0x07FE,
0x00BF,  /* Squelch threshold = 0 */
0xD247,  /* set DFE resolution to 4mV */
0xAFC0,
0x0DFE,
0x0BD2,
0x1026,  /* password unprotected */
0x00AA,
0x0012,
0x0200, /* tximpcal_th[2:0] 0 : For TX_IMP, Rximpcal_th[2:0] 0 For RX_IMP */
0xC0FA,
0x36D6,
0x30D6,
0x33A0,
0x3113, /* VREGTXRX[1:0] 0 : For Saving the power (lower regulator voltage), VTHVCOCAL[1:0] 00  : For PLL CAL, SEL_VTHVCOCONT 1*/
0x0104, /* VREGVCO[1:0] 0 : For Saving the power (lower regulator voltage) */
0x0026, /* ICP = 6 */
0x0800,
0xD528, /* set align90 to 85d */
0x1093, /* MSI recommend leaving dtl_sq_ploop_en at 1.*/
0x0B28, /* SEL_SMPLR_VCM[1:0]=0.5 V */
0x0001,
0x0600,
0x0120,
0x7470,
0x0000
};

static GT_U32 speed5Data[] =
{
0xFC8C,
0x07FE,
0x00BF,  /* Squelch threshold = 0 */
0xD147,  /* set DFE resolution to 4mV */
0xAFC0,
0x0DFE,
0x0BD2,
0x1026,  /* password unprotected */
0x0033,
0x0012,
0x0200, /* tximpcal_th[2:0] 0 : For TX_IMP, Rximpcal_th[2:0] 0 For RX_IMP */
0xC0FA,
0x36D6,
0x30D6,
0x33A0,
0x3113, /* VREGTXRX[1:0] 0 : For Saving the power (lower regulator voltage), VTHVCOCAL[1:0] 00  : For PLL CAL, SEL_VTHVCOCONT 1*/
0x0104, /* VREGVCO[1:0] 0 : For Saving the power (lower regulator voltage) */
0x0026, /* ICP = 6 */
0x0800,
0xD528, /* set align90 to 85d */
0x1093, /* MSI recommend leaving dtl_sq_ploop_en at 1.*/
0x0B28, /* SEL_SMPLR_VCM[1:0]=0.5 V */
0x0001,
0x0600,
0x0120,
0x7470,
0x0000
};

static GT_U32 speed6Data[] =
{
0xFC8C,
0x07FE,
0x00BF,  /* Squelch threshold = 0 */
0xD147,  /* set DFE resolution to 4mV */
0xAFC0,
0x0DFE,
0x0BD2,
0x1026,  /* password unprotected */
0x00BB,
0x0012,
0x0200, /* tximpcal_th[2:0] 0 : For TX_IMP, Rximpcal_th[2:0] 0 For RX_IMP */
0xC0FA,
0x36D6,
0x30D6,
0x33A0,
0x3113, /* VREGTXRX[1:0] 0 : For Saving the power (lower regulator voltage), VTHVCOCAL[1:0] 00  : For PLL CAL, SEL_VTHVCOCONT 1*/
0x0104, /* VREGVCO[1:0] 0 : For Saving the power (lower regulator voltage) */
0x0026, /* ICP = 6 */
0x0800,
0xD528, /* set align90 to 85d */
0x1093, /* MSI recommend leaving dtl_sq_ploop_en at 1.*/
0x0B28, /* SEL_SMPLR_VCM[1:0]=0.5 V */
0x0001,
0x0600,
0x0120,
0x7470,
0x0000
};

static GT_U32 speed7Data[] =
{
0xFC8C,
0x07FE,
0x00BF,  /* Squelch threshold = 0 */
0xD147,  /* set DFE resolution to 4mV */
0xAFC0,
0x0DFE,
0x0BD2,
0x1026,  /* password unprotected */
0x00DD,
0x0012,
0x0200, /* tximpcal_th[2:0] 0 : For TX_IMP, Rximpcal_th[2:0] 0 For RX_IMP */
0xC0FA,
0x36D6,
0x30D6,
0x33A0,
0x3113, /* VREGTXRX[1:0] 0 : For Saving the power (lower regulator voltage), VTHVCOCAL[1:0] 00  : For PLL CAL, SEL_VTHVCOCONT 1*/
0x0104, /* VREGVCO[1:0] 0 : For Saving the power (lower regulator voltage) */
0x0026, /* ICP = 6 */
0x0800,
0xD528, /* set align90 to 85d */
0x1093, /* MSI recommend leaving dtl_sq_ploop_en at 1.*/
0x0B28, /* SEL_SMPLR_VCM[1:0]=0.5 V */
0x0001,
0x0600,
0x0120,
0x7470,
0x0000
};

static GT_U32 speed10Data[] =
{
0xFC8C,
0x07FE,
0x00BF,  /* Squelch threshold = 0 */
0xD147,  /* set DFE resolution to 4mV */
0xAFC0,
0x0DFE,
0x0BD2,
0x1026,  /* password unprotected */
0x00EE,
0x0012,
0x0200, /* tximpcal_th[2:0] 0 : For TX_IMP, Rximpcal_th[2:0] 0 For RX_IMP */
0xC0FA,
0x36D6,
0x30D6,
0x33A0,
0x3113, /* VREGTXRX[1:0] 0 : For Saving the power (lower regulator voltage), VTHVCOCAL[1:0] 00  : For PLL CAL, SEL_VTHVCOCONT 1*/
0x0104, /* VREGVCO[1:0] 0 : For Saving the power (lower regulator voltage) */
0x0026, /* ICP = 6 */
0x1800,
0xD528, /* set align90 to 85d */
0x1093, /* MSI recommend leaving dtl_sq_ploop_en at 1.*/
0x0B28, /* SEL_SMPLR_VCM[1:0]=0.5 V */
0x0001,
0x0600,
0x0120,
0x7470,
0x0000
};


static GT_U32 speed12Data[] =
{
0xFC8C,
0x07FE,
0x00BF,  /* Squelch threshold = 0 */
0xD147,  /* set DFE resolution to 4mV */
0xAFC0,
0x0DFE,
0x0BD2,
0x1026,  /* password unprotected */
0x00FF,
0x0012,
0x0200, /* tximpcal_th[2:0] 0 : For TX_IMP, Rximpcal_th[2:0] 0 For RX_IMP */
0xC0FA,
0x36D6,
0x30D6,
0x33A0,
0x3113, /* VREGTXRX[1:0] 0 : For Saving the power (lower regulator voltage), VTHVCOCAL[1:0] 00  : For PLL CAL, SEL_VTHVCOCONT 1*/
0x0104, /* VREGVCO[1:0] 0 : For Saving the power (lower regulator voltage) */
0x0026, /* ICP = 6 */
0x1800,
0xD528, /* set align90 to 85d */
0x1093, /* MSI recommend leaving dtl_sq_ploop_en at 1.*/
0x0B28, /* SEL_SMPLR_VCM[1:0]=0.5 V */
0x0001,
0x0600,
0x0120,
0x7470,
0x0000
};

GT_STATUS hwsComPhyHSeqInit(void)
{
    MV_WRITE_OP_PARAMS writeLine;
    MV_POLLING_OP_PARAMS polOp;
    GT_U32 i;

    hwsOsMemSetFuncPtr(hwsSerdesSeqDb, 0, sizeof(hwsSerdesSeqDb));

    /* SERDES_SD_RESET Sequence init */
    hwsSerdesSeqDb[SERDES_SD_RESET_SEQ].seqId = SERDES_SD_RESET_SEQ;
    hwsSerdesSeqDb[SERDES_SD_RESET_SEQ].cfgSeqSize = 1;
    hwsSerdesSeqDb[SERDES_SD_RESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SD_RESET_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SD_RESET_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_1;
      writeLine.data = 0;
      writeLine.mask = (1 << 3);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SD_RESET_SEQ], 0, writeLine);
    }
    else
    {
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_SD_UNRESET Sequence init */
    hwsSerdesSeqDb[SERDES_SD_UNRESET_SEQ].seqId = SERDES_SD_UNRESET_SEQ;
    hwsSerdesSeqDb[SERDES_SD_UNRESET_SEQ].cfgSeqSize = 1;
    hwsSerdesSeqDb[SERDES_SD_UNRESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SD_UNRESET_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SD_UNRESET_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_1;
      writeLine.data = (1 << 2) | (1 << 3);
      writeLine.mask = (1 << 2) | (1 << 3);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SD_UNRESET_SEQ], 0, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_RF_RESET Sequence init */
    hwsSerdesSeqDb[SERDES_RF_RESET_SEQ].seqId = SERDES_RF_RESET_SEQ;
    hwsSerdesSeqDb[SERDES_RF_RESET_SEQ].cfgSeqSize = 1;
    hwsSerdesSeqDb[SERDES_RF_RESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_RF_RESET_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_RF_RESET_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_1;
      writeLine.data = 0;
      writeLine.mask = (1 << 6);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RF_RESET_SEQ], 0, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_RF_UNRESET Sequence init */
    hwsSerdesSeqDb[SERDES_RF_UNRESET_SEQ].seqId = SERDES_RF_UNRESET_SEQ;
    hwsSerdesSeqDb[SERDES_RF_UNRESET_SEQ].cfgSeqSize = 1;
    hwsSerdesSeqDb[SERDES_RF_UNRESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_RF_UNRESET_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_RF_UNRESET_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_1;
      writeLine.data = (1 << 6);
      writeLine.mask = (1 << 2) | (1 << 6);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RF_UNRESET_SEQ], 0, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SYNCE_RESET Sequence init */
    hwsSerdesSeqDb[SERDES_SYNCE_RESET_SEQ].seqId = SERDES_SYNCE_RESET_SEQ;
    hwsSerdesSeqDb[SERDES_SYNCE_RESET_SEQ].cfgSeqSize = 1;
    hwsSerdesSeqDb[SERDES_SYNCE_RESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SYNCE_RESET_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SYNCE_RESET_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = 0;
      writeLine.mask = (1 << 3);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SYNCE_RESET_SEQ], 0, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_SYNCE_UNRESET Sequence init */
    hwsSerdesSeqDb[SERDES_SYNCE_UNRESET_SEQ].seqId = SERDES_SYNCE_UNRESET_SEQ;
    hwsSerdesSeqDb[SERDES_SYNCE_UNRESET_SEQ].cfgSeqSize = 1;
    hwsSerdesSeqDb[SERDES_SYNCE_UNRESET_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SYNCE_UNRESET_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SYNCE_UNRESET_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = (1 << 3);
      writeLine.mask = (1 << 3);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SYNCE_UNRESET_SEQ], 0, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SERDES_POWER_UP_CTRL_SEQ Sequence init */
    hwsSerdesSeqDb[SERDES_SERDES_POWER_UP_CTRL_SEQ].seqId = SERDES_SERDES_POWER_UP_CTRL_SEQ;
    hwsSerdesSeqDb[SERDES_SERDES_POWER_UP_CTRL_SEQ].cfgSeqSize = 5;
    hwsSerdesSeqDb[SERDES_SERDES_POWER_UP_CTRL_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SERDES_POWER_UP_CTRL_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SERDES_POWER_UP_CTRL_SEQ].cfgSeq != NULL)
    {
      mvCreateDelayOp(&hwsSerdesSeqDb[SERDES_SERDES_POWER_UP_CTRL_SEQ], 0, 1);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_0;
      writeLine.data = 0x1802;
      writeLine.mask = 0x1802;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SERDES_POWER_UP_CTRL_SEQ], 1, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = 0x10;
      writeLine.mask = 0x10;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SERDES_POWER_UP_CTRL_SEQ], 2, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_1;
      writeLine.data = (1 << 3);
      writeLine.mask = (1 << 3);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SERDES_POWER_UP_CTRL_SEQ], 3, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x4;
      writeLine.data = 0x7000;
      writeLine.mask = 0x7000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SERDES_POWER_UP_CTRL_SEQ], 4, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_SERDES_POWER_DOWN_CTRL_SEQ Sequence init */
    hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ].seqId = SERDES_SERDES_POWER_DOWN_CTRL_SEQ;
    hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ].cfgSeqSize = 5;
    hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_0;
      writeLine.data = 0;
      writeLine.mask = 0;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ], 0, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_1;
      writeLine.data = 1;
      writeLine.mask = 0xFFFF77FF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ], 1, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = 0;
      writeLine.mask = 0;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ], 2, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_3;
      writeLine.data = 0x20;
      writeLine.mask = 0;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ], 3, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_RESERVED_REGISTER_0;
      writeLine.data = 0;
      writeLine.mask = 0;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SERDES_POWER_DOWN_CTRL_SEQ], 4, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SERDES_RXINT_DOWN Sequence init */
    hwsSerdesSeqDb[SERDES_SERDES_RXINT_DOWN_SEQ].seqId = SERDES_SERDES_RXINT_DOWN_SEQ;
    hwsSerdesSeqDb[SERDES_SERDES_RXINT_DOWN_SEQ].cfgSeqSize = 2;
    hwsSerdesSeqDb[SERDES_SERDES_RXINT_DOWN_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SERDES_RXINT_DOWN_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SERDES_RXINT_DOWN_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_1;
      writeLine.data = 0;
      writeLine.mask = (1 << 4);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SERDES_RXINT_DOWN_SEQ], 0, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x98;
      writeLine.data = 0;
      writeLine.mask = 0x400;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SERDES_RXINT_DOWN_SEQ], 1, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_1;
      writeLine.data = (1 << 3);
      writeLine.mask = (1 << 3);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SERDES_RXINT_UP Sequence init */
    hwsSerdesSeqDb[SERDES_SERDES_RXINT_UP_SEQ].seqId = SERDES_SERDES_RXINT_UP_SEQ;
    hwsSerdesSeqDb[SERDES_SERDES_RXINT_UP_SEQ].cfgSeqSize = 4;
    hwsSerdesSeqDb[SERDES_SERDES_RXINT_UP_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SERDES_RXINT_UP_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SERDES_RXINT_UP_SEQ].cfgSeq != NULL)
    {
      i = 0;
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x98;
      writeLine.data = 0x400;
      writeLine.mask = 0x400;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SERDES_RXINT_UP_SEQ], i++, writeLine);
      mvCreateDelayOp(&hwsSerdesSeqDb[SERDES_SERDES_RXINT_UP_SEQ], i++, 1);
      polOp.indexOffset = SERDES_UNIT;
      polOp.regOffset = SERDES_PHY_REGS + 0x8C;
      polOp.data = 0x40;
      polOp.mask = 0x40;
      polOp.numOfLoops = 100;
      polOp.waitTime = 1;
      mvCreatePollingOp(&hwsSerdesSeqDb[SERDES_SERDES_RXINT_UP_SEQ], i++, polOp);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x98;
      writeLine.data = 0;
      writeLine.mask = 0x400;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SERDES_RXINT_UP_SEQ], i++, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SPEED_1_25G_SEQ init */
    hwsSerdesSeqDb[SERDES_SPEED_1_25G_SEQ].seqId = SERDES_SPEED_1_25G_SEQ;
    hwsSerdesSeqDb[SERDES_SPEED_1_25G_SEQ].cfgSeqSize = 3 + sizeof(speed1Data) / sizeof(GT_U32);
    hwsSerdesSeqDb[SERDES_SPEED_1_25G_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SPEED_1_25G_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SPEED_1_25G_SEQ].cfgSeq != NULL)
    {
      /* global serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = 0x30;
      writeLine.mask = 0x70;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_1_25G_SEQ], 0, writeLine);
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_0;
      writeLine.data = 0x0330;
      writeLine.mask = 0x07F8;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_1_25G_SEQ], 1, writeLine);
      writeLine.regOffset = SERDES_RESERVED_REGISTER_0;
      writeLine.data = 0x0C;
      writeLine.mask = 0x1F;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_1_25G_SEQ], 2, writeLine);

      /* internal serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.mask = 0xFFFF;
      for (i = 0; i < sizeof(speed1Data) / sizeof(GT_U32); i++)
      {
        writeLine.regOffset = SERDES_PHY_REGS + serdesSpeedAddr[i];
        writeLine.data = speed1Data[i];
        mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_1_25G_SEQ], 3+i, writeLine);
      }
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* hws_SERDES_SPEED_3_125G init */
    hwsSerdesSeqDb[SERDES_SPEED_3_125G_SEQ].seqId = SERDES_SPEED_3_125G_SEQ;
    hwsSerdesSeqDb[SERDES_SPEED_3_125G_SEQ].cfgSeqSize = 3 + sizeof(speed31Data) / sizeof(GT_U32);
    hwsSerdesSeqDb[SERDES_SPEED_3_125G_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SPEED_3_125G_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SPEED_3_125G_SEQ].cfgSeq != NULL)
    {
      /* global serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = 0x30;
      writeLine.mask = 0x70;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_3_125G_SEQ], 0, writeLine);
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_0;
      writeLine.data = 0x0440;
      writeLine.mask = 0x07F8;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_3_125G_SEQ], 1, writeLine);
      writeLine.regOffset = SERDES_RESERVED_REGISTER_0;
      writeLine.data = 0x0C;
      writeLine.mask = 0x1F;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_3_125G_SEQ], 2, writeLine);

      /* internal serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.mask = 0xFFFF;
      for (i = 0; i < sizeof(speed31Data) / sizeof(GT_U32); i++)
      {
        writeLine.regOffset = SERDES_PHY_REGS + serdesSpeedAddr[i];
        writeLine.data = speed31Data[i];
        mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_3_125G_SEQ], 3+i, writeLine);
      }
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_SPEED_3_75G_SEQ init */
    hwsSerdesSeqDb[SERDES_SPEED_3_75G_SEQ].seqId = SERDES_SPEED_3_75G_SEQ;
    hwsSerdesSeqDb[SERDES_SPEED_3_75G_SEQ].cfgSeqSize = 3 + sizeof(speed37Data) / sizeof(GT_U32);
    hwsSerdesSeqDb[SERDES_SPEED_3_75G_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SPEED_3_75G_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SPEED_3_75G_SEQ].cfgSeq != NULL)
    {
      /* global serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = 0x30;
      writeLine.mask = 0x70;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_3_75G_SEQ], 0, writeLine);
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_0;
      writeLine.data = 0x04C8;
      writeLine.mask = 0x07F8;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_3_75G_SEQ], 1, writeLine);
      writeLine.regOffset = SERDES_RESERVED_REGISTER_0;
      writeLine.data = 0x0C;
      writeLine.mask = 0x1F;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_3_75G_SEQ], 2, writeLine);

      /* internal serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.mask = 0xFFFF;
      for (i = 0; i < sizeof(speed37Data) / sizeof(GT_U32); i++)
      {
        writeLine.regOffset = SERDES_PHY_REGS + serdesSpeedAddr[i];
        writeLine.data = speed37Data[i];
        mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_3_75G_SEQ], 3+i, writeLine);
      }
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_SPEED_4_25G_SEQ init */
    hwsSerdesSeqDb[SERDES_SPEED_4_25G_SEQ].seqId = SERDES_SPEED_4_25G_SEQ;
    hwsSerdesSeqDb[SERDES_SPEED_4_25G_SEQ].cfgSeqSize = 3 + sizeof(speed4Data) / sizeof(GT_U32);
    hwsSerdesSeqDb[SERDES_SPEED_4_25G_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SPEED_4_25G_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SPEED_4_25G_SEQ].cfgSeq != NULL)
    {
      /* global serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = 0x30;
      writeLine.mask = 0x70;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_4_25G_SEQ], 0, writeLine);
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_0;
      writeLine.data = 0x0550;
      writeLine.mask = 0x07F8;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_4_25G_SEQ], 1, writeLine);
      writeLine.regOffset = SERDES_RESERVED_REGISTER_0;
      writeLine.data = 0x0C;
      writeLine.mask = 0x1F;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_4_25G_SEQ], 2, writeLine);

      /* internal serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.mask = 0xFFFF;
      for (i = 0; i < sizeof(speed4Data) / sizeof(GT_U32); i++)
      {
        writeLine.regOffset = SERDES_PHY_REGS + serdesSpeedAddr[i];
        writeLine.data = speed4Data[i];
        mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_4_25G_SEQ], 3+i, writeLine);
      }
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_SPEED_5G_SEQ init */
    hwsSerdesSeqDb[SERDES_SPEED_5G_SEQ].seqId = SERDES_SPEED_5G_SEQ;
    hwsSerdesSeqDb[SERDES_SPEED_5G_SEQ].cfgSeqSize = 3 + sizeof(speed5Data) / sizeof(GT_U32);
    hwsSerdesSeqDb[SERDES_SPEED_5G_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SPEED_5G_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SPEED_5G_SEQ].cfgSeq != NULL)
    {
      /* global serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = 0x30;
      writeLine.mask = 0x70;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_5G_SEQ], 0, writeLine);
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_0;
      writeLine.data = 0x0198;
      writeLine.mask = 0x07F8;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_5G_SEQ], 1, writeLine);
      writeLine.regOffset = SERDES_RESERVED_REGISTER_0;
      writeLine.data = 0x0C;
      writeLine.mask = 0x1F;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_5G_SEQ], 2, writeLine);

      /* internal serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.mask = 0xFFFF;
      for (i = 0; i < sizeof(speed5Data) / sizeof(GT_U32); i++)
      {
        writeLine.regOffset = SERDES_PHY_REGS + serdesSpeedAddr[i];
        writeLine.data = speed5Data[i];
        mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_5G_SEQ], 3+i, writeLine);
      }
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_SPEED_6_25G_SEQ init */
    hwsSerdesSeqDb[SERDES_SPEED_6_25G_SEQ].seqId = SERDES_SPEED_6_25G_SEQ;
    hwsSerdesSeqDb[SERDES_SPEED_6_25G_SEQ].cfgSeqSize = 3 + sizeof(speed6Data) / sizeof(GT_U32);
    hwsSerdesSeqDb[SERDES_SPEED_6_25G_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SPEED_6_25G_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SPEED_6_25G_SEQ].cfgSeq != NULL)
    {
      /* global serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = 0x10;
      writeLine.mask = 0x50;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_6_25G_SEQ], 0, writeLine);
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_0;
      writeLine.data = 0x05D8;
      writeLine.mask = 0x07F8;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_6_25G_SEQ], 1, writeLine);
      writeLine.regOffset = SERDES_RESERVED_REGISTER_0;
      writeLine.data = 0x0C;
      writeLine.mask = 0x1F;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_6_25G_SEQ], 2, writeLine);

      /* internal serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.mask = 0xFFFF;
      for (i = 0; i < sizeof(speed6Data) / sizeof(GT_U32); i++)
      {
        writeLine.regOffset = SERDES_PHY_REGS + serdesSpeedAddr[i];
        writeLine.data = speed6Data[i];
        mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_6_25G_SEQ], 3+i, writeLine);
      }
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_SPEED_7_5G_SEQ init */
    hwsSerdesSeqDb[SERDES_SPEED_7_5G_SEQ].seqId = SERDES_SPEED_7_5G_SEQ;
    hwsSerdesSeqDb[SERDES_SPEED_7_5G_SEQ].cfgSeqSize = 3 + sizeof(speed7Data) / sizeof(GT_U32);
    hwsSerdesSeqDb[SERDES_SPEED_7_5G_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SPEED_7_5G_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SPEED_7_5G_SEQ].cfgSeq != NULL)
    {
      /* global serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = 0x10;
      writeLine.mask = 0x50;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_7_5G_SEQ], 0, writeLine);
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_0;
      writeLine.data = 0x06E8;
      writeLine.mask = 0x07F8;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_7_5G_SEQ], 1, writeLine);
      writeLine.regOffset = SERDES_RESERVED_REGISTER_0;
      writeLine.data = 0x0C;
      writeLine.mask = 0x1F;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_7_5G_SEQ], 2, writeLine);

      /* internal serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.mask = 0xFFFF;
      for (i = 0; i < sizeof(speed7Data) / sizeof(GT_U32); i++)
      {
        writeLine.regOffset = SERDES_PHY_REGS + serdesSpeedAddr[i];
        writeLine.data = speed7Data[i];
        mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_7_5G_SEQ], 3+i, writeLine);
      }
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* hws_SERDES_SPEED_10_3125G init */
    hwsSerdesSeqDb[SERDES_SPEED_10_3125G_SEQ].seqId = SERDES_SPEED_10_3125G_SEQ;
    hwsSerdesSeqDb[SERDES_SPEED_10_3125G_SEQ].cfgSeqSize = 2 + sizeof(speed10Data) / sizeof(GT_U32);
    hwsSerdesSeqDb[SERDES_SPEED_10_3125G_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SPEED_10_3125G_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SPEED_10_3125G_SEQ].cfgSeq != NULL)
    {
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_0;
      writeLine.data = 0x0770;
      writeLine.mask = 0x07F8;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_10_3125G_SEQ], 0, writeLine);
      writeLine.regOffset = SERDES_RESERVED_REGISTER_0;
      writeLine.data = 0x0C;
      writeLine.mask = 0x1F;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_10_3125G_SEQ], 1, writeLine);

      /* internal serdes regs */
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.mask = 0xFFFF;
      for (i = 0; i < sizeof(speed10Data) / sizeof(GT_U32); i++)
      {
        writeLine.regOffset = SERDES_PHY_REGS + serdesSpeedAddr[i];
        writeLine.data = speed10Data[i];
        mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_10_3125G_SEQ], 2+i, writeLine);
      }
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }

    /* hws_SERDES_SPEED_12_5G init */
    hwsSerdesSeqDb[SERDES_SPEED_12_5G_SEQ].seqId = SERDES_SPEED_12_5G_SEQ;
    hwsSerdesSeqDb[SERDES_SPEED_12_5G_SEQ].cfgSeqSize = 2 + sizeof(speed12Data) / sizeof(GT_U32);
    hwsSerdesSeqDb[SERDES_SPEED_12_5G_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SPEED_12_5G_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SPEED_12_5G_SEQ].cfgSeq != NULL)
    {
        writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_0;
        writeLine.data = 0x07F8;
        writeLine.mask = 0x07F8;
        mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_12_5G_SEQ], 0, writeLine);
        writeLine.regOffset = SERDES_RESERVED_REGISTER_0;
        writeLine.data = 0x0C;
        writeLine.mask = 0x1F;
        mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_12_5G_SEQ], 1, writeLine);

        /* internal serdes regs */
        writeLine.indexOffset = SERDES_UNIT;
        writeLine.mask = 0xFFFF;
        for (i = 0; i < sizeof(speed12Data) / sizeof(GT_U32); i++)
        {
            writeLine.regOffset = SERDES_PHY_REGS + serdesSpeedAddr[i];
            writeLine.data = speed12Data[i];
            mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SPEED_12_5G_SEQ], 2+i, writeLine);
        }
    }
    else
    {
        hwsComPhyHSeqFree();
        return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_SD_LPBK_NORMAL_SEQ */
    hwsSerdesSeqDb[SERDES_SD_LPBK_NORMAL_SEQ].seqId = SERDES_SD_LPBK_NORMAL_SEQ;
    hwsSerdesSeqDb[SERDES_SD_LPBK_NORMAL_SEQ].cfgSeqSize = 2;
    hwsSerdesSeqDb[SERDES_SD_LPBK_NORMAL_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SD_LPBK_NORMAL_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SD_LPBK_NORMAL_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x8C;
      writeLine.data = 0;
      writeLine.mask = (0xF << 12);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SD_LPBK_NORMAL_SEQ], 0, writeLine);
      writeLine.regOffset = SERDES_PHY_REGS + 0x54;
      writeLine.data = 0;
      writeLine.mask = (1 << 15);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SD_LPBK_NORMAL_SEQ], 1, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SD_ANA_TX_2_RX_SEQ */
    hwsSerdesSeqDb[SERDES_SD_ANA_TX_2_RX_SEQ].seqId = SERDES_SD_ANA_TX_2_RX_SEQ;
    hwsSerdesSeqDb[SERDES_SD_ANA_TX_2_RX_SEQ].cfgSeqSize = 1;
    hwsSerdesSeqDb[SERDES_SD_ANA_TX_2_RX_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SD_ANA_TX_2_RX_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SD_ANA_TX_2_RX_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x8C;
      writeLine.data = (1 << 13);
      writeLine.mask = (1 << 13);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SD_ANA_TX_2_RX_SEQ], 0, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_SD_DIG_TX_2_RX_SEQ */
    hwsSerdesSeqDb[SERDES_SD_DIG_TX_2_RX_SEQ].seqId = SERDES_SD_DIG_TX_2_RX_SEQ;
    hwsSerdesSeqDb[SERDES_SD_DIG_TX_2_RX_SEQ].cfgSeqSize = 1;
    hwsSerdesSeqDb[SERDES_SD_DIG_TX_2_RX_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SD_DIG_TX_2_RX_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SD_DIG_TX_2_RX_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x8C;
      writeLine.data = (1 << 14);
      writeLine.mask = (1 << 14);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SD_DIG_TX_2_RX_SEQ], 0, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_SD_DIG_RX_2_TX_SEQ */
    hwsSerdesSeqDb[SERDES_SD_DIG_RX_2_TX_SEQ].seqId = SERDES_SD_DIG_RX_2_TX_SEQ;
    hwsSerdesSeqDb[SERDES_SD_DIG_RX_2_TX_SEQ].cfgSeqSize = 3;
    hwsSerdesSeqDb[SERDES_SD_DIG_RX_2_TX_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SD_DIG_RX_2_TX_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SD_DIG_RX_2_TX_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x54;
      writeLine.data = 0xD0;
      writeLine.mask = 0xF0;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SD_DIG_RX_2_TX_SEQ], 0, writeLine);
      writeLine.regOffset = SERDES_PHY_REGS + 0x54;
      writeLine.data = 0x8000;
      writeLine.mask = 0x8000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SD_DIG_RX_2_TX_SEQ], 1, writeLine);
      writeLine.regOffset = SERDES_PHY_REGS + 0x8C;
      writeLine.data = 0x9000;
      writeLine.mask = 0x9000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_SD_DIG_RX_2_TX_SEQ], 2, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_PT_AFTER_PATTERN_NORMAL */
    hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ].seqId = SERDES_PT_AFTER_PATTERN_NORMAL_SEQ;
    hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ].cfgSeqSize = 6;
    hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ].cfgSeq != NULL)
    {
      i = 0;
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x54;
      writeLine.data = 0xE0;
      writeLine.mask = 0xF0;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ], i++, writeLine);
      writeLine.regOffset = SERDES_PHY_REGS + 0x54;
      writeLine.data = 0;
      writeLine.mask = 0x8000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ], i++, writeLine);
      writeLine.regOffset = SERDES_PHY_REGS + 0x54;
      writeLine.data = (1 << 14);
      writeLine.mask = (1 << 14);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ], i++, writeLine);
      writeLine.regOffset = SERDES_PHY_REGS + 0x54;
      writeLine.data = 0;
      writeLine.mask = (1 << 14);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ], i++, writeLine);
      writeLine.regOffset = SERDES_PHY_REGS + 0x8C;
      writeLine.data = (1 << 11);
      writeLine.mask = (1 << 11);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ], i++, writeLine);
      writeLine.regOffset = SERDES_PHY_REGS + 0x8C;
      writeLine.data = 0;
      writeLine.mask = (1 << 11);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_NORMAL_SEQ], i++, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_PT_AFTER_PATTERN_TEST */
    hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_TEST_SEQ].seqId = SERDES_PT_AFTER_PATTERN_TEST_SEQ;
    hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_TEST_SEQ].cfgSeqSize = 4;
    hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_TEST_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_TEST_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_TEST_SEQ].cfgSeq != NULL)
    {
      i = 0;
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x54;
      writeLine.data = 0xE0;
      writeLine.mask = 0xF0;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_TEST_SEQ], i++, writeLine);
      writeLine.regOffset = SERDES_PHY_REGS + 0x54;
      writeLine.data = 0x8000;
      writeLine.mask = 0x8000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_TEST_SEQ], i++, writeLine);
      writeLine.regOffset = SERDES_PHY_REGS + 0x84;
      writeLine.data = 0x2030;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_TEST_SEQ], i++, writeLine);
      writeLine.regOffset = SERDES_PHY_REGS + 0x84;
      writeLine.data = 0x30;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_PT_AFTER_PATTERN_TEST_SEQ], i++, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_RX_TRAINING_ENABLE_SEQ Sequence init */
    hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ].seqId = SERDES_RX_TRAINING_ENABLE_SEQ;
    hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeqSize = 21;
    hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeqSize);
    i = 0;
    if (hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x18;
      writeLine.data = 0;
      writeLine.mask = (1 << 7);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x1FC;
      writeLine.data = 1;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x30;
      writeLine.data = 0xF;
      writeLine.mask = 0x1F;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x40;
      writeLine.data = 0xD7;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xC;
      writeLine.data = 0;
      writeLine.mask = 0x100;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x2C;
      writeLine.data = 0x7C70;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x70;
      writeLine.data = 0x8800;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x78;
      writeLine.data = 0xDF3;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x34;
      writeLine.data = 0x400;
      writeLine.mask = 0x400;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x80;
      writeLine.data = 0xCABF;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x44;
      writeLine.data = 0x4F4E;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x48;
      writeLine.data = 0x4D4C;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x4C;
      writeLine.data = 0x3F3E;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x50;
      writeLine.data = 0x3D3C;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x54;
      writeLine.data = 0x2F2E;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x58;
      writeLine.data = 0x2D2C;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x5C;
      writeLine.data = 0x1F1E;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x60;
      writeLine.data = 0xF0E;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x1FC;
      writeLine.data = 0;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = (1 << 5);
      writeLine.mask = (1 << 5);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_3;
      writeLine.data = (1 << 7);
      writeLine.mask = (1 << 7);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_ENABLE_SEQ], i++, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_RX_TRAINING_DISABLE_SEQ Sequence init */
    hwsSerdesSeqDb[SERDES_RX_TRAINING_DISABLE_SEQ].seqId = SERDES_RX_TRAINING_DISABLE_SEQ;
    hwsSerdesSeqDb[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeqSize = 5;
    hwsSerdesSeqDb[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_RX_TRAINING_DISABLE_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = 0;
      writeLine.mask = (1 << 5);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_DISABLE_SEQ], 0, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_3;
      writeLine.data = 0;
      writeLine.mask = (1 << 7);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_DISABLE_SEQ], 1, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset =  SERDES_PHY_REGS + 0x1FC;
      writeLine.data = 0x1;
      writeLine.mask = 0x1;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_DISABLE_SEQ], 2, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset =  SERDES_PHY_REGS + 0x2C;
      writeLine.data = 0x0;
      writeLine.mask = 0x1;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_DISABLE_SEQ], 3, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset =  SERDES_PHY_REGS + 0x1FC;
      writeLine.data = 0x0;
      writeLine.mask = 0x1;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_RX_TRAINING_DISABLE_SEQ], 4, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_TX_TRAINING_ENABLE_SEQ Sequence init */
    hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ].seqId = SERDES_TX_TRAINING_ENABLE_SEQ;
    hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeqSize = 32;
    hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeqSize);
    i = 0;
    if (hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x94;
      writeLine.data = 0xFFF;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x18;
      writeLine.data = 0x0;
      writeLine.mask = (1 << 7);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x1FC;
      writeLine.data = 0x1;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x14;
      writeLine.data = 0x1;
      writeLine.mask = 0x1;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x28;
      writeLine.data = 0xA;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x2C;
      writeLine.data = 0x7430;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x30;
      writeLine.data = 0xF;
      writeLine.mask = 0x1F;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x34;
      writeLine.data = 0x400;
      writeLine.mask = 0x400;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x40;
      writeLine.data = 0xD7;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xC;
      writeLine.data = 0x0;
      writeLine.mask = 0x100;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x80;
      writeLine.data = 0xCABF;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x78;
      writeLine.data = 0xDF3;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x68;
      writeLine.data = 0x2C00;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x70;
      writeLine.data = 0x8800;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xA0;
      writeLine.data = 0x2000;
      writeLine.mask = 0xF000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xA4;
      writeLine.data = 0x4000;
      writeLine.mask = 0xF000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xA8;
      writeLine.data = 0x6000;
      writeLine.mask = 0xF000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xAC;
      writeLine.data = 0x7000;
      writeLine.mask = 0xF000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xB0;
      writeLine.data = 0x9000;
      writeLine.mask = 0xF000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xB4;
      writeLine.data = 0xA000;
      writeLine.mask = 0xF000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xB8;
      writeLine.data = 0xC000;
      writeLine.mask = 0xF000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xBC;
      writeLine.data = 0xF000;
      writeLine.mask = 0xF000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xC0;
      writeLine.data = 0xF000;
      writeLine.mask = 0xF000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xC4;
      writeLine.data = 0xF000;
      writeLine.mask = 0xF000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xC8;
      writeLine.data = 0xF000;
      writeLine.mask = 0xF000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xCC;
      writeLine.data = 0xF000;
      writeLine.mask = 0xF000;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x94;
      writeLine.data = 0xC00;
      writeLine.mask = 0x1E00;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x6C;
      writeLine.data = 0x0;
      writeLine.mask = 0xE07;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xDC;
      writeLine.data = 0x1;
      writeLine.mask = 0xF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xD8;
      writeLine.data = 0xEFF;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x1FC;
      writeLine.data = 0x0;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = (1 << 5);
      writeLine.mask = (1 << 5);
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_ENABLE_SEQ], i++, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    /* SERDES_TX_TRAINING_DISABLE_SEQ Sequence init */
    hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ].seqId = SERDES_TX_TRAINING_DISABLE_SEQ;
    hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeqSize = 13;
    hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeqSize);

    i = 0;
    if (hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ].cfgSeq != NULL)
    {
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x94;
      writeLine.data = 0x1FFF;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x1FC;
      writeLine.data = 0x1;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xC;
      writeLine.data = 0x120;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x14;
      writeLine.data = 0;
      writeLine.mask = 1;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x2C;
      writeLine.data = 0x7470;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x68;
      writeLine.data = 0x6C01;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x94;
      writeLine.data = 0x19F4;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x6C;
      writeLine.data = 0x341C;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xDC;
      writeLine.data = 0x0;
      writeLine.mask = 0xF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0xD8;
      writeLine.data = 0xE7A;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_PHY_REGS + 0x1FC;
      writeLine.data = 0x0;
      writeLine.mask = 0xFFFF;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_2;
      writeLine.data = 0x0;
      writeLine.mask = 0x20;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ], i++, writeLine);
      writeLine.indexOffset = SERDES_UNIT;
      writeLine.regOffset = SERDES_EXTERNAL_CONFIGURATION_3;
      writeLine.data = 0x0;
      writeLine.mask = 0x100;
      mvCreateWriteOp(&hwsSerdesSeqDb[SERDES_TX_TRAINING_DISABLE_SEQ], i++, writeLine);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }

    /* SERDES_SERDES_WAIT_PLL_SEQ */
    hwsSerdesSeqDb[SERDES_SERDES_WAIT_PLL_SEQ].seqId = SERDES_SERDES_WAIT_PLL_SEQ;
    hwsSerdesSeqDb[SERDES_SERDES_WAIT_PLL_SEQ].cfgSeqSize = 1;
    hwsSerdesSeqDb[SERDES_SERDES_WAIT_PLL_SEQ].cfgSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * hwsSerdesSeqDb[SERDES_SERDES_WAIT_PLL_SEQ].cfgSeqSize);

    if (hwsSerdesSeqDb[SERDES_SERDES_WAIT_PLL_SEQ].cfgSeq != NULL)
    {
      polOp.indexOffset = SERDES_UNIT;
      polOp.regOffset = SERDES_EXTERNAL_STATUS_0;
      polOp.data = (3 << 2);
      polOp.mask = (3 << 2);
      polOp.numOfLoops = 20;
      polOp.waitTime = 1;
      mvCreatePollingOp(&hwsSerdesSeqDb[SERDES_SERDES_WAIT_PLL_SEQ], 0, polOp);
    }
    else
    {
      hwsComPhyHSeqFree();
      return GT_OUT_OF_CPU_MEM;
    }
    return GT_OK;
}

void hwsComPhyHSeqFree(void)
{
  GT_U32 i;

  for (i = 0; (i < MV_SERDES_LAST_SEQ); i++)
  {
      if (hwsSerdesSeqDb[i].cfgSeq != NULL)
	  {
	    hwsOsFreeFuncPtr(hwsSerdesSeqDb[i].cfgSeq->params);
	    hwsOsFreeFuncPtr(hwsSerdesSeqDb[i].cfgSeq);
	  }
  }
}

GT_STATUS hwsComPhyHSeqGet(MV_HWS_COM_PHY_H_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 lineNum)
{
  if (seqLine == NULL)
  {
    return GT_BAD_PTR;
  }
  if ((seqType >= MV_SERDES_LAST_SEQ) || (hwsSerdesSeqDb[seqType].cfgSeq == NULL))
  {
    return GT_NO_SUCH;
  }

  if (hwsSerdesSeqDb[seqType].cfgSeqSize <= lineNum)
  {
    return GT_NO_MORE;
  }

  *seqLine = hwsSerdesSeqDb[seqType].cfgSeq[lineNum];
  return GT_OK;
}

GT_STATUS hwsComPhyHSeqSet(GT_BOOL firstLine, MV_HWS_COM_PHY_H_SUB_SEQ seqType, MV_CFG_ELEMENT *seqLine, GT_U32 numOfOp)
{
  MV_CFG_ELEMENT* newSeq;
  GT_U32 seqNum, i;

  if (seqLine == NULL)
  {
    return GT_BAD_PTR;
  }
  if (seqType >= MV_SERDES_LAST_SEQ)
  {
    return GT_NO_SUCH;
  }
  if (firstLine == GT_TRUE)
  {
    seqNum = 0;
  }
  else
  {
    if (hwsSerdesSeqDb[seqType].cfgSeqSize <= numOfOp)
    {
       /* new line added */
      newSeq = (MV_CFG_ELEMENT*)hwsOsMallocFuncPtr(sizeof(MV_CFG_ELEMENT) * (hwsSerdesSeqDb[seqType].cfgSeqSize+1));
      for (i = 0; i <= hwsSerdesSeqDb[seqType].cfgSeqSize; i++)
      {
        newSeq[i].op = hwsSerdesSeqDb[seqType].cfgSeq[i].op;
        newSeq[i].params = hwsSerdesSeqDb[seqType].cfgSeq[i].params;
      }
      hwsOsFreeFuncPtr(hwsSerdesSeqDb[seqType].cfgSeq);
      hwsSerdesSeqDb[seqType].cfgSeqSize++;
      hwsSerdesSeqDb[seqType].cfgSeq = newSeq;
      hwsSerdesSeqDb[seqType].cfgSeq[numOfOp].params = NULL;
    }
    seqNum = numOfOp;
  }
  mvCreateSeqOperation(&hwsSerdesSeqDb[seqType], seqNum, seqLine);
  return GT_OK;
}


