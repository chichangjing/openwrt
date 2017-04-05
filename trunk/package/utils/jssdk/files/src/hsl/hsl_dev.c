
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_lock.h"
#include "sal_drv.h"

#if defined MARVELL_88E6097
#include "mv88e6097_init.h"
#endif

#include "sw_api.h"

static hsl_dev_t dev_table[SW_MAX_NR_DEV];
static ssdk_init_cfg *dev_ssdk_cfg[SW_MAX_NR_DEV] = { 0 };

hsl_dev_t *hsl_dev_ptr_get(a_uint32_t dev_id)
{
    if (dev_id >= SW_MAX_NR_DEV)
        return NULL;

    return &dev_table[dev_id];
}

sw_error_t hsl_dev_init(a_uint32_t dev_id, ssdk_init_cfg *cfg)
{
    sw_error_t rv = SW_OK;

    if (SW_MAX_NR_DEV <= dev_id) {
        return SW_BAD_PARAM;
    }

    aos_mem_set(&dev_table[dev_id], 0, sizeof (hsl_dev_t));

    SW_RTN_ON_ERROR(sal_drv_init(dev_id,cfg));

#if defined API_LOCK
    SW_RTN_ON_ERROR(hsl_api_lock_init());
#endif

#if defined MARVELL_88E6097
    rv = mv88e6097_init(dev_id, cfg);
#else if defined MARVELL_3236
    rv = mv3236_init(dev_id, cfg);
#endif

    if (NULL == dev_ssdk_cfg[dev_id]) {
        dev_ssdk_cfg[dev_id] = aos_mem_alloc(sizeof(ssdk_init_cfg));
    }

    if (NULL == dev_ssdk_cfg[dev_id]) {
        return SW_OUT_OF_MEM;
    }

    aos_mem_copy(dev_ssdk_cfg[dev_id], cfg, sizeof(ssdk_init_cfg));

    return rv;
}

sw_error_t hsl_ssdk_cfg(a_uint32_t dev_id, ssdk_cfg_t *ssdk_cfg)
{
    aos_mem_set(&(ssdk_cfg->init_cfg), 0,  sizeof(ssdk_init_cfg));
    aos_mem_copy(&(ssdk_cfg->init_cfg), dev_ssdk_cfg[dev_id], sizeof(ssdk_init_cfg));

#ifdef VERSION
    aos_mem_copy(ssdk_cfg->build_ver, VERSION, sizeof(VERSION));
#endif

#ifdef BUILD_DATE
    aos_mem_copy(ssdk_cfg->build_date, BUILD_DATE, sizeof(BUILD_DATE));
#endif

#if defined MARVELL_88E6097
    aos_mem_copy(ssdk_cfg->chip_type, "Marvell-88E6097", sizeof("Marvel-88E6097"));
#endif

#ifdef CPU
    aos_mem_copy(ssdk_cfg->cpu_type, CPU, sizeof(CPU));
#endif

#ifdef OS
    aos_mem_copy(ssdk_cfg->os_info, OS, sizeof(OS));
#endif

#ifdef IN_PORTCONTROL
    ssdk_cfg->features.in_portcontrol = A_TRUE;
#endif

#ifdef IN_FDB
    ssdk_cfg->features.in_fdb = A_TRUE;
#endif
#ifdef IN_IGMP
    ssdk_cfg->features.in_igmp = A_TRUE;
#endif

#ifdef IN_MIB
    ssdk_cfg->features.in_mib = A_TRUE;
#endif
#ifdef IN_MIRROR
    ssdk_cfg->features.in_mirror = A_TRUE;
#endif
#ifdef IN_MISC
    ssdk_cfg->features.in_misc = A_TRUE;
#endif

#ifdef IN_PORTVLAN
    ssdk_cfg->features.in_portvlan = A_TRUE;
#endif
#ifdef IN_QOS
    ssdk_cfg->features.in_qos = A_TRUE;
#endif
#ifdef IN_RATE
    ssdk_cfg->features.in_rate = A_TRUE;
#endif
#ifdef IN_STP
    ssdk_cfg->features.in_stp = A_TRUE;
#endif
#ifdef IN_VLAN
    ssdk_cfg->features.in_vlan = A_TRUE;
#endif


    return SW_OK;
}

sw_error_t hsl_dev_cleanup(void)
{
    sw_error_t rv = SW_OK;
    a_uint32_t dev_id;

#if defined MARVELL_88E6097
    rv = mv88e6097_cleanup();
#endif

    for (dev_id = 0; dev_id < SW_MAX_NR_DEV; dev_id++) {
        if (dev_ssdk_cfg[dev_id]) {
            aos_mem_free(dev_ssdk_cfg[dev_id]);
            dev_ssdk_cfg[dev_id] = NULL;
        }
    }

    return rv;
}

sw_error_t hsl_access_mode_set(a_uint32_t dev_id, hsl_access_mode reg_mode)
{
    sw_error_t rv;

    if (SW_MAX_NR_DEV <= dev_id) {
        return SW_BAD_PARAM;
    }

#if defined MARVELL_88E6097
    rv = mv88e6097_access_mode_set(dev_id, reg_mode);
#endif
    return rv;
}

