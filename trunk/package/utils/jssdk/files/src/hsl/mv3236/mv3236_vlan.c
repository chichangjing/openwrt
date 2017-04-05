 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv3236_global.h"
#include "mv3236_vlan.h"
#include "mv3236_port_ctrl.h"
#include "mv3236_reg.h"
#include "cpss/dxCh/dxChxGen/bridge/cpssDxChBrgVlan.h"

//#include "msApi.h"
//#include "gtHwCntl.h"
//#include "gtDrvSwRegs.h"

//extern GT_QD_DEV *pJwDev;

typedef enum{
	port_based_vlan = 1,
	ieee8021q_vlan =2,
}VLAN_MODE;

static a_uint32_t vlan_mode;
static a_uint8_t  Unique_UT_Port[MV3236_MAX_PORT_NUM];
static a_uint32_t DestroyFlag[4096];
static a_uint32_t DefaultVlanSetting[MV3236_MAX_PORT_NUM];
//= { MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED,
//															       MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED,
//															        MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED };

#define DEFAULT_PVID	1
#define DEFAULT_VID     1

sw_error_t mv3236_vlan_clean(a_uint32_t dev_id, a_uint16_t vid)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    GT_STATUS           rc = GT_OK;
    CPSS_PORTS_BMP_STC  portsMembers;
    CPSS_PORTS_BMP_STC  portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC  cpssVlanInfo;   /* cpss vlan info format    */
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd; /* ports tagging command */
    GT_BOOL             isValidPtr;
    /* Write default VLAN entry */
    rc = cpssDxChBrgVlanEntryRead(dev_id, vid,
                                   &portsMembers,
                                   &portsTagging,
                                   &cpssVlanInfo,
                                   &isValidPtr,
                                   &portsTaggingCmd);
    if(rc != GT_OK)
    {
    	printf("%s.%d cpssDxChBrgVlanEntryRead failed %d \n", __FILE__,__LINE__,rc);
    	return SW_FAIL;
    }

    memset(&portsMembers, 0, sizeof(CPSS_PORTS_BMP_STC));

    /* Write default VLAN entry */
    rc = cpssDxChBrgVlanEntryWrite(dev_id, vid,
                                   &portsMembers,
                                   &portsTagging,
                                   &cpssVlanInfo,
                                   &portsTaggingCmd);
    if(rc != GT_OK)
    {
    	printf("%s.%d cpssDxChBrgVlanEntryWrite failed %d \n",  __FILE__,__LINE__,rc);
    	return SW_FAIL;
    }

    return SW_OK;

}

sw_error_t mv3236_vlan_set(a_uint32_t dev_id, a_uint16_t vid, a_uint8_t *vlan_setting)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    GT_STATUS           rc = GT_OK;
    CPSS_PORTS_BMP_STC  portsMembers;
    CPSS_PORTS_BMP_STC  portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC  cpssVlanInfo;   /* cpss vlan info format    */
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd; /* ports tagging command */
    GT_U32              port, tport;           /* current port number      */

    /* Fill Vlan info */
    osMemSet(&cpssVlanInfo, 0, sizeof(cpssVlanInfo));
    /* default IP MC VIDX */
    cpssVlanInfo.unregIpmEVidx = 0xFFF;

    cpssVlanInfo.naMsgToCpuEn           = GT_TRUE;

    cpssVlanInfo.autoLearnDisable       = GT_TRUE; /* Disable auto learn on VLAN , FIXME*/

    cpssVlanInfo.unkUcastCmd            = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregIpv6McastCmd      = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregIpv4McastCmd      = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregNonIpMcastCmd     = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregNonIpv4BcastCmd   = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregIpv4BcastCmd      = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.floodVidx              = 0xFFF;

    cpssVlanInfo.mirrToRxAnalyzerIndex = 0;
    cpssVlanInfo.mirrToTxAnalyzerEn = GT_FALSE;
    cpssVlanInfo.mirrToTxAnalyzerIndex = 0;
    cpssVlanInfo.fidValue = vid;
    cpssVlanInfo.unknownMacSaCmd = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.ipv4McBcMirrToAnalyzerEn = GT_FALSE;
    cpssVlanInfo.ipv4McBcMirrToAnalyzerIndex = 0;
    cpssVlanInfo.ipv6McMirrToAnalyzerEn = GT_FALSE;
    cpssVlanInfo.ipv6McMirrToAnalyzerIndex = 0;

    /* Fill ports and tagging members */
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsMembers);
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsTagging);
    osMemSet(&portsTaggingCmd, 0, sizeof(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC));

    /* set all ports as VLAN members */
    for (port = 0; port < MV3236_MAX_PORT_NUM - 1; port++)
    {
    	if(vlan_setting[port] == -1)
    	{
    		continue;
    	}

    	SW_RTN_ON_ERROR(map_lport2hport(port+1, &tport));

        //CPSS_ENABLER_PORT_SKIP_CHECK(dev_id, tport);

        /* set the port as member of vlan */
        CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembers, tport);
