/* *< 中威文件编码格式为gb2312,cp936 */
/* * $Id$ $DateTime$
 * * @file vlan_config.c
 * * @brief This is a table of the switch's vlan configure.
 * * @version 0.0.1
 * * @since 0.0.1
 * * @author code<songyaofei@josware.com>
 * * @date 2016-05-24
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
    VLAN_CFG_GET_KEY,
    __VLAN_CFG_GET_MAX,
};

enum {
    VLAN_CFG_SET_KEY,
    __VLAN_CFG_SET_MAX,
};

enum {
    VLAN_CFG_CLEAR_KEY,
    __VLAN_CFG_CLEAR_MAX,
};

static const struct blobmsg_policy vlan_get_policy[] = {
    [VLAN_CFG_GET_KEY] = {.name = "get_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy vlan_set_policy[] = {
    [VLAN_CFG_SET_KEY] = {.name = "set_args", .type = BLOBMSG_TYPE_ARRAY},
};

static const struct blobmsg_policy vlan_clear_policy[] = {
    [VLAN_CFG_CLEAR_KEY] = {.name = "clear_args", .type = BLOBMSG_TYPE_ARRAY},
};

enum {
    VLAN_MODE,
    VLAN_PORT_BASE_PORTLIST,
    VLAN_DOT1Q_NUM,
    VLAN_DOT1Q_ID,
    VLAN_DOT1Q_NAME,
    VLAN_DOT1Q_PORTLIST,
    VLAN_DOT1Q_TAGLIST,
    __VLAN_CFG_TBL_MAX,
};

#define MAX_DOT1Q_NAME_LEN 24
#define MAX_VLAN_ID 4094
typedef struct{
    int idx;
    int flag;
    fal_vlan_id_t id;
    char name[MAX_DOT1Q_NAME_LEN];
    fal_vlan_members_t port_list;
    fal_vlan_egress_tag_t tag_list;
    int pl_num;
    int tl_num;
    struct list_head node;
}vlanList_S;

static struct{
    fal_vlan_mode_t mode; 
    int dot1q_num;
    struct{
        fal_vlan_members_t port_list;
        int pl_num;
    }port_base[CONFIG_JWS_PORT_TOTAL_NR]; 
    struct list_head entry;
}vlan;

/* *
 * * @brief         delete specified list option in UCI 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static void vlanDeleteUciEntry(vlanList_S *vl)
{
    char buf[MAX_DOT1Q_NAME_LEN+OCTET_NUM*2+8];
    char memb_s[3*MV88E6097_MAX_MEMBERS_SIZE];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "vlan",
        .option = "dot1q_entry",
    };

    sprintf(buf,"%d:%s:",vl->id,vl->name);

    int i;
    for(i = 0;i < MV88E6097_MAX_MEMBERS_SIZE;i++)
    {
        sprintf(memb_s + 3*i,"%02x-",vl->port_list.members[i]);
    }
    memb_s[i * 3 - 1] = ':';
    strcat(buf,memb_s);
    for(i = 0;i < MV88E6097_MAX_MEMBERS_SIZE;i++)
    {
        sprintf(memb_s + 3*i,"%02x-",vl->tag_list.members[i]);
    }
    memb_s[i * 3 - 1] = 0;
    strcat(buf,memb_s);
    ubus_collect_config(&ptr,buf,UCI_TYPE_LIST,DO_DEL);
}

/* *
 * * @brief         delete specified entry(contain driver and uci) 
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanDeleteEntry(vlanList_S *vl)
{
    /**<    delete specified entry in driver  */
    int ret;
    ret = fal_8021q_vlan_row_destroy(0,vl->id);
    if(ret)
    {
        LOG_ERROR("vlan dot1q destroy one entry fail!\n");
        return -1;
    }

    /**<    delete specified entry in UCI file  */
    vlanDeleteUciEntry(vl);

    return 0;
}


