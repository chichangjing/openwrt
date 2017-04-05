
#include "sw.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv3236_mib.h"
#include "mv3236_port_ctrl.h"
//#include "msApi.h"
//#include "gtHwCntl.h"
//#include "gtDrvSwRegs.h"

//extern GT_QD_DEV *pJwDev;

static sw_error_t _mv3236_mib_info_get(a_uint32_t dev_id, fal_port_t port_id, fal_mib_info_t *mib_info)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;
#if 0
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		GT_STATS_COUNTER_SET3 CounterStats;

	    if((ret = gstatsGetPortAllCounters3(pJwDev, hport_id, &CounterStats)) != GT_OK) {
			printf("%s, gstatsGetPortAllCounters3 Fail!\n",__FUNCTION__);
	        return SW_FAIL;
	    }

		mib_info->RxGoodByteLo		= CounterStats.InGoodOctetsLo;
		mib_info->RxGoodByteHi		= CounterStats.InGoodOctetsHi;
		mib_info->RxUnicastPkts		= CounterStats.InUnicasts;
		mib_info->RxBroadcastPkts	= CounterStats.InBroadcasts;
		mib_info->RxMulticastPkts	= CounterStats.InMulticasts;
		mib_info->RxPausePkts		= CounterStats.InPause;
		
		mib_info->TxByteLo			= CounterStats.OutOctetsLo;
		mib_info->TxByteHi			= CounterStats.OutOctetsHi;
		mib_info->TxUnicastPkts		= CounterStats.OutUnicasts;
		mib_info->TxBroadcastPkts	= CounterStats.OutBroadcasts;
		mib_info->TxMulticastPkts	= CounterStats.OutMulticasts;
		mib_info->TxPausePkts		= CounterStats.OutPause;
	}
#endif
    return rv;
}

static sw_error_t _mv3236_mib_info_clear(a_uint32_t dev_id, fal_port_t port_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t hport_id;
#if 0
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;

	    if((ret = gstatsFlushPort(pJwDev, hport_id)) != GT_OK) {
			printf("%s,%d Failed!\n",__FUNCTION__,__LINE__);
	        return SW_FAIL;
	    }
	}
#endif
    return rv;
}

/**
 * @brief Get mib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @param[out] mib_info mib infomation
 * @return SW_OK or error code
 */
sw_error_t mv3236_mib_info_get(a_uint32_t dev_id, fal_port_t port_id, fal_mib_info_t *mib_info)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;
#if 0
    HSL_API_LOCK;
    rv = _mv3236_mib_info_get(dev_id, port_id, mib_info);
    HSL_API_UNLOCK;
#endif
    return rv;
}

/**
 * @brief Clear mib infomation on particular port.
 * @param[in] dev_id device id
 * @param[in] port_id port id
 * @return SW_OK or error code
 */
sw_error_t mv3236_mib_info_clear(a_uint32_t dev_id, fal_port_t port_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;
#if 0
    HSL_API_LOCK;
    rv = _mv3236_mib_info_clear(dev_id, port_id);
    HSL_API_UNLOCK;
#endif
    return rv;
}

#if 0
sw_error_t mv3236_mib_test_show(a_uint32_t dev_id, fal_port_t port_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	fal_mib_info_t mibinfo;

	SW_RTN_ON_ERROR(mv3236_mib_info_get(dev_id, port_id, &mibinfo));
	
	printf("\n");
	printf("[Port%d mib information]\n", port_id);
	printf("  RxGoodByteLo     : %-10u  TxByteLo         : %-10u\n", mibinfo.RxGoodByteLo, mibinfo.TxByteLo);	
	printf("  RxGoodByteHi     : %-10u  TxByteHi         : %-10u\n", mibinfo.RxGoodByteHi, mibinfo.TxByteHi);
	printf("  RxUnicastPkts    : %-10u  TxUnicastPkts    : %-10u\n", mibinfo.RxUnicastPkts, mibinfo.TxUnicastPkts);
	printf("  RxBroadcastPkts  : %-10u  TxBroadcastPkts  : %-10u\n", mibinfo.RxBroadcastPkts, mibinfo.TxBroadcastPkts);
	printf("  RxMulticastPkts  : %-10u  TxMulticastPkts  : %-10u\n", mibinfo.RxMulticastPkts, mibinfo.TxMulticastPkts);
	printf("  RxPausePkts      : %-10u  TxPausePkts      : %-10u\n", mibinfo.RxPausePkts, mibinfo.TxPausePkts);
	printf("\n");
	
    return SW_OK;
}
#endif

sw_error_t mv3236_mib_init(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;

	pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
	
    p_api->mib_info_get = mv3236_mib_info_get;
    p_api->mib_info_clear = mv3236_mib_info_clear;

    return SW_OK;
}



