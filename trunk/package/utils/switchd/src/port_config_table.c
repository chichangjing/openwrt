/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file eth_stats_table.c
 * * @brief This is a table of the switch's port ethernet statistics.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@obtelecom.com>
 * * @date 2016-04-25
 * Created it
 * */
/* *****************************************************************
 * *@note
 * Copyright 2012, OB Telecom Electronics Co., Ltd.
 * ALL RIGHTS RESERVED
 * Permission is hereby granted to licensees of OB Telecom Electronics
 * Co., Ltd. products to use or abstract this computer program for
 * the sole purpose of implementing a product based on OB Telecom
 * Electronics Co., Ltd. products. No other rights to reproduce, use,
 * or disseminate this computer program,whether in part or in whole,
 * are granted. OB Telecom Electronics Co., Ltd. makes no
 * representation or warranties with respect to the performance of
 * this computer program, and specifically disclaims any
 * responsibility for any damages, special or consequential,
 * connected with the use of this program.
 * For details, see http://obtelecom.com
 * *******************************************************************/
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

#include "switchd.h"


static struct blob_buf b;
static struct ubus_context *_ctx;

enum {
    PORT_CFG_GET_KEYS,
    __PORT_CFG_GET_MAX
};

enum {
    PORT_CFG_SET_ARRAY,
    __PORT_CFG_SET_MAX
};

