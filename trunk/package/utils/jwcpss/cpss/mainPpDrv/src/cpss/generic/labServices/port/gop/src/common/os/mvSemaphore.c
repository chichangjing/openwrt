/*******************************************************************************
   Copyright (C) Marvell International Ltd. and its affiliates
********************************************************************************
Marvell GNU General Public License FreeRTOS Exception

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the Lesser
General Public License Version 2.1 plus the following FreeRTOS exception.
An independent module is a module which is not derived from or based on
FreeRTOS.
Clause 1:
Linking FreeRTOS statically or dynamically with other modules is making a
combined work based on FreeRTOS. Thus, the terms and conditions of the GNU
General Public License cover the whole combination.
As a special exception, the copyright holder of FreeRTOS gives you permission
to link FreeRTOS with independent modules that communicate with FreeRTOS solely
through the FreeRTOS API interface, regardless of the license terms of these
independent modules, and to copy and distribute the resulting combined work
under terms of your choice, provided that:
1. Every copy of the combined work is accompanied by a written statement that
details to the recipient the version of FreeRTOS used and an offer by yourself
to provide the FreeRTOS source code (including any modifications you may have
made) should the recipient request it.
2. The combined work is not itself an RTOS, scheduler, kernel or related
product.
3. The independent modules add significant and primary functionality to
FreeRTOS and do not merely extend the existing functionality already present in
FreeRTOS.
Clause 2:
FreeRTOS may not be used for any competitive or comparative purpose, including
the publication of any form of run time or compile time metric, without the
express permission of Real Time Engineers Ltd. (this is the norm within the
industry and is intended to ensure information accuracy).
*******************************************************************************/

#include <common/os/mvSemaphore.h>

#if defined(CHX_FAMILY) || defined(EXMXPM_FAMILY)
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#define whoAmI()		0
#else
#include <common.h>
#endif


GT_U32 mvPexRead(GT_U8 devNum, GT_U32 addr)
{
#if defined(CHX_FAMILY) || defined(EXMXPM_FAMILY)
	GT_U32 data;

	cpssDrvPpHwInternalPciRegRead(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, addr, &data);
	return data;
#else
	return MV_REG_BYTE_READ(addr);
#endif
}

GT_STATUS mvPexWrite(GT_U8 devNum, GT_U32 addr, GT_U32 data)
{
#if defined(CHX_FAMILY) || defined(EXMXPM_FAMILY)
	return cpssDrvPpHwInternalPciRegWrite(devNum, CPSS_PORT_GROUP_UNAWARE_MODE_CNS, addr, data);
#else
	MV_REG_BYTE_WRITE(addr, data);
	return GT_OK;
#endif
}

GT_BOOL mvSemaLock(GT_U8 devNum, GT_U32 num)
{
	GT_U32 tmp;
	GT_U32 cpuId;
	if (num > MV_MAX_SEMA)
	{
		osPrintf("Error: Invalid semaphore number (%d)\n", num);
		return GT_FALSE;
	}

	cpuId = whoAmI();
	do
	{
		tmp = mvPexRead(devNum, MV_SEMA_REG_BASE + num);
	} while ((tmp & 0xFF) != cpuId);

	return GT_TRUE;
}

GT_BOOL mvSemaTryLock(GT_U8 devNum, GT_U32 num)
{
	GT_U32 tmp;
	if (num > MV_MAX_SEMA)
	{
		osPrintf("Error: Invalid semaphore number (%d)\n", num);
		return GT_FALSE;
	}

	tmp = mvPexRead(devNum, MV_SEMA_REG_BASE + num);
	if ((tmp & 0xFF) != whoAmI())
	{
		return GT_FALSE;
	}
	else
		return GT_TRUE;
}

GT_BOOL mvSemaUnlock(GT_U8 devNum, GT_U32 num)
{
	if (num > MV_MAX_SEMA)
	{
		osPrintf("Error: Invalid semaphore number (%d)\n", num);
		return GT_FALSE;
	}

	CHECK_STATUS(mvPexWrite(devNum, MV_SEMA_REG_BASE + num, 0xFF));
	return GT_TRUE;
}

