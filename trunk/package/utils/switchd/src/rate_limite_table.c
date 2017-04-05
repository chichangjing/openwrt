/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file rate_limite_table.c
 * * @brief This is a table of the switch's port rate limite.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@obtelecom.com>
 * * @date 2016-05-11
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
    RATE_LIMIT_GET_ARRAY,
    __RATE_LIMIT_GET_MAX,
};

enum {
    RATE_LIMIT_SET_ARRAY,
    __RATE_LIMIT_SET_MAX,
};

static const struct blobmsg_policy rate_limit_get_policy[] = {
    [RATE_LIMIT_GET_ARRAY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy rate_limit_set_policy[] = {
    [RATE_LIMIT_SET_ARRAY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    INGRESS_ENABLE,
    INGRESS_TYPE_MASK,
    INGRESS_RATE,
    EGRESS_ENABLE,
    EGRESS_RATE,
    __RATE_LIMIT_TBL_MAX,
};


#define SPEED_LO_LEVEL 64
#define SPEED_HI_LEVEL 1000000
#define SPEED_F_HI_LEVEL 100000
#define MAX_TYPE 0x1F

#define BUCKET_NUM 5
static struct{
    struct{
        fal_enable_t enable;
        fal_rate_speed_t rate;
    }egress;
    struct{
        fal_enable_t enable;
        fal_rate_mask_t type;
        fal_rate_speed_t rate;
    }ingress[BUCKET_NUM];
}ratelimit[CONFIG_JWS_PORT_TOTAL_NR];

const char *bucket[3][BUCKET_NUM] = {
    {"ingress_bucket0_enable","ingress_bucket1_enable","ingress_bucket2_enable","ingress_bucket3_enable","ingress_bucket4_enable"},
    {"ingress_bucket0_type_mask","ingress_bucket1_type_mask","ingress_bucket2_type_mask","ingress_bucket3_type_mask","ingress_bucket4_type_mask"},
    {"ingress_bucket0_rate","ingress_bucket1_rate","ingress_bucket2_rate","ingress_bucket3_rate","ingress_bucket4_rate"}};

/* *
 * * @brief     get ingress typemask
 * * @param[port] port index.
 * * @retval    0: OK
 * * @retval    -1: ERROR
 * */
static int ratelimitGetIngressTypemask(struct blob_buf *buf, int port,int bkt)
{
    char type_mask_s[3];

    sprintf(type_mask_s,"%d",ratelimit[port-1].ingress[bkt].type);
    LOG_NORMAL("rate limit ingress bucket%d type = %s\n",bkt,type_mask_s);
    blobmsg_add_string(buf, bucket[INGRESS_TYPE_MASK][bkt], type_mask_s);

    return 0;
}

/* *
 * * @brief     set ingress typemask
 * * @param[port] port index.
 * * @param[p]  typemask bits[0-4]
 * * @retval    0: OK
 * * @retval    -1: ERROR
 * */
static int ratelimitSetIngressTypemask(int port,int bkt, void *p)
{
    fal_rate_mask_t type = atoi((char *)p);
    sw_error_t ret;

    /**<    check arguement  */
    if(ratelimit[port-1].ingress[bkt].enable != FAL_ENABLE)
    {
        LOG_WARN("the feature for ingress type in port [%d] | bucket [%d] is disable!\n",port,bkt);
        return 0;
    }

    if(type == ratelimit[port-1].ingress[bkt].type)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }

    int i;
    for(i = 0;i < 5;i++)
    {
        if(i == bkt)
            continue;

        if(type & ratelimit[port-1].ingress[i].type)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }
    }

    /**<    set rate limit if avaliable  */
    if(ratelimit[port-1].ingress[bkt].rate)
    {
        ret = fal_rate_bucket_pirl_set(0,port,bkt,type,ratelimit[port-1].ingress[bkt].rate,FAL_ENABLE);
        if(ret)
        {
            LOG_ERROR("set ingress rate limit fail,errno = [%d]!\n",ret);
            return -1;
        }
    }

    ratelimit[port-1].ingress[bkt].type= type;

    char buf[256];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "rate_limit",
        .option = bucket[INGRESS_TYPE_MASK][bkt],
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            ratelimit[0].ingress[bkt].type,ratelimit[1].ingress[bkt].type,ratelimit[2].ingress[bkt].type,\
            ratelimit[3].ingress[bkt].type,ratelimit[4].ingress[bkt].type,ratelimit[5].ingress[bkt].type,\
            ratelimit[6].ingress[bkt].type,ratelimit[7].ingress[bkt].type,ratelimit[8].ingress[bkt].type,\
            ratelimit[9].ingress[bkt].type);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set port [%d] | bucket [%d] ingress type = [%d] OK\n",port,bkt,type);

    return 0;
}

