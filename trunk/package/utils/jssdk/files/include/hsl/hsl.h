
#ifndef _HSL_H
#define _HSL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "init/ssdk_init.h"
#if defined MARVELL_88E6097
#include "hsl/mv88e6097/mv88e6097_reg_access.h"
#endif

typedef sw_error_t (*hsl_reg_access_mode_set) (a_uint32_t dev_id, hsl_access_mode mode);

#if defined MARVELL_88E6097
    #define _hsl_reg_entry_get     mv88e6097_reg_get
    #define _hsl_reg_entry_set     mv88e6097_reg_set
    #define _hsl_reg_field_get     mv88e6097_reg_field_get
    #define _hsl_reg_field_set     mv88e6097_reg_field_set
    #define _hsl_reg_entry_gen_get mv88e6097_reg_get
    #define _hsl_reg_entry_gen_set mv88e6097_reg_set
    #define _hsl_reg_field_gen_get mv88e6097_reg_field_get
    #define _hsl_reg_field_gen_set mv88e6097_reg_field_set
#endif

#define HSL_REG_ENTRY_GET(rv, dev, reg, index, value, val_len) \
    rv = _hsl_reg_entry_get(dev, reg##_OFFSET + ((a_uint32_t)index) * reg##_E_OFFSET, (a_uint8_t*)value, (a_uint8_t)val_len);

#define HSL_REG_ENTRY_SET(rv, dev, reg, index, value, val_len) \
    rv = _hsl_reg_entry_set (dev, reg##_OFFSET + ((a_uint32_t)index) * reg##_E_OFFSET, (a_uint8_t*)value, (a_uint8_t)val_len);

#define HSL_REG_FIELD_GET(rv, dev, reg, index, field, value, val_len) \
    rv = _hsl_reg_field_get(dev, reg##_OFFSET + ((a_uint32_t)index) * reg##_E_OFFSET, reg##_##field##_BOFFSET, reg##_##field##_BLEN, (a_uint8_t*)value, val_len);

#define HSL_REG_FIELD_SET(rv, dev, reg, index, field, value, val_len) \
    rv = _hsl_reg_field_set(dev, reg##_OFFSET + ((a_uint32_t)index) * reg##_E_OFFSET, reg##_##field##_BOFFSET, reg##_##field##_BLEN, (a_uint8_t*)value, val_len);

#define HSL_REG_ENTRY_GEN_GET(rv, dev, addr, reg_len, value, val_len) \
    rv = _hsl_reg_entry_gen_get (dev, addr, (a_uint8_t*)value, val_len);

#define HSL_REG_ENTRY_GEN_SET(rv, dev, addr, reg_len, value, val_len) \
    rv = _hsl_reg_entry_gen_set (dev, addr, (a_uint8_t*)value, val_len);

#define HSL_REG_FIELD_GEN_GET(rv, dev, regaddr, bitlength, bitoffset, value, val_len) \
    rv = _hsl_reg_field_gen_get(dev, regaddr, bitoffset, bitlength, (a_uint8_t *) value, val_len);

#define HSL_REG_FIELD_GEN_SET(rv, dev, regaddr, bitlength, bitoffset, value, val_len) \
    rv = _hsl_reg_field_gen_set(dev, regaddr, bitoffset, bitlength, (a_uint8_t *) value, val_len);


#if (defined(API_LOCK) && (defined(HSL_STANDALONG) || (defined(KERNEL_MODULE) && defined(USER_MODE))))
extern  aos_lock_t sw_hsl_api_lock;
#define HSL_API_LOCK    aos_lock(&sw_hsl_api_lock)
#define HSL_API_UNLOCK  aos_unlock(&sw_hsl_api_lock)
#else
#define HSL_API_LOCK
#define HSL_API_UNLOCK
#endif

#ifdef __cplusplus
    }
#endif
#endif

