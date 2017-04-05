 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv3236_global.h"
#include "mv3236_rate.h"
#include "mv3236_port_ctrl.h"
#include "mv3236_reg.h"
#include "cpss/generic/bridge/cpssGenBrgGen.h"
#include "cpss/dxCh/dxChxGen/bridge/cpssDxChBrgGen.h"
#include "cpss/generic/config/private/prvCpssConfigTypes.h"
//#include "msApi.h"
//#include "gtHwCntl.h"
//#include "gtDrvSwRegs.h"


//extern GT_QD_DEV *pJwDev;

static a_uint32_t EgrlEnableFlag[MV3236_MAX_PORT_NUM] = {0};
static a_uint32_t PirlEnableFlag[MV3236_MAX_PORT_NUM] = {0};
static a_uint32_t PirlBucketFlag[MV3236_MAX_BUCKET_NUM] = {0};

static a_uint32_t bktTypeMask[MV3236_MAX_PORT_NUM][MV3236_MAX_BUCKET_NUM];
static a_uint32_t TypeMask[MV3236_MAX_PORT_NUM];
static a_uint32_t Mask[MV3236_MAX_PORT_NUM][MV3236_MAX_BUCKET_NUM];

#define WINDOWSIZE 1024

static inline sw_error_t _mv3236_rate_bucket_egrl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t speed,fal_enable_t enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t kbRate, hport, mode;
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport));
	{
		GT_STATUS ret;
	    GT_U16    burstSize ;/*FIXME. no parameters for this value*/
		GT_U32    maxRatePtr ;
		GT_BOOL   enable;
		if(enable == FAL_ENABLE) {
		    burstSize = 9600;/*FIXME. no parameters for this value*/
		    maxRatePtr = speed;
		    enable = GT_TRUE;
		}
		else
		{
			burstSize = 0;
			maxRatePtr = 0;
			enable = GT_FALSE;
		}

		if((ret = cpssDxChPortTxShaperProfileSet(dev_id, hport, burstSize, &maxRatePtr)) != GT_OK) {
			printf("cpssDxChPortTxShaperProfileSet failed! ret is %d \n", ret);
			goto FAIL;
		}

		if((ret = cpssDxChPortTxShaperEnableSet(dev_id, hport, enable)) != GT_OK) {
			printf("cpssDxChPortTxShaperProfileSet failed! ret is %d \n", ret);
			goto FAIL;
		}
	}
	
    return SW_OK;
FAIL:
	printf("TODO.%s.%d retstore action need here failed!\n",__FILE__,__LINE__);
	return SW_FAIL;
}

static inline sw_error_t _mv3236_rate_bucket_egrl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t * speed,fal_enable_t * enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
#if 0
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
#endif
    return SW_OK;
}

