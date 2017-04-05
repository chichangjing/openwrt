#include "ubus_call.h"
#include "device_info.h"
#include "port_config.h"
#if 0
static void port_config_recive_config_all_port_row(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct show_tbl *tbl;
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	HIGHT_LIGHT();
	_PRINT_INFO("\n%20s%20s%20s", "Interface:", "Config Type:", "Value:")
	UN_HIGHT_LIGHT();
	print_row(60);
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
				_PRINT_INFO("\n%11s[%7s]", "",port_name_to_show(blobmsg_name(lvl[3])))
				bool mark = 1;
				blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
				if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
					blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
					if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
						tbl = str_parse_show(blobmsg_name(lvl[5]));
						_PRINT_INFO("%20s%20s", portconfig_to_show(blobmsg_name(lvl[5])), tbl->tbl[atoi(blobmsg_get_string(lvl[5]))])
						_PRINT_INFO(mark ? "\t ":"\n")
						mark = mark ? 0:1;
					}else{_PRINT_ERROR("\nLV5 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[5])])}}
				}else{_PRINT_ERROR("\nLV4 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[4])])}}
			}else{_PRINT_ERROR("\nLV3 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR("\nLV2 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR("\nLV1 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
	print_row(60);
}

static void port_config_get_config_message_all_port(struct blob_buf *buf, const char *config){
	int i;
	void *tb[6];
	tb[0] = blobmsg_open_array(buf, "get_args");
		for(i=0; i<_PORT_MAX; i++){
			tb[1] = blobmsg_open_table(buf, NULL);
				tb[2] = blobmsg_open_array(buf, port_name[0][i]);
					blobmsg_add_string(buf, NULL, config);
				blobmsg_close_array(buf, tb[2]);
			blobmsg_close_table(buf, tb[1]);
		}
	blobmsg_close_array(buf, tb[0]);
}

static void port_config_get_config_all_port(const char *config){
	blobmsg_buf_init(&buf);
	port_config_get_config_message_all_port(&buf, config);

	const char *path = "port_config";
	const char *method = "port_config_get";

	ubus_call(path, method, &buf, &port_config_recive_config_all_port_row);
}

void port_config_get_type_all_port(void){
	const char *config = "type";
	port_config_get_config_all_port(config);
}

void port_config_get_admin_mode_all_port(void){
	const char *config = "admin_mode";
	port_config_get_config_all_port(config);
}

void port_config_get_operational_status_all_port(void){
	const char *config = "operational_status";
	port_config_get_config_all_port(config);
}

void port_config_get_physical_mode_all_port(void){
	const char *config = "physical_mode";
	port_config_get_config_all_port(config);
}

void port_config_get_physical_status_all_port(void){
	const char *config = "physical_status";
	port_config_get_config_all_port(config);
}

void port_config_get_flow_control_mode_all_port(void){
	const char *config = "flow_control_mode";
	port_config_get_config_all_port(config);
}

void port_config_get_dot1d_state_all_port(void){
	const char *config = "dot1d_state";
	port_config_get_config_all_port(config);
}

static void port_config_recive_config_one_port_all_row(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	int i=0;
	char *(*tbl)[];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
				HIGHT_LIGHT();
				_PRINT_INFO("\n%22s%20s[%7s]", "Port Configure:", "",port_name_to_show(blobmsg_name(lvl[3])))
				UN_HIGHT_LIGHT();
				print_row(51);
				blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
				if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
					blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
					if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
						tbl = info_tbl[i].tbl;
						_PRINT_INFO("\n%21s:%29s", port_config_name[1][i], (*tbl)[atoi(blobmsg_get_string(lvl[5]))])
						i++;
					}else{_PRINT_ERROR("\nLV5 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[5])])}}
				}else{_PRINT_ERROR("\nLV4 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[4])])}}
			}else{_PRINT_ERROR("\nLV3 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR("\nLV2 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR("\nLV1 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	print_row(51);
	fflush(stdout);
}

static void port_config_get_config_message_one_port_all(struct blob_buf *buf, const char *port){
	int j;
	void *tb[6];
	tb[0] = blobmsg_open_array(buf, "get_args");
		tb[1] = blobmsg_open_table(buf, NULL);
			tb[2] = blobmsg_open_array(buf, port);
				for (j = 0; j < _PORT_CFG_MAX; j++){
					blobmsg_add_string(buf, NULL, port_config_name[0][j]);
				}
			blobmsg_close_array(buf, tb[2]);
		blobmsg_close_table(buf, tb[1]);
	blobmsg_close_array(buf, tb[0]);
}

void port_config_get_config_one_port_all(const char *port){
	blobmsg_buf_init(&buf);
	port_config_get_config_message_one_port_all(&buf, port);

	const char *path = "port_config";
	const char *method = "port_config_get";

	ubus_call(path, method, &buf, &port_config_recive_config_one_port_all_row);
}

static void
port_config_get_config_all_all_recive_tbl(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	int i=0, j=0;
	char *(*tbl)[];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	for (i=0; i<=_PORT_CFG_MAX;i++){
		if(i==0){
			HIGHT_LIGHT();
			_PRINT_INFO("\n%-12s", "")
		}else{
			UN_HIGHT_LIGHT();
			_PRINT_INFO("\n%11s:", port_config_name[1][i-1])
		}
		blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
		if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
			blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
			if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
				blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
				j = 0;
				if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
					if (!i)
						_PRINT_INFO("%11s",port_name_to_show(blobmsg_name(lvl[3])))
					blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
					if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
						blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
						j++;
						if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
							if(i == j){
								tbl = info_tbl[i-1].tbl;
								_PRINT_INFO("%11s", (*tbl)[atoi(blobmsg_get_string(lvl[5]))])}
						}else{_PRINT_ERROR("\nLV5 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[5])])}}
					}else{_PRINT_ERROR("\nLV4 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[4])])}}
				}else{_PRINT_ERROR("\nLV3 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[3])])}}
			}else{_PRINT_ERROR("\nLV2 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[2])])}}
			if (!i){
				UN_HIGHT_LIGHT();
				print_row(122);
			}
		}else{_PRINT_ERROR("\nLV1 Message Type Error:%s", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	}
	print_row(122);
	fflush(stdout);
}

static void
port_config_get_config_all_all_message(struct blob_buf *buf){
	int i, j;
	void *tb[6];
	tb[0] = blobmsg_open_array(buf, "get_args");
		for(i=0; i<_PORT_MAX; i++){
			tb[1] = blobmsg_open_table(buf, NULL);
				tb[2] = blobmsg_open_array(buf, port_name[0][i]);
					for (j = 0; j < _PORT_CFG_MAX; j++)
						blobmsg_add_string(buf, NULL, port_config_name[0][j]);
				blobmsg_close_array(buf, tb[2]);
			blobmsg_close_table(buf, tb[1]);
		}
	blobmsg_close_array(buf, tb[0]);
}

void port_config_get_config_all_all(void){
	blobmsg_buf_init(&buf);
	port_config_get_config_all_all_message(&buf);

	const char *path = "port_config";
	const char *method = "port_config_get";

	ubus_call(path, method, &buf, &port_config_get_config_all_all_recive_tbl);
}
#endif

static void recive_port_config_one(struct ubus_request *req, int type, struct blob_attr *msg){
    int i, rem[6];
    char * port_config[]={
            "admin mode:",
            "port type:",
            "physical mode:",
            "physical status:",
    };
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3], lvl[2], rem[3]){
            if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_ARRAY){
                printf("%50s\n", blobmsg_name(lvl[3]));
                print_row(50);
                i=0;
                blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
                if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
                    blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
                    if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
                        switch(i){
                            case 0:
                                printf("%24s%26s\n", port_config[i], port_config_admin_mode_tbl[0][atoi(blobmsg_get_string(lvl[5]))]);
                                break;
                            case 1:
                                printf("%24s%26s\n", port_config[i], port_type_tbl[0][atoi(blobmsg_get_string(lvl[5]))]);
                                break;
                            case 2:
                                printf("%24s%26s\n", port_config[i], port_config_physical_mode_tbl[0][atoi(blobmsg_get_string(lvl[5]))]);
                                break;
                            case 3:
                                printf("%24s%26s\n", port_config[i], port_config_physical_status_tbl[0][atoi(blobmsg_get_string(lvl[5]))]);
                                break;
                            }
                        i++;
                    }else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
                }else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    print_row(50);
    fflush(stdout);
}


void port_config_get_port_config_one(const char * port){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "admin_mode");
                blobmsg_add_string(&buf, NULL, "type");
                blobmsg_add_string(&buf, NULL, "physical_mode");
                blobmsg_add_string(&buf, NULL, "physical_status");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "port_config";
    const char *method = "port_config_get";

    ubus_call(path, method, &buf, &recive_port_config_one);
}

static void recive_port_config_get_admin_mode_one(struct ubus_request *req, int type, struct blob_attr *msg){
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

void port_config_get_admin_mode_one(const char * port){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "admin_mode");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "port_config";
    const char *method = "port_config_get";

    ubus_call(path, method, &buf, &recive_port_config_get_admin_mode_one);
}

static void port_config_recive_set(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(0 == strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% Set successfully.\n")
		else
			_PRINT_INFO(" %% Set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

static void port_config_set_config_message_one_one(struct blob_buf *buf, const char *port, const char *config, const char *mode){
	void *tb[6];
	tb[0] = blobmsg_open_array(buf, "set_args");
		tb[1] = blobmsg_open_table(buf, NULL);
			tb[2] = blobmsg_open_array(buf, port);
				tb[3] = blobmsg_open_table(buf, NULL);
					blobmsg_add_string(buf, config, mode);
				blobmsg_close_table(buf, tb[3]);
			blobmsg_close_array(buf, tb[2]);
		blobmsg_close_table(buf, tb[1]);
	blobmsg_close_array(buf, tb[0]);
}

static void port_config_set_config_one_one(const char *config, const char *port, const char *mode){
	blobmsg_buf_init(&buf);

	port_config_set_config_message_one_one(&buf, port, config, mode);
	const char *path = "port_config";
	const char *method = "port_config_set";

	ubus_call(path, method, &buf, &port_config_recive_set);
}

void port_config_set_admin_mode_one(const char *port, const char *mode){
	const char *config = "admin_mode";
	port_config_set_config_one_one(config, port, mode);
}

void port_config_set_physical_mode_one(const char *port, const char *mode){
	const char *config = "physical_mode";
	port_config_set_config_one_one(config, port, mode);
}

void port_config_set_flow_control_mode_one(const char *port, const char *mode){
	const char *config = "flow_control_mode";
	port_config_set_config_one_one(config, port, mode);
}

static void port_config_set_config_message_all(struct blob_buf *buf, const char *config, const char *mode){
	int i;
	void *tb[6];
	tb[0] = blobmsg_open_array(buf, "set_args");
		for(i=0; i<_PORT_MAX; i++){
			tb[1] = blobmsg_open_table(buf, NULL);
				tb[2] = blobmsg_open_array(buf, port_name[0][i]);
					tb[3] = blobmsg_open_table(buf, NULL);
						blobmsg_add_string(buf, config, mode);
					blobmsg_close_table(buf, tb[3]);
				blobmsg_close_array(buf, tb[2]);
			blobmsg_close_table(buf, tb[1]);
		}
	blobmsg_close_array(buf, tb[0]);
}

static void port_config_set_config_all(const char *config, const char *mode){
	blobmsg_buf_init(&buf);
	port_config_set_config_message_all(&buf, config, mode);

	const char *path = "port_config";
	const char *method = "port_config_set";

	ubus_call(path, method, &buf, &port_config_recive_set);
}

void port_config_set_admin_mode_all(const char *mode){
	const char *config = "admin_mode";
	port_config_set_config_all(config, mode);
}

void port_config_set_physical_mode_all(const char *mode){
	const char *config = "physical_mode";
	port_config_set_config_all(config, mode);
}

void port_config_set_flow_control_mode_all(const char *mode){
	const char *config = "flow_control_mode";
	port_config_set_config_all(config, mode);
}

static void recive_if_trap_threshold_tx_status_d(struct ubus_request *req, int type, struct blob_attr *msg){
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

static void recive_if_trap_threshold_tx_status(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(0 == strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% Set successfully.\n")
        else
            _PRINT_INFO(" %% Set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}

void set_if_trap_threshold_tx_status(const char * port, const char * mode){
    void *tb[6];
    blobmsg_buf_init(&buf);

    const char *path = "trap";
    const char *method = "trap_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "port_tx_shreshold_enable", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_if_trap_threshold_tx_status);
}

void set_if_trap_threshold_rx_status(const char * port, const char * mode){
    void *tb[6];
    blobmsg_buf_init(&buf);

    const char *path = "trap";
    const char *method = "trap_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "port_rx_shreshold_enable", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_if_trap_threshold_tx_status);
}

void get_if_trap_threshold_tx_status_d(const char * port){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "port_tx_shreshold_enable");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "trap";
    const char *method = "trap_get";

    ubus_call(path, method, &buf, &recive_if_trap_threshold_tx_status_d);
}

void get_if_trap_threshold_rx_status_d(const char * port){
    void *tb[6];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "port_rx_shreshold_enable");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "trap";
    const char *method = "trap_get";

    ubus_call(path, method, &buf, &recive_if_trap_threshold_tx_status_d);
}

void set_if_trap_threshold_tx_value(const char * port, const char * value){
    void *tb[6];
    blobmsg_buf_init(&buf);

    const char *path = "trap";
    const char *method = "trap_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "port_tx_shreshold_perc", value);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_if_trap_threshold_tx_status);
}

void set_if_trap_threshold_rx_value(const char * port, const char * value){
    void *tb[6];
    blobmsg_buf_init(&buf);

    const char *path = "trap";
    const char *method = "trap_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "port_rx_shreshold_perc", value);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_if_trap_threshold_tx_status);
}

static void recive_get_if_trap_all(struct ubus_request *req, int type, struct blob_attr *msg){
    int i;
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
        blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
        if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
            blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
            if (blobmsg_type(lvl[3]) ==  BLOBMSG_TYPE_ARRAY){
                printf(" %-9s", blobmsg_name(lvl[3]));
                i=0;
                blobmsg_for_each_attr(lvl[4], lvl[3], rem[4]){
                if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
                    blobmsg_for_each_attr(lvl[5], lvl[4], rem[5]){
                    if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
                        switch(i){
                            case 0:
                            case 1:
                            case 3:
                                if(!strcmp(blobmsg_get_string(lvl[5]), "1"))
                                    printf("  %20s", "enable");
                                else if(!strcmp(blobmsg_get_string(lvl[5]), "2"))
                                    printf("  %20s", "disable");
                                else
                                    printf("  %20s", "unknow");
                                break;
                            case 2:
                            case 4:
                                printf("  %21s", blobmsg_get_string(lvl[5]));
                                break;
                        }
                        i++;
                    }else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
                }else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
                printf("\n");
            }else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
        }else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
}

void get_if_trap_all(void){
    int port;
    void *tb[6];
    blobmsg_buf_init(&buf);

    const char *path = "trap";
    const char *method = "trap_get";

    tb[0] = blobmsg_open_array(&buf, "get_args");
    for(port=PORT_1;port<_PORT_MAX;port++){
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port_name[2][port]);
                blobmsg_add_string(&buf, NULL, "port_trap");
                blobmsg_add_string(&buf, NULL, "port_tx_shreshold_enable");
                blobmsg_add_string(&buf, NULL, "port_tx_shreshold_perc");
                blobmsg_add_string(&buf, NULL, "port_rx_shreshold_enable");
                blobmsg_add_string(&buf, NULL, "port_rx_shreshold_perc");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    }
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_get_if_trap_all);
}

void set_if_trap_port_link_status(const char * port, const char * mode){
    void *tb[6];
    blobmsg_buf_init(&buf);

    const char *path = "trap";
    const char *method = "trap_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "port_trap", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_if_trap_threshold_tx_status);
}
