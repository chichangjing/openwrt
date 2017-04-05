 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv88e6097_vlan.h"
#include "mv88e6097_port_ctrl.h"
#include "mv88e6097_reg.h"
#include "msApi.h"
#include "gtHwCntl.h"
#include "gtDrvSwRegs.h"

#include "joyware.h"


extern GT_QD_DEV *pJwDev;

typedef enum{
	port_based_vlan = 1,
	ieee8021q_vlan =2,
}VLAN_MODE;

static a_uint32_t vlan_mode;
static a_uint8_t Unique_UT_Port[CONFIG_JWS_PORT_TOTAL_NR+1];
static a_uint32_t DestroyFlag[4096];
static a_uint8_t DefaultVlanSetting[MV88E6097_MAX_PORT_NUM-1] = { MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED,
															       MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED,
															        MEMBER_EGRESS_UNTAGGED, MEMBER_EGRESS_UNTAGGED };

#define DEFAULT_PVID	1
#define DEFAULT_VID     1



sw_error_t mv88e6097_default_vlan_set(a_uint8_t *defautsetting)
{
	GT_STATUS status;
	GT_VTU_ENTRY vtuEntry;
	a_uint16_t vid;
	a_uint32_t i, hport;
	a_uint8_t DefaultSettingBuf[CONFIG_JWS_PORT_TOTAL_NR];

	/* Receive DefaultVlanSetting to DefaultSettingBuf arry */
	for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++){
		DefaultSettingBuf[i] = defautsetting[i];
	}

	vid = DEFAULT_VID;
	gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
	vtuEntry.DBNum = 0;
	vtuEntry.vid = vid; 		
	for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {
		
		SW_RTN_ON_ERROR(map_lport2hport((i+1), &hport));
		
		vtuEntry.vtuData.memberTagP[hport] = DefaultSettingBuf[i];
	}
	vtuEntry.vtuData.memberTagP[pJwDev->cpuPortNum] = MEMBER_EGRESS_UNTAGGED;

	if((status = gvtuAddEntry(pJwDev,&vtuEntry)) != GT_OK) {
		printf("gvtuAddEntry return Failed\n");
		return SW_FAIL;
	}

	return SW_OK;

}



static inline sw_error_t _mv88e6097_vlan_mode_set(a_uint32_t dev_id,fal_vlan_mode_t mode)
{
    sw_error_t rv = SW_OK;
	a_uint32_t i, j, k;
	a_uint32_t lport, hport, hport_tmp;	
	GT_LPORT memPorts[CONFIG_JWS_PORT_TOTAL_NR+1];	
	a_uint8_t memPortsLen;
	GT_STATUS status;
	
	switch(mode) {
		case FAL_VLAN_MODE_PORT_BASED:
			vlan_mode = port_based_vlan;
			
			/* Clear Unique Untag Port Flag */
			memset(Unique_UT_Port,0,CONFIG_JWS_PORT_TOTAL_NR+1);
			
			/* Reset DefaultVlanSetting Members All MEMBER_EGRESS_UNTAGGED */
			for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++){
				DefaultVlanSetting[i] = MEMBER_EGRESS_UNTAGGED;
			}
			
			/* Clear All VLAN ID Table */
			if((status = gvtuFlush(pJwDev)) != GT_OK) {
				printf("gvtuFlush returned failed\r\n");
				return SW_FAIL;
			}

			/* Use Port Based Vlan Table,All Port set 8021q Vlan Disable Mode */
			for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR+1; i++) {
				SW_RTN_ON_ERROR(map_lport2hport(i, &hport));
				
				if((status = gvlnSetPortVlanDot1qMode(pJwDev, hport, GT_DISABLE)) != GT_OK) {
					printf("gvlnSetPortVlanDot1qMode return Failed, port=%d, status=%d\r\n", (int)pJwDev->cpuPortNum, status);
					return SW_FAIL;
				}
			}
								
			break;
			
		case FAL_VLAN_MODE_8021Q:
			vlan_mode = ieee8021q_vlan;

			/* Disable Port Based vlan while enable 8021q vlan */
			for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {
				
				lport = (i+1);
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));

				memset(memPorts, 0, (CONFIG_JWS_PORT_TOTAL_NR+1) * sizeof(GT_LPORT));
				k=0;
				for(j=0; j<CONFIG_JWS_PORT_TOTAL_NR; j++) {
					
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
			for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {
				
				lport = (i+1);
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
				
				if((status = gvlnSetPortVlanDot1qMode(pJwDev, hport, GT_SECURE)) != GT_OK) {
					printf("Error: gvlnSetPortVlanDot1qMode, status=%d\r\n", status);
					return SW_FAIL; 
				}		
			}
					
			/* Enable 802.1Q for CPU port as GT_FALLBACK mode */
			if((status = gvlnSetPortVlanDot1qMode(pJwDev, pJwDev->cpuPortNum, GT_FALLBACK)) != GT_OK) {
				printf("Error: gvlnSetPortVlanDot1qMode, status=%d\r\n", status);
				return SW_FAIL;	
			}	

			/* Set Default Vlan */
			if((status = mv88e6097_default_vlan_set(DefaultVlanSetting)) != SW_OK) {
				printf("mv88e6097_default_vlan_set return Failed\n");
				return SW_FAIL;
			}

			break;

		default:
			break;
	}

    return rv;

}

