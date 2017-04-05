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

#define MV_BOBK_NW_PORT_START   56

/************************* definition *****************************************************/


/************************* Globals ********************************************************/

GT_U32 bobKPorts0To47SupModesMap[LAST_PORT_MODE] =
{
    NA_NUM, /* _100Base_FX             */
    0,      /* SGMII                   */
    1,      /* _1000Base_X             */
    2,      /* SGMII2_5                */
    3,      /* QSGMII                  */
    NA_NUM, /* _10GBase_KX4            */
    NA_NUM, /* _10GBase_KX2            */
    NA_NUM, /* _10GBase_KR             */
    NA_NUM, /* _20GBase_KR             */
    NA_NUM, /* _40GBase_KR             */
    NA_NUM, /* _100GBase_KR10          */
    NA_NUM, /* HGL                     */
    NA_NUM, /* RHGL                    */
    NA_NUM, /* CHGL                    */
    NA_NUM, /* RXAUI                   */
    NA_NUM, /* _20GBase_KX4            */
    NA_NUM, /* _10GBase_SR_LR          */
    NA_NUM, /* _20GBase_SR_LR          */
    NA_NUM, /* _40GBase_SR_LR          */
    NA_NUM, /* _12_5GBase_KR           */
    NA_NUM, /* XLHGL_KR4               */
    NA_NUM, /* HGL16G                  */
    NA_NUM, /* HGS                     */
    NA_NUM, /* HGS4                    */
    NA_NUM, /* _100GBase_SR10          */
    NA_NUM, /* CHGL_LR12               */
    NA_NUM, /* TCAM                    */
    NA_NUM, /* INTLKN_12Lanes_6_25G    */
    NA_NUM, /* INTLKN_16Lanes_6_25G    */
    NA_NUM, /* INTLKN_24Lanes_6_25G    */
    NA_NUM, /* INTLKN_12Lanes_10_3125G */
    NA_NUM, /* INTLKN_16Lanes_10_3125G */
    NA_NUM, /* INTLKN_12Lanes_12_5G    */
    NA_NUM, /* INTLKN_16Lanes_12_5G    */
    NA_NUM, /* INTLKN_16Lanes_3_125G   */
    NA_NUM, /* INTLKN_24Lanes_3_125G   */
    NA_NUM, /* CHGL11_LR12             */
    NA_NUM, /* INTLKN_4Lanes_3_125G    */
    NA_NUM, /* INTLKN_8Lanes_3_125G    */
    NA_NUM, /* INTLKN_4Lanes_6_25G     */
    NA_NUM, /* INTLKN_8Lanes_6_25G     */
    NA_NUM, /* _2_5GBase_QX            */
    NA_NUM, /* _5GBase_DQX             */
    NA_NUM, /* _5GBase_HX              */
    NA_NUM, /* _12GBaseR               */
    NA_NUM, /* _5GBaseR                */
    NA_NUM, /* _48GBaseR               */
    NA_NUM, /* _12GBase_SR             */
    NA_NUM  /* _48GBase_SR             */
};

GT_U32 bobKPorts56To71SupModesMap[LAST_PORT_MODE] =
{
    NA_NUM, /* _100Base_FX             */
    0,      /* SGMII                   */
    1,      /* _1000Base_X             */
    2,      /* SGMII2_5                */
    3,      /* QSGMII                  */
    4,      /* _10GBase_KX4            */
    5,      /* _10GBase_KX2            */
    6,      /* _10GBase_KR             */
    7,      /* _20GBase_KR             */
    8,      /* _40GBase_KR             */
    NA_NUM, /* _100GBase_KR10          */
    NA_NUM, /* HGL                     */
    NA_NUM, /* RHGL                    */
    NA_NUM, /* CHGL                    */
    9,      /* RXAUI                   */
    10,     /* _20GBase_KX4            */
    11,     /* _10GBase_SR_LR          */
    12,     /* _20GBase_SR_LR          */
    13,     /* _40GBase_SR_LR          */
    14,     /* _12_5GBase_KR           */
    15,     /* XLHGL_KR4               */
    NA_NUM, /* HGL16G                  */
    NA_NUM, /* HGS                     */
    NA_NUM, /* HGS4                    */
    NA_NUM, /* _100GBase_SR10          */
    NA_NUM, /* CHGL_LR12               */
    NA_NUM, /* TCAM                    */
    NA_NUM, /* INTLKN_12Lanes_6_25G    */
    NA_NUM, /* INTLKN_16Lanes_6_25G    */
    NA_NUM, /* INTLKN_24Lanes_6_25G    */
    NA_NUM, /* INTLKN_12Lanes_10_3125G */
    NA_NUM, /* INTLKN_16Lanes_10_3125G */
    NA_NUM, /* INTLKN_12Lanes_12_5G    */
    NA_NUM, /* INTLKN_16Lanes_12_5G    */
    NA_NUM, /* INTLKN_16Lanes_3_125G   */
    NA_NUM, /* INTLKN_24Lanes_3_125G   */
    NA_NUM, /* CHGL11_LR12             */
    NA_NUM, /* INTLKN_4Lanes_3_125G    */
    NA_NUM, /* INTLKN_8Lanes_3_125G    */
    NA_NUM, /* INTLKN_4Lanes_6_25G     */
    NA_NUM, /* INTLKN_8Lanes_6_25G     */
    NA_NUM, /* _2_5GBase_QX            */
    NA_NUM, /* _5GBase_DQX             */
    NA_NUM, /* _5GBase_HX              */
    16,     /* _12GBaseR               */
    NA_NUM, /* _5GBaseR                */
    17,     /* _48GBaseR               */
    NA_NUM, /* _12GBase_SR             */
    NA_NUM  /* _48GBase_SR             */
};

