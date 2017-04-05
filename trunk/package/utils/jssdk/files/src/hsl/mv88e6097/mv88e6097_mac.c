 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv88e6097_mac.h"
#include "mv88e6097_port_ctrl.h"
#include "mv88e6097_reg.h"
#include "gtHwCntl.h"
#include "gtDrvSwRegs.h"

#include "joyware.h"

extern GT_QD_DEV *pJwDev;

static a_uint32_t SecurePort[CONFIG_JWS_PORT_TOTAL_NR+1];
static a_uint32_t SecurityPortVec;		//All Security Port Summation Vector

LIST_HEAD(FdbMacList);
LIST_HEAD(FdbPortVecList);




sw_error_t fdb_atu_int_disable()  
{
	GT_U16 data;
	GT_U32 eventType;

	/* get the previous interrupt event */
	if(eventGetActive(pJwDev,&eventType) != GT_OK)
		return SW_FAIL;
	
	/* Only disable ATU_PROB */
	data = eventType & 0xF7;
	if(eventSetActive(pJwDev,data) != GT_OK)
		return SW_FAIL;
	return SW_OK;
}

sw_error_t fdb_atu_int_enable()
{
	GT_U16 data;
	GT_U32 eventType;

	/* get the previous interrupt event */
	if(eventGetActive(pJwDev,&eventType) != GT_OK)
		return SW_FAIL;
		
	data = GT_ATU_PROB | eventType;
	if(eventSetActive(pJwDev,data) != GT_OK)
		return SW_FAIL;
	return SW_OK;
}

sw_error_t fdb_atu_clear_violation(GT_QD_DEV *pJwDev, GT_ATU_OPERATION atuOp, GT_U8 *port, GT_U32 *intCause)
{
	GT_STATUS       retVal;
	GT_U16          data;
	GT_U16          opcodeData;

	/* Wait until the ATU in ready. */
	data = 1;
	while(data == 1) {
		retVal = hwGetGlobalRegField(pJwDev,QD_REG_ATU_OPERATION,15,1,&data);
		if(retVal != GT_OK) {
			return retVal;
		}
	}

	opcodeData = 0;    
	opcodeData |= ((1 << 15) | (atuOp << 12));
	retVal = hwWriteGlobalReg(pJwDev,QD_REG_ATU_OPERATION,opcodeData);
	if(retVal != GT_OK) {
		return retVal;
	}

	/* get the Interrupt Cause */
	retVal = hwGetGlobalRegField(pJwDev,QD_REG_ATU_OPERATION,4,3,&data);
	if(retVal != GT_OK) {
		return retVal;
	}

	switch (data) {
		case 4:	/* Member Violation */
		*intCause = GT_MEMBER_VIOLATION;
		break;
		case 2:	/* Miss Violation */
		*intCause = GT_MISS_VIOLATION;
		break;
		case 1:	/* Full Violation */
		*intCause = GT_FULL_VIOLATION;
		break;
		default:
		*intCause = 0;
		return SW_OK;
	}

	retVal = hwReadGlobalReg(pJwDev,QD_REG_ATU_DATA_REG,&data);
	if(retVal != GT_OK) {
		return retVal;
	}

	*port = data & 0xF;

	return SW_OK;
}

