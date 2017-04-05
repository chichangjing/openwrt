
#include "ctype.h"
#include "switchd.h"
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>


const void *jw_switchd_get_context(const char *name, const struct jw_switch_policy *policy_tbl, int item_max)
{
    int i = 0;

    for (i = 0; i < item_max; i++) 
    {
        if (!strcmp(name, policy_tbl[i].name)) 
        {
            return &policy_tbl[i];
        }
    }

    return NULL;
}

/* *
 * * @brief :       char to hex
 * * @param :       ch  - character
 * * @retval :      0   - OK
 * * @retval :      -1  - ERROR
 * */
int JW_CharToHex(char ch)
{
    if(ch >= '0' && ch <= '9')
        return ch - '0';
    if(ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    if(ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    
    return 0;
}

/* *
 * * @brief :       octet string to array
 * * @param :       max     - max size of array
 * * @param :       *dest   - buffer store array
 * * @param :       *str    - buffer store octet string
 * * @retval :      0       - OK
 * * @retval :      -1      - ERROR
 * */
int JW_ObjStrToArray(int max,unsigned char dest[],char *str)
{
    if(strlen(str) > (max * 3 - 1))
    {
        LOG_ERROR("analyse group member error!\n");
        return -1;
    }

    char *p;
    char offset;
    int num = 0;
    do{
        p = strchr(str,'-');
        if(!p)
            offset = strlen(str);
        else
            offset = p - str;

        if(offset == 2 && isxdigit(*str) && isxdigit(*(str + 1)))
        {
            *dest++ = JW_CharToHex(*str++) * 16 + JW_CharToHex(*str++);
        }
        else if(offset == 1 && isxdigit(*str))
        {
            *dest++ = JW_CharToHex(*str++);
        }
        else
        {
            LOG_ERROR("analyse group member error!\n");
            return -1;
        }

        num++;
        if(!*str++)
            break;
    }while(1);

    return num;
}

/* *
 * * @brief :       load corresponding arguement's value from UCI file
 * * @param :       *name   - option name
 * * @param :       *type   - string or list
 * * @param :       *ptr    - UCI pointer
 * * @retval :      0       - OK
 * * @retval :      -1      - ERROR
 * */
int load_config(char *name,enum uci_option_type type,struct uci_ptr *ptr,struct uci_context *ctx)
{
    if(name == NULL)
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;    
    }
    
//    char *name_bak = strdup(name);

    if(UCI_OK != uci_lookup_ptr(ctx,ptr,name,true))
    {
        LOG_WARN("no matching arguement: %s!\n",name); 
//        free(name_bak);
        return -1;
    }
    
//    free(name_bak);
    if(ptr->o !=NULL){
        if(ptr->o->type != type)
        {
            LOG_ERROR("UCI option type is not match!\n"); 
            return -1; 
        }
    }else{
        LOG_WARN("UCI list is NULL!\n"); 
        return -1; 
    }

    return 0;
}

int set_config(struct uci_context *ctx,struct uci_ptr *ptr,enum uci_option_type type)
{
    int ret;

    if(type == UCI_TYPE_STRING)
    {
        ret = uci_set(ctx,ptr);
        if(ret)
        {
            LOG_ERROR("ubus set fail!\n"); 
            return -1;
        }
    }
    else if(type == UCI_TYPE_LIST)
    {
        ret = uci_add_list(ctx,ptr);
        if(ret)
        {
            LOG_ERROR("ubus add list fail!\n"); 
            return -1;
        }
    }
    else
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }
   
    return 0;
}

int del_config(struct uci_context *ctx,struct uci_ptr *ptr,enum uci_option_type type)
{
    int ret;

    if(type == UCI_TYPE_STRING)
    {
        ret = uci_delete(ctx,ptr);
        if(ret)
        {
            LOG_ERROR("ubus delete option fail!\n"); 
            return -1;
        }
    }
    else if(type == UCI_TYPE_LIST)
    {
        ret = uci_del_list(ctx,ptr);
        if(ret)
        {
            LOG_ERROR("ubus delete list fail!\n"); 
            return -1;
        }
    }
    else
    {
        LOG_ERROR("invalid arguement!\n"); 
        return -1;
    }
   
    return 0;
}

