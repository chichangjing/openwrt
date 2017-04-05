 
#include "sw.h"
#include "sal_sys.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "mv88e6097_trunk.h"
#include "mv88e6097_port_ctrl.h"
#include "mv88e6097_reg.h"
#include "msApi.h"
#include "gtHwCntl.h"
#include "gtDrvSwRegs.h"

#include "joyware.h"


extern GT_QD_DEV *pJwDev;

static a_uint32_t Pag_Mode;
static a_uint8_t TrunkCount;
//struct list_head TrunkCommitList;
static LIST_HEAD(TrunkCommitList);


a_uint32_t trunk_find_group(fal_port_t lport)
{
	TrunkCommit_S *trunk,*trunk_bak;
	a_uint32_t TrunkCountNum=0;


	list_for_each_entry_safe(trunk, trunk_bak, &TrunkCommitList, node)
    {
    	if(trunk->TrunkCount != 0)
			TrunkCountNum++;

	//	printf("trunk->TrunkCount=%d, trunk-id=%d, trunk-member=%04x\n",trunk->TrunkCount,trunk->TrunkGroupRec.Trunkid,trunk->TrunkGroupRec.TrunkMemberMask);

		if((trunk->TrunkGroupRec.TrunkMemberMask & (1<<(lport-1))) != 0)
			return trunk->TrunkGroupRec.Trunkid;
    }

	//printf("TrunkCountNum = %d\n",TrunkCountNum);

	return 0xFF;
}


sw_error_t trunk_add_number(fal_trunk_id_t trunk_id, fal_port_t lport)
{
	GT_STATUS status;
	GT_U32 trunk_mask, mask;
	a_uint8_t trunk_port_array[MAX_TRUNK_GROUP_PORT_NUM];
	GT_U32 hwport_vec;
	a_uint32_t hport;
	int j, k, index, trunk_port_num;


	SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
	
	/* Get Trunk Route Table for the given Trunk ID */
	if((status = gsysGetTrunkRouting(pJwDev, trunk_id, (GT_U32 *)&hwport_vec)) != GT_OK) {
		printf("gsysGetTrunkRouting error");
		return SW_FAIL;
	}

	hwport_vec |= (1 << hport);

	/* Enable trunk on the given port */
	if((status = gprtSetTrunkPort(pJwDev, hport, GT_TRUE, trunk_id)) != GT_OK) {
		printf("gprtSetTrunkPort error");
		return SW_FAIL;
	}
	
	/* Set Trunk Route Table for the given Trunk ID */
	if((status = gsysSetTrunkRouting(pJwDev, trunk_id, hwport_vec)) != GT_OK) {
		printf("gsysSetTrunkRouting error");
		return SW_FAIL;
	}

	memset(trunk_port_array, 0, MAX_TRUNK_GROUP_PORT_NUM);
	index = 0;
	for(j=0; j<pJwDev->numOfPorts; j++) {
		if(hwport_vec & (1<<j)) {
			trunk_port_array[index] = j;
			index++;
			if(index == MAX_TRUNK_GROUP_PORT_NUM)
				break;					
		}
	}

	trunk_port_num = index;
	printf("trunk_port_num = %d\n",trunk_port_num);
	
	/* Set Trunk Mask Table for load balancing. */
	trunk_mask = 0x7FF;
	trunk_mask &= ~hwport_vec;
	for(k=0; k<8; k++) {
		index = k % trunk_port_num;
		mask = trunk_mask | (1 << trunk_port_array[index]);
		if((status = gsysSetTrunkMaskTable(pJwDev, k, mask)) != GT_OK) {
			printf("gsysSetTrunkMaskTable error");
			return SW_FAIL;
		}
	}

	fal_port_dot1d_state_set(0, lport, FAL_PORT_DOT1D_STATE_FORWARDING);

	printf("ubus call trunk add number success!\n");
	return SW_OK;
}