static const struct blobmsg_policy port_config_get_policy[] = {
    [PORT_CFG_GET_KEYS] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy port_config_set_policy[] = {
    [PORT_CFG_SET_ARRAY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    PORT_CFG_TYPE,
    PORT_CFG_COMB,
    PORT_CFG_COMB_TYPE,
    PORT_CFG_ADMIN_MODE,
    PORT_CFG_LINK_STATUS,
    PORT_CFG_PHYSICAL_MODE,
    PORT_CFG_PHYSICAL_STATUS,
    PORT_CFG_FLOW_CONTROL_MODE,
    PORT_CFG_STATE,
    PORT_CFG_FLASH_MAC,
    __PORT_CFG_TBL_MAX,
};

static struct{
    fal_port_comb_t comb;
    fal_port_comb_type_t type;
    fal_enable_t sw;
    fal_port_physical_mode_t phy_mode;
    fal_enable_t flw_ctr;
}portCfg[CONFIG_JWS_PORT_TOTAL_NR];

/* jssdk function */
extern sw_error_t comb_port_led_mode_select(fal_port_t lport,unsigned int media_link);

/* *
 * * @brief             get port type
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portGetType(struct blob_buf *buf, int port_idx)
{
    fal_port_type_t type;
    fal_port_comb_t comb;
    char type_s[2];
    sw_error_t res;

    res = fal_port_type_get(0,port_idx,&type, &comb);
    //LOG_NORMAL("port [%d]  get comb = [%d] OK\n",port_idx,comb);
    
    if(res)
    {
        LOG_ERROR("port [%d] get type error = %d!\n",port_idx,res);
        blobmsg_add_string(buf, "type", "-1");
    }
    else
    {
        sprintf(type_s,"%d",type);
        LOG_NORMAL("port [%d] get type = [%s] OK\n",port_idx,type_s);
        blobmsg_add_string(buf, "type", type_s);
    }

    return 0;
}

/* *
 * * @brief             get port comb type
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portGetCombType(struct blob_buf *buf, int port_idx)
{
    char comb_mode_s[2];

    sprintf(comb_mode_s,"%d",portCfg[port_idx-1].type);
    LOG_NORMAL("port [%d] get comb type = [%s] OK\n",port_idx,comb_mode_s);
    blobmsg_add_string(buf, "comb_type", comb_mode_s);

    return 0;
}
/* *
 * * @brief             set port type
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */

/* 2 Comb port */
int Led_mode[2];
fal_port_comb_type_t CombPort_type[2];
static int portSetCombType(int port_idx, void *p)
{
    fal_port_comb_type_t status = atoi((char *)p);
    sw_error_t res;

    if(portCfg[port_idx-1].comb == FAL_PORT_NOT_COMB_PORT){
        LOG_NORMAL("port[%d] is not comb port\n", port_idx);
        return 0;
    }
    
    if(status != FAL_PORT_COMB_FORCE_1000BASE_X && status != FAL_PORT_COMB_FORCE_100BASE_FX \
       && status != FAL_PORT_COMB_FORCE_COPPER && status != FAL_PORT_COMB_AUTO_FIBER_COPPER)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    res = fal_port_comb_type_set(0,port_idx,status);
    if(res)
    {
        LOG_ERROR("set port comb type error  = %d!\n",res);
        return -1;
    }
    
    portCfg[port_idx-1].type = status;
	
	/* for jssdk comb port led mode dynamic setting, 1:fiber led mode, 0:copper led mode */
	if((status == FAL_PORT_COMB_FORCE_1000BASE_X) || (status == FAL_PORT_COMB_FORCE_100BASE_FX)){
		res = comb_port_led_mode_select(port_idx, 1);
		if(res)
		{
            LOG_ERROR("ubus poll comb_port_led_mode_select error!\n");
			return -1;
        }
		Led_mode[port_idx-1] = 1;

		if(status == FAL_PORT_COMB_FORCE_1000BASE_X){
			CombPort_type[port_idx-1] = FAL_PORT_COMB_FORCE_1000BASE_X;
			printf("lport_%d CombPort_type=%d\n",port_idx,CombPort_type[port_idx-1]);
		}else if(status == FAL_PORT_COMB_FORCE_100BASE_FX){
			CombPort_type[port_idx-1] = FAL_PORT_COMB_FORCE_100BASE_FX;
			printf("lport_%d CombPort_type=%d\n",port_idx,CombPort_type[port_idx-1]);
		}
	}else if(status == FAL_PORT_COMB_FORCE_COPPER){
		res = comb_port_led_mode_select(port_idx, 0);
		if(res)
		{
            LOG_ERROR("ubus poll comb_port_led_mode_select error!\n");
			return -1;
        }
		Led_mode[port_idx-1] = 0;
	}else if(status == FAL_PORT_COMB_AUTO_FIBER_COPPER){
		CombPort_type[port_idx-1] = FAL_PORT_COMB_AUTO_FIBER_COPPER;
		printf("lport_%d CombPort_type=%d\n",port_idx,CombPort_type[port_idx-1]);
		/* Led_mode init not 0,1, for dynamic setting auto-detect led mode */
		Led_mode[port_idx-1] = 2;
	}
    
    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "port_config",
        .option  = "comb_type",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            portCfg[0].type,portCfg[1].type,portCfg[2].type,portCfg[3].type,portCfg[4].type,\
            portCfg[5].type,portCfg[6].type,portCfg[7].type,portCfg[8].type,portCfg[9].type);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);
    LOG_NORMAL("set port [%d] comb type [%d] OK!\n",port_idx, status);
    
    return 0;
}

