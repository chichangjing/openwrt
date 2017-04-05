#include <stdio.h>
#include "shell.h"
#include "fal.h"

static int sw_devid = 0;

sw_error_t cmd_set_devid(a_uint32_t *arg_val)
{
    if (arg_val[1] >= SW_MAX_NR_DEV) {
        dprintf("dev_id should be less than <%d>\n", SW_MAX_NR_DEV);
        return SW_FAIL;
    }
    sw_devid = arg_val[1];
    
    return SW_OK;
}

int get_devid(void)
{
    return sw_devid;
}


