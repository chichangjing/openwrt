
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv88e6097_port_ctrl.h"
#include "mv88e6097_mib.h"
#include "mv88e6097_mirror.h"
#include "mv88e6097_trunk.h"
#include "mv88e6097_rate.h"
#include "mv88e6097_multicast.h"
#include "mv88e6097_vlan.h"
#include "mv88e6097_qos.h"
#include "mv88e6097_mac.h"
#include "mv88e6097_reg_access.h"
#include "mv88e6097_reg.h"
#include "mv88e6097_init.h"

#include "msJwSwitch.h"
#include "gtHwCntl.h"
#include "gtDrvSwRegs.h"

#include "joyware.h"

extern GT_QD_DEV *pJwDev;

static ssdk_init_cfg * mv88e6097_cfg[SW_MAX_NR_DEV] = { 0 };

#if !(defined(KERNEL_MODULE) && defined(USER_MODE))  
static sw_error_t mv88e6097_portproperty_init(a_uint32_t dev_id)
{
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
						case GE_Z20208D_2008:
						case GE_Z20208D_2004:
						case GE_Z20204A_2013:
						case GE_Z20204A_2017:
						case GE_Z20210D_2028:
						case GE_Z20210D_2024:
						case GE_Z20208D_2008_P:
						case GE_Z20208D_2004_P:
							if((port_id >= 3) && (port_id <= 10))
								SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, HSL_PP_PHY));
						break;

						case GE_Z20505D_1414:
						case GE_Z20208D_1008:
						case GE_Z20208D_1004:
						case GE_Z20204A_1013:
						case GE_Z20204A_1017:
						case GE_SDI1000_0014_T:
						case GE_SDI1000_0014_R:
							if((port_id >= 2) && (port_id <= 10))
								SW_RTN_ON_ERROR(hsl_port_prop_set(dev_id, port_id, HSL_PP_PHY));
						break;

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

