 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv3236_global.h"
#include "mv3236_mirror.h"
#include "mv3236_port_ctrl.h"
#include "mv3236_reg.h"
#include "cpss/dxCh/dxChxGen/mirror/cpssDxChMirror.h"
//#include "msApi.h"
//#include "gtHwCntl.h"
//#include "gtDrvSwRegs.h"

//extern GT_QD_DEV *pJwDev;
/*right now, support only one analyzer interface according 6097's requirement*/
#define DESTINTERFACEINDEX  1
static a_uint32_t DestPort,SourcePort;


static inline sw_error_t _mv3236_mirror_dest_port_set(a_uint32_t dev_id, fal_port_t port_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	GT_STATUS ret;
	a_uint32_t hport_id;
	a_uint32_t i,lport;
	GT_BOOL is_PhysicalPort  = GT_TRUE;/*FIXME. not consider eport case*/

	{
		CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_MODE_ENT   modePtr;
		cpssDxChMirrorToAnalyzerForwardingModeGet(dev_id, &modePtr);
		SW_PRINTFL(DETAIL,"====enter modePtr a %d ====\n", modePtr);

		cpssDxChMirrorToAnalyzerForwardingModeSet(dev_id, CPSS_DXCH_MIRROR_TO_ANALYZER_FORWARDING_SOURCE_BASED_OVERRIDE_E);

		cpssDxChMirrorToAnalyzerForwardingModeGet(dev_id, &modePtr);
		SW_PRINTFL(DETAIL,"====enter modePtr b %d ====\n", modePtr);

	}

	DestPort = (1 << port_id);

	/* Disable previous source port */
	SourcePort &= ~(1 << port_id);
	/* Use port_id 0 For Disable All Mirror Feature */
	if(port_id == 0){
		for(i=0; i<MV3236_MAX_PORT_NUM-1; i++){
			lport = i+1;
			
			SW_RTN_ON_ERROR(map_lport2hport(lport, &hport_id));
			if((ret = cpssDxChMirrorTxPortSet(dev_id, hport_id, is_PhysicalPort, GT_FALSE, DESTINTERFACEINDEX)) != GT_OK) {
					printf("cpssDxChMirrorTxPortSet Failed\n");
					return SW_FAIL;
				}	
			if((ret = cpssDxChMirrorRxPortSet(dev_id, hport_id, is_PhysicalPort, GT_FALSE, DESTINTERFACEINDEX)) != GT_OK) {
				printf("cpssDxChMirrorRxPortSet Failed\n");
				return SW_FAIL;
			}
		}
		/* Clear All SourcePort Flag */
		SourcePort = 0;
		
		return rv;
	}
	
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	if((ret = cpssDxChMirrorTxPortSet(dev_id, hport_id, is_PhysicalPort, GT_FALSE, DESTINTERFACEINDEX)) != GT_OK) {
			printf("cpssDxChMirrorTxPortSet Failed\n");
			return SW_FAIL;
		}	
	if((ret = cpssDxChMirrorRxPortSet(dev_id, hport_id, is_PhysicalPort, GT_FALSE, DESTINTERFACEINDEX)) != GT_OK) {
		printf("cpssDxChMirrorRxPortSet Failed\n");
		return SW_FAIL;
	}
	SW_PRINTFL(DETAIL,"====enter====\n");
	/* mirror destnation port select */
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport_id));
	{
		GT_STATUS ret;
		CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   interfacePtr;
		GT_HW_DEV_NUM   hwDevNum;
		cpssDxChCfgHwDevNumGet(dev_id, &hwDevNum);

		SW_PRINTFL(DETAIL,"========dev_id %d, hwDevNum %d\n", dev_id, hwDevNum);
		interfacePtr.interface.type = CPSS_INTERFACE_PORT_E;
		interfacePtr.interface.devPort.hwDevNum = hwDevNum;
		interfacePtr.interface.devPort.portNum = hport_id;
		SW_PRINTFL(DETAIL,"====enter====\n");
		if((ret = cpssDxChMirrorAnalyzerInterfaceSet(dev_id, DESTINTERFACEINDEX, &interfacePtr)) != GT_OK) {
			printf("cpssDxChMirrorAnalyzerInterfaceSet dev_id %d , hport_id %dFailed\n", dev_id, hport_id);
    		return SW_FAIL;
		}

	}

    return rv;

}

static inline sw_error_t _mv3236_mirror_dest_port_get(a_uint32_t dev_id, fal_port_t *port_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    GT_STATUS ret;
    a_uint32_t lport, hport, default_id;
	CPSS_DXCH_MIRROR_ANALYZER_INTERFACE_STC   interfacePtr;

	if((ret = cpssDxChMirrorAnalyzerInterfaceGet(dev_id, DESTINTERFACEINDEX, &interfacePtr)) != GT_OK) {
		printf("cpssDxChMirrorAnalyzerInterfaceGet  Failed\n");
		return SW_FAIL;
	}
	
	lport = interfacePtr.interface.devPort.portNum;

	SW_PRINTFL(DETAIL,"======== lport%d , type %d\n", lport, interfacePtr.interface.type);
	//if(lport < 0 || lport > MV3236_MAX_PORT_NUM)
	if(lport < 0 || lport > 28)
	{
		printf("lport %d is out of range Failed\n", lport);
		return SW_FAIL;
	}

	SW_RTN_ON_ERROR(map_hport2lport(lport, port_id));

	{
		int i;
		for(i = 0; i < 6; i++)
		{
			GT_U32 enable = 0;
			cpssDxChMirrorAnalyzerInterfaceGet(dev_id, i, &interfacePtr);
			lport = interfacePtr.interface.devPort.portNum;
			SW_PRINTFL(DETAIL,"======== lport%d , type %d, enable %d \n", lport, interfacePtr.interface.type, enable);
		}


	}

    return SW_OK;
}

