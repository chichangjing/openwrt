#include "ubus_call.h"
#include "device_info.h"
#include "device.h"
#include "ob_ring.h"

static void recive_obring_get_status(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
            if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
                if(blobmsg_get_string(lvl[3])
                        && is_csu8(blobmsg_get_string(lvl[3])))
                    global_ring->mode = str2csu8(blobmsg_get_string(lvl[3]));
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_obring_get_num(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
            if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
                if(blobmsg_get_string(lvl[3])
                        && is_csu32(blobmsg_get_string(lvl[3]))){
                    dev_ring_spec(str2csu32(blobmsg_get_string(lvl[3])));
                }
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_obring_get_global_ring(struct ubus_request * req, int type, struct blob_attr * msg){
    int rem[6];
    int i=0, point=0;
    const char * idx;
    struct blob_attr * lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
            if(i==global_ring->count)
                return;
            global_ring->dev_ring[i].idx = idx2id(blobmsg_name(lvl[3]));
            point = 0;
            if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
                blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
                if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
                    blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
                    if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
                        switch(point){
                            case 0:
                                global_ring->dev_ring[i].ring_id = ring2id(blobmsg_get_string(lvl[5]));
                                break;
                            case 1:
                                global_ring->dev_ring[i].mode = is_csu8(blobmsg_get_string(lvl[5])) ? \
                                        str2csu8(blobmsg_get_string(lvl[5])) : 0;
                                break;
                            case 2:
                                global_ring->dev_ring[i].priority = is_csu8(blobmsg_get_string(lvl[5])) ? \
                                        str2csu8(blobmsg_get_string(lvl[5])) : 0;
                                break;
                            case 3:
                                if(str2csu32(blobmsg_get_string(lvl[5]))>=PORT_1 && str2csu32(blobmsg_get_string(lvl[5]))<_PORT_MAX){
                                    global_ring->dev_ring[i].master = &global_port->dev_port[str2csu32(blobmsg_get_string(lvl[5]))];
                                    global_port->dev_port[str2csu32(blobmsg_get_string(lvl[5]))].dev_ring = &global_ring->dev_ring[i];
                                }else{
                                    global_ring->dev_ring[i].master= NULL;
                                    }
                                break;
                            case 4:
                                if(str2csu32(blobmsg_get_string(lvl[5]))>=PORT_1 && str2csu32(blobmsg_get_string(lvl[5]))<_PORT_MAX){
                                    global_ring->dev_ring[i].slave = &global_port->dev_port[str2csu32(blobmsg_get_string(lvl[5]))];
                                    global_port->dev_port[str2csu32(blobmsg_get_string(lvl[5]))].dev_ring = &global_ring->dev_ring[i];
                                }else{
                                    global_ring->dev_ring[i].slave= NULL;
                                    }
                                break;
                            default:
                                break;
                        }
                        point++;
                    }else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
                }else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
            i++;
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

void obring_get_status(void){
    void *tb[3];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        blobmsg_add_string(&buf, NULL, "jrpp_status");
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "ring_config";
    const char *method = "ring_config_get";

    ubus_call(path, method, &buf, &recive_obring_get_status);
}

void obring_get_num(void){
    void *tb[3];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        blobmsg_add_string(&buf, NULL, "ring_num");
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "ring_config";
    const char *method = "ring_config_get";

    ubus_call(path, method, &buf, &recive_obring_get_num);
}


void obring_get_global_ring(void){
    int i;
    void * tb[3];
    char idx[10];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        for(i=1; i<=global_ring->count; i++){
            tb[1] = blobmsg_open_table(&buf, NULL);
                sprintf(idx, "%4s%d", "idx-", i);
                tb[2] = blobmsg_open_array(&buf, idx);
                    blobmsg_add_string(&buf, NULL, "ring_name");
                    blobmsg_add_string(&buf, NULL, "admin");
                    blobmsg_add_string(&buf, NULL, "priority");
                    blobmsg_add_string(&buf, NULL, "primary_port");
                    blobmsg_add_string(&buf, NULL, "secondary_port");
                blobmsg_close_array(&buf, tb[2]);
            blobmsg_close_table(&buf, tb[1]);
        }
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "ring_config";
    const char *method = "ring_config_get";

    ubus_call(path, method, &buf, &recive_obring_get_global_ring);
}

void obring_get_general(void){
    obring_get_num();
    obring_get_status();
    obring_get_global_ring();
}

void dev_ring_list(cs_u32 ring_id){
    int i=0;
    //printf("%16s : %-d\n", "idx", global_ring->dev_ring[__I].idx);
#define __print_ring(__I)   do{HIGHT_LIGHT();\
                                printf("%64s\n", id2ring(global_ring->dev_ring[__I].ring_id));\
                                UN_HIGHT_LIGHT();\
                                print_row(64);\
                                printf("%16s : %-s\n", "mode", global_ring->dev_ring[__I].mode ? \
                                        jrpp_mode[global_ring->dev_ring[__I].mode]:"Not defined");\
                                printf("%16s : %-s\n", "master port", global_ring->dev_ring[__I].master ? \
                                        global_ring->dev_ring[__I].master->name:"Not defined");\
                                printf("%16s : %-s\n", "slave port", global_ring->dev_ring[__I].slave ? \
                                        global_ring->dev_ring[__I].slave->name:"Not defined");\
                                printf("%16s : %-s\n", "priority", ring_prio[global_ring->dev_ring[__I].priority] ? \
                                        ring_prio[global_ring->dev_ring[__I].priority]:"Not defined");\
                            }while(0)
    if(ring_id){
        for(;i<global_ring->count;i++){
            if(ring_id == global_ring->dev_ring[i].ring_id){
                __print_ring(i);
                return;
            }else{
                continue;
            }
        }
       printf(" %% Ring %d is not exsit!\n", ring_id);
       return;
    }else{
        for(;i<global_ring->count;i++){
            __print_ring(i);
        }
        return;
    }
}

