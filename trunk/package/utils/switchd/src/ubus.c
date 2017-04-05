/*
 * Copyright (C) 2013 Felix Fietkau <nbd@openwrt.org>
 * Copyright (C) 2013 John Crispin <blogic@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <sys/resource.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> 

#include "switchd.h"
#include <string.h>

#include <jwf-tool.h>
#include "joyware.h"
#include <sys/select.h>

char *ubus_socket = NULL;
static struct ubus_context *ctx;
static struct uloop_timeout ubus_timer;
static struct uloop_timeout jssdk_timer;
static struct uloop_timeout jssdk_comb_timer;
static LIST_HEAD(uciCommitList);
static LIST_HEAD(extCommitList);

/* Ubus call jssdk function */
extern int ubus_poll_fdb_mac_create(void);
extern int getPortConfig(int port_idx, PortConfig *physical_mode);
extern sw_error_t trunk_link_status_change(fal_port_t lport, fal_port_link_status_t link_status);
extern sw_error_t comb_port_fiber_link_status_detect(fal_port_t lport,unsigned int *media_status);
extern sw_error_t comb_port_led_mode_select(fal_port_t lport,unsigned int media_link);
extern int lport2hport(unsigned int lport, unsigned int *hport);

extern int Led_mode[2];
extern fal_port_comb_type_t CombPort_type[2];

static void
ubus_reconnect_cb(struct uloop_timeout *timeout)
{
	if (!ubus_reconnect(ctx, ubus_socket))
		ubus_add_uloop(ctx);
	else
		uloop_timeout_set(timeout, 2000);
}

static void
ubus_disconnect_cb(struct ubus_context *ctx)
{
	ubus_timer.cb = ubus_reconnect_cb;
	uloop_timeout_set(&ubus_timer, 2000);
}

#define MV88E1112PHY_Fiber_Energy_Detect 	0x10
#define MV88E1112PHY_Fiber_Link			 	0x80
#define MV88E1112PHY_100M_Fiber_Link		0x4000
#define MV88E1112PHY_Realtime_link 			0x400//0x600
#define MV88E1112PHY_Fiber_or_Copper_link	0x08
#define MV88E1112PHY_FIBER_LED_MODE			1//0x1288
#define MV88E1112PHY_COPPER_LED_MODE		0//0x8217