static inline sw_error_t  _mv3236_mirror_source_port_mode_set(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	GT_STATUS status;
	a_uint32_t hport_id;
	GT_BOOL is_PhysicalPort  = GT_TRUE;/*FIXME. not consider eport case*/

	SW_RTN_ON_ERROR(map_lport2hport(source_port, &hport_id));
	{
		GT_STATUS ret;
		
		/* port mirror enable, default all */
		if(mode == FAL_MIRROR_MODE_DISABLE) {

			SourcePort &= ~(1 << source_port); 

			if((ret = cpssDxChMirrorTxPortSet(dev_id, hport_id, is_PhysicalPort, GT_FALSE, DESTINTERFACEINDEX)) != GT_OK) {
					printf("cpssDxChMirrorTxPortSet Failed\n");
					return SW_FAIL;
        		}
			
			if((ret = cpssDxChMirrorRxPortSet (dev_id, hport_id, is_PhysicalPort, GT_FALSE, DESTINTERFACEINDEX)) != GT_OK) {
				printf("cpssDxChMirrorTxPortSet Failed\n");
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
					if((ret = cpssDxChMirrorTxPortSet(dev_id, hport_id, is_PhysicalPort, GT_TRUE, DESTINTERFACEINDEX)) != GT_OK) {
						printf("cpssDxChMirrorTxPortSet Failed\n");
						return SW_FAIL;
	        		}
				
					if((ret = cpssDxChMirrorRxPortSet(dev_id, hport_id, is_PhysicalPort, GT_FALSE, DESTINTERFACEINDEX)) != GT_OK) {
						printf("cpssDxChMirrorRxPortSet Failed\n");
						return SW_FAIL;
		    		}
					break;
					
				case FAL_MIRROR_MODE_RX:
					if((ret = cpssDxChMirrorTxPortSet(dev_id, hport_id, is_PhysicalPort, GT_FALSE, DESTINTERFACEINDEX)) != GT_OK) {
						printf("cpssDxChMirrorTxPortSet Failed\n");
						return SW_FAIL;
	        		}
				
					if((ret = cpssDxChMirrorRxPortSet(dev_id, hport_id, is_PhysicalPort, GT_TRUE, DESTINTERFACEINDEX)) != GT_OK) {
						printf("cpssDxChMirrorRxPortSet Failed\n");
						return SW_FAIL;
		    		}
					break;
					
				case FAL_MIRROR_MODE_BOTH:
					if((ret = cpssDxChMirrorTxPortSet(dev_id, hport_id, is_PhysicalPort, GT_TRUE, DESTINTERFACEINDEX)) != GT_OK) {
						printf("cpssDxChMirrorTxPortSet Failed\n");
						return SW_FAIL;
	        		}
				
					if((ret = cpssDxChMirrorRxPortSet(dev_id, hport_id, is_PhysicalPort, GT_TRUE, DESTINTERFACEINDEX)) != GT_OK) {
						printf("cpssDxChMirrorRxPortSet Failed\n");
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

static inline sw_error_t  _mv3236_mirror_source_port_mode_get(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t * mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	a_uint32_t hport_id;
	GT_U32 indexPtr = 0;
	GT_BOOL is_PhysicalPort  = GT_TRUE;/*FIXME. not consider eport case*/

	SW_RTN_ON_ERROR(map_lport2hport(source_port, &hport_id));
	{
		GT_STATUS ret;
		GT_BOOL ingress, egress;

		if((ret = cpssDxChMirrorTxPortGet(dev_id, hport_id, is_PhysicalPort, &egress, &indexPtr)) != GT_OK) {
			printf("cpssDxChMirrorTxPortGet Failed\n");
			return SW_FAIL;
		}
	
		if((ret = cpssDxChMirrorRxPortGet(dev_id, hport_id, is_PhysicalPort, &ingress, &indexPtr)) != GT_OK) {
			printf("cpssDxChMirrorTxPortGet Failed\n");
			return SW_FAIL;
		}

		SW_PRINTFL(DETAIL,"====hport_id %d, egress %d, ingress %d, indexPtr %d ====\n", hport_id, egress, ingress, indexPtr);
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

sw_error_t mv3236_mirror_dest_port_set(a_uint32_t dev_id, fal_port_t port_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_mirror_dest_port_set(dev_id, port_id);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_mirror_dest_port_get(a_uint32_t dev_id, fal_port_t *port_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_mirror_dest_port_get(dev_id, port_id);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_mirror_source_port_mode_set(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_mirror_source_port_mode_set(dev_id, source_port, mode);
    HSL_API_UNLOCK;
	
    return rv;	
}

sw_error_t mv3236_mirror_source_port_mode_get(a_uint32_t dev_id,fal_port_t source_port,fal_mirror_mode_t * mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_mirror_source_port_mode_get(dev_id, source_port, mode);
    HSL_API_UNLOCK;
	
    return rv;	
}

sw_error_t mv3236_port_mirror_init(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);
	
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

    p_api->mirror_dest_port_set = mv3236_mirror_dest_port_set;
	p_api->mirror_dest_port_get = mv3236_mirror_dest_port_get;
	p_api->mirror_source_port_mode_set = mv3236_mirror_source_port_mode_set;
	p_api->mirror_source_port_mode_get = mv3236_mirror_source_port_mode_get;

	return SW_OK;
}


