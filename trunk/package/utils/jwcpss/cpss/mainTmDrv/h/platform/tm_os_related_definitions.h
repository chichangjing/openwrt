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
 * @brief declaration & macro expanding of TM platform dependent parts of code
 *
 * @file tm_platform_implementation_definitions.h
 *
 * $Revision: 2.0 $
 */
#ifndef TM_OS_RELATED_DEFINITIONS_H 
#define TM_OS_RELATED_DEFINITIONS_H


#undef _TM_VC6
#ifdef _VISUALC
#if _MSC_VER == 1200
	#define _TM_VC6
#endif

#endif /* _VISUALC */

#ifdef _TM_VC6
	typedef unsigned int        uint32_t;
	typedef unsigned short int  uint16_t;
	typedef unsigned char       uint8_t;
#else /* _TM_VC6 */
#ifdef _VXWORKS
	#include <vxWorks.h>
#else /* _VXWORKS */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#endif /* _VXWORKS */
#endif /* _TM_VC6 */


#include <cpss/extServices/private/prvCpssBindFunc.h>

/* packing  attribute */
#define __ATTRIBUTE_PACKED__  


#endif  /* TM_OS_RELATED_DEFINITIONS_H */

