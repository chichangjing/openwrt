
#ifndef _MV88E6097_MIRROR_H
#define _MV88E6097_MIRROR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fal/fal_mirror.h"


sw_error_t mv88e6097_port_mirror_init(a_uint32_t dev_id);
sw_error_t mv88e6097_mirror_dest_port_set(a_uint32_t dev_id, fal_port_t port_id);
sw_error_t mv88e6097_mirror_dest_port_get(a_uint32_t dev_id, fal_port_t *port_id);
sw_error_t mv88e6097_mirror_source_port_mode_set(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t mode);
sw_error_t mv88e6097_mirror_source_port_mode_get(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t * mode);


#ifdef __cplusplus
}
#endif
#endif

