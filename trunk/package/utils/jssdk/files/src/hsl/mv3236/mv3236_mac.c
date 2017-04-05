#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv3236_global.h"
#include "mv3236_mac.h"
#include "mv3236_port_ctrl.h"
#include "mv3236_reg.h"
#include "cpss/generic/bridge/cpssGenBrgFdb.h"
#include "cpss/dxCh/dxChxGen/bridge/cpssDxChBrgFdb.h"

typedef struct {
	GT_U8  dev;
	CPSS_DXCH_BRG_FDB_DEL_MODE_ENT 	del_mode;
	GT_U32 actDev;
	GT_U32 actDevMask;
	GT_U16 vlanId;
	GT_U16 vlanIdMask;
	GT_U32 actUserDefined;
	GT_U32 actUserDefinedMask;
	GT_U32 trunkAgingMode;
	GT_U32 actIsTrunk;
	GT_U32 actIsTrunkMask;
	GT_U32 actTrunkPort;
	GT_U32 actTrunkPortMask;
} BRG_TRIGGER_CFG;

//extern GT_QD_DEV *pJwDev;

static a_uint32_t SecurePort[MV3236_MAX_PORT_NUM];
static a_uint32_t SecurityPortVec;		//All Security Port Summation Vector
static sw_error_t _mv3236_clear_bridge_internal_util(BRG_TRIGGER_CFG *cfg);

#define FDBTABLESIZE 16384 // reference as the value PRV_CPSS_DXCH_PP_MAC(devNum)->fineTuning.tableSize.fdb
LIST_HEAD(FdbMacList);
LIST_HEAD(FdbPortVecList);


#if 0

sw_error_t fdb_atu_int_disable()  
{
	GT_U16 data;
#if 0
	data = 0;
	if(eventSetActive(pJwDev,data) != GT_OK)
		return SW_FAIL;
#endif
	return SW_OK;
}

sw_error_t fdb_atu_int_enable()
{
	GT_U16 data;

	data = GT_ATU_PROB;
#if 0
	if(eventSetActive(pJwDev,data) != GT_OK)
		return SW_FAIL;
#endif
	return SW_OK;
}

sw_error_t fdb_atu_clear_violation(GT_QD_DEV *pJwDev, GT_ATU_OPERATION atuOp, GT_U8 *port, GT_U32 *intCause)
{
	GT_STATUS       retVal;
	GT_U16          data;
	GT_U16          opcodeData;
#if 0
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
#endif
	return SW_OK;
}

sw_error_t fdb_atu_get_violation(GT_QD_DEV *pJwDev, GT_ATU_OPERATION atuOp, GT_ATU_ENTRY *entry)
{
	GT_STATUS       retVal;
	GT_U16          data=0;
	GT_U8           i;

#if 0
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
#endif
	return SW_OK;
}
#endif

