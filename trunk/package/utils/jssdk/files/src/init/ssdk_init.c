
#include "sw.h"
#include "ssdk_init.h"
#include "fal_init.h"
#include "hsl.h"
#include "hsl_dev.h"

sw_error_t ssdk_init(a_uint32_t dev_id, ssdk_init_cfg * cfg)
{
    sw_error_t rv;

    rv = fal_init(dev_id, cfg);

    return rv;
}

sw_error_t ssdk_cleanup(void)
{
    sw_error_t rv;

    rv = fal_cleanup();

    return rv;
}

