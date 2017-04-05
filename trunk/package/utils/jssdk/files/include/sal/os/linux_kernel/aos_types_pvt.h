#ifndef _AOS_PVTTYPES_H
#define _AOS_PVTTYPES_H

#include <asm/types.h>
#include <linux/compiler.h>

/*
 * Private definitions of general data types
 */

/* generic data types */
typedef struct device *   __aos_device_t;
typedef int               __aos_size_t;

typedef   u8 __iomem * __aos_iomem_t;

typedef   u8              __a_uint8_t;    
typedef   s8              __a_int8_t;     
typedef   u16             __a_uint16_t;   
typedef   s16             __a_int16_t;    
typedef   u32             __a_uint32_t;   
typedef   s32             __a_int32_t;    
typedef   u64             __a_uint64_t;   
typedef   s64             __a_int64_t;      

#define aos_printk        printk

#endif 