/* *
 * * @brief         get vlan mode
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanGetMode(struct blob_buf *buf)
{
    char mode_s[2];

    sprintf(mode_s,"%d",vlan.mode);
    blobmsg_add_string(buf, "mode", mode_s);
    LOG_NORMAL("vlan get mode = [%s] OK\n",mode_s);

    return 0;
}

/* *
 * * @brief         set vlan mode
 * * @param[*p]     vlan mode[1-port bast;2-802.1q]
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanSetMode(void *p)
{
    int mode = atoi((char *)p);
    sw_error_t ret;

    if(mode == vlan.mode)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    else if(mode != FAL_VLAN_MODE_PORT_BASED && mode != FAL_VLAN_MODE_8021Q)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    ret = fal_vlan_mode_set(0,mode);
    if(ret)
    {
        LOG_ERROR("set vlan mode fail!\n");
        return -1;
    }

    vlan.mode = mode;
    
    char buf[2];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "vlan",
        .option = "mode",
    };

    sprintf(buf,"%d",mode);
    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    /**<    set every entry in corresponding mode  */
    vlanList_S *vl,*vl_bak;
    if(mode == FAL_VLAN_MODE_PORT_BASED)
    {
        int i;
        for(i = 0;i < CONFIG_JWS_PORT_TOTAL_NR;i++)
        {
            ret = fal_port_based_vlan_set(0,i + 1,&vlan.port_base[i].port_list,vlan.port_base[i].pl_num);
            if(ret)
            {
                LOG_ERROR("analyse port based fail!\n");
                return -1;
            }
        }
    }
    else
	{
        list_for_each_entry_safe(vl, vl_bak, &vlan.entry, node)
        {
            if(!(vl->id > 1 && vl->id <= MAX_VLAN_ID) || vl->name[0] == 0 || vl->pl_num == 0 || vl->tl_num == 0)
                continue;

            ret = fal_8021q_vlan_row_create(0,vl->id,&vl->port_list,vl->pl_num,&vl->tag_list);
            if(ret)
            {
                LOG_ERROR("set new vlan 8021q entry error!\n");
                return -1;
            }          
        }
	}

    LOG_NORMAL("set vlan mode = [%d] OK\n",mode);
    
    return 0;
}

/* *
 * * @brief         get portlist of port-base mode
 * * @param[idx]    port index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanGetPortBasePortlist(struct blob_buf *buf,int idx)
{
    char portlist_s[3*MV88E6097_MAX_MEMBERS_SIZE] = {0};
    int num = vlan.port_base[idx-1].pl_num;

    if(!num)
    {
        blobmsg_add_string(buf, "port_base_vlan_portlist", "-1");
        LOG_ERROR("port %d lack portlist!\n");
        return -1;
    }

    int i;
    for(i = 0;i < num;i++)
    {
        sprintf(portlist_s + 3*i,"%02x-",vlan.port_base[idx-1].port_list.members[i]);
    }
    portlist_s[i * 3 - 1] = 0;

    blobmsg_add_string(buf, "port_base_vlan_portlist", portlist_s);
    LOG_NORMAL("vlan get port [%d] portlist OK\n",idx);


    return 0;
}

/* *
 * * @brief         set portlist of port-base mode
 * * @param[idx]    port index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanSetPortBasePortlist(int idx,void *p)
{
    int num;

    if(vlan.mode != FAL_VLAN_MODE_PORT_BASED)
    {
        LOG_ERROR("current mode is not port base!\n");
        return -1;
    }

    fal_vlan_members_t port_list_new;
    num = JW_ObjStrToArray(MV88E6097_MAX_MEMBERS_SIZE,port_list_new.members,p);
    if(num == -1)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    fal_vlan_members_t *port_list_old = &vlan.port_base[idx-1].port_list;

    if(!memcmp(&port_list_new,port_list_old,sizeof(fal_vlan_members_t)))
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }
    
    sw_error_t ret;
    ret = fal_port_based_vlan_set(0,idx,&port_list_new,num);
    if(ret)
    {
        LOG_ERROR("vlan set port based fail!\n");
        return -1;
    }

    vlan.port_base[idx-1].pl_num = num;
    memcpy(&vlan.port_base[idx-1].port_list,&port_list_new,sizeof(fal_vlan_members_t));
    
    char buf[128];
    char memb_s[3*MV88E6097_MAX_MEMBERS_SIZE];
    struct uci_ptr ptr = {
        .package = "switchd",
        .section = "vlan",
        .option = "port_base",
    };

    int j;
    for(j = 0;j < CONFIG_JWS_PORT_TOTAL_NR;j++)
    {
        sprintf(buf + j*(OCTET_NUM*3 + strlen("portx:")),"port%d:",j+1);

        int i;
        for(i = 0;i < OCTET_NUM;i++)
        {
            sprintf(memb_s + 3*i,"%02x-",vlan.port_base[j].port_list.members[i]);
        }
        memb_s[i * 3 - 1] = ' ';

        strcat(buf,memb_s);
    }
    buf[j*(OCTET_NUM*3+strlen("portx:"))-1] = 0;

    ubus_collect_config(&ptr,buf,UCI_TYPE_STRING,DO_SET);

    LOG_NORMAL("vlan set port base portlist = %s OK!\n",(char *)p);

    return 0;
}

/* *
 * * @brief         get 801.1q entry nember
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanGetDot1qNum(struct blob_buf *buf)
{
    char num_s[2];

    sprintf(num_s,"%d",vlan.dot1q_num);
    blobmsg_add_string(buf, "dot1q_vlan_num", num_s);
    LOG_NORMAL("vlan get dot1q entry num = [%s] OK\n",num_s);

    return 0;
}

/* *
 * * @brief         get id of one 802.1q entry 
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanGetDot1qId(struct blob_buf *buf,int idx)
{
    char id_s[5];

    vlanList_S *vl,*vl_bak;
    list_for_each_entry_safe(vl, vl_bak, &vlan.entry, node)
    {
        if(idx == vl->idx)
        {
            sprintf(id_s,"%d",vl->id);
            blobmsg_add_string(buf, "dot1q_vlan_id", id_s);
            LOG_NORMAL("vlan get index %d id = [%s] OK\n",idx,id_s);
            
            return 0;
        }
    }
            
    blobmsg_add_string(buf, "dot1q_vlan_id", "-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);
    
    return -1;
}

/* *
 * * @brief         set id of one 801.1q entry 
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanSetDot1qId(int idx,void *p)
{
    if(vlan.mode != FAL_VLAN_MODE_8021Q)
    {
        LOG_ERROR("current mode is not 802.1q!\n");
        return -1;
    }

    int id_new   = atoi((char *)p);
    
    if(id_new < 2 || id_new > MAX_VLAN_ID)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    /**<    foreach every node  */
    vlanList_S *vl,*vl_bak;
    vlanList_S *v = NULL;
    list_for_each_entry_safe(vl, vl_bak, &vlan.entry, node)
    {
        if(idx == vl->idx)
        {
            v = vl;       
        }
        else if(id_new == vl->id)
        {
            v = NULL;
            LOG_ERROR("dot1q vlan id duplicate!\n");
            break;
        }
    }

    if(v == NULL)
    {
        LOG_ERROR("something error while foreach!\n");
        return -1;
    }

    if(id_new == v->id)
    {
        LOG_NORMAL("same arguement,do nothing\n");
        return 0;
    }        

    /**<    destory old entry if exist  */
    if(v->name[0] && v->pl_num && v->tl_num)  
    {
        if(v->id > 1 && v->id <= MAX_VLAN_ID)
        {
            if(vlanDeleteEntry(v))
            {
                LOG_ERROR("vlan delete entry fail!\n");
                return -1;
            }
        }
        
        v->flag = 1;
    }

    v->id = id_new;
    LOG_NORMAL("vlan dot1q index [%d] set id = %d OK!\n",idx,id_new);
    return 0;
}

