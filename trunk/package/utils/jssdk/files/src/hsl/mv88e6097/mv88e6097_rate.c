 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv88e6097_rate.h"
#include "mv88e6097_port_ctrl.h"
#include "mv88e6097_reg.h"
#include "msApi.h"
#include "gtHwCntl.h"
#include "gtDrvSwRegs.h"

#include "joyware.h"



extern GT_QD_DEV *pJwDev;

static a_uint32_t EgrlEnableFlag[CONFIG_JWS_PORT_TOTAL_NR+1] = {0};
static a_uint32_t PirlEnableFlag[CONFIG_JWS_PORT_TOTAL_NR+1] = {0};
static a_uint32_t PirlBucketFlag[MV88E6097_MAX_BUCKET_NUM] = {0};

static a_uint32_t bktTypeMask[CONFIG_JWS_PORT_TOTAL_NR+1][MV88E6097_MAX_BUCKET_NUM];
static a_uint32_t TypeMask[CONFIG_JWS_PORT_TOTAL_NR+1];
static a_uint32_t Mask[CONFIG_JWS_PORT_TOTAL_NR+1][MV88E6097_MAX_BUCKET_NUM];



static inline sw_error_t _mv88e6097_rate_bucket_egrl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t speed,fal_enable_t enable)
{
    sw_error_t rv = SW_OK;
	GT_ERATE_TYPE egress_rate;
	a_uint32_t kbRate, hport, mode;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport));
	{
		GT_STATUS ret;

		if(enable == FAL_ENABLE) {

			EgrlEnableFlag[hport] = 1;
			
			if(speed < 1000)   		 /* less than to 1Mbps */
	        {
	        	kbRate = ((a_uint32_t)(speed / 64))*64;
	        }
	        else if(speed <= 100000)     /* less than or equal to 100Mbps */
	        {
	            kbRate = ((a_uint32_t)(speed / 1000))*1000;
	        }
	        else if(speed <= 1000000)    /* less than or equal to 1000Mbps */
	        {
	            kbRate = ((a_uint32_t)(speed / 10000))*10000;
	        }else {
				printf("Limit Rate > 1000000\n");
				return SW_BAD_PARAM;
			}
			
			
			egress_rate.kbRate = kbRate;

			grcGetELimitMode(pJwDev, hport, &mode);

			printf("mode = %d , kbRate = %d\n",mode,kbRate);

			if(kbRate != 0) { /* 0 :not limit egress rate */
				if((ret = grcSetEgressRate(pJwDev, hport, (GT_ERATE_TYPE *)&egress_rate)) != GT_OK) {
					return SW_FAIL;
		        }
			}
			printf("set egrl ok\n");
		}else{
			EgrlEnableFlag[hport] = 0;
			kbRate = 0;         /* 0 :not limit egress rate */
			egress_rate.kbRate = kbRate;
			if((ret = grcSetEgressRate(pJwDev, hport, (GT_ERATE_TYPE *)&egress_rate)) != GT_OK) {
					return SW_FAIL;
		    }
			printf("no limit\n");
		}

	}
	
	
    return rv;

}

static inline sw_error_t _mv88e6097_rate_bucket_egrl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t * speed,fal_enable_t * enable)
{
    sw_error_t rv = SW_OK;
	GT_ERATE_TYPE egress_rate;
	a_uint32_t hport;
	
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport));
	{
		GT_STATUS ret;

		if(EgrlEnableFlag[hport] == 1) {
			printf("get ok\n");
			*enable = FAL_ENABLE;
			if((ret = grcGetEgressRate(pJwDev, hport, (GT_ERATE_TYPE *)&egress_rate)) != GT_OK) {
				return SW_FAIL;
	        }
			*speed = egress_rate.kbRate;
		} else {
			*enable = FAL_DISABLE;
			if((ret = grcGetEgressRate(pJwDev, hport, (GT_ERATE_TYPE *)&egress_rate)) != GT_OK) {
				return SW_FAIL;
	        }
			*speed = egress_rate.kbRate;
		}
	}
	
    return rv;

}