#define MV88E6097Switch_PCS_register		0x01
#define MV88E6097Switch_Global_Ctrl_R		0x04
static void jssdk_comb_timer_handler(struct uloop_timeout *timeout)
{
	int ret;
	fal_port_t lport;
	fal_port_link_status_t link_status;
	unsigned int fiber_signal;
	unsigned int signal;
	
	/* delay 20ms */
	struct timeval delay_time;
	delay_time.tv_sec = 0;
	delay_time.tv_usec = 20*1000;
	PortConfig portconfig;


	/* Comb led mode dynamic setting ,only port 1 and port 2*/
	for(lport=1; lport<=CONFIG_JWS_PORT_TOTAL_NR; lport++)
	{
		/* Get port config */
		getPortConfig(lport, &portconfig);
		
		/* Only comb port's physical mode is fiber-auto support this function*/
		if(portconfig.port_comb == FAL_PORT_COMB_PORT) {
			/* Only comb port set auto-fiber detect use this function */
			if(portconfig.phy_mode == FAL_PORT_PHYSICAL_MODE_FIBER_AUTO) {						
				/* Fiber energy detect: 1000base-x or 100base-fx */
				ret = comb_port_fiber_link_status_detect(lport,&fiber_signal);
				if(ret)
				{
		            LOG_ERROR("ubus poll comb_port_fiber_link_status_detect error!\n");
		        }
				/* if 0: Fiber signal detected, 1: no Fiber signal detect */
				signal = (fiber_signal & MV88E1112PHY_Fiber_Energy_Detect) >> 4;
				if(signal == 0) {
					LOG_NORMAL(" Lport_%d Combport_type = %d \n",lport,CombPort_type[lport-1]);
					switch(CombPort_type[lport-1]) {
						case FAL_PORT_COMB_AUTO_FIBER_COPPER:	
							LOG_NORMAL("Lport_%d in Auto-detect mode Auto-detect Down\n",lport);
							/* Force 1000Base-X mode */
							ret = fal_port_comb_type_set(0,lport,FAL_PORT_COMB_FORCE_1000BASE_X);
							if(ret)
							{
					            LOG_ERROR("ubus poll fal_port_comb_type_set error!\n");
					        }
							select(0,NULL,NULL,NULL,&delay_time);
							ret = comb_port_fiber_link_status_detect(lport,&fiber_signal);
							if(ret)
							{
					            LOG_ERROR("ubus poll comb_port_fiber_link_status_detect error!\n");
					        }
							
							/* if 1: Fiber link */
							signal = (fiber_signal & MV88E1112PHY_Fiber_Link) >> 7;
							/* Fiber energy detected, and Fiber realtime link up */
							if(signal == 1) {
								/* Fiber Link, doing nothing */
								CombPort_type[lport-1] = FAL_PORT_COMB_FORCE_1000BASE_X;
								LOG_NORMAL("Lport_%d in Auto-detect mode 1000base-x Fiber Link\n",lport);
							}
						break;
						
						case FAL_PORT_COMB_FORCE_1000BASE_X:
							ret = comb_port_fiber_link_status_detect(lport,&fiber_signal);
							if(ret)
							{
					            LOG_ERROR("ubus poll comb_port_fiber_link_status_detect error!\n");
					        }
							/* if 1: Fiber link */
							signal = (fiber_signal & MV88E1112PHY_Fiber_Link) >> 7;
							/* Fiber energy detected, and Fiber realtime link up */
							if((signal == 1) && ((fiber_signal & MV88E1112PHY_Realtime_link) >> 10)){
								/* Fiber Link, doing nothing */
								CombPort_type[lport-1] = FAL_PORT_COMB_FORCE_1000BASE_X;
								LOG_NORMAL("Lport_%d in 1000Base-X mode 1000base-x Fiber Link fiber_signal = 0x%x\n",lport,fiber_signal);
							} else{
								LOG_NORMAL("Lport_%d in 1000Base-X mode 1000base-x Fiber Down fiber_signal = 0x%x\n",lport,fiber_signal);
								ret = fal_port_comb_type_set(0,lport,FAL_PORT_COMB_FORCE_1000BASE_X);
								if(ret)
								{
						            LOG_ERROR("ubus poll fal_port_comb_type_set error!\n");
						        }
								select(0,NULL,NULL,NULL,&delay_time);
								ret = comb_port_fiber_link_status_detect(lport,&fiber_signal);
								if(ret)
								{
						            LOG_ERROR("ubus poll comb_port_fiber_link_status_detect error!\n");
						        }
								
								/* if 1: Fiber link */
								signal = (fiber_signal & MV88E1112PHY_Fiber_Link) >> 7;
								/* Fiber energy detected, and Fiber realtime link up */
								if((signal == 1) && ((fiber_signal & MV88E1112PHY_Realtime_link) >> 10)){
									/* Fiber Link, doing nothing */
									CombPort_type[lport-1] = FAL_PORT_COMB_FORCE_1000BASE_X;
									LOG_NORMAL("Lport_%d in 1000Base-X mode 1000base-x Fiber Link fiber_signal = 0x%x\n",lport,fiber_signal);
								} else{
									LOG_NORMAL("Lport_%d in 1000Base-X mode 1000base-x Fiber Down fiber_signal = 0x%x\n",lport,fiber_signal);
									ret = fal_port_comb_type_set(0,lport,FAL_PORT_COMB_FORCE_100BASE_FX);
									if(ret)
									{
							            LOG_ERROR("ubus poll fal_port_comb_type_set error!\n");
							        }
									select(0,NULL,NULL,NULL,&delay_time);
									ret = comb_port_fiber_link_status_detect(lport,&fiber_signal);
									if(ret)
									{
							            LOG_ERROR("ubus poll comb_port_fiber_link_status_detect error!\n");
							        }
									
									/* if 1: Fiber link */
									signal = (fiber_signal & MV88E1112PHY_Fiber_Link) >> 7;
									if(signal == 1){
										/* Fiber Link, doing nothing, wait for jssdk timer timeout to enable this physical-mode */
										CombPort_type[lport-1] = FAL_PORT_COMB_FORCE_100BASE_FX;
										LOG_NORMAL("Lport_%d in 1000Base-X mode 100base-fx Fiber Link fiber_signal = 0x%x\n",lport,fiber_signal);
									}else{
										LOG_NORMAL("Lport_%d in 1000Base-X mode 100base-fx Fiber Down fiber_signal = 0x%x\n",lport,fiber_signal);
									}
								}
							}
							
						break;

						case FAL_PORT_COMB_FORCE_100BASE_FX:
							ret = comb_port_fiber_link_status_detect(lport,&fiber_signal);
							if(ret)
							{
					            LOG_ERROR("ubus poll comb_port_fiber_link_status_detect error!\n");
					        }
							/* if 1: Fiber link */
							signal = (fiber_signal & MV88E1112PHY_Fiber_Link) >> 7;
							/* Fiber energy detected, and Fiber realtime link up */
							if((signal == 1) && ((fiber_signal & MV88E1112PHY_Realtime_link) >> 10)){
								/* Fiber Link, doing nothing */
								CombPort_type[lport-1] = FAL_PORT_COMB_FORCE_100BASE_FX;
								LOG_NORMAL("Lport_%d in 100Base-FX mode 100base-fx Fiber Link fiber_signal = 0x%x\n",lport,fiber_signal);
							} else{
								LOG_NORMAL("Lport_%d in 100Base-FX mode 100base-fx Fiber Down fiber_signal = 0x%x\n",lport,fiber_signal);
								ret = fal_port_comb_type_set(0,lport,FAL_PORT_COMB_FORCE_100BASE_FX);
								if(ret)
								{
						            LOG_ERROR("ubus poll fal_port_comb_type_set error!\n");
						        }
								select(0,NULL,NULL,NULL,&delay_time);
								ret = comb_port_fiber_link_status_detect(lport,&fiber_signal);								
								if(ret)
								{
						            LOG_ERROR("ubus poll comb_port_fiber_link_status_detect error!\n");
						        }
								
								/* if 1: Fiber link */
								signal = (fiber_signal & MV88E1112PHY_Fiber_Link) >> 7;
								/* Fiber energy detected, and Fiber realtime link up */
								if((signal == 1) && ((fiber_signal & MV88E1112PHY_Realtime_link) >> 10)){
									/* Fiber Link, doing nothing */
									CombPort_type[lport-1] = FAL_PORT_COMB_FORCE_100BASE_FX;
									LOG_NORMAL("Lport_%d in 100Base-FX mode 100base-fx Fiber Link fiber_signal = 0x%x\n",lport,fiber_signal);
								} else{
									LOG_NORMAL("Lport_%d in 100Base-FX mode 100base-fx Fiber Down fiber_signal = 0x%x\n",lport,fiber_signal);
									ret = fal_port_comb_type_set(0,lport,FAL_PORT_COMB_FORCE_1000BASE_X);
									if(ret)
									{
							            LOG_ERROR("ubus poll fal_port_comb_type_set error!\n");
							        }
									select(0,NULL,NULL,NULL,&delay_time);
									ret = comb_port_fiber_link_status_detect(lport,&fiber_signal);									
									if(ret)
									{
							            LOG_ERROR("ubus poll comb_port_fiber_link_status_detect error!\n");
							        }
									
									/* if 1: Fiber link */
									signal = (fiber_signal & MV88E1112PHY_Fiber_Link) >> 7;
									if(signal == 1){
										/* Fiber Link, doing nothing, wait for jssdk timer timeout to enable this physical-mode */
										CombPort_type[lport-1] = FAL_PORT_COMB_FORCE_1000BASE_X;
										LOG_NORMAL("Lport_%d in 100Base-FX mode 1000base-x Fiber Link fiber_signal = 0x%x\n",lport,fiber_signal);
									}else {
										LOG_NORMAL("Lport_%d in 100Base-FX mode 1000base-x Fiber Down fiber_signal = 0x%x\n",lport,fiber_signal);
									}
								}
							}	
						break;

						default:
						break;
					}
				}else{
					/* No fiber detected, recover the comb type to auto-detect */
					ret = fal_port_comb_type_set(0,lport,FAL_PORT_COMB_AUTO_FIBER_COPPER);
					if(ret)
					{
			            LOG_ERROR("ubus poll fal_port_comb_type_set error!\n");
			        }
					CombPort_type[lport-1] = FAL_PORT_COMB_AUTO_FIBER_COPPER;
				}
			}
		} 		
	}	
	uloop_timeout_set(&jssdk_comb_timer, 2000);
}

