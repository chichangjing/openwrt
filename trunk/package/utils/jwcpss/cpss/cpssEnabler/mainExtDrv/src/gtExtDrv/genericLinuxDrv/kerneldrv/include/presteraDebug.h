/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************
* presteraDebug.h
*
* DESCRIPTION:
*       This file includes the declaration of the struct we want to send to kernel mode,
*       from user mode.
*
* DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 3 $
*
*******************************************************************************/
#ifndef __INCPresteraDebugh
#define __INCPresteraDebugh

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#ifdef PRESTERA_DEBUG
#define _printk(x...) printk(x)
#define DBG_TRACE(func) (((int)(func)) | ((_printk("at %s:%d, calling %s\n", __FUNCTION__, __LINE__, TOSTRING(func)))&0))
#define DBG_TRACE_VOID(func) (func);_printk("at %s:%d, calling %s\n", __FUNCTION__, __LINE__, TOSTRING(func))
#else
#define _printk(x...)
#define DBG_TRACE(func) func
#define DBG_TRACE_VOID(func) func
#endif

#define _bspDmaRead(x...)               DBG_TRACE(bspDmaRead(x))
#define _bspDmaWrite(x...)              DBG_TRACE(bspDmaWrite(x))
#define _bspPciConfigReadReg(x...)      DBG_TRACE(bspPciConfigReadReg(x))
#define _bspPciConfigWriteReg(x...)     DBG_TRACE(bspPciConfigWriteReg(x))
#define _bspPciGetResourceStart(x...)   DBG_TRACE(bspPciGetResourceStart(x))
#define _bspPciGetResourceLen(x...)     DBG_TRACE(bspPciGetResourceLen(x))
#define _bspPciFindDev(x...)            DBG_TRACE(bspPciFindDev(x))
#define _bspPciFindDevReset(x...)       DBG_TRACE(bspPciFindDevReset(x))
#define _bspPciGetIntVec(x...)          DBG_TRACE(bspPciGetIntVec(x))
#define _bspTwsiInitDriver(x...)        DBG_TRACE(bspTwsiInitDriver(x))
#define _bspTwsiMasterReadTrans(x...)   DBG_TRACE(bspTwsiMasterReadTrans(x))
#define _bspTwsiMasterWriteTrans(x...)  DBG_TRACE(bspTwsiMasterWriteTrans(x))
#define _bspTwsiWaitNotBusy(x...)       DBG_TRACE(bspTwsiWaitNotBusy(x))
#define _bspVirt2Phys(x...)             DBG_TRACE(bspVirt2Phys(x))
#define _bspCacheDmaMalloc(x...)        DBG_TRACE(bspCacheDmaMalloc(x))
#define _bspHsuMalloc(x...)             DBG_TRACE(bspHsuMalloc(x))
#define _bspWarmRestart(x...)           DBG_TRACE_VOID(bspWarmRestart(x))
#define _bspCacheDmaFree(x...)          DBG_TRACE(bspCacheDmaFree(x))
#define _bspDragoniteSharedMemRead(x...)  DBG_TRACE(bspDragoniteSharedMemRead(x))
#define _bspDragoniteSharedMemWrite(x...) DBG_TRACE(bspDragoniteSharedMemWrite(x))
#define _bspDragoniteEnableSet(x...)    DBG_TRACE(bspDragoniteEnableSet(x))
#define _bspDragoniteGetIntVec(x...)    DBG_TRACE(bspDragoniteGetIntVec(x))
#define _bspDragoniteInit(x...)         DBG_TRACE(bspDragoniteInit(x))
#define _bspDragoniteSWDownload(x...)   DBG_TRACE(bspDragoniteSWDownload(x))
#define _bspDragoniteSharedMemoryBaseAddrGet(x...)  DBG_TRACE(bspDragoniteSharedMemoryBaseAddrGet(x))
#define _bspDragoniteFwCrcCheck(x...)   DBG_TRACE(bspDragoniteFwCrcCheck(x))

#define _bspEthCpuCodeToQueue(x...)    DBG_TRACE(bspEthCpuCodeToQueue(x))
#define _bspEthInit(x...)              DBG_TRACE_VOID(bspEthInit(x))
#define _bspEthInputHookAdd(x...)      DBG_TRACE(bspEthInputHookAdd(x))
#define _bspEthPortDisable(x...)       DBG_TRACE(bspEthPortDisable(x))
#define _bspEthPortEnable(x...)        DBG_TRACE(bspEthPortEnable(x))
#define _bspEthPortRxInit(x...)        DBG_TRACE(bspEthPortRxInit(x))
#define _bspEthPortTxInit(x...)        DBG_TRACE(bspEthPortTxInit(x))
#define _bspEthPortTxModeSet(x...)     DBG_TRACE(bspEthPortTxModeSet(x))
#define _bspEthPortTxQueue(x...)       DBG_TRACE(bspEthPortTxQueue(x))
#define _bspEthRxPacketFree(x...)      DBG_TRACE(bspEthRxPacketFree(x))
#define _bspEthTxCompleteHookAdd(x...) DBG_TRACE(bspEthTxCompleteHookAdd(x))
#define _bspPhys2Virt(x...)            DBG_TRACE(bspPhys2Virt(x))
#define _bspSmiReadReg(x...)           DBG_TRACE(bspSmiReadReg(x))
#define _bspSmiWriteReg(x...)          DBG_TRACE(bspSmiWriteReg(x))
#define _bspEthMuxSet(x...)            DBG_TRACE(bspEthMuxSet(x))
#define _bspEthMuxGet(x...)            DBG_TRACE(bspEthMuxGet(x))

#endif /* __INCPresteraDebug */