/* *
 * * @brief         get name of one 801.1q entry 
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanGetDot1qName(struct blob_buf *buf,int idx)
{
    vlanList_S *vl,*vl_bak;
    list_for_each_entry_safe(vl, vl_bak, &vlan.entry, node)
    {
        if(idx == vl->idx)
        {
            blobmsg_add_string(buf, "dot1q_vlan_name", vl->name);
            LOG_NORMAL("vlan get index %d name = [%s] OK\n",idx,vl->name);
            
            return 0;
        }
    }
            
    blobmsg_add_string(buf, "dot1q_vlan_name", "-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);
    
    return -1;
}

/* *
 * * @brief         set name of one 801.1q entry 
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanSetDot1qName(int idx,void *p)
{
    if(vlan.mode != FAL_VLAN_MODE_8021Q)
    {
        LOG_ERROR("current mode is not 802.1q!\n");
        return -1;
    }

    char *name = p;
    
    if(strlen(name) > (MAX_DOT1Q_NAME_LEN -2))
    {
        LOG_ERROR("invalid arguement!\n");
        return -1; 
    }

    vlanList_S *vl,*vl_bak;
    list_for_each_entry_safe(vl, vl_bak, &vlan.entry, node)
    {
        if(idx == vl->idx)
        {
            if(!strcmp(vl->name,name))
            {
                LOG_NORMAL("same arguement,nothing change\n");
                return 0;
            }

            if((vl->id > 1 && vl->id <= MAX_VLAN_ID) && vl->name[0] && vl->tl_num && vl->pl_num)
            {
                /**<    delete specified entry in UCI file  */
                vlanDeleteUciEntry(vl);
            }
                
            strcpy(vl->name,name);

            LOG_NORMAL("vlan dot1q index %d set entry name = %s OK!\n",idx,name);
            return 0;
        }
    }
    
    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}