sw_error_t trunk_delete_number(fal_trunk_id_t trunk_id, fal_port_t lport)
{
	GT_STATUS status;
	GT_U32 trunk_mask, mask;
	a_uint8_t trunk_port_array[MAX_TRUNK_GROUP_PORT_NUM];
	GT_U32 hwport_vec;
	a_uint32_t hport;
	int j, k, index, trunk_port_num;
	

	SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));

	/* Get Trunk Route Table for the given Trunk ID */
	if((status = gsysGetTrunkRouting(pJwDev, trunk_id, (GT_U32 *)&hwport_vec)) != GT_OK) {
		printf("gsysGetTrunkRouting error");
		return SW_FAIL;
	}

	hwport_vec &= ~(1 << hport);	
	
	/* Disable trunk on the given port */
	if((status = gprtSetTrunkPort(pJwDev, hport, GT_FALSE, trunk_id)) != GT_OK) {
		printf("gprtSetTrunkPort error");
		return SW_FAIL;
	}
	
	/* Set Trunk Route Table for the given Trunk ID */
	if((status = gsysSetTrunkRouting(pJwDev, trunk_id, hwport_vec)) != GT_OK) {
		printf("gsysSetTrunkRouting error");
		return SW_FAIL;
	}

	memset(trunk_port_array, 0, MAX_TRUNK_GROUP_PORT_NUM);
	index = 0;
	for(j=0; j<pJwDev->numOfPorts; j++) {
		if(hwport_vec & (1<<j)) {
			trunk_port_array[index] = j;
			index++;
			if(index == MAX_TRUNK_GROUP_PORT_NUM)
				break;					
		}
	}

	trunk_port_num = index;
	printf("trunk_port_num = %d\n",trunk_port_num);

	if(trunk_port_num == 0) {
		fal_port_dot1d_state_set(0, lport, FAL_PORT_DOT1D_STATE_BLOCKING);
		return SW_OK;
	}

	/* Set Trunk Mask Table for load balancing. */
	trunk_mask = 0x7FF;
	trunk_mask &= ~hwport_vec;
	for(k=0; k<8; k++) {
		index = k % trunk_port_num;
		mask = trunk_mask | (1 << trunk_port_array[index]);
		if((status = gsysSetTrunkMaskTable(pJwDev, k, mask)) != GT_OK) {
			printf("gsysSetTrunkMaskTable error");
			return SW_FAIL;
		}
	}

	fal_port_dot1d_state_set(0, lport, FAL_PORT_DOT1D_STATE_BLOCKING);

	printf("ubus call trunk delete number success!\n");
	return SW_OK;
}

sw_error_t trunk_link_status_change(fal_port_t lport, fal_port_link_status_t link_status)
{
	GT_STATUS ret;
	fal_trunk_id_t trunk_id;

	if((trunk_id = trunk_find_group(lport)) == 0xFF)
	{
	///	printf("Error: Not a TrunkMember change link status, trunk_id = 0x%02X\n",trunk_id);
		return SW_FAIL;
	}
		printf("trunk_id = %d\n",trunk_id);

	switch(link_status) {
		case FAL_PORT_LINK_STATUS_DOWN:
			if((ret = trunk_delete_number(trunk_id, lport)) != GT_OK) {
				printf("trunk_delete_number error");
				return SW_FAIL;
			}
		break;

		default: /* LINK_UP */
			if((ret = trunk_add_number(trunk_id, lport)) != GT_OK) {
				printf("trunk_add_number error");
				return SW_FAIL;
			}	
		break;
	}
	return SW_OK;
}

