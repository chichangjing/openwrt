#include "mcast_config.h"
#include "ubus_call.h"
#include "device_info.h"
#include "device.h"

static void recive_mcast_config_enable_status(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
			    global_mcast->mode = is_csu8(blobmsg_get_string(lvl[3])) ? str2csu8(blobmsg_get_string(lvl[3])):0;
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_mcast_config_group_num(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
                global_mcast->count = is_csu32(blobmsg_get_string(lvl[3])) ? str2csu32(blobmsg_get_string(lvl[3])):0;
                dev_mcast_spec(global_mcast->count);
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_mcast_config_group_members(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	int i=0, m, point;
	char * member;
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
	            if(i==global_mcast->count)
	                return;
	            global_mcast->dev_mcast[i].idx = idx2id(blobmsg_name(lvl[3]));
	            point = 0;
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_ARRAY){
				blobmsg_for_each_attr(lvl[4],lvl[3], rem[4]){
				if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
					blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
					if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
					    switch(point){
                            case 0:
                                global_mcast->dev_mcast[i].mac = str2mac(blobmsg_get_string(lvl[5]));
                                break;
                            case 1:
                                global_mcast->dev_mcast[i].port = 0;
                                    for(member = blobmsg_get_string(lvl[5]); *member; member++)
                                            global_mcast->dev_mcast[i].port = is_hex(*member) \
                                                    ? global_mcast->dev_mcast[i].port<<4 | letter2num(*member)\
                                                    : global_mcast->dev_mcast[i].port;
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

void mcast_config_get_enable_status(void){
	void *tb;
	blobmsg_buf_init(&buf);
	tb = blobmsg_open_array(&buf, "get_args");
	blobmsg_add_string(&buf, NULL, "enable_status");
	blobmsg_close_array(&buf, tb);

	const char *path = "mcast_config";
	const char *method = "mcast_config_get";

	ubus_call(path, method, &buf, &recive_mcast_config_enable_status);
}

void mcast_config_get_group_num(void){
	void *tb;
	blobmsg_buf_init(&buf);
	tb = blobmsg_open_array(&buf, "get_args");
	blobmsg_add_string(&buf, NULL, "group_num");
	blobmsg_close_array(&buf, tb);

	const char *path = "mcast_config";
	const char *method = "mcast_config_get";

	ubus_call(path, method, &buf, &recive_mcast_config_group_num);
}

void mcast_config_get_group_members(void){
    int i;
    void * tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        for(i=0; i<global_mcast->count; i++){
            tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, id2idx(i+1));
                blobmsg_add_string(&buf, NULL, "mac_address");
                blobmsg_add_string(&buf, NULL, "group_members");
            blobmsg_close_array(&buf, tb[2]);
            blobmsg_close_table(&buf, tb[1]);
        }
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "mcast_config";
    const char *method = "mcast_config_get";

    ubus_call(path, method, &buf, &recive_mcast_config_group_members);
}

void mcast_list(void){
    cs_u32 i = 0, port=0;
    if(!global_mcast->count){
        printf(" %% No mutilcast group set!\n");
        return;
    }
    HIGHT_LIGHT();
    _PRINT_INFO("%98", "Static Mutilcast Group Members:\n")
    UN_HIGHT_LIGHT();
    print_row(98);
    for(;i<global_mcast->count;i++){
        printf(" %-16s:", mac2str(global_mcast->dev_mcast[i].mac, _base_mac_2));
        for(port=PORT_1;port<_PORT_MAX;port++){
            1<<(16-((port-PORT_1)+1)) & global_mcast->dev_mcast[i].port ?\
                    printf(" %6s ", port_name[2][port]):printf("    %s   ", "~");
        }
        printf("\n");
    }
    print_row(98);
}


void mcast_get_general(void){
    mcast_config_get_group_num();
    mcast_config_get_enable_status();
    mcast_config_get_group_members();
}


static void recive_mcast_config_set_enable_status(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% Set Mutilcast status successfully.\n")
		else
			_PRINT_INFO(" %% Set Mutilcast status failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

static void recive_mcast_config_set_mac_address(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0")){
			_PRINT_INFO(" %% Mutilcast Mac address set successfully.\n")
		}else{
			_PRINT_INFO(" %% Mutilcast Mac address set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
		}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

void recive_mcast_config_set_group_members_one(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% Mutilcast Group Members set successfully.\n")
		else
			_PRINT_INFO(" %% Mutilcast Group Members set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

void recive_mcast_clear_one(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% Mutilcast Group Members clear successfully.\n")
		else
			_PRINT_INFO(" %% Mutilcast Group Members clear failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

void mcast_config_set_enable_status(const char * mode){
	void *tb[2];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
	blobmsg_add_string(&buf, "enable_status", mode);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "mcast_config";
	const char *method = "mcast_config_set";

	ubus_call(path, method, &buf, &recive_mcast_config_set_enable_status);
}

void mcast_config_set_mac_address_one(const char * idx, char * mac){
	void *tb[4];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
	tb[2] = blobmsg_open_array(&buf, idx);
	tb[3] = blobmsg_open_table(&buf, NULL);
	blobmsg_add_string(&buf, "mac_address", mac);
	blobmsg_close_table(&buf, tb[3]);
	blobmsg_close_array(&buf, tb[2]);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);
	const char *path = "mcast_config";
	const char *method = "mcast_config_set";

	ubus_call(path, method, &buf, &recive_mcast_config_set_mac_address);
}

void mcast_config_set_group_members_one(const char * idx, const char * group_members){
	void *tb[4];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
	tb[1] = blobmsg_open_table(&buf, NULL);
	tb[2] = blobmsg_open_array(&buf, idx);
	tb[3] = blobmsg_open_table(&buf, NULL);
	blobmsg_add_string(&buf, "group_members", group_members);
	blobmsg_close_table(&buf, tb[3]);
	blobmsg_close_array(&buf, tb[2]);
	blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "mcast_config";
	const char *method = "mcast_config_set";

	ubus_call(path, method, &buf, &recive_mcast_config_set_group_members_one);
}

void mcast_config_clear_one(const char * idx){
	void *tb;
	blobmsg_buf_init(&buf);
	tb = blobmsg_open_array(&buf, "clear_args");
		blobmsg_add_string(&buf, NULL, idx);
	blobmsg_close_array(&buf, tb);

	const char *path = "mcast_config";
	const char *method = "mcast_config_clear";

	ubus_call(path, method, &buf, &recive_mcast_clear_one);
}
