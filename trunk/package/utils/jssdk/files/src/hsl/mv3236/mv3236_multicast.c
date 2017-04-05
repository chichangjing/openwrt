 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv3236_global.h"
#include "mv3236_multicast.h"
#include "mv3236_port_ctrl.h"
#include "mv3236_reg.h"
#include "cpss/generic/bridge/cpssGenBrgFdb.h"
//#include "msApi.h"
//#include "gtHwCntl.h"
//#include "gtDrvSwRegs.h"

//extern GT_QD_DEV *pJwDev;
static a_uint32_t EnableFlag = 0;
static LIST_HEAD(macCommitList);

sw_error_t multicast_mac_add(a_uint8_t *Mac, a_uint32_t hport_vec)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	GT_STATUS ret;
	a_uint32_t i;
    GT_BOOL	completed = GT_FALSE;
    GT_BOOL	succeeded = GT_FALSE;

	CPSS_MAC_ENTRY_EXT_STC  macEntry;
	memset(&macEntry, 0, sizeof(CPSS_MAC_ENTRY_EXT_STC));
	macEntry.isStatic = GT_TRUE;

	/*FIXME. these parameters setting need verify when debug*/
	macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macEntry.key.key.macVlan.vlanId = VLANINDEX;
	macEntry.key.key.macVlan.macAddr.arEther[0] = Mac[0];
	macEntry.key.key.macVlan.macAddr.arEther[1] = Mac[1];
	macEntry.key.key.macVlan.macAddr.arEther[2] = Mac[2];
	macEntry.key.key.macVlan.macAddr.arEther[3] = Mac[3];
	macEntry.key.key.macVlan.macAddr.arEther[4] = Mac[4];
	macEntry.key.key.macVlan.macAddr.arEther[5] = Mac[5];

	macEntry.dstInterface.type = CPSS_INTERFACE_VIDX_E;

	if((ret = cpssDxChBrgFdbMacEntrySet(DEVICENUM, &macEntry)) != GT_OK) {
		printf("cpssDxChBrgFdbMacEntrySet return failed\r\n");
		return SW_FAIL;
	}

	while(completed == GT_TRUE)
	{
	    if((ret = cpssDxChBrgFdbFromCpuAuMsgStatusGet(DEVICENUM, &completed, &succeeded, NULL)) != GT_OK) {
			printf("cpssDxChBrgFdbFromCpuAuMsgStatusGet return failed\r\n");
			return SW_FAIL;
	    }
	    sleep(1);
	}

	if(succeeded != GT_TRUE)
	{
		printf("cpssDxChBrgFdbFromCpuAuMsgStatusGet return failed  succeeded is %d \r\n", succeeded);
		return SW_FAIL;
	}

	return SW_OK;
}

sw_error_t multicast_mac_delete(a_uint8_t *Mac)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	GT_STATUS ret;
	a_uint32_t i;
    GT_BOOL	completed = GT_FALSE;
    GT_BOOL	succeeded = GT_FALSE;

	CPSS_MAC_ENTRY_EXT_STC  macEntry;
	memset(&macEntry, 0, sizeof(CPSS_MAC_ENTRY_EXT_STC));
	macEntry.isStatic = GT_TRUE;

	/*FIXME. these parameters setting need verify when debug*/
	macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
	macEntry.key.key.macVlan.vlanId = VLANINDEX;
	macEntry.key.key.macVlan.macAddr.arEther[0] = Mac[0];
	macEntry.key.key.macVlan.macAddr.arEther[1] = Mac[1];
	macEntry.key.key.macVlan.macAddr.arEther[2] = Mac[2];
	macEntry.key.key.macVlan.macAddr.arEther[3] = Mac[3];
	macEntry.key.key.macVlan.macAddr.arEther[4] = Mac[4];
	macEntry.key.key.macVlan.macAddr.arEther[5] = Mac[5];

	macEntry.dstInterface.type = CPSS_INTERFACE_VIDX_E;

	if((ret = cpssDxChBrgFdbMacEntryDelete(DEVICENUM, &macEntry)) != GT_OK) {
		printf("cpssDxChBrgFdbMacEntryDelete return failed\r\n");
		return SW_FAIL;
	}

	while(completed == GT_TRUE)
	{
	    if((ret = cpssDxChBrgFdbFromCpuAuMsgStatusGet(DEVICENUM, &completed, &succeeded, NULL)) != GT_OK) {
			printf("cpssDxChBrgFdbFromCpuAuMsgStatusGet return failed\r\n");
			return SW_FAIL;
	    }
	    sleep(1);
	}

	if(succeeded != GT_TRUE)
	{
		printf("cpssDxChBrgFdbFromCpuAuMsgStatusGet return failed  succeeded is %d \r\n", succeeded);
		return SW_FAIL;
	}

	return SW_OK;
}

