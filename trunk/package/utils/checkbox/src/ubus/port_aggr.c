#include "ubus_call.h"
#include "device_info.h"
#include "port_aggr.h"

static void recive_port_aggr_mode_select(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	device_gt->current_mode = -1;
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
				device_gt->current_mode = atoi(blobmsg_get_string(lvl[3]));
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_port_aggr_load_balance(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_STRING){
				if(0 == strcmp("-1", blobmsg_get_string(lvl[3]))){
					_PRINT_INFO(" %% Aggregation is Disable!\n")
				}else{
					HIGHT_LIGHT();
					_PRINT_INFO("%60s", "Aggregation Channel Mode:\n")
					UN_HIGHT_LIGHT();
					print_row(60);
					_PRINT_INFO("%44s%16s", "Aggregation load balance:\n", port_aggr_load_balance_to_show(blobmsg_get_string(lvl[3])))
					print_row(60);
				}
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

static void recive_port_aggr_channel_id_one(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	const char *idx;
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
			idx = blobmsg_name(lvl[3]);
			if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
				blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
				if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
					blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
					if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
						HIGHT_LIGHT();
						_PRINT_INFO("%44s%6s\n", "Aggregation Channel ID:", idx)
						UN_HIGHT_LIGHT();
						print_row(50);
						_PRINT_INFO("%44s%6s\n", "", blobmsg_get_string(lvl[5]))
						print_row(50);
					}else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
				}else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

static void recive_port_aggr_channel_num(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	device_gt->idx_num = -1;
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

static void recive_port_aggr_channel_id_one_d(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	const char *idx;
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
			idx = blobmsg_name(lvl[3]);
			if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
				blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
				if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
					blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
					if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
						if(0 == strcmp(blobmsg_get_string(lvl[5]), device_gt->current_view_name)){
							strcpy(device_gt->current_idx, idx);
							return;
						}
					}else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
				}else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_port_aggr_channel_member_one(struct ubus_request *req, int type, struct blob_attr *msg){
	int i;
	int rem[6];
	char *member;
	device_gt->vlan_port_list = 0x0000;
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
				blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
				if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
					blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
					if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
						member = blobmsg_get_string(lvl[5]);
						for(i=0; i<strlen(member); i++)
							'-' == member[i] ? 1:(device_gt->vlan_port_list = (device_gt->vlan_port_list<<4) + (member[i]>0x60 ? member[i]-0x57 : member[i]-0x30));
					}else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
				}else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_port_aggr_channel_member_all(struct ubus_request *req, int type, struct blob_attr *msg){
	int i;
	int tmp;
	int rem[6];
    char * member;
    device_gt->vlan_tag_list = 0x0000;
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
				blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
				if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
					blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
					if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
						member = blobmsg_get_string(lvl[5]);
						for(i=0, tmp=0; i<strlen(member); i++)
							'-' == member[i] ? 1:(tmp = (tmp<<4) + (member[i]>0x60 ? member[i]-0x57 : member[i]-0x30));
						device_gt->vlan_tag_list = device_gt->vlan_tag_list | tmp;
					}else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
				}else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

static void recive_port_aggr_channel_all(struct ubus_request *req, int type, struct blob_attr *msg){
	int m, j;
	int rem[6];
	char * member;
	HIGHT_LIGHT();
	_PRINT_INFO("%10s%8s%89s", "Group Num:", "ID:", "Members:\n")
	UN_HIGHT_LIGHT();
	print_row(107);
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
				_PRINT_INFO("%5s%4s:", "Group", blobmsg_name(lvl[3])+strlen("idx-"))
				bool mark = true;
				blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
				if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
					blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
					if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
						if(1 == mark){
							_PRINT_INFO("    [%2s]", blobmsg_get_string(lvl[5]))
						}else{
							if(!strcmp(blobmsg_get_string(lvl[5]), "")){
								_PRINT_INFO(" %% No Mutilcast Group Members Set!\n")
							}else{
								member = blobmsg_get_string(lvl[5]);
								for(j=PORT_1, m=0; j<_PORT_MAX; j++, m = m%8, m=m<<1){
									if(1 == j%4){
										m = j/4;
										m = 2 == m ? 3:m;
										m = member[m];
										m = m>0x60 ? m-0x57 : m-0x30;
									}
									if(1 == m/8)
										printf(" %7s ", port_name[1][j]);
									else
										printf(" %7s ", "   ~   ");
								}
								mark = false;
							}
							printf("\n");
						}
						mark = mark ? 0:1;
					}else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
				}else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	print_row(107);
	fflush(stdout);
}

