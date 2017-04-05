
#include "sw.h"
#include "fal_reg_access.h"
#include "hsl_api.h"

static inline sw_error_t _fal_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->reg_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->reg_get(dev_id, reg_addr, value, value_len);
    return rv;
}

static inline sw_error_t _fal_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->reg_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->reg_set(dev_id, reg_addr, value, value_len);
    return rv;
}


static inline sw_error_t _fal_reg_field_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t bit_offset, a_uint32_t field_len, a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->reg_field_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->reg_field_get(dev_id, reg_addr, bit_offset, field_len, value, value_len);
    return rv;
}

static inline sw_error_t _fal_reg_field_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t bit_offset, a_uint32_t field_len, const a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->reg_field_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->reg_field_set(dev_id, reg_addr, bit_offset, field_len, value, value_len);
    return rv;
}

static inline sw_error_t _fal_phy_reg_get(a_uint32_t dev_id,a_uint32_t phy_id,a_uint32_t regAddr,a_uint32_t *data)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->phy_reg_get)
        return SW_NOT_SUPPORTED;

    rv = p_api->phy_reg_get(dev_id, phy_id, regAddr, data);
    return rv;
}

static inline sw_error_t _fal_phy_reg_set(a_uint32_t dev_id,a_uint32_t phy_id,a_uint32_t regAddr,a_uint32_t data)
{
    sw_error_t rv;
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    if (NULL == p_api->phy_reg_set)
        return SW_NOT_SUPPORTED;

    rv = p_api->phy_reg_set(dev_id, phy_id, regAddr, data);
    return rv;
}


/**
  * fal_reg_get - get value of specific register
  * @reg_addr: address of the register
  * @value: pointer to the memory storing the value.
  * @value_len: length of the value.
  *
  * Get the value of a specific register field with related parameter
  */
sw_error_t fal_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_reg_get(dev_id, reg_addr, value, value_len);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * fal_reg_set - set value of specific register
  * @reg_addr: address of the register
  * @value: pointer to the memory storing the value.
  * @value_len: length of the value.
  *
  * Get the value of a specific register field with related parameter
  */
sw_error_t fal_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_reg_set(dev_id, reg_addr, value, value_len);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * fal_reg_field_get - get value of specific register field
  * @reg_addr: address of the register
  * @bit_offset: position of the field in bit
  * @field_len: length of the field in bit
  * @value: pointer to the memory storing the value.
  * @value_len: length of the value.
  *
  * Get the value of a specific register field with related parameter
  */
sw_error_t fal_reg_field_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t bit_offset, a_uint32_t field_len, a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_reg_field_get(dev_id, reg_addr, bit_offset, field_len, value, value_len);
    FAL_API_UNLOCK;
    return rv;
}

/**
  * fal_reg_field_set - set value of specific register field
  * @reg_addr: address of the register
  * @bit_offset: position of the field in bit
  * @field_len: length of the field in bit
  * @value: pointer to the memory storing the value.
  * @value_len: length of the value.
  *
  * Set the value of a specific register field with related parameter
  */
sw_error_t fal_reg_field_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t bit_offset, a_uint32_t field_len, const a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_reg_field_set(dev_id, reg_addr, bit_offset, field_len, value, value_len);
    FAL_API_UNLOCK;
    return rv;
}

sw_error_t fal_phy_reg_get(a_uint32_t dev_id,a_uint32_t phy_id,a_uint32_t regAddr,a_uint32_t *data)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_phy_reg_get(dev_id, phy_id, regAddr, data);
    FAL_API_UNLOCK;
	
    return rv;
}

sw_error_t fal_phy_reg_set(a_uint32_t dev_id,a_uint32_t phy_id,a_uint32_t regAddr,a_uint32_t data)
{
    sw_error_t rv;

    FAL_API_LOCK;
    rv = _fal_phy_reg_set(dev_id, phy_id, regAddr, data);
    FAL_API_UNLOCK;
	
    return rv;
}