/* Comb port Polling */
static void jssdk_timer_handler(struct uloop_timeout *timeout)
{
	int ret;
	fal_port_t lport,hport;
	//fal_enable_t admin_mode;
	fal_port_link_status_t link_status;
	//a_uint32_t pcs_status;
	unsigned int fiber_status, signal;
	unsigned int media;
	static unsigned int CurrLinkMap = 0, PrevLinkMap = 0, Link_DownUp_Map, Link_UpDown_Map;
	static unsigned int PPU_Enable_Map=0;

	PortConfig portconfig;
	unsigned int pcs_mode, pcs_status, pcs_data;
	unsigned int regAddr;

	/* Polling link status in order to dynamic change trunk status */
	for(lport=1; lport<=CONFIG_JWS_PORT_TOTAL_NR; lport++)
	{
		/* Get port config */
		getPortConfig(lport, &portconfig);
		/* Comb led mode dynamic setting */
		if(portconfig.port_comb == FAL_PORT_COMB_PORT) {
			ret = fal_port_link_status_get(0, lport, &link_status);
			if(ret)
			{
	            LOG_ERROR("ubus poll fal_port_link_status_get error!\n");
	        } else {
				if(link_status == FAL_PORT_LINK_STATUS_UP)
					CurrLinkMap |= (1<<(lport-1));
				else if(link_status == FAL_PORT_LINK_STATUS_DOWN)
					CurrLinkMap &= ~(1<<(lport-1));
			}
		}
	}

	if(CurrLinkMap ^ PrevLinkMap) {
		Link_DownUp_Map = CurrLinkMap & (CurrLinkMap ^ PrevLinkMap);
		Link_UpDown_Map = PrevLinkMap & (CurrLinkMap ^ PrevLinkMap);
		for(lport=1; lport<=CONFIG_JWS_PORT_TOTAL_NR; lport++) {
			/* Get port config */
			getPortConfig(lport, &portconfig);
			/* Comb led mode dynamic setting */
			if(portconfig.port_comb == FAL_PORT_COMB_PORT) {
				if(Link_DownUp_Map & (1<<(lport-1))) {
					LOG_NORMAL("lport_%d Link Up\n",lport);

					trunk_link_status_change(lport, FAL_PORT_LINK_STATUS_UP);
				}
				if(Link_UpDown_Map & (1<<(lport-1))) {
					LOG_NORMAL("lport_%d Link Down\n",lport);
					
					/* Flush link status up to down port's ATU */
					ret = fal_mac_flush_port(0, lport);
					if(ret)
					{
			            LOG_ERROR("ubus poll fal_mac_flush_port error!\n");
			        }
					trunk_link_status_change(lport, FAL_PORT_LINK_STATUS_DOWN);
				}
			}
		}
	}

	PrevLinkMap = CurrLinkMap;
	//LOG_NORMAL("PrevLinkMap = %04x, CurrLinkMap = %04x\n",PrevLinkMap,CurrLinkMap);

	/* Detect for comb port led select */
	for(lport=1; lport<=CONFIG_JWS_PORT_TOTAL_NR; lport++)
	{
		/* Get port config */
		getPortConfig(lport, &portconfig);

		/* Comb led mode dynamic setting */
		if(portconfig.port_comb == FAL_PORT_COMB_PORT) {
			ret = comb_port_fiber_link_status_detect(lport,&fiber_status);
			if(ret)
			{
	            LOG_ERROR("ubus poll comb_port_fiber_link_status_detect error!\n");
	        }
		
			/* Fiber or Copper link up */
			if((fiber_status & MV88E1112PHY_Fiber_or_Copper_link) >> 3){
				/* 1:Fiber link led, 0:Copper link led */
				media = (fiber_status & MV88E1112PHY_Fiber_Link) >> 7;
				if(media){
					if(Led_mode[lport-1] != MV88E1112PHY_FIBER_LED_MODE){
						//LOG_NORMAL("Fiber, fiber_status = 0x%x , led_mode = %d\n",fiber_status,Led_mode[lport-1]);
						ret = comb_port_led_mode_select(lport, media);
						if(ret)
						{
				            LOG_ERROR("ubus poll comb_port_led_mode_select error!\n");
				        }
						Led_mode[lport-1] = 1;
					}
				}else{
					if(Led_mode[lport-1] != MV88E1112PHY_COPPER_LED_MODE){
						//LOG_NORMAL("Copper, fiber_status = 0x%x , led_mode = %d\n",fiber_status,Led_mode[lport-1]);
						ret = comb_port_led_mode_select(lport, media);
						if(ret)
						{
				            LOG_ERROR("ubus poll comb_port_led_mode_select error!\n");
				        }
						Led_mode[lport-1] = 0;
					}
				}
			}	
		}	
	}

	/* Comb port link status solve, when not real link up, set normal operation */
	for(lport=1; lport<=CONFIG_JWS_PORT_TOTAL_NR; lport++)
	{
		/* Get port config */
		getPortConfig(lport, &portconfig);
		
		/* Comb led mode dynamic setting */
		if(portconfig.port_comb == FAL_PORT_COMB_PORT) {	
			/* if 0: Fiber signal detected, 1: no Fiber signal detect */
			ret = comb_port_fiber_link_status_detect(lport,&fiber_status);
			if(ret)
			{
	            LOG_ERROR("ubus poll comb_port_fiber_link_status_detect error!\n");
	        }

			/* Port Register: 0x10+hport */
			lport2hport(lport,&hport);
			regAddr = ((0x10+hport)<<8) | MV88E6097Switch_PCS_register;
			
			ret = fal_reg_get(0,regAddr,&pcs_mode,4);
			if(ret)
			{
	            LOG_ERROR("ubus poll fal_reg_get error!\n");
	        }
			
			//switch(portconfig.comb_type) {
			switch(CombPort_type[lport-1]) {	
				case FAL_PORT_COMB_FORCE_100BASE_FX:
					/* Realtime link up */
					if((fiber_status & MV88E1112PHY_Realtime_link) >> 10){
						/* Force PCS link up, full-duplex, 100M */
						if((pcs_mode & 0x3f) != 0x3d){
							ret = fal_reg_get(0,regAddr,&pcs_status,4);
							if(ret)
							{
					            LOG_ERROR("ubus poll fal_reg_get error!\n");
					        }
							/* pcs_data will be set the real vaule to PCS Register */
							pcs_data = pcs_status & 0xFFC0;
							pcs_data = pcs_data | 0x3d;

							ret = fal_reg_set(0,regAddr,&pcs_data,4);
							if(ret)
							{
					            LOG_ERROR("ubus poll fal_reg_set error!\n");
					        }
						}
					}else{
						/* Not Force PCS link and duplex status, Normal operation */
						signal = (fiber_status & MV88E1112PHY_Fiber_Energy_Detect) >> 4;
						/* Only no Fiber energy detect, polling, enable Flow control */
						if(signal == 1) {	
							/* Normal operation */
							if((pcs_mode & 0x3f) != 0x03){
								ret = fal_reg_get(0,regAddr,&pcs_status,4);
								if(ret)
								{
						            LOG_ERROR("ubus poll fal_reg_get error!\n");
						        }
								/* pcs_data will be set the real vaule to PCS Register */
								pcs_data = pcs_status & 0xFFC0;
								pcs_data = pcs_data | 0x3;

								ret = fal_reg_set(0,regAddr,&pcs_data,4);
								if(ret)
								{
						            LOG_ERROR("ubus poll fal_reg_set error!\n");
						        }
							}
						}
					}
				break;
				
				default:
				break;
			}
		}
	}

	/* Only GE_Z20210D use Polling to polling FDB */
	if(ubus_poll_fdb_mac_create())
    {
		LOG_ERROR("Connection to ubus failed\n");
    }
	
	uloop_timeout_set(&jssdk_timer, 50);
}

