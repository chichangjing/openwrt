/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* gtGenTypes.h
*
* DESCRIPTION:    Common types definitions
*
* DEPENDENCIES:   None.
*
* FILE REVISION NUMBER:
*       $Revision: 24 $
*******************************************************************************/

#ifndef __gtGenTypesh
#define __gtGenTypesh

#include <gtOs/gtEnvDep.h>

#ifndef __FUNCTION__
/* __func__ is even newer option can't use it */
#define __FUNCTION__ ""
#endif

/* The gcc is compild with -ansi that is inline is not supported the __inline__ is gcc alternative */
/* The prev #ifdef _VXWORKS has nothing to with it */
#ifndef GT_INLINE
# ifdef __GNUC__
#  define GT_INLINE __inline__
# else
#  ifdef _DIAB_TOOL
#   define GT_INLINE
#  else
#   define GT_INLINE
#  endif
# endif
#endif

#ifndef GT_PACKED
#ifdef __GNUC__
#define GT_PACKED __attribute__ ((packed))
#endif

/* override for VXWORKS && CPU == PPC603 */ 
#ifdef _VXWORKS
#if (CPU==PPC603)
#undef GT_PACKED
#endif
#endif
#endif

/* all other cases */
#ifndef GT_PACKED
#define GT_PACKED
#endif

#if !defined(GT_UNUSED) && defined(__GNUC__)
# define GT_UNUSED __attribute__ ((unused))
#endif
#ifndef GT_UNUSED
# define GT_UNUSED
#endif


#ifndef NULL
#define NULL ((void*)0)
#endif

#undef IN
#define IN
#undef OUT
#define OUT
#undef INOUT
#define INOUT

#if (!defined(__KERNEL__) && !defined(KERNEL) && !defined(INKERNEL) && !defined(_KERNEL))
    typedef void          (*GT_VOIDFUNCPTR) (); /* ptr to function returning void */
    typedef unsigned int  (*GT_INTFUNCPTR)  (); /* ptr to function returning int  */
    typedef void          (*GT_VOIDINTFUNCPTR) (int);/* ptr to function returning void */
#else
    typedef void          (*GT_VOIDFUNCPTR) (void); /* ptr to function returning void */
    typedef unsigned int  (*GT_INTFUNCPTR)  (void); /* ptr to function returning int  */
    typedef void          (*GT_VOIDINTFUNCPTR) (int);/* ptr to function returning void */
#endif

/*
 * Typedef: enum GT_COMP_RES
 *
 * Description: Values to be returned by compare function
 *
 */
typedef enum
{
    GT_EQUAL   = 0,
    GT_GREATER = 1,
    GT_SMALLER = 2
}GT_COMP_RES;


typedef unsigned int GT_STATUS;

/***** generic return codes **********************************/
#define GT_ERROR                 (-1)
#define GT_OK                    (0x00) /* Operation succeeded */
#define GT_FAIL                  (0x01) /* Operation failed    */

#define GT_BAD_VALUE             (0x02) /* Illegal value        */
#define GT_OUT_OF_RANGE          (0x03) /* Value is out of range*/
#define GT_BAD_PARAM             (0x04) /* Illegal parameter in function called  */
#define GT_BAD_PTR               (0x05) /* Illegal pointer value                 */
#define GT_BAD_SIZE              (0x06) /* Illegal size                          */
#define GT_BAD_STATE             (0x07) /* Illegal state of state machine        */
#define GT_SET_ERROR             (0x08) /* Set operation failed                  */
#define GT_GET_ERROR             (0x09) /* Get operation failed                  */
#define GT_CREATE_ERROR          (0x0A) /* Fail while creating an item           */
#define GT_NOT_FOUND             (0x0B) /* Item not found                        */
#define GT_NO_MORE               (0x0C) /* No more items found                   */
#define GT_NO_SUCH               (0x0D) /* No such item                          */
#define GT_TIMEOUT               (0x0E) /* Time Out                              */
#define GT_NO_CHANGE             (0x0F) /* The parameter is already in this value*/
#define GT_NOT_SUPPORTED         (0x10) /* This request is not support           */
#define GT_NOT_IMPLEMENTED       (0x11) /* This request is not implemented       */
#define GT_NOT_INITIALIZED       (0x12) /* The item is not initialized           */
#define GT_NO_RESOURCE           (0x13) /* Resource not available (memory ...)   */
#define GT_FULL                  (0x14) /* Item is full (Queue or table etc...)  */
#define GT_EMPTY                 (0x15) /* Item is empty (Queue or table etc...) */
#define GT_INIT_ERROR            (0x16) /* Error occurred while INIT process     */
#define GT_NOT_READY             (0x1A) /* The other side is not ready yet       */
#define GT_ALREADY_EXIST         (0x1B) /* Tried to create existing item         */
#define GT_OUT_OF_CPU_MEM        (0x1C) /* Cpu memory allocation failed.         */
#define GT_ABORTED               (0x1D) /* Operation has been aborted.           */
#define GT_NOT_APPLICABLE_DEVICE (0x1E) /* API not applicable to device , can
                                           be returned only on devNum parameter  */
#define GT_UNFIXABLE_ECC_ERROR   (0x1F) /* the CPSS detected ECC error that can't
                                           be fixed when reading from the memory which is protected by ECC.
                                           NOTE: relevant only when the table resides in the CSU ,
                                           the ECC is used , and the CPSS emulates the ECC detection
                                           and correction for 'Read entry' operations */
#define GT_UNFIXABLE_BIST_ERROR  (0x20) /* Built-in self-test detected unfixable error */
#define GT_CHECKSUM_ERROR        (0x21) /* checksum doesn't fits received data */
#define GT_DSA_PARSING_ERROR     (0x22) /* DSA tag parsing error */


#endif   /* __gtGenTypesh */


