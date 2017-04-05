
#ifndef _HSL_DEV_H
#define _HSL_DEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hsl_api.h"
#include "ssdk_init.h"

typedef struct {
    a_uint32_t dev_id;
    a_uint32_t cpu_port;
    a_uint32_t nr_ports;
} hsl_dev_t;

#define HSL_DEV_ID_CHECK(dev_id) \
do { \
    if (dev_id >= SW_MAX_NR_DEV) \
        return SW_OUT_OF_RANGE; \
} while (0)

#define HSL_DEV_PORT_ID_CHECK(dev_id, port_id) \
do { \
    hsl_dev_t *pdev = NULL; \
    pdev = hsl_dev_ptr_get(dev_id); \
    if (pdev == NULL) \
        return SW_NOT_INITIALIZED; \
    if (port_id >= pdev->nr_ports) \
        return SW_OUT_OF_RANGE; \
} while (0)

hsl_dev_t *hsl_dev_ptr_get(a_uint32_t dev_id);
sw_error_t hsl_dev_init(a_uint32_t dev_id, ssdk_init_cfg * cfg);
sw_error_t hsl_ssdk_cfg(a_uint32_t dev_id, ssdk_cfg_t *ssdk_cfg);
sw_error_t hsl_dev_cleanup(void);
sw_error_t hsl_access_mode_set(a_uint32_t dev_id, hsl_access_mode reg_mode);

#ifdef __cplusplus
}
#endif
#endif

