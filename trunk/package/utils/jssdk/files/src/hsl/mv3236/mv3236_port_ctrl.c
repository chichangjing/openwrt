 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv3236_global.h"
#include "mv3236_port_ctrl.h"
#include "mv3236_reg.h"
#include "cpss/dxCh/dxChxGen/bridge/cpssDxChBrgStp.h"
#include "cpss/generic/port/cpssPortCtrl.h"
//#include "msApi.h"
//#include "gtHwCntl.h"
//#include "gtDrvSwRegs.h"

//extern GT_QD_DEV *pJwDev;

#if 0
static fal_port_map_t port_map_20208d[MV3236_MAX_PORT_NUM] = {
/****************************************
   Lport  Hport     Type
 ****************************************/
	{0,    9,   FAL_PORT_TYPE_UNKOWN}, 
	{1,    10,  FAL_PORT_TYPE_GX}, 
	{2,    8,   FAL_PORT_TYPE_GX}, 
	{3,    6,   FAL_PORT_TYPE_FE}, 
	{4,    7,   FAL_PORT_TYPE_FE}, 
	{5,    4,   FAL_PORT_TYPE_FE},
	{6,    5,   FAL_PORT_TYPE_FE}, 
	{7,    2,   FAL_PORT_TYPE_FE}, 
	{8,    3,   FAL_PORT_TYPE_FE}, 
	{9,    0,   FAL_PORT_TYPE_FE},
	{10,   1,   FAL_PORT_TYPE_FE}
};

static fal_port_map_t port_map_20505d[MV3236_MAX_PORT_NUM] = {
/****************************************
   Lport  Hport     Type
 ****************************************/
	{0,    9,   FAL_PORT_TYPE_UNKOWN}, 
	{1,    10,  FAL_PORT_TYPE_GX}, 
	{2,    8,   FAL_PORT_TYPE_FX}, 
	{3,    6,   FAL_PORT_TYPE_GE}, 
	{4,    7,   FAL_PORT_TYPE_GE}, 
	{5,    4,   FAL_PORT_TYPE_GE},
	{6,    5,   FAL_PORT_TYPE_GE}, 
	{7,    2,   FAL_PORT_TYPE_FE}, 
	{8,    3,   FAL_PORT_TYPE_FE}, 
	{9,    0,   FAL_PORT_TYPE_FE},
	{10,   1,   FAL_PORT_TYPE_FE}
};
#endif
static fal_port_map_t port_map[MV3236_MAX_PORT_NUM];

static a_uint32_t PowerDownFlag[MV3236_MAX_PORT_NUM] = {0};


sw_error_t map_lport2hport(a_uint32_t lport, a_uint32_t *hport)
{
	struct jws_port_info Port_Info[MV3236_MAX_PORT_NUM+1];

	memset(&Port_Info, 0, sizeof(Port_Info));
	jws_port_info_get(&Port_Info);
	*hport = Port_Info[lport].physical_seq;
    SW_PRINTFL(DETAIL,"====enter hport is  %d ====\n", *hport);
	return SW_OK;
}

sw_error_t map_hport2lport(a_uint32_t hport, a_uint32_t *lport)
{
	struct jws_port_info Port_Info[MV3236_MAX_PORT_NUM+1];
	char i;
	memset(&Port_Info, 0, sizeof(Port_Info));
	jws_port_info_get(&Port_Info);
	for(i=0; i<MV3236_MAX_PORT_NUM+1; i++) {
		if(hport == Port_Info[i].physical_seq) {
			*lport = Port_Info[i].logic_seq;
			break;
		}
	}
	 SW_PRINTFL(DETAIL,"====enter lport is  %d ====\n", *lport);
	return SW_OK;
}

/* one byte shift high/low bit */
a_uint8_t shift_high_low_bit(a_uint8_t data)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	data = (data<<4) | (data>>4);
	data = ((data<<2) & 0xcc) | ((data>>2) & 0x33);
	data = ((data<<1) & 0xaa) | ((data>>1) & 0x55);
	return data;
}

