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
 * @brief utilities for work with register bit fields.
 *
 * @file tm_register_processing.c
 *
 * $Revision: 2.0 $
 */

#include "tm_registers_processing.h"
/*#include "stdint.h" */
#include "tm_platform_implementation_definitions.h"

void __set_field(void * address, int shift, int width, int value)
{
	uint64_t * pvar=(uint64_t *) address;

	uint64_t mask= (1 << width) - 1 ;
	uint64_t uvalue=value;
	uvalue &= mask; /* removing extra hi bits (if any) */
	mask = ~(mask << shift);
	uvalue <<= shift;
	*pvar = (*pvar & mask) | uvalue;

}

unsigned int __get_field(void * address, int shift, int width)
{
	uint64_t * pvar=(uint64_t *) address;
	uint64_t uvalue;
	uint64_t mask= (1 <<width)-1;
	mask <<= shift;
	uvalue= (*pvar & mask);
	uvalue >>=shift;
	return (unsigned int) uvalue;
}
