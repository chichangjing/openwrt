 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv88e6097_multicast.h"
#include "mv88e6097_port_ctrl.h"
#include "mv88e6097_reg.h"
#include "msApi.h"
#include "gtHwCntl.h"
#include "gtDrvSwRegs.h"
#include "msJwSwitch.h"

#include "joyware.h"


extern GT_QD_DEV *pJwDev;
static a_uint32_t EnableFlag = 0;

static LIST_HEAD(macCommitList);


sw_error_t multicast_mac_add(a_uint8_t *Mac, a_uint32_t hport_vec)
{
	GT_STATUS ret;
	GT_ATU_ENTRY macEntry;

	memset(&macEntry, 0, sizeof(GT_ATU_ENTRY));
	macEntry.macAddr.arEther[0] = Mac[0];
	macEntry.macAddr.arEther[1] = Mac[1];
	macEntry.macAddr.arEther[2] = Mac[2];
	macEntry.macAddr.arEther[3] = Mac[3];
	macEntry.macAddr.arEther[4] = Mac[4];
	macEntry.macAddr.arEther[5] = Mac[5];	

	macEntry.DBNum = 0;
	macEntry.portVec = hport_vec;
	macEntry.prio = 0;
	macEntry.entryState.mcEntryState = GT_MC_STATIC;

	if((ret = gfdbAddMacEntry(pJwDev,&macEntry)) != GT_OK) {
		printf("gfdbAddMacEntry return failed\r\n");
		return SW_FAIL;
	}

	printf("Mcast = %02x-%02x-%02x-%02x-%02x-%02x\n",macEntry.macAddr.arEther[0],macEntry.macAddr.arEther[1],macEntry.macAddr.arEther[2],macEntry.macAddr.arEther[3],macEntry.macAddr.arEther[4],macEntry.macAddr.arEther[5]);
	
	return ret;
}

sw_error_t multicast_mac_delete(a_uint8_t *Mac)
{
	GT_STATUS ret;
	GT_ETHERADDR macAddress;
	
	memset(&macAddress, 0, sizeof(GT_ETHERADDR));
	
	macAddress.arEther[0] = Mac[0];
	macAddress.arEther[1] = Mac[1];
	macAddress.arEther[2] = Mac[2];
	macAddress.arEther[3] = Mac[3];
	macAddress.arEther[4] = Mac[4];
	macAddress.arEther[5] = Mac[5];

	if((ret = gfdbDelMacEntry(pJwDev,&macAddress)) != GT_OK) {
		printf("gfdbAddMacEntry return failed\r\n");
		return SW_FAIL;
	}

	printf("Mcast = %02x-%02x-%02x-%02x-%02x-%02x\n",macAddress.arEther[0],macAddress.arEther[1],macAddress.arEther[2],macAddress.arEther[3],macAddress.arEther[4],macAddress.arEther[5]);

	return ret;
}