static MV_HWS_PORT_INIT_PARAMS hwsPort0InitParams[] = {
  /* port mode,   MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {SGMII,         GEMAC_NET_SG,0,        GPCS_NET,		0,	  FEC_OFF,    _1_25G,        0,		  1,		0,		XAUI_MEDIA,  _10BIT_ON},
  {_1000Base_X,   GEMAC_NET_X, 0,        GPCS_NET,		0,	  FEC_OFF,    _1_25G,        0,		  1,		0,		XAUI_MEDIA,  _10BIT_ON},
  {SGMII2_5,      GEMAC_NET_X, 0,        GPCS_NET,		0,	  FEC_OFF,    _3_125G,       0,		  1,		0,		XAUI_MEDIA,  _10BIT_ON},
  {QSGMII,        QSGMII_MAC,  0,        QSGMII_PCS,	0,	  FEC_OFF,    _5G ,          0,		  1,		0,		XAUI_MEDIA,  _10BIT_OFF},
};

static MV_HWS_PORT_INIT_PARAMS hwsPort1InitParams[] = {
    /* port mode,   MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
    {QSGMII,        QSGMII_MAC,  1,        QSGMII_PCS,	1,	  FEC_OFF,    _5G ,          0,		  0,		0,		XAUI_MEDIA,  _10BIT_OFF},
};

static MV_HWS_PORT_INIT_PARAMS hwsPort56InitParams[] = {
  /* port mode,   MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
  {SGMII,          GEMAC_SG,   56,        GPCS,         56,   FEC_OFF,   _1_25G,        27,       1,        0,      XAUI_MEDIA,   _10BIT_ON},
  {_1000Base_X,    GEMAC_X,    56,        GPCS,         56,   FEC_OFF,   _1_25G,        27,       1,        0,      XAUI_MEDIA,   _10BIT_ON},
  {SGMII2_5,       GEMAC_X,    56,        GPCS,         56,   FEC_OFF,   _3_125G,       27,       1,        0,      XAUI_MEDIA,   _10BIT_ON},
  {QSGMII,         QSGMII_MAC, 56,        QSGMII_PCS,   56,   FEC_OFF,   _5G ,          27,       1,        0,      XAUI_MEDIA,   _10BIT_OFF},
  {_10GBase_KX4,   XGMAC,      56,        XPCS,         56,   FEC_OFF,   _3_125G,       24,       4,        0,      XAUI_MEDIA,   _10BIT_NA},
  {_10GBase_KX2,   XGMAC,      56,        XPCS,         56,   FEC_OFF,   _6_25G,        26,       2,        0,      XAUI_MEDIA,   _10BIT_NA},
  {_10GBase_KR,    XLGMAC,     56,        MMPCS,        56,   FEC_OFF,   _10_3125G,     27,       1,        0,      XAUI_MEDIA,   _10BIT_NA},
  {_20GBase_KR,    XLGMAC,     56,        MMPCS,        56,   FEC_OFF,   _10_3125G,     26,       2,        0,      XAUI_MEDIA,   _10BIT_NA},
  {_40GBase_KR,    XLGMAC,     56,        MMPCS,        56,   FEC_OFF,   _10_3125G,     24,       4,        0,      XAUI_MEDIA,   _10BIT_NA},
  {RXAUI,          XGMAC,	   56,		  XPCS,         56,   FEC_OFF,   _6_25G,        26,       2,        0,      RXAUI_MEDIA,  _10BIT_NA},
  {_20GBase_KX4,   XGMAC,      56,        XPCS,         56,   FEC_OFF,   _6_25G,        24,       4,        0,      XAUI_MEDIA,   _10BIT_NA},
  {_10GBase_SR_LR, XLGMAC,     56,        MMPCS,        56,   FEC_OFF,   _10_3125G,     27,       1,        0,      XAUI_MEDIA,   _10BIT_NA},
  {_20GBase_SR_LR, XLGMAC,     56,        MMPCS,        56,   FEC_OFF,   _10_3125G,     26,       2,        0,      XAUI_MEDIA,   _10BIT_NA},
  {_40GBase_SR_LR, XLGMAC,     56,        MMPCS,        56,   FEC_OFF,   _10_3125G,     24,       4,        0,      XAUI_MEDIA,   _10BIT_NA},
  {_12_5GBase_KR,  XLGMAC,     56,        MMPCS,        56,   FEC_OFF,   _12_5G,        27,       1,        0,      XAUI_MEDIA,   _10BIT_NA},
  {XLHGL_KR4,      XLGMAC,     56,        MMPCS,        56,   FEC_OFF,   _12_5G,        24,       4,        0,      XAUI_MEDIA,   _10BIT_NA},
  {_12GBaseR,      XLGMAC,     56,        MMPCS,        56,   FEC_OFF,   _12_1875G,     27,       1,        0,      XAUI_MEDIA,   _10BIT_OFF},
  {_48GBaseR,      XLGMAC,     56,        MMPCS,        56,   FEC_OFF,   _12_1875G,     24,       4,        0,      XAUI_MEDIA,   _10BIT_OFF},
};

static MV_HWS_PORT_INIT_PARAMS hwsPort62InitParams[] = {
    /* port mode,   MAC Type    Mac Num    PCS Type    PCS Num    FEC       Speed      FirstLane LanesNum  LanesPtr    Media         10Bit */
    {SGMII,          GEMAC_SG,   62,        GPCS,         62,   FEC_OFF,   _1_25G,        27,       1,        0,      XAUI_MEDIA,   _10BIT_ON},
    {_1000Base_X,    MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {SGMII2_5,       MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {QSGMII,         MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_10GBase_KX4,   MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_10GBase_KX2,   MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_10GBase_KR,    MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_20GBase_KR,    MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_40GBase_KR,    MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {RXAUI,          MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_20GBase_KX4,   MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_10GBase_SR_LR, MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_20GBase_SR_LR, MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_40GBase_SR_LR, MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_12_5GBase_KR,  MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {XLHGL_KR4,      MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_12GBaseR,      MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_48GBaseR,      MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_12GBase_SR,    MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
    {_48GBase_SR,    MAC_NA,    NA_NUM,     PCS_NA,     NA_NUM, FEC_OFF,   SPEED_NA,      0,        0,        0,      XAUI_MEDIA,   _10BIT_NA},
};

/* array of all ports (72) init parameters */
MV_HWS_PORT_INIT_PARAMS *hwsPortsBobKParams[] =
{
    /* ports 1 - 48 init by hwsBobcatPortsParamsCfg() */
    hwsPort0InitParams,
    hwsPort1InitParams,
    NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* till port 47 */
    NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, /* ports 48 - 55 are not in use */
    hwsPort56InitParams,
    NULL,
    NULL,
    NULL,
    NULL,NULL,                               /* ports 60 - 61 are not in use */
    hwsPort62InitParams,
    NULL,                                    /* ports 63 is not in use */
    NULL,NULL,NULL,NULL,                     /* port 64 - 67*/
    NULL,NULL,NULL,NULL                      /* port 68 - 71*/
};

/* store serdes num for ports 56 - 71 */
GT_U32 bobK10GPortSerdesMapping[] =
    {27,     26,     25, 24,     /* 56 - 59 */
     NA_NUM, NA_NUM, 20, NA_NUM, /* 60 - 63 */
     35,     34,     33, 32,     /* 64 - 67 */
     31,     30,     29, 28};    /* 68 - 71 */

GT_U32 bobK20GPortSerdesMapping[] =
    {26,     24,      /* 56 & 58 */
     NA_NUM, NA_NUM,  /* 60 & 62 */
     34,     32,      /* 64 & 66 */
     30,     28};     /* 68 & 70 */

GT_U32 bobK40GPortSerdesMapping[] =
    {24,      /* 56 */
     NA_NUM,  /* 60 */
     32,      /* 64 */
     28};     /* 68 */

GT_U32 bobKQsgmiiGPortSerdesMapping[] =
    {27,      /* 56 */
     NA_NUM,  /* 60 */
     35,      /* 64 */
     31};     /* 68 */

/************************* functions ******************************************************/

GT_U32 hwsBobKPortParamIndexGet
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  phyPortNum,
    MV_HWS_PORT_STANDARD    portMode
)
{
    GT_U32* bobKMapTable;

    /* avoid warnings */
    devNum = devNum;
    portGroup = portGroup;

    if (hwsPortsBobKParams[phyPortNum] == NULL)
    {
        return NA_NUM;
    }

    if(hwsBobcat3ForceModeGet())
    {
        return bobKPorts56To71SupModesMap[portMode];
    }

    /* QSGMII ports requires valid entry (to allow port functionality
       such as link check and loopback) but it won;t be used to initialize
       the port (since all ports are initialized by first port of QSGMII) */
    if ((phyPortNum <= 47) && (phyPortNum % 4))
    {
        return (portMode == QSGMII) ? 0 : NA_NUM;
    }

    /* for all other port modes */
    bobKMapTable = (phyPortNum <= 47) ? bobKPorts0To47SupModesMap : bobKPorts56To71SupModesMap;

    return bobKMapTable[portMode];
}

void hwsPortsParamsReset(MV_HWS_PORT_INIT_PARAMS* portParams)
{
    portParams->portMacType = portParams->portPcsType = 0;
    portParams->portMacNumber = portParams->portPcsNumber = NA_NUM;
    portParams->firstLaneNum = 0;
}

/* init ports 1 - 47 */
GT_STATUS hwsBobKPorts0To47Cfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 portNum;
    GT_U32 portMode;
    GT_U32 portModeIdx;
    GT_BOOL isQsgmiiOnlyPort;
    GT_U32 portParamsArrSize;
    MV_HWS_PORT_INIT_PARAMS* portParamsArr;

    if(hwsBobcat3ForceModeGet())
    {
        /* no need for such 48 ports ... see hwsBobKPorts56To71Cfg(...) */
        return GT_OK;
    }

    for (portNum = 0; portNum < 48; portNum++)
    {
        if(hwsPortsBobKParams[portNum] != NULL)
        {
            continue; /* no need to allocate since it's already exists (such as port 0) */
        }

        isQsgmiiOnlyPort = (portNum % 4) ? GT_TRUE : GT_FALSE;

        portParamsArrSize = (isQsgmiiOnlyPort) ? sizeof(hwsPort1InitParams) : sizeof(hwsPort0InitParams);
        portParamsArr = (isQsgmiiOnlyPort) ? hwsPort1InitParams : hwsPort0InitParams;

        curPortParams = (MV_HWS_PORT_INIT_PARAMS *)hwsOsMallocFuncPtr(portParamsArrSize);
        if (curPortParams == NULL)
        {
            return GT_NO_RESOURCE;
        }

        /* assign entry to array */
        hwsPortsBobKParams[portNum] = curPortParams;

        /* initialize entry with default port params */
        hwsOsMemCopyFuncPtr(curPortParams, portParamsArr, portParamsArrSize);

        /* update entry according to port params */
        for (portMode = 0; portMode < LAST_PORT_MODE; portMode++)
        {
            portModeIdx = hwsBobKPortParamIndexGet(devNum, portGroup, portNum, portMode);

            if (portModeIdx == NA_NUM)
            {
                continue;
            }

            curPortParams[portModeIdx].portMacNumber = portNum;
            curPortParams[portModeIdx].portPcsNumber = portNum;
            curPortParams[portModeIdx].firstLaneNum = portNum / 4;

            /* init active lanes in ports DB for NETWORK ports */
            /*CHECK_STATUS(mvHwsBuildActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)portMode));*/
        }
    }

    return GT_OK;
}

/* init ports 56 - 71 */
GT_STATUS hwsBobKPorts56To71Cfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams;
    GT_U32 portNum;
    GT_U32 portMode;
    GT_U32 portModeIdx;

    if(hwsBobcat3ForceModeGet())
    {
        portNum = 0;/* allow all ports to be XLG+Gig */
    }
    else
    {
        portNum = MV_BOBK_NW_PORT_START;
    }

    for (/*already initialized*/; portNum < hwsDeviceSpecInfo[devNum].portsNum; portNum++)
    {
        if(hwsBobcat3ForceModeGet())
        {
            /* allocate for all ports in the loop */
        }
        else
        if(hwsPortsBobKParams[portNum] != NULL)
        {
            continue; /* no need to allocate since it's already exists (such as port 62) */
        }

        /* allocate entry */
        curPortParams = (MV_HWS_PORT_INIT_PARAMS *)hwsOsMallocFuncPtr(sizeof(hwsPort56InitParams));
        if (curPortParams == NULL)
        {
          return GT_NO_RESOURCE;
        }

        /* assign entry to array */
        hwsPortsBobKParams[portNum] = curPortParams;

        /* initialize entry with default port params */
        hwsOsMemCopyFuncPtr(curPortParams, hwsPort56InitParams, sizeof(hwsPort56InitParams));

        /* update entry according to port params */
        for (portMode = 0; portMode < LAST_PORT_MODE; portMode++)
        {
            portModeIdx = hwsBobKPortParamIndexGet(devNum, portGroup, portNum, portMode);

            /* check that this portMode is supported*/
            if (portModeIdx == NA_NUM || curPortParams[portModeIdx].numOfActLanes == 0)
            {
                continue;
            }

            curPortParams[portModeIdx].portMacNumber = portNum;
            curPortParams[portModeIdx].portPcsNumber = portNum;
            curPortParams[portModeIdx].firstLaneNum = NA_NUM; /* set by default to NA. if relevant if would be changed */

            /* set first lane num according to port mode and serdes mapping. in case it is not supported
               for this port num, reset its params */

            if(hwsBobcat3ForceModeGet())
            {
                if((portNum % curPortParams[portModeIdx].numOfActLanes) == 0)
                {
                    curPortParams[portModeIdx].firstLaneNum = portNum;
                }
            }
            else
            /* check if 40G port mode */
            if((curPortParams[portModeIdx].numOfActLanes == 4) && ((portNum % 4) == 0))
            {
                curPortParams[portModeIdx].firstLaneNum = bobK40GPortSerdesMapping[(portNum - MV_BOBK_NW_PORT_START)/4];
            }
            /* check if 20G port mode */
            else if((curPortParams[portModeIdx].numOfActLanes == 2) && ((portNum % 2) == 0))
            {
                curPortParams[portModeIdx].firstLaneNum = bobK20GPortSerdesMapping[(portNum - MV_BOBK_NW_PORT_START)/2];
            }
            /* check if 10G port mode */
            else if(curPortParams[portModeIdx].numOfActLanes == 1)
            {
                if(portMode == QSGMII)
                {
                    /* QSGMII is single lane port mode with special Serdes mapping */
                    curPortParams[portModeIdx].firstLaneNum = bobKQsgmiiGPortSerdesMapping[(portNum - MV_BOBK_NW_PORT_START)/4];
                }
                else
                {
                    curPortParams[portModeIdx].firstLaneNum = bobK10GPortSerdesMapping[portNum - MV_BOBK_NW_PORT_START];
                }
            }

            /* for invalid entries */
            if(curPortParams[portModeIdx].firstLaneNum == NA_NUM)
            {
                hwsPortsParamsReset(&curPortParams[portModeIdx]);
            }

            /* init active lines in ports DB for NETWORK ports */
            /*
            if (curPortParams[portModeIdx].numOfActLanes != 0)
                CHECK_STATUS(mvHwsBuildActiveLaneList(devNum, portGroup, portNum, (MV_HWS_PORT_STANDARD)portMode));
            */
        }
    }

    return GT_OK;
}

GT_STATUS hwsBobKPortsElementsCfg
(
    GT_U8              devNum,
    GT_U32             portGroup
)
{
    CHECK_STATUS(hwsBobKPorts0To47Cfg(devNum, portGroup));
    CHECK_STATUS(hwsBobKPorts56To71Cfg(devNum, portGroup));

    /* build active lane list */
    CHECK_STATUS(mvHwsBuildDeviceLaneList(devNum, portGroup));

    return GT_OK;
}

void hwsBobKPortsParamsCfgGet
(
   GT_U32               portNum,
   MV_HWS_PORT_STANDARD portMode,
   GT_U32               *macNum,
   GT_U32               *pcsNum,
   GT_U32               *sdVecSize,
   GT_U32               **sdVector
)
{
    MV_HWS_PORT_INIT_PARAMS *curPortParams = hwsPortsBobKParams[portNum];
    GT_U32 portModeIdx = hwsBobKPortParamIndexGet(0, 0, portNum, portMode);

    *macNum = curPortParams[portModeIdx].portMacNumber;
    *pcsNum = curPortParams[portModeIdx].portPcsNumber;

    if (curPortParams[portModeIdx].numOfActLanes != 0)
    {
      *sdVecSize = curPortParams[portModeIdx].numOfActLanes;
      *sdVector = curPortParams[portModeIdx].activeLanesList;
    }

    return;
}