static inline sw_error_t _mv3236_port_type_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_type_t *type_info)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t idx;
	a_uint32_t type, speed;
	a_uint32_t hport;

	struct jws_port_info Port_Info[MV3236_MAX_PORT_NUM+1];

	map_lport2hport(port_id, &hport);
	jws_port_info_get(&Port_Info);
	type = Port_Info[port_id].type;
	speed = Port_Info[port_id].speed;
	
	SW_PRINTFL(DETAIL,"hport %d, type %d, speed %d \n", hport, type, speed);
	switch(type) {
		case JWS_PORT_TYPE_ETHER_E:
			/* Ether 100M */
			if(speed == JWS_PORT_SPEED_MEGA_E)
				*type_info = FAL_PORT_TYPE_FE;
			/* Ether 1000M */
			else if(speed == JWS_PORT_SPEED_GIGA_E)
				*type_info = FAL_PORT_TYPE_FX;

			/* Normal Port Type */
			//*comb_port = FAL_PORT_NOT_COMB_PORT;
		break;

		case JWS_PORT_TYPE_FIBER_E:
			/* Fiber 100M */
			if(speed == JWS_PORT_SPEED_MEGA_E)
				*type_info = FAL_PORT_TYPE_GE;
			/* Fiber 1000M */
			else if(speed == JWS_PORT_SPEED_GIGA_E)
				*type_info = FAL_PORT_TYPE_GX;

			/* Normal Port Type */
			//*comb_port = FAL_PORT_NOT_COMB_PORT;
		break;

		case JWS_PORT_TYPE_MANAGE_E:
			/* Cpu port Type Unknown */
			*type_info = FAL_PORT_TYPE_CPU;

		break;

		default:
		break;
	}


    return rv;
}

static inline sw_error_t _mv3236_port_link_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_link_status_t *status)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;	

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		GT_BOOL link_state;
		SW_PRINTFL(DETAIL,"====dev_id %d, hport_id %d====\n", dev_id, hport_id);
	    if((ret = cpssDxChPortLinkStatusGet(dev_id, hport_id, &link_state)) != GT_OK) {
	        return SW_FAIL;
	    }
			
		if(link_state == GT_TRUE)
			*status = FAL_PORT_LINK_STATUS_UP;
		else
			*status = FAL_PORT_LINK_STATUS_DOWN;
	}

    return rv;
}

static inline sw_error_t _mv3236_port_admin_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;
	sal_sys_model_t model;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		SW_RTN_ON_ERROR(sal_sys_model_get(&model));
		switch(model.model_id) {
			default:
				if(enable == FAL_ENABLE) {
				    SW_PRINTFL(DETAIL,"========\n");
					if((ret = cpssDxChPortEnableSet(dev_id, hport_id, GT_TRUE)) != GT_OK) {
						printf("Error: FE cpssDxChPortEnableSet Fail! lport=%d\n",port_id);
						return SW_FAIL;
					} else {
						PowerDownFlag[hport_id] = 0;
					}
				} else {
				    SW_PRINTFL(DETAIL,"========\n");
					if((ret = cpssDxChPortEnableSet(dev_id, hport_id, GT_FALSE)) != GT_OK) {
						printf("Error: FE cpssDxChPortEnableSet Fail! lport=%d\n",port_id);
						return SW_FAIL;
					} else {
						PowerDownFlag[hport_id] = 1;
					}
				}

			break;
		}
	}

    return rv;
}

static inline sw_error_t _mv3236_port_admin_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;
	sal_sys_model_t model;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		GT_U16 u16Data;
		GT_BOOL forcelink, linkvalue;

		SW_RTN_ON_ERROR(sal_sys_model_get(&model));
		switch(model.model_id) {
			default:
				if((ret = cpssDxChPortEnableGet(dev_id, hport_id, &linkvalue)) != GT_OK) {
					return SW_FAIL;
				}

				if((linkvalue == GT_FALSE)) {
					*enable = FAL_DISABLE;
				} else {
					*enable = FAL_ENABLE;
				}
				break;
		}
	}

    return rv;
}