sw_error_t fdb_atu_get_violation(GT_QD_DEV *pJwDev, GT_ATU_OPERATION atuOp, GT_ATU_ENTRY *entry)
{
	GT_STATUS       retVal;
	GT_U16          data=0;
	GT_U8           i;


	/* Wait until the ATU in ready. */
	data = 1;
	while(data == 1) {
		retVal = hwGetGlobalRegField(pJwDev,QD_REG_ATU_OPERATION,15,1,&data);
		if(retVal != GT_OK) {
			return retVal;
		}
	}

	/* If the operation is to service violation operation wait for the response   */
	if(atuOp == SERVICE_VIOLATIONS) {		
		entry->DBNum = 0;
		retVal = hwGetGlobalRegField(pJwDev,QD_REG_ATU_OPERATION,12,3,&data);
		if((retVal != GT_OK)||(data != SERVICE_VIOLATIONS)) {
			return SW_FAIL;
		}

		/* Get the Mac address  */
		for(i = 0; i < 3; i++) {
			data = 0;
			retVal = hwReadGlobalReg(pJwDev,(GT_U8)(QD_REG_ATU_MAC_BASE+i),&data);
			if(retVal != GT_OK) {
				return retVal;
			}
			entry->macAddr.arEther[2*i] = data >> 8;
			entry->macAddr.arEther[1 + 2*i] = data & 0xFF;
			hwWriteGlobalReg(pJwDev,(GT_U8)(QD_REG_ATU_MAC_BASE+i),0);
		}
	} 

	return SW_OK;
}

sw_error_t fdb_atu_add_mac(unsigned char hport, GT_ATU_UC_STATE ucState, char *MacAddr)
{
	GT_STATUS status;
	GT_ATU_ENTRY macEntry;

	memset(&macEntry,0,sizeof(GT_ATU_ENTRY));	
	macEntry.macAddr.arEther[0] = MacAddr[0];
	macEntry.macAddr.arEther[1] = MacAddr[1];
	macEntry.macAddr.arEther[2] = MacAddr[2];
	macEntry.macAddr.arEther[3] = MacAddr[3];
	macEntry.macAddr.arEther[4] = MacAddr[4];
	macEntry.macAddr.arEther[5] = MacAddr[5];
	macEntry.DBNum = 0;
	macEntry.portVec = (1 << hport);
	macEntry.prio = 0;
	macEntry.entryState.ucEntryState = ucState;

	if((status = gfdbAddMacEntry(pJwDev,&macEntry)) != GT_OK) {
		return SW_FAIL;
	}

	return SW_OK;
}

sw_error_t fdb_atu_delete_mac(a_uint8_t *Mac)
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

	return ret;
}