/* *
 * * @brief             get port comb
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portGetCombMode(struct blob_buf *buf, int port_idx)
{
    fal_port_type_t type;
    fal_port_comb_t comb;
    char comb_s[2];
    sw_error_t res;

    res = fal_port_type_get(0,port_idx,&type, &comb);
    
    if(res)
    {
        LOG_ERROR("port [%d] get comb error = %d!\n",port_idx,res);
        blobmsg_add_string(buf, "comb_mode", "-1");
    }
    else
    {
        sprintf(comb_s,"%d",comb);
        LOG_NORMAL("port [%d] get comb = [%s] OK\n",port_idx,comb_s);
        blobmsg_add_string(buf, "comb_mode", comb_s);
    }

    return 0;
}

/* *
 * * @brief             get port admin mode
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portGetAdminMode(struct blob_buf *buf, int port_idx)
{
    char admin_mode_s[2];

    sprintf(admin_mode_s,"%d",portCfg[port_idx-1].sw);
    LOG_NORMAL("port [%d] get admin mode = [%s] OK\n",port_idx,admin_mode_s);
    blobmsg_add_string(buf, "admin_mode", admin_mode_s);

    return 0;
}


/* *
 * * @brief             get port link status
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portGetOperateStatus(struct blob_buf *buf, int port_idx)
{
    fal_port_link_status_t link_status;
    char link_status_s[2];
    sw_error_t res;

    res = fal_port_link_status_get(0,port_idx,&link_status);

    if(res)
    {
        LOG_ERROR("port [%d] get link status error = %d!\n",port_idx,res);
        blobmsg_add_string(buf, "operational_status", "-1");
    }
    else
    {
        sprintf(link_status_s,"%d",link_status);
        //LOG_NORMAL("port [%d] get port link status = [%s]\n",port_idx,link_status_s);
        blobmsg_add_string(buf, "operational_status", link_status_s);
    }

    return 0;
}


/* *
 * * @brief             get port physical mode
 * * @param[port_idx]   port index
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portGetPhysicalMode(struct blob_buf *buf,int port_idx)
{
    char phy_mode_s[2];

    sprintf(phy_mode_s,"%d",portCfg[port_idx-1].phy_mode);
    LOG_NORMAL("port [%d] get physical mode = [%s]\n",port_idx,phy_mode_s);
    blobmsg_add_string(buf, "physical_mode", phy_mode_s);

    return 0;
}

/* *
 * * @brief             get Port Config for jssdk_timer
 * * @param[port_idx]   port index
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */

int getPortConfig(int port_idx, PortConfig *physical_mode)
{

    physical_mode->phy_mode = portCfg[port_idx-1].phy_mode;
	physical_mode->port_comb = portCfg[port_idx-1].comb;
	physical_mode->comb_type = portCfg[port_idx-1].type;

    return 0;
}