static inline sw_error_t _mv3236_port_physical_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;
	sal_sys_model_t model;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;

		if(PowerDownFlag[hport_id] == 1) {
			printf("PowerDown, exit!\n");			
			return rv;
		}

		SW_RTN_ON_ERROR(sal_sys_model_get(&model));
		switch(model.model_id) {
			default:
				switch(mode) {
					case FAL_PORT_PHYSICAL_MODE_AUTONEG:

					break;

					case FAL_PORT_PHYSICAL_MODE_S10_HALF:
						if((ret = cpssDxChPortSpeedSet(dev_id, hport_id, CPSS_PORT_SPEED_10_E)) != GT_OK)
						{
							return SW_FAIL;
						}
						if((ret = cpssDxChPortDuplexModeSet(dev_id, hport_id, CPSS_PORT_HALF_DUPLEX_E)) != GT_OK)
						{
							return SW_FAIL;
						}
						break;

					case FAL_PORT_PHYSICAL_MODE_S10_FULL:
						if((ret = cpssDxChPortSpeedSet(dev_id, hport_id, CPSS_PORT_SPEED_10_E)) != GT_OK)
						{
							return SW_FAIL;
						}
						if((ret = cpssDxChPortDuplexModeSet(dev_id, hport_id, CPSS_PORT_FULL_DUPLEX_E)) != GT_OK)
						{
							return SW_FAIL;
						}
						break;

					case FAL_PORT_PHYSICAL_MODE_S100_HALF:
						if((ret = cpssDxChPortSpeedSet(dev_id, hport_id, CPSS_PORT_SPEED_100_E)) != GT_OK)
						{
							return SW_FAIL;
						}
						if((ret = cpssDxChPortDuplexModeSet(dev_id, hport_id, CPSS_PORT_HALF_DUPLEX_E)) != GT_OK)
						{
							return SW_FAIL;
						}
						break;

					case FAL_PORT_PHYSICAL_MODE_S100_FULL:
						if((ret = cpssDxChPortSpeedSet(dev_id, hport_id, CPSS_PORT_SPEED_100_E)) != GT_OK)
						{
							return SW_FAIL;
						}
						if((ret = cpssDxChPortDuplexModeSet(dev_id, hport_id, CPSS_PORT_FULL_DUPLEX_E)) != GT_OK)
						{
							return SW_FAIL;
						}
						break;

					case FAL_PORT_PHYSICAL_MODE_S1000_FULL:
						if((ret = cpssDxChPortSpeedSet(dev_id, hport_id, CPSS_PORT_SPEED_1000_E)) != GT_OK)
						{
							return SW_FAIL;
						}
						if((ret = cpssDxChPortDuplexModeSet(dev_id, hport_id, CPSS_PORT_FULL_DUPLEX_E)) != GT_OK)
						{
							return SW_FAIL;
						}
						break;

					default:
						break;
				}		
			break;
		}	
	}

    return rv;
}

static inline sw_error_t _mv3236_port_physical_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t *mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	printf("Not Support This Feature\n");
	return SW_FAIL;
}

static inline sw_error_t _mv3236_port_physical_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_status_t *status)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		CPSS_PORT_DUPLEX_ENT duplex_mode;
		CPSS_PORT_SPEED_ENT speed_mode;
		
	    if((ret = cpssDxChPortDuplexModeGet(dev_id, hport_id,&duplex_mode)) != GT_OK) {
	        return SW_FAIL;
	    }

		if((ret = cpssDxChPortSpeedGet(dev_id,hport_id,&speed_mode)) != GT_OK) {
			return SW_FAIL;
		}
		
		switch(speed_mode) {
			case CPSS_PORT_SPEED_10_E:
			*status = (duplex_mode == CPSS_PORT_FULL_DUPLEX_E)? FAL_PORT_PHYSICAL_STATUS_S10_FULL: FAL_PORT_PHYSICAL_STATUS_S10_HALF;
			break;

			case CPSS_PORT_SPEED_100_E:
			*status = (duplex_mode == CPSS_PORT_FULL_DUPLEX_E)? FAL_PORT_PHYSICAL_STATUS_S100_FULL: FAL_PORT_PHYSICAL_STATUS_S100_HALF;
			break;

			case CPSS_PORT_SPEED_1000_E:/*FIXME*/
			*status = FAL_PORT_PHYSICAL_STATUS_S1000_FULL;
			break;

			default:
			*status = FAL_PORT_PHYSICAL_STATUS_UNKOWN;
			break;
		}
	}	

    return rv;
}