/* *
 * * @brief     get ingress rate 
 * * @param[port] port index.
 * * @retval    0: OK
 * * @retval    -1: ERROR
 * */
static int ratelimitGetIngressRate(struct blob_buf *buf, int port,int bkt)
{
    char rate_s[10];

    sprintf(rate_s,"%d",ratelimit[port-1].ingress[bkt].rate);
    LOG_NORMAL(" port [%d] | bucket [%d] ingress rate = %s\n",port,bkt,rate_s);
    blobmsg_add_string(buf, bucket[INGRESS_RATE][bkt], rate_s);

    return 0;
}

/* *
 * * @brief     set ingress rate
 * * @param[port] port index.
 * * @param[p]  rate value 64~100000
 * * @retval    0: OK
 * * @retval    -1: ERROR
 * */
static int ratelimitSetIngressRate(int port,int bkt, void *p)
{
    int rate = atoi((char *)p);
    sw_error_t ret;
    fal_port_type_t type;
    
    /**<    check arguement  */
    if(rate == ratelimit[port-1].ingress[bkt].rate)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    type = get_port_type(port);

    if((type == FAL_PORT_TYPE_FE)||(type == FAL_PORT_TYPE_FX)){
        if(rate < SPEED_LO_LEVEL || rate > SPEED_F_HI_LEVEL)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }
    }else{
        if(rate < SPEED_LO_LEVEL || rate > SPEED_HI_LEVEL)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }
    }
    if(ratelimit[port-1].ingress[bkt].enable != FAL_ENABLE)
    {
        LOG_ERROR("the feature for ingress rate in port [%d] | bucket [%d] is disable!\n",port,bkt);
        return -1;
    }

    /**<    set rate limit if avaliable  */
    if(ratelimit[port-1].ingress[bkt].type)
    {
        ret = fal_rate_bucket_pirl_set(0,port,bkt,ratelimit[port-1].ingress[bkt].type,rate,FAL_ENABLE);
        if(ret)
        {
            LOG_ERROR("set ingress rate limit fail,errno = [%d]!\n",ret);
            return -1;
        }
    }

    ratelimit[port-1].ingress[bkt].rate  = rate;

    char buf[256];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "rate_limit",
        .option = bucket[INGRESS_RATE][bkt],
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
           ratelimit[0].ingress[bkt].rate,ratelimit[1].ingress[bkt].rate,ratelimit[2].ingress[bkt].rate,\
           ratelimit[3].ingress[bkt].rate,ratelimit[4].ingress[bkt].rate,ratelimit[5].ingress[bkt].rate,\
           ratelimit[6].ingress[bkt].rate,ratelimit[7].ingress[bkt].rate,ratelimit[8].ingress[bkt].rate,\
           ratelimit[9].ingress[bkt].rate);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("set port [%d] | bucket [%d] ingress rate = [%d] OK!\n",port,bkt,rate);

    return 0;
}


/* *
 * * @brief     get ingress switch status
 * * @param[port] port index.
 * * @retval    0: OK
 * * @retval    -1: ERROR
 * */
static int ratelimitGetIngressEnable(struct blob_buf *buf, int port,int bkt)
{
    char enable_s[2];

    sprintf(enable_s,"%d",ratelimit[port-1].ingress[bkt].enable);
    blobmsg_add_string(buf, bucket[INGRESS_ENABLE][bkt], enable_s);
    LOG_NORMAL("get port [%d] | bucket [%d] ingress enable = %s\n",port,bkt,enable_s);

    return 0;
}

/* *
 * * @brief     set ingress switch
 * * @param[port] port index.
 * * @param[p]  FAL_ENABLE/FAL_DISABLE
 * * @retval    0: OK
 * * @retval    -1: ERROR
 * */