static inline sw_error_t _mv3236_rate_bucket_pirl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t type_mask,fal_rate_speed_t speed,fal_enable_t enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	a_uint32_t hport;
	GT_STATUS ret;
    GT_U32 irlRes, Rate;
	CPSS_DXCH_BRG_GEN_RATE_LIMIT_PORT_STC   portGfgPtr;
	CPSS_DXCH_BRG_GEN_RATE_LIMIT_STC 		brgRateLimitPtr;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport));
	{		
		if((ret = cpssDxChBrgGenPortRateLimitGet(dev_id, hport, &portGfgPtr)) != GT_OK)
		{
			printf("cpssDxChBrgGenPortRateLimitGet failed!\n");
			goto FAIL;
		}

		if(enable == FAL_ENABLE) {
			portGfgPtr.enableBc = !!(type_mask&0x1);
			portGfgPtr.enableMc = !!(type_mask&0x2);
			portGfgPtr.enableMcReg = !!(type_mask&0x4);
			portGfgPtr.enableUcUnk = !!(type_mask&0x8);
			portGfgPtr.enableUcKnown = !!(type_mask&0x10);

			if((ret = cpssDxChBrgGenPortRateLimitTcpSynSet(dev_id, hport, GT_TRUE)) != GT_OK)
			{
				printf("%s.%d cpssDxChBrgGenPortRateLimitTcpSynSet failed!\n",__FILE__,__LINE__);
				goto FAIL;
			}

			if((ret = cpssDxChBrgGenRateLimitGlobalCfgGet(dev_id, &brgRateLimitPtr)) != GT_OK)
			{
				printf("%s.%d cpssDxChBrgGenRateLimitGlobalCfgGet failed!\n",__FILE__,__LINE__);
				goto FAIL;
			}

			brgRateLimitPtr.rMode       = CPSS_RATE_LIMIT_BYTE_BASED_E;
			brgRateLimitPtr.win10Mbps   = WINDOWSIZE;
			brgRateLimitPtr.win100Mbps  = WINDOWSIZE;
			brgRateLimitPtr.win1000Mbps = WINDOWSIZE;
			brgRateLimitPtr.win10Gbps   = WINDOWSIZE;

			if((ret = cpssDxChBrgGenRateLimitGlobalCfgSet(dev_id, &brgRateLimitPtr)) != GT_OK)
			{
				printf("%s.%d cpssDxChBrgGenRateLimitGlobalCfgSet failed!\n",__FILE__,__LINE__);
				goto FAIL;
			}

			{
				 GT_U32 tempLimit;
				 GT_U32 rateGranularityInBytes, maxRateValue;
				 GT_U32 isSip  = PRV_CPSS_SIP_CHECK_MAC(dev_id,PRV_CPSS_SIP_5_10_E);
				 if (isSip)
				 {
					 rateGranularityInBytes = 1;
				     maxRateValue = 4194303;
				 }
				 else
				 {
					 rateGranularityInBytes = 64;
					 maxRateValue = 65535;
				 }

				 tempLimit = (speed * (WINDOWSIZE/1000000) * 1000) / (rateGranularityInBytes * 8);
				 if(tempLimit <= maxRateValue)
				 {
					 portGfgPtr.rateLimit = tempLimit;
				 }
				 else
				 {
					 printf("Rate limit out of range [%s] for kbps = %d ", tempLimit, speed);
					 goto FAIL;
				 }

			}
			printf("pirl enable \n");

		} else {
			
			portGfgPtr.enableBc = GT_FALSE;
			portGfgPtr.enableMc = GT_FALSE;
			portGfgPtr.enableMcReg = GT_FALSE;
			portGfgPtr.enableUcUnk = GT_FALSE;
			portGfgPtr.enableUcKnown = GT_FALSE;

			if((ret = cpssDxChBrgGenPortRateLimitTcpSynSet(dev_id, hport, GT_FALSE)) != GT_OK)
			{
				printf("%s.%d cpssDxChBrgGenPortRateLimitTcpSynSet failed!\n",__FILE__,__LINE__);
				goto FAIL;
			}
			printf("pirl disable \n");
		}

		if((ret = cpssDxChBrgGenPortRateLimitSet(dev_id, hport, &portGfgPtr)) != GT_OK)
		{
			printf("%s.%d cpssDxChBrgGenPortRateLimitSet failed!\n",__FILE__,__LINE__);
			goto FAIL;
		}
	}

	return SW_OK;

FAIL:
	/*FIXME. we need restore action where error occurred!!!*/
	printf("TODO.%s.%d retstore action need here failed!\n",__FILE__,__LINE__);
	return SW_FAIL;
}

static inline sw_error_t _mv3236_rate_bucket_pirl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t * type_mask,fal_rate_speed_t * speed,fal_enable_t * enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
#if 0
	a_uint32_t hport;
	GT_STATUS ret;
    GT_PIRL2_DATA pirlData;
    GT_U32 irlRes;

	irlRes = bucket_id;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport));
	{
		GT_STATUS ret;

		if((PirlEnableFlag[hport] == 1)) {
			if(PirlBucketFlag[bucket_id] == 1) {	
				
				if((ret = gpirl2ReadResource(pJwDev,hport,irlRes,(GT_PIRL2_DATA *)&pirlData)) != GT_OK) {
					return SW_FAIL;
		        }
				
				*enable = FAL_ENABLE;
				*speed = pirlData.ingressRate;
				printf("speed = %d \n",*speed);
				*type_mask = pirlData.bktTypeMask;
				printf("type mask = %d \n",pirlData.bktTypeMask);
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
#endif
	return SW_OK;
}

sw_error_t mv3236_rate_bucket_egrl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t speed,fal_enable_t enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_rate_bucket_egrl_set(dev_id, port_id, speed, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_rate_bucket_egrl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_speed_t *speed,fal_enable_t *enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_rate_bucket_egrl_get(dev_id, port_id, speed, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_rate_bucket_pirl_set(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t type_mask,fal_rate_speed_t speed,fal_enable_t enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_rate_bucket_pirl_set(dev_id, port_id, bucket_id, type_mask, speed, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_rate_bucket_pirl_get(a_uint32_t dev_id,fal_port_t port_id,fal_rate_bucket_t bucket_id,fal_rate_mask_t * type_mask,fal_rate_speed_t *speed,fal_enable_t *enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_rate_bucket_pirl_get(dev_id, port_id, bucket_id, type_mask, speed, enable);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_rate_limit_init(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;
	

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);
	
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    p_api->rate_bucket_egrl_set = mv3236_rate_bucket_egrl_set;
	p_api->rate_bucket_egrl_get = mv3236_rate_bucket_egrl_get;
	p_api->rate_bucket_pirl_set = mv3236_rate_bucket_pirl_set;
	p_api->rate_bucket_pirl_get = mv3236_rate_bucket_pirl_get;

	return SW_OK;
}