/* *
 * * @brief         get portlist of one 801.1q entry 
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanGetDot1qPortlist(struct blob_buf *buf,int idx)
{
    char portlist_s[3*MV88E6097_MAX_MEMBERS_SIZE];

    vlanList_S *vl,*vl_bak;
    list_for_each_entry_safe(vl, vl_bak, &vlan.entry, node)
    {
        if(idx == vl->idx)
        {
            if(!vl->pl_num)
            {
                blobmsg_add_string(buf, "dot1q_vlan_portlist", "0");
                LOG_WARN("vlan dot1q index [%d] entry portlist has not been set!\n",idx);
                return 0;
            }

            int i;
            for(i = 0;i < vl->pl_num;i++)
            {
                sprintf(portlist_s + 3*i,"%02x-",vl->port_list.members[i]);
            }

            portlist_s[i * 3 - 1] = 0;
                     
            blobmsg_add_string(buf, "dot1q_vlan_portlist", portlist_s);
            LOG_NORMAL("vlan dot1q index [%d] get portlist = [%s] OK\n",idx,portlist_s);
            return 0;
        }
    }

    blobmsg_add_string(buf, "dot1q_vlan_portlist", "-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}

/* *
 * * @brief         set portlist of one 801.1q entry 
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanSetDot1qPortlist(int idx,void *p)
{
    if(vlan.mode != FAL_VLAN_MODE_8021Q)
    {
        LOG_ERROR("current mode is not 802.1q!\n");
        return -1;
    }

    fal_vlan_members_t port_list;
    int num;
    
    num = JW_ObjStrToArray(MV88E6097_MAX_MEMBERS_SIZE,port_list.members,p);
    if(num == -1)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    vlanList_S *vl,*vl_bak;
    list_for_each_entry_safe(vl, vl_bak, &vlan.entry, node)
    {
        if(idx == vl->idx)
        {
            if(!memcmp(&vl->port_list,&port_list,sizeof(fal_vlan_members_t)))
            {
                LOG_NORMAL("same arguement,do nothing\n");
                return 0;
            }

            if((vl->id > 1 && vl->id <= MAX_VLAN_ID) && vl->name[0] && vl->tl_num)
            {
                if(vl->pl_num)
                {
                    /**<    delete specified entry in UCI file  */
                    vlanDeleteUciEntry(vl);
                }

                vl->flag = 1;
            }
            
            vl->pl_num = num;
            memcpy(&vl->port_list,&port_list,sizeof(sizeof(fal_vlan_members_t)));

            LOG_NORMAL("vlan dot1q index %d set portlist = %s OK!\n",idx,(char *)p);
            return 0;
        }
    }
    
    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}

/* *
 * * @brief         get taglist of one 801.1q entry 
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanGetDot1qTaglist(struct blob_buf *buf,int idx)
{
    char taglist_s[3*MV88E6097_MAX_MEMBERS_SIZE];

    vlanList_S *vl,*vl_bak;
    list_for_each_entry_safe(vl, vl_bak, &vlan.entry, node)
    {
        if(idx == vl->idx)
        {
            if(!vl->tl_num)
            {
                blobmsg_add_string(buf, "dot1q_vlan_taglist", "0");
                LOG_WARN("vlan dot1q index [%d] entry taglist has not been set!\n",idx);
                return 0;
            }

            int i;
            for(i = 0;i < vl->tl_num;i++)
            {
                sprintf(taglist_s + 3*i,"%02x-",vl->tag_list.members[i]);
            }

            taglist_s[i * 3 - 1] = 0;
                     
            blobmsg_add_string(buf, "dot1q_vlan_taglist", taglist_s);
            LOG_NORMAL("vlan dot1q index [%d] get tagalist = [%s] OK\n",idx,taglist_s);
            return 0;
        }
    }

    blobmsg_add_string(buf, "dot1q_vlan_taglist", "-1");
    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}

/* *
 * * @brief         set taglist of one 801.1q entry 
 * * @param[idx]    entry index
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanSetDot1qTaglist(int idx,void *p)
{
    if(vlan.mode != FAL_VLAN_MODE_8021Q)
    {
        LOG_ERROR("current mode is not 802.1q!\n");
        return -1;
    }

    fal_vlan_egress_tag_t tag_list;
    int num;
    
    num = JW_ObjStrToArray(MV88E6097_MAX_MEMBERS_SIZE,tag_list.members,p);
    if(num == -1)
    {
        LOG_ERROR("invalid arguement!\n");
        return -1;
    }

    vlanList_S *vl,*vl_bak;
    list_for_each_entry_safe(vl, vl_bak, &vlan.entry, node)
    {
        if(idx == vl->idx)
        {
      /*      if(!memcmp(&vl->tag_list,&tag_list,sizeof(fal_vlan_egress_tag_t)))
            {
                LOG_NORMAL("same arguement,do nothing\n");
                return 0;
            }*/

            if((vl->id > 1 && vl->id <= MAX_VLAN_ID) && vl->name[0] && vl->pl_num)
            {
                if(vl->tl_num)
                {
                    /**<    delete specified entry in UCI file  */
                    vlanDeleteUciEntry(vl);
                }

                vl->flag = 1;
            }
            
            vl->tl_num = num;
            memcpy(&vl->tag_list,&tag_list,sizeof(sizeof(fal_vlan_egress_tag_t)));

            LOG_NORMAL("vlan dot1q index %d set taglist = %s OK!\n",idx,(char *)p);
            return 0;
        }
    }
    
    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}

static const struct jw_switch_policy vlan_tbl[] = {
    [VLAN_MODE] = {.name = "mode", .get_ext_handler = vlanGetMode, .set_ext_handler = vlanSetMode}, 
    [VLAN_PORT_BASE_PORTLIST] = {.name = "port_base_vlan_portlist", .get_handler = vlanGetPortBasePortlist, .set_handler = vlanSetPortBasePortlist},
    [VLAN_DOT1Q_NUM] = {.name = "dot1q_vlan_num", .get_ext_handler = vlanGetDot1qNum}, 
    [VLAN_DOT1Q_ID] = {.name = "dot1q_vlan_id", .get_handler = vlanGetDot1qId, .set_handler = vlanSetDot1qId},
    [VLAN_DOT1Q_NAME] = {.name = "dot1q_vlan_name", .get_handler = vlanGetDot1qName, .set_handler = vlanSetDot1qName},
    [VLAN_DOT1Q_PORTLIST] = {.name = "dot1q_vlan_portlist", .get_handler = vlanGetDot1qPortlist, .set_handler = vlanSetDot1qPortlist},
    [VLAN_DOT1Q_TAGLIST] = {.name = "dot1q_vlan_taglist", .get_handler = vlanGetDot1qTaglist, .set_handler = vlanSetDot1qTaglist},
};