static int ratelimitSetIngressEnable(int port,int bkt, void *p)
{
    fal_enable_t enable = atoi((char *)p);
    sw_error_t ret;

    /**<    check arguement  */
    if(enable == ratelimit[port-1].ingress[bkt].enable)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    else if(enable != FAL_ENABLE && enable != FAL_DISABLE)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    /**<    set rate limit disable if avaliable  */
    if(enable == FAL_DISABLE)
    {
        ratelimit[port-1].ingress[bkt].type = 0;
        char buf[256];
        struct uci_ptr ptr = {
            .package = "switchd",
            .section = "rate_limit",
            .option = bucket[INGRESS_TYPE_MASK][bkt],
        };

        sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
               ratelimit[0].ingress[bkt].type,ratelimit[1].ingress[bkt].type,ratelimit[2].ingress[bkt].type,\
               ratelimit[3].ingress[bkt].type,ratelimit[4].ingress[bkt].type,ratelimit[5].ingress[bkt].type,\
               ratelimit[6].ingress[bkt].type,ratelimit[7].ingress[bkt].type,ratelimit[8].ingress[bkt].type,\
               ratelimit[9].ingress[bkt].type);
        ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);
        ret = fal_rate_bucket_pirl_set(0,port,bkt,0,0,FAL_DISABLE);
        if(ret)
        {
            LOG_ERROR("set ingress rate limit error = %d!\n",ret);
            return -1;
        } 
    }
    
    ratelimit[port-1].ingress[bkt].enable = enable;    

    /**<    save change to UCI file  */
    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "rate_limit",
        .option = bucket[INGRESS_ENABLE][bkt],
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
           ratelimit[0].ingress[bkt].enable,ratelimit[1].ingress[bkt].enable,ratelimit[2].ingress[bkt].enable,\
           ratelimit[3].ingress[bkt].enable,ratelimit[4].ingress[bkt].enable,ratelimit[5].ingress[bkt].enable,\
           ratelimit[6].ingress[bkt].enable,ratelimit[7].ingress[bkt].enable,ratelimit[8].ingress[bkt].enable,\
           ratelimit[9].ingress[bkt].enable);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);
   
    LOG_NORMAL("set port [%d] | bucket [%d] ingress rate limit enable = [%d] OK\n",port,bkt,enable);
 
    return 0;
}


/* *
 * * @brief     get egress switch status
 * * @param[port] port index.
 * * @retval    0: OK
 * * @retval    -1: ERROR
 * */
static int ratelimitGetEgressRateEnable(struct blob_buf *buf, int port)
{
    char enable_s[2];

    sprintf(enable_s,"%d",ratelimit[port-1].egress.enable);
    LOG_NORMAL(" port [%d] rate limit egress enable = %s\n",port,enable_s);
    blobmsg_add_string(buf, "egress_rate_enable", enable_s);

    return 0;
}

/* *
 * * @brief     set egress switch
 * * @param[port] port index.
 * * @param[p]  FAL_ENABLE/FAL_DISABLE
 * * @retval    0: OK
 * * @retval    -1: ERROR
 * */
static int ratelimitSetEgressRateEnable(int port, void *p)
{
    fal_enable_t enable = atoi((char *)p);
    sw_error_t ret = 0;

    /**<    check arguement  */
    if(enable == ratelimit[port-1].egress.enable)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    else if(enable != FAL_ENABLE && enable != FAL_DISABLE)
    {
        LOG_ERROR("invalid enable arguement!\n");
        return -1;
    }
    
    /**<    set egress rate if avaliable  */
    if(enable == FAL_DISABLE)
    {
        ret = fal_rate_bucket_egrl_set(0,port,0,enable);
    }
    else if(enable == FAL_ENABLE && ratelimit[port-1].egress.rate)
    {
        ret = fal_rate_bucket_egrl_set(0,port,ratelimit[port-1].egress.rate,enable);
    }
    if(ret)
    {
        LOG_ERROR("set egress rate fail!\n");
        return -1;
    }

    ratelimit[port-1].egress.enable = enable;    

    /**<    save change to UCI file  */
    char buf[128];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "rate_limit",
        .option = "egress_rate_enable",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            ratelimit[0].egress.enable,ratelimit[1].egress.enable,ratelimit[2].egress.enable,ratelimit[3].egress.enable,\
            ratelimit[4].egress.enable,ratelimit[5].egress.enable,ratelimit[6].egress.enable,ratelimit[7].egress.enable,\
            ratelimit[8].egress.enable,ratelimit[9].egress.enable);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);
   
    LOG_NORMAL("port [%d] set egress rate limit enable  = [%d] OK\n",port,enable);
    return 0;
}