sw_error_t fdb_mac_row_create(fal_port_t port_id, a_uint8_t * mac_addr)
{
	GT_STATUS status;
	GT_U16 intCause;
	GT_U8 tempPort = 0;
	GT_U32 tempintCause = 0;
	GT_ATU_ENTRY atuIntStatus;
	a_uint32_t lport, hport;
	a_uint8_t SecurityMac[6];

	FdbCommit_S *mac,*mac_bak;

	//printf("ubus call fdb_mac_row_create\n");

	intCause = 0;
	if((status = eventGetIntStatus(pJwDev,&intCause)) != GT_OK) {
		printf("eventGetIntStatus Failed\n");
		return SW_FAIL;
	}
	if(intCause & GT_ATU_PROB) {
		fdb_atu_int_disable();
		if(fdb_atu_clear_violation(pJwDev, SERVICE_VIOLATIONS,&tempPort,&tempintCause) != GT_OK) {
			fdb_atu_int_enable();
			return SW_OK;
		}

		SW_RTN_ON_ERROR(map_hport2lport(tempPort, &lport));
		
		#if 0
		if(tempintCause == GT_MEMBER_VIOLATION) {
			printf("Receive an ATU Member Violation interrupt, Port=%d\r\n", lport);
		}

		if(tempintCause == GT_MISS_VIOLATION) {
			printf("Receive an Miss Violation interrupt, Port=%d\r\n", lport);
		}
		#endif

		if((tempintCause != GT_MISS_VIOLATION) && (tempintCause != GT_MEMBER_VIOLATION)){
			return SW_OK;
		}

		if(lport == 0xFF) {
			fdb_atu_int_enable();
			return SW_OK;
		}
		
		if(fdb_atu_get_violation(pJwDev, SERVICE_VIOLATIONS, &atuIntStatus) != GT_OK) {
			fdb_atu_int_enable();
			return SW_OK;
		 } 

		#if 0
		printf("receive mac (%02x:%02x:%02x:%02x:%02x:%02x)     ", 
			atuIntStatus.macAddr.arEther[0],atuIntStatus.macAddr.arEther[1],atuIntStatus.macAddr.arEther[2],
			atuIntStatus.macAddr.arEther[3],atuIntStatus.macAddr.arEther[4],atuIntStatus.macAddr.arEther[5]);
		printf("security mac (%02x:%02x:%02x:%02x:%02x:%02x) \r\n",
			mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5]);
		#endif
		
		if(atuIntStatus.macAddr.arEther[0] & 0x01) {
			fdb_atu_int_enable();
			return SW_OK;
		}
				
		//SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport));
		//SecurityPortVec |= 1<<hport;

		list_for_each_entry_safe(mac, mac_bak, &FdbMacList, node)
		{
			/* Querying for all mac list, add to mac Entry with static or dynamic */
			memset(SecurityMac, 0, 6);
			memcpy(SecurityMac, mac->ptr.mac, 6);
			SW_RTN_ON_ERROR(map_lport2hport(mac->port_id, &hport));
			//printf("MAC_ADDR    %02x:%02x:%02x:%02x:%02x:%02x\n",SecurityMac[0],SecurityMac[1],SecurityMac[2],SecurityMac[3],SecurityMac[4],SecurityMac[5]);

			/* Only add the mac which is access but can not add to mac Entry automatically */
			if(memcmp(atuIntStatus.macAddr.arEther, SecurityMac, 6) == 0) {			
				if(tempintCause == GT_MISS_VIOLATION) {	
					if((1<<tempPort) & (1<<hport)) {
						fdb_atu_add_mac(tempPort, GT_UC_STATIC, (char *)SecurityMac);
					} 
				} 
				
				if(tempintCause == GT_MEMBER_VIOLATION) {
					if((1<<tempPort) & (1<<hport)) {
						fdb_atu_add_mac(tempPort, GT_UC_STATIC, (char *)SecurityMac);
					} else {
						if(((1<<tempPort) & SecurityPortVec) == 0) {
							fdb_atu_add_mac(tempPort, GT_UC_DYNAMIC, (char *)SecurityMac);	
						}
					}
				} 						
			}
		}
		
		fdb_atu_int_enable();		
	}

	return SW_OK;
}




static inline sw_error_t _mv88e6097_mac_aging_timeout_set(a_uint32_t dev_id,fal_mac_aging_t aging_time)
{
	GT_STATUS status;
	a_uint32_t timeout;

	if(aging_time > 3825){
		return SW_BAD_PARAM;
	}
	timeout = aging_time;
	
	if((status = gfdbSetAgingTimeout(pJwDev, timeout)) != GT_OK) {
		printf("gfdbFlush Fail\n");
		return SW_FAIL;
	}

	return SW_OK;
}

