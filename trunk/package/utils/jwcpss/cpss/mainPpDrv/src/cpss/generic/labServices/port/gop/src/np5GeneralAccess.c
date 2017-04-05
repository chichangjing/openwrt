/*******************************************************************************
*
*         Copyright 2003, MARVELL SEMICONDUCTOR ISRAEL, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED AS IS. MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), MARVELL SEMICONDUCTOR ISRAEL. (MSIL),  MARVELL TAIWAN, LTD. AND      *
* SYSKONNECT GMBH.                                                             *
********************************************************************************
* wrapEvents.h
*
* DESCRIPTION:
*       This file implements wrapper functions for the events library
*
* DEPENDENCIES:
*
* FILE REVISION NUMBER:
*       $Revision: 8 $
*******************************************************************************/

/********* Include files ******************************************************/
#include <gtOs/gtGenTypes.h>
#include <common/os/gtOs.h>
#include <common/configElementDb/mvCfgElementDb.h>


extern GT_STATUS np5GenRegisterSet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 data, GT_U32 mask);
extern GT_STATUS np5GenRegisterGet (GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 *data, GT_U32 mask);

extern GT_STATUS hwIfSetReg
(
 IN   GT_U8    devNum,
 IN   GT_U32   regAddr,
 IN   GT_U32   data
 );
extern GT_STATUS hwIfSetMaskReg
(
 IN   GT_U8    devNum,
 IN   GT_U32   regAddr,
 IN   GT_U32   mask,
 IN   GT_U32   data
 );
extern GT_STATUS hwIfGetReg
(
 IN   GT_U8    devNum,
 IN   GT_U32   regAddr,
 OUT  GT_U32*  pData
 );
extern GT_STATUS hwIfGetMaskReg
(
 IN   GT_U8    devNum,
 IN   GT_U32   regAddr,
 IN   GT_U32   mask,
 OUT  GT_U32*  pData
);

GT_STATUS genRegisterSet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 data, GT_U32 mask)
{
	GT_STATUS rc;

	if((address >> 28) == 0x1)
	{
		/* implemented in mainDrv */
		return np5GenRegisterSet(devNum, portGroup, address & 0xFFFFFFF, data, mask);
	}

	if (mask == 0)
    {
        rc = (hwIfSetReg(devNum, address, data));
    }
    else
    {
		rc = (hwIfSetMaskReg(devNum, address,mask,data));
    }

	return rc;
}


GT_STATUS genRegisterGet (GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 *data, GT_U32 mask)
{
	GT_STATUS rc;

	if((address >> 28) == 0x1)
	{
		/* implemented in mainDrv */
		return  np5GenRegisterGet (devNum, portGroup, address & 0xFFFFFFF, data, mask);
	}
	

	if (portGroup > devNum)
    {
        devNum = portGroup;
    }
    if (mask == 0)
    {
        rc = hwIfGetReg((GT_U8)devNum, address, data);
    }
    else
    {
        rc = hwIfGetMaskReg((GT_U8)devNum, address, mask, data);
    }
	
    return rc;
}

#if 0
GT_STATUS genInterlakenRegSet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 data, GT_U32 mask)
{
	/* stub function */
	return GT_NOT_IMPLEMENTED;
}

GT_STATUS genInterlakenRegGet(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 *data, GT_U32 mask)
{
	/* stub function */
	return GT_NOT_IMPLEMENTED;
}
#endif

void mvApSetSharedMem(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 data)
{
	printf("mvApSetSharedMem is NOT IMPLEMENTED\n");
}

void mvApGetSharedMem(GT_U8 devNum, GT_U32 portGroup, GT_U32 address, GT_U32 *data)
{
	printf("mvApGetSharedMem is NOT IMPLEMENTED\n");
}

void coCpuSeqReadOperation(GT_U8 devNum, GT_U16 operAddr, MV_EL_DB_OPERATION *oper, MV_OP_PARAMS *params)
{
	printf("coCpuSeqReadOperation is NOT IMPLEMENTED\n");
}

/************************************************************************/
/*          Load firmware through Confi                                 */
/************************************************************************/

#include <confi2/confi2.h>


#ifdef WIN32
GT_CONFI_BUFFER  confiBuffer;
#else
extern GT_CONFI_BUFFER  confiBuffer;
#endif


GT_U32 *fwCode = NULL;
GT_U32 codeSize;

GT_BOOL debugLoad = GT_FALSE;
GT_BOOL swapLoad = GT_TRUE;

extern unsigned char* auiFirmware;
extern unsigned int uiFirmwareSize;

void loadCode()
{
	GT_U32 codeSize;
	GT_U8* tempBuf = confiBuffer.buf;
	GT_U32 i;
	GT_U32 j;
	
	GT_U8 szTempBuf[9];
	szTempBuf[8] = '\0';
	
	osPrintf("Load firmware code from Confi buffer\n");

	if (debugLoad)
	{
		osPrintf("xxx[] = { \n");
		for (i=0; i<200; i++)
		{
			osPrintf("'%c', ", confiBuffer.buf[i]);
		}
		osPrintf("}\n");
	}
	
	codeSize = confiBuffer.dataSize/9;

	if(fwCode != NULL)
    {
        osFree(fwCode);
    }

	osPrintf("Code size: %d\n", codeSize * sizeof(GT_U32));
	
    /* allocate buffer */
    fwCode = (GT_U32*)osMalloc(codeSize*sizeof(GT_U32));
    if(fwCode == NULL)
    {
		osPrintf("GT_OUT_OF_CPU_MEM\n");
		return;
    }

	if (debugLoad)
	{
		for (i = 0; i<100; i+=9)
		{
			for (j = 0; j<8; j++)
			{
				printf("%c", confiBuffer.buf[i+j]);
			}

			printf("\n");
		}
	}

	if (debugLoad)
	{
		osPrintf("\n******** COPYING  ********\n");
	}

	for (i = 0; i<codeSize; i++)
	{
		osMemCpy(szTempBuf, tempBuf, 8);
		sscanf(szTempBuf, "%x", &fwCode[i]);
		tempBuf +=9;
		
		if (debugLoad == GT_TRUE && i<100)
		{
			printf("%c%c%c%c%c%c%c%c:   ",szTempBuf[7], szTempBuf[6], szTempBuf[5], szTempBuf[4], szTempBuf[3], szTempBuf[2], szTempBuf[1], szTempBuf[0]); 
			printf("0x%x", fwCode[i]);
		}

		if (swapLoad == GT_TRUE)
		{
			fwCode[i] = (fwCode[i] & 0xFF) << 24 |
						(fwCode[i] & 0xFF00) << 8 | 
						(fwCode[i] & 0xFF0000) >> 8 | 
						(fwCode[i] & 0xFF000000) >> 24;
		}

		if (debugLoad == GT_TRUE && i<100)
		{
			printf("       0x%x\n", fwCode[i]);
		}
	}
	
	auiFirmware = (unsigned char*)fwCode;
	uiFirmwareSize = (unsigned int)(codeSize * sizeof(GT_U32));

	if (debugLoad)
	{
		osPrintf("\n******** Debug  ********\n");
		
		osPrintf("Size %d\n", uiFirmwareSize);

		for (i = 0; i<100; i+=4)
		{
			for (j = 0; j<4; j++)
			{
				printf("%02x", auiFirmware[i+j]);
			}
			
			printf("\n");
		}
	}
}