struct json_object *json_search_element(struct json_object *object,const char *name)
{
    if(name == NULL)
    {
        LOG_ERROR("arguement - name is invalid!\n");
        return NULL;
    }

    json_type type = json_object_get_type(object);
    if(type == json_type_object)
    {
        json_object_object_foreach(object,key,value)
        {
            if(!strcmp(name,key))
            {
                return value; 
            }
            
            type = json_object_get_type(value);
            if(type == json_type_object || type == json_type_array)
            {
                struct json_object *obj = json_search_element(value,name);
                if(obj != NULL)
                {
                    return obj;
                }
            }
        }
    }
    else if(type == json_type_array)
    {
        int len = json_object_array_length(object);
        int i;
        struct json_object *array;

        for(i = 0;i < len;i++)
        {
            array = json_object_array_get_idx(object,i); 

            if(json_object_get_type(array) != json_type_object)
                continue;

            struct json_object *obj = json_search_element(array,name);
            if(obj != NULL)
            {
                return obj; 
            }
        }
    }

    return NULL;
}

int if_valid_ip(const char *str)
{
    struct in_addr addr;
    
    if(inet_pton(AF_INET,str,&addr) <= 0)
        return -1;

    return 0;
}

void foreach_json_array_get(int idx,struct blob_attr *lvl3,struct blob_buf *buf,const struct jw_switch_policy *tb,int tb_size)
{
    int rem;
    void *ret_table;
    struct blob_attr *lvl4;
    struct jw_switch_policy *policy_p;
    
    blobmsg_for_each_attr(lvl4,lvl3,rem) 
    {
        if(blobmsg_type(lvl4) == BLOBMSG_TYPE_STRING)
        {
            char *value = blobmsg_get_string(lvl4);

            policy_p = (struct jw_switch_policy *)jw_switchd_get_context(value, tb, tb_size);
            if(policy_p && policy_p->get_handler)
            {
                ret_table = blobmsg_open_table(buf, NULL);
                policy_p->get_handler(buf,idx);
                blobmsg_close_table(buf, ret_table);	
            }
            else
            {
                LOG_ERROR("policy check not pass!\n");
            }
        }
        else
        {
            LOG_ERROR("level 4 message type is not available!\n");
        }
    }
}