static inline sw_error_t _mv88e6097_mac_secure_port_enable_set(a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable)
{
	GT_STATUS ret;
	a_uint32_t hport,lport;
	FdbCommit_S *mac,*mac_bak;

	/* cant set cpu port */
	if(port_id == 0){
		printf("cant set cpu port\n");
		return SW_FAIL;
	}
	
	lport = port_id;
	SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));

	if(enable == FAL_ENABLE) {
		SecurePort[hport] = 1;
		SecurityPortVec |= 1<<hport;
		
		/* Disable forwarding frame with unknow unicast address */
		if((ret = gprtSetForwardUnknown(pJwDev, hport, GT_FALSE)) != GT_OK) {
    		printf("Error: gprtSetForwardUnknown, hport=%d\r\n", hport);
			return SW_FAIL;
		}
		/* Set locked port, interrupt */
		if((ret = gprtSetLockedPort(pJwDev, hport, GT_TRUE)) != GT_OK) {
    		printf("Error: gprtSetLockedPort, hport=%d\r\n", hport);
			return SW_FAIL;
		}
		/* Set the drop on lock */
		if((ret = gprtSetSAFiltering(pJwDev, hport, GT_SA_DROP_ON_LOCK)) != GT_OK) {
    		printf("Error: gprtSetSAFiltering, hport=%d\r\n", hport);
			return SW_FAIL;
		}	
		/* Flush all unblocked adress on this port */
		if((ret = gfdbRemovePort(pJwDev, GT_FLUSH_ALL_UNBLK, hport)) != GT_OK) {
    		printf("Error: gfdbRemovePort, hport=%d\r\n", hport);
			return SW_FAIL;
		}

	}else {
		SecurePort[hport] = 0;
		SecurityPortVec &= ~(1<<hport);

		/* Enable forwarding frame with unknow unicast address */
		if((ret = gprtSetForwardUnknown(pJwDev, hport, GT_TRUE)) != GT_OK) {
    		printf("Error: gprtSetForwardUnknown, hport=%d\r\n", hport);
			return SW_FAIL;
		}
		/* Normal address learning will occur */
		if((ret = gprtSetLockedPort(pJwDev,hport,GT_FALSE)) != GT_OK) {
    		printf("Error: gprtSetLockedPort, hport=%d\r\n", hport);
			return SW_FAIL;
		}
		if((ret = gprtSetSAFiltering(pJwDev, hport, GT_SA_FILTERING_DISABLE)) != GT_OK) {
    		printf("Error: gprtSetSAFiltering, hport=%d\r\n", hport);
			return SW_FAIL;
		}
	

		/* destroy mac list which port is be disabled */
		list_for_each_entry_safe(mac, mac_bak, &FdbMacList, node)
		{
			if(mac->port_id == port_id) {
				mac->portVec &= ~(1<<hport);
				if((ret = fdb_atu_delete_mac(mac->ptr.mac)) != GT_OK) {
					printf("fdb_atu_delete_mac Fail\n");
					return SW_FAIL;
				}
				list_del(&mac->node);
				free(mac);			
			}
		}
		
	}
	
	return SW_OK;
}

