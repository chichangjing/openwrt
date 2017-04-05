 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv88e6097_port_ctrl.h"
#include "mv88e6097_reg.h"
#include "msApi.h"
#include "gtHwCntl.h"
#include "gtDrvSwRegs.h"

#include "joyware.h"
#include<sys/time.h>

#include <sys/select.h>



extern GT_QD_DEV *pJwDev;
static a_uint32_t PowerDownFlag[CONFIG_JWS_PORT_TOTAL_NR+1] = {0};


sw_error_t map_lport2hport(a_uint32_t lport, a_uint32_t *hport)
{
	struct jws_port_info *Port_Info,portinfo[CONFIG_JWS_PORT_TOTAL_NR + 1];
	
	Port_Info = portinfo;
	jws_port_info_get(Port_Info);
	*hport = Port_Info[lport].physical_seq;
	
	return SW_OK;	
}

sw_error_t map_hport2lport(a_uint32_t hport, a_uint32_t *lport)
{	
	struct jws_port_info *Port_Info,portinfo[CONFIG_JWS_PORT_TOTAL_NR + 1];
	char i;
	
	Port_Info = portinfo;
	jws_port_info_get(Port_Info);
	for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR+1; i++) {
		if(hport == Port_Info[i].physical_seq) {
			*lport = Port_Info[i].logic_seq;
			break;
		}
	}
	
	return SW_OK;
}


/* one byte shift high/low bit */
a_uint8_t shift_high_low_bit(a_uint8_t data)
{
	data = (data<<4) | (data>>4);
	data = ((data<<2) & 0xcc) | ((data>>2) & 0x33);
	data = ((data<<1) & 0xaa) | ((data>>1) & 0x55);
	return data;
}


#define MV88E1112FIBER_OR_COPPER_LINK_UP	0x08
#define MV88E1112FIBER_LINK					0x80
#define MV88E1112100BASE_FX_MODE 			0x4000

#define MARVELL1112PHY_PAGE_ADDRESS		22
#define MARVELL1112PHY_MODE_SELECT 		16
#define MARVELL1112PHY_LINK_STATUS		17
#define MARVELL1112PHY_CONTROL_REGISTER 0

/* 88E1112 PHY page 117 */
#define FORCE_1000BASEX_MODE		0x7		//MOde Select: bit[9:7] 111
#define FORCE_100BASEFX_MODE		0x0		//MOde Select: bit[9:7] 000
#define FORCE_COPPER_MODE			0x5		//MOde Select: bit[9:7] 101
#define AUTO_1000BASEX_COPPER_MODE	0x3		//MOde Select: bit[9:7] 011

#define FIBER_LED_MODE		0x1288			//2: init pin function as normal init operation
#define COPPER_LED_MODE		0x8217			//2: init pin function as normal init operation

