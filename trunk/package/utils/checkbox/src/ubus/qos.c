#include "ubus_call.h"
#include "device_info.h"
#include "qos.h"
static void recive_qos_set_P8021_queue(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
			_PRINT_INFO(" %% 802.1P Priority-map set successfully.\n")
		else
			_PRINT_INFO(" %% 802.1P Priority-map set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}

static void recive_qos_set_dscp_queue(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
		if(!strcmp(blobmsg_get_string(lvl[1]), "0")){
			_PRINT_INFO(" %% DSCP Priority-map set successfully.\n")
		}else{
			_PRINT_INFO(" %% DSCP Priority-map set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
		}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	fflush(stdout);
}


static void recive_qos_set_mode(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% Qos mode set successfully.\n")
        else
            _PRINT_INFO(" %% Qos mode set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}
static void recive_qos_set_schedule_mode(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% Qos schedule mode set successfully.\n")
        else
            _PRINT_INFO(" %% Qos schedule mode set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}
static void recive_qos_set_default_priority(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% Default priority set successfully.\n")
        else
            _PRINT_INFO(" %% Default priority set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}
static void recive_qos_set_map_rule(struct ubus_request *req, int type, struct blob_attr *msg){
    int rem[6];
    struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
    blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
    if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_STRING){
        if(!strcmp(blobmsg_get_string(lvl[1]), "0"))
            _PRINT_INFO(" %% Qos map rule set successfully.\n")
        else
            _PRINT_INFO(" %% Qos map rule set failed: [%s]!\n", blobmsg_get_string(lvl[1]))
    }else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
    fflush(stdout);
}


static void recive_qos_get_dscp_queue(struct ubus_request *req, int type, struct blob_attr *msg){
	int rem[6];
	int i=0;
	HIGHT_LIGHT();
	for(i=0; i<7; i++)
		_PRINT_INFO("%6s%8s%4s", "DSCP:", "Priority", "")
	_PRINT_INFO("%6s%8s\n", "DSCP:", "Priority")
	i=0;
	UN_HIGHT_LIGHT();
	print_row(140);
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_ARRAY){
				printf("%5s:", blobmsg_name(lvl[3])+5);
				blobmsg_for_each_attr(lvl[4],lvl[3], rem[4]){
				if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
					blobmsg_for_each_attr(lvl[5],lvl[4], rem[5]){
					if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
						printf("%8s%4s", blobmsg_get_string(lvl[5]), "");
					}else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
				}else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
            if(7 == i%8)
                printf("\n");
            i++;
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	print_row(140);
}
static void recive_qos_get_p8021_queue(struct ubus_request *req, int type, struct blob_attr *msg){
	HIGHT_LIGHT();
	_PRINT_INFO("%16s%16s\n", "802.1P:", "Priority")
	UN_HIGHT_LIGHT();
	print_row(32);
	int rem[6];
	struct blob_attr *lvl[6] = {msg, NULL, NULL, NULL, NULL, NULL};
	blobmsg_for_each_attr(lvl[1],lvl[0], rem[1]){
	if (blobmsg_type(lvl[1]) == BLOBMSG_TYPE_ARRAY){
		blobmsg_for_each_attr(lvl[2],lvl[1], rem[2]){
		if (blobmsg_type(lvl[2]) == BLOBMSG_TYPE_TABLE){
			blobmsg_for_each_attr(lvl[3],lvl[2], rem[3]){
			if (blobmsg_type(lvl[3]) == BLOBMSG_TYPE_ARRAY){
				printf("%15s:", blobmsg_name(lvl[3])+6);
				blobmsg_for_each_attr(lvl[4],lvl[3], rem[4]){
				if (blobmsg_type(lvl[4]) == BLOBMSG_TYPE_TABLE){
					blobmsg_for_each_attr(lvl[5],lvl[4], rem[5]){
					if (blobmsg_type(lvl[5]) == BLOBMSG_TYPE_STRING){
						printf("%16s\n", blobmsg_get_string(lvl[5]));
					}else{_PRINT_ERROR(" %% Terminal work error v5: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[5])])}}
				}else{_PRINT_ERROR(" %% Terminal work error v4: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[4])])}}
			}else{_PRINT_ERROR(" %% Terminal work error v3: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[3])])}}
		}else{_PRINT_ERROR(" %% Terminal work error v2: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[2])])}}
	}else{_PRINT_ERROR(" %% Terminal work error v1: [%s]!\n", blobmsg_type_info[blobmsg_type(lvl[1])])}}
	print_row(32);
}

static void
recive_port_config_get_qos_mode(struct ubus_request *req, int type, struct blob_attr *msg){
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

void qos_get_mode(const char * port){
    void *tb[3];
    blobmsg_buf_init(&buf);
    tb[0] = blobmsg_open_array(&buf, "get_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                blobmsg_add_string(&buf, NULL, "port_priority_assignment");
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    const char *path = "qos_config";
    const char *method = "qos_config_get";

    ubus_call(path, method, &buf, &recive_port_config_get_qos_mode);
}

void qos_set_mode(const char * port, const char * mode){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "qos_config";
    const char *method = "qos_config_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "port_priority_assignment", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_qos_set_mode);
}

void qos_set_schedul_mode(const char * port, const char * mode){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "qos_config";
    const char *method = "qos_config_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "port_schedule_mode", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_qos_set_schedule_mode);
}

void qos_set_default_priority(const char * port, const char * mode){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "qos_config";
    const char *method = "qos_config_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "port_default_priority_level", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_qos_set_default_priority);
}
void qos_set_map_rule(const char * port, const char * mode){
    void *tb[3];
    blobmsg_buf_init(&buf);

    const char *path = "qos_config";
    const char *method = "qos_config_set";

    tb[0] = blobmsg_open_array(&buf, "set_args");
        tb[1] = blobmsg_open_table(&buf, NULL);
            tb[2] = blobmsg_open_array(&buf, port);
                tb[3] = blobmsg_open_table(&buf, NULL);
                    blobmsg_add_string(&buf, "port_priority_map_rule", mode);
                blobmsg_close_table(&buf, tb[3]);
            blobmsg_close_array(&buf, tb[2]);
        blobmsg_close_table(&buf, tb[1]);
    blobmsg_close_array(&buf, tb[0]);

    ubus_call(path, method, &buf, &recive_qos_set_map_rule);
}

void qos_get_dscp_queue(void){
	int i, j;
	void *tb[3];
	char dscp_id[8];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		for(i=0; i<8; i++){
			for(j=0; j<8; j++){
				sprintf(dscp_id, "%5s%-d", "dscp-", i+j*8);
				tb[1] = blobmsg_open_table(&buf, NULL);
					tb[2] = blobmsg_open_array(&buf, dscp_id);
					    blobmsg_add_string(&buf, NULL, "dscp_priority_queue_id");
					blobmsg_close_array(&buf, tb[2]);
				blobmsg_close_table(&buf, tb[1]);
			}
		}
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "qos_config";
	const char *method = "qos_config_get";

	ubus_call(path, method, &buf, &recive_qos_get_dscp_queue);
}

void qos_get_P8021_queue(void){
	int i;
	void *tb[3];
	char P8021_id[8];
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "get_args");
		for(i=0; i<8; i++){
			sprintf(P8021_id, "%6s%-d", "8021p-", i);
			tb[1] = blobmsg_open_table(&buf, NULL);
				tb[2] = blobmsg_open_array(&buf, P8021_id);
				blobmsg_add_string(&buf, NULL, "8021p_priority_queue_id");
				blobmsg_close_array(&buf, tb[2]);
			blobmsg_close_table(&buf, tb[1]);
		}
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "qos_config";
	const char *method = "qos_config_get";

	ubus_call(path, method, &buf, &recive_qos_get_p8021_queue);
}

void qos_set_P8021_queue(const char *p8021_id, const char *p8021_priority){
	void *tb[4];
	char P8021_id[8];
	sprintf(P8021_id, "%6s%s", "8021p-", p8021_id);
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
			tb[2] = blobmsg_open_array(&buf, P8021_id);
			tb[3] = blobmsg_open_table(&buf, NULL);
			blobmsg_add_string(&buf, "8021p_priority_queue_id", p8021_priority);
			blobmsg_close_table(&buf, tb[3]);
			blobmsg_close_array(&buf, tb[2]);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "qos_config";
	const char *method = "qos_config_set";

	ubus_call(path, method, &buf, &recive_qos_set_P8021_queue);
}

void qos_set_dscp_queue(const char *dcsp_id, const char *dscp_priority){
	void *tb[4];
	char Dcsp_id[8];
	sprintf(Dcsp_id, "%5s%s", "dscp-", dcsp_id);
	blobmsg_buf_init(&buf);
	tb[0] = blobmsg_open_array(&buf, "set_args");
		tb[1] = blobmsg_open_table(&buf, NULL);
			tb[2] = blobmsg_open_array(&buf, Dcsp_id);
			tb[3] = blobmsg_open_table(&buf, NULL);
			blobmsg_add_string(&buf, "dscp_priority_queue_id", dscp_priority);
			blobmsg_close_table(&buf, tb[3]);
			blobmsg_close_array(&buf, tb[2]);
		blobmsg_close_table(&buf, tb[1]);
	blobmsg_close_array(&buf, tb[0]);

	const char *path = "qos_config";
	const char *method = "qos_config_set";

	ubus_call(path, method, &buf, &recive_qos_set_dscp_queue);
}