static inline sw_error_t _mv88e6097_vlan_mode_get(a_uint32_t dev_id,fal_vlan_mode_t * mode)
{
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

static inline sw_error_t _mv88e6097_port_based_vlan_set(a_uint32_t dev_id,fal_port_t port_id,fal_vlan_members_t *vlan_members, a_uint32_t member_size)
{
    sw_error_t rv = SW_OK;
	GT_LPORT memPorts[CONFIG_JWS_PORT_TOTAL_NR+1];
	a_uint8_t memPortsLen;
	a_uint32_t hport, lport, hport_tmp;
	a_uint32_t i, j, k;
	a_uint32_t VlanMembers;
	a_uint8_t member_byte[MV88E6097_MAX_MEMBERS_SIZE];
	sal_sys_model_t model;
	GT_LPORT fpgaPort;

	VlanMembers = 0;
	memset(member_byte, 0, MV88E6097_MAX_MEMBERS_SIZE);

	if(vlan_mode == port_based_vlan) {

		for(i=0; i<member_size; i++) {
			member_byte[i] = shift_high_low_bit(vlan_members->members[i]);
			//printf("%s, members_byte[%d] = %02x\n",__FUNCTION__,i,member_byte[i]);
			VlanMembers |= (member_byte[i] << (i*8));
		}
	//	printf("%s, VlanMembers = %04x, i=%d\n",__FUNCTION__,VlanMembers,i);
				
		lport = port_id;
		SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));

		memset(memPorts, 0, (CONFIG_JWS_PORT_TOTAL_NR+1) * sizeof(GT_LPORT));
		k=0;
		for(j=0; j<CONFIG_JWS_PORT_TOTAL_NR; j++) {
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

		/* Use for yuxi board only, switch - fpga port */
		SW_RTN_ON_ERROR(sal_sys_model_get(&model));
		switch(model.model_id) {
			case GE_SDI1000_0014_T:
			case GE_SDI1000_0014_R:
				fpgaPort = 10;
				memPorts[k] = fpgaPort;
				k++;		
				memPortsLen = k;
			break;

			default:
				break;
		}
			
		if(gvlnSetPortVlanPorts(pJwDev, hport, memPorts, memPortsLen) != GT_OK) {
			printf("gvlnSetPortVlanPorts Fail\n");
			return SW_FAIL;
		}	
		
	}else{
		printf("Port Based Vlan Set Fail, Not Port Based Mode Now\n");
		return SW_FAIL;
	}

    return rv;

}