/* *
 * * @brief             get port physical status
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portGetPhysicalStatus(struct blob_buf *buf, int port_idx)
{
    fal_port_physical_status_t phy_status;
    char phy_status_s[2];
    sw_error_t res;
    
    res = fal_port_physical_status_get(0,port_idx,&phy_status);
    

    if(res)
    {
        LOG_ERROR("port [%d] get physical status error = %d!\n",port_idx,res);
        blobmsg_add_string(buf, "physical_status", "-1");
    }
    else
    {
        sprintf(phy_status_s,"%d",phy_status);
        LOG_NORMAL("port [%d] get physical status = %s\n",port_idx,phy_status_s);
        blobmsg_add_string(buf, "physical_status", phy_status_s);
    }


    return 0;
}


/* *
 * * @brief             get port flow control mode
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portGetFlowControlMode(struct blob_buf *buf, int port_idx)
{
    char fcm_s[2];
    
    sprintf(fcm_s,"%d",portCfg[port_idx-1].flw_ctr);
    LOG_NORMAL("port [%d] get flow control mode = [%s]\n",port_idx,fcm_s);
    blobmsg_add_string(buf, "flow_control_mode", fcm_s);

    return 0;
}


/* *
 * * @brief             get port stp state
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portGetStpState(struct blob_buf *buf, int port_idx)
{
    char stp_state_s[2];

    fal_port_dot1d_state_t  stp_status;
    sw_error_t res;
    
    res = fal_port_dot1d_state_get(0,port_idx,&stp_status);
    if(res)
    {
        LOG_ERROR("port [%d] get stp status error = %d!\n",port_idx,res);
        blobmsg_add_string(buf, "dot1d_status", "-1");
    }
    else
    {
        sprintf(stp_state_s,"%d",stp_status);
        LOG_NORMAL("port [%d] get stp status = %s\n",port_idx,stp_state_s);
        blobmsg_add_string(buf, "dot1d_status", stp_state_s);
    }

    return 0;
}

/* *
 * * @brief             set port stp state
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portSetStpState(int port_idx, void *p)
{
    fal_port_dot1d_state_t status = atoi((char *)p);
    sw_error_t res;
    
    if(status != FAL_PORT_DOT1D_STATE_DISABLED && status != FAL_PORT_DOT1D_STATE_BLOCKING \
       && status != FAL_PORT_DOT1D_STATE_LEARNING && status != FAL_PORT_DOT1D_STATE_FORWARDING)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }
    
    res = fal_port_dot1d_state_set(0,port_idx,status);
    if(res)
    {
        LOG_ERROR("set port stp state error = %d!\n",res);
        return -1;
    }
    LOG_NORMAL("set port [%d] stp state [%d] OK!\n",port_idx, status);
    
    return 0;
}


/* *
 * * @brief             flash port mac
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portGetFlashMac(struct blob_buf *buf, int port_idx)
{

    sw_error_t res;
    
    res = fal_mac_flush_port(0,port_idx);
    if(res)
    {
        LOG_ERROR("port [%d] flash mac error = %d!\n",port_idx,res);
        blobmsg_add_string(buf, "flash_mac", "-1");
    }
    else
    {
        LOG_NORMAL("port [%d] flash mac ok\n",port_idx);
        blobmsg_add_string(buf, "flash_mac", "0");
    }

    return 0;
}

/* *
 * * @brief             set admin mode
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portSetAdminMode(int port_idx, void *p)
{
    fal_enable_t am = atoi((char *)p);
    sw_error_t res;

    if(am == portCfg[port_idx-1].sw)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }

    if(am != FAL_ENABLE && am != FAL_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    res = fal_port_admin_mode_set(0,port_idx,am);
    if(res)
    {
        LOG_ERROR("set port admin mode error = %d!\n",res);
        return -1;
    }

    portCfg[port_idx-1].sw = am;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "port_config",
        .option = "admin_mode",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            portCfg[0].sw,portCfg[1].sw,portCfg[2].sw,portCfg[3].sw,portCfg[4].sw,\
            portCfg[5].sw,portCfg[6].sw,portCfg[7].sw,portCfg[8].sw,portCfg[9].sw);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set port [%d] admin mode [%d] OK!\n",port_idx, am);
    
    return 0;
}


/* *
 * * @brief             set port physical mode
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portSetPhysicalMode(int port_idx, void *p)
{
    fal_port_physical_mode_t pm = atoi((char *)p);
    sw_error_t ret;

    if(portCfg[port_idx-1].sw != FAL_ENABLE)
    {
        LOG_ERROR("function of port config disable!\n");
        return -1;
    }

    if(pm == portCfg[port_idx-1].phy_mode)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }

    fal_port_type_t type;
    fal_port_comb_t comb;
    if(fal_port_type_get(0,port_idx,&type, &comb))
    {
        LOG_ERROR("get port %d type fail!\n",port_idx); 
        return -1;
    }

    if((FAL_PORT_TYPE_GX == type && (pm != FAL_PORT_PHYSICAL_MODE_AUTONEG && pm != FAL_PORT_PHYSICAL_MODE_S1000_FULL)) || (FAL_PORT_TYPE_FE == type && (pm < FAL_PORT_PHYSICAL_MODE_AUTONEG || pm > FAL_PORT_PHYSICAL_MODE_S100_FULL)))
    {
        LOG_ERROR("invalid arguement\n"); 
        return -1;
    }

    ret = fal_port_physical_mode_set(0,port_idx,pm);
    if(ret)
    {
        LOG_ERROR("set port physical mode error = %d!\n",ret);
        return -1;
    }
    
    portCfg[port_idx-1].phy_mode = pm;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "port_config",
        .option = "physical_mode",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            portCfg[0].phy_mode,portCfg[1].phy_mode,portCfg[2].phy_mode,portCfg[3].phy_mode,portCfg[4].phy_mode,\
            portCfg[5].phy_mode,portCfg[6].phy_mode,portCfg[7].phy_mode,portCfg[8].phy_mode,portCfg[9].phy_mode);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set port [%d] physical mode = [%d] OK!\n",port_idx, pm);
    
    return 0;
}


/* *
 * * @brief             set port flow control mode
 * * @param[port_idx]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portSetFlowControlMode(int port_idx, void *p)
{
    fal_enable_t fcm = atoi((char *)p);
    sw_error_t res;

    if(portCfg[port_idx-1].sw != FAL_ENABLE)
    {
        LOG_ERROR("function of port config disable!\n");
        return -1;
    }

    if(fcm == portCfg[port_idx-1].flw_ctr)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }

    if(fcm != FAL_ENABLE && fcm != FAL_DISABLE)
    {
        LOG_ERROR("invalid arguement"); 
        return -1;
    }

    res = fal_port_flowctrl_mode_set(0,port_idx,fcm);
    if(res)
    {
        LOG_ERROR("fal_port_flowctrl_mode_set error = %d!\n",res);
        return -1;
    }
    
    portCfg[port_idx-1].flw_ctr = fcm;

    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "port_config",
        .option = "flow_control_mode",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            portCfg[0].flw_ctr,portCfg[1].flw_ctr,portCfg[2].flw_ctr,portCfg[3].flw_ctr,portCfg[4].flw_ctr,\
            portCfg[5].flw_ctr,portCfg[6].flw_ctr,portCfg[7].flw_ctr,portCfg[8].flw_ctr,portCfg[9].flw_ctr);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set port [%d] flow control mode = [%d] OK!\n",port_idx, fcm);
        
    return 0;
}


static const struct jw_switch_policy port_config_tbl[] = {
    [PORT_CFG_TYPE] = {.name = "type", .get_handler = portGetType},
    [PORT_CFG_COMB] = {.name = "comb_mode", .get_handler = portGetCombMode},
    [PORT_CFG_COMB_TYPE] = {.name = "comb_type", .get_handler = portGetCombType, .set_handler = portSetCombType},
    [PORT_CFG_ADMIN_MODE] = {.name = "admin_mode", .get_handler = portGetAdminMode, .set_handler = portSetAdminMode}, 
    [PORT_CFG_LINK_STATUS] = {.name = "operational_status", .get_handler = portGetOperateStatus},
    [PORT_CFG_PHYSICAL_MODE] = {.name = "physical_mode", .get_handler = portGetPhysicalMode, .set_handler = portSetPhysicalMode},
    [PORT_CFG_PHYSICAL_STATUS] = {.name = "physical_status", .get_handler = portGetPhysicalStatus},
    [PORT_CFG_FLOW_CONTROL_MODE] = {.name = "flow_control_mode", .get_handler = portGetFlowControlMode, .set_handler = portSetFlowControlMode},
    [PORT_CFG_STATE] = {.name = "dot1d_state", .get_handler = portGetStpState, .set_handler = portSetStpState},
    [PORT_CFG_FLASH_MAC] = {.name = "flash_mac", .get_handler = portGetFlashMac},
};


/* *
 * * @brief         parse port config get
 * * @param[lvl3]   level 3 message .
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int portParseGet(struct blob_attr *lvl3, struct blob_buf *buf)
{
    const char *name    = blobmsg_name(lvl3);
    int idx             = atoi(name + strlen(PORT_IDX));

    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        //LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }

    void *ret_array;

    ret_array= blobmsg_open_array(buf, name);
    foreach_json_array_get(idx,lvl3,buf,port_config_tbl, __PORT_CFG_TBL_MAX);
    blobmsg_close_array(buf, ret_array);	

    return 0;
}


/* *
 * * @brief         parse port config set
 * * @param[lvl3]   level 3 message.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int portParseSet(struct blob_attr *lvl3)
{
    const char *name    = blobmsg_name(lvl3);
    int idx             = atoi(name + strlen(PORT_IDX));

    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }
    
    /**<    foreach array of level 3 message  */
    int ret;
    ret = foreach_json_array_set(idx,lvl3,port_config_tbl, __PORT_CFG_TBL_MAX);
    if(ret)
    {
        LOG_ERROR("foreach index %d error!\n",idx);
        return -1;
    }

    return 0;
}