sw_error_t _mv88e6097_mac_row_create(a_uint32_t dev_id,fal_port_t port_id,fal_fdb_mac_t * mac_addr)
{
	GT_STATUS status;
	a_uint32_t hport;
	const unsigned char mac_all_00[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	FdbCommit_S *mac,*mac_bak;
	
	/* cant set cpu port */
	if(port_id == 0){
		printf("cant set cpu port\n");
		return SW_FAIL;
	}

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport));
	
	if(SecurePort[hport] == 1) {
		/* Can not Set the Same Mac with the Same Port */
		list_for_each_entry_safe(mac, mac_bak, &FdbMacList, node)
		{
			if(memcmp(mac->ptr.mac, mac_addr->mac, 6) == 0) {
				if(mac->port_id == port_id) {
					printf("Can not Set the Same Mac with the Same Port!\n");
					return SW_FAIL;
				}
			}
		}
#if 0
		/* Use another List to Querying the Same Mac's all PortVec */
		list_for_each_entry_safe(mac, mac_bak, &FdbMacList, node)
		{
			if(memcmp(mac->ptr.mac, mac_addr, 6) == 0) {
				printf("Mac has existed !\n");
				mac->portVec |= 1<<hport;

				MacPortVec |= mac->portVec;
				printf("mac (%02x:%02x:%02x:%02x:%02x:%02x) MacPortVec = 0x%04X\n",
					mac->ptr.mac[0],mac->ptr.mac[1],mac->ptr.mac[2],mac->ptr.mac[3],mac->ptr.mac[4],mac->ptr.mac[5],MacPortVec);

				Nullflag = 1;
			}
		}

		printf("MacPortVec = 0x%04X\n",MacPortVec);

		if(Nullflag == 0) {
			fdb = (FdbPortVec_S *)calloc(1,sizeof(FdbPortVec_S));
			memcpy(&fdb->ptr, mac_addr, sizeof(fal_fdb_mac_t));
			fdb->PortVec = 1<<hport;
			printf("Unique FDB (%02x:%02x:%02x:%02x:%02x:%02x) fdb->PortVec = 0x%04X\n",
						fdb->ptr.mac[0],fdb->ptr.mac[1],fdb->ptr.mac[2],fdb->ptr.mac[3],fdb->ptr.mac[4],fdb->ptr.mac[5],fdb->PortVec);
			list_add(&fdb->node,&FdbPortVecList);
		}else if(Nullflag == 1) {
			Nullflag = 0;
			list_for_each_entry_safe(fdb, fdb_bak, &FdbPortVecList, node)
			{
				if(memcmp(fdb->ptr.mac, mac_addr, 6) == 0) {
					fdb->PortVec = MacPortVec;
					printf("FDB (%02x:%02x:%02x:%02x:%02x:%02x) fdb->PortVec = 0x%04X\n",
						fdb->ptr.mac[0],fdb->ptr.mac[1],fdb->ptr.mac[2],fdb->ptr.mac[3],fdb->ptr.mac[4],fdb->ptr.mac[5],fdb->PortVec);
				}
			}
		}
#endif

		if(memcmp(mac_addr->mac, mac_all_00, 6) == 0) {
			printf("Mac Addr cant be 00-00-00-00-00-00\n");
			return SW_FAIL;
		}
		/* Cant Set Multicast Address to FDB */
		if((mac_addr->mac[0] & 0x01) != 0) {
			printf("Cant Set Multicast Address to FDB\n");
			return SW_FAIL;
		}
		
		mac = (FdbCommit_S *)calloc(1,sizeof(FdbCommit_S));
		memcpy(&mac->ptr, mac_addr, sizeof(fal_fdb_mac_t));
		mac->port_id = port_id;
		mac->portVec = 1<<hport;

		list_add(&mac->node,&FdbMacList);


		fdb_atu_int_enable();

		/* Add to Mac Address Table */
		if((status = fdb_mac_row_create(port_id, mac_addr->mac)) != GT_OK) {
			printf("fdb_mac_row_create Fail\n");
			return SW_FAIL;
		}
	
	}else{
		printf("Secure Port Not enable\n");
		return SW_FAIL;
	}
	
	return SW_OK;
}

static inline sw_error_t _mv88e6097_mac_row_destroy(a_uint32_t dev_id,fal_fdb_mac_t * mac_addr, fal_port_t port_id)
{
	GT_STATUS status;
	a_uint32_t lport,hport;

	FdbCommit_S *mac,*mac_bak;

	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport));
	if((status = fdb_atu_delete_mac(mac_addr->mac)) != GT_OK) {
		printf("fdb_atu_delete_mac Fail\n");
		return SW_FAIL;
	}	


	list_for_each_entry_safe(mac, mac_bak, &FdbMacList, node)
	{		
		if(memcmp(mac->ptr.mac, mac_addr->mac, 6) == 0) {
			if(mac->port_id == port_id) {
				lport = mac->port_id;
			
				printf("destroy mac (%02x:%02x:%02x:%02x:%02x:%02x), lport = %d \r\n",
					mac->ptr.mac[0],mac->ptr.mac[1],mac->ptr.mac[2],mac->ptr.mac[3],mac->ptr.mac[4],mac->ptr.mac[5],lport);
				
				list_del(&mac->node);
				free(mac);		
			}
		}
	}
	
	return SW_OK;
}

