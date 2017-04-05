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
* mvHwsCgMacRev3If.c
*
* DESCRIPTION:
*       CG MAC Puma3 B0 interface
*
* FILE REVISION NUMBER:
*       $Revision: 1 $
*
*******************************************************************************/
#include <common/siliconIf/mvSiliconIf.h>
#include <common/siliconIf/siliconAddress.h>

#include <mac/cgMac/mvHwsCgMacIf.h>
#include <mac/cgMac/mvHwsCgMacRev3If.h>
#include <mac/cgMac/mvHwsCgMacDb.h>

static char* mvHwsMacTypeGet(void)
{
  return "CGMAC_Rev3";
}
static GT_BOOL hwsInitDone = GT_FALSE;

/*******************************************************************************
* hwsCgMacIfInit
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
GT_STATUS hwsCgMacRev3IfInit(MV_HWS_MAC_FUNC_PTRS *funcPtrArray)
{
    if (hwsInitDone == GT_FALSE)
    {
        /* init sequences DB */
        CHECK_STATUS(hwsCgMacSeqInit());
    }

    funcPtrArray[CGMAC].macRestFunc = mvHwsCgMacReset;
    funcPtrArray[CGMAC].macModeCfgFunc = mvHwsCgMacModeCfg;
    funcPtrArray[CGMAC].macLinkGetFunc = mvHwsCgMacLinkStatus;
    funcPtrArray[CGMAC].macLbCfgFunc = mvHwsCgMacLoopbackCfg;
#ifndef CO_CPU_RUN
    funcPtrArray[CGMAC].macLbStatusGetFunc = mvHwsCgMacLoopbackStatusGet;
#endif
    funcPtrArray[CGMAC].macTypeGetFunc = mvHwsMacTypeGet;

    /* new functions */
    funcPtrArray[CGMAC].macClearChannelCfgFunc = mvHwsCgMacRev3ClearChannelCfg;
    funcPtrArray[CGMAC].macFcStateCfgFunc = NULL;

    hwsInitDone = GT_TRUE;

    return GT_OK;
}

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
*       txIpg      - Number of octets in steps of 4. Valid values are 8, 12, 16, ..., 100.
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
)
{
    GT_U32 accessAddr;
    GT_U32 baseAddr;
    GT_U32 unitIndex;
    GT_U32 data;

    mvUnitInfoGet(devNum, CG_UNIT, &baseAddr, &unitIndex);

    txCrc = txCrc;
    rxCrc = rxCrc;
    /* config TX IPG */
    accessAddr  = baseAddr + CG_TX_IPG_LENGTH + macNum * unitIndex;
    data = txIpg/4;
    CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, data, 0));

    /* config preamble */
    if ((txPreamble == rxPreamble) && (txPreamble == 4))
    {
        data = (1 << 30) + (1 << 21);
        accessAddr  = baseAddr + CG_COMMAND_CONFIG + macNum * unitIndex;
        CHECK_STATUS(genRegisterSet(devNum, portGroup, accessAddr, data, data));
    }
    return GT_OK;
}

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
void hwsCgMacRev3IfClose(void)
{
  hwsCgMacSeqFree();
  hwsInitDone = GT_FALSE;
}
