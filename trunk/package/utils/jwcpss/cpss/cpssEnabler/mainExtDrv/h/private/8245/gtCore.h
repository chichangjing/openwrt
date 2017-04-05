/*******************************************************************************
*                   Copyright 2002, GALILEO TECHNOLOGY, LTD.                   *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL.                      *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
*                                                                              *
* MARVELL COMPRISES MARVELL TECHNOLOGY GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, *
* MARVELL INTERNATIONAL LTD. (MIL), MARVELL TECHNOLOGY, INC. (MTI), MARVELL    *
* SEMICONDUCTOR, INC. (MSI), MARVELL ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K.  *
* (MJKK), GALILEO TECHNOLOGY LTD. (GTL) AND GALILEO TECHNOLOGY, INC. (GTI).    *
********************************************************************************
* gtCore.h - Header File for Basic gtCore logic functions and definitions
*
* DESCRIPTION:
*       This header file contains simple read/write macros for addressing
*       the SDRAM, devices, GT`s registers and PCI (using the PCI`s
*       address space). The macros take care of Big/Little endian conversions. 
*
* DEPENDENCIES:
*       None.
*
*       $Revision: 9 $
*
*******************************************************************************/

#ifndef __INCgtCoreh
#define __INCgtCoreh

#ifndef __KERNEL__
#if __GNUC__ >= 4
extern volatile unsigned int gtInternalRegBaseAddr; 
#define VOLATILE_UINT_LCAST
#else
extern unsigned int gtInternalRegBaseAddr; 
#define VOLATILE_UINT_LCAST  (volatile unsigned int)
#endif
#endif

/* defines  */
#define NONE_CACHEABLE(address) ((GT_UINTPTR)(address))
/****************************************/
/*          PCI Controller              */
/****************************************/
#define GT8245_INTERNAL_SPACE_SIZE		0x10000
#define GT8245_INTERNAL_SPACE_DEFAULT_ADDR	0x14000000
/****************************************/
/*          GENERAL Definitions         */
/****************************************/
#define NO_BIT          0x00000000
#define BIT0            0x00000001
#define BIT1            0x00000002
#define BIT2            0x00000004
#define BIT3            0x00000008
#define BIT4            0x00000010
#define BIT5            0x00000020
#define BIT6            0x00000040
#define BIT7            0x00000080
#define BIT8            0x00000100
#define BIT9            0x00000200
#define BIT10           0x00000400
#define BIT11           0x00000800
#define BIT12           0x00001000
#define BIT13           0x00002000
#define BIT14           0x00004000
#define BIT15           0x00008000
#define BIT16           0x00010000
#define BIT17           0x00020000
#define BIT18           0x00040000
#define BIT19           0x00080000
#define BIT20           0x00100000
#define BIT21           0x00200000
#define BIT22           0x00400000
#define BIT23           0x00800000
#define BIT24           0x01000000
#define BIT25           0x02000000
#define BIT26           0x04000000
#define BIT27           0x08000000
#define BIT28           0x10000000
#define BIT29           0x20000000
#define BIT30           0x40000000
#define BIT31           0x80000000

#ifndef _1K
#define _1K             0x00000400
#define _2K             0x00000800
#define _4K             0x00001000
#define _8K             0x00002000
#define _16K            0x00004000
#define _32K            0x00008000
#define _64K            0x00010000
#define _128K           0x00020000
#define _256K           0x00040000
#define _512K           0x00080000

#define _1M             0x00100000
#define _2M             0x00200000
#define _3M             0x00300000
#define _4M             0x00400000
#define _5M             0x00500000
#define _6M             0x00600000
#define _7M             0x00700000
#define _8M             0x00800000
#define _9M             0x00900000
#define _10M            0x00a00000
#define _11M            0x00b00000
#define _12M            0x00c00000
#define _13M            0x00d00000
#define _14M            0x00e00000
#define _15M            0x00f00000
#define _16M            0x01000000
#define _32M            0x02000000
#endif

/* Little to Big endian conversion macros */
               
