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
* mvHwsBobcat2PortModeElements.c
*
* DESCRIPTION:
*
*
* FILE REVISION NUMBER:
*       $Revision: 12 $
*
*******************************************************************************/

#include <private/mvPortModeElements.h>
#include <private/mvHwsPortPrvIf.h>
#include <common/siliconIf/mvSiliconIf.h>


static MV_HWS_PORT_INIT_PARAMS hwsPort0InitParams[] = {
  /* port mode,   MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {_100Base_FX,    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {SGMII,          GEMAC_NET_SG,0,        GPCS_NET,		0,		 FEC_OFF,    _1_25G,  0,		1,		   0,		 XAUI_MEDIA,  _10BIT_ON},
  {_1000Base_X,    GEMAC_NET_X, 0,        GPCS_NET,		0,		 FEC_OFF,    _1_25G,  0,		1,		   0,		 XAUI_MEDIA,  _10BIT_ON},
  {SGMII2_5,       GEMAC_NET_X, 0,        GPCS_NET,		0,		 FEC_OFF,    _3_125G, 0,		1,		   0,		 XAUI_MEDIA,  _10BIT_ON},
  {QSGMII,         QSGMII_MAC,  0,        QSGMII_PCS,	0,		 FEC_OFF,    _5G ,    0,		1,		   0,		 XAUI_MEDIA,  _10BIT_OFF},
  {_10GBase_KX4,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX2,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KR,    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_20GBase_KR,    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_KR,    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_KR10, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RHGL,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RXAUI,          MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_20GBase_KX4,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_SR_LR, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_20GBase_SR_LR, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_SR_LR, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12_5GBase_KR,  MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {XLHGL_KR4,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL16G,         MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS4,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_SR10, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL_LR12,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {TCAM,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL11_LR12,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_2_5GBase_QX,                MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_5GBase_DQX,                 MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_5GBase_HX,                  MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_12GBaseR,                   MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_5GBaseR,                    MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_48GBaseR,                   MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_12GBase_SR,                 MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_48GBase_SR,                 MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},

  {NON_SUP_MODE,                MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_NA,     SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA}
};

static MV_HWS_PORT_INIT_PARAMS hwsPort48InitParams[] = {
  /* port mode,   MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {_100Base_FX,    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {SGMII,          GEMAC_SG,   48,  GPCS,   48,  FEC_OFF,    _1_25G,    12,  1, 0,   XAUI_MEDIA, _10BIT_ON},
  {_1000Base_X,    GEMAC_X,    48,  GPCS,   48,  FEC_OFF,    _1_25G,    12,  1, 0,   XAUI_MEDIA, _10BIT_ON},
  {SGMII2_5,       GEMAC_X,    48,  GPCS,   48,  FEC_OFF,    _3_125G,   12,  1, 0,   XAUI_MEDIA, _10BIT_ON},
  {QSGMII,         QSGMII_MAC, 48,  QSGMII_PCS,48,  FEC_OFF,    _5G ,      12,  1, 0,   XAUI_MEDIA, _10BIT_OFF},
  {_10GBase_KX4,   XGMAC,      48,  XPCS,   48,  FEC_OFF,    _3_125G,   12,  4, 0,   XAUI_MEDIA, _10BIT_NA},
  {_10GBase_KX2,   XGMAC,      48,  XPCS,   48,  FEC_OFF,    _6_25G,    12,  2, 0,   XAUI_MEDIA, _10BIT_NA},
  {_10GBase_KR,    XLGMAC,     48,  MMPCS,  48,  FEC_OFF,    _10_3125G, 12,  1, 0,   XAUI_MEDIA, _10BIT_NA},
  {_20GBase_KR,    XLGMAC,     48,  MMPCS,  48,  FEC_OFF,    _10_3125G, 12,  2, 0,   XAUI_MEDIA, _10BIT_NA},
  {_40GBase_KR,    XLGMAC,     48,  MMPCS,  48,  FEC_OFF,    _10_3125G, 12,  4, 0,   XAUI_MEDIA, _10BIT_NA},
  {_100GBase_KR10, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RHGL,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RXAUI,          XGMAC,	   48,		  XPCS,  48,  FEC_OFF,    _6_25G,   12,   2, 0,   RXAUI_MEDIA, _10BIT_NA},
  {_20GBase_KX4,   XGMAC,      48,  XPCS,   48,  FEC_OFF,    _6_25G,    12,  4, 0,   XAUI_MEDIA, _10BIT_NA},
  {_10GBase_SR_LR, XLGMAC,     48,  MMPCS,  48,  FEC_OFF,    _10_3125G, 12,  1, 0,   XAUI_MEDIA, _10BIT_NA},
  {_20GBase_SR_LR, XLGMAC,     48,  MMPCS,  48,  FEC_OFF,    _10_3125G, 12,  2, 0,   XAUI_MEDIA, _10BIT_NA},
  {_40GBase_SR_LR, XLGMAC,     48,  MMPCS,  48,  FEC_OFF,    _10_3125G, 12,  4, 0,   XAUI_MEDIA, _10BIT_NA},
  {_12_5GBase_KR,  XLGMAC,     48,  MMPCS,  48,  FEC_OFF,    _12_5G,    12,  1, 0,   XAUI_MEDIA, _10BIT_NA},
  {XLHGL_KR4,      XLGMAC,     48,  MMPCS,  48,  FEC_OFF,    _12_5G,    12,  4, 0,   XAUI_MEDIA, _10BIT_NA},
  {HGL16G,         MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS4,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_SR10, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL_LR12,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {TCAM,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL11_LR12,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_2_5GBase_QX,                MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_5GBase_DQX,                 MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_5GBase_HX,                  MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_12GBaseR,                   XLGMAC,     48,         MMPCS,      48,         FEC_OFF,    _12_1875G,      12,     1,      0,      XAUI_MEDIA,     _10BIT_OFF},
  {_5GBaseR,                    MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_48GBaseR,                   XLGMAC,     48,         MMPCS,      48,         FEC_OFF,    _12_1875G,      12,     4,      0,      XAUI_MEDIA,     _10BIT_OFF},
  {_12GBase_SR,                 XLGMAC,     48,         MMPCS,      48,         FEC_OFF,    _12_1875G,      12,     1,      0,      XAUI_MEDIA,     _10BIT_OFF},
  {_48GBase_SR,                 XLGMAC,     48,         MMPCS,      48,         FEC_OFF,    _12_1875G,      12,     4,      0,      XAUI_MEDIA,     _10BIT_OFF},

  {NON_SUP_MODE,                MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_NA,     SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA}
};

static MV_HWS_PORT_INIT_PARAMS hwsPort49InitParams[] = {
  /* port mode,   MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {_100Base_FX,    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {SGMII,          GEMAC_SG,   49,  GPCS,   49,  FEC_OFF,    _1_25G,    13,  1, 0,   XAUI_MEDIA, _10BIT_ON},
  {_1000Base_X,    GEMAC_X,    49,  GPCS,   49,  FEC_OFF,    _1_25G,    13,  1, 0,   XAUI_MEDIA, _10BIT_ON},
  {SGMII2_5,       GEMAC_X,    49,  GPCS,   49,  FEC_OFF,    _3_125G,   13,  1, 0,   XAUI_MEDIA, _10BIT_ON},
  {QSGMII,         QSGMII_MAC, 49,  QSGMII_PCS,49,  FEC_OFF,    _5G ,      13,  1, 0,   XAUI_MEDIA, _10BIT_OFF},
  {_10GBase_KX4,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX2,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KR,    XLGMAC,     49,  MMPCS,  49,  FEC_OFF,    _10_3125G, 13,  1, 0,   XAUI_MEDIA, _10BIT_NA},
  {_20GBase_KR,    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_KR,    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_KR10, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RHGL,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RXAUI,          MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_20GBase_KX4,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_SR_LR, XLGMAC,     49,  MMPCS,  49,  FEC_OFF,    _10_3125G, 13,  1, 0,   XAUI_MEDIA, _10BIT_NA},
  {_20GBase_SR_LR, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_SR_LR, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12_5GBase_KR,  XLGMAC,     49,  MMPCS,  49,  FEC_OFF,    _12_5G,    13,  1, 0,   XAUI_MEDIA, _10BIT_NA},
  {XLHGL_KR4,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL16G,         MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS4,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_SR10, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL_LR12,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {TCAM,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL11_LR12,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_2_5GBase_QX,                MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_5GBase_DQX,                 MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_5GBase_HX,                  MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_12GBaseR,                   XLGMAC,     49,         MMPCS,      49,         FEC_OFF,    _12_1875G,      13,     1,      0,      XAUI_MEDIA,     _10BIT_OFF},
  {_5GBaseR,                    MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_48GBaseR,                   MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_12GBase_SR,                 XLGMAC,     49,         MMPCS,      49,         FEC_OFF,    _12_1875G,      13,     1,      0,      XAUI_MEDIA,     _10BIT_OFF},
  {_48GBase_SR,                 MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},

  {NON_SUP_MODE,                MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_NA,     SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA}
};

static MV_HWS_PORT_INIT_PARAMS hwsPort50InitParams[] = {
  /* port mode,   MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {_100Base_FX,    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {SGMII,          GEMAC_SG,   50,  GPCS,   50,  FEC_OFF,    _1_25G,    14,  1, 0,   XAUI_MEDIA, _10BIT_ON},
  {_1000Base_X,    GEMAC_X,    50,  GPCS,   50,  FEC_OFF,    _1_25G,    14,  1, 0,   XAUI_MEDIA, _10BIT_ON},
  {SGMII2_5,       GEMAC_X,    50,  GPCS,   50,  FEC_OFF,    _3_125G,   14,  1, 0,   XAUI_MEDIA, _10BIT_ON},
  {QSGMII,         QSGMII_MAC,   50,  QSGMII_PCS,   50,  FEC_OFF,    _5G ,      14,  1, 0,   XAUI_MEDIA, _10BIT_OFF},
  {_10GBase_KX4,   MAC_NA,     NA_NUM,      PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX2,   XGMAC,      50,  XPCS,   50,  FEC_OFF,    _6_25G,    14,  2, 0,   XAUI_MEDIA, _10BIT_NA},
  {_10GBase_KR,    XLGMAC,     50,  MMPCS,  50,  FEC_OFF,    _10_3125G, 14,  1, 0,   XAUI_MEDIA, _10BIT_NA},
  {_20GBase_KR,    XLGMAC,     50,  MMPCS,  50,  FEC_OFF,    _10_3125G, 14,  2, 0,   XAUI_MEDIA, _10BIT_NA},
  {_40GBase_KR,    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_KR10, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RHGL,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RXAUI,          XGMAC,      50,		  XPCS,		  50,		 FEC_OFF,   _6_25G,   14,		2,		   0,		 RXAUI_MEDIA,  _10BIT_NA},
  {_20GBase_KX4,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_SR_LR, XLGMAC,     50,  MMPCS,  50,  FEC_OFF,    _10_3125G, 14,  1, 0,   XAUI_MEDIA, _10BIT_NA},
  {_20GBase_SR_LR, XLGMAC,     50,  MMPCS,  50,  FEC_OFF,    _10_3125G, 14,  2, 0,   XAUI_MEDIA, _10BIT_NA},
  {_40GBase_SR_LR, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12_5GBase_KR,  XLGMAC,     50,  MMPCS,  50,  FEC_OFF,    _12_5G,    14,  1, 0,   XAUI_MEDIA, _10BIT_NA},
  {XLHGL_KR4,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL16G,         MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS4,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_SR10, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL_LR12,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {TCAM,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL11_LR12,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_2_5GBase_QX,                MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_5GBase_DQX,                 MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_5GBase_HX,                  MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_12GBaseR,                   XLGMAC,     50,         MMPCS,      50,         FEC_OFF,    _12_1875G,      14,     1,      0,      XAUI_MEDIA,     _10BIT_OFF},
  {_5GBaseR,                    MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_48GBaseR,                   MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_12GBase_SR,                 XLGMAC,     50,         MMPCS,      50,         FEC_OFF,    _12_1875G,      14,     1,      0,      XAUI_MEDIA,     _10BIT_OFF},
  {_48GBase_SR,                 MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},

  {NON_SUP_MODE,                MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_NA,     SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA}
};

static MV_HWS_PORT_INIT_PARAMS hwsPort51InitParams[] = {
  /* port mode,   MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {_100Base_FX,    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {SGMII,          GEMAC_SG,   51,  GPCS,   51,  FEC_OFF,    _1_25G,    15,  1, 0,   XAUI_MEDIA, _10BIT_ON},
  {_1000Base_X,    GEMAC_X,    51,  GPCS,   51,  FEC_OFF,    _1_25G,    15,  1, 0,   XAUI_MEDIA, _10BIT_ON},
  {SGMII2_5,       GEMAC_X,    51,  GPCS,   51,  FEC_OFF,    _3_125G,   15,  1, 0,   XAUI_MEDIA, _10BIT_ON},
  {QSGMII,         QSGMII_MAC,   51,  QSGMII_PCS,   51,  FEC_OFF,    _5G ,      15,  1, 0,   XAUI_MEDIA, _10BIT_OFF},
  {_10GBase_KX4,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KX2,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_KR,    XLGMAC,     51,  MMPCS,  51,  FEC_OFF,    _10_3125G, 15,  1, 0,   XAUI_MEDIA, _10BIT_NA},
  {_20GBase_KR,    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_KR,    MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_KR10, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RHGL,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {RXAUI,          MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_20GBase_KX4,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_10GBase_SR_LR, XLGMAC,     51,  MMPCS,  51,  FEC_OFF,    _10_3125G, 15,  1, 0,   XAUI_MEDIA, _10BIT_NA},
  {_20GBase_SR_LR, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_40GBase_SR_LR, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_12_5GBase_KR,  XLGMAC,     51,  MMPCS,  51,  FEC_OFF,    _12_5G,    15,  1, 0,   XAUI_MEDIA, _10BIT_NA},
  {XLHGL_KR4,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGL16G,         MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS,            MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {HGS4,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_NA,   SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_100GBase_SR10, MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL_LR12,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {TCAM,           MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,        0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_6_25G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_10_3125G,   MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_12Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_12_5G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_16Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_24Lanes_3_125G,     MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {CHGL11_LR12,               MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_3_125G,      MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_4Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {INTLKN_8Lanes_6_25G,       MAC_NA,     NA_NUM,    PCS_NA,     NA_NUM,    FEC_OFF,  SPEED_NA,  0,         0,         0,        XAUI_MEDIA,  _10BIT_NA},
  {_2_5GBase_QX,                MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_5GBase_DQX,                 MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_5GBase_HX,                  MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_12GBaseR,                   XLGMAC,     51,         MMPCS,      51,         FEC_OFF,    _12_1875G,      15,     1,      0,      XAUI_MEDIA,     _10BIT_OFF},
  {_5GBaseR,                    MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_48GBaseR,                   MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},
  {_12GBase_SR,                 XLGMAC,     51,         MMPCS,      51,         FEC_OFF,    _12_1875G,      15,     1,      0,      XAUI_MEDIA,     _10BIT_OFF},
  {_48GBase_SR,                 MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_OFF,    SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA},

  {NON_SUP_MODE,                MAC_NA,     NA_NUM,     PCS_NA,     NA_NUM,     FEC_NA,     SPEED_NA,       0,      0,      0,      XAUI_MEDIA,     _10BIT_NA}
};

/* array of all ports (72) init parameters */
MV_HWS_PORT_INIT_PARAMS *hwsPortsBobcatParams[] =
{
    /* ports 1 - 48 init by hwsBobcatPortsParamsCfg() */
    hwsPort0InitParams,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

    hwsPort48InitParams,
    hwsPort49InitParams,
    hwsPort50InitParams,
    hwsPort51InitParams,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL
};

/* store serdes num for ports 52 - 71 */
GT_U32 portSerdesNum[] =
    {16, 17, 18, 19, /* 52 - 55 */
     27, 26, 25, 24, /* 56 - 59 */
     23, 22, 21, 20, /* 60 - 63 */
     35, 34, 33, 32, /* 64 - 67 */
     31, 30, 29, 28};/* 68 - 71 */

GT_U32 port20GSerdesNum[] =
    {16, 18,  /* 52 & 54 */
     26, 24,  /* 56 & 58 */
     22, 20,  /* 60 & 62 */
     34, 32,  /* 64 & 66 */
     30, 28}; /* 68 & 70 */

GT_U32 port40GSerdesNum[] =
    {16,  /* 52 */
     24,  /* 56 */
     20,  /* 60 */
     32,  /* 64 */
     28}; /* 68 */

/* init ports 1 - 47 */
GT_STATUS hwsBobcatPorts47Cfg
(
	GT_U8                   devNum,
	GT_U32                  portGroup
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 portNum;
    GT_U32 portMode;

    for (portNum = 1; portNum < 48; portNum++)
    {
        curPortParams = (MV_HWS_PORT_INIT_PARAMS *)hwsOsMallocFuncPtr(sizeof(hwsPort0InitParams));
        if (curPortParams == NULL)
        {
          return GT_NO_RESOURCE;
        }
        hwsPortsBobcatParams[portNum] = curPortParams;
        hwsOsMemCopyFuncPtr(curPortParams, hwsPort0InitParams, sizeof(hwsPort0InitParams));

        for (portMode = 0; portMode <= hwsDeviceSpecInfo[devNum].lastSupPortMode; portMode++){
            if (curPortParams[portMode].portMacType == MAC_NA)
            {
                continue;
            }

            if (((portNum % 4) != 0) && curPortParams[portMode].portStandard == QSGMII)
            {
                curPortParams[portMode].portMacNumber = portNum; /* in order to be able to check the link */
                curPortParams[portMode].portPcsNumber = portNum; /* in order to be able to set loopback on port */
                curPortParams[portMode].numOfActLanes = 0;
                curPortParams[portMode].activeLanesList = NULL; /* override the 'pointer' that was allocated from 'port 0' !!! */
                curPortParams[portMode].firstLaneNum = portNum / 4;

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

/* init ports 52 - 71 */
GT_STATUS hwsBobcatPorts71Cfg
(
	GT_U8                   devNum,
	GT_U32                  portGroup
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 portNum;
    GT_U32  portMode;

    for (portNum = 52; portNum < 72; portNum++)
    {
        curPortParams = (MV_HWS_PORT_INIT_PARAMS *)hwsOsMallocFuncPtr(sizeof(hwsPort48InitParams));
        if (curPortParams == NULL)
        {
          return GT_NO_RESOURCE;
        }
        hwsPortsBobcatParams[portNum] = curPortParams;
        hwsOsMemCopyFuncPtr(curPortParams, hwsPortsBobcatParams[portNum - 4], sizeof(hwsPort48InitParams));

        for (portMode = 0; portMode <= hwsDeviceSpecInfo[devNum].lastSupPortMode; portMode++)
        {
            if (curPortParams[portMode].portMacType == MAC_NA)
            {
                continue;
            }
            curPortParams[portMode].portMacNumber = portNum;
            curPortParams[portMode].portPcsNumber = portNum;

			/* set first lane num according to port mode 
			   (for 10G they are reversed and this confuses mvHwsBuildActiveLaneList
			   if you use the same lane mapping for 20G and 40G */
			if(portMode == _10GBase_KX2 || portMode == RXAUI || portMode == _20GBase_KR || portMode == _20GBase_SR_LR)
			{
				curPortParams[portMode].firstLaneNum = port20GSerdesNum[(portNum - 52)/2];
			}
			else if(portMode == _10GBase_KX4 || portMode == _20GBase_KX4 || portMode == _40GBase_KR || 
				    portMode == _40GBase_SR_LR || portMode == XLHGL_KR4 || portMode == _48GBaseR || portMode == _48GBase_SR)
			{
				curPortParams[portMode].firstLaneNum = port40GSerdesNum[(portNum - 52)/4];
			}
			else
			{
				curPortParams[portMode].firstLaneNum = portSerdesNum[portNum - 52];
			}
            /* init active lines in ports DB for NETWORK ports */
            if (curPortParams[portMode].numOfActLanes != 0)
                mvHwsBuildActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)portMode);
        }
    }

    /* init Interlaken mode on ports 64 */
	curPortParams = hwsPortsBobcatParams[64];
    curPortParams[INTLKN_4Lanes_3_125G].firstLaneNum = 32;
    curPortParams[INTLKN_4Lanes_3_125G].numOfActLanes = 4;
    curPortParams[INTLKN_4Lanes_3_125G].portMacType = INTLKN_MAC;
    curPortParams[INTLKN_4Lanes_3_125G].portMacNumber = 0;
    curPortParams[INTLKN_4Lanes_3_125G].portPcsType = INTLKN_PCS;
    curPortParams[INTLKN_4Lanes_3_125G].portPcsNumber = 0;
    curPortParams[INTLKN_4Lanes_3_125G].portFecMode = FEC_OFF;
    curPortParams[INTLKN_4Lanes_3_125G].serdesSpeed = _3_125G;
    /* init active lines in ports DB for NETWORK ports */
    if (curPortParams[INTLKN_4Lanes_3_125G].numOfActLanes != 0)
        mvHwsBuildActiveLaneList(devNum, portGroup, 64, INTLKN_4Lanes_3_125G);

    curPortParams = hwsPortsBobcatParams[64];
    curPortParams[INTLKN_4Lanes_6_25G].firstLaneNum = 32;
    curPortParams[INTLKN_4Lanes_6_25G].numOfActLanes = 4;
    curPortParams[INTLKN_4Lanes_6_25G].portMacType = INTLKN_MAC;
    curPortParams[INTLKN_4Lanes_6_25G].portMacNumber = 0;
    curPortParams[INTLKN_4Lanes_6_25G].portPcsType = INTLKN_PCS;
    curPortParams[INTLKN_4Lanes_6_25G].portPcsNumber = 0;
    curPortParams[INTLKN_4Lanes_6_25G].portFecMode = FEC_OFF;
    curPortParams[INTLKN_4Lanes_6_25G].serdesSpeed = _6_25G;
    /* init active lines in ports DB for NETWORK ports */
    if (curPortParams[INTLKN_4Lanes_6_25G].numOfActLanes != 0)
        mvHwsBuildActiveLaneList(devNum, portGroup, 64, INTLKN_4Lanes_6_25G);

	curPortParams = hwsPortsBobcatParams[64];
    curPortParams[INTLKN_8Lanes_3_125G].firstLaneNum = 28;
    curPortParams[INTLKN_8Lanes_3_125G].numOfActLanes = 8;
    curPortParams[INTLKN_8Lanes_3_125G].portMacType = INTLKN_MAC;
    curPortParams[INTLKN_8Lanes_3_125G].portMacNumber = 0;
    curPortParams[INTLKN_8Lanes_3_125G].portPcsType = INTLKN_PCS;
    curPortParams[INTLKN_8Lanes_3_125G].portPcsNumber = 0;
    curPortParams[INTLKN_8Lanes_3_125G].portFecMode = FEC_OFF;
    curPortParams[INTLKN_8Lanes_3_125G].serdesSpeed = _3_125G;
	/* init active lines in ports DB for NETWORK ports */
    if (curPortParams[INTLKN_8Lanes_3_125G].numOfActLanes != 0)
        mvHwsBuildActiveLaneList(devNum, portGroup, 64, INTLKN_8Lanes_3_125G);

    curPortParams = hwsPortsBobcatParams[64];
    curPortParams[INTLKN_8Lanes_6_25G].firstLaneNum = 28;
    curPortParams[INTLKN_8Lanes_6_25G].numOfActLanes = 8;
    curPortParams[INTLKN_8Lanes_6_25G].portMacType = INTLKN_MAC;
    curPortParams[INTLKN_8Lanes_6_25G].portMacNumber = 0;
    curPortParams[INTLKN_8Lanes_6_25G].portPcsType = INTLKN_PCS;
    curPortParams[INTLKN_8Lanes_6_25G].portPcsNumber = 0;
    curPortParams[INTLKN_8Lanes_6_25G].portFecMode = FEC_OFF;
    curPortParams[INTLKN_8Lanes_6_25G].serdesSpeed = _6_25G;
    /* init active lines in ports DB for NETWORK ports */
    if (curPortParams[INTLKN_8Lanes_6_25G].numOfActLanes != 0)
        mvHwsBuildActiveLaneList(devNum, portGroup, 64, INTLKN_8Lanes_6_25G);

#if 0
	    curPortParams = hwsPortsBobcatParams[68];
    curPortParams[INTLKN_8Lanes_3_125G].firstLaneNum = 28;
    curPortParams[INTLKN_8Lanes_3_125G].numOfActLanes = 8;
    curPortParams[INTLKN_8Lanes_3_125G].portMacType = INTLKN_MAC;
    curPortParams[INTLKN_8Lanes_3_125G].portMacNumber = 0;
    curPortParams[INTLKN_8Lanes_3_125G].portPcsType = INTLKN_PCS;
    curPortParams[INTLKN_8Lanes_3_125G].portPcsNumber = 0;
    curPortParams[INTLKN_8Lanes_3_125G].portFecMode = FEC_OFF;
    curPortParams[INTLKN_8Lanes_3_125G].serdesSpeed = _3_125G;
	/* init active lines in ports DB for NETWORK ports */
    if (curPortParams[INTLKN_8Lanes_3_125G].numOfActLanes != 0)
        mvHwsBuildActiveLaneList(devNum, portGroup, 68, INTLKN_8Lanes_3_125G);

    curPortParams = hwsPortsBobcatParams[68];
    curPortParams[INTLKN_8Lanes_6_25G].firstLaneNum = 28;
    curPortParams[INTLKN_8Lanes_6_25G].numOfActLanes = 8;
    curPortParams[INTLKN_8Lanes_6_25G].portMacType = INTLKN_MAC;
    curPortParams[INTLKN_8Lanes_6_25G].portMacNumber = 0;
    curPortParams[INTLKN_8Lanes_6_25G].portPcsType = INTLKN_PCS;
    curPortParams[INTLKN_8Lanes_6_25G].portPcsNumber = 0;
    curPortParams[INTLKN_8Lanes_6_25G].portFecMode = FEC_OFF;
    curPortParams[INTLKN_8Lanes_6_25G].serdesSpeed = _6_25G;
    /* init active lines in ports DB for NETWORK ports */
    if (curPortParams[INTLKN_8Lanes_6_25G].numOfActLanes != 0)
        mvHwsBuildActiveLaneList(devNum, portGroup, 68, INTLKN_8Lanes_6_25G);
#endif

    return GT_OK;
}

void hwsBobcatPortsParamsCfgGet
(
   GT_U32               portNum,
   MV_HWS_PORT_STANDARD portMode,
   GT_U32               *macNum,
   GT_U32               *pcsNum,
   GT_U32               *sdVecSize,
   GT_U32               **sdVector
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams = hwsPortsBobcatParams[portNum];

    *macNum = curPortParams[portMode].portMacNumber;
    *pcsNum = curPortParams[portMode].portPcsNumber;

    if (curPortParams[portMode].numOfActLanes != 0)
    {
      *sdVecSize = curPortParams[portMode].numOfActLanes;
      *sdVector = curPortParams[portMode].activeLanesList;
    }
    return;
}