static int portGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__PORT_CFG_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(port_config_get_policy, __PORT_CFG_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[PORT_CFG_GET_KEYS];     
    if (!lvl1) 
	{
        LOG_ERROR("invalid params in %s\n",__func__);
        return -1;
    }

	if (blobmsg_type(lvl1) != BLOBMSG_TYPE_ARRAY) 
	{
        LOG_ERROR("blobmsg type is not array\n");
        return -1;
    }

    struct blob_attr *lvl2 = NULL; 
    int rem;
    void *ret_array;
    void *ret_table;
		
	blob_buf_init(&b, 0);
    ret_array = blobmsg_open_array(&b, "ret"); 
    
    blobmsg_for_each_attr(lvl2,lvl1, rem) 
    {
        if(blobmsg_type(lvl2) == BLOBMSG_TYPE_TABLE)
        {
            struct blob_attr *lvl3 = blobmsg_data(lvl2);

            if(blobmsg_type(lvl3) == BLOBMSG_TYPE_ARRAY)
            {
                ret_table = blobmsg_open_table(&b, NULL);
                portParseGet(lvl3,&b);
                blobmsg_close_table(&b, ret_table);	
            }
            else
            {
                LOG_ERROR("level 3 message type is not available!\n");
            }
        }
        else
        {
            LOG_ERROR("level 2 message type is not available!\n");
        }
    }    

    blobmsg_close_array(&b, ret_array);

	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;
}


