#include "vlan.h"
#include "ubus_call.h"
#include "device_info.h"
#include "device.h"
#include "../caselib/cs_type.h"

static void recive_vlan_get_mode(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
	    blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
		    blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
			    global_vlan->mode = is_csu8(blobmsg_get_string(lvl[3])) ? str2csu8(blobmsg_get_string(lvl[3])):0;
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_vlan_set_mode(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[2];
	struct blob_attr *lvl[2] = {msg, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			printf(" %% Vlan Mode set successfully.\n");
		else
			printf(" %% Vlan Mode set failed: [%s]!\n", blobmsg_get_string(lvl[1]));
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

void vlan_get_mode(void){
	void *tb;
	blobmsg_buf_init(&buf);

	tb = blobmsg_open_array(&buf, "get_args");
		blobmsg_add_string(&buf, NULL, "mode");
	blobmsg_close_array(&buf, tb);

	const char *path = "vlan_config";
	const char *method = "vlan_config_get";

	ubus_call(path, method, &buf, &recive_vlan_get_mode);
}

void vlan_set_mode(const char *mode){
	blobmsg_buf_init(&buf);
	void *tb[2];
	tb[0] = blobmsg_open_array(&buf, "set_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
			blobmsg_add_string(&buf, "mode", mode);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "vlan_config";
	const char *method = "vlan_config_set";

	ubus_call(path, method, &buf, &recive_vlan_set_mode);
}

static void recive_port_base_vlan_get_port_list(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	char *member;
	int i=0;
	int m=0;
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_ARRAY){
				if(i>=global_isot->count)
				    return;
				global_isot->dev_isot[i].idx = port2id(blobmsg_name(lvl[3]));
				global_isot->dev_isot[i].dev_port = &global_port->dev_port[global_isot->dev_isot[i].idx];
				blobmsg_for_each_attr(lvl[4],lvl[3], rem[4]){
				if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
					blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
					if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
					    global_isot->dev_isot[i].mask = 0;
						for(member = (blobmsg_get_string(lvl[5])); *member; member++){
						        global_isot->dev_isot[i].mask =  is_hex(*member) ?
						                global_isot->dev_isot[i].mask<<4|letter2num(*member):global_isot->dev_isot[i].mask;
						}
					}else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
				}else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
			i++;
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_port_base_vlan_set_port_list(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[2];
	struct blob_attr *lvl[2] = {msg, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			printf(" %% Port Based Vlan Members set successfully.\n");
		else
			printf(" %% Port Based Vlan Members set failed: [%s]!\n", blobmsg_get_string(lvl[1]));
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

void port_base_vlan_get_port_list(void){
	int i;
	void *tb[6];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		for(i=0; i<global_isot->count; i++){
			tb[1] = blobmsg_open_table(&buf, NULL);
				tb[2] = blobmsg_open_array(&buf, id2port(i+1));
					blobmsg_add_string(&buf, NULL, "port_base_vlan_portlist");
				blobmsg_close_array(&buf, tb[2]);
			blobmsg_close_table(&buf, tb[1]);
		}
	blobmsg_close_array(&buf, tb[0]);
	const char *path = "vlan_config";
	const char *method = "vlan_config_get";

	ubus_call(path, method, &buf, &recive_port_base_vlan_get_port_list);
}

void port_base_vlan_set_port_list(const char * idx_1, const char *portlist_1, const char * idx_2, const char *portlist_2){
    void *tb[4];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
			tb[2] = blobmsg_open_array(&buf, idx_1);
				tb[3] = blobmsg_open_table(&buf, NULL);
					blobmsg_add_string(&buf, "port_base_vlan_portlist", portlist_1);
				blobmsg_close_table(&buf, tb[3]);
			blobmsg_close_array(&buf, tb[2]);
		blobmsg_close_table(&buf, tb[1]);
		tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, idx_2);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "port_base_vlan_portlist", portlist_2);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "vlan_config";
	const char *method = "vlan_config_set";

	ubus_call(path, method, &buf, &recive_port_base_vlan_set_port_list);
}


static void recive_Q8021_vlan_set_port_list(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[2];
	struct blob_attr *lvl[2] = {msg, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(0 == strcmp(blobmsg_get_string(lvl[1]), "0"))
			printf(" %% 802.1Q Vlan Members set successfully.\n");
		else
			printf(" %% 802.1Q Vlan Members set failed: [%s]!\n", blobmsg_get_string(lvl[1]));
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

static void recive_Q8021_vlan_get_port_list_all(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	char * member;
	int i=0;
	int m=0;
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	global_vlan->untag_list=0;
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_ARRAY){
				if(i>=global_vlan->count)
				    return;
				global_vlan->dev_vlan[i].idx=idx2id(blobmsg_name(lvl[3]));
				m=0;
				blobmsg_for_each_attr(lvl[4],lvl[3], rem[4]){
				if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
					blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
					if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
						switch (m){
							case 0:
							    global_vlan->dev_vlan[i].vlan_id = is_csu32(blobmsg_get_string(lvl[5]))
							        ? str2csu32(blobmsg_get_string(lvl[5])):0;
								break;
							case 1:
							    strcpy(global_vlan->dev_vlan[i].name, blobmsg_get_string(lvl[5]));
								break;

							case 2:
                            global_vlan->dev_vlan[i].tag_list = 0;
                            for(member = blobmsg_get_string(lvl[5]); *member; member++)
                                global_vlan->dev_vlan[i].tag_list = is_hex(*member) \
                                            ? global_vlan->dev_vlan[i].tag_list<<4 | letter2num(*member)\
                                            : global_vlan->dev_vlan[i].tag_list;
								break;
                        case 3:
                           global_vlan->dev_vlan[i].utag_list = 0;
                            for(member = blobmsg_get_string(lvl[5]); *member; member++)
                                global_vlan->dev_vlan[i].utag_list = is_hex(*member) \
                                            ? global_vlan->dev_vlan[i].utag_list<<4 | letter2num(*member)\
                                            : global_vlan->dev_vlan[i].utag_list;
                            global_vlan->dev_vlan[i].utag_list=~global_vlan->dev_vlan[i].tag_list&global_vlan->dev_vlan[i].utag_list;
                            if(i){
                                global_vlan->untag_list |= global_vlan->dev_vlan[i].utag_list;
                                }
                            break;
							default:
							    break;
						}
						m++;
					}else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
				}else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
				i++;
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_vlan_get_num(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
            if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
                global_vlan->count = is_csu32(blobmsg_get_string(lvl[3])) ? str2csu32(blobmsg_get_string(lvl[3])):0;
                dev_vlan_spec(global_vlan->count);
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

void vlan_get_num(void){
	void * tb;
	blobmsg_buf_init(&buf);

	tb = blobmsg_open_array(&buf, "get_args");
		blobmsg_add_string(&buf, NULL, "dot1q_vlan_num");
	blobmsg_close_array(&buf, tb);

	const char *path = "vlan_config";
	const char *method = "vlan_config_get";

	ubus_call(path, method, &buf, &recive_vlan_get_num);
}

void Q8021_vlan_get_port_list(void){
	int i;
	void * tb[6];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		for(i=0; i<global_vlan->count; i++){
			tb[1] = blobmsg_open_table(&buf, NULL);
				tb[2] = blobmsg_open_array(&buf, id2idx(i+1));
					blobmsg_add_string(&buf, NULL, "dot1q_vlan_id");
					blobmsg_add_string(&buf, NULL, "dot1q_vlan_name");
					blobmsg_add_string(&buf, NULL, "dot1q_vlan_taglist");
					blobmsg_add_string(&buf, NULL, "dot1q_vlan_portlist");
				blobmsg_close_array(&buf, tb[2]);
			blobmsg_close_table(&buf, tb[1]);
		}
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "vlan_config";
	const char *method = "vlan_config_get";

	ubus_call(path, method, &buf, &recive_Q8021_vlan_get_port_list_all);
}

void Q8021_vlan_set_id(const char *idx, const char * id){
    void *tb[4];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, idx);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "dot1q_vlan_id", id);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "vlan_config";
    const char *method = "vlan_config_set";

    ubus_call(path, method, &buf, &recive_Q8021_vlan_set_port_list);
}

void Q8021_vlan_set_name(const char * idx, const char * name){
    void *tb[4];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, idx);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "dot1q_vlan_name", name);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "vlan_config";
    const char *method = "vlan_config_set";

    ubus_call(path, method, &buf, &recive_Q8021_vlan_set_port_list);
}

void Q8021_vlan_set_port_list(const char *idx, const char *portlist, const char * taglist){
	void *tb[4];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
			tb[2] = blobmsg_open_array(&buf, idx);
				tb[3] = blobmsg_open_table(&buf, NULL);
					blobmsg_add_string(&buf, "dot1q_vlan_portlist", portlist);
				blobmsg_close_table(&buf, tb[3]);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "dot1q_vlan_taglist", taglist);
                blobmsg_close_table(&buf, tb[3]);
			blobmsg_close_array(&buf, tb[2]);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "vlan_config";
	const char *method = "vlan_config_set";

	ubus_call(path, method, &buf, &recive_Q8021_vlan_set_port_list);
}

void recive_Q8021_vlan_clear(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% Vlan clear successfully.\n")
        else
            _PRINT_INFO(" %% Vlan clear failed: [%s]!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}

void Q8021_vlan_clear(const char * idx){
    void *tb;
    blobmsg_buf_init(&buf);
    tb = blobmsg_open_array(&buf, "clear_args");
        blobmsg_add_string(&buf, NULL, idx);
    blobmsg_close_array(&buf, tb);

    const char *path = "vlan_config";
    const char *method = "vlan_config_clear";

    ubus_call(path, method, &buf, &recive_Q8021_vlan_clear);
}


void vlan_get_general(void){
    vlan_get_num();
    vlan_get_mode();
    port_base_vlan_get_port_list();
    Q8021_vlan_get_port_list();
}

void vlan_list(void){
    if(global_vlan->count){
        cs_u8 i = 0;
        cs_u8 port;
        cs_u8 num = 0;
        for(;i<global_vlan->count;i++){
            HIGHT_LIGHT();
            printf("%44s%4d\n", "vlan ", global_vlan->dev_vlan[i].vlan_id);
            UN_HIGHT_LIGHT();
            print_row(48);
            printf("%12s : %s\n", "name", global_vlan->dev_vlan[i].name);
//            printf("%12s : %d\n", "idx", global_vlan->dev_vlan[i].idx);
            printf("%12s : ", "untag port");
            for(num = 0, port=PORT_1;port<_PORT_MAX;port++){
                if(1<<(16-((port-PORT_1)+1)) & global_vlan->dev_vlan[i].utag_list){
                    printf("%s  ", port_name[2][port]);
                    num++;
                    if(4==num){
                        printf("\n%12s   ", "");
                        num=0;
                    }
                }
            }
            printf("\n");
            printf("%12s : ", "tag port");
            for(num = 0, port=PORT_1;port<_PORT_MAX;port++){
                if(1<<(16-((port-PORT_1)+1)) & global_vlan->dev_vlan[i].tag_list){
                    printf("%s  ", port_name[2][port]);
                    num++;
                    if(4==num){
                        printf("\n%12s   ", "");
                        num=0;
                    }
                }
            }
            printf("\n");
        }
        print_row(48);
    }else{
        printf(" %% No vlan set yet!\n");
    }
    printf(" %% Vlan mode is %s now. Vlan total: %d.\n", vlan_mode_tbl[0][global_vlan->mode], global_vlan->count);
}

void isolate_list(void){
    cs_u8 i = 0;
    cs_u8 port;
    for(;i<global_isot->count;i++){
        if(&global_isot->dev_isot[i] && global_isot->dev_isot[i].dev_port && global_isot->dev_isot[i].dev_port->name){
            printf("%8s : ", global_isot->dev_isot[i].dev_port->name);
            for(port=PORT_1;port<_PORT_MAX;port++){
                printf("%6s  ", 1<<(16-((port-PORT_1)+1)) & global_isot->dev_isot[i].mask?port_name[2][port]:"~");
              }
            printf("\n");
        }
    }
    printf(" %% Vlan mode is %s now.\n", vlan_mode_tbl[0][global_vlan->mode]);
}
