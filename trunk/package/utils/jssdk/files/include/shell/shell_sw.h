#ifndef _SHELL_SW_H_
#define _SHELL_SW_H_

#ifdef __cplusplus
extern "C" {
#endif    

#include "sw.h"

int get_devid(void);
sw_error_t cmd_set_devid(a_uint32_t *arg_val);

#ifdef __cplusplus
}
#endif
#endif