static int portSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__PORT_CFG_SET_MAX];
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(port_config_set_policy, __PORT_CFG_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[PORT_CFG_SET_ARRAY];     
    if (!lvl1) 
	{
        LOG_ERROR("invalid params in %s\n",__func__);
        goto end;
    }
    
    if (blobmsg_type(lvl1) != BLOBMSG_TYPE_ARRAY) 
	{
        LOG_ERROR("level 1 message type is not array\n");
        goto end;
    }

    struct blob_attr *lvl2 = NULL;
    int rem;

	blob_buf_init(&b, 0);


    blobmsg_for_each_attr(lvl2, lvl1, rem) 
    {
        if (blobmsg_type(lvl2) == BLOBMSG_TYPE_TABLE) 
        {
            struct blob_attr *lvl3 = blobmsg_data(lvl2);
            
            if(blobmsg_type(lvl3) == BLOBMSG_TYPE_ARRAY)
            {
                ret = portParseSet(lvl3);
                if(ret)
                    goto end;
            }
            else
            {
                LOG_ERROR("level 3 message type is not available!\n");
                ret = -1;
                goto end;
            }
        }
        else
        {
            LOG_ERROR("level 2 message type is not available!\n");
            ret = -1;
            goto end;
        }
    }
     
end:
    blobmsg_add_string(&b, "ret", ret?"-1":"0");
	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;
}


static const struct ubus_method port_config_methods[] = {
	UBUS_METHOD("port_config_get", portGetHandler, port_config_get_policy),
	UBUS_METHOD("port_config_set", portSetHandler, port_config_set_policy),
};


static struct ubus_object_type switch_object_type =
	UBUS_OBJECT_TYPE("port_config", port_config_methods);


static struct ubus_object switch_object = {
	.name = "port_config",
	.type = &switch_object_type,
	.methods = port_config_methods,
	.n_methods = ARRAY_SIZE(port_config_methods),
};