static inline sw_error_t _mv3236_port_speed_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_speed_t *speed)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		CPSS_PORT_SPEED_ENT speed_mode;
		
		if((ret = cpssDxChPortSpeedGet(dev_id,hport_id,&speed_mode)) != GT_OK) {
			return SW_FAIL;
		}

		switch(speed_mode) {
			case CPSS_PORT_SPEED_1000_E:
			*speed = FAL_PORT_SPEED_1000;
			break;

			case CPSS_PORT_SPEED_100_E:
			*speed = FAL_PORT_SPEED_100;
			break;

			default:
			*speed = FAL_PORT_SPEED_10;
			break;
		}
	}

    return rv;
}

static inline sw_error_t _mv3236_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_duplex_t *duplex)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		CPSS_PORT_DUPLEX_ENT duplex_mode;
		
	    if((ret = cpssDxChPortDuplexModeGet(dev_id, hport_id,&duplex_mode)) != GT_OK) {
	        return SW_FAIL;
	    }

		if(duplex_mode == CPSS_PORT_FULL_DUPLEX_E)
			*duplex = FAL_PORT_DUPLEX_FULL;
		else
			*duplex = FAL_PORT_DUPLEX_HALF;
			
	}	

    return rv;
}

static inline sw_error_t _mv3236_port_flowctrl_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;

		/*below steps are needed as lua function flow_control_func mentioned
		 * set FC mode
		 * link status of port checking
		 * Port full duplex mode checking:only full duplex supported, FIXME
		 * Flow control enabling condition checking: only speed < PRV_CPSS_PORT_XG_E supported, FIXME
		 * Port flow-control autonegatiation mode setting
		 * */
		if(enable == FAL_ENABLE) {
			/* Program Phy's Pause bit in AutoNegotiation Advertisement Register. */
			if((ret = cpssDxChPortFlowControlEnableSet(dev_id, hport_id, CPSS_PORT_FLOW_CONTROL_RX_TX_E)) != GT_OK) {
				printf("Error: cpssDxChPortFlowControlEnableSet! lport=%d\n",port_id);
				return SW_FAIL;
			}

		} else {
			/* Program Phy's Pause bit in AutoNegotiation Advertisement Register. */
			if((ret = cpssDxChPortFlowControlEnableSet(dev_id, hport_id, CPSS_PORT_FLOW_CONTROL_DISABLE_E)) != GT_OK) {
				printf("Error: cpssDxChPortFlowControlEnableSet! lport=%d\n",port_id);
				return SW_FAIL;
			}
		}
	}

    return rv;
}

static inline sw_error_t _mv3236_port_flowctrl_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		GT_BOOL forcefc, fcvalue;

		if((ret = cpssDxChPortFlowControlEnableGet(dev_id, hport_id, &forcefc)) != GT_OK) {
			return SW_FAIL;
		}
		
		if(forcefc == CPSS_PORT_FLOW_CONTROL_RX_TX_E)
			*enable = FAL_ENABLE;
		else
			*enable = FAL_DISABLE;
	}	

    return rv;
}

sw_error_t _mv3236_port_dot1d_state_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t state)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;

    /*note:cpssDxChBrgVlanToStpIdBind must be set before call this routine*/
    SW_OUT_ON_ERROR_FL((rv = cpssDxChBrgStpStateSet(dev_id,  port_id, STPINDEX, state-1)));

    return rv;

out:
	SW_PRINTFL(WARN," %s error %d \n", __FUNCTION__);
    return rv;
}

static inline sw_error_t _mv3236_port_dot1d_state_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t *state)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;
#if 0
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		
		GT_STATUS ret;
		GT_PORT_STP_STATE stpState;

	    if((ret = gstpGetPortState(pJwDev, hport_id, &stpState)) != GT_OK) {
	        return SW_FAIL;
	    }
		
		switch(stpState) {
			case GT_PORT_DISABLE:
			*state = FAL_PORT_DOT1D_STATE_DISABLED;
			break;

			case GT_PORT_BLOCKING:
			*state = FAL_PORT_DOT1D_STATE_BLOCKING;			
			break;

			case GT_PORT_LEARNING:
			*state = FAL_PORT_DOT1D_STATE_LEARNING;				
			break;

			case GT_PORT_FORWARDING:
			*state = FAL_PORT_DOT1D_STATE_FORWARDING;				
			break;

			default:
			return SW_FAIL;
		}
	}