static inline sw_error_t _mv88e6097_trunk_pag_mode_set(a_uint32_t dev_id,fal_trunk_mode_t pag_mode)
{
	a_uint32_t i, j;
	a_uint32_t trunk_id;
	a_uint32_t hport, lport;
	GT_STATUS ret;
	a_uint32_t lport_vec=0,index=0;
	
	TrunkCommit_S *trunk,*trunk_bak;
	
	switch(pag_mode) {
		case FAL_TRUNK_MODE_DISABLE:
			Pag_Mode = 0;
			TrunkCount = 0;

			list_for_each_entry_safe(trunk, trunk_bak, &TrunkCommitList, node)
			{	
				lport_vec |= trunk->TrunkGroupRec.TrunkMemberMask;

				list_del(&trunk->node);
				free(trunk);
			}
			printf("trunk disable: \n");
			for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {		
				if((lport_vec >> i) & 0x01) {			
					lport = (i+1);
					SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));

					/* Forwarding the previous blocking trunk port to forwarding */
					fal_port_dot1d_state_set(0, lport, FAL_PORT_DOT1D_STATE_FORWARDING);
					
					printf("	lport_%d  ",lport);

					/* max trunk group port num 8 */
					index++;
					if(index > MAX_TRUNK_GROUP_PORT_NUM)
						return SW_BAD_PARAM;
							
					/*if((ret = gprtSetTrunkPort(pJwDev, hport, GT_FALSE, trunk_id)) != GT_OK) {
						printf("gprtSetTrunkPort error\n");
						return SW_FAIL;
					}*/			
				} 		
			}
			printf("\n");

			/* destroy all trunk 1 ~ MAX_TRUNK_GROUP_ID */
			for(i=1; i<=MAX_TRUNK_GROUP_ID; i++) {
				trunk_id = i;	
				for(j=1; j<CONFIG_JWS_PORT_TOTAL_NR+1; j++) {
					lport = j;
					SW_RTN_ON_ERROR(map_lport2hport(lport, &hport));
					
					if((ret = gprtSetTrunkPort(pJwDev, hport, GT_FALSE, trunk_id)) != GT_OK) {
						printf("gprtSetTrunkPort error");
						return SW_FAIL;
					}	
				}

				if((ret = gsysSetTrunkRouting(pJwDev, trunk_id, 0)) != GT_OK) {
					printf("gsysSetTrunkRouting error \n");
					return SW_FAIL;
				}	
			}

			/* Set TrunkMaskTable as 0x7FF */
			for(i=0; i<8; i++) {
		        if((ret = gsysSetTrunkMaskTable(pJwDev, i, 0x7FF)) != GT_OK) {
		            printf(("gsysSetTrunkMaskTable return Failed\n"));
		            return SW_FAIL;
		        }
		    }
			
			break;
		case FAL_TRUNK_MODE_STATIC:
			Pag_Mode = 1;
			break;
		default:
			break;
	}

	return SW_OK;
}

static inline sw_error_t _mv88e6097_trunk_pag_mode_get(a_uint32_t dev_id,fal_trunk_mode_t * pag_mode)
{
	if(Pag_Mode == 1)
		*pag_mode = FAL_TRUNK_MODE_STATIC;
	else	
		*pag_mode = FAL_TRUNK_MODE_DISABLE;

	return SW_OK;
}

static inline sw_error_t _mv88e6097_trunk_load_balance_set(a_uint32_t dev_id, fal_trunk_load_balance_t mode)
{
	GT_STATUS ret;
	
	if(Pag_Mode == 1) {
		switch(mode) {
			/* GT_TRUE to use HASH, GT_FALSE to use XOR.*/
			case FAL_TRUNK_LOAD_BALANCE_XOR:		
				if((ret = gsysSetHashTrunk(pJwDev, GT_FALSE)) != GT_OK) {
					return SW_FAIL;
				}			
				break;
				
			case FAL_TRUNK_LOAD_BALANCE_HASH:
				if((ret = gsysSetHashTrunk(pJwDev, GT_TRUE)) != GT_OK) {
					return SW_FAIL;
				}
				break;
				
			default:
				break;
		}
	}else {
		printf("set trunk fail,not a static mode\n");
		return SW_FAIL;
	}
	
	return SW_OK;

}

static inline sw_error_t _mv88e6097_trunk_load_balance_get(a_uint32_t dev_id, fal_trunk_load_balance_t *mode)
{
	GT_STATUS ret;
	GT_BOOL	load_balance;

	if(Pag_Mode == 1) { 
		if((ret = gsysGetHashTrunk(pJwDev, &load_balance)) != GT_OK) {
			return SW_FAIL;
		}
		/* load_balance == GT_TRUE,HASH, else xor */
		if(load_balance == GT_TRUE) {
			*mode = FAL_TRUNK_LOAD_BALANCE_HASH;
		} else {
			*mode = FAL_TRUNK_LOAD_BALANCE_XOR;
		}
	} else{
		printf("trunk load balance get fail,not a static mode\n");
		return SW_FAIL;
	}
	return SW_OK;

}