#ifdef LE /* Little Endian */ 
    #ifndef  GT_SHORT_SWAP
        #define GT_SHORT_SWAP(X) (X)
    #endif
    #ifndef  GT_WORD_SWAP
        #define GT_WORD_SWAP(X) (X)
    #endif
    #ifndef  GT_LONG_SWAP
        #define GT_LONG_SWAP(X) ((l64)(X))
    #endif
#else    /* Big Endian */
    #ifndef  GT_SHORT_SWAP
        #define GT_SHORT_SWAP(X) ((X <<8 ) | (X >> 8))
    #endif
    
    #ifndef  GT_WORD_SWAP
        #define GT_WORD_SWAP(X) (((X)&0xff)<<24) |       \
                             (((X)&0xff00)<<8) |      \
                             (((X)&0xff0000)>>8) |    \
                             (((X)&0xff000000)>>24)
    #endif
    
    #ifndef  GT_LONG_SWAP
        #define GT_LONG_SWAP(X) ( (l64) (((X)&0xffULL)<<56) |             \
                                     (((X)&0xff00ULL)<<40) |              \
                                     (((X)&0xff0000ULL)<<24) |            \
                                     (((X)&0xff000000ULL)<<8) |           \
                                     (((X)&0xff00000000ULL)>>8) |         \
                                     (((X)&0xff0000000000ULL)>>24) |      \
                                     (((X)&0xff000000000000ULL)>>40) |    \
                                     (((X)&0xff00000000000000ULL)>>56))   
    #endif
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef __KERNEL__
/* Read/Write to/from GT`s registers */
#ifndef GT_REG_READ
    #define GT_REG_READ(offset, pData)                                      \
            *pData = ( (volatile unsigned int)*((unsigned int *)            \
                     (NONE_CACHEABLE(gtInternalRegBaseAddr) | (offset))) ); \
            *pData = GT_WORD_SWAP(*pData)
#endif    

#ifndef GTREGREAD
    #define GTREGREAD(offset)                                               \
            GT_WORD_SWAP( (volatile unsigned int)*((unsigned int *)         \
            (NONE_CACHEABLE(gtInternalRegBaseAddr) | (offset))) )

#endif    

#ifndef GT_REG_WRITE
    #define GT_REG_WRITE(offset, data)                                      \
            VOLATILE_UINT_LCAST *((unsigned int *)                          \
            (NONE_CACHEABLE(gtInternalRegBaseAddr) |                        \
            (offset))) = GT_WORD_SWAP(data)

#endif    

#endif /* no __KERNEL__ */

/* Write 32/16/8 bit NonCacheable */
#define GT_WRITE_CHAR(address, data)                                        \
        (volatile unsigned char)*((unsigned char *)                         \
        (NONE_CACHEABLE(address))) = (data)

#define GT_WRITE_SHORT(address, data)                                       \
        (volatile unsigned short)*((unsigned short *)                       \
        (NONE_CACHEABLE(address))) = (unsigned short)(data)
        
#define GT_WRITE_WORD(address, data)                                        \
        (volatile unsigned int)*((unsigned int *)                           \
        (NONE_CACHEABLE(address))) = (unsigned int)(data)

/* Write 32/16/8 bit Cacheable */
#define GT_WRITE_CHAR_CACHEABLE(address, data)                              \
        (volatile unsigned char)*((unsigned char *)                         \
        (CACHEABLE(address))) = (data)
                
#define GT_WRITE_SHORT_CACHEABLE(address, data)                             \
        (volatile unsigned short)*((unsigned short *)                       \
        (CACHEABLE(address))) = (unsigned short)(data)

#define GT_WRITE_WORD_CACHEABLE(address, data)                              \
        (volatile unsigned int)*((unsigned int *)                           \
        (CACHEABLE(address))) = (unsigned int)(data)
                
/* Read 32/16/8 bits NonCacheable - returns data in variable. */
#define GT_READ_CHAR(address, pData)                                        \
        *pData = (volatile unsigned char)*((volatile unsigned char *)       \
        (NONE_CACHEABLE(address)))
                
#define GT_READ_SHORT(address, pData)                                       \
        *pData = (volatile unsigned short)*((volatile unsigned short *)     \
        (NONE_CACHEABLE(address)))

#define GT_READ_WORD(address, pData)                                        \
        *pData = (volatile unsigned int)*((volatile unsigned int *)         \
        (NONE_CACHEABLE(address)))
        