static inline sw_error_t _mv88e6097_8021q_vlan_row_create(a_uint32_t dev_id,fal_vlan_id_t vlan_id,fal_vlan_members_t * port_members, a_uint32_t member_size, fal_vlan_egress_tag_t * egress_tag)
{
	sw_error_t rv = SW_OK;
	GT_STATUS status;
	a_uint32_t hport, lport;
	a_uint32_t i, j;
	a_uint32_t VlanMembers, Tag;
	a_uint8_t member_byte[MV88E6097_MAX_MEMBERS_SIZE];	
	a_uint8_t tag_byte[MV88E6097_MAX_MEMBERS_SIZE];	
	GT_VTU_ENTRY vtuEntry;
	a_uint16_t vid, pvid;
	a_uint8_t vlan_setting[CONFIG_JWS_PORT_TOTAL_NR+1];

	
	if(vlan_mode == ieee8021q_vlan) {
		
		VlanMembers = 0;
		Tag = 0;
		memset(member_byte, 0, MV88E6097_MAX_MEMBERS_SIZE);	
		memset(tag_byte, 0, MV88E6097_MAX_MEMBERS_SIZE);	
		memset(vlan_setting, 0, CONFIG_JWS_PORT_TOTAL_NR+1);

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
		
		for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {	
			lport = i+1;
			SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
			
			if((VlanMembers >> i) & 0x01) { 		
				if((Tag >> i) & 0x01) {							/* Tag Port */
					vlan_setting[i] = MEMBER_EGRESS_TAGGED;
					pvid = DEFAULT_PVID; 

					/* Other Vlan port Egress from Untag to Tag, Set vlan 1 to untag port */				
					if((DestroyFlag[vid] >> i) & 0x01){
						DefaultVlanSetting[i] = MEMBER_EGRESS_UNTAGGED;
						Unique_UT_Port[i] = 0;
						DestroyFlag[vid] &= ~(1<<i);

						/* Other Vlan port Egress from Untag to Tag, Set Pvid to default */
						if((status = gvlnSetPortVid(pJwDev, hport, pvid)) != GT_OK) {
							printf("Error: gvlnSetPortVid, status=%d\r\n", status);
							return SW_FAIL; 
						}
					}			
				}else {											/* Untag Port */

					if((DestroyFlag[vid] >> i) & 0x01){			/* same vlan untag port can set again */
						/* this vlan's untag port can set again, Do nothing */	
					}else{
						if(Unique_UT_Port[i] == 1){
							printf("8021q Vlan Set Error, Untag port Must be Unique\n");
							return SW_FAIL;
						}
					}				
				
					Unique_UT_Port[i] = 1;
					DestroyFlag[vid] |= (1<<i);				
					vlan_setting[i] = MEMBER_EGRESS_UNTAGGED;
					DefaultVlanSetting[i] = NOT_A_MEMBER;
					pvid = vlan_id;

					if((status = gvlnSetPortVid(pJwDev, hport, pvid)) != GT_OK) {
						printf("Error: gvlnSetPortVid, status=%d\r\n", status);
						return SW_FAIL; 
					}
	
				}		
			}else { 	
				vlan_setting[i] = NOT_A_MEMBER;
				pvid = DEFAULT_PVID;			
			}
		
			if((status = gvlnSetPortVlanDot1qMode(pJwDev, hport, GT_SECURE)) != GT_OK) {
				printf("Error: gvlnSetPortVlanDot1qMode, status=%d\r\n", status);
				return SW_FAIL; 
			}
			
		}

		/* 3) Enable 802.1Q for CPU port as GT_FALLBACK mode */
		if((status = gvlnSetPortVlanDot1qMode(pJwDev, pJwDev->cpuPortNum, GT_FALLBACK)) != GT_OK) {
			printf("gvlnSetPortVlanDot1qMode return Failed, port=%d, status=%d\r\n", (int)pJwDev->cpuPortNum, status);
			return SW_FAIL;
		}

		/* 4) Add VLAN ID and add vlan members */		
		gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
		vtuEntry.DBNum = 0;
		vtuEntry.vid = vid; 		
		
		for(j=0; j<CONFIG_JWS_PORT_TOTAL_NR; j++) {			
			lport = j+1;
			SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
			vtuEntry.vtuData.memberTagP[hport] = vlan_setting[j];			
		}
		vtuEntry.vtuData.memberTagP[pJwDev->cpuPortNum] = MEMBER_EGRESS_UNTAGGED;

		if((status = gvtuAddEntry(pJwDev,&vtuEntry)) != GT_OK) {
			printf("gvtuAddEntry return Failed\n");
			return SW_FAIL;
		}

		/* Reset Default Vlan, Delete Untag Member if other Vlan Has Set This Port Untag */
		if((status = mv88e6097_default_vlan_set(DefaultVlanSetting)) != SW_OK) {
			printf("mv88e6097_default_vlan_set return Failed\n");
			return SW_FAIL;
		}
		
	}else{
		printf("8021q Vlan Set Fail,Not 8021q Mode Now\n");
		return SW_FAIL;
	}

    return rv;

}

