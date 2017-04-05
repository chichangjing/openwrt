
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "sal_drv.h"
#include "mv88e6097_reg_access.h"

#include "msJwSwitch.h"
#include "gtHwCntl.h"

static hsl_access_mode reg_mode;

#if defined(API_LOCK)
    static aos_lock_t mdio_lock;
    #define MDIO_LOCKER_INIT    aos_lock_init(&mdio_lock)
    #define MDIO_LOCKER_LOCK    aos_lock(&mdio_lock)
    #define MDIO_LOCKER_UNLOCK  aos_unlock(&mdio_lock)
#else
    #define MDIO_LOCKER_INIT
    #define MDIO_LOCKER_LOCK
    #define MDIO_LOCKER_UNLOCK
#endif

static sw_error_t _mv88e6097_mdio_reg_get(a_uint32_t dev_id, a_uint32_t phy_reg_addr, a_uint8_t value[], a_uint32_t value_len)
{
	GT_BOOL ret;
    a_uint32_t phyAddr, regAddr;
    a_uint32_t regVal;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;
	
	phyAddr = (phy_reg_addr & 0x0000ff00) >> 8;
	regAddr = phy_reg_addr & 0x000000ff;
	
	ret = jwMiiRead(pJwDev, phyAddr, regAddr, &regVal);
	if(ret != GT_TRUE)
		return SW_FAIL;
		
	aos_mem_copy(value, &regVal, sizeof(a_uint32_t));
		
    return SW_OK;
}

static sw_error_t _mv88e6097_mdio_reg_set(a_uint32_t dev_id, a_uint32_t phy_reg_addr, a_uint8_t value[], a_uint32_t value_len)
{
	GT_BOOL ret;
    a_uint32_t phyAddr, regAddr, regVal;
	
    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

	aos_mem_copy(&regVal, value, sizeof (a_uint32_t));
	
	phyAddr = (phy_reg_addr & 0x0000ff00) >> 8;
	regAddr = phy_reg_addr & 0x000000ff;
	
	ret = jwMiiWrite(pJwDev, phyAddr, regAddr, regVal);
	if(ret != GT_TRUE)
		return SW_FAIL;
	
    return SW_OK;
}

static sw_error_t _mv88e6097_phy_reg_get(a_uint32_t dev_id,a_uint32_t phy_id,a_uint32_t regAddr,a_uint32_t * data)
{
	GT_BOOL ret;
	GT_U16 value;

	ret = hwReadPhyReg(pJwDev, phy_id, regAddr,&value);
	if(ret != GT_OK) {
		printf("%s, hwReadPhyReg Failed,ret=%d\n",__func__,ret);
		return SW_FAIL;
	}

	*data = value;
	return SW_OK;
}

static sw_error_t _mv88e6097_phy_reg_set(a_uint32_t dev_id,a_uint32_t phy_id,a_uint32_t regAddr,a_uint32_t data)
{
	GT_BOOL ret;

	ret = hwWritePhyReg(pJwDev, phy_id, regAddr, data);
	if(ret != GT_OK) {
		printf("%s, hwWritePhyReg Failed,ret=%d\n",__func__,ret);
		return SW_FAIL;
	}
	
	return SW_OK;
}


sw_error_t mv88e6097_reg_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;

    MDIO_LOCKER_LOCK;
    rv = _mv88e6097_mdio_reg_get(dev_id, reg_addr, value, value_len);
    MDIO_LOCKER_UNLOCK;

    return rv;
}