#if 0
        /* Set port pvid */
        rc = cpssDxChBrgVlanPortVidSet(dev, port, CPSS_DIRECTION_INGRESS_E,vid);
        if(rc != GT_OK)
        {
        	printf("%s.%d cpssDxChBrgVlanPortVidSet failed %d \n", rc);
        	return SW_FAIL;
        }
#endif
        portsTaggingCmd.portsCmd[tport] = vlan_setting[port];
    }

    /* Write default VLAN entry */
    rc = cpssDxChBrgVlanEntryWrite(dev_id, vid,
                                   &portsMembers,
                                   &portsTagging,
                                   &cpssVlanInfo,
                                   &portsTaggingCmd);
    if(rc != GT_OK)
    {
    	printf("%s.%d cpssDxChBrgVlanEntryWrite failed %d \n", rc);
    	return SW_FAIL;
    }

	return SW_OK;
}

#if 0
/*********************************************************/
/* Default VLAN configuration: VLAN 1 contains all ports */
/*********************************************************/
sw_error_t mv3236_default_vlan_set(a_uint32_t dev_id, a_uint8_t *defautsetting)
{
    GT_STATUS           rc = GT_OK;
    CPSS_PORTS_BMP_STC  portsMembers;
    CPSS_PORTS_BMP_STC  portsTagging;
    CPSS_DXCH_BRG_VLAN_INFO_STC  cpssVlanInfo;   /* cpss vlan info format    */
    CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC portsTaggingCmd; /* ports tagging command */
    GT_U32              port;           /* current port number      */
    GT_U16              vid;            /* default VLAN id          */

    /* default VLAN id is 1 */
    vid = 1;

    /* Fill Vlan info */
    osMemSet(&cpssVlanInfo, 0, sizeof(cpssVlanInfo));
    /* default IP MC VIDX */
    cpssVlanInfo.unregIpmEVidx = 0xFFF;

    cpssVlanInfo.naMsgToCpuEn           = GT_TRUE;

    if(appDemoSysConfig.forceAutoLearn == GT_FALSE)
    {
        cpssVlanInfo.autoLearnDisable       = GT_TRUE; /* Disable auto learn on VLAN */
    }

    cpssVlanInfo.unkUcastCmd            = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregIpv6McastCmd      = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregIpv4McastCmd      = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregNonIpMcastCmd     = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregNonIpv4BcastCmd   = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.unregIpv4BcastCmd      = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.floodVidx              = 0xFFF;

    cpssVlanInfo.mirrToRxAnalyzerIndex = 0;
    cpssVlanInfo.mirrToTxAnalyzerEn = GT_FALSE;
    cpssVlanInfo.mirrToTxAnalyzerIndex = 0;
    cpssVlanInfo.fidValue = vid;
    cpssVlanInfo.unknownMacSaCmd = CPSS_PACKET_CMD_FORWARD_E;
    cpssVlanInfo.ipv4McBcMirrToAnalyzerEn = GT_FALSE;
    cpssVlanInfo.ipv4McBcMirrToAnalyzerIndex = 0;
    cpssVlanInfo.ipv6McMirrToAnalyzerEn = GT_FALSE;
    cpssVlanInfo.ipv6McMirrToAnalyzerIndex = 0;

    /* Fill ports and tagging members */
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsMembers);
    CPSS_PORTS_BMP_PORT_CLEAR_ALL_MAC(&portsTagging);
    osMemSet(&portsTaggingCmd, 0, sizeof(CPSS_DXCH_BRG_VLAN_PORTS_TAG_CMD_STC));

    /* set all ports as VLAN members */
    for (port = 0; port < MV3236_MAX_PORT_NUM; port++)
    {
        CPSS_ENABLER_PORT_SKIP_CHECK(dev,port);

        /* set the port as member of vlan */
        CPSS_PORTS_BMP_PORT_SET_MAC(&portsMembers, port);

        /* Set port pvid */
        rc = cpssDxChBrgVlanPortVidSet(dev, port, CPSS_DIRECTION_INGRESS_E,vid);
        if(rc != GT_OK)
        {
        	printf("%s.%d cpssDxChBrgVlanPortVidSet failed %d \n", rc);
        	return SW_FAIL;
        }

        portsTaggingCmd.portsCmd[port] = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
    }

    /* Write default VLAN entry */
    rc = cpssDxChBrgVlanEntryWrite(dev, vid,
                                   &portsMembers,
                                   &portsTagging,
                                   &cpssVlanInfo,
                                   &portsTaggingCmd);
    if(rc != GT_OK)
    {
    	printf("%s.%d cpssDxChBrgVlanEntryWrite failed %d \n", rc);
    	return SW_FAIL;
    }

	return SW_OK;
}
#endif