#endif
    return rv;
}

sw_error_t mv3236_port_type_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_type_t *type)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_type_get(dev_id, port_id, type);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_link_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_link_status_t *status)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_link_status_get(dev_id, port_id, status);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_admin_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_admin_mode_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_admin_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_admin_mode_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_physical_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_physical_mode_set(dev_id, port_id, mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_physical_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t *mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_physical_mode_get(dev_id, port_id, mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_physical_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_status_t *status)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_physical_status_get(dev_id, port_id, status);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_speed_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_speed_t *speed)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_speed_get(dev_id, port_id, speed);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_duplex_t *duplex)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_duplex_get(dev_id, port_id, duplex);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_flowctrl_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_flowctrl_mode_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_flowctrl_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_flowctrl_mode_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_dot1d_state_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t state)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_dot1d_state_set(dev_id, port_id, state);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_dot1d_state_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t *state)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_dot1d_state_get(dev_id, port_id, state);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_test_show_all(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	hsl_dev_t *pdev = NULL;
	a_uint32_t lport;
	fal_port_type_t lport_type;
	fal_port_link_status_t link_status;
	fal_enable_t admin_status;
	fal_port_physical_status_t physical_status;
	fal_enable_t flowctrl_mode;
	fal_port_dot1d_state_t dot1dState;
	
	sal_sys_model_t model;
	
	printf("\n");
	printf("  Port Status Information Display :\n");
	printf("  =========================================================================\n");
	printf("  Port  Type  AdminMode  LinkState   Speed-Duplex  FlowControl  Dot1dState \n");
	printf("  =========================================================================\n");
	
    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;

	SW_RTN_ON_ERROR(sal_sys_model_get(&model));
	switch(model.model_id) {
#if 0
		case GE_Z20208D:
			for(lport=0; lport<pdev->nr_ports; lport++) {
				SW_RTN_ON_ERROR(mv3236_port_type_get(dev_id, lport, &lport_type));
				SW_RTN_ON_ERROR(mv3236_port_link_status_get(dev_id, lport, &link_status));
				SW_RTN_ON_ERROR(mv3236_port_admin_mode_get(dev_id, lport, &admin_status));
				SW_RTN_ON_ERROR(mv3236_port_physical_status_get(dev_id, lport, &physical_status));
				SW_RTN_ON_ERROR(mv3236_port_flowctrl_mode_get(dev_id, lport, &flowctrl_mode));
				SW_RTN_ON_ERROR(mv3236_port_dot1d_state_get(dev_id, lport, &dot1dState));
				
				printf("   %02d    %-3s   %-9s    %-6s  %13s    %-9s  %-10s\r\n", lport, 	
					(lport_type == FAL_PORT_TYPE_FE)? "FE" : \
					(lport_type == FAL_PORT_TYPE_FX)? "FX" : \
					(lport_type == FAL_PORT_TYPE_GE)? "GE" : \
					(lport_type == FAL_PORT_TYPE_GX)? "GX" : "UN",
					(admin_status == FAL_ENABLE)? "Enabled" : "Disabled",
					(link_status == FAL_PORT_LINK_STATUS_UP)? "Up" : "Down",
					(link_status == FAL_PORT_LINK_STATUS_DOWN)? "--     " : \
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S10_HALF)? "10Mbps-Half" : 
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S10_FULL)? "10Mbps-Full" : 
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S100_HALF)? "100Mbps-Half" : 
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S100_FULL)? "100Mbps-Full" : 
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S1000_FULL)? "1000Mbps-Full" : "Unkown",
					(flowctrl_mode == FAL_ENABLE)? "Enabled" : "Disabled",
					(dot1dState == FAL_PORT_DOT1D_STATE_DISABLED)? "Disabled" : 
					(dot1dState == FAL_PORT_DOT1D_STATE_BLOCKING)? "Blocking" : 
					(dot1dState == FAL_PORT_DOT1D_STATE_LEARNING)? "Learning" : 
					(dot1dState == FAL_PORT_DOT1D_STATE_FORWARDING)? "Forwarding" : "Unkown");	
			}
			printf("\n");
		break;

		case GE_Z20505D:
			for(lport=0; lport<pdev->nr_ports; lport++) {
				SW_RTN_ON_ERROR(mv3236_port_type_get(dev_id, lport, &lport_type));
				SW_RTN_ON_ERROR(mv3236_port_link_status_get(dev_id, lport, &link_status));
				SW_RTN_ON_ERROR(mv3236_port_admin_mode_get(dev_id, lport, &admin_status));
				SW_RTN_ON_ERROR(mv3236_port_physical_status_get(dev_id, lport, &physical_status));
				SW_RTN_ON_ERROR(mv3236_port_flowctrl_mode_get(dev_id, lport, &flowctrl_mode));
				SW_RTN_ON_ERROR(mv3236_port_dot1d_state_get(dev_id, lport, &dot1dState));
				
				printf("   %02d    %-3s   %-9s    %-6s  %13s    %-9s  %-10s\r\n", lport, 	
					(lport_type == FAL_PORT_TYPE_FE)? "FE" : \
					(lport_type == FAL_PORT_TYPE_FX)? "FX" : \
					(lport_type == FAL_PORT_TYPE_GE)? "GE" : \
					(lport_type == FAL_PORT_TYPE_GX)? "GX" : "UN",
					(admin_status == FAL_ENABLE)? "Enabled" : "Disabled",
					(link_status == FAL_PORT_LINK_STATUS_UP)? "Up" : "Down",
					(link_status == FAL_PORT_LINK_STATUS_DOWN)? "--     " : \
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S10_HALF)? "10Mbps-Half" : 
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S10_FULL)? "10Mbps-Full" : 
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S100_HALF)? "100Mbps-Half" : 
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S100_FULL)? "100Mbps-Full" : 
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S1000_FULL)? "1000Mbps-Full" : "Unkown",
					(flowctrl_mode == FAL_ENABLE)? "Enabled" : "Disabled",
					(dot1dState == FAL_PORT_DOT1D_STATE_DISABLED)? "Disabled" : 
					(dot1dState == FAL_PORT_DOT1D_STATE_BLOCKING)? "Blocking" : 
					(dot1dState == FAL_PORT_DOT1D_STATE_LEARNING)? "Learning" : 
					(dot1dState == FAL_PORT_DOT1D_STATE_FORWARDING)? "Forwarding" : "Unkown");	
			}
			printf("\n");
		break;