static inline sw_error_t _mv88e6097_mac_addr_table_show(a_uint32_t dev_id)
{
	GT_STATUS stat;
	GT_ATU_ENTRY MacEntry;
	char s[20];
	a_uint32_t i;
	int index;
	GT_U8 *logic_port_list, *list;
	int list_length;
	GT_U32 hport_vec;
	
	printf("\r\n");
	printf("  =======================================================\r\n");
	printf("   No.  Mac Address          EntryState         PortList \r\n");
	printf("  =======================================================\r\n");

	memset(&MacEntry,0,sizeof(GT_ATU_ENTRY));

	index = 0;
	
	while(1) {

		if((stat = gfdbGetAtuEntryNext(pJwDev,&MacEntry)) != GT_OK)
			break;
		
		if((MacEntry.macAddr.arEther[0] == 0xff) && (MacEntry.macAddr.arEther[1] == 0xff) && (MacEntry.macAddr.arEther[2] == 0xff) && 
			(MacEntry.macAddr.arEther[3] == 0xff) && (MacEntry.macAddr.arEther[4] == 0xff) && (MacEntry.macAddr.arEther[5] == 0xff))
			break;	

		if((MacEntry.macAddr.arEther[0] == 0x01) && (MacEntry.macAddr.arEther[1] == 0x00) && (MacEntry.macAddr.arEther[2] == 0x5e) && 
			(MacEntry.macAddr.arEther[3] == 0x64) && (MacEntry.macAddr.arEther[4] == 0xe4) && (MacEntry.macAddr.arEther[5] == 0xe4))
			continue;

		if((MacEntry.macAddr.arEther[0] & 0x1) == 1) {	// multicast address
			switch(MacEntry.entryState.mcEntryState) {
				case GT_MC_STATIC:
				sprintf(s,"%s","Mc-Static");
				break;

				case GT_MC_PRIO_MGM_STATIC:
				sprintf(s,"%s","Mc-Static-MGMT");
				break;
				
				default:
				sprintf(s,"%s","Mc-Unkown");
				break;
			}	
		} else {										// unicast address
			switch(MacEntry.entryState.ucEntryState) {
				case GT_UC_STATIC:
				sprintf(s,"%s","Uc-Static");
				break;

				case GT_UC_TO_CPU_STATIC:
				sprintf(s,"%s","Uc-Static-ToCPU");
				break;
				
				case GT_UC_DYNAMIC:
				sprintf(s,"%s","Uc-Dynamic");
				break;
				
				default:
				sprintf(s,"%s","Uc-Unkown");
				break;
			}
		}

		logic_port_list = (GT_U8 *)malloc(pJwDev->maxPorts);
		hport_vec = pJwDev->validPortVec & MacEntry.portVec;
		list_length=0;
		for(i=0, list=logic_port_list; i<pJwDev->maxPorts; i++) {
			if((MacEntry.portVec >> i) & 0x1) {
				if(i == pJwDev->cpuPortNum)
					*list++ = 0;
				else
					SW_RTN_ON_ERROR(map_hport2lport(i, (a_uint32_t *)&(*list++)));
				list_length++;
			}
		}
		
		printf("   %03d  (%02x-%02x-%02x-%02x-%02x-%02x)  %-15s    ",
				index,
				MacEntry.macAddr.arEther[0],
				MacEntry.macAddr.arEther[1],
				MacEntry.macAddr.arEther[2],
				MacEntry.macAddr.arEther[3],
				MacEntry.macAddr.arEther[4],
				MacEntry.macAddr.arEther[5], s);

		
		for(i=0, list=logic_port_list; i<list_length; i++) {
			if(i==list_length-1) {
				if(*list == 0) {
					printf("CpuPort");
				} else {
					printf("P%d", *list);
				}
			} else {
				if(*list == 0) {
					printf("CpuPort,");
					list++;
				} else {
					printf("P%d,", *list++);
				}
			}
		}
		printf("\r\n");

		free(logic_port_list);
		logic_port_list = NULL;

		index++;
	}

	return SW_OK;
}

static inline sw_error_t _mv88e6097_mac_addr_table_clear(a_uint32_t dev_id)
{
	GT_STATUS status;
	if((status = gfdbFlush(pJwDev,GT_FLUSH_ALL_UNLOCKED)) != GT_OK) {
		printf("gfdbFlush Fail\n");
		return SW_FAIL;
	}

	return SW_OK;
}

