/*
 * (c), Copyright 2009-2014, Marvell International Ltd.  (Marvell)
 *
 * This code contains confidential information of Marvell.
 * No rights are granted herein under any patent, mask work right or copyright
 * of Marvell or any third party. Marvell reserves the right at its sole
 * discretion to request that this code be immediately returned to Marvell.
 * This code is provided "as is". Marvell makes no warranties, expressed,
 * implied or otherwise, regarding its accuracy, completeness or performance.
 */
/**
 * @brief basic H/W read wtite utilities for read/write registers implementation for cpss
 *
 * @file cpss_hw_registers_proc.c
 *
 * $Revision: 2.0 $
 */

#include "cpss_tm_rw_registers_proc.h"

#include <cpssDriver/pp/hardware/prvCpssDrvHwCntl.h>
#include <cpssDriver/pp/hardware/cpssDriverPpHw.h>
#include <cpss/generic/cpssHwInit/private/prvCpssHwRegisters.h>


#include <stdio.h>


#define TM_GLOBAL_ADDR       0xC000000
#define TM_GLOBAL_REG        0xC0FFFF8
#define TM_GLOBAL_READ_REG   0xC0FFFF0

/* #define WRITE_REGISTERS_DUMP  */

#ifdef WRITE_REGISTERS_DUMP
	CPSS_OS_FILE	fregistersDump=CPSS_OS_FILE_INVALID;
	int		stringIndex=0;
#endif

/* #define RW_TEST  */
/* #define RW_TEST_LOG  */

#ifdef RW_TEST_LOG
	#define RW_TEST
	FILE *	fRWerrorLog=NULL;
#endif


int tm_log_write_requests = 0;
int tm_log_read_requests = 0;
void *  tm_pAliasingDummy=NULL;

/**
 */
int tm_read_register_proc(GT_U8 devNumber,GT_U8	burstMode,  GT_U64 reg_addr,  void *data)
{
    GT_STATUS   ret;
    GT_U32      regAddr[2];
    GT_U32      regValue[2];
    GT_U32      dontCare=0x00000123;

    /* 20 LSbits of Cider address */
    regAddr[0] = TM_U32_GET_FIELD(reg_addr.l[0],0,20) + TM_GLOBAL_ADDR;
    /* 23 MSbits of Cider address */
    regAddr[1] = TM_U32_GET_FIELD(reg_addr.l[0],20,12) | (TM_U32_GET_FIELD(reg_addr.l[1],0,11) << 12);

    if (burstMode)
	{

        ret = prvCpssHwPpWriteRam(devNumber, TM_GLOBAL_REG, 1, &(regAddr[1]));
        if (GT_OK != ret) return ret;

        ret = prvCpssHwPpReadRegister(devNumber, regAddr[0], &(regValue[0])); /* LSB */
        if (GT_OK != ret) return ret;
    
/*        ret = prvCpssHwPpReadRegister(devNumber, regAddr[0]+4, &(regValue[1]));*/ /* MSB */
        ret = prvCpssHwPpReadRegister(devNumber, TM_GLOBAL_READ_REG, &(regValue[1])); /* MSB */
        if (GT_OK != ret) return ret;    
        ((GT_U64 *)data)->l[0] = regValue[0];
        ((GT_U64 *)data)->l[1] = regValue[1];
    }
    else
    {
        ret = prvCpssHwPpWriteRam(devNumber, TM_GLOBAL_REG, 1, &(regAddr[1]));
        if (GT_OK != ret) return ret;

		ret = prvCpssHwPpWriteRam(devNumber, TM_GLOBAL_REG+4, 1, &dontCare);
        if (GT_OK != ret) return ret;

		ret = prvCpssHwPpReadRegister(devNumber, regAddr[0], &(regValue[0])); /* LSB */
        if (GT_OK != ret) return ret;
    
/*        ret = prvCpssHwPpReadRegister(devNumber, regAddr[0]+4, &(regValue[1]));*/ /* MSB */
        ret = prvCpssHwPpReadRegister(devNumber, TM_GLOBAL_READ_REG, &(regValue[1])); /* MSB */
        if (GT_OK != ret) return ret;
    
        ((GT_U64 *)data)->l[0] = regValue[0];
        ((GT_U64 *)data)->l[1] = regValue[1];
    }
    return ret;
}