static inline sw_error_t _mv88e6097_rate_bucket_pirl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t type_mask,fal_rate_speed_t speed,fal_enable_t enable)
{
	a_uint32_t hport;
	GT_STATUS ret;
    GT_PIRL2_DATA pirlData;
    GT_U32 irlRes, Rate;
	a_uint32_t buf;
   // GT_LPORT port;

	if(speed < 1000)			
	{
	   Rate = ((a_uint32_t)(speed / 64))*64;
	}
	else if(speed <= 100000) 	
	{
	   Rate = ((a_uint32_t)(speed / 1000))*1000;
	}
	else if(speed <= 1000000)	
	{
	   Rate = ((a_uint32_t)(speed / 10000))*10000;
	}else {
		printf("Limit Rate > 1000000\n");
		return SW_BAD_PARAM;
	}

	if(bucket_id > 4) {
		printf("bucket_id belong to 0~4 \n");
		return SW_BAD_PARAM;
	}

	irlRes = bucket_id;

	pirlData.ingressRate = Rate;
	pirlData.customSetup.isValid = GT_FALSE;

    pirlData.accountQConf       = GT_FALSE;
    pirlData.accountFiltered    = GT_TRUE;

    pirlData.mgmtNrlEn = GT_TRUE;
    pirlData.saNrlEn   = GT_TRUE;
    pirlData.daNrlEn   = GT_FALSE;
    pirlData.samplingMode = GT_FALSE;
    pirlData.actionMode = PIRL_ACTION_USE_LIMIT_ACTION;

    pirlData.ebsLimitAction        = ESB_LIMIT_ACTION_DROP;
    pirlData.bktRateType        = BUCKET_TYPE_TRAFFIC_BASED;
	
  /*  pirlData.bktTypeMask        = BUCKET_TRAFFIC_BROADCAST |
                                  BUCKET_TRAFFIC_MULTICAST |
                                  BUCKET_TRAFFIC_UNICAST   |
                                  BUCKET_TRAFFIC_MGMT_FRAME|
                                  BUCKET_TRAFFIC_ARP; */

    pirlData.priORpt = GT_TRUE;
    pirlData.priMask = 0;

    pirlData.byteTobeCounted    = GT_PIRL2_COUNT_ALL_LAYER2;

	//memset(PirlEnableFlag,0,CONFIG_JWS_PORT_TOTAL_NR+1);	
	//memset(PirlBucketFlag,0,MV88E6097_MAX_BUCKET_NUM);
	
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport));
	{		

#if 1
		if(enable == FAL_ENABLE) {
			TypeMask[hport] = bktTypeMask[hport][0] | bktTypeMask[hport][1] | bktTypeMask[hport][2] | bktTypeMask[hport][3] | bktTypeMask[hport][4];			
			TypeMask[hport] &= ~bktTypeMask[hport][bucket_id];
			Mask[hport][bucket_id] = type_mask;

			if((TypeMask[hport] & Mask[hport][bucket_id]) != 0) {/*compile input mask with previous mask*/
				printf("TypeMask input error, Must Be Unique!\n");
				return SW_BAD_PARAM;			
			}else {
				bktTypeMask[hport][bucket_id] = type_mask;		/*add mask to same bucket*/
				pirlData.bktTypeMask = bktTypeMask[hport][bucket_id];
			}
			
			PirlEnableFlag[hport] = 1;
			PirlBucketFlag[bucket_id] = 1;
		
			if((ret = gpirl2WriteResource(pJwDev,hport,irlRes,(GT_PIRL2_DATA *)&pirlData)) != GT_OK)
			{
				return SW_FAIL;
			}
		} else {
			PirlBucketFlag[bucket_id] = 0;

			/* clear bucket TypeMask */			
			buf = (~bktTypeMask[hport][bucket_id]);
			bktTypeMask[hport][bucket_id] &= buf;
			TypeMask[hport] &= (~bktTypeMask[hport][bucket_id]);
			
			if((ret = gpirl2DisableResource(pJwDev,hport,irlRes)) != GT_OK)
			{
				return SW_FAIL;
			}
			printf("pirl disable \n");
		}
#endif
	}
	
	return SW_OK;
}

static inline sw_error_t _mv88e6097_rate_bucket_pirl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t * type_mask,fal_rate_speed_t * speed,fal_enable_t * enable)
{
	a_uint32_t hport;
	GT_STATUS ret;
    GT_PIRL2_DATA pirlData;
    GT_U32 irlRes;

	irlRes = bucket_id;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport));
	{
		if((PirlEnableFlag[hport] == 1)) {
			if(PirlBucketFlag[bucket_id] == 1) {	
				
				if((ret = gpirl2ReadResource(pJwDev,hport,irlRes,(GT_PIRL2_DATA *)&pirlData)) != GT_OK) {
					return SW_FAIL;
		        }
				
				*enable = FAL_ENABLE;
				*speed = pirlData.ingressRate;
				printf("speed = %d \n",*speed);
				*type_mask = pirlData.bktTypeMask;
				printf("type mask = %d \n",(int)pirlData.bktTypeMask);
			} else {
				if((ret = gpirl2ReadResource(pJwDev,hport,irlRes,(GT_PIRL2_DATA *)&pirlData)) != GT_OK) {
					return SW_FAIL;
		        }
				*enable = FAL_DISABLE;
				*speed = pirlData.ingressRate;
				*type_mask = pirlData.bktTypeMask;
			}
		} else {
			if((ret = gpirl2ReadResource(pJwDev,hport,irlRes,(GT_PIRL2_DATA *)&pirlData)) != GT_OK) {
					return SW_FAIL;
	        }
			*enable = FAL_DISABLE;
			*speed = pirlData.ingressRate;
			*type_mask = pirlData.bktTypeMask;
		}
	}
	
	return SW_OK;
}




sw_error_t mv88e6097_rate_bucket_egrl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t speed,fal_enable_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_rate_bucket_egrl_set(dev_id, port_id, speed, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_rate_bucket_egrl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t *speed,fal_enable_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_rate_bucket_egrl_get(dev_id, port_id, speed, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_rate_bucket_pirl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t type_mask,fal_rate_speed_t speed,fal_enable_t enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_rate_bucket_pirl_set(dev_id, port_id, bucket_id, type_mask, speed, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_rate_bucket_pirl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t * type_mask,fal_rate_speed_t *speed,fal_enable_t *enable)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_rate_bucket_pirl_get(dev_id, port_id, bucket_id, type_mask, speed, enable);
    HSL_API_UNLOCK;
	
    return rv;
}







sw_error_t mv88e6097_rate_limit_init(a_uint32_t dev_id)
{
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;
	

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);
	
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    p_api->rate_bucket_egrl_set = mv88e6097_rate_bucket_egrl_set;
	p_api->rate_bucket_egrl_get = mv88e6097_rate_bucket_egrl_get;
	p_api->rate_bucket_pirl_set = mv88e6097_rate_bucket_pirl_set;
	p_api->rate_bucket_pirl_get = mv88e6097_rate_bucket_pirl_get;

	return SW_OK;
}