static inline sw_error_t _mv88e6097_mac_show_blacklist(a_uint32_t dev_id)
{

#if 0

	GT_STATUS status;
	FdbCommit_S *mac,*mac_bak;
	a_uint32_t num;

	if(list_empty(&FdbMacList) == 1){
		printf("\n\r");
		printf("No BlackList Mac\n");		
		printf("\n\r");
		return SW_OK;
	}

	num = 0;
	list_for_each_entry_safe(mac, mac_bak, &FdbMacList, node)
	{
		num++;
		if(mac->permission == FAL_MAC_BLACKLIST) {
			printf("   %03d   %02x-%02x-%02x-%02x-%02x-%02x   P%d \r\n", num,
				mac->ptr.mac[0],mac->ptr.mac[1],mac->ptr.mac[2],mac->ptr.mac[3],mac->ptr.mac[4],mac->ptr.mac[5],mac->port_id);	
		}
	}
#endif

	printf("Not Support This Feature\n");

	return SW_OK;
}

static inline sw_error_t _mv88e6097_mac_flush_port(a_uint32_t dev_id, fal_port_t port_id)
{
	GT_STATUS status;
	a_uint32_t hport;
	
	SW_RTN_ON_ERROR(map_lport2hport(port_id, &hport));
	
	if((status = gfdbRemovePort(pJwDev,GT_MOVE_ALL_UNLOCKED,hport)) != GT_OK) {
		printf("gfdbRemovePort Fail\n");
		return SW_FAIL;
	}
	return SW_OK;
}

static inline sw_error_t _mv88e6097_8021x_port_auth_set(a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable)
{
	GT_STATUS ret;
	a_uint32_t hport,lport;

	/* cant set cpu port */
	if(port_id == 0){
		printf("cant set cpu port\n");
		return SW_FAIL;
	}
	
	lport = port_id;
	SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));

	if(enable == FAL_ENABLE) {
		/* Disable forwarding frame with unknow unicast address */
		if((ret = gprtSetForwardUnknown(pJwDev, hport, GT_FALSE)) != GT_OK) {
    		printf("Error: gprtSetForwardUnknown, hport=%d\r\n", hport);
			return SW_FAIL;
		}
		/* Set locked port, interrupt */
		if((ret = gprtSetLockedPort(pJwDev, hport, GT_TRUE)) != GT_OK) {
    		printf("Error: gprtSetLockedPort, hport=%d\r\n", hport);
			return SW_FAIL;
		}
		/* Set the drop on lock */
		if((ret = gprtSetSAFiltering(pJwDev, hport, GT_SA_DROP_ON_LOCK)) != GT_OK) {
    		printf("Error: gprtSetSAFiltering, hport=%d\r\n", hport);
			return SW_FAIL;
		}	
		/* Flush all unblocked adress on this port */
		if((ret = gfdbRemovePort(pJwDev, GT_FLUSH_ALL_UNBLK, hport)) != GT_OK) {
    		printf("Error: gfdbRemovePort, hport=%d\r\n", hport);
			return SW_FAIL;
		}
	}else {
		/* Enable forwarding frame with unknow unicast address */
		if((ret = gprtSetForwardUnknown(pJwDev, hport, GT_TRUE)) != GT_OK) {
    		printf("Error: gprtSetForwardUnknown, hport=%d\r\n", hport);
			return SW_FAIL;
		}
		/* Normal address learning will occur */
		if((ret = gprtSetLockedPort(pJwDev,hport,GT_FALSE)) != GT_OK) {
    		printf("Error: gprtSetLockedPort, hport=%d\r\n", hport);
			return SW_FAIL;
		}
		if((ret = gprtSetSAFiltering(pJwDev, hport, GT_SA_FILTERING_DISABLE)) != GT_OK) {
    		printf("Error: gprtSetSAFiltering, hport=%d\r\n", hport);
			return SW_FAIL;
		}
	}

	return SW_OK;
}