static inline sw_error_t _mv88e6097_port_type_get(a_uint32_t dev_id,fal_port_t port_id,fal_port_type_t * type_info, fal_port_comb_t *comb_port)
{
	struct jws_port_info *Port_Info,portinfo[CONFIG_JWS_PORT_TOTAL_NR + 1];
	int type, speed;
	GT_U16 link_status, media_mode;
	GT_LPORT hport;
	GT_STATUS ret;

	Port_Info = portinfo;

	map_lport2hport(port_id, &hport);
	jws_port_info_get(Port_Info);
	type = Port_Info[port_id].type;
	speed = Port_Info[port_id].speed;
	switch(type) {
		case JWS_PORT_TYPE_ETHER_E:
			/* Ether 100M */
			if(speed == JWS_PORT_SPEED_MEGA_E)
				*type_info = FAL_PORT_TYPE_FE;
			/* Ether 1000M */
			else if(speed == JWS_PORT_SPEED_GIGA_E)
				*type_info = FAL_PORT_TYPE_GE;
			
			/* Normal Port Type */
			*comb_port = FAL_PORT_NOT_COMB_PORT;
		break;

		case JWS_PORT_TYPE_FIBER_E:
			/* Fiber 100M */
			if(speed == JWS_PORT_SPEED_MEGA_E)
				*type_info = FAL_PORT_TYPE_FX;
			/* Fiber 1000M */
			else if(speed == JWS_PORT_SPEED_GIGA_E)
				*type_info = FAL_PORT_TYPE_GX;

			/* Normal Port Type */
			*comb_port = FAL_PORT_NOT_COMB_PORT;
		break;

		case JWS_PORT_TYPE_MANAGE_E:
			/* Cpu port Type Unknown */
			*type_info = FAL_PORT_TYPE_CPU;

			/* Normal Port Type */
			*comb_port = FAL_PORT_NOT_COMB_PORT;
		break;

		case JWS_PORT_TYPE_COMB_E:
			/* Comb port Type */
			if((ret = gprtSetPhyReg(pJwDev,hport,MARVELL1112PHY_PAGE_ADDRESS,0x2)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_PAGE_ADDRESS Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			if((ret = gprtGetPhyReg(pJwDev,hport,MARVELL1112PHY_MODE_SELECT,&media_mode)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_MODE_SELECT Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			
			if((ret = gprtSetPhyReg(pJwDev,hport,MARVELL1112PHY_PAGE_ADDRESS,0x1)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_PAGE_ADDRESS Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			if((ret = gprtGetPhyReg(pJwDev,hport,MARVELL1112PHY_LINK_STATUS,&link_status)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_LINK_STATUS Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}

			/* 88E1112 PHY Mode Select, page 117 */
			media_mode = ((media_mode & 0x0380) >> 7);

			/* Comb Port Type */
			*comb_port = FAL_PORT_COMB_PORT;
			
			if((link_status & MV88E1112FIBER_OR_COPPER_LINK_UP) != 0){
				/* Fiber or Copper link, MV88E1112100BASE_FX_MODE == 0 1000base-x, 1: 100base-fx*/
 				if(((link_status & MV88E1112FIBER_LINK) != 0) && ((link_status & MV88E1112100BASE_FX_MODE) == 0)){
 					/* 1000M Fiber link */
 					*type_info = FAL_PORT_TYPE_GX;
 				} else if(((link_status & MV88E1112FIBER_LINK) != 0) && ((link_status & MV88E1112100BASE_FX_MODE) != 0)) {
					/* 100M Fiber link */
 					*type_info = FAL_PORT_TYPE_FX;
				}
				else{
					/* Copper link */
					*type_info = FAL_PORT_TYPE_GE;
				}
			}else{
				/* Force Media */
				if(media_mode == FORCE_1000BASEX_MODE)
					*type_info = FAL_PORT_TYPE_GX;
				else if(media_mode == FORCE_100BASEFX_MODE)
					*type_info = FAL_PORT_TYPE_FX;
				else if(media_mode == FORCE_COPPER_MODE)
					*type_info = FAL_PORT_TYPE_GE;
				else if(media_mode == AUTO_1000BASEX_COPPER_MODE)
					*type_info = FAL_PORT_TYPE_COMB;
			}				
		break;

		default:
		break;
	}
		
	return SW_OK;		
}

static inline sw_error_t _mv88e6097_port_link_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_link_status_t *status)
{
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;	

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		GT_BOOL link_state;
		
	    if((ret = gprtGetLinkState(pJwDev, hport_id, &link_state)) != GT_OK) {
			printf("Error: SPF gprtGetLinkState Fail! lport=%d\n",port_id);
	        return SW_FAIL;
	    }
			
		if(link_state == GT_TRUE)
			*status = FAL_PORT_LINK_STATUS_UP;
		else
			*status = FAL_PORT_LINK_STATUS_DOWN;
	}
	
    return rv;
}

static inline sw_error_t _mv88e6097_port_admin_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable)
{
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;
	sal_sys_model_t model;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;

		SW_RTN_ON_ERROR(sal_sys_model_get(&model));
		switch(model.model_id) {
			case GE_Z20208D_2008:
			case GE_Z20208D_1008:
			case GE_Z20208D_2004:
			case GE_Z20208D_1004:
			case GE_Z20204A_2013:
			case GE_Z20204A_1013:
			case GE_Z20204A_2017:
			case GE_Z20204A_1017:
			case GE_Z20208D_2008_P:
			case GE_Z20208D_2004_P:
				if((hport_id == 10) || (hport_id == 8)) {
					if(enable == FAL_ENABLE) {
					    if((ret = gpcsSetForcedLink(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
							printf("Error: SPF gpcsSetForcedLink Fail! lport=%d\n",port_id);
					        return SW_FAIL;
					    }		
					} else {
					    if((ret = gpcsSetLinkValue(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
							printf("Error: SPF gpcsSetLinkValue Fail! lport=%d\n",port_id);
					        return SW_FAIL;
					    }
					    if((ret = gpcsSetForcedLink(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
							printf("Error: SPF gpcsSetForcedLink Fail! lport=%d\n",port_id);
					        return SW_FAIL;
					    }				
					}
				} else {
					if(enable == FAL_ENABLE) {
					    if((ret = gprtPortPowerDown(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
							printf("Error: FE gprtPortPowerDown Fail! lport=%d\n",port_id);
					        return SW_FAIL;
					    } else {
							PowerDownFlag[hport_id] = 0;
						}			
					} else {
					    if((ret = gprtPortPowerDown(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
							printf("Error: FE gprtPortPowerDown Fail! lport=%d\n",port_id);
					        return SW_FAIL;
					    } else {
							PowerDownFlag[hport_id] = 1;
						}
					}
				}
			break;

			case GE_Z20505D_1414:
				if(hport_id == 10) {
					if(enable == FAL_ENABLE) {
					    if((ret = gpcsSetForcedLink(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
							printf("Error: SPF gpcsSetForcedLink Fail! lport=%d\n",port_id);
					        return SW_FAIL;
					    }		
					} else {
					    if((ret = gpcsSetLinkValue(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
							printf("Error: SPF gpcsSetLinkValue Fail! lport=%d\n",port_id);
					        return SW_FAIL;
					    }
					    if((ret = gpcsSetForcedLink(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
							printf("Error: SPF gpcsSetForcedLink Fail! lport=%d\n",port_id);
					        return SW_FAIL;
					    }				
					}
				} else {
					if(enable == FAL_ENABLE) {
					    if((ret = gprtPortPowerDown(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
							printf("Error: FE gprtPortPowerDown Fail! lport=%d\n",port_id);
					        return SW_FAIL;
					    } else {
							PowerDownFlag[hport_id] = 0;
						}			
					} else {
					    if((ret = gprtPortPowerDown(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
							printf("Error: FE gprtPortPowerDown Fail! lport=%d\n",port_id);
					        return SW_FAIL;
					    } else {
							PowerDownFlag[hport_id] = 1;
						}
					}
				}		
			break;

			case GE_SDI1000_0014_T:
			case GE_SDI1000_0014_R:
				if((hport_id == 0) || (hport_id == 1) || (hport_id == 2) || (hport_id == 3) || (hport_id == 8)){
					if(enable == FAL_ENABLE) {
					    if((ret = gprtPortPowerDown(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
							printf("Error: FE gprtPortPowerDown Fail! lport=%d\n",port_id);
					        return SW_FAIL;
					    } else {
							PowerDownFlag[hport_id] = 0;
						}			
					} else {
					    if((ret = gprtPortPowerDown(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
							printf("Error: FE gprtPortPowerDown Fail! lport=%d\n",port_id);
					        return SW_FAIL;
					    } else {
							PowerDownFlag[hport_id] = 1;
						}
					}
				}
			break;

			case GE_Z20210D_2028:
			case GE_Z20210D_2024:
				if(enable == FAL_ENABLE) {
				    if((ret = gprtPortPowerDown(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
						printf("Error: FE gprtPortPowerDown Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    } else {
						PowerDownFlag[hport_id] = 0;
					}			
				} else {
				    if((ret = gprtPortPowerDown(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
						printf("Error: FE gprtPortPowerDown Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    } else {
						PowerDownFlag[hport_id] = 1;
					}
				}
			break;

			default:
			break;
		}
		
	}
	
    return rv;
}

static inline sw_error_t _mv88e6097_port_admin_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable)
{
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
			case GE_Z20208D_2008:
			case GE_Z20208D_1008:
			case GE_Z20208D_2004:
			case GE_Z20208D_1004:
			case GE_Z20208D_2008_P:
			case GE_Z20208D_2004_P:
				if((hport_id == 10) || (hport_id == 9) || (hport_id == 8)) {
					if((ret = gpcsGetForcedLink(pJwDev, hport_id, &forcelink)) != GT_OK) {
						printf("Error: gpcsGetForcedLink Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    }
					if((ret = gpcsGetLinkValue(pJwDev, hport_id, &linkvalue)) != GT_OK) {
						printf("Error: gpcsGetLinkValue Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    }
					if((forcelink == GT_TRUE) && (linkvalue == GT_FALSE)) {
						*enable = FAL_DISABLE;
					} else {
						*enable = FAL_ENABLE;
					}
				} else {
				    if((ret = hwGetPhyRegField(pJwDev, hport_id, QD_PHY_CONTROL_REG, 11, 1, &u16Data)) != GT_OK) {
						printf("Error: hwGetPhyRegField Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    }
					if(u16Data)
						*enable = FAL_DISABLE;
					else
						*enable = FAL_ENABLE;
				}			
			break;

			case GE_Z20505D_1414:
			case GE_SDI1000_0014_T:
			case GE_SDI1000_0014_R:
				if((hport_id == 10) || (hport_id == 9)) {
					if((ret = gpcsGetForcedLink(pJwDev, hport_id, &forcelink)) != GT_OK) {
						printf("Error: gpcsGetForcedLink Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    }
					if((ret = gpcsGetLinkValue(pJwDev, hport_id, &linkvalue)) != GT_OK) {
						printf("Error: gpcsGetLinkValue Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    }
					if((forcelink == GT_TRUE) && (linkvalue == GT_FALSE)) {
						*enable = FAL_DISABLE;
					} else {
						*enable = FAL_ENABLE;
					}
				} else {
				    if((ret = hwGetPhyRegField(pJwDev, hport_id, QD_PHY_CONTROL_REG, 11, 1, &u16Data)) != GT_OK) {
						printf("Error: hwGetPhyRegField Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    }
					if(u16Data)
						*enable = FAL_DISABLE;
					else
						*enable = FAL_ENABLE;
				}
			break;

			case GE_Z20204A_2013:
			case GE_Z20204A_1013:
			case GE_Z20204A_2017:
			case GE_Z20204A_1017:
				if((hport_id == 10) || (hport_id == 8)) {
					if((ret = gpcsGetForcedLink(pJwDev, hport_id, &forcelink)) != GT_OK) {
						printf("Error: gpcsGetForcedLink Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    }
					if((ret = gpcsGetLinkValue(pJwDev, hport_id, &linkvalue)) != GT_OK) {
						printf("Error: gpcsGetLinkValue Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    }
					if((forcelink == GT_TRUE) && (linkvalue == GT_FALSE)) {
						*enable = FAL_DISABLE;
					} else {
						*enable = FAL_ENABLE;
					}
				} else {
				    if((ret = hwGetPhyRegField(pJwDev, hport_id, QD_PHY_CONTROL_REG, 11, 1, &u16Data)) != GT_OK) {
						printf("Error: hwGetPhyRegField Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    }
					if(u16Data)
						*enable = FAL_DISABLE;
					else
						*enable = FAL_ENABLE;
				}
			break;

			case GE_Z20210D_2028:
			case GE_Z20210D_2024:
				if(hport_id == 9) {
					if((ret = gpcsGetForcedLink(pJwDev, hport_id, &forcelink)) != GT_OK) {
						printf("Error: gpcsGetForcedLink Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    }
					if((ret = gpcsGetLinkValue(pJwDev, hport_id, &linkvalue)) != GT_OK) {
						printf("Error: gpcsGetLinkValue Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    }
					if((forcelink == GT_TRUE) && (linkvalue == GT_FALSE)) {
						*enable = FAL_DISABLE;
					} else {
						*enable = FAL_ENABLE;
					}
				} else {
				    if((ret = hwGetPhyRegField(pJwDev, hport_id, QD_PHY_CONTROL_REG, 11, 1, &u16Data)) != GT_OK) {
						printf("Error: hwGetPhyRegField Fail! lport=%d\n",port_id);
				        return SW_FAIL;
				    }
					if(u16Data)
						*enable = FAL_DISABLE;
					else
						*enable = FAL_ENABLE;
				}
			break;

			default:
			break;
		}
		
		
	}	
    return rv;
}

static inline sw_error_t _mv88e6097_port_physical_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t mode)
{
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;
	sal_sys_model_t model;
	unsigned int pageaddr;
	
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;


		if(PowerDownFlag[hport_id] == 1) {
			printf("PowerDown, exit!\n");			
			return rv;
		}

		SW_RTN_ON_ERROR(sal_sys_model_get(&model));
		switch(model.model_id) {
			case GE_Z20208D_2008:
			case GE_Z20208D_1008:
			case GE_Z20208D_2004:
			case GE_Z20208D_1004:
			case GE_Z20208D_2008_P:
			case GE_Z20208D_2004_P:
				switch(mode) {
					case FAL_PORT_PHYSICAL_MODE_AUTONEG:
						if((hport_id != 8) && (hport_id != 9) && (hport_id != 10)) {
							/* Enable auto-negotiation */
							if((ret = gprtPortAutoNegEnable(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
								printf("Error: FE gprtPortAutoNegEnable Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Restart auto-negotiation */
							if((ret = gprtPortRestartAutoNeg(pJwDev, hport_id)) != GT_OK) {
								printf("Error: FE gprtPortRestartAutoNeg Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* LED0 Normal */
							if((ret=hwSetPhyRegField(pJwDev,hport_id, 0x19, 0, 2, 0)) != GT_OK) {
								printf("Error: FE hwSetPhyRegField LED0 ON Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
						}else if((hport_id == 8) || (hport_id == 10)) {
							/* Enable auto-negotiation */
							if((ret = gpcsSetPCSAnEn(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetPCSAnEn, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
							/* Restart auto-negotiation */
							if((ret = gpcsSetRestartPCSAn(pJwDev, hport_id)) != GT_OK) {
								printf("Error: gpcsSetRestartPCSAn Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Set Normal speed detection occurs */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, 0x03)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
							/* Set Normal duplex detection occurs */					
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S10_HALF:
						if((hport_id != 8) && (hport_id != 9) && (hport_id != 10)) {
							/* Disable auto-negotiation */
							if((ret = gprtPortAutoNegEnable(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
								printf("Error: FE gprtPortAutoNegEnable Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* LED0 OFF */
							if((ret=hwSetPhyRegField(pJwDev,hport_id, 0x19, 0, 2, 2)) != GT_OK) {
								printf("Error: FE hwSetPhyRegField LED0 OFF Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Set speed to 10Mbps */
							if((ret = gprtSetPortSpeed(pJwDev, hport_id, PHY_SPEED_10_MBPS)) != GT_OK) {
								printf("Error: FE gprtSetPortSpeed 10Mbps Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Set duplex to half-duplex */
							if((ret = gprtSetPortDuplexMode(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
								printf("Error: FE gprtSetPortDuplexMode half-duplex Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S10_FULL:
						if((hport_id != 8) && (hport_id != 9) && (hport_id != 10)) {	
							/* Disable auto-negotiation */
							if((ret = gprtPortAutoNegEnable(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
								printf("Error: FE gprtPortAutoNegEnable Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }					
							/* LED0 OFF */
							if((ret=hwSetPhyRegField(pJwDev,hport_id, 0x19, 0, 2, 2)) != GT_OK) {
								printf("Error: FE hwSetPhyRegField LED0 OFF Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Set speed to 10Mbps */
							if((ret = gprtSetPortSpeed(pJwDev, hport_id, PHY_SPEED_10_MBPS)) != GT_OK) {
								printf("Error: FE gprtSetPortSpeed 10Mbps Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Set duplex to full-duplex */
							if((ret = gprtSetPortDuplexMode(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
								printf("Error: FE gprtSetPortDuplexMode full-duplex Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S100_HALF:
						if((hport_id != 8) && (hport_id != 9) && (hport_id != 10)) {
							/* Disable auto-negotiation */
							if((ret = gprtPortAutoNegEnable(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
								printf("Error: FE gprtPortAutoNegEnable Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Set speed to 100M bps */
							if((ret = gprtSetPortSpeed(pJwDev, hport_id, PHY_SPEED_100_MBPS)) != GT_OK) {
								printf("Error: FE gprtSetPortSpeed 100Mbps Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Set duplex to half-duplex */
							if((ret = gprtSetPortDuplexMode(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
								printf("Error: FE gprtSetPortDuplexMode half-duplex Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* LED0 Normal */
							if((ret=hwSetPhyRegField(pJwDev,hport_id, 0x19, 0, 2, 0)) != GT_OK) {
								printf("Error: FE hwSetPhyRegField LED0 ON Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S100_FULL:
						if((hport_id != 8) && (hport_id != 9) && (hport_id != 10)) {
							/* Disable auto-negotiation */
							if((ret = gprtPortAutoNegEnable(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
								printf("Error: FE gprtPortAutoNegEnable Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Set speed to 100M bps */
							if((ret = gprtSetPortSpeed(pJwDev, hport_id, PHY_SPEED_100_MBPS)) != GT_OK) {
								printf("Error: FE gprtSetPortSpeed 100Mbps Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Set duplex to full-duplex */
							if((ret = gprtSetPortDuplexMode(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
								printf("Error: FE gprtSetPortDuplexMode full-duplex Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* LED0 ON */
							if((ret=hwSetPhyRegField(pJwDev,hport_id, 0x19, 0, 2, 0)) != GT_OK) {
								printf("Error: FE hwSetPhyRegField LED0 ON Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S1000_FULL:
						if((hport_id == 8) || (hport_id == 10)) {
							/* Disable auto-negotiation */
							if((ret = gpcsSetPCSAnEn(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetPCSAnEn, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 1000M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_1000_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to full-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					default:
					break;
				}		
			break;

			case GE_Z20505D_1414:
			case GE_SDI1000_0014_T:
			case GE_SDI1000_0014_R:
				switch(mode) {
					case FAL_PORT_PHYSICAL_MODE_AUTONEG:
						if((hport_id == 0) || (hport_id == 1) || (hport_id == 2) || (hport_id == 3) || (hport_id == 8)) {
						//if((hport_id != 9) && (hport_id != 10)){
							/* Enable auto-negotiation */
							if((ret = gprtPortAutoNegEnable(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
								printf("Error: FE gprtPortAutoNegEnable Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Restart auto-negotiation */
							if((ret = gprtPortRestartAutoNeg(pJwDev, hport_id)) != GT_OK) {
								printf("Error: FE gprtPortRestartAutoNeg Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							
							/* Set speed to Normal mode, not force speed */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, 3)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set Normal duplex detection occurs */					
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }					
						}else if(hport_id == 10) {
							/* Enable auto-negotiation */
							if((ret = gpcsSetPCSAnEn(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetPCSAnEn, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
							/* Restart auto-negotiation */
							if((ret = gpcsSetRestartPCSAn(pJwDev, hport_id)) != GT_OK) {
								printf("Error: gpcsSetRestartPCSAn Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Set Normal speed detection occurs */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, 0x03)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
							/* Set Normal duplex detection occurs */					
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S10_HALF:
						if((hport_id == 0) || (hport_id == 1) || (hport_id == 2) || (hport_id == 3) || (hport_id == 8)) {
							/* Disable auto-negotiation, 10M half-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0x8000)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 10M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_10_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to half-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S10_FULL:
						if((hport_id == 0) || (hport_id == 1) || (hport_id == 2) || (hport_id == 3) || (hport_id == 8)) {	
							/* Disable auto-negotiation, 10M full-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0x8100)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 10M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_10_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to full-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S100_HALF:
						if((hport_id == 0) || (hport_id == 1) || (hport_id == 2) || (hport_id == 3) || (hport_id == 8)) {
							/* Disable auto-negotiation, 100M half-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0xa000)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 100M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_100_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to half-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S100_FULL:
						if((hport_id != 9) && (hport_id != 10)) {						
							/* Disable auto-negotiation, 100M full-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0xa100)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 100M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_100_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to full-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S1000_FULL:
						if(hport_id == 10) {							
							/* Disable auto-negotiation */
							if((ret = gpcsSetPCSAnEn(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetPCSAnEn, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 1000M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_1000_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to full-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
						} else if (hport_id == 8) {						
							/* Disable auto-negotiation, 1000M full-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0x8140)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 1000M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_1000_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to full-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S1000_HALF:
						/* 1000M can not support half mode */
					break;

					default:
					break;
				}		
			break;

			case GE_Z20204A_2013:
			case GE_Z20204A_1013:
			case GE_Z20204A_2017:
			case GE_Z20204A_1017:
				switch(mode) {
					case FAL_PORT_PHYSICAL_MODE_AUTONEG:
						if((hport_id != 2) && (hport_id != 8) && (hport_id != 10)) {
							/* Set speed to Normal mode, not force speed */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, 3)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set Normal duplex detection occurs */					
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }	
							
							/* Enable auto-negotiation */
							if((ret = gprtPortAutoNegEnable(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
								printf("Error: FE gprtPortAutoNegEnable Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Restart auto-negotiation */
							if((ret = gprtPortRestartAutoNeg(pJwDev, hport_id)) != GT_OK) {
								printf("Error: FE gprtPortRestartAutoNeg Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }				
						}else if((hport_id == 8) || (hport_id == 10)) {
							/* Enable auto-negotiation */
							if((ret = gpcsSetPCSAnEn(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetPCSAnEn, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
							/* Restart auto-negotiation */
							if((ret = gpcsSetRestartPCSAn(pJwDev, hport_id)) != GT_OK) {
								printf("Error: gpcsSetRestartPCSAn Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Set Normal speed detection occurs */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, 0x03)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
							/* Set Normal duplex detection occurs */					
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S10_HALF:
						if((hport_id != 2) && (hport_id != 8) && (hport_id != 10)) {
							/* Disable auto-negotiation, 10M half-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0x8000)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 10M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_10_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to half-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S10_FULL:
						if((hport_id != 2) && (hport_id != 8) && (hport_id != 10)) {	
							/* Disable auto-negotiation, 10M full-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0x8100)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 10M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_10_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to full-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S100_HALF:
						if((hport_id != 2) && (hport_id != 8) && (hport_id != 10)) {
							/* Disable auto-negotiation, 100M half-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0xa000)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 100M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_100_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to half-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S100_FULL:
						if((hport_id != 2) && (hport_id != 8) && (hport_id != 10)) {						
							/* Disable auto-negotiation, 100M full-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0xa100)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 100M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_100_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to full-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S1000_FULL:
						if((hport_id == 8) || (hport_id == 10)) {							
							/* Disable auto-negotiation */
							if((ret = gpcsSetPCSAnEn(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetPCSAnEn, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 1000M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_1000_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to full-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, lport=%d, ret=%d\r\n", port_id,ret);
								return SW_FAIL;
					        }
						} else if (hport_id == 9) {						
							/* Disable auto-negotiation, 1000M full-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0x8140)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 1000M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_1000_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to full-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S1000_HALF:
					break;

					default:
					break;
				}		
			break;

			case GE_Z20210D_2028:
			case GE_Z20210D_2024:
				switch(mode){
					case FAL_PORT_PHYSICAL_MODE_COPPER_FIBER_AUTO:
						if((hport_id == 8) || (hport_id == 10)){
							for(pageaddr=0; pageaddr<2; pageaddr++){
								/* Copper/Fiber Auto Operation, pageaddr 0:Copper, pageaddr 1:Fiber */
								/* 88E1112 PHY, Page Address 0x16 Reset to pageaddr */
								hwWritePhyReg(pJwDev, hport_id, 0x16, pageaddr);							
								/* Enable auto-negotiation */
								if((ret = gprtPortAutoNegEnable(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
									printf("Error: FE gprtPortAutoNegEnable Fail! lport=%d\n",port_id);
									return SW_FAIL;
						        }
								/* Restart auto-negotiation */
								if((ret = gprtPortRestartAutoNeg(pJwDev, hport_id)) != GT_OK) {
									printf("Error: FE gprtPortRestartAutoNeg Fail! lport=%d\n",port_id);
									return SW_FAIL;
						        }
								
								/* Set speed to Normal mode, not force speed */
								if((ret = gpcsSetForceSpeed(pJwDev, hport_id, 3)) != GT_OK) {
						            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
									return SW_FAIL;
						        }
								/* Set Normal duplex detection occurs */					
								if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
						            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
									return SW_FAIL;
						        }
							}
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_FIBER_AUTO:
						if((hport_id == 8) || (hport_id == 10)){
							/* Fiber Auto Operation, pageaddr 1:Fiber */
							/* 88E1112 PHY, Page Address 0x16 Reset to pageaddr */
							pageaddr = 1;
							hwWritePhyReg(pJwDev, hport_id, 0x16, pageaddr);							
							/* Enable auto-negotiation */
							if((ret = gprtPortAutoNegEnable(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
								printf("Error: FE gprtPortAutoNegEnable Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Restart auto-negotiation */
							if((ret = gprtPortRestartAutoNeg(pJwDev, hport_id)) != GT_OK) {
								printf("Error: FE gprtPortRestartAutoNeg Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							
							/* Set speed to Normal mode, not force speed */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, 3)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set Normal duplex detection occurs */					
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_AUTONEG:
						if((hport_id != 8) && (hport_id != 9) && (hport_id != 10)) {
							/* Enable auto-negotiation */
							if((ret = gprtPortAutoNegEnable(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
								printf("Error: FE gprtPortAutoNegEnable Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							/* Restart auto-negotiation */
							if((ret = gprtPortRestartAutoNeg(pJwDev, hport_id)) != GT_OK) {
								printf("Error: FE gprtPortRestartAutoNeg Fail! lport=%d\n",port_id);
								return SW_FAIL;
					        }
							
							/* Set speed to Normal mode, not force speed */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, 3)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set Normal duplex detection occurs */					
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }					
						}else if((hport_id == 8) || (hport_id == 10)){
							for(pageaddr=0; pageaddr<2; pageaddr++){
								/* Copper/Fiber Auto Operation, pageaddr 0:Copper, pageaddr 1:Fiber */
								/* 88E1112 PHY, Page Address 0x16 Reset to pageaddr */
								hwWritePhyReg(pJwDev, hport_id, 0x16, pageaddr);							
								/* Enable auto-negotiation */
								if((ret = gprtPortAutoNegEnable(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
									printf("Error: FE gprtPortAutoNegEnable Fail! lport=%d\n",port_id);
									return SW_FAIL;
						        }
								/* Restart auto-negotiation */
								if((ret = gprtPortRestartAutoNeg(pJwDev, hport_id)) != GT_OK) {
									printf("Error: FE gprtPortRestartAutoNeg Fail! lport=%d\n",port_id);
									return SW_FAIL;
						        }
								
								/* Set speed to Normal mode, not force speed */
								if((ret = gpcsSetForceSpeed(pJwDev, hport_id, 3)) != GT_OK) {
						            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
									return SW_FAIL;
						        }
								/* Set Normal duplex detection occurs */					
								if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
						            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
									return SW_FAIL;
						        }
							}
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S10_HALF:
						//if((hport_id != 8) && (hport_id != 9) && (hport_id != 10)) {
						if(hport_id != 9){
							if((hport_id == 8) || (hport_id == 10)){
								/* 88E1112 PHY, Page Address 0x16 Reset to 0, Copper */
								hwWritePhyReg(pJwDev, hport_id, 0x16, 0);
							}
							/* Disable auto-negotiation, 10M half-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0x8000)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 10M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_10_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to half-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S10_FULL:
						//if((hport_id != 8) && (hport_id != 9) && (hport_id != 10)) {
						if(hport_id != 9){
							if((hport_id == 8) || (hport_id == 10)){
								/* 88E1112 PHY, Page Address 0x16 Reset to 0, Copper */
								hwWritePhyReg(pJwDev, hport_id, 0x16, 0);
							}
							/* Disable auto-negotiation, 10M full-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0x8100)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 10M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_10_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to full-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S100_HALF:
						//if((hport_id != 8) && (hport_id != 9) && (hport_id != 10)) {	
						if(hport_id != 9){
							if((hport_id == 8) || (hport_id == 10)){
								/* 88E1112 PHY, Page Address 0x16 Reset to 0, Copper */
								hwWritePhyReg(pJwDev, hport_id, 0x16, 0);
							}
							/* Disable auto-negotiation, 100M half-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0xa000)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 100M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_100_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to half-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S100_FULL:
						//if((hport_id != 8) && (hport_id != 9) && (hport_id != 10)) {
						if(hport_id != 9){
							if((hport_id == 8) || (hport_id == 10)){
								/* 88E1112 PHY, Page Address 0x16 Reset to 0, Copper */
								hwWritePhyReg(pJwDev, hport_id, 0x16, 0);
							}
							/* Disable auto-negotiation, 100M full-duplex */
							if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0xa100)) != GT_OK) {
					            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set speed to 100M bps */
							if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_100_MBPS)) != GT_OK) {
					            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
							/* Set duplex to full-duplex */
							if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }						
							if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
								return SW_FAIL;
					        }
						}
					break;

					case FAL_PORT_PHYSICAL_MODE_S1000_FULL:
						if((hport_id == 8) || (hport_id == 10)){
							for(pageaddr=0; pageaddr<2; pageaddr++){
								/* Copper/Fiber Auto Operation, pageaddr 0:Copper, pageaddr 1:Fiber */
								/* 88E1112 PHY, Page Address 0x16 Reset to pageaddr */
								hwWritePhyReg(pJwDev, hport_id, 0x16, pageaddr);	
								/* Disable auto-negotiation, 1000M full-duplex */
								if((ret = gprtSetPagedPhyReg(pJwDev, hport_id, 0, 0, 0x8140)) != GT_OK) {
						            printf("Error: gprtSetPortSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
									return SW_FAIL;
						        }
								/* Set speed to 1000M bps */
								if((ret = gpcsSetForceSpeed(pJwDev, hport_id, PHY_SPEED_1000_MBPS)) != GT_OK) {
						            printf("Error: gpcsSetForceSpeed, hwport=%d, ret=%d\r\n", hport_id,ret);
									return SW_FAIL;
						        }
								/* Set duplex to full-duplex */
								if((ret = gpcsSetDpxValue(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
						            printf("Error: gpcsSetDpxValue, hwport=%d, ret=%d\r\n", hport_id,ret);
									return SW_FAIL;
						        }						
								if((ret = gpcsSetForcedDpx(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
						            printf("Error: gpcsSetForcedDpx, hwport=%d, ret=%d\r\n", hport_id,ret);
									return SW_FAIL;
						        }
							}
						}
					break;

					default:
					break;
				}
			break;

			default:
			break;
		}	
	}
    return rv;
}

static inline sw_error_t _mv88e6097_port_physical_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t *mode)
{

	printf("Not Support This Feature\n");
	return SW_OK;
}

static inline sw_error_t _mv88e6097_port_physical_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_status_t *status)
{
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;
	
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		GT_BOOL duplex_mode;
		GT_PORT_SPEED_MODE speed_mode;
		
	    if((ret = gprtGetDuplex(pJwDev, hport_id,&duplex_mode)) != GT_OK) {
			printf("Error: gprtGetDuplex Failed! lport=%d\r\n", port_id);
	        return SW_FAIL;
	    }

		if((ret = gprtGetSpeedMode(pJwDev,hport_id,&speed_mode)) != GT_OK) {
			printf("Error: gprtGetSpeedMode Failed! lport=%d\r\n", port_id);
			return SW_FAIL;
		}
		
		switch(speed_mode) {
			case PORT_SPEED_10_MBPS:
			*status = (duplex_mode == GT_TRUE)? FAL_PORT_PHYSICAL_STATUS_S10_FULL: FAL_PORT_PHYSICAL_STATUS_S10_HALF;
			break;

			case PORT_SPEED_100_MBPS:
			*status = (duplex_mode == GT_TRUE)? FAL_PORT_PHYSICAL_STATUS_S100_FULL: FAL_PORT_PHYSICAL_STATUS_S100_HALF;
			break;

			case PORT_SPEED_1000_MBPS:
			*status = (duplex_mode == GT_TRUE)? FAL_PORT_PHYSICAL_STATUS_S1000_FULL: FAL_PORT_PHYSICAL_STATUS_S1000_HALF;
			break;

			default:
			*status = FAL_PORT_PHYSICAL_STATUS_UNKOWN;
			break;
		}
	}	
    return rv;
}

static inline sw_error_t _mv88e6097_port_speed_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_speed_t *speed)
{
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;
	
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		GT_PORT_SPEED_MODE speed_mode;
		
		if((ret = gprtGetSpeedMode(pJwDev,hport_id,&speed_mode)) != GT_OK) {
			printf("Error: gprtGetSpeedMode Failed! lport=%d\r\n", port_id);
			return SW_FAIL;
		}

		switch(speed_mode) {
			case PORT_SPEED_1000_MBPS:
			*speed = FAL_PORT_SPEED_1000;
			break;

			case PORT_SPEED_100_MBPS:
			*speed = FAL_PORT_SPEED_100;
			break;

			default:
			*speed = FAL_PORT_SPEED_10;
			break;
		}
	}
    return rv;
}

static inline sw_error_t _mv88e6097_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_duplex_t *duplex)
{
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;
	
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		GT_BOOL duplex_mode;
		
	    if((ret = gprtGetDuplex(pJwDev, hport_id,&duplex_mode)) != GT_OK) {
			printf("Error: gprtGetDuplex Failed! lport=%d\r\n", port_id);
	        return SW_FAIL;
	    }

		if(duplex_mode == GT_TRUE)
			*duplex = FAL_PORT_DUPLEX_FULL;
		else
			*duplex = FAL_PORT_DUPLEX_HALF;
			
	}	
    return rv;
}

static inline sw_error_t _mv88e6097_port_flowctrl_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable)
{
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;

		if((hport_id == 10) || (hport_id == 8)) {
			if(enable == FAL_ENABLE) {
				if((ret = gpcsSetFCValue(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					printf("Error: SPF gpcsSetFCValue Fail! lport=%d\n",port_id);
					return SW_FAIL;
			    }
				if((ret = gpcsSetForcedFC(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					printf("Error: SPF gpcsSetForcedFC Fail! lport=%d\n",port_id);
					return SW_FAIL;
			    }		
			} else {
				if((ret = gpcsSetForcedFC(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					printf("Error: SPF gpcsSetForcedFC Fail! lport=%d\n",port_id);
					return SW_FAIL;
			    }
			}
		} else {
			if(enable == FAL_ENABLE) {
				/* Program Phy's Pause bit in AutoNegotiation Advertisement Register. */
				if((ret = gprtSetPause(pJwDev, hport_id, GT_PHY_PAUSE)) != GT_OK) {
					printf("Error: FE gprtSetPause Fail! lport=%d\n",port_id);
					return SW_FAIL;
			    }
				/* Program Port's Flow Control. */
				if((ret = gprtSetForceFc(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
					printf("Error: FE gprtSetForceFc Fail! lport=%d\n",port_id);
					return SW_FAIL;
			    }		
			} else {
				/* Program Phy's Pause bit in AutoNegotiation Advertisement Register. */
				if((ret = gprtSetPause(pJwDev, hport_id, GT_PHY_NO_PAUSE)) != GT_OK) {
					printf("Error: FE gprtSetPause Fail! lport=%d\n",port_id);
					return SW_FAIL;
			    }
				/* Program Port's Flow Control. */
				if((ret = gprtSetForceFc(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					printf("Error: FE gprtSetForceFc Fail! lport=%d\n",port_id);
					return SW_FAIL;
			    }
			}
		}
	}
    return rv;
}

static inline sw_error_t _mv88e6097_port_flowctrl_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable)
{
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		GT_BOOL forcefc, fcvalue;
		
		if((hport_id == 10) || (hport_id == 8)) {
			if((ret = gpcsGetFCValue(pJwDev, hport_id, &fcvalue)) != GT_OK) {
				printf("Error: gpcsGetFCValue Failed! lport=%d\r\n", port_id);
		        return SW_FAIL;
		    }
			if((ret = gpcsGetForcedFC(pJwDev, hport_id, &forcefc)) != GT_OK) {
				printf("Error: gpcsGetFCValue Failed! lport=%d\r\n", port_id);
		        return SW_FAIL;
		    }
			if((forcefc == GT_TRUE) && (fcvalue == GT_TRUE)) {
				*enable = FAL_ENABLE;
			} else {
				*enable = FAL_DISABLE;
			}
		} else {
			if((ret = gprtGetForceFc(pJwDev, hport_id, &forcefc)) != GT_OK) {
				printf("Error: gprtGetForceFc Failed! lport=%d\r\n", port_id);
				return SW_FAIL;
		    }
			if(forcefc == GT_TRUE)
				*enable = FAL_ENABLE;
			else
				*enable = FAL_DISABLE;
		}
	}	
    return rv;
}

static inline sw_error_t _mv88e6097_port_dot1d_state_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t state)
{
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		
		GT_STATUS ret;
		GT_PORT_STP_STATE stpState;

		switch(state) {
			case FAL_PORT_DOT1D_STATE_DISABLED:
			stpState = GT_PORT_DISABLE;
			break;

			case FAL_PORT_DOT1D_STATE_BLOCKING:
			stpState = GT_PORT_BLOCKING;
			break;

			case FAL_PORT_DOT1D_STATE_LEARNING:
			stpState = GT_PORT_LEARNING;
			break;

			case FAL_PORT_DOT1D_STATE_FORWARDING:
			stpState = GT_PORT_FORWARDING;
			break;

			default:
			return SW_FAIL;
		}

	    if((ret = gstpSetPortState(pJwDev, hport_id, stpState)) != GT_OK) {
			printf("Error: gstpSetPortState Fail! hport=%d\n",hport_id);
	        return SW_FAIL;
	    }
	}

    return rv;
}

static inline sw_error_t _mv88e6097_port_dot1d_state_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t *state)
{
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		
		GT_STATUS ret;
		GT_PORT_STP_STATE stpState;

	    if((ret = gstpGetPortState(pJwDev, hport_id, &stpState)) != GT_OK) {
			printf("Error: gstpGetPortState Fail! hport=%d\n",hport_id);
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
			break;
		}
	}
	
    return rv;
}

static inline sw_error_t _mv88e6097_port_pcs_status_get(a_uint32_t dev_id,fal_port_t port_id,a_uint32_t *pcs_status)
{
	a_uint32_t hport;
	GT_STATUS ret;
	GT_BOOL PCSLink, SyncOk, SyncFail;

	if(port_id > 2) {
		printf("PSC Control Register only support 1000BASE-X ports\n");
		return SW_FAIL;
	}

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport));

	if((ret = gpcsGetPCSLink(pJwDev, hport, &PCSLink)) != GT_OK) {
		printf("Error: gpcsGetPCSLink Fail! hport=%d\n",hport);
	    return SW_FAIL;
	}

	if((ret = gpcsGetSyncOK(pJwDev, hport, &SyncOk)) != GT_OK) {
		printf("Error: gpcsGetSyncOK Fail! hport=%d\n",hport);
	    return SW_FAIL;
	}

	if((ret = gpcsGetSyncFail(pJwDev, hport, &SyncFail)) != GT_OK) {
		printf("Error: gpcsGetSyncFail Fail! hport=%d\n",hport);
	    return SW_FAIL;
	}
	 
	
	*pcs_status= ((PCSLink<<15) | (SyncOk<<14) | SyncFail<<13);

	//printf("PCS pcs_status = 0x%02x, PCSLink = %d, SyncOk = %d, SyncFail = %d\n",*pcs_status, PCSLink, SyncOk, SyncFail);

	return SW_OK;
}

static inline sw_error_t _mv88e6097_port_comb_type_set(a_uint32_t dev_id,fal_port_t port_id,fal_port_comb_type_t comb_type)
{	
	a_uint32_t hport_id;
	GT_U16 data;
	fal_port_type_t type_info=0;
	fal_port_comb_t comb_port=0;
	sw_error_t ret;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));

	ret = _mv88e6097_port_type_get(pJwDev, port_id, &type_info, &comb_port);
	if(ret != SW_OK) {
		printf("_mv88e6097_port_type_get failed,ret = %d\n",ret);
		return SW_FAIL;
	}

	/* Only Comb Port can Use This Feature */
	if(comb_port != FAL_PORT_COMB_PORT) {
		printf("Only Comb Port Use!\n");
		return SW_FAIL;
	}	

	/* Set PHY Page Address */
	if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_PAGE_ADDRESS,0x2)) != GT_OK) {
		printf("Error: gprtSetPhyReg MARVELL1112PHY_PAGE_ADDRESS Fail! lport=%d\n",port_id);
	    return SW_FAIL;
	}

	switch(comb_type) {
		case FAL_PORT_COMB_FORCE_1000BASE_X:
			/* Mode Select 1000Base-X */
			if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_MODE_SELECT,0x388)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_MODE_SELECT Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			/* PHY Reset */
			if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_PAGE_ADDRESS,0x1)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_PAGE_ADDRESS Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_CONTROL_REGISTER,0x9140)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_CONTROL_REGISTER Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			/* Force Link-up, 1000M, Full-duplex */
			if((ret = hwReadPortReg(pJwDev,hport_id,QD_REG_PCS_CONTROL,&data)) != GT_OK) {
				printf("Error: hwReadPortReg QD_REG_PCS_CONTROL Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			data &= 0xFFC0;
			//data |= 0x3e;	
			data |= 0x3;
			if((ret = hwWritePortReg(pJwDev,hport_id,QD_REG_PCS_CONTROL,data)) != GT_OK) {
				printf("Error: hwWritePortReg QD_REG_PCS_CONTROL Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
		break;

		case FAL_PORT_COMB_FORCE_100BASE_FX:
			/* Mode Select 100Base-Fx */
			if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_MODE_SELECT,0x8)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_MODE_SELECT Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			/* PHY Reset */
			if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_PAGE_ADDRESS,0x1)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_PAGE_ADDRESS Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_CONTROL_REGISTER,0x9140)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_CONTROL_REGISTER Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			/* Force Link-up, 100M, Full-duplex */
			if((ret = hwReadPortReg(pJwDev,hport_id,QD_REG_PCS_CONTROL,&data)) != GT_OK) {
				printf("Error: hwReadPortReg QD_REG_PCS_CONTROL Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			data &= 0xFFC0;
			data |= 0x3d;
			if((ret = hwWritePortReg(pJwDev,hport_id,QD_REG_PCS_CONTROL,data)) != GT_OK) {
				printf("Error: hwWritePortReg QD_REG_PCS_CONTROL Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
		break;

		case FAL_PORT_COMB_FORCE_COPPER:
			/* Mode Select Copper */
			if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_MODE_SELECT,0x288)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_MODE_SELECT Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			/* PHY Reset */
			if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_PAGE_ADDRESS,0x0)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_PAGE_ADDRESS Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_CONTROL_REGISTER,0x9140)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_CONTROL_REGISTER Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			/* Not Force Link-up, Force duplex, Normal Operation */
			if((ret = hwReadPortReg(pJwDev,hport_id,QD_REG_PCS_CONTROL,&data)) != GT_OK) {
				printf("Error: hwReadPortReg QD_REG_PCS_CONTROL Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			data &= 0xFFC0;
			data |= 0x3;
			if((ret = hwWritePortReg(pJwDev,hport_id,QD_REG_PCS_CONTROL,data)) != GT_OK) {
				printf("Error: hwWritePortReg QD_REG_PCS_CONTROL Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}	
		break;

		case FAL_PORT_COMB_AUTO_FIBER_COPPER:
			/* Mode Select Auto Fiber/Copper, Preferred Fiber Medium*/
			if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_MODE_SELECT,0x588)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_MODE_SELECT Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			/* PHY Reset */
			if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_PAGE_ADDRESS,0x1)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_PAGE_ADDRESS Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_CONTROL_REGISTER,0x9140)) != GT_OK) {
				printf("Error: gprtSetPhyReg MARVELL1112PHY_CONTROL_REGISTER Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			/* Not Force Link-up, Force duplex, Normal Operation */
			if((ret = hwReadPortReg(pJwDev,hport_id,QD_REG_PCS_CONTROL,&data)) != GT_OK) {
				printf("Error: hwReadPortReg QD_REG_PCS_CONTROL Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}
			data &= 0xFFC0;
			data |= 0x3;
			if((ret = hwWritePortReg(pJwDev,hport_id,QD_REG_PCS_CONTROL,data)) != GT_OK) {
				printf("Error: hwWritePortReg QD_REG_PCS_CONTROL Fail! lport=%d\n",port_id);
			    return SW_FAIL;
			}	
		break;

		default:
		break;
	}

	return SW_OK;
}

static inline sw_error_t _mv88e6097_port_comb_type_get(a_uint32_t dev_id,fal_port_t port_id,fal_port_comb_type_t *comb_type)
{
	sw_error_t rv = SW_OK;
	a_uint32_t hport_id;	  

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		GT_U16 media_mode;

		if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_PAGE_ADDRESS,0x2)) != GT_OK) {
			printf("Error: gprtSetPhyReg MARVELL1112PHY_PAGE_ADDRESS Fail! lport=%d\n",port_id);
		    return SW_FAIL;
		}
		if((ret = gprtGetPhyReg(pJwDev,hport_id,MARVELL1112PHY_MODE_SELECT,&media_mode)) != GT_OK) {
			printf("Error: gprtSetPhyReg MARVELL1112PHY_MODE_SELECT Fail! lport=%d\n",port_id);
		    return SW_FAIL;
		}

		/* PHY addr only can be 0~1, otherwise Fiber cant be detect */
		if((ret = gprtSetPhyReg(pJwDev,hport_id,MARVELL1112PHY_PAGE_ADDRESS,0x1)) != GT_OK) {
			printf("Error: gprtSetPhyReg MARVELL1112PHY_PAGE_ADDRESS Fail! lport=%d\n",port_id);
		    return SW_FAIL;
		}

		/* 88E1112 PHY Mode Select, page 117 */
		media_mode = ((media_mode & 0x0380) >> 7);

		if(media_mode == FORCE_1000BASEX_MODE) {
			*comb_type = FAL_PORT_COMB_FORCE_1000BASE_X;
		}
		else if(media_mode == FORCE_100BASEFX_MODE) {
			*comb_type = FAL_PORT_COMB_FORCE_100BASE_FX;
		}
		else if(media_mode == FORCE_COPPER_MODE) {
			*comb_type = FAL_PORT_COMB_FORCE_COPPER;
		}
		else if(media_mode == AUTO_1000BASEX_COPPER_MODE) {
			*comb_type = FAL_PORT_COMB_AUTO_FIBER_COPPER;
		}	
	}

	return rv;
}

sw_error_t comb_port_fiber_link_status_detect(fal_port_t lport,unsigned int *media_status)
{
	a_uint32_t hport;
	GT_STATUS ret;
	GT_U16 link_status;
	
	SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));

	if((ret = gprtSetPhyReg(pJwDev,hport,MARVELL1112PHY_PAGE_ADDRESS,0x1)) != GT_OK) {
		printf("Error: gprtSetPhyReg MARVELL1112PHY_PAGE_ADDRESS Fail! lport=%d\n",lport);
	    return SW_FAIL;
	}
	if((ret = gprtGetPhyReg(pJwDev,hport,MARVELL1112PHY_LINK_STATUS,&link_status)) != GT_OK) {
		printf("Error: gprtSetPhyReg MARVELL1112PHY_LINK_STATUS Fail! lport=%d\n",lport);
		return SW_FAIL;
	}

	*media_status = link_status;

	return SW_OK;
}

sw_error_t comb_port_led_mode_select(fal_port_t lport,unsigned int media_link)
{
	a_uint32_t hport;
	GT_STATUS ret;

	SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));

	if((ret = gprtSetPhyReg(pJwDev,hport,MARVELL1112PHY_PAGE_ADDRESS,0x3)) != GT_OK) {
		printf("Error: gprtSetPhyReg MARVELL1112PHY_PAGE_ADDRESS Fail! lport=%d\n",lport);
	    return SW_FAIL;
	}

	/* 1:Febir link, 0:Copper link */
	if(media_link == 1){
		if((ret = gprtSetPhyReg(pJwDev,hport,MARVELL1112PHY_MODE_SELECT,FIBER_LED_MODE)) != GT_OK) {
			printf("Error: gprtSetPhyReg MARVELL1112PHY_MODE_SELECT Fail! lport=%d\n",lport);
		    return SW_FAIL;
		}
	} else if(media_link == 0){
		if((ret = gprtSetPhyReg(pJwDev,hport,MARVELL1112PHY_MODE_SELECT,COPPER_LED_MODE)) != GT_OK) {
			printf("Error: gprtSetPhyReg MARVELL1112PHY_MODE_SELECT Fail! lport=%d\n",lport);
		    return SW_FAIL;
		}
	}

	/* PHY addr only can be 0~1, otherwise Fiber cant be detect */
	if((ret = gprtSetPhyReg(pJwDev,hport,MARVELL1112PHY_PAGE_ADDRESS,0x1)) != GT_OK) {
		printf("Error: gprtSetPhyReg MARVELL1112PHY_PAGE_ADDRESS Fail! lport=%d\n",lport);
	    return SW_FAIL;
	} 
	return SW_OK;
}



sw_error_t mv88e6097_port_type_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_type_t *type_info, fal_port_comb_t *comb_port)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_type_get(dev_id, port_id, type_info, comb_port);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_link_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_link_status_t *status)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_link_status_get(dev_id, port_id, status);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_admin_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_admin_mode_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_admin_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_admin_mode_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_physical_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_physical_mode_set(dev_id, port_id, mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_physical_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_mode_t *mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_physical_mode_get(dev_id, port_id, mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_physical_status_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_physical_status_t *status)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_physical_status_get(dev_id, port_id, status);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_speed_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_speed_t *speed)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_speed_get(dev_id, port_id, speed);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_duplex_t *duplex)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_duplex_get(dev_id, port_id, duplex);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_flowctrl_mode_set(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_flowctrl_mode_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_flowctrl_mode_get(a_uint32_t dev_id, fal_port_t port_id, fal_enable_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_flowctrl_mode_get(dev_id, port_id, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_dot1d_state_set(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t state)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_dot1d_state_set(dev_id, port_id, state);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_dot1d_state_get(a_uint32_t dev_id, fal_port_t port_id, fal_port_dot1d_state_t *state)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_dot1d_state_get(dev_id, port_id, state);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_pcs_status_get(a_uint32_t dev_id,fal_port_t port_id,a_uint32_t *pcs_status)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_pcs_status_get(dev_id, port_id, pcs_status);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_comb_type_set(a_uint32_t dev_id,fal_port_t port_id,fal_port_comb_type_t comb_type)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_comb_type_set(dev_id, port_id, comb_type);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_comb_type_get(a_uint32_t dev_id,fal_port_t port_id,fal_port_comb_type_t *comb_type)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_comb_type_get(dev_id, port_id, comb_type);
    HSL_API_UNLOCK;
	
    return rv;
}


sw_error_t mv88e6097_port_test_show_all(a_uint32_t dev_id)
{
	hsl_dev_t *pdev = NULL;
	a_uint32_t lport;
	fal_port_type_t lport_type;
	fal_port_link_status_t link_status;
	fal_enable_t admin_status;
	fal_port_physical_status_t physical_status;
	fal_enable_t flowctrl_mode;
	fal_port_dot1d_state_t dot1dState;
	fal_port_comb_t comb_port;
	
	sal_sys_model_t model;
	
	printf("\n");
	printf("  Port Status Information Display :  %s\n",CONFIG_JWS_HARDWARE_VERSION);
	printf("  ===========================================================================\n");
	printf("  Port  Type    AdminMode  LinkState   Speed-Duplex  FlowControl  Dot1dState \n");
	printf("  ===========================================================================\n");
	
    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;

	SW_RTN_ON_ERROR(sal_sys_model_get(&model));
	switch(model.model_id) {
		case GE_Z20208D_2008:
		case GE_Z20208D_1008:
		case GE_Z20208D_2004:
		case GE_Z20208D_1004:
		case GE_Z20505D_1414:
		case GE_Z20204A_2013:
		case GE_Z20204A_1013:
		case GE_Z20204A_2017:
		case GE_Z20204A_1017:
		case GE_Z20210D_2028:
		case GE_Z20210D_2024:
		case GE_Z20208D_2008_P:
		case GE_Z20208D_2004_P:
		case GE_SDI1000_0014_T:
		case GE_SDI1000_0014_R:
			for(lport=0; lport<CONFIG_JWS_PORT_TOTAL_NR+1; lport++) {
				SW_RTN_ON_ERROR(mv88e6097_port_type_get(dev_id, lport, &lport_type, &comb_port));
				SW_RTN_ON_ERROR(mv88e6097_port_link_status_get(dev_id, lport, &link_status));
				SW_RTN_ON_ERROR(mv88e6097_port_admin_mode_get(dev_id, lport, &admin_status));
				SW_RTN_ON_ERROR(mv88e6097_port_physical_status_get(dev_id, lport, &physical_status));
				SW_RTN_ON_ERROR(mv88e6097_port_flowctrl_mode_get(dev_id, lport, &flowctrl_mode));
				SW_RTN_ON_ERROR(mv88e6097_port_dot1d_state_get(dev_id, lport, &dot1dState));
				
				printf("   %02d    %-5s   %-9s    %-6s  %13s    %-9s  %-10s\r\n", lport, 	
					(lport_type == FAL_PORT_TYPE_FE)? "FE" : \
					(lport_type == FAL_PORT_TYPE_FX)? "FX" : \
					(lport_type == FAL_PORT_TYPE_GE)? "GE" : \
					(lport_type == FAL_PORT_TYPE_GX)? "GX" : \
					(lport_type == FAL_PORT_TYPE_CPU)? "CPU" : \
					(lport_type == FAL_PORT_TYPE_COMB)? "COMB" :"UN",
					(admin_status == FAL_ENABLE)? "Enabled" : "Disabled",
					(link_status == FAL_PORT_LINK_STATUS_UP)? "Up" : "Down",
					(link_status == FAL_PORT_LINK_STATUS_DOWN)? "--     " : \
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S10_HALF)? "10Mbps-Half" : 
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S10_FULL)? "10Mbps-Full" : 
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S100_HALF)? "100Mbps-Half" : 
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S100_FULL)? "100Mbps-Full" :
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S1000_HALF)? "1000Mbps-Half" :
					(physical_status == FAL_PORT_PHYSICAL_STATUS_S1000_FULL)? "1000Mbps-Full" : "Unkown",
					(flowctrl_mode == FAL_ENABLE)? "Enabled" : "Disabled",
					(dot1dState == FAL_PORT_DOT1D_STATE_DISABLED)? "Disabled" : 
					(dot1dState == FAL_PORT_DOT1D_STATE_BLOCKING)? "Blocking" : 
					(dot1dState == FAL_PORT_DOT1D_STATE_LEARNING)? "Learning" : 
					(dot1dState == FAL_PORT_DOT1D_STATE_FORWARDING)? "Forwarding" : "Unkown");	
			}
			printf("\n");
		break;

		default:
		break;
	}

    return SW_OK;
}


sw_error_t mv88e6097_port_ctrl_init(a_uint32_t dev_id)
{
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;
    sw_error_t rv = SW_OK;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
	
    p_api->port_type_get= mv88e6097_port_type_get;
    p_api->port_link_status_get = mv88e6097_port_link_status_get;
    p_api->port_admin_mode_set = mv88e6097_port_admin_mode_set;
    p_api->port_admin_mode_get = mv88e6097_port_admin_mode_get;	
    p_api->port_physical_mode_set = mv88e6097_port_physical_mode_set;
    p_api->port_physical_mode_get = mv88e6097_port_physical_mode_get;
    p_api->port_physical_status_get= mv88e6097_port_physical_status_get;	
	p_api->port_speed_get= mv88e6097_port_speed_get;	
	p_api->port_duplex_get= mv88e6097_port_duplex_get;		
    p_api->port_flowctrl_mode_set = mv88e6097_port_flowctrl_mode_set;
    p_api->port_flowctrl_mode_get = mv88e6097_port_flowctrl_mode_get;
    p_api->port_dot1d_state_set = mv88e6097_port_dot1d_state_set;
    p_api->port_dot1d_state_get = mv88e6097_port_dot1d_state_get;	
    p_api->port_test_show_all = mv88e6097_port_test_show_all;
	p_api->port_pcs_status_get = mv88e6097_port_pcs_status_get;
	p_api->port_comb_type_set = mv88e6097_port_comb_type_set;
	p_api->port_comb_type_get = mv88e6097_port_comb_type_get;
	
    return rv;
}