static void
ubus_connect_cb(struct uloop_timeout *timeout)
{
	ctx = ubus_connect(ubus_socket);

	if (!ctx) {
		LOG_ERROR("Connection to ubus failed\n");
		uloop_timeout_set(&ubus_timer, 1000);
		return;
	}

	ctx->connection_lost = ubus_disconnect_cb;

	LOG_NORMAL("Connected to ubus, id=%08x\n", ctx->local_id);
	ubus_add_uloop(ctx);
	ubus_init_sysinfo(ctx);     // MIB - jwsDevGeneral
    ubus_init_port_config(ctx); // MIB - jwsIfExtPortInfo
    ubus_init_rate_limit(ctx);  // MIB - jwsPriRateLimitGroup
    ubus_init_port_mirror(ctx); // MIB - jwsMirrorConfigGroup
    ubus_init_eth_stats(ctx);       // MIB - jwsIfExtEthStats
	ubus_init_port_aggr(ctx);       // MIB - jwsPagMibGroup
    ubus_init_mcast_config(ctx);    // MIB - jwsStaticMcastGroup
    ubus_init_vlan_config(ctx);     // MIB - jwsVlanMngtGroup
    ubus_init_qos_config(ctx);      // MIB - jwsQosConfigGroup
    ubus_init_lldp_config(ctx);     // MIB - jwsLldpMib
    ubus_init_network(ctx);         // MIB - jwsDevNetworkConfig
    ubus_init_trap(ctx);            // MIB - jwsTrapMib
    ubus_init_snmp(ctx);            // MIB - jwsDevSnmpConfig
    ubus_init_fdb(ctx);             // MIB - jwsFdbMib
    ubus_init_rstp(ctx);            // MIB - jwsrstpMib
    ubus_init_ring(ctx);            // MIB - jwsringMib
    ubus_init_gpio(ctx);
    //ubus_init_8021x(ctx);
    //switchd_state_ubus_connect();
    
	/* GPIO interrupt Init */
	if(gpioInterruptInit() < 0)
    {
        LOG_ERROR("init gpio interrupt error\n");
        return -1;
    }
}
 