#endif
		default:
		break;
	}
/*
	for(lport=0; lport<pdev->nr_ports; lport++) {
	
		SW_RTN_ON_ERROR(mv3236_port_type_get(dev_id, lport, &lport_type));
		SW_RTN_ON_ERROR(mv3236_port_link_status_get(dev_id, lport, &link_status));
		SW_RTN_ON_ERROR(mv3236_port_admin_mode_get(dev_id, lport, &admin_status));
		SW_RTN_ON_ERROR(mv3236_port_physical_status_get(dev_id, lport, &physical_status));
		SW_RTN_ON_ERROR(mv3236_port_flowctrl_mode_get(dev_id, lport, &flowctrl_mode));
		SW_RTN_ON_ERROR(mv3236_port_dot1d_state_get(dev_id, lport, &dot1dState));
		
		printf("   %02d    %-3s   %-9s    %-6s  %13s    %-9s  %-10s\r\n", lport, 	
			(lport_type == FAL_PORT_TYPE_FE)? "FE" : \
			(lport_type == FAL_PORT_TYPE_FX)? "FX" : \
			(lport_type == FAL_PORT_TYPE_GE)? "GE" : \
			(lport_type == FAL_PORT_TYPE_GX)? "GX" : "UN",
			(admin_status == FAL_ENABLE)? "Enabled" : "Disabled",
			(link_status == FAL_PORT_LINK_STATUS_UP)? "Up" : "Down",
			(link_status == FAL_PORT_LINK_STATUS_DOWN)? "--     " : \
			(physical_status == FAL_PORT_PHYSICAL_STATUS_S10_HALF)? "10Mbps-Half" : 
			(physical_status == FAL_PORT_PHYSICAL_STATUS_S10_FULL)? "10Mbps-Full" : 
			(physical_status == FAL_PORT_PHYSICAL_STATUS_S100_HALF)? "100Mbps-Half" : 
			(physical_status == FAL_PORT_PHYSICAL_STATUS_S100_FULL)? "100Mbps-Full" : 
			(physical_status == FAL_PORT_PHYSICAL_STATUS_S1000_FULL)? "1000Mbps-Full" : "Unkown",
			(flowctrl_mode == FAL_ENABLE)? "Enabled" : "Disabled",
			(dot1dState == FAL_PORT_DOT1D_STATE_DISABLED)? "Disabled" : 
			(dot1dState == FAL_PORT_DOT1D_STATE_BLOCKING)? "Blocking" : 
			(dot1dState == FAL_PORT_DOT1D_STATE_LEARNING)? "Learning" : 
			(dot1dState == FAL_PORT_DOT1D_STATE_FORWARDING)? "Forwarding" : "Unkown");	
	}
	printf("\n");
	*/
    return SW_OK;
}


