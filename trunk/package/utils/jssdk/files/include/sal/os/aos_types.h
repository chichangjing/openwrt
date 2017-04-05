#ifndef _AOS_TYPES_H
#define _AOS_TYPES_H

#ifdef KERNEL_MODULE
#include "sal/os/linux_kernel/aos_types_pvt.h"
#else
#include "sal/os/linux_user/aos_types_pvt.h"
#endif

#ifndef NULL
#define NULL 0
#endif

typedef enum {
    A_FALSE,
    A_TRUE           
} a_bool_t;

typedef __a_uint8_t    a_uint8_t;
typedef __a_int8_t     a_int8_t;
typedef __a_uint16_t   a_uint16_t;
typedef __a_int16_t    a_int16_t;
typedef __a_uint32_t   a_uint32_t;
typedef __a_int32_t    a_int32_t;
typedef __a_uint64_t   a_uint64_t;
typedef __a_int64_t    a_int64_t;


typedef void * acore_t;
typedef __aos_device_t	aos_device_t;
typedef __aos_size_t	aos_size_t;
typedef __aos_iomem_t   aos_iomem_t;

typedef enum {
    A_STATUS_OK,
    A_STATUS_FAILED,
    A_STATUS_ENOENT,
    A_STATUS_ENOMEM,
    A_STATUS_EINVAL,
    A_STATUS_EINPROGRESS,
    A_STATUS_ENOTSUPP,
    A_STATUS_EBUSY,
}a_status_t;


typedef enum {
    AOS_RESOURCE_TYPE_MEM,
    AOS_RESOURCE_TYPE_IO,
}aos_resource_type_t;

typedef struct {
    a_uint32_t          start;
    a_uint32_t          end;
    aos_resource_type_t type;
}aos_resource_t;


#endif