static inline sw_error_t _mv3236_multicast_enable_set(a_uint32_t dev_id, fal_enable_t enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	a_uint32_t i;
	a_uint32_t lport, hport;
	GT_STATUS ret;
	MacCommit_S *mac,*mac_bak;

	if(enable == FAL_ENABLE) {
		EnableFlag = 1;
		printf("Multicast feature Enable\n");
	} else {
		EnableFlag = 0;
				
		for(i=0; i<MV3236_MAX_PORT_NUM-1; i++) {
			lport = i+1;
			SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
			
			/* Multicast frames with unknown DAs can egress from this port */
		    if((ret = cpssDxChBrgPortEgrFltUregMcastEnable(dev_id, hport, GT_FALSE)) != GT_OK) {
		        printf("Error: gprtSetDefaultForward failed\r\n");
		        return SW_FAIL;
		    }	
		}

		list_for_each_entry_safe(mac, mac_bak, &macCommitList, node)
	    {			
			if((ret = multicast_mac_delete(mac->ptr.mac)) != SW_OK) {
				printf("Error: multicast_mac_delete failed\r\n");
				return SW_FAIL;
			}
			
			list_del(&mac->node);
			free(mac);			
	    }
	
		printf("\n");
		//printf("Multicast feature Disable\n");
	}	

	return SW_OK;
}

static inline sw_error_t _mv3236_multicast_enable_get(a_uint32_t dev_id, fal_enable_t *enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	GT_STATUS ret;
	a_uint32_t i,hport,lport;
	GT_BOOL data;
	a_uint32_t num;

	num = 0;
	for(i=0; i<MV3236_MAX_PORT_NUM-1; i++) {
		lport = (i+1);
		SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
		if((ret = cpssDxChBrgPortEgrFltUregMcastEnableGet(dev_id, hport, &data)) != GT_OK) {
			printf("Error: gprtGetDefaultForward failed\r\n");
			return SW_FAIL;
		}
		/* multicast port num */
		if(data == GT_TRUE){
			num++;
		}
		
	}

	if((num > 0) || (EnableFlag == 1)) {
		*enable = FAL_ENABLE;
		return SW_OK;
	}else{
		*enable = FAL_DISABLE;
		return SW_OK;
	}
}

static inline sw_error_t _mv3236_multicast_row_create(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	GT_STATUS ret;
	a_uint32_t i;
	a_uint32_t lport, hport;
	a_uint32_t hport_vec = 0;
	a_uint32_t mul_members;
	a_uint8_t member_byte[MV3236_MAX_MEMBERS_SIZE];
	const unsigned char mac_all_00[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	a_uint32_t lport_vec = 0;

	MacCommit_S *mac,*mac_bak;

	mul_members = 0;
	memset(member_byte, 0, MV3236_MAX_MEMBERS_SIZE);

	if(EnableFlag == 1) {

		for(i=0; i<members_size; i++) {
			member_byte[i] = shift_high_low_bit(multicast_members->members[i]);
			mul_members |= (member_byte[i] << (i*8));
		}

		/* Delete the same mac_addr list, to add new list */
		list_for_each_entry_safe(mac, mac_bak, &macCommitList, node)
		{
			if(memcmp(multicast_addr->mac, mac->ptr.mac, 6) == 0) {
				lport_vec = mac->lportvec;
			
				list_del(&mac->node);
				free(mac);			
			}
		}
		/* Disable the previous multicast port */
		for(i=0; i<MV3236_MAX_PORT_NUM-1; i++) {
			if((lport_vec >> i) & 0x01) {	
				lport = i+1;
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));

				/* Multicast frames with unknown DAs can egress from this port */
			    if((ret = cpssDxChBrgPortEgrFltUregMcastEnable(dev_id, hport, GT_FALSE)) != GT_OK) {
			        printf("Error: gprtSetDefaultForward failed\r\n");
			        return SW_FAIL;
			    }	
			}
		}

		/* Multicast input restrict condition */
		if(memcmp(multicast_addr->mac, mac_all_00, 6) == 0) {
			printf("Multicast Addr cant be 00-00-00-00-00-00\n");
			return SW_FAIL;
		}
		if((multicast_addr->mac[0] & 0x01) != 1) {
			printf("Not a Multicast_addr input\n");
			return SW_BAD_PARAM;
		}
			
		for(i=0; i<MV3236_MAX_PORT_NUM-1; i++) {
			if((mul_members >> i) & 0x01) {				
				
				lport = (i+1);
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
				/* Do not egress any frame with an unknow multicast DA on this port */
				if((ret = cpssDxChBrgPortEgrFltUregMcastEnable(dev_id, hport, GT_TRUE)) != GT_OK) {
			        printf("Error: gprtSetDefaultForward failed\r\n");
			        return SW_FAIL;
				}
				
				hport_vec |= (1 << hport);
				printf("lport_%d  ",lport);
			}
			
		}
		printf("\n");

		if((ret = multicast_mac_add(multicast_addr->mac, hport_vec)) != SW_OK) {
	        printf("Error: multicast_mac_add failed\r\n");
	        return SW_FAIL;
		}		

	    mac = (MacCommit_S *)calloc(1,sizeof(MacCommit_S));
		mac->lportvec = mul_members;
	    memcpy(&mac->ptr,multicast_addr,sizeof(fal_multicast_addr_t));    
	    list_add(&mac->node, &macCommitList);

		
	}else{
		printf("Multicast set Failed\n");
		return SW_FAIL;
	}

	return SW_OK;
}