/* Read 32/16/8 bit NonCacheable - returns data direct. */
#define GT_READCHAR(address)                                                \
        (volatile unsigned char)*((volatile unsigned char *)                \
        (NONE_CACHEABLE(address)))
                                                                             
#define GT_READSHORT(address)                                               \
        (volatile unsigned short)*((volatile unsigned short *)              \
        (NONE_CACHEABLE(address)))

#define GT_READWORD(address)                                                \
        (volatile unsigned int)*((volatile unsigned int *)                  \
        (NONE_CACHEABLE(address)))
                
/* Read 32/16/8 bit Cacheable */
#define GT_READ_CHAR_CACHEABLE(address, pData)                              \
        *pData = (unsigned char)*((unsigned char *)                         \
        (CACHEABLE(address)))
                
#define GT_READ_SHORT_CACHEABLE(address, pData)                             \
        *pData = (unsigned short)*((unsigned short *)                       \
        (CACHEABLE(address)))

#define GT_READ_WORD_CACHEABLE(address, pData)                              \
        *pData = (unsigned int)*((unsigned int *)                           \
        (CACHEABLE(address)))
        
/* Read 32/16/8 bit Cacheable - returns data direct. */
#define GT_READCHAR_CACHEABLE(address)                                      \
        (unsigned char)*((unsigned char *)                                  \
        (CACHEABLE(address)))
                
#define GT_READSHORT_CACHEABLE(address)                                     \
        (unsigned short)*((unsigned short *)                                \
        (CACHEABLE(address)))

#define GT_READWORD_CACHEABLE(address)                                      \
        (unsigned int)*((unsigned int *)                                    \
        (CACHEABLE(address)))

#ifndef __KERNEL__
/* GT_SET_REG_BITS(regOffset,bits) - 
   gets register offset and bits: a 32bit value. It set to logic '1' in the  
   register the bits which given as an input example:
   GT_SET_REG_BITS(0x840,BIT3 | BIT24 | BIT30) - set bits: 3,24 and 30 to logic
   '1' in register 0x840 while the other bits stays as is. */
#define GT_SET_REG_BITS(regOffset,bits)                                     \
        (volatile unsigned int)*(unsigned int*)                             \
        (NONE_CACHEABLE(gtInternalRegBaseAddr) | (regOffset)) |=            \
        (unsigned int)GT_WORD_SWAP(bits)       
                                         
/* GT_RESET_REG_BITS(regOffset,bits) - 
   gets register offset and bits: a 32bit value. It set to logic '0' in the  
   register the bits which given as an input example:
   GT_RESET_REG_BITS(0x840,BIT3 | BIT24 | BIT30) - set bits: 3,24 and 30 to 
   logic '0' in register 0x840 while the other bits stays as is. */
#define GT_RESET_REG_BITS(regOffset,bits)                                   \
        (volatile unsigned int)*(unsigned int*)                             \
        (NONE_CACHEABLE(gtInternalRegBaseAddr) | (regOffset)) &=            \
        ~( (unsigned int)GT_WORD_SWAP(bits) )

#endif /* no __KERNEL__ */

#define GT_PSS_SIZE        (128*1024*1024)
#define GT_8245_VIRT_ADD   (GT_PPS_VIRT_ADD + GT_PSS_SIZE)
#define GT_8245_SIZE       0x10000
#define GT_BUFFS_VIRT_ADD  (GT_8245_VIRT_ADD + GT_8245_SIZE)
#define GT_BUFFS_SIZE      (1024*1024)
#define GT_SHARED_VIRT_ADD (GT_BUFFS_VIRT_ADD + GT_BUFFS_SIZE)
#define GT_SHARED_SIZE     (64*1024*1024)
#define GT_PPS_VIRT_ADD    0x50000000
#define _NUM_PP 2
#define _CONFIG_SIZE       (4*1024)
#define _REGS_SIZE         (64*1024*1024)
#define _PP_SIZE           (_CONFIG_SIZE + _REGS_SIZE)
#define GT_PPS_SIZE        (_NUM_PP * _PP_SIZE)

/* typedefs */
typedef enum _bool{false,true} bool;

#endif /* __INCgtCoreh */