static inline sw_error_t _mv88e6097_trunk_id_set(a_uint32_t dev_id, fal_trunk_id_t trunk_id)
{
	return SW_OK;

}

static inline sw_error_t _mv88e6097_trunk_members_set(a_uint32_t dev_id, fal_trunk_members_t *trunk_members)
{
	return SW_OK;

}

static inline sw_error_t _mv88e6097_trunk_row_create(a_uint32_t dev_id,fal_trunk_id_t trunk_id,fal_trunk_members_t *trunk_members,a_uint32_t member_size)
{
	GT_STATUS ret;
	a_uint32_t i, index=0;
	a_uint32_t lport;
	a_uint32_t hport_id,hport_vec=0;
	a_uint32_t tru_members;
	a_uint8_t member_byte[MV88E6097_MAX_MEMBERS_SIZE];
	a_uint32_t lport_vec=0;

	TrunkCommit_S *trunk,*trunk_bak;

	if(Pag_Mode == 1) {
		/* max trunk id is one to sixteen */
		if(trunk_id > MAX_TRUNK_GROUP_ID)
			return SW_BAD_PARAM;

		tru_members = 0;
 
		for(i=0; i<member_size; i++) {
			member_byte[i] = shift_high_low_bit(trunk_members->members[i]);
			tru_members |= (member_byte[i] << (i*8));
		}

		/* Initialize TrunkCommitList */
		TrunkCount++;
		if(TrunkCount > 8) {
		//	printf("Max TrunkCount is 8!\n");
		//	return SW_FAIL;
		}

		/* Delete the same trunk_id list, to add new list */
		list_for_each_entry_safe(trunk, trunk_bak, &TrunkCommitList, node)
		{
			if(trunk->TrunkGroupRec.Trunkid == trunk_id){
				lport_vec = trunk->TrunkGroupRec.TrunkMemberMask;
				
				list_del(&trunk->node);
				free(trunk);
			}
		}

		/* Remove the previous same trunk_id's trunk port, to reset this trunk */
		for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {		
			if((lport_vec >> i) & 0x01) {			
				lport = (i+1);
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport_id));
				
				/* Forwarding the previous trunk port */
				fal_port_dot1d_state_set(0, lport, FAL_PORT_DOT1D_STATE_FORWARDING);
						
				if((ret = gprtSetTrunkPort(pJwDev, hport_id, GT_FALSE, trunk_id)) != GT_OK) {
					printf("gprtSetTrunkPort error\n");
					return SW_FAIL;
				}			
			} 		
		}

		printf("Add trunk_members: \n");
		for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {
			if((tru_members >> i) & 0x01) {		
				/* GT_TRUE ,Enable trunk on the given port */
				lport = (i+1);
				SW_RTN_ON_ERROR(map_lport2hport(lport, &hport_id));

				hport_vec |= (1 << hport_id);

				printf("    lport_%d ",lport);

				/* max trunk group port num 8 */
				index++;
				if(index > MAX_TRUNK_GROUP_PORT_NUM)
					return SW_BAD_PARAM;
				
				if((ret = gprtSetTrunkPort(pJwDev, hport_id, GT_TRUE, trunk_id)) != GT_OK) {
					printf("gprtSetTrunkPort error\n");
					return SW_FAIL;
				}	
				
			}

		}
		printf("\n");

		trunk = (TrunkCommit_S *)calloc(1,sizeof(TrunkCommit_S));
		trunk->TrunkCount = TrunkCount;
	    trunk->TrunkGroupRec.Trunkid = trunk_id;
		trunk->TrunkGroupRec.TrunkPortNum = index;
		trunk->TrunkGroupRec.TrunkMemberMask = tru_members;
	    list_add(&trunk->node, &TrunkCommitList);

		/*if((ret = gsysSetTrunkRouting(pJwDev, trunk_id, hport_vec)) != GT_OK) {
			printf("gsysSetTrunkRouting error \n");
			return SW_FAIL;
		}	*/

	}else{
		printf("trunk create fail,not a static mode\n");
		return SW_FAIL;
	}
	return SW_OK;
	
}