sw_error_t mv3236_port_ctrl_init(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;
    sw_error_t rv = SW_OK;
	sal_sys_model_t model;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);

	SW_RTN_ON_ERROR(sal_sys_model_get(&model));
	switch(model.model_id) {
#if 0
		case GE_Z20208D:
			aos_mem_copy(port_map, port_map_20208d, sizeof(port_map));
		break;

		case GE_Z20505D:
			aos_mem_copy(port_map, port_map_20505d, sizeof(port_map));
		break;
#endif
		default:
		break;
	}
	
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
	
    p_api->port_type_get= mv3236_port_type_get;
    p_api->port_link_status_get = mv3236_port_link_status_get;
    p_api->port_admin_mode_set = mv3236_port_admin_mode_set;
    p_api->port_admin_mode_get = mv3236_port_admin_mode_get;
    p_api->port_physical_mode_set = mv3236_port_physical_mode_set;
    p_api->port_physical_mode_get = mv3236_port_physical_mode_get;
    p_api->port_physical_status_get= mv3236_port_physical_status_get;
	p_api->port_speed_get= mv3236_port_speed_get;
	p_api->port_duplex_get= mv3236_port_duplex_get;
    p_api->port_flowctrl_mode_set = mv3236_port_flowctrl_mode_set;
    p_api->port_flowctrl_mode_get = mv3236_port_flowctrl_mode_get;
    p_api->port_dot1d_state_set = mv3236_port_dot1d_state_set;
    p_api->port_dot1d_state_get = mv3236_port_dot1d_state_get;
    p_api->port_test_show_all = mv3236_port_test_show_all;
	
    return rv;
}

sw_error_t comb_port_led_mode_detect(fal_port_t lport,unsigned int *led_mode)
{
	printf("%s.%d  not support yet \n",__FUNCTION__,__LINE__);
	return SW_OK;
}

sw_error_t comb_port_fiber_link_status_detect(fal_port_t lport, unsigned int *media_status)
{
	printf("%s.%d  not support yet \n",__FUNCTION__,__LINE__);
	return SW_OK;
}

sw_error_t comb_port_copper_link_status_detect(fal_port_t lport,unsigned int *media_status)
{
	printf("%s.%d  not support yet \n",__FUNCTION__,__LINE__);
	return SW_OK;
}

sw_error_t switch_port_register_get(fal_port_t port_id, unsigned int regaddr, unsigned int *data)
{
	printf("%s.%d  not support yet \n",__FUNCTION__,__LINE__);
	return SW_OK;
}

sw_error_t switch_port_register_set(fal_port_t port_id,unsigned int regaddr,unsigned int data)
{
	printf("%s.%d  not support yet \n",__FUNCTION__,__LINE__);
	return SW_OK;
}

sw_error_t comb_port_led_mode_select(fal_port_t lport,unsigned int media_link)
{
	printf("%s.%d  not support yet \n",__FUNCTION__,__LINE__);
	return SW_OK;
}

sw_error_t switch_global_register_set(unsigned int regaddr,unsigned int data)
{
	printf("%s.%d  not support yet \n",__FUNCTION__,__LINE__);
	return SW_OK;
}

sw_error_t switch_global_register_get(unsigned int regaddr, unsigned int *data)
{
	printf("%s.%d  not support yet \n",__FUNCTION__,__LINE__);
	return SW_OK;
}