static void recive_port_aggr_set_mode_select(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% Aggregation Mode select set successfully.\n")
		else
			_PRINT_INFO(" %% Aggregation Mode select set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

static void recive_port_aggr_set_channel_id(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% Aggregation Channel id set successfully.\n")
		else
			_PRINT_INFO(" %% Aggregation Channel id set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

static void recive_port_aggr_set_load_balance(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[2];
	struct blob_attr *lvl[2] = {msg, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% Aggregation Load balance set successfully.\n")
		else
			_PRINT_INFO(" %% Aggregation Load balance set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

static void recive_port_aggr_set_channel_member(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% Aggregation Channel member set successfully.\n")
		else
			_PRINT_INFO(" %% Aggregation Channel member set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

static void recive_port_aggr_set_channel(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% Aggregation Channel set successfully.\n")
		else
			_PRINT_INFO(" %% Aggregation Channel set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

static void recive_port_aggr_set_channel_id_d(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% Aggregation Channel id set successfully.\n")
		else
			_PRINT_INFO(" %% Aggregation Channel id set failed: [%s]\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

void recive_port_aggr_clear_one(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% Aggregation Group Members clear successfully.\n")
		else
			_PRINT_INFO(" %% Aggregation Group Members clear failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

void port_aggr_get_mode_select(void){
	void * tb;
	blobmsg_buf_init(&buf);
	tb = blobmsg_open_array(&buf, "get_args");
	blobmsg_add_string(&buf, NULL, "mode_select");
	blobmsg_close_array(&buf, tb);

	const char *path = "port_aggr";
	const char *method = "port_aggr_get";

	ubus_call(path, method, &buf, &recive_port_aggr_mode_select);
}

void port_aggr_get_load_balance(void){
	void * tb;
	blobmsg_buf_init(&buf);
	tb = blobmsg_open_array(&buf, "get_args");
	blobmsg_add_string(&buf, NULL, "load_balance");
	blobmsg_close_array(&buf, tb);

	const char *path = "port_aggr";
	const char *method = "port_aggr_get";

	ubus_call(path, method, &buf, &recive_port_aggr_load_balance);
}

void port_aggr_get_channel_num(void){
	void * tb;
	blobmsg_buf_init(&buf);
	tb = blobmsg_open_array(&buf, "get_args");
	blobmsg_add_string(&buf, NULL, "channel_num");
	blobmsg_close_array(&buf, tb);

	const char *path = "port_aggr";
	const char *method = "port_aggr_get";

	ubus_call(path, method, &buf, &recive_port_aggr_channel_num);
}

void port_aggr_get_channel_id_one(const char *idx){
	void * tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
			tb[2] = blobmsg_open_array(&buf, idx);
				blobmsg_add_string(&buf, NULL, "channel_id");
			blobmsg_close_array(&buf, tb[2]);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_aggr";
	const char *method = "port_aggr_get";

	ubus_call(path, method, &buf, &recive_port_aggr_channel_id_one);
}

void port_aggr_get_channel_id_one_d(void){
	int i;
	void * tb[3];
	char idx[10];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		for(i=1; i<=device_gt->idx_num; i++){
			tb[1] = blobmsg_open_table(&buf, NULL);
				sprintf(idx, "%4s%-d", "idx-", i);
				tb[2] = blobmsg_open_array(&buf, idx);
					blobmsg_add_string(&buf, NULL, "channel_id");
				blobmsg_close_array(&buf, tb[2]);
			blobmsg_close_table(&buf, tb[1]);
		}
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_aggr";
	const char *method = "port_aggr_get";

	ubus_call(path, method, &buf, &recive_port_aggr_channel_id_one_d);
}

void port_aggr_get_channel_member_one(const char *idx){
	void * tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
			tb[2] = blobmsg_open_array(&buf, idx);
				blobmsg_add_string(&buf, NULL, "channel_member");
			blobmsg_close_array(&buf, tb[2]);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_aggr";
	const char *method = "port_aggr_get";

	ubus_call(path, method, &buf, &recive_port_aggr_channel_member_one);
}

void port_aggr_get_channel_member_all(void){
	int i;
	void * tb[3];
	char idx[12];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		for(i=1; i<=device_gt->idx_num; i++){
			sprintf(idx, "%4s%-d", "idx-", i);
			tb[1] = blobmsg_open_table(&buf, NULL);
				tb[2] = blobmsg_open_array(&buf, idx);
					blobmsg_add_string(&buf, NULL, "channel_member");
				blobmsg_close_array(&buf, tb[2]);
			blobmsg_close_table(&buf, tb[1]);
		}
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_aggr";
	const char *method = "port_aggr_get";

	ubus_call(path, method, &buf, &recive_port_aggr_channel_member_all);
}

void port_aggr_get_channel_one(const char *idx){
	void * tb[3];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
			tb[2] = blobmsg_open_array(&buf, idx);
				blobmsg_add_string(&buf, NULL, "channel_id");
				blobmsg_add_string(&buf, NULL, "channel_member");
			blobmsg_close_array(&buf, tb[2]);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_aggr";
	const char *method = "port_aggr_get";

	ubus_call(path, method, &buf, &recive_port_aggr_channel_all);
}

void port_aggr_get_channel_all(void){
	int i;
	void * tb[3];
	char idx[12];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		for(i=1; i<=device_gt->idx_num; i++){
			sprintf(idx, "%4s%-d", "idx-", i);
			tb[1] = blobmsg_open_table(&buf, NULL);
				tb[2] = blobmsg_open_array(&buf, idx);
					blobmsg_add_string(&buf, NULL, "channel_id");
					blobmsg_add_string(&buf, NULL, "channel_member");
				blobmsg_close_array(&buf, tb[2]);
			blobmsg_close_table(&buf, tb[1]);
		}
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_aggr";
	const char *method = "port_aggr_get";
	ubus_call(path, method, &buf, &recive_port_aggr_channel_all);
}

void port_aggr_set_mode_select(const char *mode_select){
	void *tb[2];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
			blobmsg_add_string(&buf, "mode_select", mode_select);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_aggr";
	const char *method = "port_aggr_set";

	ubus_call(path, method, &buf, &recive_port_aggr_set_mode_select);
}

void port_aggr_set_load_balance(const char *load_balance){
	void *tb[2];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
			blobmsg_add_string(&buf, "load_balance", load_balance);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_aggr";
	const char *method = "port_aggr_set";

	ubus_call(path, method, &buf, &recive_port_aggr_set_load_balance);
}

void port_aggr_set_channel_id_one(const char *idx, const char *channel_id){
	void *tb[4];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
			tb[2] = blobmsg_open_array(&buf, idx);
				tb[3] = blobmsg_open_table(&buf, NULL);
					blobmsg_add_string(&buf, "channel_id", channel_id);
				blobmsg_close_table(&buf, tb[3]);
			blobmsg_close_array(&buf, tb[2]);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_aggr";
	const char *method = "port_aggr_set";

	ubus_call(path, method, &buf, &recive_port_aggr_set_channel_id);
}

void port_aggr_set_channel_id_one_d(void){
	void *tb[4];
	char idx[12];
	blobmsg_buf_init(&buf);
	sprintf(idx, "%4s%-d", "idx-", device_gt->idx_num + 1);
	strcpy(device_gt->current_idx, idx);
	tb[0] = blobmsg_open_array(&buf, "set_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
			tb[2] = blobmsg_open_array(&buf, device_gt->current_idx);
				tb[3] = blobmsg_open_table(&buf, NULL);
					blobmsg_add_string(&buf, "channel_id", device_gt->current_view_name);
				blobmsg_close_table(&buf, tb[3]);
			blobmsg_close_array(&buf, tb[2]);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_aggr";
	const char *method = "port_aggr_set";

	ubus_call(path, method, &buf, &recive_port_aggr_set_channel_id_d);
}

void port_aggr_set_channel_member_one(const char *idx, const char *channel_member){
	void *tb[4];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
			tb[2] = blobmsg_open_array(&buf, idx);
				tb[3] = blobmsg_open_table(&buf, NULL);
					blobmsg_add_string(&buf, "channel_member", channel_member);
				blobmsg_close_table(&buf, tb[3]);
			blobmsg_close_array(&buf, tb[2]);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_aggr";
	const char *method = "port_aggr_set";

	ubus_call(path, method, &buf, &recive_port_aggr_set_channel_member);
}

void port_aggr_set_channel_one(const char *idx, const char *channel_id, const char *channel_member){
	void *tb[4];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
		tb[2] = blobmsg_open_array(&buf, idx);
			tb[3] = blobmsg_open_table(&buf, NULL);
				blobmsg_add_string(&buf, "channel_id", channel_id);
			blobmsg_close_table(&buf, tb[3]);
			tb[3] = blobmsg_open_table(&buf, NULL);
				blobmsg_add_string(&buf, "channel_member", channel_member);
			blobmsg_close_table(&buf, tb[3]);
		blobmsg_close_array(&buf, tb[2]);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "port_aggr";
	const char *method = "port_aggr_set";

	ubus_call(path, method, &buf, &recive_port_aggr_set_channel);
}

void port_aggr_clear_one(const char *idx){
	void *tb;
	blobmsg_buf_init(&buf);
	tb = blobmsg_open_array(&buf, "clear_args");
		blobmsg_add_string(&buf, NULL, idx);
	blobmsg_close_array(&buf, tb);

	const char *path = "port_aggr";
	const char *method = "port_aggr_clear";

	ubus_call(path, method, &buf, &recive_port_aggr_clear_one);
}
