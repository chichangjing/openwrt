
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv3236_port_ctrl.h"
#include "mv3236_mib.h"
#include "mv3236_mirror.h"
#include "mv3236_trunk.h"
#include "mv3236_rate.h"
#include "mv3236_multicast.h"
#include "mv3236_vlan.h"
#include "mv3236_qos.h"
#include "mv3236_mac.h"
#include "mv3236_reg_access.h"
#include "mv3236_reg.h"
#include "mv3236_init.h"

//#include "msJwSwitch.h"
//#include "gtHwCntl.h"
//#include "gtDrvSwRegs.h"
#include  "sysHwConfig.h"
typedef int GT_QD_DEV;
GT_QD_DEV *pJwDev = NULL;
//int multiProcessAppDemo = 0;

static ssdk_init_cfg * mv3236_cfg[SW_MAX_NR_DEV] = { 0 };

#if !(defined(KERNEL_MODULE) && defined(USER_MODE))  
static sw_error_t mv3236_portproperty_init(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    hsl_port_prop_t p_type;
    hsl_dev_t *pdev = NULL;
    fal_port_t port_id;
	sal_sys_model_t model;
	
    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
	SW_RTN_ON_ERROR(sal_sys_model_get(&model));
	
    for (port_id = 0; port_id < pdev->nr_ports; port_id++) {
        hsl_port_prop_portmap_set(dev_id, port_id);
		
        for (p_type = HSL_PP_PHY; p_type < HSL_PP_END; p_type++) {
            switch (p_type) {
				case HSL_PP_PHY:
					switch(model.model_id) {
#if 0
						case GE_Z20208D:
							if((port_id >= 3) && (port_id <= 10))
								SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, HSL_PP_PHY));
						break;

						case GE_Z20505D:
							if((port_id >= 2) && (port_id <= 10))
								SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, HSL_PP_PHY));
						break;
#endif
						default:
						break;
					}	
				break;
				
	            case HSL_PP_INCL_CPU:
	           		SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
	            break;

	            case HSL_PP_EXCL_CPU:
	                if (port_id != pdev->cpu_port)
	                    SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, p_type));
                break;

	            default:
	            break;
	        }
        }		
    }

    return SW_OK;
}

static sw_error_t mv3236_hw_init(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    hsl_dev_t  *pdev = NULL;
    a_uint32_t port_id;//, data;
    //sw_error_t rv;

    pdev = hsl_dev_ptr_get(dev_id);
    if (NULL == pdev) {
        return SW_NOT_INITIALIZED;
    }

    for (port_id = 0; port_id < pdev->nr_ports; port_id++) {
        if (port_id == pdev->cpu_port) {
            continue;
        }

		/* To be add */
    }

    return SW_OK;
}


#endif


static sw_error_t mv3236_dev_init(a_uint32_t dev_id, a_bool_t en_init_chip)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    hsl_dev_t *pdev = NULL;
	
    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;//

	pdev->dev_id = dev_id;
    pdev->cpu_port = 0;		/* Logic port */
    pdev->nr_ports = 28;

   startEngine(1);

#if 0
    if(jwSwitchStart(JW_CPU_PORT_NO, (int)en_init_chip) != GT_OK)
		return SW_NOT_INITIALIZED;
#endif

    return SW_OK;
}

static inline sw_error_t _mv3236_reset(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
    //a_uint32_t val;
    sw_error_t rv;

    /* To be add */
	
    rv = mv3236_hw_init(dev_id);
    SW_RTN_ON_ERROR(rv);
#endif

    return SW_OK;
}

static inline sw_error_t _mv3236_mode_tag(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	{
#if 0
		GT_ATU_ENTRY	atuEntry,macEntry;

		GT_U16	enBits;

		/* Setup the Frame Mode for CPU port */
	    if(gprtSetFrameMode(pJwDev, pJwDev->cpuPortNum, GT_FRAME_MODE_DSA) != GT_OK) {
			printf("gprtSetFrameMode failed\n");
	        return SW_FAIL;
	    }

    	if(gsysSetMGMTPri(pJwDev,7) != GT_OK) {
			printf("gsysSetMGMTPri failed\n");
            return SW_FAIL;
        }

    	if(gsysGetRsvd2CpuEnables(pJwDev,&enBits) != GT_OK) {
			printf("gsysGetRsvd2CpuEnables failed\n");
            return SW_FAIL;
        }
    	enBits = 0x4001; /* Enable 01-80-C2-00-00-(00/0E) */
    	if(gsysSetRsvd2CpuEnables(pJwDev,enBits) != GT_OK) {
			printf("gsysSetRsvd2CpuEnables failed\n");
            return SW_FAIL;
        }

		/* Enable detect MGMT frames */
    	if(gsysSetRsvd2Cpu(pJwDev,GT_TRUE) != GT_OK) {
			printf("gsysSetRsvd2Cpu failed\n");
            return SW_FAIL;
        }


		memset(&atuEntry,0,sizeof(GT_ATU_ENTRY));
		
        /* Set the Atu entry parameters.    */
        atuEntry.macAddr.arEther[0] = 0x01;
        atuEntry.macAddr.arEther[1] = 0x80;
        atuEntry.macAddr.arEther[2] = 0xC2;
        atuEntry.macAddr.arEther[3] = 0x00;
        atuEntry.macAddr.arEther[4] = 0x00;
        atuEntry.macAddr.arEther[5] = 0x00;
		atuEntry.portVec = (GT_U32)(1<<JW_CPU_PORT_NO);
        atuEntry.DBNum = 0;
        atuEntry.prio = 3;
        atuEntry.exPrio.useMacFPri = 0;
        atuEntry.exPrio.macFPri = 0;
        atuEntry.exPrio.macQPri = 0;
        atuEntry.entryState.mcEntryState = GT_MC_PRIO_MGM_STATIC;

        if (gfdbAddMacEntry(pJwDev,&atuEntry) != GT_OK) {
			printf("gfdbAddMacEntry failed\n");
            return SW_FAIL;
        }

		/* Obring Mgmt Multicast */
		memset(&macEntry,0,sizeof(GT_ATU_ENTRY));
		
		/* Set the Atu entry parameters.    */
        macEntry.macAddr.arEther[0] = 0x0D;
        macEntry.macAddr.arEther[1] = 0xA4;
        macEntry.macAddr.arEther[2] = 0x2A;
        macEntry.macAddr.arEther[3] = 0x00;
        macEntry.macAddr.arEther[4] = 0x00;
        macEntry.macAddr.arEther[5] = 0x05;
		macEntry.DBNum = 0;
		macEntry.portVec = (1<<JW_CPU_PORT_NO);
		macEntry.prio = 3;
		macEntry.entryState.mcEntryState = GT_MC_PRIO_MGM_STATIC;

		if(gfdbAddMacEntry(pJwDev,&macEntry) != GT_OK) {
			printf("gfdbAddMacEntry return failed\r\n");
			return SW_FAIL;
		}

		printf("MGMT frames setup successfully!\n");
#endif
	}
    return rv;
}


