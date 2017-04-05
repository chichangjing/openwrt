
#ifndef _SAL_SYS_H
#define _SAL_SYS_H

#include "common/sw.h"

#define SAL_SYS_MODEL_STR_LEN 32


/*
 *  typedef: struct sal_sys_unique_id_t
 *
 *  Description: product unique id.
 *
 *  Fields:
 *      sys_type 				- 53代表工业以太网
 *      sys_sub_type 			- 10:百兆，20:千兆系类，30:2.5G系类,40:万兆系类，50:同轴系列   +  01 (CPU型号)
 *      sys_hardware_type 		- 端口数
 *		sys_hardware_version	- 版本从1开始叠加
 *		reserved				- 预留
 *
 *  Comment:
 */


typedef struct {
    a_uint8_t sys_type;
    a_uint8_t sys_sub_type;
    a_uint8_t sys_hardware_type;
    a_uint8_t sys_hardware_version;
    a_uint8_t reserved[4];
} sal_sys_unique_id_t;

/*
 * P: protect
 * T: transmit
 * R: receive
 */
typedef enum {
	GE_Z20208D_2008 = 0x53211001,
	GE_Z20505D_1414 = 0x53211002,
	GE_Z20208D_1008 = 0x53211003,
	GE_Z20208D_2004 = 0x53211004,
	GE_Z20208D_1004 = 0x53211005,
	GE_Z20204A_2013 = 0x53211006,
	GE_Z20204A_1013 = 0x53211007,
	GE_Z20204A_2017 = 0x53211008,
	GE_Z20204A_1017 = 0x53211009,
	GE_Z20210D_2028 = 0x5321100A,
	GE_Z20210D_2024 = 0x5321100B,
	GE_Z20208D_2008_P = 0x5321100C,
	GE_Z20208D_2004_P = 0x5321100D,
	GE_SDI1000_0014_T = 0x5321100E,
	GE_SDI1000_0014_R = 0x5321100F
} sal_sys_model_id_t;

typedef struct {
	a_uint32_t model_id;
	a_uint8_t  model_string[SAL_SYS_MODEL_STR_LEN];
} sal_sys_model_t;


#define MAX_MAC_LEN 8
#define MAX_UID_LEN 32
#define MAX_HDV_LEN 32
#define MAX_PRDT_LEN 32
#define MTD_DEV_PATH "/dev/mtd3"


struct jw_factory {
      unsigned char jw_mac[MAX_MAC_LEN]; //align 4
      char jw_uid[MAX_UID_LEN];
      char jw_hardware_ver[MAX_HDV_LEN];
      char jw_product_type[MAX_PRDT_LEN];
};


sw_error_t sal_sys_unique_id_get(sal_sys_unique_id_t  *unique_id);
sw_error_t sal_sys_model_get(sal_sys_model_t *sys_model);

#ifdef __cplusplus
}
#endif
#endif


