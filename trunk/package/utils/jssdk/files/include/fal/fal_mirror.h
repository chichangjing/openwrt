
#ifndef _FAL_MIRROR_H_
#define _FAL_MIRROR_H_

#ifdef __cplusplus
extern "c" {
#endif

#include "common/sw.h"
#include "fal/fal_type.h"

typedef enum {
	FAL_MIRROR_DEST_PORT_ENABLE  = 1,
	FAL_MIRROR_DEST_PORT_DISABLE = 2,
	FAL_MIRROR_DEST_PORT_END 	 = 3
} fal_mirror_dest_port_t;


typedef enum {
	FAL_MIRROR_MODE_TX  = 1,
    FAL_MIRROR_MODE_RX  = 2,
    FAL_MIRROR_MODE_BOTH = 3,
    FAL_MIRROR_MODE_DISABLE = 4,
    FAL_MIRROR_MODE_END = 5
} fal_mirror_mode_t;

sw_error_t	fal_mirror_dest_port_set(a_uint32_t dev_id, fal_port_t port_id);
sw_error_t	fal_mirror_dest_port_get(a_uint32_t dev_id, fal_port_t *port_id);
sw_error_t	fal_mirror_source_port_mode_set(a_uint32_t dev_id, fal_port_t source_port, fal_mirror_mode_t mode);
sw_error_t 	fal_mirror_source_port_mode_get(a_uint32_t dev_id, fal_port_t source_port, fal_mirror_mode_t *mode);

#ifdef __cplusplus
}
#endif
#endif