static int vlanCheckKey(const char *key,int action)
{
    int idx = -1;

    if(!strncmp(key,PORT_IDX,strlen(PORT_IDX)))
    {
        idx = atoi(key + strlen(PORT_IDX));            
        if(idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR)
        {
            idx = -1;
        }
    }
    else if(!strncmp(key,ENTRY_IDX,strlen(ENTRY_IDX)))
    {
        idx = atoi(key + strlen(ENTRY_IDX));            
        if(idx < (1 + action) || idx > (vlan.dot1q_num + action))
        {
            idx = -1;
        }
    }

    return idx;
}


/* *
 * * @brief         vlan config parse recv form upper
 * * @param[lvl3]   level 3 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int JW_ParseVlanCfgGet(struct blob_attr *lvl3, struct blob_buf *buf)
{
    const char *name    = blobmsg_name(lvl3);
    int         idx     = vlanCheckKey(name,0);

    if (idx < 0) 
    {
        LOG_ERROR("vlan Check Key error!\n");
        return -1;
    }

    /**<    get arguements of vlan id 1  */
	if(vlan.mode == FAL_VLAN_MODE_8021Q && idx == 1)
	{
		int ret;
        fal_vlan_egress_tag_t port_list;
        fal_vlan_id_t vlan_id;

		ret = fal_8021q_vlan_row_status(0,&vlan_id,&port_list);
		if(ret)
		{
			LOG_ERROR("get index [1] vlan dot1q status fail!\n");
			return -1;
		}

        vlanList_S *vl = list_first_entry(&vlan.entry,vlanList_S,node);
        memcpy(&vl->port_list,&port_list,sizeof(fal_vlan_egress_tag_t)); 
        LOG_NORMAL("Get index 1 vlan dot1q status OK!\n");
	}
    
    /**<    foreach array of level 3 message  */
    void *ret_array;

    ret_array= blobmsg_open_array(buf, name);
    foreach_json_array_get(idx,lvl3,buf,vlan_tbl, __VLAN_CFG_TBL_MAX);
    blobmsg_close_array(buf, ret_array);	

    return 0; 
}

/* *
 * * @brief         vlan config parse recv form upper
 * * @param[lvl3]   level 3 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int JW_ParseVlanCfgSet(struct blob_attr *lvl3)
{
    const char *name  = blobmsg_name(lvl3);
    int         idx     = vlanCheckKey(name,1);

    if (idx < 0) 
    {
        LOG_ERROR("vlan Check Key error!\n");
        return -1;
    }
    
    /**<    add new index/entry  */
    if(!strncmp(name,ENTRY_IDX,strlen(ENTRY_IDX)) && idx == (vlan.dot1q_num + 1))
    {
        vlan.dot1q_num++;
        vlanList_S *vl = (vlanList_S *)calloc(1,sizeof(vlanList_S));
        list_add_tail(&vl->node,&vlan.entry);
        vl->idx = vlan.dot1q_num;
    }

    /**<    foreach array of level 3 message  */
    int ret;
    ret = foreach_json_array_set(idx,lvl3,vlan_tbl, __VLAN_CFG_TBL_MAX);
    if(ret)
    {
        LOG_ERROR("foreach index %d error!\n",idx);
        return -1;
    }

    /**<    create each multicast row which has been changed  */
    vlanList_S *vl,*vl_bak;
    list_for_each_entry_safe(vl, vl_bak, &vlan.entry, node)
    {
        if(vl->flag)
        {
            ret = fal_8021q_vlan_row_create(0,vl->id,&vl->port_list,vl->pl_num,&vl->tag_list);
            if(ret)
            {
                /**<    add free memory later  */
                LOG_ERROR("set new mac address error!\n");
                vl->flag = 0;
                return -1;
            }
            vl->flag = 0;

            char buf[MAX_DOT1Q_NAME_LEN+OCTET_NUM*2+8];
            char memb_s[3*MV88E6097_MAX_MEMBERS_SIZE];
            struct uci_ptr ptr = {
                .package = "switchd",
                .section = "vlan",
                .option = "dot1q_entry",
            };

            sprintf(buf,"%d:%s:",vl->id,vl->name);
            int i;
            for(i = 0;i < MV88E6097_MAX_MEMBERS_SIZE;i++)
            {
                sprintf(memb_s + 3*i,"%02x-",vl->port_list.members[i]);
            }
            memb_s[i * 3 - 1] = ':';
            strcat(buf,memb_s);
            for(i = 0;i < MV88E6097_MAX_MEMBERS_SIZE;i++)
            {
                sprintf(memb_s + 3*i,"%02x-",vl->tag_list.members[i]);
            }
            memb_s[i * 3 - 1] = 0;
            strcat(buf,memb_s);
            ubus_collect_config(&ptr,buf,UCI_TYPE_LIST,DO_SET);

            LOG_NORMAL("vlan set one entry: %s OK\n",buf);
        }
    }

    return 0;
}