/* *
 * * @brief             get egress rate 
 * * @param[port]   port index.
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ratelimitGetEgressRate(struct blob_buf *buf, int port)
{
    char rate_s[10];

    sprintf(rate_s,"%d",ratelimit[port-1].egress.rate);
    LOG_NORMAL(" port [%d] rate limit egress rate = %s\n",port,rate_s);
    blobmsg_add_string(buf, "egress_rate", rate_s);

    return 0;
}

/* *
 * * @fn        ratelimitSetEgressRate
 * * @brief     set egress rate
 * * @param[port] port index.
 * * @param[p]  rate value 64~100000
 * * @retval    0: OK
 * * @retval    -1: ERROR
 * */
static int ratelimitSetEgressRate(int port, void *p)
{
    int rate = atoi((char *)p);
    sw_error_t ret;
    fal_port_type_t type;

    /**<    check arguement  */
    if(rate == ratelimit[port-1].egress.rate)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    else if(ratelimit[port-1].egress.enable != FAL_ENABLE)
    {
        LOG_NORMAL("the feature of port [%d] rate limit egress is disable!\n",port);
        return -1;
    }
    type = get_port_type(port);

    if((type == FAL_PORT_TYPE_FE)||(type == FAL_PORT_TYPE_FX)){
        if(rate < SPEED_LO_LEVEL || rate > SPEED_F_HI_LEVEL)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }
    }else{
        if(rate < SPEED_LO_LEVEL || rate > SPEED_HI_LEVEL)
        {
            LOG_ERROR("invalid enable arguement!\n");
            return -1;
        }
    }

    /**<    set egress rate if avaliable  */
    ret = fal_rate_bucket_egrl_set(0,port,rate,FAL_ENABLE);
    if(ret)
    {
        LOG_ERROR("set ingress rate limit fail,errno = [%d]!\n",ret);
        return -1;
    }

    ratelimit[port-1].egress.rate  = rate;
    
    char buf[256];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "rate_limit",
        .option = "egress_rate",
    };

    sprintf(buf,"port1:%d port2:%d port3:%d port4:%d port5:%d port6:%d port7:%d port8:%d port9:%d port10:%d",\
            ratelimit[0].egress.rate,ratelimit[1].egress.rate,ratelimit[2].egress.rate,ratelimit[3].egress.rate,\
            ratelimit[4].egress.rate,ratelimit[5].egress.rate,ratelimit[6].egress.rate,ratelimit[7].egress.rate,\
            ratelimit[8].egress.rate,ratelimit[9].egress.rate);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("port [%d] set egress rate limit rate = [%d] OK!\n",port,rate);
    
    return 0;
}


static const struct jw_switch_policy rate_limit_tbl[] = {
    {.name = "egress_rate_enable", .get_handler = ratelimitGetEgressRateEnable, .set_handler = ratelimitSetEgressRateEnable},
    {.name = "egress_rate", .get_handler = ratelimitGetEgressRate, .set_handler = ratelimitSetEgressRate},
};

typedef struct{
    char *name;
    int(*func_get)(struct blob_buf *,int,int);
    int(*func_set)(int,int,void *p);
}Policy_S;
static const Policy_S policyTable[] = {
    {.name = "ingress_enable",.func_get = ratelimitGetIngressEnable,.func_set = ratelimitSetIngressEnable},
    {.name = "ingress_type_mask",.func_get = ratelimitGetIngressTypemask,.func_set = ratelimitSetIngressTypemask},
    {.name = "ingress_rate",.func_get = ratelimitGetIngressRate,.func_set = ratelimitSetIngressRate},
};

/* *
 * * @brief         parse port limit rate config get
 * * @param[lvl3]   level 3 message structure.
 * * @retval        0: OK
 * * @retval        -1: ERROR
 * */
