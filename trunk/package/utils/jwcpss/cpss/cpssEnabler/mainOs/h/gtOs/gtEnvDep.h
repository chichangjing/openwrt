/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* gtEnvDep.h
*
* DESCRIPTION:    Hardware environment depended types definition
*
* NOTE :
*       -- next comments relevant for the system that manipulate the CPSS via
*          the PSS
*       1. This file name remain gtEnvDep.h (like on PSS)
*       2. This file remain with __gtEnvDeph deliberately in order to be
*          duplicate with the PSS --> allowing to call the CPSS from the PSS
*          without any compiler warnings
*       3. This file need to be synchronized with the one in the PSS in order
*          to get correct behavior (not depended on the order of
*          #include <...H file> from CPSS and PSS)
*
* FILE REVISION NUMBER:
*       $Revision: 15 $
*
* COMMENTS:
*       Must be synchronized for
*           common/h/cpss/extServices/os/gtOs/gtEnvDep.h
*           mainPpDrv/src/cpss/generic/labServices/ddr/ddr3libv2/h/Os/gtOs/gtEnvDep.h
*           cpssEnabler/mainOs/h/gtOs/gtEnvDep.h
*
*******************************************************************************/
#ifndef __gtEnvDeph
#define __gtEnvDeph

/*******************************************************************************
********************************************************************************
* PART1: TYPES
*
* The folling types defined in this section:
*   GT_U8, GT_16, GT_32, GT_U8, GT_U16, GT_U32
*   GT_INTPTR, GT_UINTPTR, GT_PTRDIFF, GT_SIZE_T, GT_SSIZE_T
*   GT_CHAR, GT_CHAR_PTR
*   GT_VOID_PTR
*   GT_PTR
*   GT_U64
*   GT_FLOAT32, GT_FLOAT64
*   GT_BOOL
*
********************************************************************************
*******************************************************************************/

/* In c++ compiler typedef void is not allowed therefore define is used */
#define GT_VOID void

typedef void  *GT_VOID_PTR;
typedef char  GT_CHAR, *GT_CHAR_PTR;

/* Add indication for the simulation to know that GT_CHAR is implimented,
   so the simulation will not define it. */
#define IMPL_GT_CHAR

#if !defined(UNIX) && !defined(__KERNEL__)

typedef char  GT_8,   *GT_8_PTR;
typedef unsigned char  GT_U8,   *GT_U8_PTR;
typedef short GT_16,  *GT_16_PTR;
typedef unsigned short GT_U16,  *GT_U16_PTR;

#define VAR_32BIT   long

#if defined(MIPS64_CPU) || defined(INTEL64_CPU)
#undef VAR_32BIT
#define VAR_32BIT   int
#else

#if defined(CPU) && defined(SIMSPARCSOLARIS)
    #if (CPU==SIMSPARCSOLARIS)
        #undef VAR_32BIT
        #define VAR_32BIT   int
    #endif
#endif
#endif

typedef VAR_32BIT   GT_32,  *GT_32_PTR;
typedef unsigned VAR_32BIT   GT_U32,  *GT_U32_PTR;

#ifndef IS_64BIT_OS
typedef VAR_32BIT           GT_INTPTR;
typedef unsigned VAR_32BIT  GT_UINTPTR;
typedef VAR_32BIT           GT_PTRDIFF;
typedef unsigned VAR_32BIT  GT_SIZE_T;
typedef VAR_32BIT           GT_SSIZE_T;
#else /* IS_64BIT_OS */
#define VAR_64BIT           long long
typedef VAR_64BIT           GT_INTPTR;
typedef unsigned VAR_64BIT  GT_UINTPTR;
typedef VAR_64BIT           GT_PTRDIFF;
typedef unsigned VAR_64BIT  GT_SIZE_T;
typedef VAR_64BIT           GT_SSIZE_T;
#endif /* IS_64BIT_OS */

#else /* defined(UNIX) || defined(__KERNEL__) */
/* use system defined types */
#ifndef __KERNEL__
#   include <inttypes.h>
#   include <stddef.h>
#   include <sys/types.h>
#else
#   include <linux/version.h>
#   include <linux/types.h>
#   if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
#       ifndef _UINTPTR_T
#           define _UINTPTR_T
            typedef unsigned long uintptr_t;
#       endif
#   endif
    typedef uintptr_t intptr_t;
#endif
typedef int8_t      GT_8,   *GT_8_PTR;
typedef uint8_t     GT_U8,  *GT_U8_PTR;
typedef int16_t     GT_16,  *GT_16_PTR;
typedef uint16_t    GT_U16, *GT_U16_PTR;
typedef int32_t     GT_32,  *GT_32_PTR;
typedef uint32_t    GT_U32,  *GT_U32_PTR;

typedef intptr_t    GT_INTPTR;
typedef uintptr_t   GT_UINTPTR;
typedef ptrdiff_t   GT_PTRDIFF;
typedef size_t      GT_SIZE_T;
typedef ssize_t     GT_SSIZE_T;
#endif

typedef void   *GT_PTR;

typedef union
{
    GT_U8   c[8];
    GT_U16  s[4];
    GT_U32  l[2];
}GT_U64;

/* definitions for floating point types:
 GT_FLOAT32 - Single precision - exponent 8 bits, significand (mantissa) 23 bits
 GT_FLOAT64 - Double precision - exponent 11 bits, significand (mantissa) 52 bits */
typedef float GT_FLOAT32, *GT_FLOAT32_PTR;
typedef double GT_FLOAT64, *GT_FLOAT64_PTR;

/*
 * typedef: enum GT_BOOL
 *
 * Description: Enumeration of boolean.
 *
 * Enumerations:
 *    GT_FALSE - false.
 *    GT_TRUE  - true.
 */
typedef enum
{
    GT_FALSE = 0,
    GT_TRUE  = 1
} GT_BOOL;


/*******************************************************************************
********************************************************************************
* PART2: compiler related
*
* Disable some warnings for VC
* define VC macro to be used for preprocessor warning
*
********************************************************************************
*******************************************************************************/

#ifdef _VISUALC

/* Disable warning messages for VC W4 warnings level */

/* warning of casting of void * to function's pointer :
warning C4055: 'type cast' :
from data pointer 'void *' to function pointer
         'unsigned int (__cdecl *)(int ,unsigned long ,unsigned long *)'
*/
#pragma warning( disable : 4055 )


/* warning for code like " while(1) {....}":
  warning C4127: conditional expression is constant*/
#pragma warning( disable : 4127 )



/* define macro to be used for preprocessor warning
 * Usage:
 *    #pragma message (__LOC_WARN__ "the warning message")
 */
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __LOC_WARN__ __FILE__ "("__STR1__(__LINE__)") : warning: "


#endif /* _VISUALC */


/*******************************************************************************
********************************************************************************
* PART3: target related
*
* define GT_SYNC
*
********************************************************************************
*******************************************************************************/


/* added macro for memory barrier synchronize   */

#if defined(PPC_CPU) || defined(POWERPC_CPU)
#ifdef __GNUC__
#ifdef _FreeBSD
#define GT_SYNC __asm __volatile (" eieio")
#else
#define GT_SYNC __asm__("   eieio")
#endif
#else
#ifdef _DIAB_TOOL
#define GT_SYNC asm(" eieio ")
#else
#error
#endif
#endif
#endif

#define GT_SYNC_MIPS			\
	__asm__ __volatile__(			\
		".set	push\n\t"		\
		".set	noreorder\n\t"		\
		".set	mips2\n\t"		\
		"sync\n\t"			\
		".set	pop"			\
		: /* no output */		\
		: /* no input */		\
		: "memory")


#define GT_SYNC_INTEL64 \
  {int tmp; __asm__ __volatile__("cpuid" : "=a" (tmp) \
   : "0" (1) : "ebx", "ecx", "edx", "memory"); }

#define GT_SYNC_INTEL64_32 sync_core()

#ifdef MIPS_CPU
#define GT_SYNC GT_SYNC_MIPS
#endif

#ifdef MIPS64_CPU
#define GT_SYNC GT_SYNC_MIPS
#endif

#ifdef INTEL_CPU
#define GT_SYNC
#endif

#ifdef INTEL64_CPU
# ifdef GT_SYNC
#  undef GT_SYNC
# endif
# if defined(IS_64BIT_OS)
#  define GT_SYNC GT_SYNC_INTEL64
# else
#  ifdef __WORDSIZE
#   if __WORDSIZE == 64
#    define GT_SYNC GT_SYNC_INTEL64
#   endif
#  endif
# endif
# ifndef GT_SYNC
#  define GT_SYNC GT_SYNC_INTEL64_32
# endif
#endif

#ifdef CPU_ARM
#define GT_SYNC
#endif

#ifndef GT_SYNC
#define GT_SYNC
#endif

#endif   /* __gtEnvDeph */