/* *
 * * @brief         vlan config parse recv form upper
 * * @param[lvl2]   level 2 message
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int JW_ParseVlanCfgClear(struct blob_attr *lvl2)
{
    char *value = blobmsg_get_string(lvl2);
    int idx     = vlanCheckKey(value,0);

    if (idx < 0) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }

    vlanList_S *vl,*vl_bak;
    int flag = 0;
    list_for_each_entry_safe(vl, vl_bak, &vlan.entry, node)
    {
        if(flag)
        {
            vl->idx--;
        }
        else if(idx == vl->idx)
        {
            /**<    delete specified entry  */
            if((vl->id > 1 && vl->id <= MAX_VLAN_ID) && vl->name[0] && vl->pl_num && vl->tl_num)
            {
                if(vlanDeleteEntry(vl))
                {
                    LOG_ERROR("vlan delete entry fail!\n");
                    return -1;
                }
            }
            
            flag = 1;

            /**<    delete specified node from link list  */
            vlan.dot1q_num--;
            list_del(&vl->node);
            free(vl);
        }
    }

    if(flag)
    {
        LOG_NORMAL("vlan dot1q destroy one entry which index = %d OK\n",idx);
        return 0;
    }

    LOG_ERROR("no match index [%d], something error!\n",idx);
    return -1;
}