static inline sw_error_t _mv3236_vlan_mode_set(a_uint32_t dev_id,fal_vlan_mode_t mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	a_uint32_t i, j, k;
	a_uint32_t lport, hport, hport_tmp;	
//	GT_LPORT memPorts[MV3236_MAX_PORT_NUM];
	a_uint8_t memPortsLen;
	GT_STATUS status;

	switch(mode) {
		case FAL_VLAN_MODE_PORT_BASED:
#if 0
			vlan_mode = port_based_vlan;
			
			/* Clear Unique Untag Port Flag */
			memset(Unique_UT_Port,0,MV3236_MAX_PORT_NUM);
			
			/* Reset DefaultVlanSetting Members All MEMBER_EGRESS_UNTAGGED */
			for(i=0; i<MV3236_MAX_PORT_NUM-1; i++){
				DefaultVlanSetting[i] = MEMBER_EGRESS_UNTAGGED;
			}
			
			/* Clear All VLAN ID Table , FIXME, do we need do this here */

			/* Use Port Based Vlan Table,All Port set 8021q Vlan Disable Mode */
			for(i=0; i<MV3236_MAX_PORT_NUM; i++) {
				SW_RTN_ON_ERROR(map_lport2hport(i, &hport));
				if((status = cpssDxChBrgNestVlanAccessPortSet(dev_id, hport, GT_TRUE)) != GT_OK) {
					printf("cpssDxChBrgNestVlanAccessPortSet return Failed, status=%d\r\n", status);
					return SW_FAIL;
				}
			}
#endif
			printf("port_based_vlan is not supported right now!!!!\n");
			break;
			
		case FAL_VLAN_MODE_8021Q:
			vlan_mode = ieee8021q_vlan;
#if 0
			/* Disable Port Based vlan while enable 8021q vlan */
			for(i=0; i<MV3236_MAX_PORT_NUM-1; i++) {
				
				lport = (i+1);
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));

				memset(memPorts, 0, (MV3236_MAX_PORT_NUM) * sizeof(GT_LPORT));
				k=0;
				for(j=0; j<MV3236_MAX_PORT_NUM-1; j++) {
					
					if(j == i)
						continue;
			
					SW_RTN_ON_ERROR(map_lport2hport((j+1), &hport_tmp));
					memPorts[k] = (hport_tmp);
					//printf("%s, memPorts[%d] = %04x\n",__FUNCTION__,k,memPorts[k]);
					k++;
					
				}
				memPorts[k] = pJwDev->cpuPortNum;
				k++;		
				memPortsLen = k;

				/* Port Based Vlan Set All Ports Unisolation */
				if(gvlnSetPortVlanPorts(pJwDev, hport, memPorts, memPortsLen) != GT_OK) {
					printf("gvlnSetPortVlanPorts Fail\n");
					return SW_FAIL;
				}

				/* Set Default pvid 1 */
				if((status = gvlnSetPortVid(pJwDev, hport, DEFAULT_PVID)) != GT_OK) {
					printf("Error: gvlnSetPortVid, status=%d\r\n", status);
					return SW_FAIL; 
				}
			}
			/*  Clear VLAN ID Table */
			if((status = gvtuFlush(pJwDev)) != GT_OK) {
				printf("gvtuFlush returned failed\r\n");
				return SW_FAIL;
			}

			/* Enable 802.1Q for each port except CPU port as GT_SECURE mode */
			for(i=0; i<MV3236_MAX_PORT_NUM; i++) {
				
				lport = (i+1);
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
				
				if((status = cpssDxChBrgNestVlanAccessPortSet(dev_id, hport, GT_FALSE)) != GT_OK) {
					printf("Error: cpssDxChBrgNestVlanAccessPortSet, status=%d\r\n", status);
					return SW_FAIL; 
				}		
			}

			/* Enable 802.1Q for CPU port as GT_FALLBACK mode */
			if((status = gvlnSetPortVlanDot1qMode(pJwDev, pJwDev->cpuPortNum, GT_FALLBACK)) != GT_OK) {
				printf("Error: gvlnSetPortVlanDot1qMode, status=%d\r\n", status);
				return SW_FAIL;	
			}	

			/* Set Default Vlan */
			if((status = mv3236_default_vlan_set(DefaultVlanSetting)) != SW_OK) {
				printf("mv3236_default_vlan_set return Failed\n");
				return SW_FAIL;
			}
