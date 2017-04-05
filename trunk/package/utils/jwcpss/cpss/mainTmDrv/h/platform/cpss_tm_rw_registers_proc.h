#ifndef _CPSS_TM_RW_REGISTERS_PROC_H_
#define _CPSS_TM_RW_REGISTERS_PROC_H_

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
 * @brief utilities for read/write registers in cpss.
 *
 * @file cpss_tm_registers_proc.h
 *
 * $Revision: 2.0 $
 */


#include <cpss/generic/tm/cpssTmPublicDefs.h>


/* Return the mask including "numOfBits" bits. for 0..31 bits   */
#define TM_BIT_MASK_0_31(numOfBits) (~(0xFFFFFFFF << (numOfBits)))

/* the macro of TM_BIT_MASK_0_31() in VC will act in wrong way for 32 bits, and will
   result 0 instead of 0xffffffff
   so macro TM_BIT_MASK - is improvement of TM_BIT_MASK_0_31 to support 32 bits
*/
#define TM_BIT_MASK(numOfBits)    ((numOfBits) == 32 ? 0xFFFFFFFF : TM_BIT_MASK_0_31(numOfBits))

/* Calculate the field mask for a given offset & length */
/* e.g.: TM_FIELD_MASK_NOT(8,2) = 0xFFFFFCFF            */
#define TM_FIELD_MASK_NOT(offset, len)                    \
        (~(TM_BIT_MASK((len)) << (offset)))

/* Calculate the field mask for a given offset & length */
/* e.g.: TM_FIELD_MASK(8,2) = 0x00000300                */
#define TM_FIELD_MASK(offset, len)                        \
        ((TM_BIT_MASK((len)) << (offset)))

/* Returns the info located at the specified offset & length in data. */
#define TM_U32_GET_FIELD(data, offset, length)            \
        (((data) >> (offset)) & TM_BIT_MASK(length))

/* Sets the field located at the specified offset & length in data.   */
#define TM_U32_SET_FIELD(data, offset, length, val)       \
   (data) = (((data) & TM_FIELD_MASK_NOT((offset),(length))) | ((val)<<(offset)))



int tm_read_register_proc(GT_U8 devNumber,GT_U8	burstMode,  GT_U64 reg_addr,  void *data);

int tm_write_register_proc(GT_U8 devNumber,GT_U8	burstMode,  GT_U64 reg_addr,  void *data);



#endif	/* _TM_RW_REGISTERS_INTERFACE_H_ */
