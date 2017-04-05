#ifndef _SET_HW_REGISTERS_IMP_H_
#define _SET_HW_REGISTERS_IMP_H_


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
 * @brief useful macro wrappers for read/write/reset register functions.
 *
 * @file set_hw_registers_imp.h
 *
 * $Revision: 2.0 $
 */



#include  "tm_connection_management.h"
#include  "tm_rw_registers_interface.h"
#include  "tm_os_interface.h"



#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "tm_regs.h"

#define SHIFT_TABLE
/*#define STRUCTS */

#if defined(STRUCTS)
	#include "tm_payloads.h"
#elif defined(SHIFT_TABLE)
	#include "tm_regs_description.h"
#else

#endif

#include "tm_registers_processing.h"

#if __STDC_VERSION__ < 199901L
# if __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif

#define	COMPLETE_HW_WRITE	\
	if (0==rc) rc = flush_hw_connection(TM_ENV(ctl));\
	if (rc) reset_hw_connection(TM_ENV(ctl),rc);

#if defined(STRUCTS)
	#define	TM_RESET_REGISTER(address, register_name)	\
	/*	tm_printf(" function %s -  reset register reserved fields %s(0x%08X)... \n",__FUNCTION__,#address,address);*/\
		rc = tm_register_reset(TM_ENV(ctl), REGISTER_ADDRESS_ARG(address),  TM_REGISTER_VAR_ADDR(register_name));\
		/**/if (rc) tm_printf(" function %s -  failed to reset reserved fields for register %s(0x%08X) \n",__func__,#address,address);/**/
#elif defined(SHIFT_TABLE)
	#define	TM_RESET_REGISTER(address, register_name)	\
	TM_REGISTER_RESET(register_name)\
	rc = 0;
#else
	#define	TM_RESET_REGISTER(address, register_name)  /* do nothing */
#endif


#if defined(STRUCTS)
	#define	TM_RESET_TABLE_REGISTER(address, index , register_name)	\
	/*	tm_printf(" function %s -  reset table register reserved fields %s(base address 0x%08X , shift %d)  value 0x%016X\n",__FUNCTION__,#address,address,index, *((uint64_t*)TM_REGISTER_VAR_ADDR(register_name)));*/\
		rc = tm_table_entry_reset(TM_ENV(ctl), REGISTER_ADDRESS_ARG(address), index, TM_REGISTER_VAR_ADDR(register_name));\
		/**/if (rc) tm_printf(" function %s -  failed to reset reserved fields to table  register %s(0x%08X) , index %ld \n",__func__,#address,address,(long int)index);/**/
#elif defined(SHIFT_TABLE)
	#define	TM_RESET_TABLE_REGISTER(address, index , register_name)	\
	TM_REGISTER_RESET(register_name)\
	rc = 0;
#else
	#define	TM_RESET_TABLE_REGISTER(address,index , register_name)  /* do nothing */
#endif

extern int tm_log_write_requests;
extern int tm_log_read_requests;
extern void *  tm_pAliasingDummy;

#define	TM_WRITE_REGISTER(address, register_name)	\
	if(tm_log_write_requests)\
	{\
		tm_pAliasingDummy = (void*)TM_REGISTER_VAR_ADDR(register_name);\
		tm_printf(" function %s -  writing to register %s(0x%08X %08X) value 0x%016llX\n",__FUNCTION__,#address,address.l[1],address.l[0],*((uint64_t*)tm_pAliasingDummy));\
	}\
	/*	tm_printf(" function %s -  writing to register %s value 0x%016llX\n",__FUNCTION__,#address,*((uint64_t*)TM_REGISTER_VAR_ADDR(register_name)));*/\
	rc = tm_register_write(TM_ENV(ctl), REGISTER_ADDRESS_ARG(address),  TM_REGISTER_VAR_ADDR(register_name));\
    /**/if (rc) tm_printf(" function %s -  failed writing to register %s(0x%08X %08X) \n",__func__,#address,address.l[1],address.l[0]);/**/

#define	TM_READ_REGISTER(address, register_name)	\
	if(tm_log_read_requests) 	tm_printf(" function %s -  reading from register %s(0x%08X %08X) ... ",__FUNCTION__, #address, address.l[1], address.l[0]);\
	rc = tm_register_read(TM_ENV(ctl), REGISTER_ADDRESS_ARG(address),  TM_REGISTER_VAR_ADDR(register_name));\
	if(tm_log_read_requests)\
	{\
		if (rc==0)\
		{\
			tm_pAliasingDummy = (void*)TM_REGISTER_VAR_ADDR(register_name);\
			tm_printf(" result 0x%016llX\n",*((uint64_t*)tm_pAliasingDummy));\
		}\
		else tm_printf(" failed !\n");\
	} \
	/**/if (rc) tm_printf(" function %s -  failed to read from register %s(0x%08X %08X) \n",__func__,#address,address.l[1],address.l[0]);/**/

#define	TM_WRITE_TABLE_REGISTER(address, index , register_name)	\
	if(tm_log_write_requests)\
	{\
		tm_pAliasingDummy = (void*)TM_REGISTER_VAR_ADDR(register_name);\
		tm_printf(" function %s -  writing to table register %s(base address 0x%08X %08X) , shift %ld)  value 0x%016llX\n",__FUNCTION__,#address,address.l[1],address.l[0],index, *((uint64_t*)tm_pAliasingDummy));\
	}\
	/*	tm_printf(" function %s -  writing to table register %s , shift %ld  value 0x%016llX\n",__FUNCTION__, #address, index, *((uint64_t*)TM_REGISTER_VAR_ADDR(register_name)));*/\
	rc = tm_table_entry_write(TM_ENV(ctl), REGISTER_ADDRESS_ARG(address), index, TM_REGISTER_VAR_ADDR(register_name));\
    /**/if (rc) tm_printf(" function %s -  failed writing to table  %s (0x%08X %08X) , index %ld \n",__func__,#address,address.l[1],address.l[0],(long int)index);/**/

#define	TM_READ_TABLE_REGISTER(address, index , register_name)	\
	if(tm_log_read_requests) 	tm_printf(" function %s -  reading from  table register %s(base address 0x%08X %08X , shift %d) ...",__FUNCTION__,#address,address.l[1],address.l[0],index);\
	rc = tm_table_entry_read(TM_ENV(ctl), REGISTER_ADDRESS_ARG(address), index, TM_REGISTER_VAR_ADDR(register_name));\
	if(tm_log_read_requests)\
	{\
		if (rc==0)\
		{\
			tm_pAliasingDummy = (void*)TM_REGISTER_VAR_ADDR(register_name);\
			tm_printf(" result 0x%016llX\n",*((uint64_t*)tm_pAliasingDummy));\
		}\
		else tm_printf(" failed !\n");\
	} \
    /**/if (rc) tm_printf(" function %s -  failed to read from table %s (0x%08X %08X) , index %ld \n",__func__,#address,address.l[1],address.l[0],(long int)index);/**/



#endif  /* _SET_HW_REGISTERS_IMP_H_ */