static inline sw_error_t _mv88e6097_trunk_row_destroy(a_uint32_t dev_id,fal_trunk_id_t trunk_id)
{
	GT_STATUS ret;
	a_uint32_t i, index=0;
	a_uint32_t hport;
	a_uint32_t lport_id,lport_vec=0;

	TrunkCommit_S *trunk,*trunk_bak;


	if(Pag_Mode == 1) {
		/* max trunk id is one to sixteen */
		if(trunk_id > MAX_TRUNK_GROUP_ID)
			return SW_BAD_PARAM;
		
		/*if((ret = gsysGetTrunkRouting(pJwDev, trunk_id, &hport_vec)) != GT_OK) {
			return SW_FAIL;
		}*/

		list_for_each_entry_safe(trunk, trunk_bak, &TrunkCommitList, node)
		{
			if(trunk->TrunkGroupRec.Trunkid == trunk_id){
				lport_vec = trunk->TrunkGroupRec.TrunkMemberMask;
			}
		}
		
		printf("delete trunk_members:  ");

		for(i=0; i<CONFIG_JWS_PORT_TOTAL_NR; i++) {		
			if((lport_vec >> i) & 0x01) {			
				lport_id = (i+1);
				SW_RTN_ON_ERROR(map_lport2hport(lport_id, &hport));

				/* Forwarding the previous trunk port */
				fal_port_dot1d_state_set(0, lport_id, FAL_PORT_DOT1D_STATE_FORWARDING);
				
				printf("lport_%d  ",lport_id);

				/* max trunk group port num 8 */
				index++;
				if(index > MAX_TRUNK_GROUP_PORT_NUM)
					return SW_BAD_PARAM;
						
				if((ret = gprtSetTrunkPort(pJwDev, hport, GT_FALSE, trunk_id)) != GT_OK) {
					printf("gprtSetTrunkPort error\n");
					return SW_FAIL;
				}			
			} 		
		}

		printf("\n");

		if((ret = gsysSetTrunkRouting(pJwDev, trunk_id, 0)) != GT_OK) {
				printf("gsysSetTrunkRouting error \n");
				return SW_FAIL;
		}

		/* Set TrunkMaskTable as 0x7FF */
		for(i=0; i<8; i++) {
	        if((ret = gsysSetTrunkMaskTable(pJwDev, i, 0x7FF)) != GT_OK) {
	            printf(("gsysSetTrunkMaskTable return Failed\n"));
	            return SW_FAIL;
	        }
	    }

		TrunkCount--;
		list_for_each_entry_safe(trunk, trunk_bak, &TrunkCommitList, node)
	    {
	    	if(trunk->TrunkGroupRec.Trunkid == trunk_id) {
				trunk->TrunkCount = TrunkCount;
				
			//	printf("trunk->TrunkCount=%d, trunk-id=%d, trunk-member=%04x\n",trunk->TrunkCount,trunk->TrunkGroupRec.Trunkid,trunk->TrunkGroupRec.TrunkMemberMask);
				list_del(&trunk->node);
				free(trunk);
			}
	    }
	}else{
		printf("trunk destroy fail,not a static mode\n");	
		return SW_FAIL;
	}
		
	return SW_OK;

}






