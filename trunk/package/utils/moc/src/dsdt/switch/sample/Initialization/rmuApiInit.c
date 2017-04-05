/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************

*
* init.c
*
* DESCRIPTION:
*       QD initialization module
*
* DEPENDENCIES:   Platform
*
* FILE REVISION NUMBER:
*
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "Wrapper.h"
#include "msApi.h"
#include "rmuPcap.h"

#define MSG_PRINT printf

GT_SYS_CONFIG   cfg;
GT_QD_DEV       diagDev;
GT_QD_DEV       *qddev=&diagDev;

static char* SWGetDeviceName(int devId)
{
	char* ret;
	switch(devId)
	{
		case GT_88E6190:
		case GT_88E6190X:
		case GT_88E6390:
        case GT_88E6290:
		case GT_88E6390X:
			ret = "Peridot";
			break;
		default:
			ret = "Unknown";
			break;
	}
	return ret;
}

static GT_BOOL SMIRead(GT_QD_DEV* dev, GT_U32 phyAddr,
               GT_U32 miiReg, GT_U32* value)
{
	int ret = 0;

	ret=readRegister(phyAddr, miiReg);
	*value = (GT_U16)ret;
	if(ret < 0)
	{
		return GT_FALSE;
	}
	else
	{
		return GT_TRUE;
	}
}
                                                                                
static GT_BOOL SMIWrite(GT_QD_DEV* dev, GT_U32 phyAddr,
                      GT_U32 miiReg, GT_U32 value)
{
	int ret = 0;
	ret = writeRegister(phyAddr, miiReg, value);

	if(ret < 0)
	{
		return GT_FALSE;
	}
	else
		return GT_TRUE;
}

static GT_BOOL RMURead(GT_QD_DEV* dev, GT_U32 phyAddr,
               GT_U32 miiReg, GT_U32* value)
{
	int ret = 0;
	ret = gtRmuReadRegister(dev, phyAddr, miiReg, value);
	if(ret < 0)
		return GT_FALSE;
	else
		return GT_TRUE;
}

static GT_BOOL RMUWrite(GT_QD_DEV* dev, GT_U32 phyAddr,
                      GT_U32 miiReg, GT_U32 value)
{
	int ret = 0;
	ret = gtRmuWriteRegister(dev, phyAddr, miiReg, value);
	if(ret < 0)
		return GT_FALSE;
	else
		return GT_TRUE;
}

/*
static GT_STATUS rmuEthTxRx(
                GT_U8 *req_packet,
                GT_U32 req_pktlen,
                GT_U8 **rsp_packet,
                GT_U32 *rsp_pktlen)
{
	*rsp_packet = req_packet;
	*rsp_pktlen = req_pktlen;
	return GT_OK;
}*/

static GT_STATUS RMURegAccess(GT_QD_DEV* dev, HW_DEV_REG_ACCESS *gtRegAccess)
{
    int ret = 0;

    ret = gtRmuRegAccess(dev, gtRegAccess);
    if(ret != GT_TRUE)
        return GT_FAIL;
    else
        return GT_OK;
}

#ifdef USE_SEMAPHORE
static GT_SEM osSemCreate(GT_SEM_BEGIN_STATE state)
{
	GT_SEM pSem;
	int count = (int)state;

	pSem = semaphoreCreate(count);

	return pSem;
}
static GT_STATUS osSemDelete(GT_SEM smid)
{
	int ret = 0 ;

	ret = semaphoreClose(smid);
	
	if(ret < 0)
		return GT_FAIL;
	else
		return GT_OK;
}
static GT_STATUS osSemWait(GT_SEM smid, GT_U32 timeOut)
{
	int ret = 0 ;

	ret = semaphoreWait(smid, timeOut);
	
	if(ret < 0)
		return GT_FAIL;
	else
		return GT_OK;
}
static GT_STATUS osSemSignal(GT_SEM smid)
{
	int ret = 0 ;

	ret = semaphoreRelease(smid);
	
	if(ret < 0)
		return GT_FAIL;
	else
		return GT_OK;
}
#endif

GT_STATUS qdStart(GT_QD_DEV* d, GT_U8 baseAddr, GT_U32 phyAddr, GT_U32 cpuPort)
{
	GT_STATUS status = GT_FAIL;
	GT_SYS_CONFIG   cfg;

	memset((char*)&cfg,0,sizeof(GT_SYS_CONFIG));
	memset((char*)d,0,sizeof(GT_QD_DEV));

	cfg.BSPFunctions.readMii   = SMIRead;
	cfg.BSPFunctions.writeMii  = SMIWrite;
#ifdef GT_RMGMT_ACCESS
	cfg.BSPFunctions.hwAccess  = RMURegAccess;
    cfg.BSPFunctions.rmuReadMii   = RMURead;
    cfg.BSPFunctions.rmuWriteMii  = RMUWrite;
	cfg.BSPFunctions.rmuTxRxPacket= rmuEthTxRx;
#endif

#ifdef USE_SEMAPHORE
	cfg.BSPFunctions.semCreate = osSemCreate;
	cfg.BSPFunctions.semDelete = osSemDelete;
	cfg.BSPFunctions.semTake   = osSemWait;
	cfg.BSPFunctions.semGive   = osSemSignal;
#else
	cfg.BSPFunctions.semCreate = NULL;
	cfg.BSPFunctions.semDelete = NULL;
	cfg.BSPFunctions.semTake   = NULL;
	cfg.BSPFunctions.semGive   = NULL;
#endif

	cfg.initPorts = GT_TRUE;
	cfg.cpuPortNum = qdLong2Char(cpuPort);

	if((status=qdLoadDriver(&cfg, d)) != GT_OK)
	{
		return status;
	}

	printf("Driver Load Success\n");

	return GT_OK;
}

void qdClose() 
{
	if (qddev->devEnabled)
		qdUnloadDriver(qddev);
}

/* 
 *	Look for any SOHO Switch devices that are attached to the SMI pair.
*/
GT_STATUS qdInit()
{
	GT_U8		baseAddr;
	GT_U32		phyAddr, cpuPort;
	GT_STATUS	status;

#ifdef USE_SMI_MULTI_ADDR
	baseAddr = 2;   /* depends on hardware configuration */
#else
	baseAddr = 0;
#endif

	cpuPort = 0;  /* Not use so far  */
	phyAddr = 0;  /* Not use so far  */

	status = qdStart(qddev,baseAddr,phyAddr, cpuPort);
	if (GT_OK != status)
	{
		return GT_FAIL;
	}

	return GT_OK;
}

void showDeviceInformation()
{
	printf("\r\nCurrent device information as follows:\n");
	printf("\tDevice Name      = %s\n", SWGetDeviceName((int) qddev->deviceId));
	printf("\tRevision         = %#x\n", qddev->revision);
	printf("\tDevice Enabled   = %s\n",  qddev->devEnabled? "Yes" : "No");
	printf("\tSMI Address      = %#x\n", qddev->baseRegAddr);
	printf("\tNumber of Ports  = %d\n",  qddev->numOfPorts);
	printf("\tCPU Port         = %d\n",  qddev->cpuPortNum);
	printf("Type any key to return to main menu.");
	fflush(stdin);
	getchar();
}