/* *
 * * @brief         method "vlan_config_get" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanCfgGetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__VLAN_CFG_GET_MAX];    
	struct blob_attr *lvl1 = NULL;  

    blobmsg_parse(vlan_get_policy, __VLAN_CFG_GET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[VLAN_CFG_GET_KEY];     
    if (!lvl1) 
	{
        LOG_ERROR("invalid params in %s\n",__func__);
        return UBUS_STATUS_INVALID_ARGUMENT;
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
    
	blobmsg_buf_init(&b);
    ret_array = blobmsg_open_array(&b, "ret"); 

    blobmsg_for_each_attr(lvl2,lvl1, rem) 
    {
        switch (blobmsg_type(lvl2))
        {
            case BLOBMSG_TYPE_STRING:
            {
                struct jw_switch_policy *policy_p;
                char *name = blobmsg_get_string(lvl2);

                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, vlan_tbl, __VLAN_CFG_TBL_MAX);
                if (policy_p && policy_p->get_ext_handler) 
                {
                    ret_table = blobmsg_open_table(&b, NULL);
                    policy_p->get_ext_handler(&b);
                    blobmsg_close_table(&b, ret_table);	
                }
                else
                {
                    LOG_ERROR("policy check not pass!\n");
                }
            }
                break;
            case BLOBMSG_TYPE_TABLE:
            {
                struct blob_attr *lvl3 = blobmsg_data(lvl2);

                if(blobmsg_type(lvl3) == BLOBMSG_TYPE_ARRAY)
                {
                    ret_table = blobmsg_open_table(&b, NULL);
                    JW_ParseVlanCfgGet(lvl3,&b);
                    blobmsg_close_table(&b, ret_table);	
                }
                else
                {
                    LOG_ERROR("level 3 message type is not available!\n");
                }
            }
                break;
            default:
                LOG_ERROR("level 2 message type is not available!\n");
                break;
        }
    }
    
    blobmsg_close_array(&b, ret_array);	
    ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;

}

/* *
 * * @brief         method "vlan_config_set" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanCfgSetHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__VLAN_CFG_SET_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(vlan_set_policy, __VLAN_CFG_SET_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[VLAN_CFG_SET_KEY];     
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
            
            switch(blobmsg_type(lvl3))
            {
                case BLOBMSG_TYPE_STRING:
                {
                    struct jw_switch_policy *policy_p;
                    const char *name  = blobmsg_name(lvl3);
                    char *value = blobmsg_get_string(lvl3);

                    policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, vlan_tbl, __VLAN_CFG_TBL_MAX);
                    if (policy_p && policy_p->set_ext_handler) 
                    {
                        ret = policy_p->set_ext_handler(value);
                        if(ret)
                            goto end;
                    }
                    else
                    {
                        LOG_ERROR("policy check not pass!\n");
                        ret = -1;
                        goto end;
                    }
                }
                    break;
                case BLOBMSG_TYPE_ARRAY:
                    ret = JW_ParseVlanCfgSet(lvl3);
                    if(ret)
                        goto end;
                    break;
                default:
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

/* *
 * * @brief         method "vlan_config_clear" handler
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int vlanCfgClearHandler(struct ubus_context *ctx, struct ubus_object *obj,
                struct ubus_request_data *req, const char *method,
                struct blob_attr *msg)
{
    struct blob_attr *tb[__VLAN_CFG_CLEAR_MAX];    
	struct blob_attr *lvl1 = NULL;  
    int ret = -1;

    blobmsg_parse(vlan_clear_policy, __VLAN_CFG_CLEAR_MAX, tb, blob_data(msg), blob_len(msg));
	lvl1 = tb[VLAN_CFG_CLEAR_KEY];     
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
        if (blobmsg_type(lvl2) == BLOBMSG_TYPE_STRING) 
        {
                        
            ret = JW_ParseVlanCfgClear(lvl2);
        }
        else
        {
            LOG_ERROR("level 2 message type is not available!\n");
        }
    }

end:
    blobmsg_add_string(&b, "ret", ret?"-1":"0");
	ubus_send_reply(ctx, req, b.head);

	return UBUS_STATUS_OK;
}



static const struct ubus_method vlan_methods[] = {
	UBUS_METHOD("vlan_config_get", vlanCfgGetHandler, vlan_get_policy),
	UBUS_METHOD("vlan_config_set", vlanCfgSetHandler, vlan_set_policy),
	UBUS_METHOD("vlan_config_clear", vlanCfgClearHandler, vlan_clear_policy),
};


static struct ubus_object_type vlan_object_type =
	UBUS_OBJECT_TYPE("vlan_config", vlan_methods);


static struct ubus_object vlan_object = {
	.name = "vlan_config",
	.type = &vlan_object_type,
	.methods = vlan_methods,
	.n_methods = ARRAY_SIZE(vlan_methods),
};

/* *
 * * @brief             load vlan arguement - mode from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int vlanLoadMode(struct uci_ptr *ptr)
{
    int mode = atoi(ptr->o->v.string);
    int ret;

    if(mode != FAL_VLAN_MODE_PORT_BASED && mode != FAL_VLAN_MODE_8021Q)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }

    ret = fal_vlan_mode_set(0,mode);
    if(ret)
    {
        LOG_ERROR("set vlan mode fail!\n");
        return -1;
    }

    vlan.mode = mode;
    LOG_NORMAL("vlan load mode = [%d] OK\n", mode);
    
    return 0;
}

/* *
 * * @brief             load vlan arguement - port status from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int vlanLoadPortBase(struct uci_ptr *ptr)
{
    char *pstr;
    char *string = strdup(ptr->o->v.string);

    while((pstr = strsep(&string," ")) != NULL)
    {
        if(strncmp(pstr,"port",strlen("port")))
        {
            LOG_ERROR("port base format in UCI file error!\n"); 
            return -1;
        }

        int port = atoi(pstr + 4);
        if(port < 1 || port > CONFIG_JWS_PORT_TOTAL_NR)
        {
            LOG_ERROR("port base arguement-port index in UCI file error!\n"); 
            return -1;
        }

        /**<    separate vlan port and portlist  */
        char *pdelim = strchr(pstr,':');
        if(pdelim == NULL)
        {
            LOG_ERROR("port base format in UCI file error!\n"); 
            return -1;
        }

        fal_vlan_members_t port_list;
        int num = JW_ObjStrToArray(MV88E6097_MAX_MEMBERS_SIZE,port_list.members,pdelim + 1);
        if(num == -1)
        {
            LOG_ERROR("parse vlan port base members fail!\n");
            return -1;
        }

        /**<    set entry to driver  */
        if(vlan.mode == FAL_VLAN_MODE_PORT_BASED)
        {
            int ret = fal_port_based_vlan_set(0,port,&port_list,num);
            if(ret)
            {
                LOG_ERROR("set port %d port list fail!\n",port);
                return -1;
            }
        }

        vlan.port_base[port-1].pl_num = num;
        memcpy(&vlan.port_base[port-1].port_list,&port_list,sizeof(fal_vlan_members_t));
    }
    free(string);

    LOG_NORMAL("vlan load port base port list OK\n");

    return 0;
}

