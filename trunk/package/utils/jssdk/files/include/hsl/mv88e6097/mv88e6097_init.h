
#ifndef _MV88E6097_INIT_H_
#define _MV88E6097_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "init/ssdk_init.h"

sw_error_t mv88e6097_init(a_uint32_t dev_id, ssdk_init_cfg * cfg);
sw_error_t mv88e6097_cleanup(void);
sw_error_t mv88e6097_reset(a_uint32_t dev_id);
sw_error_t mv88e6097_mode_tag(a_uint32_t dev_id);

#ifdef __cplusplus
}
#endif
#endif

