
#ifndef _FAL_MIB_H
#define _FAL_MIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/sw.h"
#include "fal/fal_type.h"
	
typedef struct {
    a_uint32_t RxGoodByteLo;
    a_uint32_t RxGoodByteHi;
    a_uint32_t RxUnicastPkts;
    a_uint32_t RxBroadcastPkts;
    a_uint32_t RxMulticastPkts;
    a_uint32_t RxPausePkts;

    a_uint32_t TxByteLo;
    a_uint32_t TxByteHi;
    a_uint32_t TxUnicastPkts;
    a_uint32_t TxBroadcastPkts;
    a_uint32_t TxMulticastPkts;
    a_uint32_t TxPausePkts;
}fal_mib_info_t;

sw_error_t fal_mib_info_get(a_uint32_t dev_id, fal_port_t port_id, fal_mib_info_t *mib_info);
sw_error_t fal_mib_info_clear(a_uint32_t dev_id, fal_port_t port_id);

#ifdef __cplusplus
}
#endif

#endif

