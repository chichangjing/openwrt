
#ifndef _MV88E6097_MIB_H
#define _MV88E6097_MIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fal/fal_mib.h"

sw_error_t mv88e6097_mib_info_get(a_uint32_t dev_id, fal_port_t port_id, fal_mib_info_t *mib_info);
sw_error_t mv88e6097_mib_info_clear(a_uint32_t dev_id, fal_port_t port_id);
sw_error_t mv88e6097_mib_init(a_uint32_t dev_id);

#ifdef __cplusplus
}
#endif
#endif