int tm_write_register_proc(GT_U8 devNumber,GT_U8	burstMode,  GT_U64 reg_addr,  void *data)
{
    GT_STATUS   ret;
    GT_U32      regAddr[2];
    GT_U32      regValue[2];
    GT_U32      dontCare=0x00000123;
#ifdef RW_TEST
    GT_STATUS   read_ret;
    GT_U32      readValue[2];
#endif

    /* 20 LSbits of Cider address */
    regAddr[0] = TM_U32_GET_FIELD(reg_addr.l[0],0,20) + TM_GLOBAL_ADDR;
    /* 23 MSbits of Cider address */
    regAddr[1] = TM_U32_GET_FIELD(reg_addr.l[0],20,12) | (TM_U32_GET_FIELD(reg_addr.l[1],0,11) << 12);

    regValue[0] = ((GT_U64 *)data)->l[0];
    regValue[1] = ((GT_U64 *)data)->l[1];

#ifdef WRITE_REGISTERS_DUMP
	fregistersDump=cpssOsFopen("c:/temp/dtemp/write_registers_dump.txt","a");
	if (fregistersDump != CPSS_OS_FILE_INVALID)
	{
		cpssOsFprintf(fregistersDump,"%6d  address=0x%x %x     value=0x%08x%08x\n",stringIndex,regAddr[0],regAddr[1],regValue[1],regValue[0]);
		stringIndex++;
		cpssOsFclose(fregistersDump);
	}
#endif
	if (burstMode)
	{
        ret = prvCpssHwPpWriteRam(devNumber, TM_GLOBAL_REG, 1, &(regAddr[1]));
        if (GT_OK != ret) return ret;
    
        ret = prvCpssHwPpWriteRam(devNumber, regAddr[0], 2, regValue);
    }
    else
	{/*non burst mode*/
        ret = prvCpssHwPpWriteRam(devNumber, TM_GLOBAL_REG, 1, &(regAddr[1]));
        if (GT_OK != ret) return ret;

		ret = prvCpssHwPpWriteRam(devNumber, TM_GLOBAL_REG+4, 1, &dontCare);
        if (GT_OK != ret) return ret;

        ret = prvCpssHwPpWriteRam(devNumber, regAddr[0], 1, &(regValue[0]));
        if (GT_OK != ret) return ret;

        ret = prvCpssHwPpWriteRam(devNumber, regAddr[0]+4, 1, &(regValue[1]));
    }
#ifdef RW_TEST
	read_ret=tm_read_register_proc(devNumber,burstMode,reg_addr,&readValue) ;
	if (GT_OK != ret) return read_ret;
	if ((readValue[0] != regValue[0]) || (readValue[1] != regValue[1]))
	{
#ifdef RW_TEST_LOG
/*		fRWerrorLog=fopen("c:/temp/dtemp/rw_errors_log.txt","a");
		if (fRWerrorLog)
		{
*/
		cpssOsFprintf(CPSS_OS_FILE_STDERR,"address=0x%x %x   written_value=0x%08x%08x   read_value=0x%08x%08x \n",regAddr[0],regAddr[1],regValue[1],regValue[0],readValue[1],readValue[0]);
/*			fclose(fRWerrorLog);
		}
*/
#endif
		/* place to set breakpoint */
		read_ret=ret;
	}
#endif
	

    return ret;
}



#include  "tm_os_interface.h"



int tm_prv_print_rw_registers_log_status()
{
	tm_printf(" --------------------\n");
	tm_printf("\n  print write calls : ");
	tm_printf(tm_log_write_requests ? "yes" : "no");
	tm_printf("\n  print read  calls : ");
	tm_printf(tm_log_read_requests ? "yes" : "no");
	tm_printf("\n");
	return 0;
}


int tm_prv_set_rw_registers_log_status(int write_status, int read_status)
{
	switch (write_status)
	{ 
		case 0:
		case 1:break;
		default : tm_printf("wrong write flag value , abort ...\n"); return 1;
	}
	switch (read_status)
	{ 
		case 0:
		case 1:break;
		default : tm_printf("wrong read flag value , abort ...\n"); return 1;
	}
	tm_log_write_requests = write_status;
	tm_log_read_requests = read_status;
	return tm_prv_print_rw_registers_log_status();
}
