 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv88e6097_mirror.h"
#include "mv88e6097_port_ctrl.h"
#include "mv88e6097_reg.h"
#include "msApi.h"
#include "gtHwCntl.h"
#include "gtDrvSwRegs.h"

#include "joyware.h"


extern GT_QD_DEV *pJwDev;
static a_uint32_t DestPort,SourcePort;



static inline sw_error_t _mv88e6097_mirror_dest_port_set(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv = SW_OK;
	GT_STATUS ret;
	a_uint32_t hport_id;
	a_uint32_t i,lport;

	DestPort = (1 << port_id);

	/* Disable previous source port */
	SourcePort &= ~(1 << port_id);
	/* Use port_id 0 For Disable All Mirror Feature */
	if(port_id == 0){
		for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++){
			lport = i+1;
			
			SW_RTN_ON_ERROR(map_lport2hport(lport, &hport_id));
			if((ret = gprtSetEgressMonitorSource(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					printf("gprtSetEgressMonitorSource Failed\n");
					return SW_FAIL;
				}	
			if((ret = gprtSetIngressMonitorSource(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
				printf("gprtSetIngressMonitorSource Failed\n");
				return SW_FAIL;
			}
		}
		/* Clear All SourcePort Flag */
		SourcePort = 0;
		
		return rv;
	}
	
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	if((ret = gprtSetEgressMonitorSource(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
			printf("gprtSetEgressMonitorSource Failed\n");
			return SW_FAIL;
		}	
	if((ret = gprtSetIngressMonitorSource(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
		printf("gprtSetIngressMonitorSource Failed\n");
		return SW_FAIL;
	}
	
	/* mirror destnation port select */
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		if((ret = gsysSetIngressMonitorDest(pJwDev, hport_id)) != GT_OK) {
    		return SW_FAIL;
		}

		if((ret = gsysSetEgressMonitorDest(pJwDev, hport_id)) != GT_OK) {
			return SW_FAIL;
		}
	}
	
    return rv;

}

static inline sw_error_t _mv88e6097_mirror_dest_port_get(a_uint32_t dev_id, fal_port_t *port_id)
{
    sw_error_t rv = SW_OK;
	GT_LPORT In_hport_id, En_hport_id, In_lport, En_lport, default_id;
	/* mirror destnation port select */

	GT_STATUS ret;
	if((ret = gsysGetIngressMonitorDest(pJwDev, &In_hport_id)) != GT_OK) {
		return SW_FAIL;
	}

	if((ret = gsysGetEgressMonitorDest(pJwDev, &En_hport_id)) != GT_OK) {
		return SW_FAIL;
	}

	if((In_hport_id == 0xF) && (En_hport_id == 0xF)) {
		default_id = 0;
		*port_id = default_id;
		return rv;
	}		
	
	SW_RTN_ON_ERROR(map_hport2lport((a_uint32_t)In_hport_id, (a_uint32_t *)&In_lport));
	if((In_lport > 0) && (In_lport <11))
		*port_id = In_lport;

	SW_RTN_ON_ERROR(map_hport2lport((a_uint32_t)En_hport_id, (a_uint32_t *)&En_lport));
	if((En_lport > 0) && (En_lport <11))
		*port_id = En_lport;
	//printf("lport = %d\n",lport);
	
    return rv;

}

static inline sw_error_t  _mv88e6097_mirror_source_port_mode_set(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t mode)
{
	a_uint32_t hport_id;

	SW_RTN_ON_ERROR(map_lport2hport(source_port, &hport_id));
	{
		GT_STATUS ret;
		
		/* port mirror enable, default all */
		if(mode == FAL_MIRROR_MODE_DISABLE) {

			SourcePort &= ~(1 << source_port); 
			
			if((ret = gprtSetEgressMonitorSource(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
					printf("gprtSetEgressMonitorSource Failed\n");
					return SW_FAIL;
        		}
			
			if((ret = gprtSetIngressMonitorSource(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
				printf("gprtSetIngressMonitorSource Failed\n");
				return SW_FAIL;
    		}
		} else {
			/* check SourcePort and Dest port whether different*/
			SourcePort |= (1 << source_port);
			if((SourcePort & DestPort) != 0){
				SourcePort &= ~(1 << source_port);
				printf("error: sourceport or destport cant be the same!\n");
				return SW_BAD_PARAM;
			}
	
			/* mirror source port mode  select, tx frames, rx frames, both tx and rx frames */
			switch(mode) {
				case FAL_MIRROR_MODE_TX:
					if((ret = gprtSetEgressMonitorSource(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
						printf("gprtSetEgressMonitorSource Failed\n");
						return SW_FAIL;
	        		}
				
					if((ret = gprtSetIngressMonitorSource(pJwDev, hport_id, GT_FALSE)) != GT_OK) {	
						printf("gprtSetIngressMonitorSource Failed\n");
						return SW_FAIL;
		    		}
					break;
					
				case FAL_MIRROR_MODE_RX:
					if((ret = gprtSetEgressMonitorSource(pJwDev, hport_id, GT_FALSE)) != GT_OK) {
						printf("gprtSetEgressMonitorSource Failed\n");
						return SW_FAIL;
	        		}
				
					if((ret = gprtSetIngressMonitorSource(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
						printf("gprtSetIngressMonitorSource Failed\n");
						return SW_FAIL;
		    		}
					break;
					
				case FAL_MIRROR_MODE_BOTH:
					if((ret = gprtSetEgressMonitorSource(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
						printf("gprtSetEgressMonitorSource Failed\n");
						return SW_FAIL;
	        		}
				
					if((ret = gprtSetIngressMonitorSource(pJwDev, hport_id, GT_TRUE)) != GT_OK) {
						printf("gprtSetIngressMonitorSource Failed\n");
						return SW_FAIL;
		    		}
					break;
				default:
					break;
			}
		}
	}

	return SW_OK;
}

static inline sw_error_t  _mv88e6097_mirror_source_port_mode_get(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t * mode)
{
	a_uint32_t hport_id;
	
	SW_RTN_ON_ERROR(map_lport2hport(source_port, &hport_id));
	{
		GT_STATUS ret;
		GT_BOOL ingress, egress;

		if((ret = gprtGetEgressMonitorSource(pJwDev, hport_id, &egress)) != GT_OK) {
			return SW_FAIL;
		}
	
		if((ret = gprtGetIngressMonitorSource(pJwDev, hport_id, &ingress)) != GT_OK) {
			return SW_FAIL;
		}
		/* get mirror direction , tx frames, rx frames, all frames */
		if((egress == GT_TRUE) && (ingress == GT_TRUE)) {
			*mode = FAL_MIRROR_MODE_BOTH;
		} else if((egress == GT_TRUE) && (ingress == GT_FALSE)) {
			*mode = FAL_MIRROR_MODE_TX;
		} else if((egress == GT_FALSE) && (ingress == GT_TRUE)) {
			*mode = FAL_MIRROR_MODE_RX;
		} else if((egress == GT_FALSE) && (ingress == GT_FALSE)) {
			*mode = FAL_MIRROR_MODE_DISABLE;
		}
	}

	return SW_OK;
}



sw_error_t mv88e6097_mirror_dest_port_set(a_uint32_t dev_id, fal_port_t port_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mirror_dest_port_set(dev_id, port_id);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_mirror_dest_port_get(a_uint32_t dev_id, fal_port_t *port_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mirror_dest_port_get(dev_id, port_id);
    HSL_API_UNLOCK;
	
    return rv;
}


sw_error_t mv88e6097_mirror_source_port_mode_set(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mirror_source_port_mode_set(dev_id, source_port, mode);
    HSL_API_UNLOCK;
	
    return rv;	
}

sw_error_t mv88e6097_mirror_source_port_mode_get(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mirror_source_port_mode_get(dev_id, source_port, mode);
    HSL_API_UNLOCK;
	
    return rv;	
}



sw_error_t mv88e6097_port_mirror_init(a_uint32_t dev_id)
{
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);
	
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    p_api->mirror_dest_port_set = mv88e6097_mirror_dest_port_set;
	p_api->mirror_dest_port_get = mv88e6097_mirror_dest_port_get;
	p_api->mirror_source_port_mode_set = mv88e6097_mirror_source_port_mode_set;
	p_api->mirror_source_port_mode_get = mv88e6097_mirror_source_port_mode_get;

	return SW_OK;
}