sw_error_t mv88e6097_trunk_pag_mode_set(a_uint32_t dev_id,fal_trunk_mode_t pag_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_trunk_pag_mode_set(dev_id, pag_mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_trunk_pag_mode_get(a_uint32_t dev_id,fal_trunk_mode_t * pag_mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_trunk_pag_mode_get(dev_id, pag_mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_trunk_load_balance_set(a_uint32_t dev_id, fal_trunk_load_balance_t mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_trunk_load_balance_set(dev_id, mode);
    HSL_API_UNLOCK;
	
    return rv;
}

sw_error_t mv88e6097_trunk_load_balance_get(a_uint32_t dev_id, fal_trunk_load_balance_t *mode)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_trunk_load_balance_get(dev_id, mode);
    HSL_API_UNLOCK;
	
    return rv;

}

sw_error_t mv88e6097_trunk_id_set(a_uint32_t dev_id, fal_trunk_id_t trunk_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_trunk_id_set(dev_id, trunk_id);
    HSL_API_UNLOCK;
	
    return rv;

}

sw_error_t mv88e6097_trunk_members_set(a_uint32_t dev_id, fal_trunk_members_t *trunk_members)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_trunk_members_set(dev_id, trunk_members);
    HSL_API_UNLOCK;
	
    return rv;

}

sw_error_t mv88e6097_trunk_row_create(a_uint32_t dev_id, fal_trunk_id_t trunk_id, fal_trunk_members_t *trunk_members, a_uint32_t member_size)
{  
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_trunk_row_create(dev_id, trunk_id, trunk_members, member_size);
    HSL_API_UNLOCK;
	
    return rv;

}

sw_error_t mv88e6097_trunk_row_destroy(a_uint32_t dev_id, fal_trunk_id_t trunk_id)
{
    sw_error_t rv;

    HSL_API_LOCK;
    rv = _mv88e6097_trunk_row_destroy(dev_id, trunk_id);
    HSL_API_UNLOCK;

	return rv;
}

sw_error_t mv88e6097_trunk_show_row_status(a_uint32_t dev_id, fal_trunk_id_t trunk_id)
{
	int i, j, k;
	a_uint32_t	lport, hport;
	fal_trunk_id_t TrunkId;
	char	port_member_str[36];
	GT_STATUS status;
	GT_U32 hwport_vec, trunk_mask;



	printf("\r\n");
	printf("  Port Aggregation Status Information Display :\r\n");
	printf("  ====================================================\r\n");
	printf("  TrunkID	   Port-Member							   \r\n");
	printf("  ====================================================\r\n");
	
	memset(port_member_str, 0, 36);

	TrunkId = trunk_id;
	/* Get Trunk Route Table for the given Trunk ID */
	if((status = gsysGetTrunkRouting(pJwDev, TrunkId, (GT_U32 *)&hwport_vec)) != GT_OK) {
		return SW_FAIL;
	}

	j=0;
	for(k=0; k<pJwDev->maxPorts; k++) {
		if(hwport_vec & (1<<k)) {
			hport = k;
			SW_RTN_ON_ERROR(map_hport2lport(hport, &lport));
			//lport = hal_swif_hport_2_lport(hport);
			if(j>8)
				break;
			sprintf(&port_member_str[j*4],"P%02d ", lport);
			j++;
		}
	}		
	printf("    %02d		    %s\r\n", TrunkId, port_member_str);
	printf("\r\n");

	printf("  Trunk Mask Table Display :\r\n");
	printf("  ====================================================\r\n");
	for(i=0; i<8; i++) {
		if((status = gsysGetTrunkMaskTable(pJwDev, i, &trunk_mask)) != GT_OK) {
			return SW_FAIL;
		}
		printf("  TrunkMask[%d]   ...	 0x%04X\r\n", i, (unsigned int)trunk_mask);
	}
	printf("  ====================================================\r\n");
	printf("\r\n");
	

	return SW_OK;

}




sw_error_t mv88e6097_port_trunk_init(a_uint32_t dev_id)
{
    hsl_api_t *p_api;
	hsl_dev_t *pdev = NULL;

    pdev = hsl_dev_ptr_get(dev_id);
    if (pdev == NULL)
        return SW_NOT_INITIALIZED;
	
    HSL_DEV_ID_CHECK(dev_id);
	
    SW_RTN_ON_NULL(p_api = hsl_api_ptr_get(dev_id));

	p_api->trunk_pag_mode_set = mv88e6097_trunk_pag_mode_set;
	p_api->trunk_pag_mode_get = mv88e6097_trunk_pag_mode_get;
	p_api->trunk_load_balance_set = mv88e6097_trunk_load_balance_set;
	p_api->trunk_load_balance_get = mv88e6097_trunk_load_balance_get;
	p_api->trunk_id_set = mv88e6097_trunk_id_set;
	p_api->trunk_members_set = mv88e6097_trunk_members_set;
	p_api->trunk_row_create = mv88e6097_trunk_row_create;
	p_api->trunk_row_destroy = mv88e6097_trunk_row_destroy;
	p_api->trunk_show_row_status = mv88e6097_trunk_show_row_status;
	
    return SW_OK;
}