static int JW_ParsePortLimitRateCfgGet(struct blob_attr *lvl3, struct blob_buf *buf)
{
    const char *name    = blobmsg_name(lvl3);
    int idx             = atoi(name + strlen(PORT_IDX));

    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }
    
    int rem;
    void *ret_array;
    void *ret_table;
    struct blob_attr *lvl4;

    ret_array= blobmsg_open_array(&b, name);

    blobmsg_for_each_attr(lvl4,lvl3,rem) 
    {
        if(blobmsg_type(lvl4) == BLOBMSG_TYPE_STRING)
        {
            char *value = blobmsg_get_string(lvl4);
            char *ptr;

            /**<    check if bucket is valid while get ingress arguement  */
            ptr = strstr(value,"bucket");
            if(ptr != NULL)
            {
                int bkt = atoi(ptr + strlen("bucket"));
                
                if(bkt < 0 || bkt > 4)
                {
                    LOG_ERROR("bucket %d is invalid!\n");
                    continue;
                }

                char *value_bak = strdup(value);
                strcpy(value_bak + (ptr - value),ptr + strlen("bucketx_"));

                int i;
                for(i = 0;i < 4;i++)
                {
                    if(!strcmp(value_bak,policyTable[i].name))
                        break;
                }
                if(i == 4)
                {
                    LOG_ERROR("policy can not pass!\n");
                    continue;
                }

                ret_table = blobmsg_open_table(&b, NULL);
                policyTable[i].func_get(buf,idx,bkt);
                blobmsg_close_table(&b, ret_table);	

                free(value_bak);
            }
            else
            {
                struct jw_switch_policy *policy_p;
                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(value, rate_limit_tbl, 2);
                if(policy_p && policy_p->get_handler)
                {
                    ret_table = blobmsg_open_table(&b, NULL);
                    policy_p->get_handler(buf,idx);
                    blobmsg_close_table(&b, ret_table);	
                }
                else
                {
                    LOG_ERROR("policy check not pass!\n");
                }       
            }
        }
        else
        {
            LOG_ERROR("level 4 message type is not available!\n");
        }
    }
    
    blobmsg_close_array(&b, ret_array);	

    return 0;
}

/* *
 * * @brief     parse port limit rate config set
 * * @param[lvl3] level 3 message structure.
 * * @retval    0: OK
 * * @retval    -1: ERROR
 * */
static int JW_ParsePortLimitRateCfgSet(struct blob_attr *lvl3)
{
    const char *name    = blobmsg_name(lvl3);
    int idx             = atoi(name + strlen(PORT_IDX));

    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }

    struct blob_attr *lvl4 = NULL;
    int ret = -1;
    int rem;
    
    blobmsg_for_each_attr(lvl4, lvl3, rem) 
    {
        if (blobmsg_type(lvl4) == BLOBMSG_TYPE_TABLE) 
        {
            struct blob_attr *lvl5 = blobmsg_data(lvl4);

            if (blobmsg_type(lvl5) == BLOBMSG_TYPE_STRING) 
            {
                struct jw_switch_policy *policy_p;
                char *value = blobmsg_get_string(lvl5);
                char *ptr;
                
                name = blobmsg_name(lvl5);

                /**<    check if bucket is valid while get ingress arguement  */
                ptr = strstr(name,"bucket");
                if(ptr != NULL)
                {
                    int bkt = atoi(ptr + strlen("bucket"));
                    
                    if(bkt < 0 || bkt > 4)
                    {
                        LOG_ERROR("bucket %d is invalid!\n");
                        continue;
                    }

                    char *name_bak = strdup(name);
                    strcpy(name_bak + (ptr - name),ptr + strlen("bucketx_"));

                    int i;
                    for(i = 0;i < 4;i++)
                    {
                        if(!strcmp(name_bak,policyTable[i].name))
                            break;
                    }
                    if(i == 4)
                    {
                        LOG_ERROR("policy can not pass!\n");
                        ret = -1;
                        break;
                    }

                    ret = policyTable[i].func_set(idx,bkt,value);
                    if(ret) 
                    {
                        LOG_ERROR("do set handler error!\n");
                        break;
                    }

                    free(name_bak);
                }
                else
                {
                    policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, rate_limit_tbl, __RATE_LIMIT_TBL_MAX);
                    if (policy_p && policy_p->set_handler) 
                    {
                        ret = policy_p->set_handler(idx,value);
                        if(ret) 
                        {
                            LOG_ERROR("do set handler error!\n");
                            break;
                        }
                    }
                    else
                    {
                        LOG_ERROR("policy check not pass!\n");
                        ret = -1;
                        break;
                    }           
                }
            }
            else
            {
                LOG_ERROR("level 5 message type is not available!\n");
                ret = -1;
                break;
            }
        }
        else
        {
            LOG_ERROR("level 4 message type is not available!\n");
            ret = -1;
            break;
        }
    }

    return ret;
}