#endif
			break;

		default:
			break;
	}

    return SW_OK;
}

static inline sw_error_t _mv3236_vlan_mode_get(a_uint32_t dev_id,fal_vlan_mode_t * mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	
	if(vlan_mode == port_based_vlan) {
		
		*mode = FAL_VLAN_MODE_PORT_BASED;
		
	}else if(vlan_mode == ieee8021q_vlan) {
	
		*mode = FAL_VLAN_MODE_8021Q;	
	}else{

		*mode = FAL_VLAN_MODE_PORT_BASED;    /* default vlan mode:port-based */
	}

    return rv;
}

static inline sw_error_t _mv3236_port_based_vlan_set(a_uint32_t dev_id,fal_port_t port_id,fal_vlan_members_t *vlan_members, a_uint32_t member_size)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	//GT_LPORT memPorts[MV3236_MAX_PORT_NUM];
	a_uint8_t memPortsLen;
	a_uint32_t hport, lport, hport_tmp;
	a_uint32_t i, j, k;
	a_uint32_t VlanMembers;
	a_uint8_t member_byte[MV3236_MAX_MEMBERS_SIZE];

	VlanMembers = 0;
	memset(member_byte, 0, MV3236_MAX_MEMBERS_SIZE);

	if(vlan_mode == port_based_vlan) {
#if 0
		for(i=0; i<member_size; i++) {
			member_byte[i] = shift_high_low_bit(vlan_members->members[i]);
			//printf("%s, members_byte[%d] = %02x\n",__FUNCTION__,i,member_byte[i]);
			VlanMembers |= (member_byte[i] << (i*8));
		}
	//	printf("%s, VlanMembers = %04x, i=%d\n",__FUNCTION__,VlanMembers,i);
				
		lport = port_id;
		SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));

		memset(memPorts, 0, (MV3236_MAX_PORT_NUM) * sizeof(GT_LPORT));
		k=0;
		for(j=0; j<MV3236_MAX_PORT_NUM-1; j++) {
			if(j == lport-1)
				continue;
			if(VlanMembers & (1<<j)) {
				
				SW_RTN_ON_ERROR(map_lport2hport((j+1), &hport_tmp));
				memPorts[k] = (hport_tmp);
				k++;
			}
		}
		memPorts[k] = pJwDev->cpuPortNum;
		k++;		
		memPortsLen = k;
				
		if(gvlnSetPortVlanPorts(pJwDev, hport, memPorts, memPortsLen) != GT_OK) {
			printf("gvlnSetPortVlanPorts Fail\n");
			return SW_FAIL;
		}	
#endif
	}else{
		printf("Port Based Vlan Set Fail, Not Port Based Mode Now\n");
		return SW_FAIL;
	}

    return SW_OK;
}