/* *
 * * @brief             load port configure arguement - admin_mode from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portLoadCombMode(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    fal_port_comb_t mode;
    int port;
    int ret;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("comb mode format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("comb mode arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("comb mode format in UCI file error!\n"); 
            return -1;
        }

        mode = atoi(pdelim + 1);
        if(mode != FAL_PORT_NOT_COMB_PORT && mode != FAL_PORT_COMB_PORT)
        {
            LOG_ERROR("comb mode arguement is invalid!\n");
            return -1;
        }

        portCfg[port-1].comb = mode;
    }
    free(string);

    LOG_NORMAL("port configure load comb mode arguement OK\n");
    
    return 0;
}


/* *
 * * @brief             load port configure arguement - admin_mode from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portLoadTypeMode(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    fal_port_comb_type_t mode;
    int port;
    int ret;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("type mode format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("type mode arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("type mode format in UCI file error!\n"); 
            return -1;
        }

        mode = atoi(pdelim + 1);
        if(mode != FAL_PORT_COMB_FORCE_1000BASE_X && mode != FAL_PORT_COMB_FORCE_100BASE_FX \
            && mode != FAL_PORT_COMB_FORCE_COPPER && mode != FAL_PORT_COMB_AUTO_FIBER_COPPER \
            && mode != FAL_PORT_COMB_FORCE_100M_COPPER)
        {
            LOG_ERROR("type mode arguement is invalid!\n");
            return -1;
        }
        if(portCfg[port-1].comb == FAL_PORT_COMB_PORT){
            ret = fal_port_comb_type_set(0,port,mode);
            if(ret)
            {
                LOG_ERROR("set port type mode error = %d!\n",ret);
                return -1;
            }

			/* for jssdk comb port led mode dynamic setting, 1:fiber led mode, 0:copper led mode */
			if((mode == FAL_PORT_COMB_FORCE_1000BASE_X) || (mode == FAL_PORT_COMB_FORCE_100BASE_FX)){
				ret = comb_port_led_mode_select(port, 1);
				if(ret)
				{
		            LOG_ERROR("ubus poll comb_port_led_mode_select error!\n");
					return -1;
		        }
				Led_mode[port-1] = 1;

				if(mode == FAL_PORT_COMB_FORCE_1000BASE_X){
					CombPort_type[port-1] = FAL_PORT_COMB_FORCE_1000BASE_X;
					printf("lport_%d CombPort_type=%d\n",port,CombPort_type[port-1]);
				}else if(mode == FAL_PORT_COMB_FORCE_100BASE_FX){
					CombPort_type[port-1] = FAL_PORT_COMB_FORCE_100BASE_FX;
					printf("lport_%d CombPort_type=%d\n",port,CombPort_type[port-1]);
				}
			}else if(mode == FAL_PORT_COMB_FORCE_COPPER){
				ret = comb_port_led_mode_select(port, 0);
				if(ret)
				{
		            LOG_ERROR("ubus poll comb_port_led_mode_select error!\n");
					return -1;
		        }
				Led_mode[port-1] = 0;
			}else if(mode == FAL_PORT_COMB_AUTO_FIBER_COPPER){
				CombPort_type[port-1] = FAL_PORT_COMB_AUTO_FIBER_COPPER;
				printf("lport_%d CombPort_type=%d\n",port,CombPort_type[port-1]);
				/* Led_mode init not 0,1, for dynamic setting auto-detect led mode */
				Led_mode[port-1] = 2;
			}
		    
        }

        portCfg[port-1].type = mode;
    }
    free(string);

    LOG_NORMAL("port configure load type mode arguement OK\n");
    
    return 0;
}