/* *
 * * @fn JW_LimitRateGetHandler
 * * @brief handle function for the method of "limit_rate_get"
 * * @retval 0: OK
 * * @retval -1: ERROR
 * */
static int JW_LimitRateGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__RATE_LIMIT_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(rate_limit_get_policy, __RATE_LIMIT_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[RATE_LIMIT_GET_ARRAY];     
    if (!lvl1) 
	{
        LOG_ERROR("invalid params in %s\n",__func__);
        return -1;
    }

	if (blobmsg_type(lvl1) != BLOBMSG_TYPE_ARRAY) 
	{
        LOG_ERROR("level 1 message type is not array\n");
        return -1;
    }
    
    struct blob_attr *lvl2 = NULL;
    void *ret_array;
    void *ret_table;

    int rem;
    
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
                JW_ParsePortLimitRateCfgGet(lvl3,&b);
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

/* *
 * * @fn JW_LimitRateSetHandler
 * * @brief handle function for the method of "limit_rate_set"
 * * @retval 0: OK
 * * @retval -1: ERROR
 * */
static int JW_LimitRateSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__RATE_LIMIT_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(rate_limit_set_policy, __RATE_LIMIT_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[RATE_LIMIT_SET_ARRAY];     
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
                ret = JW_ParsePortLimitRateCfgSet(lvl3);
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


static const struct ubus_method rate_limit_methods[] = {
	UBUS_METHOD("rate_limit_get", JW_LimitRateGetHandler, rate_limit_get_policy),
	UBUS_METHOD("rate_limit_set", JW_LimitRateSetHandler, rate_limit_set_policy),
};


static struct ubus_object_type rate_limit_object_type =
	UBUS_OBJECT_TYPE("rate_limit", rate_limit_methods);


static struct ubus_object rate_limit_object = {
	.name = "rate_limit",
	.type = &rate_limit_object_type,
	.methods = rate_limit_methods,
	.n_methods = ARRAY_SIZE(rate_limit_methods),
};


/* *
 * * @brief             load ratelimit arguement - ingress_bucket_enable from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ratelimitLoadIgsBktEnable(struct uci_ptr *ptr)
{
    char *pstr;
    char *string = strdup(ptr->o->v.string);
    int bkt = atoi(ptr->option + strlen("ingress_bucket"));

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("ingress bucket %d enable format in UCI file error!\n",bkt); 
            return -1;
        }

        int port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("ingress bucket %d enable arguement - port index in UCI file error!\n",bkt); 
            return -1;
        }

        /**<    separate port index and status  */
        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("ingress bucket %d enable format in UCI file error!\n",bkt); 
            return -1;
        }

        int st = atoi(pdelim + 1);
        if(st != FAL_ENABLE && st != FAL_DISABLE)
        {
            LOG_ERROR("arguement invalid!\n");
            return -1;
        }

        ratelimit[port-1].ingress[bkt].enable = st;    
    }
    free(string);

    LOG_NORMAL("ratelimit load ingress bucket %d enable arguement OK\n",bkt);
    
    return 0;
}
 
