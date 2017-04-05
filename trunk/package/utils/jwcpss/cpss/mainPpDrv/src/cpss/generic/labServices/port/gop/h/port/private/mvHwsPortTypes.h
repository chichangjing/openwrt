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
* mvHwsPortTypes.h
*
* DESCRIPTION:
*       Ln2CpuPortunit registers and tables classes declaration.
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
*******************************************************************************/

#ifndef __mvHwsPortTypes_H
#define __mvHwsPortTypes_H

/* General H Files */
#include <common/siliconIf/mvSiliconIf.h>

/*
 * Typedef: enum MV_HWS_PORT_MAC_TYPE
 *
 * Description: Defines the different port MAC types.
 *
 */
typedef enum 
{
    MAC_NA,
    GEMAC_X,
    GEMAC_SG,
    XGMAC,
    XLGMAC,
    HGLMAC,
    CGMAC,
    INTLKN_MAC,
    TCAM_MAC,

    GEMAC_NET_X,
    GEMAC_NET_SG,

    QSGMII_MAC,

    LAST_MAC

}MV_HWS_PORT_MAC_TYPE;

/*
 * Typedef: enum MV_HWS_PORT_PCS_TYPE
 *
 * Description: Defines the different port PCS types.
 *
 */
typedef enum
{
    PCS_NA,
    GPCS,
    XPCS,
    MMPCS,
    HGLPCS,
    CGPCS,
    INTLKN_PCS,
    TCAM_PCS,

    GPCS_NET,

    QSGMII_PCS,

    LAST_PCS

}MV_HWS_PORT_PCS_TYPE;

/*
 * Typedef: enum MV_HWS_SERDES_SPEED
 *
 * Description: Defines the different values of serdes speed.
 *
 */
typedef enum
{
    SPEED_NA,
    _1_25G,
    _3_125G,
    _3_33G,
    _3_75G,
    _4_25G,
    _5G,
    _6_25G,
    _7_5G,
    _10_3125G,
    _11_25G,
    _11_5625G,
    _12_5G,
    _10_9375G,
    _12_1875G,
    _5_625G,
    LAST_MV_HWS_SERDES_SPEED

}MV_HWS_SERDES_SPEED;

/*
 * Typedef: enum MV_HWS_SERDES_MEDIA
 *
 * Description: Defines the serdes media.
 *
 */
typedef enum
{
    XAUI_MEDIA,
    RXAUI_MEDIA

}MV_HWS_SERDES_MEDIA;

/*
 * Typedef: enum MV_HWS_PORT_FEC_MODE
 *
 * Description: Defines the FEC status.
 *
 */
typedef enum 
{
    FEC_ON,
    FEC_OFF,
    FEC_NA

}MV_HWS_PORT_FEC_MODE;

/*
 * Typedef: enum MV_HWS_SERDES_10B_MODE
 *
 * Description: Defines the serdes 10Bit status.
 *
 */
typedef enum
{
    _10BIT_ON,
    _10BIT_OFF,
    _10BIT_NA,
    _20BIT_ON,
    _40BIT_ON

}MV_HWS_SERDES_10B_MODE;

/*
 * Typedef: enum MV_HWS_SERDES_TYPE
 *
 * Description: Defines the different SERDES types.
 *
 */
typedef enum
{
    COM_PHY_H,
    COM_PHY_H_REV2,
    COM_PHY_28NM,
    AVAGO,

    SERDES_LAST

}MV_HWS_SERDES_TYPE;

/* Return the mask including "numOfBits" bits.          */
#define BIT_MASK(numOfBits) (~(0xFFFFFFFF << (numOfBits)))

/* Calculate the field mask for a given offset & length */
/* e.g.: BIT_MASK(8,2) = 0xFFFFFCFF                     */
#define FIELD_MASK_NOT(offset,len)                      \
        (~(BIT_MASK((len)) << (offset)))

/* Sets the field located at the specified offset & length in data.     */
#define U32_SET_FIELD(data,offset,length,val)           \
   (data) = (((data) & FIELD_MASK_NOT((offset),(length))) | ((val) <<(offset)))

#endif /* __mvPortModeElements_H */