void ubus_collect_config(struct uci_ptr *ptr,char *value,enum uci_option_type type,UciDo_S action)
{
    UciCommit_S *ci,*ci_bak;

   // if(!((value == NULL) || (ptr->option == NULL))){// 增加/删除节点时不需要比较  
//    if(ptr->package != "jrppd"){
    if(strcmp(ptr->package, "jrppd")){
        if(type == UCI_TYPE_STRING)
        {

            list_for_each_entry_safe(ci, ci_bak, &uciCommitList, node)
            {
                if(ci->ptr.option == NULL){
                    continue;

                }
                if(!(strcmp(ci->ptr.section,ptr->section) || strcmp(ci->ptr.option,ptr->option)))
                {
                    free((void *)ci->ptr.value);
                    ci->ptr.value = strdup(value);
                    LOG_NORMAL("same section and option,merged\n");
                    return; 
                }
            }
        }
    }

    ci = (UciCommit_S *)calloc(1,sizeof(UciCommit_S));
    memcpy(&ci->ptr,ptr,sizeof(struct uci_ptr));
    ci->type = type;
    if(value !=NULL)
        ci->ptr.value = strdup(value);
    ci->action = action;

    list_add_tail(&ci->node,&uciCommitList);
}

void ubus_collect_config_ext(int (*func)(void))
{
    ExtCommit_S *ci = (ExtCommit_S *)calloc(1,sizeof(ExtCommit_S));
    ci->func = func;
    
    list_add(&ci->node,&extCommitList);
}