static sw_error_t mv88e6097_hw_init(a_uint32_t dev_id)
{
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


static sw_error_t mv88e6097_dev_init(a_uint32_t dev_id, a_bool_t en_init_chip)
{
    hsl_dev_t *pdev = NULL;
	
    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;

	pdev->dev_id = dev_id;
    pdev->cpu_port = 0;		/* Logic port */
    pdev->nr_ports = 11;

	if(jwSwitchStart(CONFIG_JWS_PORT_CPU_SEQ, (int)en_init_chip) != GT_OK) 
		return SW_NOT_INITIALIZED;

    return SW_OK;
}

static inline sw_error_t _mv88e6097_reset(a_uint32_t dev_id)
{
#if !(defined(KERNEL_MODULE) && defined(USER_MODE))
    //a_uint32_t val;
    sw_error_t rv;

    /* To be add */
	
    rv = mv88e6097_hw_init(dev_id);
    SW_RTN_ON_ERROR(rv);
#endif

    return SW_OK;
}

static inline sw_error_t _mv88e6097_mode_tag(a_uint32_t dev_id)
{
    sw_error_t rv = SW_OK;
	{
		//GT_ATU_ENTRY	atuEntry;
		GT_ATU_ENTRY macEntry;
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
    	enBits |= 0x4000; /* Enable 01-80-C2-00-00-0E */
    	if(gsysSetRsvd2CpuEnables(pJwDev,enBits) != GT_OK) {
			printf("gsysSetRsvd2CpuEnables failed\n");
            return SW_FAIL;
        }

		/* Enable detect MGMT frames */
    	if(gsysSetRsvd2Cpu(pJwDev,GT_TRUE) != GT_OK) {
			printf("gsysSetRsvd2Cpu failed\n");
            return SW_FAIL;
        }
#if 0

		memset(&atuEntry,0,sizeof(GT_ATU_ENTRY));
		
        /* Set the Atu entry parameters.    */
        atuEntry.macAddr.arEther[0] = 0x01;
        atuEntry.macAddr.arEther[1] = 0x80;
        atuEntry.macAddr.arEther[2] = 0xC2;
        atuEntry.macAddr.arEther[3] = 0x00;
        atuEntry.macAddr.arEther[4] = 0x00;
        atuEntry.macAddr.arEther[5] = 0x00;
		atuEntry.portVec = (GT_U32)(1<<pJwDev->cpuPortNum);
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
#endif

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
		macEntry.portVec = (1<<pJwDev->cpuPortNum);
		macEntry.prio = 3;
		macEntry.entryState.mcEntryState = GT_MC_PRIO_MGM_STATIC;

		if(gfdbAddMacEntry(pJwDev,&macEntry) != GT_OK) {
			printf("gfdbAddMacEntry return failed\r\n");
			return SW_FAIL;
		}


		//printf("MGMT frames setup successfully!\n");
		printf("Hport_%d DSA tag mode set successfully!\n",(int)pJwDev->cpuPortNum);
		
	}
    return rv;
}


sw_error_t mv88e6097_cleanup(void)
{
    a_uint32_t dev_id;

    for (dev_id = 0; dev_id < SW_MAX_NR_DEV; dev_id++) {
        if (mv88e6097_cfg[dev_id]) {
            aos_mem_free(mv88e6097_cfg[dev_id]);
            mv88e6097_cfg[dev_id] = NULL;
        }
    }

    /* To be add */

    return SW_OK;
}

/**
 * @brief Reset module.
 * @details Comments:
 *    This operation will reset mv88e6097.
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
sw_error_t mv88e6097_reset(a_uint32_t dev_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_reset(dev_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Reset module.
 * @details Comments:
 *    This operation will reset mv88e6097.
 * @param[in] dev_id device id
 * @return SW_OK or error code
 */
sw_error_t mv88e6097_mode_tag(a_uint32_t dev_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mode_tag(dev_id);
    HSL_API_UNLOCK;
    return rv;
}

/**
 * @brief Init module.
 * @details Comments:
 *    This operation will init mv88e6097.
 * @param[in] dev_id device id
 * @param[in] cfg configuration for initialization
 * @return SW_OK or error code
 */
sw_error_t mv88e6097_init(a_uint32_t dev_id, ssdk_init_cfg * cfg)
{
    HSL_DEV_ID_CHECK(dev_id);

    if (NULL == mv88e6097_cfg[dev_id]) {
        mv88e6097_cfg[dev_id] = (ssdk_init_cfg *)aos_mem_alloc(sizeof (ssdk_init_cfg));
    }

    if (NULL == mv88e6097_cfg[dev_id]) {
        return SW_OUT_OF_MEM;
    }

    aos_mem_copy(mv88e6097_cfg[dev_id], cfg, sizeof (ssdk_init_cfg));
    SW_RTN_ON_ERROR(mv88e6097_reg_access_init(dev_id, cfg->reg_mode));
    SW_RTN_ON_ERROR(mv88e6097_dev_init(dev_id, cfg->en_init_chip));
    
    #if !(defined(KERNEL_MODULE) && defined(USER_MODE))
    {
        SW_RTN_ON_ERROR(hsl_port_prop_init());
        SW_RTN_ON_ERROR(hsl_port_prop_init_by_dev(dev_id));
        SW_RTN_ON_ERROR(mv88e6097_portproperty_init(dev_id));
        #ifdef IN_PORTCONTROL	
        SW_RTN_ON_ERROR(mv88e6097_port_ctrl_init(dev_id));
        #endif

        #ifdef IN_MIB	
        SW_RTN_ON_ERROR(mv88e6097_mib_init(dev_id));
        #endif
		
		#ifdef IN_MIRROR
		SW_RTN_ON_ERROR(mv88e6097_port_mirror_init(dev_id));
		#endif

		#ifdef IN_TRUNK
		SW_RTN_ON_ERROR(mv88e6097_port_trunk_init(dev_id));
		#endif

		#ifdef IN_RATE
		SW_RTN_ON_ERROR(mv88e6097_rate_limit_init(dev_id));
		#endif

		#ifdef IN_MCAST
		SW_RTN_ON_ERROR(mv88e6097_multicast_init(dev_id));
		#endif

		#ifdef IN_VLAN
		SW_RTN_ON_ERROR(mv88e6097_vlan_init(dev_id));
		#endif

		#ifdef IN_QOS
		SW_RTN_ON_ERROR(mv88e6097_qos_init(dev_id));
		#endif

		#ifdef IN_FDB
		SW_RTN_ON_ERROR(mv88e6097_mac_init(dev_id));
		#endif
		
        #ifndef HSL_STANDALONG
        {
            hsl_api_t *p_api;

            SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));
            p_api->dev_reset   = mv88e6097_reset;
			p_api->dev_mode_tag = mv88e6097_mode_tag;
        }    
        #endif

        SW_RTN_ON_ERROR(mv88e6097_hw_init(dev_id));
     }
     #endif

    return SW_OK;
}