/* *
 * * @brief             load ratelimit arguement - ingress_bucket_type_mask from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ratelimitLoadIgsBktTypemask(struct uci_ptr *ptr)
{
    char *pstr;
    char *string = strdup(ptr->o->v.string);
    int bkt = atoi(ptr->option + strlen("ingress_bucket"));

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("ingress bucket %d typemask format in UCI file error!\n",bkt); 
            return -1;
        }

        int port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("ingress bucket %d typemask arguement - port index in UCI file error!\n",bkt); 
            return -1;
        }

        /**<    separate port index and typemask  */
        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("ingress bucket %d typemask format in UCI file error!\n",bkt); 
            return -1;
        }

        int type = atoi(pdelim + 1);
        int i;
        for(i = 0;i < BUCKET_NUM;i++)
        {
            if(i == bkt)
                continue;

            if(type & ratelimit[port-1].ingress[i].type)
            {
                LOG_ERROR("invalid arguement!\n");
                return -1;
            }
        }
        if(ratelimit[port-1].ingress[bkt].enable != FAL_ENABLE && type)
        {
            LOG_ERROR("invalid arguement!\n");
            return -1;
        }

        /**<    set to driver if avaliable  */
        if(ratelimit[port-1].ingress[bkt].enable == FAL_ENABLE && \
           ratelimit[port-1].ingress[bkt].rate >= SPEED_LO_LEVEL && ratelimit[port-1].ingress[bkt].rate <= SPEED_HI_LEVEL && \
           type)
        {
            int ret = fal_rate_bucket_pirl_set(0,port,bkt,type,ratelimit[port-1].ingress[bkt].rate,FAL_ENABLE);
            if(ret)
            {
                LOG_ERROR("set ingress rate limit disable fail!\n");
                return -1;
            }
        }

        ratelimit[port-1].ingress[bkt].type = type;    
    }
    free(string);

    LOG_NORMAL("ratelimit load ingress bucket %d enable arguement OK\n",bkt);
    
    return 0;
}

/* *
 * * @brief             load ratelimit arguement - ingress_bucket_rate from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ratelimitLoadIgsBktRate(struct uci_ptr *ptr)
{
    char *pstr;
    fal_port_type_t type;
    char *string = strdup(ptr->o->v.string);
    int bkt = atoi(ptr->option + strlen("ingress_bucket"));

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("ingress bucket %d rate format in UCI file error!\n",bkt); 
            return -1;
        }

        int port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("ingress bucket %d rate arguement - port index in UCI file error!\n",bkt); 
            return -1;
        }

        /**<    separate port index and rate  */
        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("ingress bucket %d rate format in UCI file error!\n",bkt); 
            return -1;
        }

        int rate = atoi(pdelim + 1);
        type = get_port_type(port);

        if((type == FAL_PORT_TYPE_FE)||(type == FAL_PORT_TYPE_FX)){
            if(rate < SPEED_LO_LEVEL || rate > SPEED_F_HI_LEVEL)
            {
                LOG_ERROR("invalid arguement!\n");
                return -1;
            }
        }else{
            if(rate < SPEED_LO_LEVEL || rate > SPEED_HI_LEVEL)
            {
                LOG_ERROR("invalid arguement!\n");
                return -1;
            }
        }

        /**<    set to driver if avaliable  */
		if(ratelimit[port-1].ingress[bkt].enable == FAL_ENABLE && ratelimit[port-1].ingress[bkt].type)
		{
            int ret = fal_rate_bucket_pirl_set(0,port,bkt,ratelimit[port-1].ingress[bkt].type,rate,FAL_ENABLE);
            if(ret)
            {
                LOG_ERROR("port [%d] bucket [%d] set rate limit ingress fail\n",port,bkt);
                return -1;
            }
		}

        ratelimit[port-1].ingress[bkt].rate = rate;    
    }
    free(string);

    LOG_NORMAL("ratelimit load ingress bucket %d rate arguement OK\n",bkt);
    
    return 0;
}

/* *
 * * @brief             load ratelimit arguement - egress_enable from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ratelimitLoadEgsEnable(struct uci_ptr *ptr)
{
    char *pstr;
    int st;
    int port;
    char *string = strdup(ptr->o->v.string);

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("egress enable format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("egress enable arguement - port index in UCI file error!\n"); 
            return -1;
        }

        /**<    separate port index and status  */
        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("egress enable format in UCI file error!\n"); 
            return -1;
        }

        st = atoi(pdelim + 1);
        if(st != FAL_ENABLE && st != FAL_DISABLE)
        {
            LOG_ERROR("arguement invalid!\n");
            return -1;
        }

        ratelimit[port-1].egress.enable = st;    
    }
    free(string);

    LOG_NORMAL("ratelimit load egress enable arguement OK\n");
    
    return 0;
}