sw_error_t fdb_atu_add_mac(unsigned char hport, char *MacAddr)
{
	GT_STATUS ret;
	a_uint32_t i;
    GT_BOOL	completed = GT_FALSE;
    GT_BOOL	succeeded = GT_FALSE;
	CPSS_MAC_ENTRY_EXT_STC  macEntry;
	GT_HW_DEV_NUM hwDevNum;

	cpssDxChCfgHwDevNumGet(DEVICENUM, &hwDevNum);

    cpssOsMemSet(&macEntry, 0, sizeof(macEntry));
    macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    macEntry.key.key.macVlan.macAddr.arEther[0] = MacAddr[0];
    macEntry.key.key.macVlan.macAddr.arEther[1] = MacAddr[1];
    macEntry.key.key.macVlan.macAddr.arEther[2] = MacAddr[2];
    macEntry.key.key.macVlan.macAddr.arEther[3] = MacAddr[3];
    macEntry.key.key.macVlan.macAddr.arEther[4] = MacAddr[4];
    macEntry.key.key.macVlan.macAddr.arEther[5] = MacAddr[5];
    macEntry.key.key.macVlan.vlanId = VLANINDEX;
    macEntry.dstInterface.type = CPSS_INTERFACE_PORT_E;
    macEntry.dstInterface.devPort.hwDevNum = hwDevNum;
    macEntry.dstInterface.devPort.portNum = hport;
    macEntry.age = GT_FALSE;
    macEntry.isStatic = GT_TRUE;
    macEntry.daCommand = CPSS_MAC_TABLE_FRWRD_E;

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

sw_error_t fdb_atu_delete_mac(a_uint8_t *Mac)
{
	GT_STATUS ret;
	GT_ETHERADDR macAddress;
	CPSS_MAC_ENTRY_EXT_STC  macEntry;

    cpssOsMemSet(&macEntry, 0, sizeof(macEntry));
    macEntry.key.entryType = CPSS_MAC_ENTRY_EXT_TYPE_MAC_ADDR_E;
    macEntry.key.key.macVlan.macAddr.arEther[0] = Mac[0];
    macEntry.key.key.macVlan.macAddr.arEther[1] = Mac[1];
    macEntry.key.key.macVlan.macAddr.arEther[2] = Mac[2];
    macEntry.key.key.macVlan.macAddr.arEther[3] = Mac[3];
    macEntry.key.key.macVlan.macAddr.arEther[4] = Mac[4];
    macEntry.key.key.macVlan.macAddr.arEther[5] = Mac[5];

    macEntry.key.key.macVlan.vlanId = VLANINDEX;

	if((ret = cpssDxChBrgFdbMacEntryDelete(DEVICENUM, &(macEntry.key)) )!= GT_OK)
	{
		SW_PRINTFL(WARN,"cpssDxChBrgFdbMacEntryDelete return failed %d\r", ret);
		return SW_FAIL;
	}

	return ret;
}

sw_error_t fdb_mac_row_create(fal_port_t port_id, a_uint8_t * mac_addr)
{
    SW_PRINTFL(DETAIL,"====enter====\n");

    SW_PRINTFL(WARN,"====how to execute this requirement\n");

	return SW_OK;
}

static sw_error_t clear_bridge_internal_cfg_get(BRG_TRIGGER_CFG *cfg)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv = SW_OK;
	GT_U8 dev;

	if(cfg == NULL)
	{
		rv = SW_BAD_VALUE;
		SW_PRINTFL(WARN,"cfg is NULL %p\n",cfg);
		goto out;
	}
//
	dev = cfg->dev;

	SW_OUT_ON_ERROR_FL((rv =cpssDxChBrgFdbStaticDelEnableGet(dev, &cfg->del_mode)));

	SW_OUT_ON_ERROR_FL((rv =cpssDxChBrgFdbActionActiveDevGet(dev, &cfg->actDev, &cfg->actDevMask)));

	SW_OUT_ON_ERROR_FL((rv =cpssDxChBrgFdbActionActiveVlanGet(dev, &cfg->vlanId, &cfg->vlanIdMask)));

	//SW_OUT_ON_ERROR_FL((rv =cpssDxChBrgFdbActionActiveUserDefinedGet(dev, &cfg->actUserDefined, &cfg->actUserDefinedMask)));

	SW_OUT_ON_ERROR_FL((rv =cpssDxChBrgFdbTrunkAgingModeGet(dev, &cfg->trunkAgingMode)));

	SW_OUT_ON_ERROR_FL((rv =cpssDxChBrgFdbActionActiveInterfaceGet(dev, &cfg->actIsTrunk, &cfg->actIsTrunkMask, &cfg->actTrunkPort,&cfg->actTrunkPortMask)));

	return rv;
out:
	return rv;
}

static sw_error_t clear_bridge_internal_cfg_set(BRG_TRIGGER_CFG *cfg)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv = SW_OK;
	GT_U8 dev;

	if(cfg ==  NULL)
	{
		rv = SW_BAD_VALUE;
		SW_PRINTFL(WARN,"cfg is NULL \n");
		goto out;
	}

	dev = cfg->dev;
	SW_OUT_ON_ERROR_FL((rv =cpssDxChBrgFdbStaticDelEnable(dev, cfg->del_mode)));

	SW_OUT_ON_ERROR_FL((rv =cpssDxChBrgFdbActionActiveDevSet(dev, cfg->actDev, cfg->actDevMask)));

	SW_OUT_ON_ERROR_FL((rv =cpssDxChBrgFdbActionActiveVlanSet(dev, cfg->vlanId, cfg->vlanIdMask)));

	//SW_OUT_ON_ERROR_FL((rv =cpssDxChBrgFdbActionActiveUserDefinedSet(dev, cfg->actUserDefined, cfg->actUserDefinedMask)));

	SW_OUT_ON_ERROR_FL((rv =cpssDxChBrgFdbTrunkAgingModeSet(dev, cfg->trunkAgingMode)));

	SW_OUT_ON_ERROR_FL((rv =cpssDxChBrgFdbActionActiveInterfaceSet(dev, cfg->actIsTrunk, cfg->actIsTrunkMask, cfg->actTrunkPort,cfg->actTrunkPortMask)));

	return rv;
out:
	return rv;
}