sw_error_t mv3236_cleanup(void)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    a_uint32_t dev_id;

    for (dev_id = 0; dev_id < SW_MAX_NR_DEV; dev_id++) {
        if (mv3236_cfg[dev_id]) {
            aos_mem_free(mv3236_cfg[dev_id]);
            mv3236_cfg[dev_id] = NULL;
        }
    }

    /* To be add */

    return SW_OK;
}

/**
 * @brief Reset module.
 * @details Comments:
 *    This operation will reset mv3236.
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
sw_error_t mv3236_reset(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_reset(dev_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Reset module.
 * @details Comments:
 *    This operation will reset mv3236.
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
sw_error_t mv3236_mode_tag(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_mode_tag(dev_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Init module.
 * @details Comments:
 *    This operation will init mv3236.
 * @param[in] dev_id device id
 * @param[in] cfg configuration for initialization
 * @return SW_OK or error code
 */
sw_error_t mv3236_init(a_uint32_t dev_id, ssdk_init_cfg * cfg)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    HSL_DEV_ID_CHECK(dev_id);

    if (NULL == mv3236_cfg[dev_id]) {
        mv3236_cfg[dev_id] = (ssdk_init_cfg *)aos_mem_alloc(sizeof (ssdk_init_cfg));
    }

    if (NULL == mv3236_cfg[dev_id]) {
        return SW_OUT_OF_MEM;
    }

    aos_mem_copy(mv3236_cfg[dev_id], cfg, sizeof (ssdk_init_cfg));
    SW_RTN_ON_ERROR(mv3236_reg_access_init(dev_id, cfg->reg_mode));
    SW_RTN_ON_ERROR(mv3236_dev_init(dev_id, cfg->en_init_chip));

    #if !(defined(KERNEL_MODULE) && defined(USER_MODE))
    {
        SW_PRINTFL(DETAIL,"====\n");
        SW_RTN_ON_ERROR(hsl_port_prop_init());
        SW_RTN_ON_ERROR(hsl_port_prop_init_by_dev(dev_id));
        SW_RTN_ON_ERROR(mv3236_portproperty_init(dev_id));
        #ifdef IN_PORTCONTROL	
        SW_RTN_ON_ERROR(mv3236_port_ctrl_init(dev_id));
        #endif

        #ifdef IN_MIB	
        SW_RTN_ON_ERROR(mv3236_mib_init(dev_id));
        #endif
		
		#ifdef IN_MIRROR
		SW_RTN_ON_ERROR(mv3236_port_mirror_init(dev_id));
		#endif

		#ifdef IN_TRUNK
		SW_RTN_ON_ERROR(mv3236_port_trunk_init(dev_id));
		#endif

		#ifdef IN_RATE
		SW_RTN_ON_ERROR(mv3236_rate_limit_init(dev_id));
		#endif

		#ifdef IN_MCAST
		SW_RTN_ON_ERROR(mv3236_multicast_init(dev_id));
		#endif

		#ifdef IN_VLAN
		SW_RTN_ON_ERROR(mv3236_vlan_init(dev_id));
		#endif

		#ifdef IN_QOS
		SW_RTN_ON_ERROR(mv3236_qos_init(dev_id));
		#endif

		#ifdef IN_FDB
		SW_RTN_ON_ERROR(mv3236_mac_init(dev_id));
		#endif

        #ifndef HSL_STANDALONG
        {
            hsl_api_t *p_api;

            SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
            p_api->dev_reset   = mv3236_reset;
			p_api->dev_mode_tag = mv3236_mode_tag;
        }    
        #endif

        SW_RTN_ON_ERROR(mv3236_hw_init(dev_id));

     }
     #endif
    SW_PRINTFL(DETAIL,"====quit====\n");
    return SW_OK;
}