/* *
 * * @brief             load ratelimit arguement - egress_rate from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ratelimitLoadEgsRate(struct uci_ptr *ptr)
{
    char *pstr;
    int rate;
    int port;
    int ret;
    fal_port_type_t type;
    char *string = strdup(ptr->o->v.string);

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("egress rate format in UCI file error!\n"); 
            return -1;
        }

        port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("egress rate arguement - port index in UCI file error!\n"); 
            return -1;
        }

        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("egress rate format in UCI file error!\n"); 
            return -1;
        }

        rate = atoi(pdelim + 1);
        type = get_port_type(port);

        if((type == FAL_PORT_TYPE_FE)||(type == FAL_PORT_TYPE_FX)){
            if(rate < SPEED_LO_LEVEL || rate > SPEED_F_HI_LEVEL)
            {
                LOG_ERROR("invalid arguement!\n");
                return -1;
            }
        }else{
            if(rate < SPEED_LO_LEVEL || rate > SPEED_HI_LEVEL)
            {
                LOG_ERROR("invalid arguement!\n");
                return -1;
            } 
        }

        if(ratelimit[port-1].egress.enable == FAL_ENABLE)
        {
            ret = fal_rate_bucket_egrl_set(0,port,rate,FAL_ENABLE);
            if(ret)
            {
                LOG_ERROR("port [%d] set rate limit egress fail\n",port);
                return -1;
            }
        }

        ratelimit[port-1].egress.rate  = rate;
    }
    free(string);

    LOG_NORMAL("ratelimit load egress rate arguement OK\n");
    
    return 0;
}

/**<    the order should not be changed  */
static UciConfig_S ratelimitUci[] = {
    {.name = "ingress_bucket0_enable",      .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktEnable},
    {.name = "ingress_bucket0_type_mask",   .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktTypemask},
    {.name = "ingress_bucket0_rate",        .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktRate},
    {.name = "ingress_bucket1_enable",      .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktEnable},
    {.name = "ingress_bucket1_type_mask",   .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktTypemask},
    {.name = "ingress_bucket1_rate",        .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktRate},
    {.name = "ingress_bucket2_enable",      .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktEnable},
    {.name = "ingress_bucket2_type_mask",   .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktTypemask},
    {.name = "ingress_bucket2_rate",        .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktRate},
    {.name = "ingress_bucket3_enable",      .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktEnable},
    {.name = "ingress_bucket3_type_mask",   .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktTypemask},
    {.name = "ingress_bucket3_rate",        .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktRate},
    {.name = "ingress_bucket4_enable",      .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktEnable},
    {.name = "ingress_bucket4_type_mask",   .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktTypemask},
    {.name = "ingress_bucket4_rate",        .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadIgsBktRate},
    {.name = "egress_rate_enable",          .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadEgsEnable},
    {.name = "egress_rate",                 .type = UCI_TYPE_STRING,    .ubusGet = ratelimitLoadEgsRate},
};

/* *
 * * @brief             load mirror configure
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int ratelimitCfgLoad(void)
{
    struct uci_context *ctx;

    ctx = uci_alloc_context();

    char name[128]; 
    int ret;
    int i;
    struct uci_ptr ptr;

    for(i = 0;i < sizeof(ratelimitUci)/sizeof(UciConfig_S);i++)
    {
        sprintf(name,"switchd.rate_limit.%s",ratelimitUci[i].name);
        
        ret = load_config(name,ratelimitUci[i].type,&ptr,ctx);
        if(ret)
        {
            uci_free_context(ctx);

            if(ratelimitUci[i].type == UCI_TYPE_LIST) continue;

            LOG_ERROR("load config %s fail!\n",name); 
            return -1;
        }
        
        ret = ratelimitUci[i].ubusGet(&ptr);
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

/* *
 * * @brief add object "rate_limit" to ubus
 * */
void ubus_init_rate_limit(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;
    ret = ratelimitCfgLoad();
    if(ret)
    {
        LOG_ERROR("load rate limit configure fail!\n");
        return;
    }
	ret = ubus_add_object(ctx, &rate_limit_object);
	if (ret)
		LOG_ERROR("Failed to add object [rate_limit]: %s\n", ubus_strerror(ret));
}