static inline sw_error_t _mv88e6097_8021x_mac_bind(a_uint32_t dev_id,fal_port_t port_id,fal_atu_status atu_status,fal_fdb_mac_t * macAddress)
{
	GT_STATUS status;
	GT_ATU_ENTRY macEntry;
	a_uint32_t hport,lport;
	const unsigned char mac_all_00[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	/* cant set cpu port */
	if(port_id == 0){
		printf("cant set cpu port\n");
		return SW_FAIL;
	}
	if(memcmp(macAddress->mac, mac_all_00, 6) == 0) {
		printf("Mac Addr cant be 00-00-00-00-00-00\n");
		return SW_FAIL;
	}
	/* Cant Set Multicast Address to FDB */
	if((macAddress->mac[0] & 0x01) != 0) {
		printf("Cant Set Multicast Address to FDB\n");
		return SW_FAIL;
	}

	lport = port_id;
	SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));

	memset(&macEntry,0,sizeof(GT_ATU_ENTRY));	
	macEntry.macAddr.arEther[0] = macAddress->mac[0];
	macEntry.macAddr.arEther[1] = macAddress->mac[1];
	macEntry.macAddr.arEther[2] = macAddress->mac[2];
	macEntry.macAddr.arEther[3] = macAddress->mac[3];
	macEntry.macAddr.arEther[4] = macAddress->mac[4];
	macEntry.macAddr.arEther[5] = macAddress->mac[5];
	macEntry.DBNum = 0;
	macEntry.portVec = (1 << hport);
	macEntry.prio = 0;
	macEntry.entryState.ucEntryState = atu_status;

	if((status = gfdbAddMacEntry(pJwDev,&macEntry)) != GT_OK) {
		return SW_FAIL;
	}

	return SW_OK;
}





sw_error_t mv88e6097_mac_aging_timeout_set(a_uint32_t dev_id,fal_mac_aging_t aging_time)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mac_aging_timeout_set(dev_id, aging_time);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_mac_secure_port_enable_set(a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mac_secure_port_enable_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_mac_row_create(a_uint32_t dev_id,fal_port_t port_id,fal_fdb_mac_t * mac_addr)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mac_row_create(dev_id, port_id, mac_addr);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_mac_row_destroy(a_uint32_t dev_id,fal_fdb_mac_t * mac_addr, fal_port_t port_id)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mac_row_destroy(dev_id, mac_addr, port_id);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_mac_addr_table_show(a_uint32_t dev_id)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mac_addr_table_show(dev_id);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_mac_addr_table_clear(a_uint32_t dev_id)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mac_addr_table_clear(dev_id);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_mac_show_blacklist(a_uint32_t dev_id)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mac_show_blacklist(dev_id);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_mac_flush_port(a_uint32_t dev_id, fal_port_t port_id)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_mac_flush_port(dev_id, port_id);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_8021x_port_auth_set(a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_8021x_port_auth_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_8021x_mac_bind(a_uint32_t dev_id,fal_port_t port_id,fal_atu_status atu_status,fal_fdb_mac_t * macAddress)
{
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_8021x_mac_bind(dev_id, port_id, atu_status, macAddress);
    HSL_API_UNLOCK;

	return rv;
}


sw_error_t mv88e6097_mac_init(a_uint32_t dev_id)
{
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);

    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	p_api->mac_aging_timeout_set = mv88e6097_mac_aging_timeout_set;
	p_api->mac_secure_port_enable_set = mv88e6097_mac_secure_port_enable_set;
	p_api->mac_row_create = mv88e6097_mac_row_create;
	p_api->mac_row_destroy = mv88e6097_mac_row_destroy;
	p_api->mac_addr_table_show = mv88e6097_mac_addr_table_show;
	p_api->mac_addr_table_clear = mv88e6097_mac_addr_table_clear;
	p_api->mac_show_blacklist = mv88e6097_mac_show_blacklist;
	p_api->mac_flush_port = mv88e6097_mac_flush_port;
	p_api->ieee8021x_port_auth_set = mv88e6097_8021x_port_auth_set;
	p_api->ieee8021x_mac_bind = mv88e6097_8021x_mac_bind;
	
    return SW_OK;
}