static inline sw_error_t _mv3236_8021q_vlan_row_create(a_uint32_t dev_id,fal_vlan_id_t vlan_id,fal_vlan_members_t * port_members, a_uint32_t member_size, fal_vlan_egress_tag_t * egress_tag)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
	sw_error_t rv = SW_OK;
	GT_STATUS status;
	a_uint32_t hport, lport;
	a_uint32_t i, j;
	a_uint32_t VlanMembers, Tag;
	a_uint8_t member_byte[MV3236_MAX_MEMBERS_SIZE];
	a_uint8_t tag_byte[MV3236_MAX_MEMBERS_SIZE];
//	GT_VTU_ENTRY vtuEntry;
	a_uint16_t vid, pvid;
	a_int32_t vlan_setting[MV3236_MAX_PORT_NUM];

	if(vlan_mode == ieee8021q_vlan) {
		
		VlanMembers = 0;
		Tag = 0;
		memset(member_byte, 0, MV3236_MAX_MEMBERS_SIZE);
		memset(tag_byte, 0, MV3236_MAX_MEMBERS_SIZE);
		memset(vlan_setting, 0, MV3236_MAX_PORT_NUM);

		/* shift high low bit, prase octec jwsportlist type */
		for(i=0; i<member_size; i++) {
			member_byte[i] = shift_high_low_bit(port_members->members[i]);
			VlanMembers |= (member_byte[i] << (i*8));
		}

		for(i=0; i<member_size; i++) {
			tag_byte[i] = shift_high_low_bit(egress_tag->members[i]);
			Tag |= (tag_byte[i] << (i*8));
		}

		/* 1) Clear VLAN ID Table */

		/* 2) Setup 802.1Q mode for each port except CPU port. */			
		vid = vlan_id;	

		if((vid<1) || (vid>4094)) {
			printf("Error: invalid vid number %d\r\n", vid);
			return SW_BAD_PARAM;
		}

		for(i=0; i<MV3236_MAX_PORT_NUM-1; i++) {
			lport = i+1;
			SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
			
			if((VlanMembers >> i) & 0x01) { 		
				if((Tag >> i) & 0x01) {							/* Tag Port */
					vlan_setting[i] = CPSS_DXCH_BRG_VLAN_PORT_TAG0_CMD_E;

					/* restore the port's pvid to 1, because this time port adds in the vlan as tagged type*/
					if((DestroyFlag[vid] >> i) & 0x01){
						pvid = DEFAULT_PVID;
						DefaultVlanSetting[i] = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
						Unique_UT_Port[i] = 0;
						DestroyFlag[vid] &= ~(1<<i);

						/* Set port pvid */
						status = cpssDxChBrgVlanPortVidSet(dev_id, hport, CPSS_DIRECTION_INGRESS_E, pvid);
						if(status != GT_OK)
						{
							printf("%s.%d cpssDxChBrgVlanPortVidSet failed %d \n", __FILE__,__LINE__, status);
							return SW_FAIL;
						}

					}			
				}else {											/* Untag Port */
					if((DestroyFlag[vid] >> i) & 0x01){			/* same vlan untag port can set again */
						/* this vlan's untag port can set again, Do nothing */	
					}else{
						if(Unique_UT_Port[i] == 1){
							printf("8021q Vlan Set Error, port can't belong to more than one pvid\n");
							return SW_FAIL;
						}
					}				
				
					Unique_UT_Port[i] = 1;
					DestroyFlag[vid] |= (1<<i);				
					vlan_setting[i] = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
					DefaultVlanSetting[i] = -1;
					pvid = vlan_id;

					/* Set port pvid */
					status = cpssDxChBrgVlanPortVidSet(dev_id, hport, CPSS_DIRECTION_INGRESS_E, pvid);
					if(status != GT_OK)
					{
						printf("%s.%d cpssDxChBrgVlanPortVidSet failed %d \n", __FILE__,__LINE__, status);
						return SW_FAIL;
					}
	
				}		
			}else { 	
				vlan_setting[i] = -1;
				pvid = DEFAULT_PVID;			
			}

		}

		/* 4) Add VLAN ID and add vlan members */
		if((status = mv3236_vlan_set(dev_id, vlan_id, vlan_setting)) != GT_OK) {
			printf("mv3236_vlan_set return Failed\n");
			return SW_FAIL;
		}

		/* Reset Default Vlan, Delete Untag Member if other Vlan Has Set This Port Untag */
		if((status = mv3236_vlan_set(dev_id, vlan_id, DefaultVlanSetting)) != SW_OK) {
			printf("mv3236_vlan_set return Failed\n");
			return SW_FAIL;
		}
		
	}else{
		printf("8021q Vlan Set Fail,Not 8021q Mode Now\n");
		return SW_FAIL;
	}

    return SW_OK;
}