/* *
 * * @brief             load vlan arguement - dot1q entry from UCI file
 * * @param[*p]         buffer to be parse 
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
static int vlanLoadDot1qEntry(struct uci_ptr *ptr)
{
    int ret;
    int count = 1;

    struct uci_element *elt;

    uci_foreach_element(&ptr->o->v.list,elt)
    {
        /**<    separate vlan id and name  */
        char *pdelim1 = strchr(elt->name,':');
        if(pdelim1 == NULL)
        {
            LOG_ERROR("vlan dot1q entry format in UCI file error!\n"); 
            return -1;
        }

        *pdelim1++ = 0;

        int id = atoi(elt->name);
        if(id < 2 || id > MAX_VLAN_ID)
        {
            LOG_ERROR("vlan dot1q entry arguement - id in UCI file error!\n"); 
            return -1;
        }

        /**<    separate vlan name and portlist  */
        char *pdelim2 = strchr(pdelim1,':');
        if(pdelim2 == NULL)
        {
            LOG_ERROR("vlan dot1q entry format in UCI file error!\n"); 
            return -1;
        }

        *pdelim2++ = 0;

        if(strlen(pdelim1) > (MAX_DOT1Q_NAME_LEN -2))
        {
            LOG_ERROR("vlan dot1q entry arguement - name in UCI file error!\n");
            return -1; 
        }

        /**<    separate vlan portlist and taglist  */
        char *pdelim3 = strchr(pdelim2,':');
        if(pdelim3 == NULL)
        {
            LOG_ERROR("vlan dot1q entry format in UCI file error!\n"); 
            return -1;
        }

        *pdelim3++ = 0;

        fal_vlan_members_t port_list;
        int pl_num = JW_ObjStrToArray(MV88E6097_MAX_MEMBERS_SIZE,port_list.members,pdelim2);
        if(pl_num == -1)
        {
            LOG_ERROR("parse vlan dot1q entry portlist fail!\n");
            return -1;
        }

        fal_vlan_egress_tag_t tag_list;
        int tl_num = JW_ObjStrToArray(MV88E6097_MAX_MEMBERS_SIZE,tag_list.members,pdelim3);
        if(tl_num == -1)
        {
            LOG_ERROR("parse vlan dot1q entry taglist fail!\n");
            return -1;
        }

        /**<    set entry to driver  */
        if(vlan.mode == FAL_VLAN_MODE_8021Q)
        {
            ret = fal_8021q_vlan_row_create(0,id,&port_list,pl_num,&tag_list);
            if(ret)
            {
                LOG_ERROR("create 802.1q vlan fail!\n");
                return -1;
            }
            LOG_NORMAL("vlan creat entry :id = %d,name = %s,portlist = %s, taglist = %s OK\n",id,pdelim1,pdelim2,pdelim3);
        }

        /**<    link entry to list  */
        vlanList_S *vl = (vlanList_S *)calloc(1,sizeof(vlanList_S));
        list_add_tail(&vl->node,&vlan.entry);

        vl->id = id;
        strcpy(vl->name,pdelim1);
        vl->pl_num = vl->tl_num = pl_num;
        memcpy(&vl->port_list,&port_list,sizeof(fal_vlan_members_t));
        memcpy(&vl->tag_list,&tag_list,sizeof(fal_vlan_egress_tag_t));
        vl->idx = ++count;
    }

    vlan.dot1q_num = count;

    LOG_NORMAL("vlan load entry OK\n");
    
    return 0;
}

static UciConfig_S vlanUci[] = {
    {.name = "mode",        .type = UCI_TYPE_STRING,    .ubusGet = vlanLoadMode},
    {.name = "port_base",   .type = UCI_TYPE_STRING,    .ubusGet = vlanLoadPortBase},
    {.name = "dot1q_entry", .type = UCI_TYPE_LIST,      .ubusGet = vlanLoadDot1qEntry},
};

static int vlanCfgLoad(void)
{
    memset(&vlan,0,sizeof(vlan));
    INIT_LIST_HEAD(&vlan.entry);

    /**<    link const entry used to store vlan id 1 to list  */
    vlanList_S *vl = (vlanList_S *)calloc(1,sizeof(vlanList_S));
    list_add_tail(&vl->node,&vlan.entry);
    vl->id = 1;
    vl->idx = 1;
    strcpy(vl->name,"default");
    vl->pl_num = vl->tl_num = (CONFIG_JWS_PORT_TOTAL_NR/8 + ((CONFIG_JWS_PORT_TOTAL_NR%8)?1:0));
    
    vlan.dot1q_num = 1;

    /**<    load configure from UCI  */
    struct uci_context *ctx = uci_alloc_context();
    
    char name[64]; 
    int ret;
    int i;
    struct uci_ptr ptr;

    for(i = 0;i < sizeof(vlanUci)/sizeof(UciConfig_S);i++)
    {
        sprintf(name,"switchd.vlan.%s",vlanUci[i].name);
        memset(&ptr, 0, sizeof(struct uci_ptr));
        ret = load_config(name,vlanUci[i].type,&ptr,ctx);
        if(ret)
        {
            if(vlanUci[i].type == UCI_TYPE_LIST) continue;

            LOG_ERROR("load config %s fail!\n",name); 
            uci_free_context(ctx);
            return -1;
        }
        
        ret = vlanUci[i].ubusGet(&ptr);
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


void ubus_init_vlan_config(struct ubus_context *ctx)
{
	int ret;

	_ctx = ctx;

    ret = vlanCfgLoad();
    if(ret)
    {
		LOG_ERROR("vlan load configure from UCI file fail\n");
        return;
    }

	ret = ubus_add_object(ctx, &vlan_object);
	if (ret)
		LOG_ERROR("Failed to add object vlan config: %s\n", ubus_strerror(ret));
}