void dev_ring_clear(cs_u32 ring_id){
    int i=0;
    if(ring_id){
        for(;i<global_ring->count;i++){
            if(ring_id == global_ring->dev_ring[i].ring_id){
                printf(" %% Clear ring %d.\n", global_ring->dev_ring[i].ring_id);
                obring_delete(id2idx(global_ring->dev_ring[i].idx));
                return;
            }else{
                continue;
            }
        }
       printf(" %% Ring %d is not exsit!\n", ring_id);
       return;
    }else{
        for(i=global_ring->count-1;i>=0;i--){
            printf(" %% Clear ring %d.\n", global_ring->dev_ring[i].ring_id);
            obring_delete(id2idx(global_ring->dev_ring[i].idx));
        }
        return;
    }
}

void obring_set_status(const char * mode){
    void *tb[3];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
    tb[1] = blobmsg_open_table(&buf, NULL);
        blobmsg_add_string(&buf, "jrpp_status", mode);
    blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "ring_config";
    const char *method = "ring_config_set";

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void obring_set_primary_port(const char * _idx, const char * port){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, _idx);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "primary_port", port);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "ring_config";
    const char *method = "ring_config_set";

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void obring_set_second_port(const char * _idx, const char * port){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, _idx);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "secondary_port", port);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "ring_config";
    const char *method = "ring_config_set";

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void obring_set_priority(const char * _idx, const char * priority){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, _idx);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "priority", priority);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "ring_config";
    const char *method = "ring_config_set";

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void obring_set_mode(const char * _idx, const char * mode){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, _idx);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "admin", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "ring_config";
    const char *method = "ring_config_set";

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void obring_name(const char * _idx, const char * name){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, _idx);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ring_name", name);
                blobmsg_close_table(&buf, tb[3]);
                tb[3] = blobmsg_open_table(&buf, NULL);
                blobmsg_add_string(&buf, "admin", cs82str(_JRPP_MODE_DISABLE));
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "ring_config";
    const char *method = "ring_config_set";

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void obring_delete(const char * _idx){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "clear_args");
    blobmsg_add_string(&buf, NULL, _idx);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "ring_config";
    const char *method = "ring_config_clear";

    ubus_call(path, method, &buf, &receive_set_result_data);
}