static sw_error_t _mv3236_clear_bridge_internal_util(BRG_TRIGGER_CFG *cfg)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv = SW_OK;
	BRG_TRIGGER_CFG curCfg;
	memset(&curCfg, 0, sizeof(BRG_TRIGGER_CFG));

	SW_OUT_ON_ERROR_FL((rv = clear_bridge_internal_cfg_get(&curCfg)));

	SW_OUT_ON_ERROR_FL((rv = clear_bridge_internal_cfg_set(cfg)));

	SW_OUT_ON_ERROR_FL((rv = cpssDxChBrgFdbTrigActionStart(cfg->dev, CPSS_FDB_ACTION_DELETING_E)));

	SW_OUT_ON_ERROR_FL((rv = clear_bridge_internal_cfg_set(&curCfg)));

	return rv;

out:
	SW_PRINTFL(WARN," %s error %d \n", __FUNCTION__);
	return rv;
}

static inline sw_error_t _mv3236_mac_aging_timeout_set(a_uint32_t dev_id,fal_mac_aging_t aging_time)
{
    SW_PRINTFL(DETAIL,"====enter====\n");

	GT_STATUS status;
	a_uint32_t timeout;
	CPSS_MAC_ACTION_MODE_ENT mode;

	/*aging_time < 5 is get from wraplCpssDxChBrgFdb.c's function wrlDxChBrgFdbAgingTimeoutCheck*/
	if(aging_time > 3825 || aging_time < 5){
		return SW_BAD_PARAM;
	}
	timeout = aging_time;
	
	/*Checking of action_mode_correctness action mode*/
	if((status = cpssDxChBrgFdbMacTriggerModeGet(dev_id, &mode)) != GT_OK) {
		SW_PRINTFL(WARN, "cpssDxChBrgFdbMacTriggerModeGet Fail error %d \n", status);
		return SW_FAIL;
	}

	if(mode != CPSS_ACT_AUTO_E)
	{
		SW_PRINTFL(WARN, "mode is not auto\n");
		return SW_FAIL;
	}

	/*Setting of Mac address-table aging timeout*/
	if((status = cpssDxChBrgFdbAgingTimeoutSet(dev_id, aging_time)) != GT_OK) {
		SW_PRINTFL(WARN, "cpssDxChBrgFdbAgingTimeoutSet Fail error %d \n", status);
		return SW_FAIL;
	}

	return SW_OK;
}

static inline sw_error_t _mv3236_mac_secure_port_enable_set(a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");

	GT_STATUS ret;
	a_uint32_t hport,lport;
	FdbCommit_S *mac,*mac_bak;
	GT_BOOL  learnStatus = true;
	GT_BOOL  cpuForwardingEnabling = true;

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
		learnStatus = false;
		cpuForwardingEnabling = false;
	}
	else{
		SecurePort[hport] = 0;
		SecurityPortVec &= ~(1<<hport);
	}

	{
		GT_BOOL status;
		CPSS_PORT_LOCK_CMD_ENT cmd;

		/*Port port lock command getting*/
		if((ret = cpssDxChBrgFdbPortLearnStatusGet(dev_id, hport, &status, &cmd)) != GT_OK) {
			SW_PRINTFL(WARN, "cpssDxChBrgFdbPortLearnStatusGet, hport=%d\r\n", hport);
			return SW_FAIL;
		}

		/*Port mac adress learning and port_lock command setting.*/
		if((ret = cpssDxChBrgFdbPortLearnStatusSet(dev_id, hport, learnStatus, cmd)) != GT_OK) {
			SW_PRINTFL(WARN, "cpssDxChBrgFdbPortLearnStatusGet, hport=%d\r\n", hport);
			return SW_FAIL;
		}
		
		/*Port mac adress learning and port_lock command setting.*/
		if((ret = cpssDxChBrgFdbNaToCpuPerPortSet(dev_id, hport, cpuForwardingEnabling)) != GT_OK) {
			SW_PRINTFL(WARN, "cpssDxChBrgFdbNaToCpuPerPortSet, hport=%d\r\n", hport);
			return SW_FAIL;
		}
	}

	return SW_OK;
}