static inline sw_error_t _mv88e6097_8021q_vlan_row_destroy(a_uint32_t dev_id,fal_vlan_id_t vlan_id)
{
    sw_error_t rv = SW_OK;
	GT_STATUS status;
	GT_VTU_ENTRY vtuEntry;
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
		
		gtMemSet(&vtuEntry,0,sizeof(GT_VTU_ENTRY));
		vtuEntry.DBNum = 0;
		vtuEntry.vid = vid;
		if((status = gvtuDelEntry(pJwDev,&vtuEntry)) != GT_OK) {
			printf("gvtuDelEntry return Failed\n");
			return SW_FAIL;
		}

		/* Clear Unique Untag Port Flag */
		for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++){
			if((DestroyFlag[vid] >> i) & 0x01){
				lport = i+1;
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
				
				DestroyFlag[vid] &= ~(1<<i);
				Unique_UT_Port[i] = 0;
				DefaultVlanSetting[i] = MEMBER_EGRESS_UNTAGGED;
				printf("i = %d, DestroyFlag[%d] = %d\n",i,vid,DestroyFlag[vid]);

				/* Other Vlan port Egress from Untag to Tag, Set Pvid to default */
				if((status = gvlnSetPortVid(pJwDev, hport, DEFAULT_PVID)) != GT_OK) {
					printf("Error: gvlnSetPortVid, status=%d\r\n", status);
					return SW_FAIL; 
				}
			}
		}	

		/* Reset Default Vlan, Add Untag Member if other Vlan Has Delete This Untag Port */
		if((status = mv88e6097_default_vlan_set(DefaultVlanSetting)) != SW_OK) {
			printf("mv88e6097_default_vlan_set return Failed\n");
			return SW_FAIL;
		}
		
	}else{
		printf("8021q Vlan Destroy Fail,Not 8021q Vlan Mode Now\n");
		return SW_FAIL;
	}

    return rv;

}

static inline sw_error_t _mv88e6097_8021q_vlan_show_status(a_uint32_t dev_id)
{
	GT_8 tag[CONFIG_JWS_PORT_TOTAL_NR][3];
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
    for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR+1; i++) {	
        printf("=====");
    }
    printf("\r\n");
    
    printf("  VlanID    ");
    for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR+1; i++) {
		if(i==0)
			printf("Cpu  ");
		else
        	printf("P%d   ",i);
    }
	printf("\r\n");

	printf("  =========");
    for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR+1; i++) {
        printf("=====");
    }
    printf("\r\n");

	for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR+1; i++) {
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
    for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR+1; i++) {
        printf("%-2s   ",tag[i]);
    }
	printf("\r\n");
	
	while(1) {
		if((ret = gvtuGetEntryNext(pJwDev,&vtuEntry)) != GT_OK)
			break;
		
		for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR+1; i++) {
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
        for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR+1; i++) {
            printf("%-2s   ",tag[i]);
        }
        printf("\r\n");
        
    }

	/* Pvid status show  */
	printf("\r\n");
	printf("  ===================\n");
	printf("  Port		Pvid\n");
	printf("  ===================\n");

	for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR+1; i++){
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

	return SW_OK;

}