int ubus_commit_config(void)
{
    if(!list_empty(&extCommitList))
    {
        ExtCommit_S *ext,*ext_bak;
        list_for_each_entry_safe(ext, ext_bak, &extCommitList, node)
        {
            LOG_NORMAL("get one node in ext commit list\n");
            ext->func();
            list_del(&ext->node);
            free(ext);
        }
    }
    
    /**<    commit changes which can not be set by C API  */
//    system("uci commit snmpd");
//    system("uci commit rstpd");

    /**<    commit changes which can be set by C API  */
    if(list_empty(&uciCommitList))
    {
        LOG_NORMAL("no arguement need to be commit to UCI file\n");
        return 0;
    }

    struct uci_context *ctx = uci_alloc_context();
    int ret;
    UciCommit_S *ci,*ci_bak;
    struct uci_ptr ptr = {
    .package = NULL, 
    };

    list_for_each_entry_safe(ci, ci_bak, &uciCommitList, node)
    {
        if(ci->action == DO_SET){
            ret = set_config(ctx,&ci->ptr,ci->type);
        }
        else if(ci->action == DO_DEL){
            if(ci->ptr.option == NULL){//删除节点
                char name[16];
                sprintf(name,"%s.%s",ci->ptr.package, ci->ptr.section);
                struct uci_ptr ptr1;
                if(UCI_OK != uci_lookup_ptr(ctx,&ptr1,name,true)){
                    LOG_WARN("no matching arguement: %s!\n",name); 
                    return -1;
                }
                ret = del_config(ctx,&ptr1,ci->type);
            }else{
                ret = del_config(ctx,&ci->ptr,ci->type); 
            }
        }

        if(ret)
        {
            LOG_ERROR("ubus set config in UCI format fail!\n"); 
            return -1;
        }

        /**<    if package has been changed,commit old UCI file  */
        if(ptr.package == NULL)
        {
            LOG_NORMAL("this is package = NULL\n");
            if(ci->ptr.option == NULL){
                char name2[16];
                sprintf(name2,"%s",ci->ptr.package);
                struct uci_ptr ptr2;
                if(UCI_OK != uci_lookup_ptr(ctx,&ptr2,name2,true)){
                    LOG_WARN("no matching arguement: %s!\n",name2); 
                    return -1;
                }
                memcpy(&ptr,&ptr2,sizeof(struct uci_ptr));
            }else{
                memcpy(&ptr,&ci->ptr,sizeof(struct uci_ptr));
            }
        }
        else if(strcmp(ptr.package,ci->ptr.package))
        {
            LOG_NORMAL("this is package not equal\n");
            ret = uci_commit(ctx,&ptr.p,false);
            if(ret)
            {
                LOG_ERROR("ubus commit fail!\n"); 
                return -1;
            }
            //snmpd相关配置生效
            if((!strcmp(ptr.package, "snmpd")) || ((!strcmp(ptr.package, "switchd"))&&(!strcmp(ptr.section, "trap")))){
                system("/etc/init.d/snmpd restart");
            }
            if(!strcmp(ptr.package, "jrppd")){
                system("/etc/init.d/jrppd stop");
                system("/etc/init.d/jrppd start"); 
            }
            //if(!strcmp(ptr.package, "rstpd")){
            //    system("/etc/init.d/rstpd stop");
            //    system("/etc/init.d/rstpd start"); 
            //}
        
            if(ci->ptr.option == NULL){
                char name2[16];
                sprintf(name2,"%s",ci->ptr.package);
                struct uci_ptr ptr2;
                if(UCI_OK != uci_lookup_ptr(ctx,&ptr2,name2,true)){
                    LOG_WARN("no matching arguement: %s!\n",name2); 
                    return -1;
                }
                memcpy(&ptr,&ptr2,sizeof(struct uci_ptr));
            }else{
                memcpy(&ptr,&ci->ptr,sizeof(struct uci_ptr));
            }

        }

        LOG_NORMAL("package name: %s, section name: %s, option name: %s\n",ci->ptr.package, ci->ptr.section,ci->ptr.option);
        list_del(&ci->node);
        free((void *)ci->ptr.value);
//        if( !strcmp(ci->ptr.package, "jrppd")) //jrppd节点是 释放 section 拷贝内存
        if((ci->action == DO_DEL) && (ci->ptr.option == NULL)) //删除UCI节点是 释放 section 拷贝内存
            free((void *)ci->ptr.section);
        free(ci);

//        LOG_NORMAL("package name: %s, section name: %s, option name: %s\n", ptr.package,ptr.section,ptr.option);
    }

    /**<    commit last UCI file  */
    ret = uci_commit(ctx,&ptr.p,false);
    if(ret)
    {
        LOG_ERROR("ubus commit fail!\n"); 
        return -1;
    }
    
    //snmpd相关配置生效
    if((!strcmp(ptr.package, "snmpd")) || ((!strcmp(ptr.package, "switchd"))&&(!strcmp(ptr.section, "trap")))){
        system("/etc/init.d/snmpd restart");
    }
    if(!strcmp(ptr.package, "jrppd")){
        system("/etc/init.d/jrppd stop");
        system("/etc/init.d/jrppd start"); 
    }
    //if(!strcmp(ptr.package, "rstpd")){
    //    system("/etc/init.d/rstpd stop");
    //    system("/etc/init.d/rstpd start"); 
    //}

    uci_free_context(ctx);

    
    return 0;
}

int ubus_reset_config(void)
{
    //system("cp -f /etc/config/.switchd /etc/config/switchd");
    system("mount -t jffs2 /dev/mtdblock4 /overlay");
    system("cp -f /usr/share/defconf.tgz /overlay/sysupgrade.tgz");
    system("sync");
    system("umount /overlay");
    system("reboot");
    return 0;
}

void switchd_connect_ubus(void)
{
	char id[]=CONFIG_JWS_SERIAL_NUMBER;
	
	ubus_timer.cb = ubus_connect_cb;
	uloop_timeout_set(&ubus_timer, 1000);

    jssdk_timer.cb = jssdk_timer_handler;
	jssdk_comb_timer.cb = jssdk_comb_timer_handler;

	/* Only GE_Z20210D use this two timer to polling */
	if((!strcasecmp(id, "53-20-01-0C-06-01-00-00")) || (!strcasecmp(id, "53-20-01-08-06-01-00-00"))){
		uloop_timeout_set(&jssdk_timer, 1000);
		uloop_timeout_set(&jssdk_comb_timer, 2000);
	}
}