sw_error_t _mv3236_mac_row_create(a_uint32_t dev_id,fal_port_t port_id,fal_fdb_mac_t * mac_addr)
{
    SW_PRINTFL(DETAIL,"====enter====\n");

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


		//fdb_atu_int_enable();

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

static inline sw_error_t _mv3236_mac_row_destroy(a_uint32_t dev_id,fal_fdb_mac_t * mac_addr, fal_port_t port_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");

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

static inline sw_error_t _mv3236_mac_addr_table_show(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");

	GT_STATUS status;
	//GT_ATU_ENTRY MacEntry;
	char s[20];
	a_uint32_t i;
	int index;
	GT_U8 *logic_port_list, *list;
	int list_length;
	GT_U32 hport_vec;
    GT_BOOL valid  = GT_FALSE;
    GT_BOOL skip   = GT_TRUE;

    GT_BOOL  validPtr;
    GT_BOOL  skipPtr;
    GT_BOOL  agedPtr;
    GT_HW_DEV_NUM  associatedHwDevNumPtr;
    CPSS_MAC_ENTRY_EXT_STC  macEntryPtr;

	printf("\r\n");
	printf("  =======================================================\r\n");
	printf("   No.  Mac Address          EntryState         PortList \r\n");
	printf("  =======================================================\r\n");

	//memset(&MacEntry,0,sizeof(GT_ATU_ENTRY));

	index = 0;
	
	while(1) {
		//CPSS_API_CALL_LOCK();
		status =  cpssDxChBrgFdbMacEntryStatusGet(dev_id, index++, &valid,
		                                                       &skip);
		//CPSS_API_CALL_UNLOCK();

		if(status != GT_OK)
		{
			SW_PRINTFL(WARN,"cpssDxChBrgFdbMacEntryStatusGet get failed status %d \n", status);
			break;
		}

		if( ((GT_FALSE == valid) || (GT_TRUE == skip)) && (index < FDBTABLESIZE))
		{
			continue;
		}

        //CPSS_API_CALL_LOCK();
        status = cpssDxChBrgFdbMacEntryRead(dev_id, index, &validPtr, &skipPtr,
                                            &agedPtr, &associatedHwDevNumPtr,
                                            &macEntryPtr);
        //CPSS_API_CALL_UNLOCK();

        if(status != GT_OK)
       	{
        	SW_PRINTFL(WARN,"cpssDxChBrgFdbMacEntryStatusGet get failed status %d \n", status);
        	break;
       	}

		/*TODO show macEntryPtr info*/

	}

	return SW_OK;
}

static inline sw_error_t _mv3236_mac_addr_table_clear(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
#if 0
	GT_STATUS status;

	if((status = gfdbFlush(pJwDev,GT_FLUSH_ALL_UNLOCKED)) != GT_OK) {
		printf("gfdbFlush Fail\n");
		return SW_FAIL;
	}
#endif
	return SW_OK;
}

static inline sw_error_t _mv3236_mac_show_blacklist(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
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

static  sw_error_t _mv3236_mac_flush_port(a_uint32_t dev_id, fal_port_t port_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv = SW_OK;
	BRG_TRIGGER_CFG cfg;
	memset(&cfg,0, sizeof(BRG_TRIGGER_CFG));
	printf("%s.%d == \n",__FILE__,__LINE__);
	cfg.actDev = dev_id;
	cfg.actTrunkPort = port_id;
	cfg.actTrunkPortMask = port_id;
	cfg.del_mode = CPSS_DXCH_BRG_FDB_DEL_MODE_ALL_E;
	printf("%s.%d == \n",__FILE__,__LINE__);
	rv =  _mv3236_clear_bridge_internal_util(&cfg);
	return rv;
}

sw_error_t mv3236_mac_aging_timeout_set(a_uint32_t dev_id,fal_mac_aging_t aging_time)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_mac_aging_timeout_set(dev_id, aging_time);//
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv3236_mac_secure_port_enable_set(a_uint32_t dev_id,fal_port_t port_id,fal_enable_t enable)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_mac_secure_port_enable_set(dev_id, port_id, enable);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv3236_mac_row_create(a_uint32_t dev_id,fal_port_t port_id,fal_fdb_mac_t * mac_addr)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_mac_row_create(dev_id, port_id, mac_addr);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv3236_mac_row_destroy(a_uint32_t dev_id,fal_fdb_mac_t * mac_addr, fal_port_t port_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_mac_row_destroy(dev_id, mac_addr, port_id);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv3236_mac_addr_table_show(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_mac_addr_table_show(dev_id);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv3236_mac_addr_table_clear(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_mac_addr_table_clear(dev_id);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv3236_mac_show_blacklist(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_mac_show_blacklist(dev_id);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv3236_mac_flush_port(a_uint32_t dev_id, fal_port_t port_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_mac_flush_port(dev_id, port_id);
    HSL_API_UNLOCK;

	return rv;
}


sw_error_t mv3236_mac_init(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
    {
    	return SW_NOT_INITIALIZED;
    }

    HSL_DEV_ID_CHECK(dev_id);
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	p_api->mac_aging_timeout_set = mv3236_mac_aging_timeout_set;
	p_api->mac_secure_port_enable_set = mv3236_mac_secure_port_enable_set;
	p_api->mac_row_create = mv3236_mac_row_create;
	p_api->mac_row_destroy = mv3236_mac_row_destroy;
	p_api->mac_addr_table_show = mv3236_mac_addr_table_show;
	p_api->mac_addr_table_clear = mv3236_mac_addr_table_clear;
	p_api->mac_show_blacklist = mv3236_mac_show_blacklist;
	p_api->mac_flush_port = mv3236_mac_flush_port;
	
    return SW_OK;
}