static inline sw_error_t _mv88e6097_8021q_vlan_row_status(a_uint32_t dev_id,fal_vlan_id_t * vlan_id,fal_vlan_egress_tag_t * egress_tag)
{	
	//GT_8 tag[CONFIG_JWS_PORT_TOTAL_NR][3];
	GT_STATUS ret;
	GT_VTU_ENTRY vtuEntry;
	a_uint32_t hport;
	a_uint32_t i, EgressUntag;
	a_uint8_t tag_byte[MV88E6097_MAX_MEMBERS_SIZE];	
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

	for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {
		
		SW_RTN_ON_ERROR(map_lport2hport((i+1), &hport));
				
		if(vtuEntry.vtuData.memberTagP[hport] == MEMBER_EGRESS_UNTAGGED){
			EgressUntag |= (1<<i);
			
		}				
	}
	printf("EgressUntag = %x\n",EgressUntag);

	tag_byte[0] = (EgressUntag & 0xFF);
	tag_byte[1] = (EgressUntag & 0xFF00) >> 8;
	
	/* Output Paran egress_tag, vlan_id, size */
	for(i=0; i<MV88E6097_MAX_MEMBERS_SIZE; i++) {
		egress_tag->members[i] = shift_high_low_bit(tag_byte[i]);		
		printf("egress_tag->members[%d] = %04x\n",i,egress_tag->members[i]);
	}
	TagMembers = (egress_tag->members[0] << 8) | egress_tag->members[1];	
	printf("TagMembers = %02x\n",TagMembers);
	
	*vlan_id = vtuEntry.vid;

	return SW_OK;

}




sw_error_t mv88e6097_vlan_mode_set(a_uint32_t dev_id,fal_vlan_mode_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_vlan_mode_set(dev_id, mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_vlan_mode_get(a_uint32_t dev_id,fal_vlan_mode_t * mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_vlan_mode_get(dev_id, mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_port_based_vlan_set(a_uint32_t dev_id,fal_port_t port_id,fal_vlan_members_t *vlan_members, a_uint32_t member_size)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_port_based_vlan_set(dev_id, port_id, vlan_members, member_size);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_8021q_vlan_row_create(a_uint32_t dev_id,fal_vlan_id_t vlan_id,fal_vlan_members_t * port_members, a_uint32_t member_size, fal_vlan_egress_tag_t * egress_tag)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_8021q_vlan_row_create(dev_id, vlan_id, port_members, member_size, egress_tag);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_8021q_vlan_row_destroy(a_uint32_t dev_id,fal_vlan_id_t vlan_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_8021q_vlan_row_destroy(dev_id, vlan_id);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_8021q_vlan_show_status(a_uint32_t dev_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_8021q_vlan_show_status(dev_id);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_8021q_vlan_row_status(a_uint32_t dev_id,fal_vlan_id_t * vlan_id,fal_vlan_egress_tag_t * egress_tag)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_8021q_vlan_row_status(dev_id, vlan_id, egress_tag);
    HSL_API_UNLOCK;
	
    return rv;
}




sw_error_t mv88e6097_vlan_init(a_uint32_t dev_id)
{
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);
	
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	p_api->vlan_mode_set = mv88e6097_vlan_mode_set;
	p_api->vlan_mode_get = mv88e6097_vlan_mode_get;
	p_api->port_based_vlan_set = mv88e6097_port_based_vlan_set;
	p_api->ieee8021q_vlan_row_create = mv88e6097_8021q_vlan_row_create;
	p_api->ieee8021q_vlan_row_destroy = mv88e6097_8021q_vlan_row_destroy;
	p_api->ieee8021q_vlan_show_status = mv88e6097_8021q_vlan_show_status;	
	p_api->ieee8021q_vlan_row_status = mv88e6097_8021q_vlan_row_status;

	return SW_OK;
}