static inline sw_error_t _mv3236_8021q_vlan_row_destroy(a_uint32_t dev_id,fal_vlan_id_t vlan_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv = SW_OK;
	GT_STATUS status;
//	GT_VTU_ENTRY vtuEntry;
	a_uint16_t vid;
	a_uint32_t lport,hport;
	a_uint32_t i;

	if(vlan_mode == ieee8021q_vlan) {
		vid = vlan_id;		
		if(vid == 1) {
			printf("Default Vlan 1 Can't be Destroied!\n");
			return SW_FAIL;
		}
		
		if((vid<1) || (vid>4094)) {
			printf("Input vid Error\n");
			return SW_BAD_PARAM;
		}
		
		if((status = mv3236_vlan_clean(dev_id, vlan_id)) != SW_OK) {
				printf("mv3236_default_vlan_set return Failed\n");
				return SW_FAIL;
		}

		/* Clear Unique Untag Port Flag */
		for(i=0; i<MV3236_MAX_PORT_NUM-1; i++){
			if((DestroyFlag[vid] >> i) & 0x01){
				lport = i+1;
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
				
				DestroyFlag[vid] &= ~(1<<i);
				Unique_UT_Port[i] = 0;
				DefaultVlanSetting[i] = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
				printf("i = %d, DestroyFlag[%d] = %d\n",i,vid,DestroyFlag[vid]);

				/* Set port pvid */
				status = cpssDxChBrgVlanPortVidSet(dev_id, hport, CPSS_DIRECTION_INGRESS_E, DEFAULT_PVID);
				if(status != GT_OK)
				{
					printf("%s.%d cpssDxChBrgVlanPortVidSet failed %d \n", __FILE__,__LINE__, status);
					return SW_FAIL;
				}
			}
		}	

		/* Reset Default Vlan  */
		if((status = mv3236_vlan_set(dev_id, vlan_id, DefaultVlanSetting)) != SW_OK) {
			printf("mv3236_vlan_set return Failed\n");
			return SW_FAIL;
		}
		
	}else{
		printf("8021q Vlan Destroy Fail,Not 8021q Vlan Mode Now\n");
		return SW_FAIL;
	}

    return SW_OK;
}

