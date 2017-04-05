#include "ubus_call.h"
#include "device_info.h"
#include "fdb.h"

static void recive_get_fdb_mode(struct ubus_request * req, int type,  struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
            if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
                blobmsg_for_each_attr(lvl[4],lvl[3], rem[4]){
                if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
                    blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
                    if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
                        device_gt->current_mode = (int)str2cs32(blobmsg_get_string(lvl[5]));
                    }else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
                }else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_get_fdb_num(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
            if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
                device_gt->idx_num = atoi(blobmsg_get_string(lvl[3]));
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_repeat_fdb(struct ubus_request * req, int type,  struct blob_attr *msg){
    int rem[6];
    char * port=NULL;
    char * mac=NULL;
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
            if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
                blobmsg_for_each_attr(lvl[4],lvl[3], rem[4]){
                if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
                    blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
                    if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
                        if(port)
                            mac=blobmsg_get_string(lvl[5]);
                        else
                            port=blobmsg_get_string(lvl[5]);
                    }else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
                }else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    if((0==strcmp(port_name_to_num(port_idx_to_port(device_gt->current_idx)), port))\
            && (0==strcmp(device_gt->tmp, mac)))
        device_gt->current_mode=1;
}

void set_mac_aging_time(const char * time){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
    tb[1] = blobmsg_open_table(&buf, NULL);
        blobmsg_add_string(&buf, "aging_time", time);
    blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "fdb";
    const char *method = "fdb_set";

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void set_fdb_mode(const char * port, const char * mode){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "status", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "fdb";
    const char *method = "fdb_set";

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void repeat_fdb(const char * __idx){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, __idx);
                    blobmsg_add_string(&buf, NULL, "port");
                    blobmsg_add_string(&buf, NULL, "mac_address");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "fdb";
    const char *method = "fdb_get";
    ubus_call(path, method, &buf, &recive_repeat_fdb);
}

void get_fdb_num_d(void){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        blobmsg_add_string(&buf, NULL, "entry_num");
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "fdb";
    const char *method = "fdb_get";

    ubus_call(path, method, &buf, &recive_get_fdb_num);
}

void get_fdb_mode_d(const char * port){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "status");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "fdb";
    const char *method = "fdb_get";

    ubus_call(path, method, &buf, &recive_get_fdb_mode);
}

void set_fdb_mac(const char * __idx, const char * port, const char * mac){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, __idx);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "port", port);
                blobmsg_close_table(&buf, tb[3]);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "mac_address", mac);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "fdb";
    const char *method = "fdb_set";
    ubus_call(path, method, &buf, &receive_set_result_data);
}