sw_error_t mv88e6097_reg_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint8_t value[], a_uint32_t value_len)
{
    sw_error_t rv;

    MDIO_LOCKER_LOCK;
    rv = _mv88e6097_mdio_reg_set(dev_id, reg_addr, value, value_len);
    MDIO_LOCKER_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_phy_reg_get(a_uint32_t dev_id,a_uint32_t phy_id,a_uint32_t regAddr,a_uint32_t * data)
{
    sw_error_t rv;

    MDIO_LOCKER_LOCK;
    rv = _mv88e6097_phy_reg_get(dev_id, phy_id, regAddr, data);
    MDIO_LOCKER_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_phy_reg_set(a_uint32_t dev_id,a_uint32_t phy_id,a_uint32_t regAddr,a_uint32_t data)
{
    sw_error_t rv;

    MDIO_LOCKER_LOCK;
    rv = _mv88e6097_phy_reg_set(dev_id, phy_id, regAddr, data);
    MDIO_LOCKER_UNLOCK;
	
    return rv;
}



sw_error_t mv88e6097_reg_field_get(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t bit_offset, a_uint32_t field_len, a_uint32_t value[], a_uint32_t value_len)
{
    a_uint16_t reg_val = 0;
	a_uint32_t value_buf;

    if ((bit_offset >= 16 || (field_len > 16)) || (field_len == 0))
        return SW_OUT_OF_RANGE;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    SW_RTN_ON_ERROR(mv88e6097_reg_get(dev_id, reg_addr, (a_uint8_t *) & reg_val, sizeof (a_uint32_t)));

   // *((a_uint32_t *) value) = SW_REG_2_FIELD(reg_val, bit_offset, field_len);
	value_buf = SW_REG_2_FIELD(reg_val, bit_offset, field_len);

	aos_mem_copy(value, &value_buf, sizeof(a_uint32_t));
	
    return SW_OK;
}

sw_error_t mv88e6097_reg_field_set(a_uint32_t dev_id, a_uint32_t reg_addr, a_uint32_t bit_offset, a_uint32_t field_len, const a_uint8_t value[], a_uint32_t value_len)
{
    a_uint16_t reg_val_u16;
	a_uint32_t reg_val_u32;
    a_uint16_t field_val_u16 = *((a_uint16_t *) value);
    a_uint32_t field_val_u32;
	
    if ((bit_offset >= 16 || (field_len > 16)) || (field_len == 0))
        return SW_OUT_OF_RANGE;

    if (value_len != sizeof (a_uint32_t))
        return SW_BAD_LEN;

    SW_RTN_ON_ERROR(mv88e6097_reg_get(dev_id, reg_addr, (a_uint8_t *) & reg_val_u16, sizeof (a_uint32_t)));

    reg_val_u32 = reg_val_u16;
	field_val_u32 = field_val_u16;
    SW_REG_SET_BY_FIELD_U32(reg_val_u32, field_val_u32, bit_offset, field_len);
	//reg_val_u16 = (a_uint16_t)(field_val_u32 & 0xFFFF);
	reg_val_u16 = (a_uint16_t)(reg_val_u32 & 0xFFFF);
	
    SW_RTN_ON_ERROR(mv88e6097_reg_set(dev_id, reg_addr, (a_uint8_t *) & reg_val_u16, sizeof (a_uint32_t)));

    return SW_OK;
}

sw_error_t mv88e6097_reg_access_init(a_uint32_t dev_id, hsl_access_mode mode)
{
    MDIO_LOCKER_INIT;
    reg_mode = mode;

#if (!(defined(USER_MODE) && defined(KERNEL_MODULE)))
#ifndef HSL_STANDALONG
    hsl_api_t *p_api;

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    p_api->reg_get = mv88e6097_reg_get;
    p_api->reg_set = mv88e6097_reg_set;
    p_api->reg_field_get = mv88e6097_reg_field_get;
    p_api->reg_field_set = mv88e6097_reg_field_set;
	p_api->phy_reg_get = mv88e6097_phy_reg_get;
	p_api->phy_reg_set = mv88e6097_phy_reg_set;

#endif
#endif

    return SW_OK;
}

sw_error_t mv88e6097_access_mode_set(a_uint32_t dev_id, hsl_access_mode mode)
{
    reg_mode = mode;
    return SW_OK;

}