static inline sw_error_t _mv3236_multicast_row_destroy(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	GT_STATUS ret;
	a_uint32_t i;
	a_uint32_t lport, hport;
	a_uint32_t mul_members;
	a_uint8_t member_byte[MV3236_MAX_MEMBERS_SIZE];

	MacCommit_S *mac,*mac_bak;

	mul_members = 0;
	memset(member_byte, 0, MV3236_MAX_MEMBERS_SIZE);

	if(EnableFlag == 1) {

		for(i=0; i<members_size; i++) {
			member_byte[i] = shift_high_low_bit(multicast_members->members[i]);
			mul_members |= (member_byte[i] << (i*8));
		}

		for(i=0; i<MV3236_MAX_PORT_NUM-1; i++) {
			if((mul_members >> i) & 0x01) {				

				lport = (i+1);
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
				/* Egress all frame with an unknow multicast DA on this port */
				if((ret = cpssDxChBrgPortEgrFltUregMcastEnable(dev_id, hport, GT_FALSE)) != GT_OK) {
			        printf("Error: hwSetPortRegField failed\r\n");
			        return SW_FAIL;
				}
				printf("lport_%d  ",lport);
			}			
		}
		printf("\n");

		if((ret = multicast_mac_delete(multicast_addr->mac)) != SW_OK) {
	        printf("Error: multicast_mac_delete failed\r\n");
	        return SW_FAIL;
		}

		list_for_each_entry_safe(mac, mac_bak, &macCommitList, node)
	    {
	    	if(memcmp(mac->ptr.mac, multicast_addr->mac, 6) == 0) {
				list_del(&mac->node);
				free(mac);
			}
	    }
		
		printf("Multicast row destroy\n");
	}else {
		printf("Multicast row destroy Failed\n");
		return SW_FAIL;
	}

	return SW_OK;
}

sw_error_t mv3236_multicast_enable_set(a_uint32_t dev_id, fal_enable_t enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_multicast_enable_set(dev_id, enable);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv3236_multicast_enable_get(a_uint32_t dev_id, fal_enable_t *enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_multicast_enable_get(dev_id, enable);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv3236_multicast_row_create(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_multicast_row_create(dev_id, multicast_members, multicast_addr, members_size);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv3236_multicast_row_destroy(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_multicast_row_destroy(dev_id, multicast_members, multicast_addr, members_size);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv3236_multicast_init(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);
	
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	p_api->multicast_enable_set = mv3236_multicast_enable_set;
	p_api->multicast_enable_get = mv3236_multicast_enable_get;
	p_api->multicast_row_create = mv3236_multicast_row_create;
	p_api->multicast_row_destroy = mv3236_multicast_row_destroy;
	
    return SW_OK;
}
