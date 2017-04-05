#include "ubus_call.h"
#include "device_info.h"
#include "rate_limit.h"

static void recive_get_rate_limit_general(struct ubus_request *req, int type, struct blob_attr *msg){
    int i;
    int mask, f_type;
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
            if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
                HIGHT_LIGHT();
                printf("%126s\n", blobmsg_name(lvl[3]));
                UN_HIGHT_LIGHT();
                print_row(126);
                i=0;
                blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
                if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
                    blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
                    if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
                        switch (i){
                            case 0:
                            case 3:
                            case 6:
                            case 9:
                            case 12:
                                if(!(strcmp(blobmsg_get_string(lvl[5]), "1"))){
                                    printf("%s%d: %8s", " bucket ", (i/3), "enable");
                                }else if(!(strcmp(blobmsg_get_string(lvl[5]), "2"))){
                                    printf("%s%d: %8s", " bucket ", (i/3), "disable");
                                }else{
                                    printf("%s%d: %8s", " bucket ", (i/3), "error");
                                    }
                                break;
                            case 1:
                            case 4:
                            case 7:
                            case 10:
                            case 13:
                                printf("\t%s: %6s Kbps", "rate", blobmsg_get_string(lvl[5]));
                                break;
                            case 2:
                            case 5:
                            case 8:
                            case 11:
                            case 14:
                                mask = _atoi_s(blobmsg_get_string(lvl[5]));
                                printf("%16s: ", "frame type");
                                for(f_type=_RATE_LIMITING_TYPE_UNKNOWN_UNICAST_FRAME;f_type<_RATE_LIMITING_TYPE_MAX;f_type++)
                                    ((1<<(f_type-1)) & mask) ? printf("  %s,", rate_limit_frame_type[f_type]):0;
                                MOVELEFT(1);
                                CLEAR_ROW();
                                printf("\n");
                                break;
                            case 15:
                                if(!(strcmp(blobmsg_get_string(lvl[5]), "1"))){
                                    printf("%s: %8s", " outbound", "enable");
                                }else if(!(strcmp(blobmsg_get_string(lvl[5]), "2"))){
                                    printf("%s: %8s", " outbound", "disable");
                                }else{
                                    printf("%s: %8s", " outbound", "error");
                                    }
                                break;
                            case 16:
                                printf("\t%s: %6s Kbps\n", "rate", blobmsg_get_string(lvl[5]));
                                break;
                        }
                        i++;
                    }else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
                }else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    print_row(126);
    fflush(stdout);
}

static void recive_ingress_get_frame(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
            if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_ARRAY){
                blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
                if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
                    blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
                    if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
                        device_gt->vlan_port_list = atoi(blobmsg_get_string(lvl[5]));
                    }else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
                }else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_ingress_buck0_get_status(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
            if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_ARRAY){
                blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
                if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
                    blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
                    if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
                        device_gt->current_mode = atoi(blobmsg_get_string(lvl[5]));
                    }else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
                }else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

void rate_limit_get_general(void){
    int i;
    void *tb[3];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        for(i=PORT_1; i<_PORT_MAX; i++){
            tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port_name[2][i]);
                blobmsg_add_string(&buf, NULL, "ingress_bucket0_enable");
                blobmsg_add_string(&buf, NULL, "ingress_bucket0_rate");
                blobmsg_add_string(&buf, NULL, "ingress_bucket0_type_mask");

                blobmsg_add_string(&buf, NULL, "ingress_bucket1_enable");
                blobmsg_add_string(&buf, NULL, "ingress_bucket1_rate");
                blobmsg_add_string(&buf, NULL, "ingress_bucket1_type_mask");

                blobmsg_add_string(&buf, NULL, "ingress_bucket2_enable");
                blobmsg_add_string(&buf, NULL, "ingress_bucket2_rate");
                blobmsg_add_string(&buf, NULL, "ingress_bucket2_type_mask");

                blobmsg_add_string(&buf, NULL, "ingress_bucket3_enable");
                blobmsg_add_string(&buf, NULL, "ingress_bucket3_rate");
                blobmsg_add_string(&buf, NULL, "ingress_bucket3_type_mask");

                blobmsg_add_string(&buf, NULL, "ingress_bucket4_enable");
                blobmsg_add_string(&buf, NULL, "ingress_bucket4_rate");
                blobmsg_add_string(&buf, NULL, "ingress_bucket4_type_mask");

                blobmsg_add_string(&buf, NULL, "egress_rate_enable");
                blobmsg_add_string(&buf, NULL, "egress_rate");
            blobmsg_close_array(&buf, tb[2]);
            blobmsg_close_table(&buf, tb[1]);
        }
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "rate_limit";
    const char *method = "rate_limit_get";

    ubus_call(path, method, &buf, &recive_get_rate_limit_general);
}

void ingress_buck0_get_frame(const char * port){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_get";

    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "ingress_bucket0_type_mask");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_ingress_get_frame);
}
void ingress_buck1_get_frame(const char * port){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_get";

    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "ingress_bucket1_type_mask");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_ingress_get_frame);
}
void ingress_buck2_get_frame(const char * port){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_get";

    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "ingress_bucket2_type_mask");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_ingress_get_frame);
}
void ingress_buck3_get_frame(const char * port){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_get";

    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "ingress_bucket3_type_mask");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_ingress_get_frame);
}
void ingress_buck4_get_frame(const char * port){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_get";

    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "ingress_bucket4_type_mask");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_ingress_get_frame);
}

void ingress_buck0_set_status(const char * port, const char * mode){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket0_enable", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck1_set_status(const char * port, const char * mode){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket1_enable", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck2_set_status(const char * port, const char * mode){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket2_enable", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck3_set_status(const char * port, const char * mode){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket3_enable", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck4_set_status(const char * port, const char * mode){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket4_enable", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck0_get_status(const char * port){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_get";

    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "ingress_bucket0_enable");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_ingress_buck0_get_status);
}

void ingress_buck1_get_status(const char * port){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_get";

    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "ingress_bucket1_enable");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_ingress_buck0_get_status);
}

void ingress_buck2_get_status(const char * port){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_get";

    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "ingress_bucket2_enable");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_ingress_buck0_get_status);
}
void ingress_buck3_get_status(const char * port){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_get";

    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "ingress_bucket3_enable");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_ingress_buck0_get_status);
}
void ingress_buck4_get_status(const char * port){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_get";

    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "ingress_bucket4_enable");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_ingress_buck0_get_status);
}

void ingress_buck0_set_value(const char * port, const char * value){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket0_rate", value);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck0_set_frame(const char * port, const char * frame){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket0_type_mask", frame);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck1_set_value(const char * port, const char * value){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket1_rate", value);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck1_set_frame(const char * port, const char * frame){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket1_type_mask", frame);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck2_set_value(const char * port, const char * value){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket2_rate", value);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck2_set_frame(const char * port, const char * frame){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket2_type_mask", frame);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck3_set_value(const char * port, const char * value){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket3_rate", value);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck3_set_frame(const char * port, const char * frame){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket3_type_mask", frame);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck4_set_value(const char * port, const char * value){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket4_rate", value);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void ingress_buck4_set_frame(const char * port, const char * frame){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "ingress_bucket4_type_mask", frame);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}


void outgress_set_status(const char * port, const char * mode){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "egress_rate_enable", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}

void outgress_get_status(const char * port){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_get";

    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "egress_rate_enable");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_ingress_buck0_get_status);
}

void outgress_set_value(const char * port, const char * value){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "rate_limit";
    const char *method = "rate_limit_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "egress_rate", value);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &receive_set_result_data);
}