static inline sw_error_t _mv3236_8021q_vlan_show_status(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
#if 0
	GT_8 tag[MV3236_MAX_PORT_NUM-1][3];
	GT_STATUS ret;
	GT_VTU_ENTRY vtuEntry;
	a_uint32_t hport,lport;
	GT_U16 pvid;
	a_uint32_t i;

	gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
	vtuEntry.vid = 0xfff;
	ret = gvtuGetEntryFirst(pJwDev,&vtuEntry);
	if(ret != GT_OK) {
		if(ret == GT_NO_SUCH) {
			printf("Warning: Vlan entry does not exist\r\n");
		} else {
			printf("gvtuGetEntryFirst return failed, ret=%d\r\n", ret);
		}
		return SW_FAIL;
	}
	
	printf("\r\n");
	printf("  =========");
    for(i=0; i<MV3236_MAX_PORT_NUM; i++) {
        printf("=====");
    }
    printf("\r\n");
    
    printf("  VlanID    ");
    for(i=0; i<MV3236_MAX_PORT_NUM; i++) {
		if(i==0)
			printf("Cpu  ");
		else
        	printf("P%d   ",i);
    }
	printf("\r\n");

	printf("  =========");
    for(i=0; i<MV3236_MAX_PORT_NUM; i++) {
        printf("=====");
    }
    printf("\r\n");

	for(i=0; i<MV3236_MAX_PORT_NUM; i++) {
		if(i==0){
			if(vtuEntry.vtuData.memberTagP[pJwDev->cpuPortNum] == NOT_A_MEMBER)
				strcpy(tag[i], "NA");
			else if(vtuEntry.vtuData.memberTagP[pJwDev->cpuPortNum] == MEMBER_EGRESS_UNTAGGED)
				strcpy(tag[i], "-T");
			else if(vtuEntry.vtuData.memberTagP[pJwDev->cpuPortNum] == MEMBER_EGRESS_TAGGED)
				strcpy(tag[i], "+T");
			else
				strcpy(tag[i], "??");
		}else{
			
			SW_RTN_ON_ERROR(map_lport2hport(i, &hport));
					
			if(vtuEntry.vtuData.memberTagP[hport] == NOT_A_MEMBER)
				strcpy(tag[i], "NA");
			else if(vtuEntry.vtuData.memberTagP[hport] == MEMBER_EGRESS_UNTAGGED)
				strcpy(tag[i], "-T");
			else if(vtuEntry.vtuData.memberTagP[hport] == MEMBER_EGRESS_TAGGED)
				strcpy(tag[i], "+T");
			else
				strcpy(tag[i], "??");
		}				
	}

    printf("    %-4d    ",vtuEntry.vid);
    for(i=0; i<MV3236_MAX_PORT_NUM; i++) {
        printf("%-2s   ",tag[i]);
    }
	printf("\r\n");
	
	while(1) {
		if((ret = gvtuGetEntryNext(pJwDev,&vtuEntry)) != GT_OK)
			break;
		
		for(i=0; i<MV3236_MAX_PORT_NUM; i++) {
			if(i==0){	
				if(vtuEntry.vtuData.memberTagP[pJwDev->cpuPortNum] == NOT_A_MEMBER)
					strcpy(tag[i], "NA");
				else if(vtuEntry.vtuData.memberTagP[pJwDev->cpuPortNum] == MEMBER_EGRESS_UNTAGGED)
					strcpy(tag[i], "-T");
				else if(vtuEntry.vtuData.memberTagP[pJwDev->cpuPortNum] == MEMBER_EGRESS_TAGGED)
					strcpy(tag[i], "+T");
				else
					strcpy(tag[i], "??");
			}else{
				
				SW_RTN_ON_ERROR(map_lport2hport(i, &hport));
				
				if(vtuEntry.vtuData.memberTagP[hport] == NOT_A_MEMBER)
					strcpy(tag[i], "NA");
				else if(vtuEntry.vtuData.memberTagP[hport] == MEMBER_EGRESS_UNTAGGED)
					strcpy(tag[i], "-T");
				else if(vtuEntry.vtuData.memberTagP[hport] == MEMBER_EGRESS_TAGGED)
					strcpy(tag[i], "+T");
				else
					strcpy(tag[i], "??");
			}
		}
        
        printf("    %-4d    ",vtuEntry.vid);
        for(i=0; i<MV3236_MAX_PORT_NUM; i++) {
            printf("%-2s   ",tag[i]);
        }
        printf("\r\n");
        
    }

	/* Pvid status show  */
	printf("\r\n");
	printf("  ===================\n");
	printf("  Port		Pvid\n");
	printf("  ===================\n");

	for(i=0; i<11; i++){
		lport = i;
		SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
		if((ret = gvlnGetPortVid(pJwDev, hport, &pvid)) != GT_OK) {
			printf("Error: gvlnGetPortVid, ret=%d\r\n", ret);
			return SW_FAIL; 
		}
		if(i==0)
			printf("   Cpu		 %d\n",pvid);
		else
			printf("   P%d		 %d\n",lport,pvid);
	}

	printf("\n\r");
	printf("  NA: Not a Vlan Member\n");
	printf("  +T: Egress Tagged\n");
	printf("  -T: Egress Untag\n");
	printf("   ?: Unknow\n\r");
#endif
	return SW_OK;
}

