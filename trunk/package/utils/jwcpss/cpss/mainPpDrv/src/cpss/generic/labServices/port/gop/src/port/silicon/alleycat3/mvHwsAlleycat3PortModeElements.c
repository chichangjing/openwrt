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
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND, ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
* mvHwsAlleycat3PortModeElements.c
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 11 $
*
*******************************************************************************/

#include <private/mvPortModeElements.h>
#include <private/mvHwsPortPrvIf.h>
#include <common/siliconIf/mvSiliconIf.h>

#ifdef WIN32
extern void printPortModeTable(MV_HWS_PORT_INIT_PARAMS **hwsPortsParams, GT_U32 numOfPorts);
#endif


static MV_HWS_PORT_INIT_PARAMS hwsPort0InitParams[] = {
  /* port mode,				  MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {_100Base_FX,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {SGMII,					 GEMAC_NET_SG,0,         GPCS_NET,		0,		FEC_OFF,  _1_25G,    0,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {_1000Base_X,				 GEMAC_NET_X, 0,         GPCS_NET,		0,		FEC_OFF,  _1_25G,    0,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {SGMII2_5,				 GEMAC_NET_X, 0,         GPCS_NET,		0,		FEC_OFF,  _3_125G,   0,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {QSGMII,					 QSGMII_MAC,  0,         QSGMII_PCS,	0,		FEC_OFF,  _5G ,      0,		   1,		  0,		XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX4,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX2,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KR,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_20GBase_KR,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_KR,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_KR10,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RHGL,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RXAUI,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_20GBase_KX4,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_SR_LR,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_20GBase_SR_LR,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_SR_LR,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12_5GBase_KR,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {XLHGL_KR4,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL16G,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS4,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_SR10,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL_LR12,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {TCAM,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL11_LR12,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_2_5GBase_QX,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_5GBase_DQX,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_5GBase_HX,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12GBaseR,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_5GBaseR,                  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_48GBaseR,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_48GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},

  {NON_SUP_MODE,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,	  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA}
};

static MV_HWS_PORT_INIT_PARAMS hwsPort24InitParams[] = {
  /* port mode,				  MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {_100Base_FX,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {SGMII,					  GEMAC_SG,   24,        GPCS_NET,		24,		FEC_OFF,  _1_25G,    6,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {_1000Base_X,				  GEMAC_X,	  24,        GPCS_NET,		24,		FEC_OFF,  _1_25G,    6,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {SGMII2_5,				  GEMAC_X,	  24,        GPCS_NET,		24,		FEC_OFF,  _3_125G,   6,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {QSGMII,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX4,			  XGMAC,      24,		 XPCS,			24,		FEC_OFF,  _3_125G,   6,		   4,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_10GBase_KX2,			  XGMAC,      24,		 XPCS,			24,		FEC_OFF,  _6_25G,    6,		   2,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_10GBase_KR,				  XLGMAC,     24,		 MMPCS,			24,		FEC_OFF,  _10_3125G, 6,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_20GBase_KR,				  XLGMAC,     24,		 MMPCS,			24,		FEC_OFF,  _10_3125G, 6,		   2,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_40GBase_KR,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_KR10,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RHGL,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RXAUI,					  XGMAC,      24,		 XPCS,			24,		FEC_OFF,  _6_25G,    6,		   2,		  0,		RXAUI_MEDIA, _10BIT_OFF},
  {_20GBase_KX4,			  XGMAC,      24,		 XPCS,			24,		FEC_OFF,  _6_25G,    6,		   4,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_10GBase_SR_LR,			  XLGMAC,     24,		 MMPCS,			24,		FEC_OFF,  _10_3125G, 6,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_20GBase_SR_LR,			  XLGMAC,     24,		 MMPCS,			24,		FEC_OFF,  _10_3125G, 6,		   2,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_40GBase_SR_LR,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12_5GBase_KR,			  XLGMAC,     24,		 MMPCS,			24,		FEC_OFF,  _12_5G,	 6,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {XLHGL_KR4,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL16G,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS4,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_SR10,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL_LR12,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {TCAM,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL11_LR12,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_2_5GBase_QX,			  XGMAC,      24,		 XPCS,			24,		FEC_OFF,  _3_125G,   6,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_5GBase_DQX,				  XGMAC,      24,		 XPCS,			24,		FEC_OFF,  _6_25G,	 6,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_5GBase_HX,				  XGMAC,      24,		 XPCS,			24,		FEC_OFF,  _3_125G,   6,		   2,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_12GBaseR,			      XLGMAC,     24,		 MMPCS,			24,		FEC_OFF,  _12_1875G, 6,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_5GBaseR,                  XLGMAC,     24,        MMPCS,         24,     FEC_OFF,  _5_625G,   6,        1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_48GBaseR,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_48GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},

  {NON_SUP_MODE,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,	  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA}
};

static MV_HWS_PORT_INIT_PARAMS hwsPort25InitParams[] = {
  /* port mode,				  MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {_100Base_FX,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {SGMII,					  GEMAC_SG,   25,        GPCS_NET,		25,		FEC_OFF,  _1_25G,    7,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {_1000Base_X,				  GEMAC_X,	  25,        GPCS_NET,		25,		FEC_OFF,  _1_25G,    7,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {SGMII2_5,				  GEMAC_X,	  25,        GPCS_NET,		25,		FEC_OFF,  _3_125G,   7,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {QSGMII,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX4,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX2,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KR,				  XLGMAC,     25,		 MMPCS,			25,		FEC_OFF,  _10_3125G, 7,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_20GBase_KR,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_KR,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_KR10,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RHGL,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RXAUI,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_20GBase_KX4,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_SR_LR,			  XLGMAC,     25,		 MMPCS,			25,		FEC_OFF,  _10_3125G, 7,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_20GBase_SR_LR,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_SR_LR,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12_5GBase_KR,			  XLGMAC,     25,		 MMPCS,			25,		FEC_OFF,  _12_5G,	 7,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {XLHGL_KR4,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL16G,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS4,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_SR10,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL_LR12,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {TCAM,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},  
  {INTLKN_12Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},  
  {INTLKN_16Lanes_10_3125G,	  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA}, 
  {INTLKN_16Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL11_LR12,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_2_5GBase_QX,              MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_5GBase_DQX,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_5GBase_HX,                MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12GBaseR,                 XLGMAC,     25,        MMPCS,      25,        FEC_OFF,  _12_1875G, 7,        1,         0,        XAUI_MEDIA,  _10BIT_OFF}, 
  {_5GBaseR,                  XLGMAC,     25,        MMPCS,      25,        FEC_OFF,  _5_625G,   7,        1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_48GBaseR,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_48GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},

  {NON_SUP_MODE,              MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,	  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA}
};

static MV_HWS_PORT_INIT_PARAMS hwsPort26InitParams[] = {
  /* port mode,				  MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {_100Base_FX,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {SGMII,					  GEMAC_SG,   26,        GPCS_NET,		26,		FEC_OFF,  _1_25G,    8,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {_1000Base_X,				  GEMAC_X,	  26,        GPCS_NET,		26,		FEC_OFF,  _1_25G,    8,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {SGMII2_5,				  GEMAC_X,	  26,        GPCS_NET,		26,		FEC_OFF,  _3_125G,   8,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {QSGMII,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX4,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX2,			  XGMAC,      26,		 XPCS,			26,		FEC_OFF,  _6_25G,    8,		   2,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_10GBase_KR,				  XLGMAC,     26,		 MMPCS,			26,		FEC_OFF,  _10_3125G, 8,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_20GBase_KR,				  XLGMAC,     26,		 MMPCS,			26,		FEC_OFF,  _10_3125G, 8,		   2,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_40GBase_KR,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_KR10,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RHGL,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RXAUI,					  XGMAC,      26,		 XPCS,			26,		FEC_OFF,  _6_25G,    8,		   2,		  0,		RXAUI_MEDIA, _10BIT_OFF},
  {_20GBase_KX4,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_SR_LR,			  XLGMAC,     26,		 MMPCS,			26,		FEC_OFF,  _10_3125G, 8,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_20GBase_SR_LR,			  XLGMAC,     26,		 MMPCS,			26,		FEC_OFF,  _10_3125G, 8,		   2,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_40GBase_SR_LR,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12_5GBase_KR,			  XLGMAC,     26,		 MMPCS,			26,		FEC_OFF,  _12_5G,	 8,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {XLHGL_KR4,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL16G,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS4,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_SR10,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL_LR12,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {TCAM,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL11_LR12,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_2_5GBase_QX,			  XGMAC,      26,		 XPCS,			26,		FEC_OFF,  _3_125G,   8,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_5GBase_DQX,				  XGMAC,      26,		 XPCS,			26,		FEC_OFF,  _6_25G,	 8,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_5GBase_HX,				  XGMAC,      26,		 XPCS,			26,		FEC_OFF,  _3_125G,   8,		   2,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_12GBaseR,			      XLGMAC,     26,		 MMPCS,			26,		FEC_OFF,  _12_1875G, 8,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_5GBaseR,                  XLGMAC,     26,        MMPCS,         26,     FEC_OFF,  _5_625G,   8,        1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_48GBaseR,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_48GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},

  {NON_SUP_MODE,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,	  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA}
};

static MV_HWS_PORT_INIT_PARAMS hwsPort27InitParams[] = {
  /* port mode,				  MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {_100Base_FX,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {SGMII,					  GEMAC_SG,   27,        GPCS_NET,	    27,		FEC_OFF,  _1_25G,    9,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {_1000Base_X,				  GEMAC_X,	  27,        GPCS_NET,		27,		FEC_OFF,  _1_25G,    9,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {SGMII2_5,				  GEMAC_X,	  27,        GPCS_NET,		27,		FEC_OFF,  _3_125G,   9,		   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
  {QSGMII,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX4,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX2,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KR,				  XLGMAC,     27,		 MMPCS,			27,		FEC_OFF,  _10_3125G, 9,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_20GBase_KR,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_KR,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_KR10,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RHGL,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RXAUI,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_20GBase_KX4,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_SR_LR,			  XLGMAC,     27,		 MMPCS,			27,		FEC_OFF,  _10_3125G, 9,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {_20GBase_SR_LR,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_SR_LR,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12_5GBase_KR,			  XLGMAC,     27,		 MMPCS,			27,		FEC_OFF,  _12_5G,	 9,		   1,		  0,		XAUI_MEDIA,  _10BIT_OFF},
  {XLHGL_KR4,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL16G,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS4,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_SR10,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL_LR12,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {TCAM,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},  
  {INTLKN_12Lanes_10_3125G,	  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},  
  {INTLKN_16Lanes_10_3125G,	  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},  
  {INTLKN_12Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL11_LR12,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_2_5GBase_QX,              MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_5GBase_DQX,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_5GBase_HX,                MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12GBaseR,                 XLGMAC,     27,        MMPCS,      27,        FEC_OFF,  _12_1875G, 9,        1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_5GBaseR,                  XLGMAC,     27,        MMPCS,      27,        FEC_OFF,  _5_625G,   9,        1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_48GBaseR,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_48GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},

  {NON_SUP_MODE,              MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,	  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA}
};

static MV_HWS_PORT_INIT_PARAMS hwsPort28InitParams[] = {
  /* port mode,               MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {_100Base_FX,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {SGMII,                     GEMAC_SG,   28,        GPCS_NET,   28,        FEC_OFF,  _1_25G,    10,       1,         0,        XAUI_MEDIA,  _10BIT_ON},
  {_1000Base_X,               GEMAC_X,    28,        GPCS_NET,   28,        FEC_OFF,  _1_25G,    10,       1,         0,        XAUI_MEDIA,  _10BIT_ON},
  {SGMII2_5,                  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {QSGMII,                    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX4,              MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX2,              MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KR,               XLGMAC,     28,        MMPCS,      28,        FEC_OFF,  _10_3125G, 10,       1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_20GBase_KR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_KR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_KR10,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL,                       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RHGL,                      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL,                      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RXAUI,                     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_20GBase_KX4,              MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_SR_LR,            XLGMAC,     28,        MMPCS,      28,        FEC_OFF,  _10_3125G, 10,       1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_20GBase_SR_LR,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_SR_LR,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12_5GBase_KR,             XLGMAC,     28,        MMPCS,      28,        FEC_OFF,  _12_5G,    10,       1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {XLHGL_KR4,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL16G,                    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS,                       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS4,                      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_SR10,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL_LR12,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {TCAM,                      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL11_LR12,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_2_5GBase_QX,              MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_5GBase_DQX,               XGMAC,      28,        XPCS,       28,        FEC_OFF,  _6_25G,    10,       1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_5GBase_HX,                MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12GBaseR,                 XLGMAC,     28,        MMPCS,      28,        FEC_OFF,  _12_1875G, 10,       1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_5GBaseR,                  XLGMAC,     28,        MMPCS,      28,        FEC_OFF,  _5_625G,   10,       1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_48GBaseR,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_48GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},

  {NON_SUP_MODE,              MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA}
};

static MV_HWS_PORT_INIT_PARAMS hwsPort29InitParams[] = {
  /* port mode,               MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {_100Base_FX,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {SGMII,                     GEMAC_SG,   29,        GPCS_NET,   29,        FEC_OFF,  _1_25G,    11,       1,         0,        XAUI_MEDIA,  _10BIT_ON},
  {_1000Base_X,               GEMAC_X,    29,        GPCS_NET,   29,        FEC_OFF,  _1_25G,    11,       1,         0,        XAUI_MEDIA,  _10BIT_ON},
  {SGMII2_5,                  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {QSGMII,                    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX4,              MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX2,              MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KR,               XLGMAC,     29,        MMPCS,      29,        FEC_OFF,  _10_3125G, 11,       1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_20GBase_KR,               XLGMAC,     29,        MMPCS,      29,        FEC_OFF,  _10_3125G, 10,       2,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_40GBase_KR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_KR10,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL,                       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RHGL,                      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL,                      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RXAUI,                     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_20GBase_KX4,              MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_SR_LR,            XLGMAC,     29,        MMPCS,      29,        FEC_OFF,  _10_3125G, 11,       1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_20GBase_SR_LR,            XLGMAC,     29,        MMPCS,      29,        FEC_OFF,  _10_3125G, 10,       2,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_40GBase_SR_LR,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12_5GBase_KR,             XLGMAC,     29,        MMPCS,      29,        FEC_OFF,  _12_5G,    11,       1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {XLHGL_KR4,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL16G,                    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS,                       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS4,                      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_SR10,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL_LR12,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {TCAM,                      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL11_LR12,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_2_5GBase_QX,	      XGMAC,      29,	     XPCS,	 29,	    FEC_OFF,  _3_125G,   11,	   1,	      0,	XAUI_MEDIA,  _10BIT_OFF},
  {_5GBase_DQX,               XGMAC,      29,        XPCS,       29,        FEC_OFF,  _6_25G,    11,       1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_5GBase_HX,                XGMAC,      29,        XPCS,       29,        FEC_OFF,  _3_125G,   10,       2,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_12GBaseR,                 XLGMAC,     29,        MMPCS,      29,        FEC_OFF,  _12_1875G, 11,       1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_5GBaseR,                  XLGMAC,     29,        MMPCS,      29,        FEC_OFF,  _5_625G,   11,       1,         0,        XAUI_MEDIA,  _10BIT_OFF},
  {_48GBaseR,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_48GBase_SR,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},

  {NON_SUP_MODE,              MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA}
};

static MV_HWS_PORT_INIT_PARAMS hwsPort31InitParams[] = {
	/* port mode,				  MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
	{_100Base_FX,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{SGMII,						  GEMAC_SG,   31,        GPCS_NET,		31,		FEC_OFF,  _1_25G,    10,	   1,		  0,		XAUI_MEDIA,  _10BIT_ON},
	{_1000Base_X,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{SGMII2_5,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{QSGMII,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{_10GBase_KX4,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{_10GBase_KX2,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{_10GBase_KR,				  MAC_NA,     NA_NUM,	 PCS_NA,	 NA_NUM,	FEC_OFF,  SPEED_NA,  0,	       0,		  0,		XAUI_MEDIA,  _10BIT_NA},
	{_20GBase_KR,				  MAC_NA,     NA_NUM,	 PCS_NA,	 NA_NUM,	FEC_OFF,  SPEED_NA,  0,	       0,		  0,		XAUI_MEDIA,  _10BIT_NA},
	{_40GBase_KR,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{_100GBase_KR10,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{HGL,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{RHGL,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{CHGL,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{RXAUI,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{_20GBase_KX4,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{_10GBase_SR_LR,			  MAC_NA,     NA_NUM,	 PCS_NA,	 NA_NUM,	FEC_OFF,  SPEED_NA,  0,		   0,		  0,		XAUI_MEDIA,  _10BIT_NA},
	{_20GBase_SR_LR,			  MAC_NA,     NA_NUM,	 PCS_NA,	 NA_NUM,	FEC_OFF,  SPEED_NA,  0,	       0,		  0,		XAUI_MEDIA,  _10BIT_NA},
	{_40GBase_SR_LR,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{_12_5GBase_KR,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{XLHGL_KR4,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{HGL16G,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{HGS,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{HGS4,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{_100GBase_SR10,			  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{CHGL_LR12,					  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{TCAM,						  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{INTLKN_12Lanes_6_25G,		  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{INTLKN_16Lanes_6_25G,        MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{INTLKN_24Lanes_6_25G,		  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{INTLKN_12Lanes_10_3125G,	  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{INTLKN_16Lanes_10_3125G,	  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{INTLKN_12Lanes_12_5G,        MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{INTLKN_16Lanes_12_5G,        MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{INTLKN_16Lanes_3_125G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{INTLKN_24Lanes_3_125G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{CHGL11_LR12,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{INTLKN_4Lanes_3_125G,        MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{INTLKN_8Lanes_3_125G,        MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{INTLKN_4Lanes_6_25G,         MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{INTLKN_8Lanes_6_25G,         MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{_2_5GBase_QX,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{_5GBase_DQX,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{_5GBase_HX,				  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
	{_12GBaseR,				      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
    {_5GBaseR,                    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
    {_48GBaseR,                   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
    {_12GBase_SR,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
    {_48GBase_SR,                 MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},

    {NON_SUP_MODE,			      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,	  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA}
};

/* array of all ports (32) init parameters */
MV_HWS_PORT_INIT_PARAMS *hwsPortsAlleycat3Params[] =
{
    hwsPort0InitParams,
	/* ports 4 - 20 init by hwsAlleycat3Ports0To23Cfg() 
	   since they are based on port 0 topology */
    NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL,
  
    hwsPort24InitParams,
	hwsPort25InitParams,
	hwsPort26InitParams,
	hwsPort27InitParams,
	hwsPort28InitParams,
    hwsPort29InitParams,
	NULL,
	hwsPort31InitParams
};

/* init ports 1 - 23 */
GT_STATUS hwsAlleycat3Ports1To23Cfg
(
	GT_U8                   devNum,
    GT_U32                  portGroup
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 portNum;
    GT_U32 portMode;

    for (portNum = 1; portNum <= 23; portNum++)
    {
        curPortParams = (MV_HWS_PORT_INIT_PARAMS *)hwsOsMallocFuncPtr(sizeof(hwsPort0InitParams));
        if (curPortParams == NULL)
        {
          return GT_NO_RESOURCE;
        }

        hwsPortsAlleycat3Params[portNum] = curPortParams;
        hwsOsMemCopyFuncPtr(curPortParams, hwsPort0InitParams, sizeof(hwsPort0InitParams));

        for (portMode = 0; portMode < NON_SUP_MODE; portMode++)
        {
            if (curPortParams[portMode].portMacType == MAC_NA)
            {
                continue;
            }

            if (((portNum % 4) != 0) && curPortParams[portMode].portStandard == QSGMII)
            {
                curPortParams[portMode].portMacNumber = portNum; /* in order to be able to check the link */
                curPortParams[portMode].portPcsNumber = portNum; /* in order to be able to set loopback on port */
                curPortParams[portMode].numOfActLanes = 0;
                curPortParams[portMode].activeLanesList = NULL;/* override the 'pointer' that was allocated from 'port 0' !!! */
                curPortParams[portMode].firstLaneNum = 0;

                continue;
            }

            curPortParams[portMode].portMacNumber = portNum;
            curPortParams[portMode].portPcsNumber = portNum;
            curPortParams[portMode].firstLaneNum = portNum / 4;

            /* init active lanes in ports DB for NETWORK ports */
            if (curPortParams[portMode].numOfActLanes != 0)
                mvHwsBuildActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)portMode);
        }
    }

    return GT_OK;
}