static inline sw_error_t _mv88e6097_multicast_enable_set(a_uint32_t dev_id, fal_enable_t enable)
{
	a_uint32_t i;
	a_uint32_t lport, hport;
	GT_STATUS ret;
	MacCommit_S *mac,*mac_bak;
	
	if(enable == FAL_ENABLE) {
		EnableFlag = 1;
		printf("Multicast feature Enable\n");
	} else {
		EnableFlag = 0;
				
		for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {
			lport = i+1;
			SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
			
			/* Multicast frames with unknown DAs can egress from this port */
		    if((ret = gprtSetDefaultForward(pJwDev, hport, GT_TRUE)) != GT_OK) {
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

static inline sw_error_t _mv88e6097_multicast_enable_get(a_uint32_t dev_id, fal_enable_t *enable)
{
	GT_STATUS ret;
	a_uint32_t i,hport,lport;
	GT_BOOL data;
	a_uint32_t num;

	num = 0;
	for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {			
		lport = (i+1);
		SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
		if((ret = gprtGetDefaultForward(pJwDev, hport, &data)) != GT_OK) {
			printf("Error: gprtGetDefaultForward failed\r\n");
			return SW_FAIL;
		}
		/* multicast port num */
		if(data == GT_FALSE){
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

static inline sw_error_t _mv88e6097_multicast_row_create(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size)
{
	GT_STATUS ret;
	a_uint32_t i;
	a_uint32_t lport, hport;
	a_uint32_t hport_vec = 0;
	a_uint32_t mul_members;
	a_uint8_t member_byte[MV88E6097_MAX_MEMBERS_SIZE];
	const unsigned char mac_all_00[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	a_uint32_t lport_vec = 0;

	MacCommit_S *mac,*mac_bak;

	mul_members = 0;
	memset(member_byte, 0, MV88E6097_MAX_MEMBERS_SIZE);

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
		for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {
			if((lport_vec >> i) & 0x01) {	
				lport = i+1;
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));

				/* Multicast frames with unknown DAs can egress from this port */
			    if((ret = gprtSetDefaultForward(pJwDev, hport, GT_TRUE)) != GT_OK) {
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
			
		for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {
			if((mul_members >> i) & 0x01) {				
				
				lport = (i+1);
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
				/* Do not egress any frame with an unknow multicast DA on this port */
				if((ret = gprtSetDefaultForward(pJwDev, hport, GT_FALSE)) != GT_OK) {
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

		printf("Multicast row create\n");
	}else{
		printf("Multicast set Failed, Multicast admin mode Not enable\n");
		return SW_FAIL;
	}
	
	return SW_OK;
}

static inline sw_error_t _mv88e6097_multicast_row_destroy(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size)
{
	GT_STATUS ret;
	a_uint32_t i;
	a_uint32_t lport, hport;
	a_uint32_t mul_members;
	a_uint8_t member_byte[MV88E6097_MAX_MEMBERS_SIZE];

	MacCommit_S *mac,*mac_bak;

	mul_members = 0;
	memset(member_byte, 0, MV88E6097_MAX_MEMBERS_SIZE);

	if(EnableFlag == 1) {

		for(i=0; i<members_size; i++) {
			member_byte[i] = shift_high_low_bit(multicast_members->members[i]);
		//	printf("%s, members_byte[%d] = %02x\n",__FUNCTION__,i,member_byte[i]);
			mul_members |= (member_byte[i] << (i*8));
		}

	//	printf("%s, mul_members = %04x, i=%d\n",__FUNCTION__,mul_members,i);

		for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {
			if((mul_members >> i) & 0x01) {				

				lport = (i+1);
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
				/* Egress all frame with an unknow multicast DA on this port */
				if((ret = gprtSetDefaultForward(pJwDev, hport, GT_TRUE)) != GT_OK) {
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
				if(mul_members == 0){
					for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {
						if((mac->lportvec >> i) & 0x01) {				
							lport = (i+1);
							SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
							/* Egress all frame with an unknow multicast DA on this port */
							if((ret = gprtSetDefaultForward(pJwDev, hport, GT_TRUE)) != GT_OK) {
						        printf("Error: hwSetPortRegField failed\r\n");
						        return SW_FAIL;
							}
							printf("lport_%d  ",lport);
						}			
					}
					printf("\n");
				}
				
				list_del(&mac->node);
				free(mac);
			}
	    }
		
		printf("Multicast row destroy\n");
	}else {
		printf("Multicast row destroy Failed, Multicast admin mode Not enable\n");
		return SW_FAIL;
	}

	return SW_OK;
}

static inline sw_error_t _mv88e6097_mgmt_frame_set(a_uint32_t dev_id,fal_multicast_addr_t *mgmt_addr,fal_enable_t enable)
{
	a_uint8_t LldpMac[6] = {0x01,0x80,0xC2,0x00,0x00,0x0E};
	a_uint8_t RstpMac[6] = {0x01,0x80,0xC2,0x00,0x00,0x00};
	a_uint8_t JrppdMac[6] = {0x0D,0xA4,0x2A,0x00,0x00,0x05};
	a_uint8_t Auth8021x[6] = {0x01,0x80,0xC2,0x00,0x00,0x03};
	GT_U16 enBits;
	GT_ATU_ENTRY atuEntry;
	GT_ETHERADDR macAddr;
	
	
	if(enable == FAL_ENABLE) {
		if(memcmp(LldpMac, mgmt_addr->mac, 6) == 0) {
			/* Lldp Mgmt Multicast */
			if(gsysGetRsvd2CpuEnables(pJwDev,&enBits) != GT_OK) {
				printf("gsysSetRsvd2CpuEnables failed\n");
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
		} else if(memcmp(RstpMac, mgmt_addr->mac, 6) == 0) {
			/* Rstp Mgmt Multicast */
			if(gsysGetRsvd2CpuEnables(pJwDev,&enBits) != GT_OK) {
				printf("gsysSetRsvd2CpuEnables failed\n");
	            return SW_FAIL;
	        }
			enBits |= 0x0001; /* Enable 01-80-C2-00-00-00 */
	    	if(gsysSetRsvd2CpuEnables(pJwDev,enBits) != GT_OK) {
				printf("gsysSetRsvd2CpuEnables failed\n");
	            return SW_FAIL;
	        }
			/* Enable detect MGMT frames */
	    	if(gsysSetRsvd2Cpu(pJwDev,GT_TRUE) != GT_OK) {
				printf("gsysSetRsvd2Cpu failed\n");
	            return SW_FAIL;
	        }
		} else if(memcmp(JrppdMac, mgmt_addr->mac, 6) == 0) {		 
			/* Obring Mgmt Multicast */
			memset(&atuEntry,0,sizeof(GT_ATU_ENTRY));	
			/* Set the Atu entry parameters.    */
	        atuEntry.macAddr.arEther[0] = 0x0D;
	        atuEntry.macAddr.arEther[1] = 0xA4;
	        atuEntry.macAddr.arEther[2] = 0x2A;
	        atuEntry.macAddr.arEther[3] = 0x00;
	        atuEntry.macAddr.arEther[4] = 0x00;
	        atuEntry.macAddr.arEther[5] = 0x05;
			atuEntry.DBNum = 0;
			atuEntry.portVec = (1<<pJwDev->cpuPortNum);
			atuEntry.prio = 7;
			atuEntry.entryState.mcEntryState = GT_MC_PRIO_MGM_STATIC;

			if(gfdbAddMacEntry(pJwDev,&atuEntry) != GT_OK) {
				printf("gfdbAddMacEntry return failed\r\n");
				return SW_FAIL;
			}
		}else if(memcmp(Auth8021x, mgmt_addr->mac, 6) == 0) {
			/* 8021x Mgmt Multicast */
			if(gsysGetRsvd2CpuEnables(pJwDev,&enBits) != GT_OK) {
				printf("gsysSetRsvd2CpuEnables failed\n");
	            return SW_FAIL;
	        }
			enBits |= 0x0008; /* Enable 01-80-C2-00-00-03 */
	    	if(gsysSetRsvd2CpuEnables(pJwDev,enBits) != GT_OK) {
				printf("gsysSetRsvd2CpuEnables failed\n");
	            return SW_FAIL;
	        }
			/* Enable detect MGMT frames */
	    	if(gsysSetRsvd2Cpu(pJwDev,GT_TRUE) != GT_OK) {
				printf("gsysSetRsvd2Cpu failed\n");
	            return SW_FAIL;
	        }
		} else{
			return SW_OK;
		}
		printf("MGMT frame %02x-%02x-%02x-%02x-%02x-%02x enable\n",mgmt_addr->mac[0],mgmt_addr->mac[1],mgmt_addr->mac[2],mgmt_addr->mac[3],mgmt_addr->mac[4],mgmt_addr->mac[5]);
	}
	else if(enable == FAL_DISABLE) {
		if(memcmp(LldpMac, mgmt_addr->mac, 6) == 0) {
			/* Lldp Mgmt Multicast */
			if(gsysGetRsvd2CpuEnables(pJwDev,&enBits) != GT_OK) {
				printf("gsysSetRsvd2CpuEnables failed\n");
	            return SW_FAIL;
	        }
	    	enBits &= ~0x4000; /* Disable 01-80-C2-00-00-0E */
	    	if(gsysSetRsvd2CpuEnables(pJwDev,enBits) != GT_OK) {
				printf("gsysSetRsvd2CpuEnables failed\n");
	            return SW_FAIL;
	        }
			/* Enable detect MGMT frames */
	    	if(gsysSetRsvd2Cpu(pJwDev,GT_TRUE) != GT_OK) {
				printf("gsysSetRsvd2Cpu failed\n");
	            return SW_FAIL;
	        }
		} else if(memcmp(RstpMac, mgmt_addr->mac, 6) == 0) {
			/* Rstp Mgmt Multicast */
			if(gsysGetRsvd2CpuEnables(pJwDev,&enBits) != GT_OK) {
				printf("gsysSetRsvd2CpuEnables failed\n");
	            return SW_FAIL;
	        }
			enBits &= ~0x0001; /* Disable 01-80-C2-00-00-00 */
	    	if(gsysSetRsvd2CpuEnables(pJwDev,enBits) != GT_OK) {
				printf("gsysSetRsvd2CpuEnables failed\n");
	            return SW_FAIL;
	        }
			/* Enable detect MGMT frames */
	    	if(gsysSetRsvd2Cpu(pJwDev,GT_TRUE) != GT_OK) {
				printf("gsysSetRsvd2Cpu failed\n");
	            return SW_FAIL;
	        }
		} else if(memcmp(JrppdMac, mgmt_addr->mac, 6) == 0) {		 
			/* Obring Mgmt Multicast */
			memset(&macAddr,0,sizeof(GT_ETHERADDR));	
			memcpy(macAddr.arEther, JrppdMac, 6);

			if(gfdbDelMacEntry(pJwDev,&macAddr) != GT_OK) {
				printf("gfdbAddMacEntry return failed\r\n");
				return SW_FAIL;
			}
		} else if(memcmp(Auth8021x, mgmt_addr->mac, 6) == 0) {
			/* 8021x Mgmt Multicast */
			if(gsysGetRsvd2CpuEnables(pJwDev,&enBits) != GT_OK) {
				printf("gsysSetRsvd2CpuEnables failed\n");
	            return SW_FAIL;
	        }
			enBits &= ~0x0008; /* Disable 01-80-C2-00-00-00 */
	    	if(gsysSetRsvd2CpuEnables(pJwDev,enBits) != GT_OK) {
				printf("gsysSetRsvd2CpuEnables failed\n");
	            return SW_FAIL;
	        }
			/* Enable detect MGMT frames */
	    	if(gsysSetRsvd2Cpu(pJwDev,GT_TRUE) != GT_OK) {
				printf("gsysSetRsvd2Cpu failed\n");
	            return SW_FAIL;
	        }
		} else {
			return SW_OK;
		}

		if(gsysGetRsvd2CpuEnables(pJwDev,&enBits) != GT_OK) {
			printf("gsysSetRsvd2CpuEnables failed\n");
            return SW_FAIL;
        }
		if(enBits == 0) {
			/* Disable detect MGMT frames */
	    	if(gsysSetRsvd2Cpu(pJwDev,GT_FALSE) != GT_OK) {
				printf("gsysSetRsvd2Cpu failed\n");
	            return SW_FAIL;
	        }	
		}
		printf("MGMT frame %02x-%02x-%02x-%02x-%02x-%02x disable\n",mgmt_addr->mac[0],mgmt_addr->mac[1],mgmt_addr->mac[2],mgmt_addr->mac[3],mgmt_addr->mac[4],mgmt_addr->mac[5]);
	}
	return SW_OK;
}





sw_error_t mv88e6097_multicast_enable_set(a_uint32_t dev_id, fal_enable_t enable)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_multicast_enable_set(dev_id, enable);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_multicast_enable_get(a_uint32_t dev_id, fal_enable_t *enable)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_multicast_enable_get(dev_id, enable);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_multicast_row_create(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_multicast_row_create(dev_id, multicast_members, multicast_addr, members_size);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_multicast_row_destroy(a_uint32_t dev_id, fal_multicast_members_t *multicast_members, fal_multicast_addr_t *multicast_addr, a_uint32_t members_size)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_multicast_row_destroy(dev_id, multicast_members, multicast_addr, members_size);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_mgmt_frame_set(a_uint32_t dev_id,fal_multicast_addr_t *mgmt_addr,fal_enable_t enable)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mgmt_frame_set(dev_id, mgmt_addr, enable);
    HSL_API_UNLOCK;

	return rv;
}





sw_error_t mv88e6097_multicast_init(a_uint32_t dev_id)
{
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);
	
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	p_api->multicast_enable_set = mv88e6097_multicast_enable_set;
	p_api->multicast_enable_get = mv88e6097_multicast_enable_get;
	p_api->multicast_row_create = mv88e6097_multicast_row_create;
	p_api->multicast_row_destroy = mv88e6097_multicast_row_destroy;
	p_api->mgmt_frame_set = mv88e6097_mgmt_frame_set;
	
    return SW_OK;
}