int foreach_json_array_set(int idx,struct blob_attr *lvl3,const struct jw_switch_policy *tb,int tb_size)
{
    int rem;
    int ret;
    struct blob_attr *lvl4;

    blobmsg_for_each_attr(lvl4, lvl3, rem) 
    {
        if (blobmsg_type(lvl4) == BLOBMSG_TYPE_TABLE) 
        {
            struct blob_attr *lvl5 = blobmsg_data(lvl4);

            if (blobmsg_type(lvl5) == BLOBMSG_TYPE_STRING) 
            {
                struct jw_switch_policy *policy_p;
                char *value = blobmsg_get_string(lvl5);
                const char *name = blobmsg_name(lvl5);

                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name,tb,tb_size);
                if (policy_p && policy_p->set_handler) 
                {
                    ret = policy_p->set_handler(idx,value);
                    if(ret) 
                    {
                        LOG_ERROR("set handler error!\n");
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
 * * @brief         parse cfg get
 * * @param[lvl3]   level 3 message.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int parseCfgGet(struct blob_attr *lvl3, struct blob_buf *buf, const struct jw_switch_policy *info_tbl, int max)
{
    const char *name    = blobmsg_name(lvl3);
    int idx             = atoi(strstr(name,"-")+1);
//    LOG_NORMAL("------------------idx = %d\n",idx);

/*    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR("invalid index %d\n", idx);
        return -1;
    }*/
 
    void *ret_array;

    ret_array= blobmsg_open_array(buf, name);
    foreach_json_array_get(idx,lvl3,buf,info_tbl, max);
    blobmsg_close_array(buf, ret_array);    

    return 0;
}

/**< get handler process*/
int getHandlerProcess(struct blob_attr *lvl1,struct blob_buf *b, const struct jw_switch_policy *info_tbl, int max){
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
    int rem = 0;
    void *ret_array;
    void *ret_table;

    blob_buf_init(b, 0);

    ret_array = blobmsg_open_array(b, "ret");

    blobmsg_for_each_attr(lvl2, lvl1, rem) 
    {
        switch(blobmsg_type(lvl2))
        {
            case BLOBMSG_TYPE_STRING:
            {
                struct jw_switch_policy *policy_p;
                char *name = blobmsg_get_string(lvl2);

                policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, info_tbl, max);
                if (policy_p && policy_p->get_ext_handler) 
                {
                    ret_table = blobmsg_open_table(b, NULL);
                    policy_p->get_ext_handler(b);
                    blobmsg_close_table(b, ret_table);
                } 
                else 
                {
                    LOG_ERROR("level 3 message type is not available!\n");
                }
                break;
            }
            case BLOBMSG_TYPE_TABLE:
            {
                struct blob_attr *lvl3 = blobmsg_data(lvl2);
                
                if (blobmsg_type(lvl3) ==  BLOBMSG_TYPE_ARRAY) 
                {
                    ret_table = blobmsg_open_table(b, NULL);
                    parseCfgGet(lvl3, b, info_tbl, max);
                    blobmsg_close_table(b, ret_table);
                } 
                else
                {
                    LOG_NORMAL("level 3 message type is not available!\n");
                }
                break;
            } 
            default: 
            {
                LOG_ERROR("level 2 message type is not available!\n");
                break;
            }
        } 
    }

    blobmsg_close_array(b, ret_array); 
    return 0;
}

/* *
 * * @brief         parse cfg set
 * * @param[lvl3]   level 3 message.
 * * @retval 0:     OK
 * * @retval -1:    ERROR
 * */
static int parseCfgSet(struct blob_attr *lvl3, struct blob_buf *buf, const struct jw_switch_policy *info_tbl, int max)
{
    const char *name    = blobmsg_name(lvl3);
    int idx             = atoi(strstr(name,"-")+1);

/*    if (idx < 1 || idx > CONFIG_JWS_PORT_TOTAL_NR) 
    {
        LOG_ERROR(" port index invalid!\n");
        return -1;;l
    }
*/

    /**<    foreach array of level 3 message  */
    int ret;
    ret = foreach_json_array_set(idx, lvl3, info_tbl, max);
    if(ret)
    {
        LOG_ERROR("foreach index %d error!\n",idx);
        return -1;
    }

    return 0;
}

/**< set handle process*/
int setHandlerProcess(struct blob_attr *lvl1,struct blob_buf *b, const struct jw_switch_policy *info_tbl, int max){
    int ret = -1;
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
    int rem = 0;
        
    blob_buf_init(b, 0);

    blobmsg_for_each_attr(lvl2,lvl1, rem) 
    {
        if (blobmsg_type(lvl2) == BLOBMSG_TYPE_TABLE) 
        {
            struct blob_attr *lvl3 = blobmsg_data(lvl2);
            
            switch(blobmsg_type(lvl3))
            {
                case BLOBMSG_TYPE_STRING:
                {
                    struct jw_switch_policy *policy_p = NULL;
                    const char *name  = blobmsg_name(lvl3);
                    char *value = blobmsg_get_string(lvl3);
                
                    policy_p = (struct jw_switch_policy *)jw_switchd_get_context(name, info_tbl, max);
                    if (policy_p && policy_p->set_ext_handler) 
                    {
                        ret = policy_p->set_ext_handler(value);
                        if(ret)
                            return -1;
                    }
                    else
                    {
                        LOG_ERROR("policy check not pass!\n");
                    }
                    break;
                }
                case BLOBMSG_TYPE_ARRAY:
                {
                    ret = parseCfgSet(lvl3, b, info_tbl, max);
                    break;
                }
                default:
                {
                    LOG_ERROR("level 3 message type is not available!\n");
                    break;
                }
            }
        } 
        else 
        {
            LOG_ERROR("level 2 message type is not available!\n");
        }
    }
    return 0;
}



/* *
 * * @brief             parse if the format of mac is correct
 * * @param[*mac_addr]  buffer to store mac
 * * @param[*p]         buffer to be parsed
 * * @retval 0:         OK
 * * @retval -1:        ERROR
 * */
int parse_mac(unsigned char mac[],char *p)
{
    int i;

    for(i = 0;i < 6;i++)
    {
        if(i < 5 && p[i*3 + 2] != '-')
        {
            LOG_ERROR("analyse \'-\' error!\n");
            return -1;
        }

        if(isxdigit(p[i*3]) && isxdigit(p[i*3 + 1])) 
        {
            mac[i] = JW_CharToHex(p[i*3]) * 16 + JW_CharToHex(p[i*3 + 1]);
        }
        else {
            LOG_ERROR("analyse mac error!\n");
            return -1;
        }
    }

    if(p[17])
    {
        LOG_ERROR("analyse EOF error!\n");
        return -1;
    }
    
    return 0; 
}

static PortStatus  g_status[CONFIG_JWS_PORT_TOTAL_NR];
/* *
 * * @brief             get the cur status of the port
 * * @param[idx]        port num
 * * @retval 0:         port status
 * */
int get_port_status(int idx)
{
    return g_status[idx-1].status;
}

/* *
 * * @brief             get the cur status of the port
 * * @param[idx]        port num
 * * @retval 0:         port status
 * */
void set_port_status(int idx, int status)
{
    g_status[idx-1].status = status;
}

int get_port_type(int port)
{
    fal_port_type_t type;
    fal_port_comb_t comb;
    sw_error_t res;

    res = fal_port_type_get(0,port, &type, &comb);

    if(res)
    {
        LOG_ERROR("port [%d] get type error = %d!\n",port, res);
        return 0;
    }
    return type;
}


#define GPIO_I2C_BASE   'H'
#define GPIO_I2C_READ   _IO(GPIO_I2C_BASE, 0x01)
#define GPIO_I2C_WRITE  _IO(GPIO_I2C_BASE, 0x02)

static int i2c_fd = -1;

static int i2c_read(const int fd, const unsigned int devaddr,
        const unsigned int regaddr, char *reg_value)
{
    unsigned int value = 0;
    int ret;

    value = ((devaddr & 0xff) << 24)|((regaddr & 0xff) << 16);
    ret = ioctl(fd, GPIO_I2C_READ, &value);
    *reg_value = value & 0xff;

    return ret;
}

int checkFpgaVersion()
{
    int ret = -1;
    char fpga_reg_value = 0;
    char id[]=CONFIG_JWS_SERIAL_NUMBER;

    if((!strcasecmp(id, "54-10-01-06-01-01-00-00")) || (!strcasecmp(id, "54-20-01-06-01-02-00-00"))){
        i2c_fd = open("/dev/gpioi2c", 0);
        if (i2c_fd<0)
        {
            LOG_ERROR("line:%d func:%s, Open gpioi2c dev error!\n", __LINE__, __FUNCTION__);
            return -1;
        }

        //通过i2c读取寄存器数值
        i2c_read(i2c_fd, 0x40, 0x19, &fpga_reg_value);
        LOG_NORMAL("fpga_reg_value = 0X%2X\n", fpga_reg_value);
        //根据唯一编码判断fpga版本
        //正确返回退出
        if(!strcasecmp(id, "54-10-01-06-01-01-00-00")){/***< 发射机*/
            if((fpga_reg_value <= 0x3f) &&(fpga_reg_value >= 0x01)){
                return 0;
            }
            else{
                ret = -1;
            }
        }     
        else if(!strcasecmp(id, "54-20-01-06-01-02-00-00")){/***< 接收机*/
            if((fpga_reg_value <= 0xaf) &&(fpga_reg_value >= 0x71)){
                return 0;
            }
            else{
                ret = -1;
            }

        }
        //错误擦除部分flash代码重启
        if(ret){
            system("dd if=/dev/mtd5 of=/tmp/111 bs=1K skip=512 count=504");
            system("jw_mtd5_erase");
            system("dd if=/tmp/111 of=/dev/mtd5 bs=1K seek=8 count=504");
            system("echo 4 > /sys/class/gpio/export");
            system("echo out > /sys/class/gpio/gpio4/direction");
            system("echo 1 > /sys/class/gpio/gpio4/value");
            system("sleep 1");
            system("echo 0 > /sys/class/gpio/gpio4/value");
            system("sleep 1");
            system("reboot");        
        }
    }
    return 0;
}

