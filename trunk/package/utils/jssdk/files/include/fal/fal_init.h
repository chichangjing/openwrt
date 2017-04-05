
#ifndef _FAL_INIT_H_
#define _FAL_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "init/ssdk_init.h"

sw_error_t fal_init(a_uint32_t dev_id, ssdk_init_cfg * cfg);
sw_error_t fal_reset(a_uint32_t dev_id);
sw_error_t fal_mode_tag(a_uint32_t dev_id);
sw_error_t fal_ssdk_cfg(a_uint32_t dev_id, ssdk_cfg_t *ssdk_cfg);
sw_error_t fal_cleanup(void);

#ifdef __cplusplus
}
#endif
#endif

