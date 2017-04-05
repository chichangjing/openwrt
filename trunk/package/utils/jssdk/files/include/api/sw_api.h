
#ifndef _SW_API_H
#define _SW_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sw.h"
#include "sw_ioctl.h"

#define SW_MAX_API_BUF         1024
#define SW_MAX_API_PARAM       12 						/* cmd type + return value + ten parameters */
#define SW_MAX_PAYLOAD         (SW_MAX_API_PARAM << 2)	/* maximum payload size for netlink msg*/
#define SW_PARAM_IN            0x1
#define SW_PARAM_OUT           0x2
#define SW_PARAM_PTR           0x4

#define SW_API_DEF(ioctl, name) {ioctl, name}
#if (!defined(KERNEL_MODULE))
#define SW_PARAM_DEF(ioctl, data, size, type, name) {ioctl, size, data, type, name}
#else
#define SW_PARAM_DEF(ioctl, data, size, type, name) {ioctl, size, data, type}
#endif

typedef enum {
    SW_UINT8 = 1,
    SW_INT8,
    SW_UINT16,
    SW_INT16,
    SW_UINT32,
    SW_INT32,
    SW_UINT64,
    SW_INT64,
    SW_ENABLE,
    SW_JWS_PORTLIST,
    
/* portctrl */   
    SW_PORT_TYPE,
    SW_PORT_LINK_STATUS,
    SW_PORT_PHYSICAL_MODE,
    SW_PORT_PHYSICAL_STATUS,
    SW_PORT_DOT1D_STATE, 
    SW_PORT_COMB_TYPE,
    SW_PORT_COMB_PORT,
/* mib */   
	SW_MIB,
/* mirror */
	SW_MIRROR_MODE,

/* trunk */
	SW_TRUNK_MODE,
	SW_TRUNK_LOAD_BALANCE,
/* rate */
	SW_RATE_SPEED,
/* multicast */
	SW_MULTICAST_ADDR,
/* vlan */
	SW_VLAN_MODE,
	SW_VLAN_EGRESS_TAG,
/* QOS */
	SW_QOS_MAP,
	SW_QOS_SCHEDULE_MODE,
	SW_QOS_ASSIGN_MODE,
	SW_QOS_DEFAULT_PRIO,
/* FDB */
	SW_BLACKWHITE_LIST,
	SW_ATU_STATUS,
	
    SW_PBMP,
    SW_SSDK_CFG,
    SW_DATA_MAX,
} sw_data_type_e;

typedef struct {
    a_uint32_t api_id;
    void *func;
} sw_api_func_t;

typedef struct {
    a_uint32_t api_id;
    a_uint16_t data_size;
    a_uint8_t  data_type;
    a_uint8_t  param_type;
#if (!defined(KERNEL_MODULE))
    a_uint8_t param_name[20];
#endif
} sw_api_param_t;

typedef struct {
    a_uint32_t api_id;
    sw_api_func_t *api_fp;
    sw_api_param_t *api_pp;
    a_uint32_t api_nr;
} sw_api_t;

#ifdef __cplusplus
}
#endif

#endif

