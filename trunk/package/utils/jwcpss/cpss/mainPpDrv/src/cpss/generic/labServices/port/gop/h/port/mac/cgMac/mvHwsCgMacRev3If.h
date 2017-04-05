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
* mvHwsCgMacRev3If.h
*
* DESCRIPTION:
*       CG MAC interface
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/

#ifndef __mvHwsCgMacRev3If_H
#define __mvHwsCgMacRev3If_H

#include <mac/mvHwsMacIf.h>

/*******************************************************************************
* hwsCgMacRev3IfInit
*
* DESCRIPTION:
*       Init XLG MAC configuration sequences and IF functions.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS hwsCgMacRev3IfInit(MV_HWS_MAC_FUNC_PTRS *funcPtrArray);

/*******************************************************************************
* mvHwsCgMacRev3ClearChannelCfg
*
* DESCRIPTION:
*       Configures MAC advanced feature  accordingly.
*
* INPUTS:
*       devNum     - system device number
*       portGroup  - port group (core) number
*       macNum    - MAC number
*       txIpg      - IPG between 2 packets in LAN mode
*       txPreamble - length, in bytes, of preamble transmitted by this port.
*       rxPreamble - length, in bytes, of preamble supported for received packets.
*       txCrc      - TX CRC
*       rxCrc      - RX CRC
*
* OUTPUTS:
*       None
*
* RETURNS:
*       0  - on success
*       1  - on error
*
*******************************************************************************/
GT_STATUS mvHwsCgMacRev3ClearChannelCfg
(
    GT_U8                   devNum,
    GT_U32                  portGroup,
    GT_U32                  macNum,
    GT_U32                  txIpg,
    GT_U32                  txPreamble,
    GT_U32                  rxPreamble,
    MV_HWS_PORT_CRC_MODE    txCrc,
    MV_HWS_PORT_CRC_MODE    rxCrc
);

/*******************************************************************************
* hwsCgMacRev3IfClose
*
* DESCRIPTION:
*       Release all system resources allocated by MAC IF functions.
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       None.
*
*******************************************************************************/
void hwsCgMacRev3IfClose(void);

#endif /* __mvHwsCgMacIf_H */