static inline sw_error_t _mv3236_8021q_vlan_row_status(a_uint32_t dev_id,fal_vlan_id_t * vlan_id,fal_vlan_egress_tag_t * egress_tag)
{	
    SW_PRINTFL(DETAIL,"====enter====\n");
#if 0
	//GT_8 tag[mv3236_MAX_PORT_NUM-1][3];
	GT_STATUS ret;
	GT_VTU_ENTRY vtuEntry;
	a_uint32_t hport;
	a_uint32_t i, EgressUntag;
	a_uint8_t tag_byte[MV3236_MAX_MEMBERS_SIZE];
	a_uint32_t TagMembers;

	EgressUntag = 0;
	TagMembers = 0;	

	gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
	vtuEntry.vid = 0xfff;
	ret = gvtuGetEntryFirst(pJwDev,&vtuEntry);
	if(ret != GT_OK) {
		if(ret == GT_NO_SUCH) {
			printf("Warning: Vlan entry does not exist\r\n");
		} else {
			printf("gvtuGetEntryFirst return failed, ret=%d\r\n", ret);
		}
		return SW_FAIL;
	}

	for(i=0; i<MV3236_MAX_PORT_NUM-1; i++) {
		
		SW_RTN_ON_ERROR(map_lport2hport((i+1), &hport));
				
		if(vtuEntry.vtuData.memberTagP[hport] == MEMBER_EGRESS_UNTAGGED){
			EgressUntag |= (1<<i);
			
		}				
	}
	printf("EgressUntag = %x\n",EgressUntag);

	tag_byte[0] = (EgressUntag & 0xFF);
	tag_byte[1] = (EgressUntag & 0xFF00) >> 8;
	
	/* Output Paran egress_tag, vlan_id, size */
	for(i=0; i<MV3236_MAX_MEMBERS_SIZE; i++) {
		egress_tag->members[i] = shift_high_low_bit(tag_byte[i]);		
		printf("egress_tag->members[%d] = %04x\n",i,egress_tag->members[i]);
	}
	TagMembers = (egress_tag->members[0] << 8) | egress_tag->members[1];	
	printf("TagMembers = %02x\n",TagMembers);
	
	*vlan_id = vtuEntry.vid;
#endif
	return SW_OK;
}

sw_error_t mv3236_vlan_mode_set(a_uint32_t dev_id,fal_vlan_mode_t mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_vlan_mode_set(dev_id, mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_vlan_mode_get(a_uint32_t dev_id,fal_vlan_mode_t * mode)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_vlan_mode_get(dev_id, mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_port_based_vlan_set(a_uint32_t dev_id,fal_port_t port_id,fal_vlan_members_t *vlan_members, a_uint32_t member_size)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_port_based_vlan_set(dev_id, port_id, vlan_members, member_size);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_8021q_vlan_row_create(a_uint32_t dev_id,fal_vlan_id_t vlan_id,fal_vlan_members_t * port_members, a_uint32_t member_size, fal_vlan_egress_tag_t * egress_tag)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_8021q_vlan_row_create(dev_id, vlan_id, port_members, member_size, egress_tag);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_8021q_vlan_row_destroy(a_uint32_t dev_id,fal_vlan_id_t vlan_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_8021q_vlan_row_destroy(dev_id, vlan_id);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_8021q_vlan_show_status(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_8021q_vlan_show_status(dev_id);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_8021q_vlan_row_status(a_uint32_t dev_id,fal_vlan_id_t * vlan_id,fal_vlan_egress_tag_t * egress_tag)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv3236_8021q_vlan_row_status(dev_id, vlan_id, egress_tag);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv3236_vlan_init(a_uint32_t dev_id)
{
    SW_PRINTFL(DETAIL,"====enter====\n");
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;
	a_uint32_t i = 0;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);
	
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	p_api->vlan_mode_set = mv3236_vlan_mode_set;
	p_api->vlan_mode_get = mv3236_vlan_mode_get;
	p_api->port_based_vlan_set = mv3236_port_based_vlan_set;
	p_api->ieee8021q_vlan_row_create = mv3236_8021q_vlan_row_create;
	p_api->ieee8021q_vlan_row_destroy = mv3236_8021q_vlan_row_destroy;
	p_api->ieee8021q_vlan_show_status = mv3236_8021q_vlan_show_status;
	p_api->ieee8021q_vlan_row_status = mv3236_8021q_vlan_row_status;

	for(i = 0; i < MV3236_MAX_PORT_NUM; i++)
	{
		DefaultVlanSetting[i] = CPSS_DXCH_BRG_VLAN_PORT_UNTAGGED_CMD_E;
	}
	return SW_OK;
}