/* *
 * * @brief             load port configure arguement - admin_mode from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portLoadAdminMode(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int mode;
    int port;
    int ret;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("admin mode format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("admin mode arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("admin mode format in UCI file error!\n"); 
            return -1;
        }

        mode = atoi(pdelim + 1);
        if(mode != FAL_ENABLE && mode != FAL_DISABLE)
        {
            LOG_ERROR("admin mode arguement is invalid!\n");
            return -1;
        }

        ret = fal_port_admin_mode_set(0,port,mode);
        if(ret)
        {
            LOG_ERROR("set port admin mode error = %d!\n",ret);
            return -1;
        }
        portCfg[port-1].sw = mode;
    }
    free(string);

    LOG_NORMAL("port configure load admin mode arguement OK\n");
    
    return 0;
}

/* *
 * * @brief             load port configure arguement - physical_mode from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portLoadPhysicalMode(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int mode;
    int port;
    int ret;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("physical mode format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("physical mode arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("physical mode format in UCI file error!\n"); 
            return -1;
        }

        mode = atoi(pdelim + 1);
        if(mode < FAL_PORT_PHYSICAL_MODE_AUTONEG || mode > FAL_PORT_PHYSICAL_MODE_FIBER_AUTO )
        {
            LOG_ERROR("physical mode arguement is invalid!\n");
            return -1;
        }
        
        ret = fal_port_physical_mode_set(0,port,mode);
        if(ret)
        {
            LOG_ERROR("set physical mode error = %d!\n",ret);
            return -1;
        }
        portCfg[port-1].phy_mode = mode; 
    }
    free(string);

    LOG_NORMAL("port configure load physical mode arguement OK\n");
    
    return 0;
}

/* *
 * * @brief             load port configure arguement - flow_control_mode from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portLoadFlowControlMode(struct uci_ptr *ptr)
{
    char *string = strdup(ptr->o->v.string);
    char *pstr;
    int mode;
    int port;
    int ret;

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("flow control mode format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("flow control mode arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("flow control mode format in UCI file error!\n"); 
            return -1;
        }

        mode = atoi(pdelim + 1);
        if(mode != FAL_ENABLE && mode != FAL_DISABLE)
        {
            LOG_ERROR("flow control mode arguement is invalid!\n");
            return -1;
        }
        
        ret = fal_port_flowctrl_mode_set(0,port,mode);
        if(ret)
        {
            LOG_ERROR("set port %d flow control mode = %d error = %d!\n",port,mode,ret);
            return -1;
        }
        portCfg[port-1].flw_ctr = mode;       
    }
    free(string);

    LOG_NORMAL("port configure load flow control mode arguement OK\n");
    
    return 0;
}

static UciConfig_S portUci[] = {
    {.name = "comb_mode",           .type = UCI_TYPE_STRING,    .ubusGet = portLoadCombMode},
    {.name = "comb_type",           .type = UCI_TYPE_STRING,    .ubusGet = portLoadTypeMode},
    {.name = "admin_mode",          .type = UCI_TYPE_STRING,    .ubusGet = portLoadAdminMode},
    {.name = "physical_mode",       .type = UCI_TYPE_STRING,    .ubusGet = portLoadPhysicalMode},
    {.name = "flow_control_mode",   .type = UCI_TYPE_STRING,    .ubusGet = portLoadFlowControlMode},
};

/* *
 * * @brief             load portor configure
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int portCfgLoad(void)
{
    struct uci_context *ctx;

    ctx = uci_alloc_context();

    char name[64]; 
    int ret;
    int i;
    struct uci_ptr ptr;

    for(i = 0;i < sizeof(portUci)/sizeof(UciConfig_S);i++)
    {
        sprintf(name,"switchd.port_config.%s",portUci[i].name);
        ret = load_config(name,portUci[i].type,&ptr,ctx);
        if(ret)
        {
            uci_free_context(ctx);

            if(portUci[i].type == UCI_TYPE_LIST) continue;

            LOG_ERROR("load config %s fail!\n",name); 
            return -1;
        }
        
        ret = portUci[i].ubusGet(&ptr);
        if(ret)
        {
            uci_free_context(ctx);

            LOG_ERROR("ubus get arguement %s fail!\n",name); 
            return -1;
        }
    }

    uci_free_context(ctx);

    return 0; 
}

void ubus_init_port_config(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;

    ret = portCfgLoad();
    if(ret)
    {
		LOG_ERROR("Init port configuration fail!\n");
        return;
    }

	ret = ubus_add_object(ctx, &switch_object);
	if (ret)
		LOG_ERROR("Failed to add object [port_config]: %s\n", ubus_strerror(ret));
}
